#include "mycanvas.h"

myCanvas::myCanvas(QWidget *parent):QWidget(parent)
{
    graphset=new map<int,myGraph>;
    lineset=new map<int,myLine>;
    polygonset=new map<int,myPolygon>;
    ellipseset=new map<int,myEllipse>;
    curveset=new map <int,myCurve>;

    isrepaint=false;
    width=1000;
    height=650;

    paintingboard=new QImage(width,height,QImage::Format_RGB32);
    QRgb background=qRgb(255,255,255);
    paintingboard->fill(background);
    color=new QColor;
    color->setRgb(0,0,0);
    drawpen=new QPen;
    drawpen->setColor(*color);
    penwidth=1;
    drawpen->setWidth(penwidth);
    drawpen->setStyle(Qt::SolidLine);
    drawpen->setCapStyle(Qt::RoundCap);
    drawpen->setJoinStyle(Qt::RoundJoin);
}

void myCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(0,0,*paintingboard);
}

void myCanvas::loadSavePath(string savepath)
{
    this->savepath=QString::fromStdString(savepath);
    int size=this->savepath.size();
    if(this->savepath[size-1]!='/')
        this->savepath+='/';
}

void myCanvas::repaintAll()
{
    isrepaint=true;
    map <int,myGraph>::iterator current;
    current=graphset->begin();
    QColor tempcolor=*color; //存储原有画笔颜色
    int temppenwidth=penwidth; //存储原有画笔粗细
    for(;current!=graphset->end();current++)
    {
        int nowid=current->first;
        Shape shape=current->second.shape; //获取当前需要重绘的图形形状
        QColor rgb=current->second.rgbcolor;
        penwidth=current->second.penwidth; //获取当前需要重绘的图形画笔粗细
        setColor(rgb.red(),rgb.green(),rgb.blue()); //依据其存储的绘制颜色更新画笔颜色以及画笔粗细
        switch (shape)
        {
            case Line:
            {
                map <int,myLine>::iterator currentline;
                currentline=lineset->find(nowid);
                drawLine(nowid,currentline->second.x0,currentline->second.y0,currentline->second.x1,
currentline->second.y1,currentline->second.method);
                break;
            }
            case Polygon:
            {
                map <int,myPolygon>::iterator currentpolygon;
                currentpolygon=polygonset->find(nowid);
                drawPolygon(nowid,currentpolygon->second.pointset,currentpolygon->second.pointnum,
currentpolygon->second.method);
                break;
            }
            case Ellipse:
            {
                map <int,myEllipse>::iterator currentellipse;
                currentellipse=ellipseset->find(nowid);
                drawEllipse(nowid,currentellipse->second.xc,currentellipse->second.yc,currentellipse->second.rx,
currentellipse->second.ry);
                break;
            }
            case Curve:
            {
                map <int,myCurve>::iterator currentcurve;
                currentcurve=curveset->find(nowid);
                drawCurve(nowid,currentcurve->second.pointset,currentcurve->second.pointnum,currentcurve->second.method);
                break;
            }
            default:
                break;
        }
    }
    penwidth=temppenwidth;
    setColor(tempcolor.red(),tempcolor.green(),tempcolor.blue()); //恢复原有画笔颜色
    isrepaint=false;
}

bool myCanvas::resetCanvas(int width,int height)
{
    this->width=width;
    this->height=height;
    QImage newboard(width,height,QImage::Format_RGB32);
    *paintingboard=newboard;
    QRgb background=qRgb(255,255,255);
    paintingboard->fill(background);
    repaintAll();
    update();
    return true;
}

bool myCanvas::resetCanvas_commandine(int width,int height)
{
    if(width<100||height<100||width>1000||height>1000)
        return false;
    this->width=width;
    this->height=height;
    QImage newboard(width,height,QImage::Format_RGB32);
    *paintingboard=newboard;
    QRgb background=qRgb(255,255,255);
    paintingboard->fill(background);
    update();
    graphset->clear();
    lineset->clear();
    polygonset->clear();
    ellipseset->clear();
    curveset->clear();
    return true;
}

bool myCanvas::setColor(int r,int g,int b)
{
    if(r<0||r>255)
        return false;
    if(g<0||g>255)
        return false;
    if(b<0||b>255)
        return false;
    color->setRgb(r,g,b);
    drawpen->setColor(*color);
    drawpen->setWidth(penwidth);
    return true;
}

bool myCanvas::saveToFile(string filepath)
{
    QString savefile(savepath);
    savefile+=QString::fromStdString(filepath);
    savefile+=".bmp";
    QImage tempboard=*paintingboard;
    bool saveok=tempboard.save(savefile);
    if(!saveok)
        qDebug() << "file: " << savefile << " failed to save";
    return saveok;
}

void myCanvas::paintSingleLine_DDA(int x0,int y0,int x1,int y1,QPainter *painter)
{
    if(x0==x1&&y0==y1)
    {
        painter->drawPoint(x0,y0);
        QWidget::update();
        return;
    }
    int length;
    if(abs(x1-x0)>=abs(y1-y0))
        length=abs(x1-x0);
    else
        length=abs(y1-y0);
    double deltax=(double)(x1-x0)/(double)length;
    double deltay=(double)(y1-y0)/(double)length;
    double x=(double)x0;
    double y=(double)y0;
    for(int i=0;i<=length;i++)
    {
        painter->drawPoint((int)round(x),(int)round(y));
        x+=deltax;
        y+=deltay;
    }
    QWidget::update();
}

void myCanvas::paintSingleLine_Bresenham(int x0,int y0,int x1,int y1,QPainter *painter)
{
    if(x0==x1&&y0==y1)
    {
        painter->drawPoint(x0,y0);
        QWidget::update();
        return;
    }
    int x,y,dx,dy,s1,s2;
    bool exchange;
    if(x1>x0)
        s1=1;
    else
        s1=-1;
    if(y1>y0)
        s2=1;
    else
        s2=-1;
    dx=abs(x1-x0);
    dy=abs(y1-y0);
    if(dy>dx)
    {
        exchange=true;
        int temp=dx;
        dx=dy;
        dy=temp;
    }
    else
        exchange=false;
    x=x0,y=y0;
    int p=2*dy-dx;
    for(int i=0;i<=dx;i++)
    {
        painter->drawPoint(x,y);
        if(p>=0)
        {
            x=x+s1;
            y=y+s2;
            p+=2*(dy-dx);
        }
        else
        {
            if(exchange)
                y=y+s2;
            else
                x=x+s1;
            p+=2*dy;
        }
    }
    QWidget::update();
}

bool myCanvas::drawLine(int id,int x0,int y0,int x1,int y1,drawingAlgorithm method)
{
    QPainter *painter=new QPainter(paintingboard);
    if(isrepaint)
    {
        painter->setPen(*drawpen);
        if(method==DDA)
            paintSingleLine_DDA(x0,y0,x1,y1,painter);
        else if(method==Bresenham)
            paintSingleLine_Bresenham(x0,y0,x1,y1,painter);
        delete painter;
        return true;
    }
    if(id<=0)
    {
        delete painter;
        return false;
    }
    map <int,myGraph>::iterator current;
    current=graphset->find(id);
    if(current!=graphset->end())
    {
        delete painter;
        return false;
    }
    myGraph graph;
    graph.id=id;
    graph.rgbcolor=*color;
    graph.shape=Line;
    graph.penwidth=penwidth;
    graphset->insert(make_pair(id,graph));
    myLine line;
    line.id=id;
    line.x0=x0;
    line.y0=y0;
    line.x1=x1;
    line.y1=y1;
    painter->setPen(*drawpen);
    if(method==DDA)
    {
        paintSingleLine_DDA(x0,y0,x1,y1,painter);
        line.method=DDA;
    }
    else if(method==Bresenham)
    {
        paintSingleLine_Bresenham(x0,y0,x1,y1,painter);
        line.method=Bresenham;
    }
    lineset->insert(make_pair(id,line));
    delete painter;
    return true;
}

bool myCanvas::drawPolygon(int id,int *pointset,int pointnum,drawingAlgorithm method)
{
    QPainter *painter=new QPainter(paintingboard);
    if(isrepaint)
    {
        painter->setPen(*drawpen);
        if(method==DDA)
        {
            for(int i=0;i<pointnum;i++)
                paintSingleLine_DDA(pointset[2*i],pointset[2*i+1],pointset[(2*(i+1))%(pointnum*2)],
pointset[(2*(i+1)+1)%(pointnum*2)],painter);
        }
        else if(method==Bresenham)
        {
            for(int i=0;i<pointnum;i++)
                paintSingleLine_Bresenham(pointset[2*i],pointset[2*i+1],pointset[(2*(i+1))%(pointnum*2)],
pointset[(2*(i+1)+1)%(pointnum*2)],painter);
        }
        delete painter;
        return true;
    }
    if(id<=0)
    {
        delete painter;
        return false;
    }
    map <int,myGraph>::iterator current;
    current=graphset->find(id);
    if(current!=graphset->end())
    {
        delete painter;
        return false;
    }
    myGraph graph;
    graph.id=id;
    graph.rgbcolor=*color;
    graph.shape=Polygon;
    graph.penwidth=penwidth;
    graphset->insert(make_pair(id,graph));
    myPolygon polygon;
    polygon.id=id;
    polygon.pointnum=pointnum;
    polygon.pointset=new int[2*pointnum];
    for(int i=0;i<2*pointnum;i++)
        polygon.pointset[i]=pointset[i];
    painter->setPen(*drawpen);
    if(method==DDA)
    {
        for(int i=0;i<pointnum;i++)
            paintSingleLine_DDA(pointset[2*i],pointset[2*i+1],pointset[(2*(i+1))%(pointnum*2)],
pointset[(2*(i+1)+1)%(pointnum*2)],painter);
        polygon.method=DDA;
    }
    else if(method==Bresenham)
    {
        for(int i=0;i<pointnum;i++)
            paintSingleLine_Bresenham(pointset[2*i],pointset[2*i+1],pointset[(2*(i+1))%(pointnum*2)],
pointset[(2*(i+1)+1)%(pointnum*2)],painter);
        polygon.method=Bresenham;
    }
    polygonset->insert(make_pair(id,polygon));
    delete painter;
    return true;
}

void myCanvas::drawEllipsePoint(int xc,int yc,int x,int y,QPainter *painter,TransMatrix *tmatrix)
{
    int x1=xc+x;
    int y1=yc+y;
    int x2=xc-x;
    int y2=yc+y;
    int x3=xc+x;
    int y3=yc-y;
    int x4=xc-x;
    int y4=yc-y;
    tmatrix->transCoordinate(x1,y1);
    tmatrix->transCoordinate(x2,y2);
    tmatrix->transCoordinate(x3,y3);
    tmatrix->transCoordinate(x4,y4);
    painter->drawPoint(x1,y1);
    painter->drawPoint(x2,y2);
    painter->drawPoint(x3,y3);
    painter->drawPoint(x4,y4);
}

bool myCanvas::drawEllipse(int id,int xc,int yc,int rx,int ry)
{
    if(!isrepaint)
    {
        if(id<=0)
            return false;
        map <int,myGraph>::iterator current;
        current=graphset->find(id);
        if(current!=graphset->end())
            return false;
        myGraph graph;
        graph.id=id;
        graph.rgbcolor=*color;
        graph.shape=Ellipse;
        graph.penwidth=penwidth;
        graphset->insert(make_pair(id,graph));
        myEllipse ellipse;
        ellipse.id=id;
        ellipse.rx=rx;
        ellipse.ry=ry;
        ellipse.xc=xc;
        ellipse.yc=yc;
        ellipseset->insert(make_pair(id,ellipse));
    }
    QPainter *painter;
    painter=new QPainter(paintingboard);
    painter->setPen(*drawpen);
    map <int,myEllipse>::iterator currentellipse;
    currentellipse=ellipseset->find(id);
    if(ry==0)
    {
        int x=0,y=0;
        for(;x<=rx;x++)
            drawEllipsePoint(xc,yc,x,y,painter,&currentellipse->second.ellipsematrix);
        QWidget::update();
        delete painter;
        return true;
    }
    int x=0,y=ry;
    double sqrx=(double)rx*rx;
    double sqry=(double)ry*ry;
    drawEllipsePoint(xc,yc,x,y,painter,&currentellipse->second.ellipsematrix);
    double p=sqry-sqrx*ry+sqrx/(double)4; //p1k的初始值
    while(sqry*x<sqrx*y) //在区域1内
    {
        if(p<0)
        {
            p=p+2*sqry*x+3*sqry;
            x++;
        }
        else
        {
            p=p+2*sqry*x-2*sqrx*y+2*sqrx+3*sqry;
            x++;
            y--;
        }
        drawEllipsePoint(xc,yc,x,y,painter,&currentellipse->second.ellipsematrix);
    }
    p=sqry*(x+0.5)*(x+0.5)+sqrx*(y-1)*(y-1)-sqrx*sqry; //根据区域1计算的最后位置的像素点坐标初始化p2k
    while(y>0) //在区域2内
    {
        if(p<=0)
        {
            p=p+2*sqry*x-2*sqrx*y+2*sqry+3*sqrx;
            x++;
            y--;
        }
        else
        {
            p=p-2*sqrx*y+3*sqrx;
            y--;
        }
        drawEllipsePoint(xc,yc,x,y,painter,&currentellipse->second.ellipsematrix);
    }
    QWidget::update();
    delete painter;
    return true;
}

bool myCanvas::translate(int id,int dx,int dy)
{
    map <int,myGraph>::iterator current;
    current=graphset->find(id);
    if(current==graphset->end())
        return false;
    if(current->second.shape==Line)
    {
        map <int,myLine>::iterator currentline;
        currentline=lineset->find(id);
        currentline->second.x0+=dx;
        currentline->second.y0+=dy;
        currentline->second.x1+=dx;
        currentline->second.y1+=dy;
    }
    else if(current->second.shape==Polygon)
    {
        map <int,myPolygon>::iterator currentpolygon;
        currentpolygon=polygonset->find(id);
        int pointnum=currentpolygon->second.pointnum;
        for(int i=0;i<pointnum;i++) //对多边形点集中的每一个坐标点进行平移
        {
            currentpolygon->second.pointset[2*i]+=dx; //x轴坐标
            currentpolygon->second.pointset[2*i+1]+=dy; //y轴坐标
        }
    }
    else if(current->second.shape==Ellipse)
    {
        map <int,myEllipse>::iterator currentellipse;
        currentellipse=ellipseset->find(id);
        currentellipse->second.ellipsematrix.translateEllipse(currentellipse->second.xc,currentellipse->second.yc,dx,dy);
        currentellipse->second.xc+=dx;
        currentellipse->second.yc+=dy;
    }
    else if(current->second.shape==Curve)
    {
        map <int,myCurve>::iterator currentcurve;
        currentcurve=curveset->find(id);
        int n=currentcurve->second.pointnum;
        for(int i=0;i<n;i++)
        {
            int x=currentcurve->second.pointset[i].x()+dx;
            int y=currentcurve->second.pointset[i].y()+dy;
            currentcurve->second.pointset[i].setX(x);
            currentcurve->second.pointset[i].setY(y);
        }
    }
    resetCanvas(width,height);
    return true;
}

bool myCanvas::rotate(int id,int x,int y,int r)
{
    map <int,myGraph>::iterator current;
    current=graphset->find(id);
    if(current==graphset->end())
        return false;
    if(current->second.shape==Line)
    {
        map <int,myLine>::iterator currentline;
        currentline=lineset->find(id);
        int x0=currentline->second.x0;
        int y0=currentline->second.y0;
        int x1=currentline->second.x1;
        int y1=currentline->second.y1;
        currentline->second.linematrix.rotateLine(r,x,y,x0,y0,x1,y1);
        currentline->second.x0=x0;
        currentline->second.y0=y0;
        currentline->second.x1=x1;
        currentline->second.y1=y1;
    }
    else if(current->second.shape==Polygon)
    {
        map <int,myPolygon>::iterator currentpolygon;
        currentpolygon=polygonset->find(id);
        int tempnum=currentpolygon->second.pointnum;
        int *tempvertex=new int[tempnum*2];
        for(int i=0;i<tempnum*2;i++)
            tempvertex[i]=currentpolygon->second.pointset[i];
        currentpolygon->second.polygonmatrix.rotatePolygon(r,x,y,tempvertex,tempnum);
        for(int i=0;i<tempnum*2;i++)
            currentpolygon->second.pointset[i]=tempvertex[i];
        delete []tempvertex;
    }
    else if(current->second.shape==Ellipse)
    {
        map <int,myEllipse>::iterator currentellipse;
        currentellipse=ellipseset->find(id);
        int xc=currentellipse->second.xc;
        int yc=currentellipse->second.yc;
        currentellipse->second.ellipsematrix.rotateEllipse(r,x,y,xc,yc);
        currentellipse->second.xc=xc;
        currentellipse->second.yc=yc;
    }
    else if(current->second.shape==Curve)
    {
        map <int,myCurve>::iterator currentcurve;
        currentcurve=curveset->find(id);
        currentcurve->second.curvematrix.rotateCurve(r,x,y,currentcurve->second.pointset,currentcurve->second.pointnum);
    }
    resetCanvas(width,height);
    return true;
}

bool myCanvas::scale(int id,int x,int y,double r)
{
    map <int,myGraph>::iterator current;
    current=graphset->find(id);
    if(current==graphset->end())
        return false;
    if(current->second.shape==Line)
    {
        map <int,myLine>::iterator currentline;
        currentline=lineset->find(id);
        int x0=currentline->second.x0;
        int y0=currentline->second.y0;
        int x1=currentline->second.x1;
        int y1=currentline->second.y1;
        currentline->second.linematrix.scaleLine(r,x,y,x0,y0,x1,y1);
        currentline->second.x0=x0;
        currentline->second.y0=y0;
        currentline->second.x1=x1;
        currentline->second.y1=y1;
    }
    else if(current->second.shape==Polygon)
    {
        map <int,myPolygon>::iterator currentpolygon;
        currentpolygon=polygonset->find(id);
        int tempnum=currentpolygon->second.pointnum;
        int *tempvertex=new int[tempnum*2];
        for(int i=0;i<tempnum*2;i++)
            tempvertex[i]=currentpolygon->second.pointset[i];
        currentpolygon->second.polygonmatrix.scalePolygon(r,x,y,tempvertex,tempnum);
        for(int i=0;i<tempnum*2;i++)
            currentpolygon->second.pointset[i]=tempvertex[i];
        delete []tempvertex;
    }
    else if(current->second.shape==Ellipse)
    {
        map <int,myEllipse>::iterator currentellipse;
        currentellipse=ellipseset->find(id);
        int xc=currentellipse->second.xc;
        int yc=currentellipse->second.yc;
        int rx=currentellipse->second.rx;
        int ry=currentellipse->second.ry;
        currentellipse->second.ellipsematrix.scaleEllipse(r,x,y,xc,yc,rx,ry);
        currentellipse->second.xc=xc;
        currentellipse->second.yc=yc;
        currentellipse->second.rx=rx;
        currentellipse->second.ry=ry;
    }
    else if(current->second.shape==Curve)
    {
        map <int,myCurve>::iterator currentcurve;
        currentcurve=curveset->find(id);
        int pointnum=currentcurve->second.pointnum;
        currentcurve->second.curvematrix.scaleCurve(r,x,y,pointnum,currentcurve->second.pointset);
    }
    resetCanvas(width,height);
    return true;
}

void myCanvas::swapNum(int &a,int &b)
{
    int temp=a;
    a=b;
    b=temp;
}

void myCanvas::swapNum(uint8_t &a,uint8_t &b)
{
    uint8_t temp=a;
    a=b;
    b=temp;
}

void myCanvas::swapNum(double &a,double &b)
{
    double temp=a;
    a=b;
    b=temp;
}

void myCanvas::encode_CohenSutherland(int x,int y,clipWindow *clipwindow,uint8_t &code)
{
    code=0;
    if(x<clipwindow->xmin)
        code+=1;
    if(x>clipwindow->xmax)
        code+=2;
    if(y<clipwindow->ymin)
        code+=4;
    if(y>clipwindow->ymax)
        code+=8;
}

bool myCanvas::clipLine_CohenSutherland(int &x0,int &y0,int &x1,int &y1,clipWindow *clipwindow)
{
    uint8_t c0=0,c1=0;
    clipWindowCode windowcode; //存放四个方向的编码
    while(true)
    {
        encode_CohenSutherland(x0,y0,clipwindow,c0); //对端点进行编码，clipwindow中存放窗口的边界信息
        encode_CohenSutherland(x1,y1,clipwindow,c1);
        if((c0&c1)!=0) //直线完全在窗口外，直接舍弃
            return false;
        if((c0|c1)==0) //直线完全在窗口内，直接保留
            return true;
        if(c0==0) //判断第一个端点是否在窗口内，如果在，那么交换第一个端点和第二个端点的信息
        {
            swapNum(x0,x1);
            swapNum(y0,y1);
            swapNum(c0,c1);
        }
        double deltax=(double)(x1-x0);
        double deltay=(double)(y1-y0);
        if((c0&windowcode.top)!=0) //第一个端点在窗口上方，windowcode.top=二进制1000
        {
            x0=(int)round(x0+deltax*(double)(clipwindow->ymax-y0)/deltay); //更新第一个端点
            y0=clipwindow->ymax;
            continue; //直接进入下一次循环
        }
        if((c0&windowcode.down)!=0) //第一个端点在窗口下方，windowcode.top=二进制0100
        {
            x0=(int)round(x0+deltax*(double)(clipwindow->ymin-y0)/deltay);
            y0=clipwindow->ymin;
            continue;
        }
        if((c0&windowcode.right)!=0) //第一个端点在窗口右方，windowcode.top=二进制0010
        {
            y0=(int)round(y0+deltay*(double)(clipwindow->xmax-x0)/deltax);
            x0=clipwindow->xmax;
            continue;
        }
        if((c0&windowcode.left)!=0) //第一个端点在窗口左方，windowcode.top=二进制0001
        {
            y0=(int)round(y0+deltay*(double)(clipwindow->xmin-x0)/deltax);
            x0=clipwindow->xmin;
            continue;
        }
    }
    return true;
}

bool myCanvas::clipLine_LiangBarsky(int &x0,int &y0,int &x1,int &y1,clipWindow *clipwindow)
{
    int p1=x0-x1,q1=x0-clipwindow->xmin;
    int p2=x1-x0,q2=clipwindow->xmax-x0;
    int p3=y0-y1,q3=y0-clipwindow->ymin;
    int p4=y1-y0,q4=clipwindow->ymax-y0;
    if(p1==0)
    {
        if(q1<0||q2<0)
            return false;
        else
        {
            int ymin=max(clipwindow->ymin,min(y0,y1));
            int ymax=min(clipwindow->ymax,max(y0,y1));
            if(ymin>ymax)
                return false;
            else
            {
                y0=ymin;
                y1=ymax;
                return true;
            }
        }
    }
    else if(p3==0)
    {
        if(q3<0||q4<0)
            return false;
        else
        {
            int xmin=max(clipwindow->xmin,min(x0,x1));
            int xmax=min(clipwindow->xmax,max(x0,x1));
            if(xmin>xmax)
                return false;
            else
            {
                x0=xmin;
                x1=xmax;
                return true;
            }
        }
    }
    else
    {
        double u1=(double)q1/(double)p1;
        double u2=(double)q2/(double)p2;
        double u3=(double)q3/(double)p3;
        double u4=(double)q4/(double)p4;
        double umin,umax;
        if(p1<0)
        {
            if(p3<0)
            {
                umin=max((double)0,max(u1,u3));
                umax=min((double)1,min(u2,u4));
            }
            else
            {
                umin=max((double)0,max(u1,u4));
                umax=min((double)1,min(u2,u3));
            }
        }
        else
        {
            if(p3<0)
            {
                umin=max((double)0,max(u2,u3));
                umax=min((double)1,min(u1,u4));
            }
            else
            {
                umin=max((double)0,max(u2,u4));
                umax=min((double)1,min(u1,u3));
            }
        }
        if(umin>umax)
            return false;
        else
        {
            int xbase=x0,ybase=y0;
            x0=xbase+(int)round(umin*(double)p2);
            x1=xbase+(int)round(umax*(double)p2);
            y0=ybase+(int)round(umin*(double)p4);
            y1=ybase+(int)round(umax*(double)p4);
            return true;
        }
    }
}

bool myCanvas::clip(int id,clipWindow *clipwindow,drawingAlgorithm method)
{
    map <int,myGraph>::iterator current;
    current=graphset->find(id);
    if(current==graphset->end())
        return false;
    if(current->second.shape!=Line)
        return false;
    map <int,myLine>::iterator currentline;
    currentline=lineset->find(id);
    int x0=currentline->second.x0;
    int y0=currentline->second.y0;
    int x1=currentline->second.x1;
    int y1=currentline->second.y1;
    bool remain=true;
    if(method==CohenSutherland)
        remain=clipLine_CohenSutherland(x0,y0,x1,y1,clipwindow);
    else if(method==LiangBarsky)
        remain=clipLine_LiangBarsky(x0,y0,x1,y1,clipwindow);
    if(remain)
    {
        currentline->second.x0=x0;
        currentline->second.y0=y0;
        currentline->second.x1=x1;
        currentline->second.y1=y1;
    }
    else
    {
        graphset->erase(currentline->first);
        lineset->erase(currentline);
    }
    resetCanvas(width,height);
    return true;
}

double myCanvas::minStep_curve(QVector <QPoint> &set,int n,drawingAlgorithm method)
{
    double maxdistance=0;
    for(int i=0;i<n;i++)
    {
        double dx=(double)(set[i].x()-set[i+1].x());
        double dy=(double)(set[i].y()-set[i+1].y());
        double dx2=dx*dx;
        double dy2=dy*dy;
        double distance=sqrt(dx2+dy2);
        if(distance>maxdistance)
            maxdistance=distance;
    }
    if(method==Bezier)
        return 0.25/maxdistance;
    else
        return 0.5/maxdistance;
}

void myCanvas::drawCurve_Bezier(QVector <QPoint> &set,int n,QPainter *painter)
{

    doublePoint **p;
    p=new doublePoint*[n+1];
    for(int i=0;i<=n;i++)
        p[i]=new doublePoint[n+1];

    double du=minStep_curve(set,n,Bezier);

    for(double u=0;u<=1;u=u+du)
    {
        for(int j=0;j<=n;j++)
        {
            p[0][j].x=(double)set[j].x();
            p[0][j].y=(double)set[j].y();
        }

        for(int i=1;i<=n;i++)
        {
            for(int j=0;j<=n-i;j++)
            {
                p[i][j].x=(1-u)*p[i-1][j].x+u*p[i-1][j+1].x;
                p[i][j].y=(1-u)*p[i-1][j].y+u*p[i-1][j+1].y;
            }
        }

        int x=(int)round(p[n][0].x);
        int y=(int)round(p[n][0].y);

        painter->drawPoint(x,y);
    }

    painter->drawPoint(set[n].x(),set[n].y());

    for(int i=0;i<=n;i++)
        delete []p[i];
    delete []p;
}

void myCanvas::drawCurve_Bspline(QVector <QPoint> &set,int n,QPainter *painter)
{
    int k=0;
    if(n>=3)
        k=3;
    else
        k=n;
    QVector <QPoint> controlset;
    doublePoint **p;
    p=new doublePoint*[k+1];
    for(int i=0;i<=k;i++)
        p[i]=new doublePoint[n+1];
    for(int i=0;i<n-k+1;i++)
    {
        controlset.clear();
        for(int j=0;j<=k;j++)
        {
            controlset.push_back(set[i+j]);
            p[0][i+j].x=(double)set[i+j].x();
            p[0][i+j].y=(double)set[i+j].y();
        }
        double du=minStep_curve(controlset,k,Bspline);
        for(double u=(double)i+k;u<(double)i+k+1+du;u=u+du)
        {
            for(int r=1;r<=k;r++)
            {
                for(int j=i+r;j<=i+k;j++)
                {
                    double lamda_u=(u-j)/(k+1-r);
                    p[r][j].x=lamda_u*p[r-1][j].x+(1-lamda_u)*p[r-1][j-1].x;
                    p[r][j].y=lamda_u*p[r-1][j].y+(1-lamda_u)*p[r-1][j-1].y;
                }
            }
            int x=(int)round(p[k][i+k].x);
            int y=(int)round(p[k][i+k].y);
            painter->drawPoint(x,y);
        }
    }
    for(int i=0;i<=k;i++)
        delete []p[i];
    delete []p;
}

bool myCanvas::drawCurve(int id,QVector <QPoint> &pointset,int pointnum,drawingAlgorithm method)
{
    if(!isrepaint)
    {
        if(id<=0)
            return false;
        map <int,myGraph>::iterator current;
        current=graphset->find(id);
        if(current!=graphset->end())
            return false;
        myGraph graph;
        graph.id=id;
        graph.rgbcolor=*color;
        graph.shape=Curve;
        graph.penwidth=penwidth;
        graphset->insert(make_pair(id,graph));
        myCurve curve;
        curve.id=id;
        curve.method=method;
        curve.pointnum=pointnum;
        for(int i=0;i<pointnum;i++)
            curve.pointset.push_back(pointset[i]);
        curveset->insert(make_pair(id,curve));
    }
    QPainter *painter;
    painter=new QPainter(paintingboard);
    painter->setPen(*drawpen);
    if(method==Bezier)
        drawCurve_Bezier(pointset,pointnum-1,painter);
    else if(method==Bspline)
        drawCurve_Bspline(pointset,pointnum-1,painter);
    delete painter;
    return true;
}
