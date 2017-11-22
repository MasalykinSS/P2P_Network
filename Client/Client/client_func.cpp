#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <cinttypes>
#include "client_func.h"

typedef boost::asio::ip::tcp tcp;

TalkToServer::TalkToServer(const std::string& ip, const int& port_, boost::asio::io_service& io):ip_address(ip),sock(io),port(port_){}
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
    else std::cout<<"Connection established\n";
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

