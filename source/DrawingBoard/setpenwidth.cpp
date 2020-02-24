#include "setpenwidth.h"
#include "ui_setpenwidth.h"

setPenwidth::setPenwidth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setPenwidth)
{
    ui->setupUi(this);

    ui->spinBox->setDisplayIntegerBase(10);
    ui->spinBox->setMinimum(1);
    ui->spinBox->setMaximum(10);
    ui->spinBox->setSingleStep(1);

    ui->horizontalSlider->setMinimum(1);
    ui->horizontalSlider->setMaximum(10);
    ui->horizontalSlider->setSingleStep(1);

    connect(ui->spinBox, SIGNAL(valueChanged(int)), ui->horizontalSlider, SLOT(setValue(int)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), ui->spinBox, SLOT(setValue(int)));

    this->setAttribute(Qt::WA_DeleteOnClose,true);
}

setPenwidth::~setPenwidth()
{
    penwidth=ui->spinBox->value();
    tempcanvas->setPenwidth(penwidth);
    tempcanvas=NULL;
    delete ui;
}

void setPenwidth::loadInfo(myCanvas *canvas,int penwidth)
{
    tempcanvas=canvas;
    this->penwidth=penwidth;

    ui->spinBox->setValue(penwidth);
    ui->horizontalSlider->setValue(penwidth);
}
