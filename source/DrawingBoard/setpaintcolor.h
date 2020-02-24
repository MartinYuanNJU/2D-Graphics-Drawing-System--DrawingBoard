#ifndef SETPAINTCOLOR_H
#define SETPAINTCOLOR_H

#include <QDialog>
#include "mycanvas.h"

namespace Ui {
class setPaintColor;
}

class setPaintColor : public QDialog
{
    Q_OBJECT

public:
    explicit setPaintColor(QWidget *parent = 0);
    ~setPaintColor();

    void loadInfo(myCanvas *canvas,Information &info);

private slots:
    void on_pushButton_clicked();

    //void on_pushButton_2_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::setPaintColor *ui;

    myCanvas *tempcanvas;
    Information information;

    QColor tcolor;
    QColor tfillcolor;
};

#endif // SETPAINTCOLOR_H
