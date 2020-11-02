#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
#include "QImage"
#include "QPainter"


static int flag = 1;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::receiveshow()
{
    connect(socket,SIGNAL(readyRead()),this,SLOT(recv()));
    QPixmap pix;
    this->show();
}

void Widget::recv()
{
    QByteArray recv_msg = socket->read(SIZE);
    char * buf = recv_msg.data();
    if(recv_msg.at(0) == '1'
            && recv_msg.at(1) == '0'
            && recv_msg.at(2) == '1'
            && recv_msg.at(3) == '0'
            && recv_msg.at(4) == '1')
    {
        size = getFileSize(buf, SIZE);
        qDebug() << size << endl;
    }
    if(recv_msg.at(0) == '1'
            && recv_msg.at(1) == '0'
            && recv_msg.at(2) == '1'
            && recv_msg.at(3) == '0'
            && recv_msg.at(4) == '2')
    {
        disposeImage(recv_msg);
    }
}

void Widget::paintEvent(QPaintEvent *event)  //背景
{
    QPainter pt(this);
    img=QImage(":/Windows2.jpg");
    QImage drawing=img.scaled(this->width(),this->height());
    pt.drawImage(0,0,drawing,0,0,this->width(),this->height());
}

void Widget::showImage()
{
    pix.loadFromData(video, "jpg");
    ui->vedioLabel->setPixmap(pix);
    ui->vedioLabel->setScaledContents(true);
    video.clear();
}

void Widget::disposeImage(QByteArray src_msg)
{
    char *buf = src_msg.data();

    if(size < SIZE - 5)
        video.append(buf+5, strlen(buf) - 5);
    else
        video.append(buf+5, SIZE - 5);
    size = size - SIZE + 5;

    if(size <= 0)
    {
        qDebug() << video << endl;
        showImage();
    }
}

void Widget::on_creampushButton_clicked()
{
    if(flag)
    {
        QString str;
        str.sprintf("%d",VIDEO_ON);
        socket->write(str.toUtf8());
        flag = 0;
        ui->creampushButton->setText("关闭监控");
    }
    else
    {
        QString str;
        str.sprintf("%d",VIDEO_OFF);
        socket->write(str.toUtf8());
        flag = 1;
        ui->creampushButton->setText("开启监控");
    }
}

void Widget::on_exit_clicked()
{
    this->close();
    emit dlgshow();
}

int Widget::getFileSize(char * buf, int len)
{
    int i = 0;
    char file_size[10] = {0};
    for(i = 0; i < len - 5; i++)
    {
        if(buf[i + 5] == '\0')
            break;
        file_size[i] = buf[i+5];
    }
    return atoi(file_size);
}
