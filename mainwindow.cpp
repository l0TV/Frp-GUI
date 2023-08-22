#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setWindowIcon(QIcon("./icon.png"));
    setFixedSize(300,300);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::pushButton1_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::pushButton2_clicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::pushButton3_clicked);
    pushButton2_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::pushButton1_clicked(int istemp=0)
{
    QString path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);

    QString addr = ui->lineEdit_4->text();
    QString port = ui->lineEdit_2->text();
    QString token = ui->lineEdit_3->text();
    QString type = ui->comboBox->currentText();
    QString ip = ui->lineEdit_5->text();
    QString local_port = ui->lineEdit_6->text();
    QString remote_port = ui->lineEdit_7->text();

    QFile* file;
    if(!istemp) file = new QFile(path+"/frpc.ini");
    else file = new QFile(path+"/frpc_temp.ini");
    if(!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return;
    }
    QTextStream stream(file);
    stream << "[common]\n";
    stream << "server_addr="<<addr<<'\n';
    stream << "server_port="<<port<<'\n';
    if(token.length()!=0) stream << "token="<<token<<"\n\n";
    stream<<"[10TV]\n";
    stream << "type="<<type<<'\n';
    stream << "local_ip="<<ip<<'\n';
    stream << "local_port="<<local_port<<'\n';
    stream << "remote_port="<<remote_port;
    file->close();
}

void MainWindow::pushButton2_clicked()
{
    QDir dir;
    QString path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
    if(!dir.exists(path)){
        dir.mkdir(path);
        QFile file("./frpc.ini");
        file.copy(path+"/frpc.ini");
    }
    QFile file(path+"/frpc.ini");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream stream(&file);
    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        if(line[0]=='[' || line[0]=='#' || line.length()==0) continue;
        QStringList str = line.split('=');
        if(str[0].trimmed()=="server_addr") ui->lineEdit_4->setText(str[1].trimmed());
        if(str[0].trimmed()=="server_port") ui->lineEdit_2->setText(str[1].trimmed());
        if(str[0].trimmed()=="token") ui->lineEdit_3->setText(str[1].trimmed());
        if(str[0].trimmed()=="type" && str[1].trimmed()=="tcp") ui->comboBox->setCurrentIndex(0);
        if(str[0].trimmed()=="type" && str[1].trimmed()=="udp") ui->comboBox->setCurrentIndex(1);
        if(str[0].trimmed()=="local_ip") ui->lineEdit_5->setText(str[1].trimmed());
        if(str[0].trimmed()=="local_port") ui->lineEdit_6->setText(str[1].trimmed());
        if(str[0].trimmed()=="remote_port") ui->lineEdit_7->setText(str[1].trimmed());
    }
    file.close();
}

void MainWindow::pushButton3_clicked()
{
    ui->pushButton_3->setDisabled(true);
    pushButton1_clicked(1);
    QString path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
    p = new QProcess(this);
    connect(p, &QProcess::readyRead, this, &MainWindow::on_read);
    QStringList list;
    list<<"-c"<<path+"/frpc_temp.ini";
    p->start("./frpc.exe", list);
    ui->label_9->setText("连接中");
}

void MainWindow::on_read()
{
    QString status = p->readAllStandardOutput();
    qDebug()<<status;
    if(status.contains("success")){
        ui->label_9->setText("连接成功");
        ui->pushButton_3->setText("断开连接");
        disconnect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::pushButton3_clicked);
        connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::stop_connection);
        ui->pushButton_3->setEnabled(true);
    }
    if(status.contains("failed")) {ui->label_9->setText("连接失败"); ui->pushButton_3->setEnabled(true);};
}

void MainWindow::stop_connection()
{
    p->kill();
    disconnect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::stop_connection);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::pushButton3_clicked);
    ui->pushButton_3->setText("连接");
    ui->label_9->setText("未连接");
}
