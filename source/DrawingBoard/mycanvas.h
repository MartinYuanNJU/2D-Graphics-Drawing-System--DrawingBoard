#ifndef MYCANVAS_H
#define MYCANVAS_H

#include "common.h"
#include "transmatrix.h"

enum Shape{
    NoneShape,Line,Ellipse,Curve,Polygon,Round
};

enum drawingAlgorithm{
    DDA,Bresenham,CohenSutherland,LiangBarsky,Bezier,Bspline
};

enum editAction{
    NoneAction,Create,Translate,Rotate,Scale,Clip,Select
};

enum drawBoard{
    paintBoard,tempBoard,bufferBoard
};

enum fileAction{
    None,New,Open,Save,Saveas
};

enum saveFileSuffix{
    BMP,PNG,JPG,JPEG
};

enum moveDirection{
    NoneDirection,Vertical,Horizontal,Diagonal
};

struct RGBcolor{
    int red;
    int green;
    int blue;
};

struct Rectangle{
    QPoint leftup; //0
    QPoint leftdown; //1
    QPoint rightup; //2
    QPoint rightdown; //3
    QPoint center;
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
    QVector <QPoint> drawpointset;
    Rectangle rect;
};

struct myPolygon{
    int id;
    int pointnum;
    int *pointset;
    drawingAlgorithm method;
    TransMatrix polygonmatrix;
    QVector <QPoint> drawpointset;
    Rectangle rect;
};

struct myEllipse{
    int id;
    int xc;
    int yc;
    int rx;
    int ry;
    TransMatrix ellipsematrix;
    QVector <QPoint> drawpointset;
    Rectangle rect;
};

struct myRound{
    int id;
    int xc;
    int yc;
    int r;
    TransMatrix roundmatrix;
    QVector <QPoint> drawpointset;
    Rectangle rect;
};

struct myCurve{
    int id;
    int pointnum;
    QVector <QPoint> pointset;
    drawingAlgorithm method;
    TransMatrix curvematrix;
    QVector <QPoint> drawpointset;
    Rectangle rect;
};

struct doublePoint{
    double x;
    double y;
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
    QColor fillcolor;
    bool fill;
    //saveFileSuffix suffix;
};

class myCanvas : public QWidget
{
    Q_OBJECT

private:
    //begining of data type
    QString savepath;

    bool draw_on_real_board; //指定当前屏幕上可视化是最终画布还是临时画布

    QImage *paintingboard; //主要的绘图区域（画像素点区域）

    QImage temppaintboard; //临时画布，用于绘制鼠标交互跟踪鼠标的图像
    QImage bufferboard; //二重临时画布，用于多边形绘制

    drawBoard drawboard;
    QColor *color;
    QPen *drawpen;
    QColor *fillcolor;
    QPen *fillpen;
    QPen *disclipen; //绘制多边形起始点终点连接虚线

    int penwidth;
    int width; //画布宽度(横向)
    int height; //画布高度(纵向)
    const int maxwidth=1230;
    const int maxheight=640;
    const int minwidth=100;
    const int minheight=100;
    bool isrepaint; //是否是重设画布之后重绘图形
    bool fill;

    drawingAlgorithm linealgorithm;
    drawingAlgorithm polygonalgorithm;
    drawingAlgorithm clipalgorithm;
    drawingAlgorithm curvealgorithm;

    QPen *pointpen;
    QPen solidpointpen;
    QVector <QPoint> pointvector; //存放鼠标交互中用户确定的图元控制点
    bool iscreatingshape; //当前是否在绘制图形
    Shape createshape; //即将要绘制的图形
    int polygonpointnum;
    bool ispolygonfirst; //用户通过鼠标绘制时，点击的点是否为多边形第一个点
    int curvepointnum;
    bool curvelinedrawed; //曲线起始的直线（决定起始终止控制点）是否已经绘制

    QPen *clipwindowpen;
    bool iseditingshape; //当前是否在编辑图形
    Shape editshape; //当前正在编辑的图形形状
    editAction editaction; //当前用户想进行的操作
    clipWindow cliplinewindow;

    //the following is used to store these graphic points
    map <int,myGraph> *graphset;
    map <int,myLine> *lineset;
    map <int,myPolygon> *polygonset;
    map <int,myEllipse> *ellipseset;
    map <int,myRound> *roundset;
    map <int,myCurve> *curveset;

    list <int> sequence;

    QString nowfilename;
    bool hasedited;
    //saveFileSuffix suffix;
    moveDirection direction;
    bool iseditingcanvas;
    int tempwidth;
    int tempheight;

    int selectid; //当前鼠标点选了哪个图元进行编辑
    int editid; //刚刚对哪个图元进行了编辑

    bool drawoutline; //指示是否画出选中图元的控制点信息
    bool onselectgraph; //平移时指示鼠标是否移动到图元上方
    bool centerdrawed; //指示旋转或者缩放中心已经按下
    bool firstpointdrawed;
    QPen *centerpen;

    QPoint lastpoint; //鼠标移动时指示上一瞬间鼠标位置
    QPoint centerpoint;
    int lastangle;
    Rectangle prevrect;
    QPoint settledpoint;

    bool oncontrolpoint; //选择图元后指示鼠标是否在控制点上
    int controlpoint; //指示是哪一个控制点
    Shape controlshape;
    map <int,myLine>::iterator uiline;
    map <int,myPolygon>::iterator uipolygon;
    map <int,myEllipse>::iterator uiellipse;
    map <int,myRound>::iterator uiround;
    map <int,myCurve>::iterator uicurve;
    QPoint pivotpoint; //修改点的时候原先图形点的位置

    Shape scaleshape;
    myLine prevline;
    myPolygon prevpolygon;
    myEllipse prevellipse;
    myRound prevround;
    myCurve prevcurve;

    QString statusinfo;

    //begining of function type

public:
    explicit myCanvas(QWidget *parent = nullptr);

    void handleFileAction(fileAction action);
    void newFile();
    void openFile();
    void saveFile();
    void saveFileas();
    QString getNowFilename();
    //void setSuffix();

    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *event); //重载鼠标按下事件触发函数
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void loadSavePath(string savepath);
    bool saveToFile(string filepath); //保存当前画布到文件

    int getWidth(); //外部获取画布宽度
    int getHeight(); //外部获取画布高度
    int getPenwidth();

    void getInformation(Information &information);
    void setPreferences(Information &information);

    void repaintAll(); //修改画布大小后重绘所有图元
    void repaintAll_improved();
    bool resetCanvas(int width,int height,bool remain); //修改画布大小
    bool setColor(int r,int g,int b); //设置画笔颜色
    bool setFillColor(int r,int g,int b);
    bool setPenwidth(int penwidth);

    void paintSingleLine_DDA(int x0,int y0,int x1,int y1,QPainter *painter); //DDA画线算法
    void paintSingleLine_Bresenham(int x0,int y0,int x1,int y1,QPainter *painter); //Bresenham画线算法
    void paintDiscreteLine_Bresenham(int x0,int y0,int x1,int y1,QPainter *painter);
    void paintSingleLine_DDA(int x0,int y0,int x1,int y1,QPainter *painter,QVector <QPoint> &pointset);
    void paintSingleLine_Bresenham(int x0,int y0,int x1,int y1,QPainter *painter,QVector <QPoint> &pointset);
    bool drawLine(int id,int x0,int y0,int x1,int y1,drawingAlgorithm method); //画直线对外接口
    bool drawPolygon(int id,int *pointset,int pointnum,drawingAlgorithm method); //画多边形对外接口
    void drawEllipsePoint(int xc,int yc,int x,int y,int lastx,int lasty,QPainter *painter,TransMatrix *tmatrix,QVector <QPoint> &pointset);
    bool drawEllipse(int id,int xc,int yc,int rx,int ry); //中点椭圆绘制算法
    void drawRoundPoint(int xc,int yc,int x,int y,QPainter *painter,QVector <QPoint> &pointset);
    void drawRoundPoint(int xc,int yc,int x,int y,QPainter *painter);
    bool drawRound(int id,int xc,int yc,int r);
    bool drawCurve(int id,QVector <QPoint> &pointset,int pointnum,drawingAlgorithm method);
    double minStep_curve(QVector <QPoint> &set,int n,drawingAlgorithm method);
    void drawCurve_Bezier(QVector <QPoint> &set,int n,QPainter *painter,QVector <QPoint> &pointset);
    void drawCurve_Bspline(QVector <QPoint> &set,int n,QPainter *painter,QVector <QPoint> &pointset);

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

    //UI界面操作
    void setAction(Shape shape,editAction action); //确定菜单栏点击的操作
    int generateID(); //在交互界面为图元生成id

    QPainter *setPainter(QPen *pen);

    bool assertion();
    void paintPoint(int x,int y,QPainter *painter,QPen &pen);
    void createLine_ui(); //交互界面绘制直线
    void drawtempLine_ui(int pivot,QPoint &point,bool fluent,QPen *pen); //交互界面用户未确定时绘制临时直线
    void createPolygon_ui();
    void drawtempEllipse_ui(QPoint &point);
    void createEllipse_ui();
    void drawtempRound_ui(QPoint &point);
    void createRound_ui();
    void drawtempClipWindow_ui(QPoint &point);
    void clipLine_ui();
    void drawtempCurve_ui(QPoint &point);
    void createCurve_ui();

    bool closeEnough_point(QPoint &point,int xstandard,int ystandard,int threshold,bool strict);
    void editCanvas(QPoint &point,moveDirection direction,bool final);
    void resetCanvasEdit(int width,int height,bool final);
    void paintGraphDot(QVector <QPoint> &drawpointset); //未作修改的图元重绘直接按照存储的绘制点绘制
    void changeSequence(int id,bool insert);
    void drawOutline(int nowid,Shape shape);
    bool closeEnough_drawpointset(QVector <QPoint> &drawpointset,int pivotx,int pivoty);
    void selectGraph(QPoint &point); //鼠标点击选择图元
    bool closeEnough_graph(int nowid,QPoint &point); //判断平移时鼠标是否浮动到足够靠近图元
    void paintOutline_rect(Rectangle &rect,QPainter *painter,QPen &temppen); //绘制图元控制点及控制矩形
    void translate_ui(QPoint &point);
    void deleteGraph();
    int rotateAngle_ui(QPoint &point);
    void closeEnough_selectcenter(QPoint &point);
    void paintCenterPoint();
    void rotate_ui(QPoint &point,bool first,bool final);
    bool selectGraphPoint(QPoint &point);
    void editGraph_ui(QPoint &point);
    void projectPoint(QPoint &a,QPoint &b,QPoint &point,QPoint &projectpoint);
    void copyInformation(bool first);
    void scale_ui(QPoint &point,bool first,bool final);

    bool closeWindow();

signals:
    void send_resCan_signal();
    void sendStatusbarinfo(QString info);

public slots:

};

#endif // MYCANVAS_H
