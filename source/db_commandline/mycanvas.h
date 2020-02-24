#ifndef MYCANVAS_H
#define MYCANVAS_H

#include "common.h"
#include "transmatrix.h"

enum Shape{
    NoneShape,Line,Ellipse,Curve,Polygon
};

enum drawingAlgorithm{
    DDA,Bresenham,CohenSutherland,LiangBarsky,Bezier,Bspline
};

enum editAction{
    NoneAction,Create,Translate,Rotate,Scale,Clip
};

struct RGBcolor{
    int red;
    int green;
    int blue;
};

struct myGraph{
    int id;
    Shape shape;
    QColor rgbcolor;
    int penwidth;
};

struct myLine{
    int id;
    int x0;
    int y0;
    int x1;
    int y1;
    drawingAlgorithm method;
    TransMatrix linematrix;
    //QVector <QPoint> drawpointset;
};

struct myPolygon{
    int id;
    int pointnum;
    int *pointset;
    drawingAlgorithm method;
    TransMatrix polygonmatrix;
    //QVector <QPoint> drawpointset;
};

struct myEllipse{
    int id;
    int xc;
    int yc;
    int rx;
    int ry;
    TransMatrix ellipsematrix;
    //QVector <QPoint> drawpointset;
};

struct myCurve{
    int id;
    int pointnum;
    QVector <QPoint> pointset;
    drawingAlgorithm method;
    TransMatrix curvematrix;
};

struct clipWindow{
    int xmin;
    int ymin;
    int xmax;
    int ymax;
};

struct clipWindowCode{
    uint8_t top=8;
    uint8_t down=4;
    uint8_t left=1;
    uint8_t right=2;
};

struct Information{
    drawingAlgorithm line;
    drawingAlgorithm polygon;
    drawingAlgorithm curve;
    drawingAlgorithm clip;
    QColor color;
    int penwidth;
};

struct doublePoint{
    double x;
    double y;
};

class myCanvas : public QWidget
{
    Q_OBJECT

private:
    //begining of data type
    QString savepath;
    QImage *paintingboard; //主要的绘图区域（画像素点区域）

    QColor *color;
    QPen *drawpen;
    int penwidth;
    int width; //画布宽度(横向)
    int height; //画布高度(纵向)
    bool isrepaint; //是否是重设画布之后重绘图形

    //the following is used to store these graphic points
    map <int,myGraph> *graphset;
    map <int,myLine> *lineset;
    map <int,myPolygon> *polygonset;
    map <int,myEllipse> *ellipseset;
    map <int,myCurve> *curveset;

    //begining of function type

public:
    explicit myCanvas(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*);

    void loadSavePath(string savepath);

    void paintSingleLine_DDA(int x0,int y0,int x1,int y1,QPainter *painter); //DDA画线算法
    void paintSingleLine_Bresenham(int x0,int y0,int x1,int y1,QPainter *painter); //Bresenham画线算法

    void paintSingleLine_DDA(int x0,int y0,int x1,int y1,QPainter *painter,QVector <QPoint> &pointset);
    void paintSingleLine_Bresenham(int x0,int y0,int x1,int y1,QPainter *painter,QVector <QPoint> &pointset);

    void repaintAll(); //修改画布大小后重绘所有图元
    bool resetCanvas(int width,int height); //修改画布大小
    bool resetCanvas_commandine(int width,int height);
    bool setColor(int r,int g,int b); //设置画笔颜色
    bool saveToFile(string filepath); //保存当前画布到文件

    double minStep_curve(QVector <QPoint> &set,int n,drawingAlgorithm method);
    void drawCurve_Bezier(QVector <QPoint> &set,int n,QPainter *painter);
    void drawCurve_Bspline(QVector <QPoint> &set,int n,QPainter *painter);

    bool drawLine(int id,int x0,int y0,int x1,int y1,drawingAlgorithm method); //画直线对外接口
    bool drawPolygon(int id,int *pointset,int pointnum,drawingAlgorithm method); //画多边形对外接口
    void drawEllipsePoint(int xc,int yc,int x,int y,QPainter *painter,TransMatrix *tmatrix);
    bool drawEllipse(int id,int xc,int yc,int rx,int ry); //中点椭圆绘制算法
    bool drawCurve(int id,QVector <QPoint> &pointset,int pointnum,drawingAlgorithm method);

    void swapNum(int &a,int &b);
    void swapNum(uint8_t &a,uint8_t &b);
    void swapNum(double &a,double &b);
    bool translate(int id,int dx,int dy); //对图元平移
    bool rotate(int id,int x,int y,int r);
    bool scale(int id,int x,int y,double r);
    bool clip(int id,clipWindow *clipwindow,drawingAlgorithm method); //对直线裁剪统一接口
    void encode_CohenSutherland(int x,int y,clipWindow *clipwindow,uint8_t &code); //对端点进行区域编码
    bool clipLine_CohenSutherland(int &x0,int &y0,int &x1,int &y1,clipWindow *clipwindow); //编码裁剪算法
    bool clipLine_LiangBarsky(int &x0,int &y0,int &x1,int &y1,clipWindow *clipwindow);

public slots:

};

#endif // MYCANVAS_H
