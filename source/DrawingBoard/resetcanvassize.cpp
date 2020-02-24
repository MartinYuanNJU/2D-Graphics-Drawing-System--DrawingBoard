#include "resetcanvassize.h"
#include "ui_resetcanvassize.h"

ResetCanvasSize::ResetCanvasSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetCanvasSize)
{
    ui->setupUi(this);
    QFont font("Times", 15, QFont::DemiBold);
    ui->label->setFont(font);
    ui->label_2->setFont(font);
    ui->label->repaint();
    ui->label_2->repaint();
    this->setAttribute(Qt::WA_DeleteOnClose,true);
}

ResetCanvasSize::~ResetCanvasSize()
{
    tempcanvas=NULL;
    delete ui;
}

void ResetCanvasSize::loadInfo(int width,int height,myCanvas *canvas)
{
    tempcanvas=canvas;
    tempwidth=width;
    tempheight=height;
    ui->spinBox->setDisplayIntegerBase(10);
    ui->spinBox->setMinimum(100);
    ui->spinBox->setMaximum(1230);
    ui->spinBox->setSingleStep(1);
    ui->spinBox->setValue(width);
    ui->spinBox_2->setDisplayIntegerBase(10);
    ui->spinBox_2->setMinimum(100);
    ui->spinBox_2->setMaximum(640);
    ui->spinBox_2->setSingleStep(1);
    ui->spinBox_2->setValue(height);
    ui->spinBox->repaint();
    ui->spinBox_2->repaint();
}

void ResetCanvasSize::on_buttonBox_accepted()
{
    int newwidth=ui->spinBox->value();
    int newheight=ui->spinBox_2->value();
    if(newwidth!=tempwidth||newheight!=tempheight)
        tempcanvas->resetCanvasEdit(newwidth,newheight,true);
    this->close();
}

void ResetCanvasSize::on_buttonBox_rejected()
{
    this->close();
}
