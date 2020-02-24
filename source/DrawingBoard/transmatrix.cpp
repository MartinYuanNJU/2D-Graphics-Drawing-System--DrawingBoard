#include "transmatrix.h"

TransMatrix::TransMatrix()
{
    clearMatrix(buffermatrix);
    angle=0;
}

void TransMatrix::clearMatrix(double a[3][3])
{
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            a[i][j]=0;
    a[0][0]=1;
    a[1][1]=1;
    a[2][2]=1;
}

void TransMatrix::matrixMutiply(double a[3][3], double b[3][3])
{
    double temp[3][3]={0};
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            temp[i][j]=a[i][0]*b[0][j]+a[i][1]*b[1][j]+a[i][2]*b[2][j];
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            a[i][j]=temp[i][j];
}

void TransMatrix::transCoordinate(int &x,int &y)
{
    double a[3][3];
    clearMatrix(a);
    a[0][0]=(double)x;
    a[0][1]=(double)y;
    a[0][2]=1;
    matrixMutiply(a,buffermatrix);
    x=(int)round(a[0][0]);
    y=(int)round(a[0][1]);
}

void TransMatrix::rotateBuffer(int angle,int xc,int yc)
{
    double sinangle=sin(((double)angle/(double)180)*M_PI); //调用sin函数需要将角度转为弧度
    if(fabs(sinangle)<1e-5)
        sinangle=0;
    double cosangle=cos(((double)angle/(double)180)*M_PI);
    if(fabs(cosangle)<1e-5)
        cosangle=0;
    if(xc==0&&yc==0) //按照原点旋转，不需要左右乘旋转基准点矩阵
    {
        buffermatrix[0][0]=buffermatrix[1][1]=cosangle;
        buffermatrix[0][1]=sinangle;
        buffermatrix[1][0]=-sinangle;
        buffermatrix[0][2]=buffermatrix[1][2]=buffermatrix[2][0]=buffermatrix[2][1]=0;
        buffermatrix[2][2]=1;
    }
    else //其余情况，三个矩阵相乘得到变换矩阵
    {
        clearMatrix(buffermatrix);
        buffermatrix[2][0]=-(double)xc;
        buffermatrix[2][1]=-(double)yc;
        double tempa[3][3]={cosangle,sinangle,0,-sinangle,cosangle,0,0,0,1};
        double tempb[3][3]={1,0,0,0,1,0,(double)xc,(double)yc,1};
        matrixMutiply(buffermatrix,tempa);
        matrixMutiply(buffermatrix,tempb);
    }
}

void TransMatrix::rotateLine(int angle,int xc,int yc,int &x0,int &y0,int &x1,int &y1)
{
    if(angle==0)
        return;
    rotateBuffer(angle,xc,yc);
    transCoordinate(x0,y0);
    transCoordinate(x1,y1);
}

void TransMatrix::rotatePolygon(int angle,int xc,int yc,int *vertex,int pointnum)
{
    if(angle==0)
        return;
    rotateBuffer(angle,xc,yc);
    for(int i=0;i<pointnum;i++)
        transCoordinate(vertex[2*i],vertex[2*i+1]);
}

void TransMatrix::rotateEllipse(int angle,int x,int y,int &xc,int &yc)
{
    if(angle==0)
        return;
    rotateBuffer(angle,x,y); //先以当前旋转角度计算为变换椭圆圆心需要的旋转变换矩阵
    transCoordinate(xc,yc); //变换椭圆圆心
    this->angle+=angle; //计算累积旋转角度
    rotateBuffer(this->angle,xc,yc); //根据累积旋转角度以及变换后圆心计算绘制椭圆时需要的旋转变换矩阵
}

void TransMatrix::rotateRound(int angle,int x,int y,int &xc,int &yc)
{
    if(angle==0)
        return;
    rotateBuffer(angle,x,y); //先以当前旋转角度计算为变换圆心需要的旋转变换矩阵
    transCoordinate(xc,yc); //变换圆心
    this->angle+=angle; //计算累积旋转角度
    rotateBuffer(this->angle,xc,yc); //根据累积旋转角度以及变换后圆心计算绘制时需要的旋转变换矩阵
}

void TransMatrix::rotateCurve(int angle,int x,int y,QVector <QPoint> &pointset,int pointnum)
{
    if(angle==0)
        return;
    rotateBuffer(angle,x,y);
    for(int i=0;i<pointnum;i++)
    {
        int x=pointset[i].x();
        int y=pointset[i].y();
        transCoordinate(x,y);
        pointset[i].setX(x);
        pointset[i].setY(y);
    }
}

void TransMatrix::scaleBuffer(double scale,int xc,int yc)
{
    if(xc==0&&yc==0)
    {
        clearMatrix(buffermatrix);
        buffermatrix[0][0]=scale;
        buffermatrix[1][1]=scale;
    }
    else
    {
        clearMatrix(buffermatrix);
        buffermatrix[2][0]=-(double)xc;
        buffermatrix[2][1]=-(double)yc;
        double tempa[3][3]={scale,0,0,0,scale,0,0,0,1};
        double tempb[3][3]={1,0,0,0,1,0,(double)xc,(double)yc,1};
        matrixMutiply(buffermatrix,tempa);
        matrixMutiply(buffermatrix,tempb);
    }
}

void TransMatrix::scaleLine(double scale,int xc,int yc,int &x0,int &y0,int &x1,int &y1)
{
    if(scale==1)
        return;
    scaleBuffer(scale,xc,yc);
    transCoordinate(x0,y0);
    transCoordinate(x1,y1);
}

void TransMatrix::scalePolygon(double scale,int xc,int yc,int *vertex,int pointnum)
{
    if(scale==1)
        return;
    scaleBuffer(scale,xc,yc);
    for(int i=0;i<pointnum;i++)
        transCoordinate(vertex[2*i],vertex[2*i+1]);
}

void TransMatrix::scaleEllipse(double scale,int x,int y,int &xc,int &yc,int &rx,int &ry)
{
    if(scale==1) //缩放1倍等于不变
        return;
    scaleBuffer(scale,x,y); //计算缩放变换矩阵
    transCoordinate(xc,yc); //根据缩放变换矩阵变换圆心坐标
    rx=(int)round((double)rx*fabs(scale)); //按倍数缩放长半轴
    ry=(int)round((double)ry*fabs(scale)); //按倍数缩放短半轴
    rotateBuffer(this->angle,xc,yc); //根据新的圆心重新计算旋转变换矩阵
}

void TransMatrix::scaleRound(double scale,int x,int y,int &xc,int &yc,int &r)
{
    if(scale==1) //缩放1倍等于不变
        return;
    scaleBuffer(scale,x,y);
    transCoordinate(xc,yc);
    r=(int)round((double)r*fabs(scale));
    rotateBuffer(this->angle,xc,yc);
}

void TransMatrix::scaleCurve(double scale,int x,int y,int pointnum,QVector <QPoint> &pointset)
{
    if(scale==1)
        return;
    scaleBuffer(scale,x,y);
    for(int i=0;i<pointnum;i++)
    {
        int x1=pointset[i].x();
        int y1=pointset[i].y();
        transCoordinate(x1,y1);
        pointset[i].setX(x1);
        pointset[i].setY(y1);
    }
}

void TransMatrix::translateEllipse(int xc,int yc,int dx,int dy)
{
    xc+=dx;
    yc+=dy;
    rotateBuffer(this->angle,xc,yc);
}

void TransMatrix::translateRound(int xc,int yc,int dx,int dy)
{
    xc+=dx;
    yc+=dy;
    rotateBuffer(this->angle,xc,yc);
}

void TransMatrix::rotatePoint(int x,int y,int &x1,int &y1,int &x2,int &y2)
{
    if(angle==0)
        return;
    double temp[3][3];
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            temp[i][j]=buffermatrix[i][j];
    clearMatrix(buffermatrix);
    rotateBuffer(-angle,x,y);
    transCoordinate(x1,y1);
    transCoordinate(x2,y2);
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            buffermatrix[i][j]=temp[i][j];
}

void TransMatrix::rebuildEllipsematrix(int xc,int yc)
{
    clearMatrix(buffermatrix);
    rotateBuffer(this->angle,xc,yc);
}

void TransMatrix::rebuildRoundmatrix(int xc,int yc)
{
    clearMatrix(buffermatrix);
    rotateBuffer(this->angle,xc,yc);
}

int TransMatrix::getAngle()
{
    return this->angle;
}
