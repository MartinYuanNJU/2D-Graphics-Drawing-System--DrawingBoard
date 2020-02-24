#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include "mycanvas.h"

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();
    void loadInfo(myCanvas *canvas,Information &info);
private slots:
    //void on_horizontalSlider_valueChanged(int value);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::Preferences *ui;

    QButtonGroup *bg1;
    QButtonGroup *bg2;
    QButtonGroup *bg3;
    QButtonGroup *bg4;

    myCanvas *tempcanvas;
    Information information;
};

#endif // PREFERENCES_H
