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
    //ui->vedioLabel->setScaledContents(true);


    QString str;
    str.sprintf("%d",VIDEO_RECV);
    socket->write(str.toUtf8());
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
    qDebug() << size << endl;
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
