#ifndef COMMON_H
#define COMMON_H

#include <QApplication>
#include <QColorDialog>
#include <QButtonGroup>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <strstream>
#include <iostream>
#include <QPainter>
#include <stdlib.h>
#include <fstream>
#include <QVector>
#include <QAction>
#include <QWidget>
#include <QString>
#include <string>
#include <QDebug>
#include <QImage>
#include <QPoint>
#include <QColor>
#include <QLabel>
#include <cmath>
#include <regex>
#include <stack>
#include <QMenu>
#include <QIcon>
#include <QDir>
#include <QRgb>
#include <QPen>
#include <list>
#include <map>
using namespace std;

#endif // COMMON_H



/*void myCanvas::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(iscreatingshape)
        {
            if(createshape==Line)
            {
                QPoint presspoint=event->pos();
                pointvector.push_back(presspoint);
                temppaintboard=*paintingboard;
                draw_on_real_board=false;
            }
            else if(createshape==Polygon)
            {
                QPoint presspoint=event->pos();
                if(ispolygonfirst)
                {
                    ispolygonfirst=false;
                    draw_on_real_board=false;
                    bufferboard=*paintingboard;
                    temppaintboard=bufferboard;
                    pointvector.push_back(presspoint);
                    polygonpointnum++;
                }
                else
                {
                    temppaintboard=bufferboard;
                    drawtempLine_ui(polygonpointnum-1,presspoint);
                }
            }
            else if(createshape==Ellipse)
            {
                QPoint presspoint=event->pos();
                pointvector.push_back(presspoint);
                temppaintboard=*paintingboard;
                draw_on_real_board=false;
            }
        }
        else if(iseditingshape)
        {
            if(editshape==Line)
            {
                if(editaction==Clip)
                {
                    QPoint presspoint=event->pos();
                    cliplinewindow.xmin=presspoint.x();
                    cliplinewindow.ymin=presspoint.y();
                    temppaintboard=*paintingboard;
                    draw_on_real_board=false;
                }
            }
        }
    }
}

void myCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(iscreatingshape)
        {
            if(createshape==Line)
            {
                draw_on_real_board=true;
                QPoint releasepoint=event->pos();
                pointvector.push_back(releasepoint);
                createLine_ui();
            }
            else if(createshape==Polygon)
            {
                QPoint releasepoint=event->pos();
                QPoint lastpoint=pointvector[polygonpointnum-1];
                if(releasepoint.x()!=lastpoint.x()||releasepoint.y()!=lastpoint.y())
                {
                    pointvector.push_back(releasepoint);
                    polygonpointnum++;
                }
                bufferboard=temppaintboard;
            }
            else if(createshape==Ellipse)
            {
                draw_on_real_board=true;
                QPoint releasepoint=event->pos();
                pointvector.push_back(releasepoint);
                createEllipse_ui();
            }
        }
        else if(iseditingshape)
        {
            if(editshape==Line)
            {
                if(editaction==Clip)
                {
                    draw_on_real_board=true;
                    QPoint releasepoint=event->pos();
                    cliplinewindow.xmax=releasepoint.x();
                    cliplinewindow.ymax=releasepoint.y();
                    clipLine_ui();
                }
            }
        }
    }
}

void myCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton)
    {
        if(iscreatingshape)
        {
            if(createshape==Line)
            {
               temppaintboard=*paintingboard;
               QPoint nowpoint=event->pos();
               drawtempLine_ui(0,nowpoint);
            }
            else if(createshape==Polygon)
            {
                temppaintboard=bufferboard;
                QPoint nowpoint=event->pos();
                drawtempLine_ui(polygonpointnum-1,nowpoint);
            }
            else if(createshape==Ellipse)
            {
                temppaintboard=*paintingboard;
                QPoint nowpoint=event->pos();
                drawtempEllipse_ui(nowpoint);
            }
        }
        else if(iseditingshape)
        {
            if(editshape==Line)
            {
                if(editaction==Clip)
                {
                    temppaintboard=*paintingboard;
                    QPoint nowpoint=event->pos();
                    drawtempClipWindow_ui(nowpoint);
                }
            }
        }
    }
}

void myCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(iscreatingshape)
    {
        if(createshape==Polygon)
        {
            draw_on_real_board=true;
            createPolygon_ui();
        }
    }
}*/
