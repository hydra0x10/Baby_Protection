#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "widget.h"
#include "common.h"
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void paintEvent(QPaintEvent *event);

private slots:
    void on_pushButton_clicked();
    void receivelogin();
    void on_pushButton_2_clicked();


signals:
    void mainshow();
private:
    Ui::Dialog *ui;
    QImage img;
};

#endif // DIALOG_H
