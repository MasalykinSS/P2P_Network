#include "server_funct.h"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <iterator>

typedef boost::asio::ip::tcp tcp;

Server::Server(const int& port_,boost::asio::io_service& io):port(port_),sock(io),acc(io,tcp::endpoint(tcp::v4(), port_)){}
Server::~Server(){}

void Server::listen()
{
    acc.accept(sock);
    std::cout << "Client has been accepted\n";
}

void Server::chooseCommand(std::map<std::string,std::string>& file_map)
{
    std::vector<std::string> commands;
    boost::asio::streambuf sb;
    boost::asio::read_until(sock,sb,"\n");
    std::istream is(&sb);
    char bfr[256];
    while(is)
    {
        is.getline(bfr,256);
        commands.push_back(bfr);
    }
    for(size_t i = 0;i<commands.size();i++)
        std::cout<<commands[i]<<"\n";
    sock.shutdown(tcp::socket::shutdown_both);
    sock.close();
    acc.close();

    if(commands[0]=="Update")
    {
        std::cout<<"Updating\n";
        if(file_map.empty())
        {
            for(size_t i=2;i<commands.size();i++)
            {
                if(commands[i]!="")
                {
                    file_map.insert(std::pair<std::string,std::string>(commands[i],commands[1]));
                }
            }
        }
        else
        {
            for(size_t i = 2; i < commands.size();i++)
            {
                if(commands[i]!="")
                {
                    auto iter=file_map.find(commands[i]);
                    if(iter==file_map.end())
                        file_map.insert(std::pair<std::string,std::string>(commands[i],commands[1]));
                    else
                    {
                      if(!(strstr(  ((*iter).second).c_str(),commands[1].c_str())))
                        (*iter).second = (*iter).second + ":" + commands[1];
                    }
                }
            }

        }

        for(auto it = file_map.begin();it!=file_map.end();it++)
        {
            std::cout<<(*it).first<<" : "<<(*it).second<<"\n";
        }

        tcp::endpoint endp(boost::asio::ip::address::from_string("127.0.0.1"  /*commands[1]*/),9090);
        sock.connect(endp);
        std::ostream os(&sb);


        for(auto mit = file_map.begin();mit!=file_map.end();mit++)
        {
            bool flag = true;
            for(size_t i = 2; i!=commands.size(); i++)
            {
                if((*mit).first==commands[i])
                {
                    flag=false;
                    break;
                }

            }
            if(flag)
                os<<(*mit).first<<"\n";
        }

        os<<"\0";
        std::cout<<sb.size()<<" ";
        size_t bts=boost::asio::write(sock,sb,ec);
        if(ec){std::cerr << "read_until failed\n;";}
        std::cout<<bts<<"\n";
        std::cout<<"sended\n";
        sock.shutdown(tcp::socket::shutdown_both);
        sock.close();
    }

}

/*void Server::updateFileList(std::map<std::string,std::string>& fmap)
{


}*/

