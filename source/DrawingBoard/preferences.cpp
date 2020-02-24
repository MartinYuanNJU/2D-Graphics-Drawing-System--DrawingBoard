#include "preferences.h"
#include "ui_preferences.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    QFont font("Times", 15, QFont::DemiBold);
    ui->label->setFont(font);
    ui->label_2->setFont(font);
    ui->label_3->setFont(font);
    ui->label_4->setFont(font);
    ui->label_5->setFont(font);
    //ui->label_6->setFont(font);
    //ui->label_7->setFont(font);
    ui->label->repaint();
    ui->label_2->repaint();
    ui->label_3->repaint();
    ui->label_4->repaint();
    ui->label_5->repaint();
    //ui->label_6->repaint();
    this->setAttribute(Qt::WA_DeleteOnClose,true);

    bg1=new QButtonGroup();
    bg1->addButton(ui->radioButton,0);
    bg1->addButton(ui->radioButton_2,1);
    bg2=new QButtonGroup();
    bg2->addButton(ui->radioButton_3,0);
    bg2->addButton(ui->radioButton_4,1);
    bg3=new QButtonGroup();
    bg3->addButton(ui->radioButton_5,0);
    bg3->addButton(ui->radioButton_6,1);
    bg4=new QButtonGroup();
    bg4->addButton(ui->radioButton_7,0);
    bg4->addButton(ui->radioButton_8,1);

    /*ui->comboBox->addItem(tr(".bmp"));
    ui->comboBox->addItem(tr(".png"));
    ui->comboBox->addItem(tr(".jpg"));
    ui->comboBox->addItem(tr(".jpeg"));*/
}

Preferences::~Preferences()
{
    tempcanvas=NULL;
    delete ui;
}

void Preferences::loadInfo(myCanvas *canvas,Information &info)
{
    tempcanvas=canvas;
    information=info;

    ui->spinBox->setDisplayIntegerBase(10);
    ui->spinBox->setMinimum(0);
    ui->spinBox->setMaximum(255);
    ui->spinBox->setSingleStep(1);
    ui->spinBox->setValue(information.color.red());
    ui->spinBox_2->setDisplayIntegerBase(10);
    ui->spinBox_2->setMinimum(0);
    ui->spinBox_2->setMaximum(255);
    ui->spinBox_2->setSingleStep(1);
    ui->spinBox_2->setValue(information.color.green());
    ui->spinBox_3->setDisplayIntegerBase(10);
    ui->spinBox_3->setMinimum(0);
    ui->spinBox_3->setMaximum(255);
    ui->spinBox_3->setSingleStep(1);
    ui->spinBox_3->setValue(information.color.blue());
    ui->spinBox->repaint();
    ui->spinBox_2->repaint();
    ui->spinBox_3->repaint();

    //ui->label_7->setText(QString::number(information.penwidth));
    //ui->label_7->repaint();

    /*ui->horizontalSlider->setMinimum(1);
    ui->horizontalSlider->setMaximum(10);
    ui->horizontalSlider->setSingleStep(1);
    ui->horizontalSlider->setValue(information.penwidth);*/

    if(information.line==DDA)
        ui->radioButton->setChecked(true);
    else
        ui->radioButton_2->setChecked(true);
    if(information.polygon==DDA)
        ui->radioButton_3->setChecked(true);
    else
        ui->radioButton_4->setChecked(true);
    if(information.curve==Bezier)
        ui->radioButton_5->setChecked(true);
    else
        ui->radioButton_6->setChecked(true);
    if(information.clip==CohenSutherland)
        ui->radioButton_7->setChecked(true);
    else
        ui->radioButton_8->setChecked(true);

    /*switch(information.suffix)
    {
        case BMP:
        {
            ui->comboBox->setCurrentIndex(0);
            break;
        }
        case PNG:
        {
            ui->comboBox->setCurrentIndex(1);
            break;
        }
        case JPG:
        {
            ui->comboBox->setCurrentIndex(2);
            break;
        }
        case JPEG:
        {
            ui->comboBox->setCurrentIndex(3);
            break;
        }
    }*/
}

/*void Preferences::on_horizontalSlider_valueChanged(int value)
{
    ui->label_7->setText(QString::number(value));
    ui->label_7->repaint();
}*/

void Preferences::on_buttonBox_accepted()
{
    if(bg1->checkedId()==0)
        information.line=DDA;
    else
        information.line=Bresenham;
    if(bg2->checkedId()==0)
        information.polygon=DDA;
    else
        information.polygon=Bresenham;
    if(bg3->checkedId()==0)
        information.curve=Bezier;
    else
        information.curve=Bspline;
    if(bg4->checkedId()==0)
        information.clip=CohenSutherland;
    else
        information.clip=LiangBarsky;
    information.color.setRgb(ui->spinBox->value(),ui->spinBox_2->value(),ui->spinBox_3->value());
    //information.penwidth=ui->horizontalSlider->value();
    /*switch(ui->comboBox->currentIndex())
    {
        case 0:
        {
            information.suffix=BMP;
            break;
        }
        case 1:
        {
            information.suffix=PNG;
            break;
        }
        case 2:
        {
            information.suffix=JPG;
            break;
        }
        case 3:
        {
            information.suffix=JPEG;
            break;
        }
    }*/

    tempcanvas->setPreferences(information);
    this->close();
}

void Preferences::on_buttonBox_rejected()
{
    this->close();
}
