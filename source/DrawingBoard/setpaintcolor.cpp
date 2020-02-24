#include "setpaintcolor.h"
#include "ui_setpaintcolor.h"

setPaintColor::setPaintColor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setPaintColor)
{
    ui->setupUi(this);

    QFont font("Times", 14, QFont::DemiBold);
    ui->label->setFont(font);
    //ui->label_2->setFont(font);
    ui->label->repaint();
    //ui->label_2->repaint();

//    ui->checkBox->setFont(font);
//    ui->checkBox->repaint();

    this->setAttribute(Qt::WA_DeleteOnClose,true);
}

setPaintColor::~setPaintColor()
{
    tempcanvas=NULL;
    delete ui;
}

void setPaintColor::loadInfo(myCanvas *canvas,Information &info)
{
    tempcanvas=canvas;
    information=info;

    QString colorstring="background: rgb(";
    colorstring+=QString::number(information.color.red());
    colorstring+=",";
    colorstring+=QString::number(information.color.green());
    colorstring+=",";
    colorstring+=QString::number(information.color.blue());
    colorstring+=")";
    ui->pushButton->setStyleSheet(colorstring);
    ui->pushButton->repaint();

//    QString fillcolorstring="background: rgb(";
//    fillcolorstring+=QString::number(information.fillcolor.red());
//    fillcolorstring+=",";
//    fillcolorstring+=QString::number(information.fillcolor.green());
//    fillcolorstring+=",";
//    fillcolorstring+=QString::number(information.fillcolor.blue());
//    fillcolorstring+=")";
//    ui->pushButton_2->setStyleSheet(fillcolorstring);
//    ui->pushButton_2->repaint();

//    if(information.fill)
//        ui->checkBox->setChecked(true);
//    else
//        ui->checkBox->setChecked(false);
//    ui->checkBox->repaint();

    tcolor=information.color;
    tfillcolor=information.fillcolor;
}

void setPaintColor::on_pushButton_clicked()
{
    QColor nowcolor=QColorDialog::getColor(tcolor);
    if(nowcolor.isValid())
    {
        tcolor=nowcolor;
        QString colorstring="background: rgb(";
        colorstring+=QString::number(tcolor.red());
        colorstring+=",";
        colorstring+=QString::number(tcolor.green());
        colorstring+=",";
        colorstring+=QString::number(tcolor.blue());
        colorstring+=")";
        ui->pushButton->setStyleSheet(colorstring);
        ui->pushButton->repaint();
    }
}

//void setPaintColor::on_pushButton_2_clicked()
//{
//    QColor nowcolor=QColorDialog::getColor(tfillcolor);
//    if(nowcolor.isValid())
//    {
//        tfillcolor=nowcolor;
//        QString fillcolorstring="background: rgb(";
//        fillcolorstring+=QString::number(tfillcolor.red());
//        fillcolorstring+=",";
//        fillcolorstring+=QString::number(tfillcolor.green());
//        fillcolorstring+=",";
//        fillcolorstring+=QString::number(tfillcolor.blue());
//        fillcolorstring+=")";
//        ui->pushButton_2->setStyleSheet(fillcolorstring);
//        ui->pushButton_2->repaint();
//    }
//}

void setPaintColor::on_buttonBox_accepted()
{
    information.color=tcolor;
    information.fillcolor=tfillcolor;
//    if(ui->checkBox->isChecked())
//        information.fill=true;
//    else
//        information.fill=false;
    tempcanvas->setPreferences(information);
}

void setPaintColor::on_buttonBox_rejected()
{
    this->close();
}
