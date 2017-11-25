#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client_func.h"
#include "filesyst_func.h"
#include <vector>
#include <string>
#include <QString>
#include <boost/asio.hpp>

std::vector<std::string> client_files;
std::vector<std::string> all_files;

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
    getFileList(client_files);
    std::vector<std::string> ipport;
    split(ipport,ui->lineEdit->text().toStdString());
    std::string ip=ipport[0];
    int port= atoi(ipport[1].c_str());
    TalkToServer talk(ip,port,service);
    talk.connect();
    talk.sendFileList(client_files,all_files);
    for(int i=0;i<all_files.size();i++)
        ui->textEdit->append(all_files[i].c_str());
}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{
    std::string ipa=currentIP();
    ui->textEdit->append(ipa.c_str());
}
