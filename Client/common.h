#ifndef COMMON_H
#define COMMON_H

#include <QWidget>

#define SIZE 512
#define SIP "169.254.29.189"
#define PORT 10011

extern QTcpSocket * socket;

enum COMMAND
{
    VIDEO_ON = 1,
    VIDEO_OFF = 2
};

#endif // COMMON_H



