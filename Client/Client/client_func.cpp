#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <cinttypes>
#include "client_func.h"
#include "filesyst_func.h"
#include <boost/regex.hpp>

typedef boost::asio::ip::tcp tcp;

boost::smatch res;
boost::regex regFileName("FileName: *(.+?)\r\n");
boost::regex regFileSize("FileSize: *([0-9]+?)\r\n");

TalkToServer::TalkToServer(const std::string& ip, const int& port_, boost::asio::io_service& io):ip_address(ip),sock(io),acc_sock(io),port(port_),acc(io,tcp::endpoint(tcp::v4(), (port_))){}
TalkToServer::~TalkToServer()
{
}

void TalkToServer::connect()
{
    tcp::endpoint ep(boost::asio::ip::address::from_string(ip_address), port);
    sock.connect(ep);
    if(ec)
    {
        std::cerr << ec.message();
        std::cin.get();
    }
}


void TalkToServer::sendFileList(const std::vector<std::string>& files,std::vector<std::string>& all_files)
{
    boost::asio::streambuf sb;
    std::ostream os(&sb);
    os<<"Update"<<"\n";
    os<<currentIP()<<"\n";
    for(size_t i=0;i<files.size();i++)
    {
        os<<files[i]<<"\n";
    }
    boost::asio::write(sock,sb);
    sock.shutdown(tcp::socket::shutdown_send);
    sock.close();

    acc.accept(acc_sock,ec);
    if(ec)
    {
        std::cerr << "accept failed\n";
    }
    boost::asio::read_until(acc_sock,sb,"\0",ec);
    if(ec)
    {
        std::cerr << "read_until failed" << ec << std::endl;
    }
    std::istream is(&sb);
    char bfr[512];
    all_files.clear();
    while(is)
    {
        is.getline(bfr,512);
        if(strcmp(bfr,""))
            all_files.push_back(bfr);
    }

    acc_sock.shutdown(tcp::socket::shutdown_both);
    acc_sock.close();
    acc.close();
}


void TalkToServer::sendFile(const std::string& fname)
{
    std::string filePath("Files/" + fname);
    std::cout<<filePath<<"\n";
    std::ifstream ifs(filePath.c_str(), std::ios::binary);
    if(!ifs){ std::cerr << "Can't read file\n"; std::cin.get();}
    std::uintmax_t fileSize = boost::filesystem::file_size(boost::filesystem::path(filePath));
    std::cout << "fileSize = " << fileSize << std::endl;
    std::size_t sentFileName = boost::asio::write(sock, boost::asio::buffer("FileName: " + boost::filesystem::path(filePath).filename().string() + "\r\n"));
    std::size_t sentFileSize = boost::asio::write(sock, boost::asio::buffer("FileSize: " + boost::lexical_cast<std::string>(fileSize) + "\r\n\r\n"));
    std::uintmax_t sentFileBody = 0;
    char fileBuf[10000];
    std::size_t fileBufSize = sizeof(fileBuf);
    while(ifs){
        ifs.read(fileBuf, fileBufSize);
        sentFileBody += boost::asio::write(sock, boost::asio::buffer(fileBuf, ifs.gcount()));
    }
    if(sentFileBody != fileSize){std::cerr << "write failed\n"; std::cin.get();}
    std::cout << "sentFileName = " << sentFileName << std::endl
    << "sentFileSize = " << sentFileSize << std::endl
    << "sentFileBody = " << sentFileBody << std::endl;
    std::cout << "OK" << std::endl;
    ifs.close();
    sock.shutdown(tcp::socket::shutdown_both);
    sock.close();
}

void TalkToServer::acceptFile()
{
    boost::asio::streambuf sBuf;;
    std::istream istr(&sBuf);
    std::size_t l = boost::asio::read_until(sock, sBuf, boost::regex("(?:\r\n){2,}"), ec);
    if(ec){std::cerr << "read_until failed\nl = " << l <<"\n"; std::cin.get();}
    std::string headers, tmp;
    while(std::getline(istr, tmp) && tmp != "\r"){
            headers += (tmp + '\n');
        }
    std::cout << "Headers:\n" << headers << std::endl;
    if(!boost::regex_search(headers, res, regFileSize)){
        std::cerr << "regFileSize not found\n";
        std::cin.get();
    }
    std::size_t fileSize = boost::lexical_cast<std::size_t>(res[1]);
    std::cout << "fileSize = " << fileSize << std::endl;
    if(!boost::regex_search(headers, res, regFileName)){
        std::cerr << "regFileName not found\n";
        std::cin.get();
    }
    std::string fileName = res[1];
    std::cout << "fileName = " << fileName << std::endl;
    std::ofstream ofs((fileName).c_str(), std::ios::binary);
    if(!ofs){std::cerr << "Can't create file\n"; std::cin.get();}
    std::size_t len = 0;
    std::cout << "sBuf.size() = " << sBuf.size() << std::endl;
    if(sBuf.size()){
        len += sBuf.size();
        ofs << &sBuf;
    }
    while(len != fileSize){
        len += boost::asio::read(sock, sBuf, boost::asio::transfer_at_least(1));
        ofs << &sBuf;
        std::cout << "len = " << len << std::endl;
    }
    std::cout << "OK" << std::endl;
    ofs.close();
    istr.rdbuf(NULL);
    sock.shutdown(tcp::socket::shutdown_both);
    sock.close();
    //acc.close();
}
