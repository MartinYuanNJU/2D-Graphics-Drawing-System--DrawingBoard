#ifndef SETPENWIDTH_H
#define SETPENWIDTH_H

#include <QWidget>
#include "mycanvas.h"

namespace Ui {
class setPenwidth;
}

class setPenwidth : public QWidget
{
    Q_OBJECT

public:
    explicit setPenwidth(QWidget *parent = 0);
    ~setPenwidth();

    void loadInfo(myCanvas *canvas,int penwidth);

private:
    Ui::setPenwidth *ui;

    myCanvas *tempcanvas;
    int penwidth;
};

#endif // SETPENWIDTH_H
