#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
#include "QImage"
#include "QFile"
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
    ui->led_off->setDisabled(true);
    ui->led_on->setEnabled(true);
    ui->creampushButton->setEnabled(true);
    connect(socket,SIGNAL(readyRead()),this,SLOT(recv()));
    this->show();
}


void Widget::recv()
{
    char buf[SIZE] = {0};
    if(socket->bytesAvailable() < SIZE)
        return;
    socket->read(buf, SIZE);
    if(buf[0]== '1'
            && buf[1] == '0'
            && buf[2] == '1'
            && buf[3] == '0'
            && buf[4] == '1')
    {
       disposeImage((uchar *)buf);
    }

    if(buf[0]== '1'
            && buf[1] == '0'
            && buf[2] == '1'
            && buf[3] == '0'
            && buf[4] == '2')
    {
        updateHumiTemp((uchar *)buf);
    }

}
void Widget::paintEvent(QPaintEvent *event)  //背景
{
    QPainter pt(this);
    img=QImage(":/Windows2.jpg");
    QImage drawing=img.scaled(this->width(),this->height());
    pt.drawImage(0,0,drawing,0,0,this->width(),this->height());
}

void Widget::showImage(const uchar * src_image, int size_image)
{
    qDebug() << size_image << endl;
    qDebug() << src_image[size_image - 2] << endl;
    if(src_image[size_image - 1] == 217)
    {
        QPixmap pix;
        pix.loadFromData(src_image, size_image, "jpg");
        ui->vedioLabel->setPixmap(pix);
    }
}

void Widget::disposeImage(const uchar * buf)
{
    int i = 5;
    char filesize[10] = {0};
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
    socket->close();
    this->close();
    emit dlgshow();
}

void Widget::updateHumiTemp(const uchar *buf)
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
    }
    i++;
    int j;
    for(j = 0; i < SIZE; i++, j++)
    {
        if(buf[i] == '#')
            break;
        temp[j] = buf[i];
    }
    ui->show_temp->setText(temp);
    ui->show_humi->setText(humi);
}

void Widget::on_led_on_clicked()
{
    QString str;
    str.sprintf("%d", LED_ON);
    socket->write(str.toUtf8());
    ui->led_on->setDisabled(true);
    ui->led_off->setEnabled(true);
}

void Widget::on_led_off_clicked()
{
    QString str;
    str.sprintf("%d", LED_OFF);
    socket->write(str.toUtf8());
    ui->led_off->setDisabled(true);
    ui->led_on->setEnabled(true);
}
