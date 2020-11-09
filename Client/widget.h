#ifndef WIDGET_H
#define WIDGET_H
#include <QtNetwork>
#include <QTcpSocket>
#include <QWidget>
#include <QPaintEvent>
#include <QPushButton>
#include <QImage>
#include <QTime>
#include <QString>
#include <QPainter>
#include <QObject>
#include <QIcon>
#include <QPixmap>
#include "common.h"
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void paintEvent(QPaintEvent *event);
    void showImage(const uchar *src_image, int size_image);
    void disposeImage(const uchar *buf);
    void updateHumiTemp(const uchar *buf);

public slots:
    void recv();

private slots:

    void receiveshow();

    void on_creampushButton_clicked();

    void on_exit_clicked();

    void on_led_on_clicked();

    void on_led_off_clicked();

private:
    Ui::Widget *ui;
    QImage img;
    int size = 0;
    QByteArray video;




signals:
    void dlgshow();
};

#endif // WIDGET_H
