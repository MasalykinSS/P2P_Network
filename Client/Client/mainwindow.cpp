#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client_func.h"
#include "filesyst_func.h"
#include <vector>
#include <string>
#include <QString>
#include <boost/asio.hpp>

std::vector<std::string> client_files;

boost::asio::io_service service;


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
    std::string server;

    try
        {
           std::vector<std::string> ipport;
           split(ipport,ui->lineEdit->text().toStdString());
           std::string ip=ipport[0];
           int port= atoi(ipport[1]);
           TalkToServer talk(ip,port,service);
           talk.connect();
           talk.sendFile("test.txt");
        }
        catch(std::exception& ex){
            std::cerr << "Exception: " << ex.what() << std::endl;
        }
}

void MainWindow::on_pushButton_2_clicked()
{
 getFileList(client_files);
 for(int i=0;i<client_files.size();i++)
     ui->textEdit->append(client_files[i].c_str());
}

void MainWindow::on_pushButton_3_clicked()
{
    std::string ipa=currentIP();
    ui->textEdit->append(ipa.c_str());
}
