#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <cinttypes>

#ifndef CLIENT_FUNC_H
#define CLIENT_FUNC_H

typedef boost::asio::ip::tcp tcp;

class TalkToServer
{
   private:
    int port;
    std::string ip_address;
    tcp::socket sock;
    boost::system::error_code ec;

   public:
    TalkToServer(const std::string&,const int&,boost::asio::io_service&);
    ~TalkToServer();
    void connect();
    void sendFile(const std::string&);
};

#endif // CLIENT_FUNC_H