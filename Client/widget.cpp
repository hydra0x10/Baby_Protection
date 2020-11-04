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
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnectedSlot()));
    QPixmap pix;
    this->show();
}
char buf[SIZE] = {0};

void Widget::recv()
{
    memset(buf, 0, SIZE);
    socket->read(buf, SIZE);
    if(buf[0]== '1'
            && buf[1] == '0'
            && buf[2] == '1'
            && buf[3] == '0'
            && buf[4] == '1')
    {
       disposeImage(buf);
    }
    if(buf[0]== '1'
            && buf[1] == '0'
            && buf[2] == '1'
            && buf[3] == '0'
            && buf[4] == '2')
    {
        updateHumiTemp(buf);
    }

}

void Widget::paintEvent(QPaintEvent *event)  //背景
{
    QPainter pt(this);
    img=QImage(":/Windows2.jpg");
    QImage drawing=img.scaled(this->width(),this->height());
    pt.drawImage(0,0,drawing,0,0,this->width(),this->height());
}

void Widget::showImage(const char * src_image, int size_image)
{
    pix.loadFromData((uchar *)src_image, size_image, "jpeg");
    ui->vedioLabel->setPixmap(pix);
}

void Widget::disposeImage(const char * buf)
{
    int i = 5; //0-4 10101 文件大小 # -----
    char filesize[10];
    while(1)
    {
        if(buf[i] == '#')
        {
            i++;
            break;
        }
        filesize[i-5] = buf[i];
        i++;
    }
    size = atoi(filesize);
    qDebug() << video.append(buf+i, size) << endl;
    video.clear();
    showImage(buf+i, size);
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

void Widget::updateHumiTemp(const char *buf)
{
    //10102湿度#温度
    int i = 0;
    char temp[10] = {0};
    char humi[10] = {0};
    if(buf[5] == '\0')
        return;
    for(i = 5; i < SIZE; i++)
    {
        if(buf[i] == '#')
            break;
        humi[i-5] = buf[i];
        qDebug() << buf[i] << endl;
    }
    i++;
    int j;
    for(j = 0; i < SIZE; i++, j++)
    {
        if(buf[i] == '\0')
            break;
        temp[j] = buf[i];
    }
    ui->show_temp->setText(temp);
    ui->show_humi->setText(humi);
}
