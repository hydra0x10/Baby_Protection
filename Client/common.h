#ifndef COMMON_H
#define COMMON_H

#include <QWidget>

#define SIZE (32*1024)
#define SIP "192.168.0.120"
#define PORT 10011

extern QTcpSocket * socket;

enum COMMAND
{
    VIDEO_ON = 1,
    VIDEO_OFF = 2,
    VIDEO_RECV,
};

#endif // COMMON_H



