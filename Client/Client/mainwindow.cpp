#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client_func.h"
#include "filesyst_func.h"
#include <vector>
#include <string>
#include <QString>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>
#include <functional>

std::string serverIP;
int serverPort;
std::string currIP;

std::vector<std::string> client_files;
std::vector<std::string> all_files;

boost::asio::io_service service;


//TalkToServer tk("127.0.0.1",9090,service);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    currIP=ui->lineEdit_2->text().toStdString();
    getFileList(client_files);
    std::vector<std::string> ipport;
    split(ipport,ui->lineEdit->text().toStdString());
    std::string ip=ipport[0];
    int port= atoi(ipport[1].c_str());
    TalkToServer talk(ip,port,service);
        if(talk.connect())
        {
            talk.sendFileList(client_files,all_files,currIP);
            ui->textEdit->clear();
            for(int i=0;i<all_files.size();i++)
            {
                if(all_files[i]!=":NoNewFiles")
                ui->textEdit->append(all_files[i].c_str());
            }
            serverIP=ip;
            serverPort=port;
            ui->pushButton->hide();
            ui->lineEdit->hide();
            std::thread thr(listenLoop,std::ref(service));
            thr.detach();
        }
        else
            ui->label->setText("Error. Try again");
}

void MainWindow::on_pushButton_2_clicked()
{
    getFileList(client_files);
    TalkToServer talk(serverIP,serverPort,service);
    if(talk.connect())
    {
        talk.sendFileList(client_files,all_files,currIP);
        ui->textEdit->clear();
        for(int i=0;i<all_files.size();i++)
        ui->textEdit->append(all_files[i].c_str());
    }
    else
    {

    }
}

void MainWindow::on_pushButton_3_clicked()
{
    std::vector<std::string> ipport;
    split(ipport,ui->lineEdit->text().toStdString());
    boost::system::error_code ec;

    TalkToServer acceptor(serverIP,serverPort,service);
    boost::asio::streambuf sb;
    std::ostream os(&sb);
    os<<"Send"<<"\n";
    os<<ui->lineEdit_3->text().toStdString()<<"\n"; /*real file name*/
    os<<currIP;
    tcp::socket sock(service);
    tcp::endpoint ep(boost::asio::ip::address::from_string(serverIP), serverPort);
    sock.connect(ep,ec);
    if(ec)
    {
        std::cerr << "connection to server failed\n";
        return;
    }
    boost::asio::write(sock,sb,ec);
    if(ec)
    {
        std::cerr << "writting to server failed\n";
    }
    sock.shutdown(tcp::socket::shutdown_send);
    sock.close();
    std::cout<<"all ok, trying to accept file\n";
    acceptor.acceptFile();
}

void MainWindow::on_pushButton_4_clicked()
{
  //  tk.doLoop();
}
