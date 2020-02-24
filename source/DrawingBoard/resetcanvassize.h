#ifndef RESETCANVASSIZE_H
#define RESETCANVASSIZE_H

#include <QDialog>
#include "mycanvas.h"

namespace Ui {
class ResetCanvasSize;
}

class ResetCanvasSize : public QDialog
{
    Q_OBJECT

public:
    explicit ResetCanvasSize(QWidget *parent = 0);
    ~ResetCanvasSize();
    void loadInfo(int width,int height,myCanvas *canvas);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ResetCanvasSize *ui;
    myCanvas *tempcanvas;
    int tempwidth;
    int tempheight;
};

#endif // RESETCANVASSIZE_H
