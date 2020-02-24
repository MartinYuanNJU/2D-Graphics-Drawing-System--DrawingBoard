#ifndef TRANSMATRIX_H
#define TRANSMATRIX_H

#include "common.h"

class TransMatrix
{
private:
    double buffermatrix[3][3]; //变换矩阵
    int angle; //旋转角度，只有椭圆需要

    void clearMatrix(double a[3][3]); //清空矩阵
    void matrixMutiply(double a[3][3], double b[3][3]); //3*3矩阵与3*3矩阵作矩阵乘法
    void rotateBuffer(int angle,int xc,int yc); //计算旋转变换矩阵
    void scaleBuffer(double scale,int xc,int yc); //计算缩放变换矩阵
public:
    TransMatrix();
    void transCoordinate(int &x,int &y); //根据变换矩阵计算变换后的坐标
    void rotateLine(int angle,int xc,int yc,int &x0,int &y0,int &x1,int &y1); //旋转直线
    void rotatePolygon(int angle,int xc,int yc,int *vertex,int pointnum); //旋转多边形
    void rotateEllipse(int angle,int x,int y,int &xc,int &yc);
    void rotateCurve(int angle,int x,int y,QVector <QPoint> &pointset,int pointnum);
    void scaleLine(double scale,int xc,int yc,int &x0,int &y0,int &x1,int &y1);
    void scalePolygon(double scale,int xc,int yc,int *vertex,int pointnum);
    void scaleEllipse(double scale,int x,int y,int &xc,int &yc,int &rx,int &ry);
    void translateEllipse(int xc,int yc,int dx,int dy);
    void scaleCurve(double scale,int x,int y,int pointnum,QVector <QPoint> &pointset);
};

#endif // TRANSMATRIX_H
