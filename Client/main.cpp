#include "widget.h"
#include "dialog.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog d;
    Widget w;
    d.show();
    d.setFixedSize(523,300);
    w.setFixedSize(600,400);
    d.setWindowTitle("Baby");
    d.setWindowIcon(QIcon(":/lovely_duck.ico"));
    w.setWindowTitle("Baby");
    w.setWindowIcon(QIcon(":/lovely_duck.ico"));

    QObject::connect(&d,SIGNAL(mainshow()),&w,SLOT(receiveshow()));
    QObject::connect(&w,SIGNAL(dlgshow()),&d,SLOT(receivelogin()));
    return a.exec();
}
