#ifndef COMMON_H
#define COMMON_H

#include <QWidget>

#define SIZE (32*1024)
#define SIP "192.168.1.120"
//#define SIP "192.168.0.214"

#define PORT 10011


extern QTcpSocket * socket;

enum COMMAND
{
    VIDEO_ON = 1,
    VIDEO_OFF,
    LED_ON,
    LED_OFF,
};

#endif // COMMON_H



