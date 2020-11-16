#include "dialog.h"
#include "ui_dialog.h"
#include <QPushButton>
#include <QHBoxLayout>
#include "dialog.h"
#include "widget.h"
#include <QDebug>
#include <QtNetwork/QTcpSocket>
QTcpSocket * socket;
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::receivelogin()
{
    this->show();
}

void Dialog::paintEvent(QPaintEvent *event)  //背景
{
      QPainter pt(this);
      img=QImage(":/windows1.jpg");
      QImage drawing=img.scaled(this->width(),this->height());
      pt.drawImage(0,0,drawing,0,0,this->width(),this->height());
}

void Dialog::on_pushButton_clicked()
{
    socket = new QTcpSocket;
    socket->connectToHost(SIP, PORT);

    QFont font ( "STKaiti", 25, 75);
    ui->label_2->setFont(font);
    ui->label_2->setStyleSheet("color:red;");

    ui->label_2->clear();
    this->close();
    emit mainshow();
}

void Dialog::on_pushButton_2_clicked()
{
    this->close();
}
