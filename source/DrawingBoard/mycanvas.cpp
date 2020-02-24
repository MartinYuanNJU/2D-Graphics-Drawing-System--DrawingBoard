#include "mycanvas.h"

myCanvas::myCanvas(QWidget *parent):QWidget(parent)
{
    graphset=new map<int,myGraph>;
    lineset=new map<int,myLine>;
    polygonset=new map<int,myPolygon>;
    ellipseset=new map<int,myEllipse>;
    roundset=new map<int,myRound>;
    curveset=new map<int,myCurve>;

    isrepaint=false;
    width=maxwidth;
    height=maxheight;

    linealgorithm=Bresenham;
    polygonalgorithm=Bresenham;
    clipalgorithm=LiangBarsky;
    curvealgorithm=Bezier;

    draw_on_real_board=true;
    drawboard=paintBoard;

    iscreatingshape=false;
    createshape=NoneShape;

    polygonpointnum=0;
    ispolygonfirst=true;

    curvepointnum=0;
    curvelinedrawed=false;

    iseditingshape=false;
    editshape=NoneShape;
    editaction=NoneAction;
    cliplinewindow.xmin=cliplinewindow.xmax=cliplinewindow.ymin=cliplinewindow.ymax=0;

    paintingboard=new QImage(width,height,QImage::Format_RGB32);
    QRgb background=qRgb(255,255,255);
    paintingboard->fill(background);

    color=new QColor;
    color->setRgb(0,0,0);
    drawpen=new QPen;
    drawpen->setColor(*color);
    penwidth=2;
    drawpen->setWidth(penwidth);
    drawpen->setStyle(Qt::SolidLine);
    drawpen->setCapStyle(Qt::RoundCap);
    drawpen->setJoinStyle(Qt::RoundJoin);

    fillcolor=new QColor;
    fillcolor->setRgb(0,0,0);
    fillpen=new QPen;
    fillpen->setColor(*fillcolor);
    fillpen->setWidth(1);
    fillpen->setStyle(Qt::SolidLine);
    fillpen->setCapStyle(Qt::RoundCap);
    fillpen->setJoinStyle(Qt::RoundJoin);

    pointpen=new QPen;
    /*pointpen->setColor(QColor(0,128,255));
    pointpen->setWidth(5);*/
    pointpen->setColor(QColor(0,0,0));
    pointpen->setWidth(1);
    pointpen->setStyle(Qt::SolidLine);
    pointpen->setCapStyle(Qt::SquareCap);

    solidpointpen.setColor(QColor(255,255,255));
    solidpointpen.setWidth(3);
    solidpointpen.setStyle(Qt::SolidLine);
    solidpointpen.setCapStyle(Qt::SquareCap);

    clipwindowpen=new QPen;
    clipwindowpen->setColor(QColor(255,0,0));
    clipwindowpen->setWidth(1);
    clipwindowpen->setStyle(Qt::DashLine);
    clipwindowpen->setCapStyle(Qt::RoundCap);
    clipwindowpen->setJoinStyle(Qt::RoundJoin);

    disclipen=new QPen;
    disclipen->setColor(QColor(0,128,255));
    //disclipen->setColor(QColor(154,154,154));
    disclipen->setWidth(1);
    disclipen->setStyle(Qt::DashLine);
    disclipen->setCapStyle(Qt::RoundCap);
    disclipen->setJoinStyle(Qt::RoundJoin);

    nowfilename.clear();
    hasedited=false;
    //suffix=BMP;

    direction=NoneDirection;
    iseditingcanvas=false;
    tempwidth=width;
    tempheight=height;

    selectid=0;
    editid=0;

    drawoutline=true;
    onselectgraph=false;
    centerdrawed=false;
    firstpointdrawed=false;

    centerpen=new QPen;
    centerpen->setColor(QColor(255,0,0));
    centerpen->setWidth(4);
    centerpen->setStyle(Qt::SolidLine);
    centerpen->setCapStyle(Qt::RoundCap);

    oncontrolpoint=false;
    controlpoint=-1;

    prevpolygon.pointset=NULL;

    QWidget::setMouseTracking(true);
}

void myCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    if(draw_on_real_board)
        painter.drawImage(0,0,*paintingboard);
    else
        painter.drawImage(0,0,temppaintboard);
}

bool myCanvas::closeEnough_point(QPoint &point,int xstandard,int ystandard,int threshold,bool strict)
{
    int x=point.x();
    int y=point.y();
    if(strict)
    {
        int x2=(xstandard-x)*(xstandard-x);
        int y2=(ystandard-y)*(ystandard-y);
        if(x2+y2<=threshold)
            return true;
        else
            return false;
    }
    else
    {
        if((fabs(xstandard-x)<=threshold) && (fabs(ystandard-y)<=threshold))
            return true;
        else
            return false;
    }
}

void myCanvas::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton) //按下的是左键
    {
        if(direction!=NoneDirection)
        {
            hasedited=true;
            iseditingcanvas=true;
            draw_on_real_board=false;
            drawboard=tempBoard;
            QPoint presspoint=event->pos();
            pointvector.push_back(presspoint);
        }
        else if(editaction==Create) //当前操作时创建图元
        {
            hasedited=true;
            drawboard=tempBoard;
            iscreatingshape=true; //正在绘制
            draw_on_real_board=false; //指示重绘事件函数之后将把临时画布画在屏幕上
            if(createshape==Line) //创建的图元是直线
            {
                QPoint presspoint=event->pos(); //获取按下的点的位置作为直线起始点
                pointvector.push_back(presspoint); //将起始点放入临时容器中，该容器用来存放正在绘制的图元的控制点
                temppaintboard=*paintingboard; //将临时画布设置与正式画布一样的图样
                QPainter *painter=setPainter(pointpen);
                paintPoint(presspoint.x(),presspoint.y(),painter,solidpointpen);
                delete painter;
            }
            else if(createshape==Polygon)
            {
                QPoint presspoint=event->pos();
                if(ispolygonfirst) //按下的点是多边形第一个点
                {
                    ispolygonfirst=false; //以后再按下鼠标就不是多边形第一个点
                    bufferboard=*paintingboard; //二重临时画布初始化为正式画布内容
                    temppaintboard=bufferboard; //临时画布初始化为二重临时画布内容
                    pointvector.push_back(presspoint); //将第一个点放入容器
                    polygonpointnum++; //多边形点数加一
                    QPainter *painter=setPainter(pointpen);
                    paintPoint(presspoint.x(),presspoint.y(),painter,solidpointpen);
                    delete painter;
                }
                else //按下的是中间点
                {
                    temppaintboard=bufferboard; //临时画布初始化为已确认点绘制出的二重临时画布内容
                    drawtempLine_ui(polygonpointnum-1,presspoint,true,drawpen); //绘制最近一次确认的点到当前按下的点的动态直线
                    if(polygonpointnum>1)
                        drawtempLine_ui(0,presspoint,false,disclipen); //绘制以当前点为终点到起点的虚线
                }
            }
            else if(createshape==Ellipse)
            {
                QPoint presspoint=event->pos();
                pointvector.push_back(presspoint);
                temppaintboard=*paintingboard;
            }
            else if(createshape==Round)
            {
                QPoint presspoint=event->pos();
                pointvector.push_back(presspoint);
                temppaintboard=*paintingboard;
            }
            else if(createshape==Curve)
            {
                QPoint presspoint=event->pos();
                temppaintboard=*paintingboard;
                if(!curvelinedrawed)
                {
                    pointvector.push_back(presspoint);
                    curvepointnum++;
                }
                else
                    drawtempCurve_ui(presspoint);
            }
        }
        else if(editaction==Clip&&editshape==Line)
        {
            hasedited=true;
            drawboard=tempBoard;
            iseditingshape=true;
            QPoint presspoint=event->pos();
            cliplinewindow.xmin=presspoint.x();
            cliplinewindow.ymin=presspoint.y();
            temppaintboard=*paintingboard;
            draw_on_real_board=false;
        }
        else if(editaction==Select)
        {
            QPoint presspoint=event->pos();
            if(oncontrolpoint)
            {
                hasedited=true;
                drawboard=tempBoard;
                draw_on_real_board=false;
                lastpoint=presspoint;
                iseditingshape=true;
            }
            else if(onselectgraph)
            {
                hasedited=true;
                drawboard=tempBoard;
                draw_on_real_board=false;
                drawoutline=false;
                lastpoint=presspoint;
                iseditingshape=true;
                resetCanvas(width,height,true);
            }
            else
            {
                selectGraph(presspoint);
            }
        }
        else if(editaction==Rotate&&selectid!=0)
        {
            QPoint presspoint=event->pos();
            if(centerdrawed)
            {
                setCursor(Qt::ClosedHandCursor);
                if(presspoint.x()!=centerpoint.x()||presspoint.y()!=centerpoint.y())
                {
                    hasedited=true;
                    lastpoint=presspoint;
                    firstpointdrawed=true;
                    iseditingshape=true;
                    drawoutline=false;
                    lastangle=0;
                    rotate_ui(presspoint,true,false);
                }
            }
            else
            {
                drawboard=tempBoard;
                temppaintboard=*paintingboard;
                draw_on_real_board=false;
                centerdrawed=true;
                iseditingshape=true;
                centerpoint=presspoint;
                closeEnough_selectcenter(presspoint);
                paintCenterPoint();
            }
        }
        else if(editaction==Scale&&selectid!=0)
        {
            QPoint presspoint=event->pos();
            if(centerdrawed)
            {
                setCursor(Qt::ClosedHandCursor);
                if(presspoint.x()!=centerpoint.x()||presspoint.y()!=centerpoint.y())
                {
                    hasedited=true;
                    lastpoint=presspoint;
                    firstpointdrawed=true;
                    iseditingshape=true;
                    drawoutline=false;
                    scale_ui(presspoint,true,false);
                }
            }
            else
            {
                drawboard=tempBoard;
                temppaintboard=*paintingboard;
                draw_on_real_board=false;
                centerdrawed=true;
                iseditingshape=true;
                centerpoint=presspoint;
                closeEnough_selectcenter(presspoint);
                paintCenterPoint();
            }
        }
    }
    else if(event->button()==Qt::RightButton)
    {
        if(editaction==Create&&iscreatingshape)
        {
            iscreatingshape=false;
            draw_on_real_board=true;
            drawboard=paintBoard;
            if(createshape==Polygon)
            {
                createPolygon_ui();
            }
            else if(createshape==Curve)
            {
                createCurve_ui();
            }
        }
        else if((editaction==Rotate||editaction==Scale)&&centerdrawed)
        {
            draw_on_real_board=true;
            drawboard=paintBoard;
            iseditingshape=false;
            centerdrawed=false;
            firstpointdrawed=false;
            resetCanvas(width,height,true);
        }
    }
}

void myCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(editaction==Create&&iscreatingshape)
        {
            if(createshape==Line)
            {
                drawboard=paintBoard;
                iscreatingshape=false;
                draw_on_real_board=true;
                QPoint releasepoint=event->pos();
                pointvector.push_back(releasepoint);
                createLine_ui();
            }
            else if(createshape==Polygon)
            {
                QPoint releasepoint=event->pos();
                QPoint lastpoint=pointvector[polygonpointnum-1];
                if(releasepoint.x()!=lastpoint.x()||releasepoint.y()!=lastpoint.y()) //松开位置的点与最后一个确认的点不重合
                {
                    drawboard=bufferBoard;
                    drawtempLine_ui(polygonpointnum-1,releasepoint,true,drawpen);
                    drawboard=tempBoard;
                    pointvector.push_back(releasepoint);
                    polygonpointnum++;
                }
                temppaintboard=bufferboard;
                QWidget::update();
            }
            else if(createshape==Ellipse)
            {
                drawboard=paintBoard;
                iscreatingshape=false;
                draw_on_real_board=true;
                QPoint releasepoint=event->pos();
                pointvector.push_back(releasepoint);
                createEllipse_ui();
            }
            else if(createshape==Round)
            {
                drawboard=paintBoard;
                iscreatingshape=false;
                draw_on_real_board=true;
                QPoint releasepoint=event->pos();
                pointvector.push_back(releasepoint);
                createRound_ui();
            }
            else if(createshape==Curve)
            {
                QPoint releasepoint=event->pos();
                if(!curvelinedrawed)
                {
                    curvelinedrawed=true;
                    pointvector.push_back(releasepoint);
                    curvepointnum++;
                }
                else
                {
                    if(curvealgorithm==Bezier)
                    {
                        QPoint lastpoint=pointvector[curvepointnum-1];
                        pointvector.pop_back();
                        pointvector.push_back(releasepoint);
                        pointvector.push_back(lastpoint);
                    }
                    else
                    {
                        pointvector.push_back(releasepoint);
                    }
                    curvepointnum++;
                }
            }
        }
        else if(editaction==Clip&&editshape==Line&&iseditingshape)
        {
            drawboard=paintBoard;
            iseditingshape=false;
            draw_on_real_board=true;
            QPoint releasepoint=event->pos();
            cliplinewindow.xmax=releasepoint.x();
            cliplinewindow.ymax=releasepoint.y();
            clipLine_ui();
        }
        else if(iseditingcanvas && direction!=NoneDirection)
        {
            drawboard=paintBoard;
            iseditingcanvas=false;
            draw_on_real_board=true;
            QPoint releasepoint=event->pos();
            editCanvas(releasepoint,direction,true);
            pointvector.clear();
        }
        else if(editaction==Select)
        {
            if(oncontrolpoint)
            {
                drawboard=paintBoard;
                draw_on_real_board=true;
                iseditingshape=false;
                QPoint releasepoint=event->pos();
                editGraph_ui(releasepoint);
            }
            else if(onselectgraph)
            {
                drawboard=paintBoard;
                draw_on_real_board=true;
                drawoutline=true;
                iseditingshape=false;
                QPoint releasepoint=event->pos();
                translate_ui(releasepoint);
            }
        }
        else if(editaction==Rotate&&centerdrawed&&firstpointdrawed)
        {
            setCursor(Qt::OpenHandCursor);
            drawboard=paintBoard;
            iseditingcanvas=false;
            draw_on_real_board=true;
            drawoutline=true;
            iseditingshape=false;
            centerdrawed=false;
            firstpointdrawed=false;
            editaction=Select;
            QPoint releasepoint=event->pos();
            rotate_ui(releasepoint,false,true);
        }
        else if(editaction==Scale&&centerdrawed&&firstpointdrawed)
        {
            setCursor(Qt::OpenHandCursor);
            drawboard=paintBoard;
            iseditingcanvas=false;
            draw_on_real_board=true;
            drawoutline=true;
            iseditingshape=false;
            centerdrawed=false;
            firstpointdrawed=false;
            editaction=Select;
            QPoint releasepoint=event->pos();
            scale_ui(releasepoint,false,true);
        }
    }
}

void myCanvas::mouseMoveEvent(QMouseEvent *event)
{
    statusinfo=tr("鼠标位置: (");
    QPoint infopoint=event->pos();
    statusinfo+=QString::number(infopoint.x());
    statusinfo+=',';
    statusinfo+=QString::number(infopoint.y());
    statusinfo+=')';
    emit sendStatusbarinfo(statusinfo);
    if(event->buttons()&Qt::LeftButton)
    {
        if(editaction==Create&&iscreatingshape)
        {
            if(createshape==Line)
            {
               temppaintboard=*paintingboard;
               QPoint nowpoint=event->pos();
               drawtempLine_ui(0,nowpoint,true,drawpen);
            }
            else if(createshape==Polygon)
            {
                QPoint nowpoint=event->pos();
                temppaintboard=bufferboard; //临时画布初始化为已确认点绘制出的二重临时画布内容
                drawtempLine_ui(polygonpointnum-1,nowpoint,true,drawpen); //绘制最近一次确认的点到当前移动到的的点的动态直线
                if(polygonpointnum>1)
                    drawtempLine_ui(0,nowpoint,false,disclipen); //绘制以当前点为终点到起点的虚线
            }
            else if(createshape==Ellipse)
            {
                temppaintboard=*paintingboard;
                QPoint nowpoint=event->pos();
                drawtempEllipse_ui(nowpoint);
            }
            else if(createshape==Round)
            {
                temppaintboard=*paintingboard;
                QPoint nowpoint=event->pos();
                drawtempRound_ui(nowpoint);
            }
            else if(createshape==Curve)
            {
                temppaintboard=*paintingboard;
                QPoint nowpoint=event->pos();
                if(!curvelinedrawed)
                    drawtempLine_ui(0,nowpoint,true,drawpen);
                else
                    drawtempCurve_ui(nowpoint);
            }
        }
        else if(editaction==Clip&&editshape==Line&&iseditingshape)
        {
            temppaintboard=*paintingboard;
            QPoint nowpoint=event->pos();
            drawtempClipWindow_ui(nowpoint);
        }
        else if(iseditingcanvas && direction!=NoneDirection)
        {
            QPoint nowpoint=event->pos();
            editCanvas(nowpoint,direction,false);
        }
        else if(editaction==Select)
        {
            QPoint nowpoint=event->pos();
            if(oncontrolpoint)
            {
                editGraph_ui(nowpoint);
            }
            else if(onselectgraph)
            {
                translate_ui(nowpoint);
                lastpoint=nowpoint;
            }
        }
        else if(editaction==Rotate&&centerdrawed&&firstpointdrawed)
        {
            setCursor(Qt::ClosedHandCursor);
            QPoint nowpoint=event->pos();
            rotate_ui(nowpoint,false,false);
        }
        else if(editaction==Scale&&centerdrawed&&firstpointdrawed)
        {
            setCursor(Qt::ClosedHandCursor);
            QPoint nowpoint=event->pos();
            scale_ui(nowpoint,false,false);
        }
    }
    else if(event->buttons()==Qt::NoButton)
    {
        if(iseditingshape && (editaction==Rotate||editaction==Scale) && selectid!=0 && centerdrawed)
        {
            setCursor(Qt::OpenHandCursor);
        }
        else if(!iscreatingshape && !iseditingshape)
        {
            QPoint point=event->pos();
            if(closeEnough_point(point,width,height,5,false))
            {
                setCursor(Qt::SizeFDiagCursor);
                direction=Diagonal;
            }
            else if(fabs(point.y()-height)<=5 && point.x()>5) //与底端边框足够近
            {
                setCursor(Qt::SizeVerCursor);
                direction=Vertical;
            }
            else if(fabs(point.x()-width)<=5 && point.y()>5) //与右边边框足够近
            {
                setCursor(Qt::SizeHorCursor);
                direction=Horizontal;
            }
            else if(editaction==Create||editaction==Clip)
            {
                setCursor(Qt::CrossCursor);
                direction=NoneDirection;
            }
            else if(editaction==Select)
            {
                if(selectid!=0)
                {
                    if(selectGraphPoint(point))
                    {
                        setCursor(Qt::SizeVerCursor);
                        oncontrolpoint=true;
                        onselectgraph=false;
                    }
                    else if(closeEnough_graph(selectid,point))
                    {
                        setCursor(Qt::SizeAllCursor);
                        onselectgraph=true;
                        oncontrolpoint=false;
                    }
                    else
                    {
                        setCursor(Qt::ArrowCursor);
                        oncontrolpoint=false;
                        onselectgraph=false;
                    }
                }
                else
                {
                    setCursor(Qt::ArrowCursor);
                    oncontrolpoint=false;
                    onselectgraph=false;
                }
                direction=NoneDirection;
            }
            else
            {
                setCursor(Qt::ArrowCursor);
                direction=NoneDirection;
            }
        }
        else if(direction != NoneDirection)
        {
            setCursor(Qt::ArrowCursor);
            direction=NoneDirection;
        }
    }
}

void myCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    /*if(editaction==Create&&iscreatingshape)
    {
        if(createshape==Polygon)
        {
            drawboard=paintBoard;
            iscreatingshape=false;
            draw_on_real_board=true;
            createPolygon_ui();
        }
    }*/
}

int myCanvas::getWidth()
{
    return tempwidth;
}

int myCanvas::getHeight()
{
    return tempheight;
}

int myCanvas::getPenwidth()
{
    return penwidth;
}

void myCanvas::loadSavePath(string savepath)
{
    this->savepath=QString::fromStdString(savepath);
}

void myCanvas::getInformation(Information &information)
{
    information.line=linealgorithm;
    information.polygon=polygonalgorithm;
    information.curve=curvealgorithm;
    information.clip=clipalgorithm;
    information.color=*color;
    information.penwidth=penwidth;
    information.fillcolor=*fillcolor;
    information.fill=fill;
    //information.suffix=suffix;
}

void myCanvas::setPreferences(Information &information)
{
    linealgorithm=information.line;
    polygonalgorithm=information.polygon;
    curvealgorithm=information.curve;
    clipalgorithm=information.clip;
    this->penwidth=information.penwidth;
    setColor(information.color.red(),information.color.green(),information.color.blue());
    setFillColor(information.fillcolor.red(),information.fillcolor.green(),information.fillcolor.blue());
    fill=information.fill;
    //suffix=information.suffix;
}

void myCanvas::handleFileAction(fileAction action)
{
    if(action==New)
        newFile();
    else if(action==Open)
        openFile();
    else if(action==Save)
        saveFile();
    else if(action==Saveas)
        saveFileas();
}

void myCanvas::newFile()
{
    QMessageBox message1(QMessageBox::Warning,"确认新建",tr("确认新建画布?"),QMessageBox::Yes | QMessageBox::No,NULL);
    if(message1.exec()==QMessageBox::Yes)
    {
        if(hasedited)
        {
            QMessageBox message2(QMessageBox::Warning,"保存画布",tr("当前画布有编辑未保存。是否保存?"),QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No,NULL);
            int ret=message2.exec();
            if(ret==QMessageBox::Yes)
                saveFile();
            else if(ret==QMessageBox::Cancel)
                return;
        }
        resetCanvas(width,height,false);
        nowfilename.clear();
        hasedited=false;
    }
}

void myCanvas::openFile()
{
    if(hasedited)
    {
        QMessageBox message(QMessageBox::Warning,"保存画布",tr("当前画布有编辑未保存。打开文件前是否保存?"),QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No,NULL);
        int ret=message.exec();
        if(ret==QMessageBox::Yes)
            saveFile();
        else if(ret==QMessageBox::Cancel)
            return;
    }
    QString file=QFileDialog::getOpenFileName(this,tr("打开文件"),QDir::currentPath(),
                                              "Bitmap File Format (*.bmp);;"
                                              "Portable Network Graphics (*.png);;"
                                              "Joint Photographic Experts Group (*.jpg);;"
                                              "Joint Photographic Experts Group (*.jpeg)");
    if(file.isEmpty())
    {
        QMessageBox message2(QMessageBox::Warning,tr("错误"),tr("打开文件失败!"),QMessageBox::Ok,NULL);
        message2.exec();
        return;
    }
    QImage openimage;
    if(!openimage.load(file))
    {
        QMessageBox message3(QMessageBox::Warning,tr("错误"),tr("加载文件内容失败!"),QMessageBox::Ok,NULL);
        message3.exec();
        return;
    }
    QSize newcanvas=openimage.size();
    resetCanvas(newcanvas.width(),newcanvas.height(),false);
    if(newcanvas.width()>maxwidth||newcanvas.height()>maxheight)
    {
        QMessageBox message4(QMessageBox::Warning,tr("错误"),tr("加载图片大小超过最大限制!"),QMessageBox::Ok,NULL);
        message4.exec();
        return;
    }
    *paintingboard=openimage;
    nowfilename=file;
    hasedited=false;
    QWidget::update();
}

void myCanvas::saveFile()
{
    if(nowfilename.isEmpty())
        saveFileas();
    else
    {
        if(hasedited)
        {
            paintingboard->save(nowfilename);
            hasedited=false;
        }
    }
}

void myCanvas::saveFileas()
{
    QString temp=QFileDialog::getSaveFileName(this,tr("保存画布"),"myDrawingBoard",
                                              "Bitmap File Format (*.bmp);;"
                                              "Portable Network Graphics (*.png);;"
                                              "Joint Photographic Experts Group (*.jpg);;"
                                              "Joint Photographic Experts Group (*.jpeg)");
    if(temp.isEmpty())
        return;
    else
    {
        nowfilename=temp;
        paintingboard->save(nowfilename);
        hasedited=false;
    }
}

QString myCanvas::getNowFilename()
{
    return nowfilename;
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
            case Round:
            {
                map <int,myRound>::iterator currentround;
                currentround=roundset->find(nowid);
                drawRound(nowid,currentround->second.xc,currentround->second.yc,currentround->second.r);
                break;
            }
            case Curve:
            {
                map <int,myCurve>::iterator currentcurve;
                currentcurve=curveset->find(nowid);
                if(editid==nowid)
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
    editid=0;
}

void myCanvas::repaintAll_improved()
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
                if(editid==nowid)
                    drawLine(nowid,currentline->second.x0,currentline->second.y0,currentline->second.x1,
currentline->second.y1,currentline->second.method);
                else
                    paintGraphDot(currentline->second.drawpointset);
                if(selectid==nowid)
                    drawOutline(nowid,Line);
                break;
            }
            case Polygon:
            {
                map <int,myPolygon>::iterator currentpolygon;
                currentpolygon=polygonset->find(nowid);
                if(editid==nowid)
                    drawPolygon(nowid,currentpolygon->second.pointset,currentpolygon->second.pointnum,
currentpolygon->second.method);
                else
                    paintGraphDot(currentpolygon->second.drawpointset);
                if(selectid==nowid)
                    drawOutline(nowid,Polygon);
                break;
            }
            case Ellipse:
            {
                map <int,myEllipse>::iterator currentellipse;
                currentellipse=ellipseset->find(nowid);
                if(editid==nowid)
                    drawEllipse(nowid,currentellipse->second.xc,currentellipse->second.yc,currentellipse->second.rx,
currentellipse->second.ry);
                else
                    paintGraphDot(currentellipse->second.drawpointset);
                if(selectid==nowid)
                    drawOutline(nowid,Ellipse);
                break;
            }
            case Round:
            {
                map <int,myRound>::iterator currentround;
                currentround=roundset->find(nowid);
                if(editid==nowid)
                    drawRound(nowid,currentround->second.xc,currentround->second.yc,currentround->second.r);
                else
                    paintGraphDot(currentround->second.drawpointset);
                if(selectid==nowid)
                    drawOutline(nowid,Round);
                break;
            }
            case Curve:
            {
                map <int,myCurve>::iterator currentcurve;
                currentcurve=curveset->find(nowid);
                if(editid==nowid)
                    drawCurve(nowid,currentcurve->second.pointset,currentcurve->second.pointnum,currentcurve->second.method);
                else
                    paintGraphDot(currentcurve->second.drawpointset);
                if(selectid==nowid)
                    drawOutline(nowid,Curve);
                break;
            }
            default:
                break;
        }
    }
    penwidth=temppenwidth;
    setColor(tempcolor.red(),tempcolor.green(),tempcolor.blue()); //恢复原有画笔颜色
    isrepaint=false;

    editid=0;
}

bool myCanvas::resetCanvas(int width,int height,bool remain)
{
    if(width<minwidth||width>maxwidth||height<minheight||height>maxheight)
        return false;
    bool sizechange=true;
    if(this->width==width&&this->height==height)
        sizechange=false;
    if(sizechange)
    {
        this->width=width;
        this->height=height;
        tempwidth=width;
        tempheight=height;
    }
    QImage newboard(width,height,QImage::Format_RGB32);
    QRgb background=qRgb(255,255,255);

    if(drawboard==paintBoard)
    {
        *paintingboard=newboard;
        paintingboard->fill(background);
    }
    else if(drawboard==tempBoard)
    {
        temppaintboard=newboard;
        temppaintboard.fill(background);
    }
    else
    {
        bufferboard=newboard;
        bufferboard.fill(background);
    }
    if(remain)
    {
        repaintAll_improved();
    }
    else
    {
        lineset->clear();
        polygonset->clear();
        ellipseset->clear();
        roundset->clear();
        graphset->clear();
        iscreatingshape=false;
        iseditingshape=false;

        ispolygonfirst=true;
        polygonpointnum=0;

        curvelinedrawed=false;
        curvepointnum=0;

        drawboard=paintBoard;
        draw_on_real_board=true;
        pointvector.clear();

        sequence.clear();

        selectid=0;
        editid=0;
        drawoutline=true;
        onselectgraph=false;
        centerdrawed=false;
        firstpointdrawed=false;

        hasedited=true;
    }
    if(sizechange && drawboard==paintBoard)
        emit send_resCan_signal();
    QWidget::update();
    return true;
}

void myCanvas::resetCanvasEdit(int width,int height,bool final)
{
    QImage newboard(width,height,QImage::Format_RGB32);
    QRgb background=qRgb(255,255,255);

    if(drawboard==paintBoard)
    {
        *paintingboard=newboard;
        paintingboard->fill(background);
    }
    else if(drawboard==tempBoard)
    {
        temppaintboard=newboard;
        temppaintboard.fill(background);
    }
    else
    {
        bufferboard=newboard;
        bufferboard.fill(background);
    }

    if(final)
    {
        this->width=width;
        this->height=height;
        tempwidth=width;
        tempheight=height;
        emit send_resCan_signal();
    }

    repaintAll_improved();
    QWidget::update();
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

bool myCanvas::setFillColor(int r,int g,int b)
{
    if(r<0||r>255)
        return false;
    if(g<0||g>255)
        return false;
    if(b<0||b>255)
        return false;
    fillcolor->setRgb(r,g,b);
    fillpen->setColor(*fillcolor);
    return true;
}

bool myCanvas::setPenwidth(int penwidth)
{
    if(penwidth>=1&&penwidth<=10)
    {
        this->penwidth=penwidth;
        drawpen->setWidth(penwidth);
        return true;
    }
    else
        return false;
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

void myCanvas::paintSingleLine_DDA(int x0,int y0,int x1,int y1,QPainter *painter,QVector <QPoint> &pointset)
{
    if(x0==x1&&y0==y1)
    {
        painter->drawPoint(x0,y0);
        pointset.push_back(QPoint(x0,y0));
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
        int drawx=(int)round(x);
        int drawy=(int)round(y);
        painter->drawPoint(drawx,drawy);
        pointset.push_back(QPoint(drawx,drawy));
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
        //qDebug() << x << ',' << y;
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

void myCanvas::paintSingleLine_Bresenham(int x0,int y0,int x1,int y1,QPainter *painter,QVector <QPoint> &pointset)
{
    if(x0==x1&&y0==y1)
    {
        painter->drawPoint(x0,y0);
        pointset.push_back(QPoint(x0,y0));
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
        pointset.push_back(QPoint(x,y));
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

void myCanvas::paintDiscreteLine_Bresenham(int x0,int y0,int x1,int y1,QPainter *painter)
{
    if(x0==x1&&y0==y1)
    {
        painter->drawPoint(x0,y0);
        QWidget::update();
        return;
    }
    bool draw=true;
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
        if(draw)
        {
            painter->drawPoint(x,y);
            draw=false;
        }
        else
            draw=true;
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
    //QPainter *painter=new QPainter(paintingboard);
    QPainter *painter=setPainter(drawpen);

    QPoint leftup,leftdown,rightup,rightdown;

    if(x0<=x1)
    {
        if(y0<=y1)
        {
            leftup=QPoint(x0,y1);
            leftdown=QPoint(x0,y0);
            rightup=QPoint(x1,y1);
            rightdown=QPoint(x1,y0);
        }
        else
        {
            leftup=QPoint(x0,y0);
            leftdown=QPoint(x0,y1);
            rightup=QPoint(x1,y0);
            rightdown=QPoint(x1,y1);
        }
    }
    else
    {
        if(y0<=y1)
        {
            leftup=QPoint(x1,y1);
            leftdown=QPoint(x1,y0);
            rightup=QPoint(x0,y1);
            rightdown=QPoint(x0,y0);
        }
        else
        {
            leftup=QPoint(x1,y0);
            leftdown=QPoint(x1,y1);
            rightup=QPoint(x0,y0);
            rightdown=QPoint(x0,y1);
        }
    }

    QPoint center((int)round(((double)x0+(double)x1)/2),(int)round(((double)y0+(double)y1)/2));

    if(isrepaint)
    {
        map <int,myLine>::iterator currentline;
        currentline=lineset->find(id);
        if(currentline==lineset->end())
        {
            qDebug() << "error in drawLine";
            if(method==DDA)
                paintSingleLine_DDA(x0,y0,x1,y1,painter);
            else if(method==Bresenham)
                paintSingleLine_Bresenham(x0,y0,x1,y1,painter);
        }
        else
        {
            currentline->second.drawpointset.clear();
            if(method==DDA)
                paintSingleLine_DDA(x0,y0,x1,y1,painter,currentline->second.drawpointset);
            else if(method==Bresenham)
                paintSingleLine_Bresenham(x0,y0,x1,y1,painter,currentline->second.drawpointset);
        }

        currentline->second.rect.leftup=leftup;
        currentline->second.rect.leftdown=leftdown;
        currentline->second.rect.rightup=rightup;
        currentline->second.rect.rightdown=rightdown;
        currentline->second.rect.center=center;

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
    line.drawpointset.clear();
    if(method==DDA)
    {
        paintSingleLine_DDA(x0,y0,x1,y1,painter,line.drawpointset);
        line.method=DDA;
    }
    else if(method==Bresenham)
    {
        paintSingleLine_Bresenham(x0,y0,x1,y1,painter,line.drawpointset);
        line.method=Bresenham;
    }

    line.rect.leftup=leftup;
    line.rect.leftdown=leftdown;
    line.rect.rightup=rightup;
    line.rect.rightdown=rightdown;
    line.rect.center=center;

    lineset->insert(make_pair(id,line));
    delete painter;
    return true;
}

bool myCanvas::drawPolygon(int id,int *pointset,int pointnum,drawingAlgorithm method)
{
    //QPainter *painter=new QPainter(paintingboard);
    QPainter *painter=setPainter(drawpen);

    int xmin=pointset[0],ymin=pointset[1],xmax=pointset[0],ymax=pointset[1];
    for(int i=1;i<pointnum;i++)
    {
        if(pointset[2*i]<xmin)
            xmin=pointset[2*i];
        if(pointset[2*i]>xmax)
            xmax=pointset[2*i];
        if(pointset[2*i+1]<ymin)
            ymin=pointset[2*i+1];
        if(pointset[2*i+1]>ymax)
            ymax=pointset[2*i+1];
    }
    QPoint leftup(xmin,ymax);
    QPoint leftdown(xmin,ymin);
    QPoint rightup(xmax,ymax);
    QPoint rightdown(xmax,ymin);
    QPoint center((int)round(((double)xmin+(double)xmax)/2),(int)round(((double)ymin+(double)ymax)/2));

    if(isrepaint)
    {
        map <int,myPolygon>::iterator currentpolygon;
        currentpolygon=polygonset->find(id);
        if(currentpolygon==polygonset->end())
        {
            qDebug() << "error in drawPolygon";
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
        }
        else
        {
            currentpolygon->second.drawpointset.clear();
            if(method==DDA)
            {
                for(int i=0;i<pointnum;i++)
                    paintSingleLine_DDA(pointset[2*i],pointset[2*i+1],pointset[(2*(i+1))%(pointnum*2)],
    pointset[(2*(i+1)+1)%(pointnum*2)],painter,currentpolygon->second.drawpointset);
            }
            else if(method==Bresenham)
            {
                for(int i=0;i<pointnum;i++)
                    paintSingleLine_Bresenham(pointset[2*i],pointset[2*i+1],pointset[(2*(i+1))%(pointnum*2)],
    pointset[(2*(i+1)+1)%(pointnum*2)],painter,currentpolygon->second.drawpointset);
            }
        }

        currentpolygon->second.rect.leftup=leftup;
        currentpolygon->second.rect.leftdown=leftdown;
        currentpolygon->second.rect.rightup=rightup;
        currentpolygon->second.rect.rightdown=rightdown;
        currentpolygon->second.rect.center=center;

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
    polygon.drawpointset.clear();
    if(method==DDA)
    {
        for(int i=0;i<pointnum;i++)
            paintSingleLine_DDA(pointset[2*i],pointset[2*i+1],pointset[(2*(i+1))%(pointnum*2)],
pointset[(2*(i+1)+1)%(pointnum*2)],painter,polygon.drawpointset);
        polygon.method=DDA;
    }
    else if(method==Bresenham)
    {
        for(int i=0;i<pointnum;i++)
            paintSingleLine_Bresenham(pointset[2*i],pointset[2*i+1],pointset[(2*(i+1))%(pointnum*2)],
pointset[(2*(i+1)+1)%(pointnum*2)],painter,polygon.drawpointset);
        polygon.method=Bresenham;
    }

    polygon.rect.leftup=leftup;
    polygon.rect.leftdown=leftdown;
    polygon.rect.rightup=rightup;
    polygon.rect.rightdown=rightdown;
    polygon.rect.center=center;

    polygonset->insert(make_pair(id,polygon));
    delete painter;
    return true;
}

void myCanvas::drawEllipsePoint(int xc,int yc,int x,int y,int lastx,int lasty,QPainter *painter,TransMatrix *tmatrix,QVector <QPoint> &pointset)
{
    int x1=xc+x;
    int y1=yc+y;
    int lastx1=xc+lastx;
    int lasty1=yc+lasty;
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
    tmatrix->transCoordinate(lastx1,lasty1);
    if((x1-lastx1)*(x1-lastx1)+(y1-lasty1)*(y1-lasty1)>2)
    {
        int lastx2=xc-lastx;
        int lasty2=yc+lasty;
        int lastx3=xc+lastx;
        int lasty3=yc-lasty;
        int lastx4=xc-lastx;
        int lasty4=yc-lasty;
        tmatrix->transCoordinate(lastx2,lasty2);
        tmatrix->transCoordinate(lastx3,lasty3);
        tmatrix->transCoordinate(lastx4,lasty4);
        paintSingleLine_Bresenham(lastx1,lasty1,x1,y1,painter,pointset);
        paintSingleLine_Bresenham(lastx2,lasty2,x2,y2,painter,pointset);
        paintSingleLine_Bresenham(lastx3,lasty3,x3,y3,painter,pointset);
        paintSingleLine_Bresenham(lastx4,lasty4,x4,y4,painter,pointset);
    }
    else
    {
        painter->drawPoint(x1,y1);
        painter->drawPoint(x2,y2);
        painter->drawPoint(x3,y3);
        painter->drawPoint(x4,y4);
        pointset.push_back(QPoint(x1,y1));
        pointset.push_back(QPoint(x2,y2));
        pointset.push_back(QPoint(x3,y3));
        pointset.push_back(QPoint(x4,y4));
    }
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

    /*QPainter *painter;
    painter=new QPainter(paintingboard);
    painter->setPen(*drawpen);*/
    QPainter *painter=setPainter(drawpen);

    map <int,myEllipse>::iterator currentellipse;
    currentellipse=ellipseset->find(id);
    currentellipse->second.drawpointset.clear();

    if(ry==0)
    {
        int x=0,y=0;
        for(;x<=rx;x++)
            drawEllipsePoint(xc,yc,x,y,x,y,painter,&currentellipse->second.ellipsematrix,currentellipse->second.drawpointset);
        QWidget::update();
        delete painter;
        return true;
    }
    int x=0,y=ry;
    int lastx=0,lasty=ry;
    double sqrx=(double)rx*rx;
    double sqry=(double)ry*ry;
    drawEllipsePoint(xc,yc,x,y,lastx,lasty,painter,&currentellipse->second.ellipsematrix,currentellipse->second.drawpointset);
    double p=sqry-sqrx*ry+sqrx/(double)4; //p1k的初始值
    while(sqry*x<sqrx*y) //在区域1内
    {
        lastx=x;
        lasty=y;
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
        drawEllipsePoint(xc,yc,x,y,lastx,lasty,painter,&currentellipse->second.ellipsematrix,currentellipse->second.drawpointset);
    }
    p=sqry*(x+0.5)*(x+0.5)+sqrx*(y-1)*(y-1)-sqrx*sqry; //根据区域1计算的最后位置的像素点坐标初始化p2k
    while(y>0) //在区域2内
    {
        lastx=x;
        lasty=y;
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
        drawEllipsePoint(xc,yc,x,y,lastx,lasty,painter,&currentellipse->second.ellipsematrix,currentellipse->second.drawpointset);
    }
    QWidget::update();

    int x1=xc-rx,y1=yc+ry;
    int x2=xc-rx,y2=yc-ry;
    int x3=xc+rx,y3=yc+ry;
    int x4=xc+rx,y4=yc-ry;

    if(oncontrolpoint&&controlshape==Ellipse)
    {
        if(controlpoint==0)
        {
            x1=prevrect.leftup.x();
            y1=prevrect.leftup.y();
            x4=prevrect.rightdown.x();
            y4=prevrect.rightdown.y();
            x2=x1;
            y2=y4;
            x3=x4;
            y3=y1;
        }
        else if(controlpoint==1)
        {
            x2=prevrect.leftdown.x();
            y2=prevrect.leftdown.y();
            x3=prevrect.rightup.x();
            y3=prevrect.rightup.y();
            x1=x2;
            y1=y3;
            x4=x3;
            y4=y2;
        }
        else if(controlpoint==2)
        {
            x2=prevrect.leftdown.x();
            y2=prevrect.leftdown.y();
            x3=prevrect.rightup.x();
            y3=prevrect.rightup.y();
            x1=x2;
            y1=y3;
            x4=x3;
            y4=y2;
        }
        else
        {
            x1=prevrect.leftup.x();
            y1=prevrect.leftup.y();
            x4=prevrect.rightdown.x();
            y4=prevrect.rightdown.y();
            x2=x1;
            y2=y4;
            x3=x4;
            y3=y1;
        }
    }

    currentellipse->second.ellipsematrix.transCoordinate(x1,y1);
    currentellipse->second.ellipsematrix.transCoordinate(x2,y2);
    currentellipse->second.ellipsematrix.transCoordinate(x3,y3);
    currentellipse->second.ellipsematrix.transCoordinate(x4,y4);

    if(oncontrolpoint&&controlshape==Ellipse)
    {
        if(controlpoint==0)
        {
            x4=settledpoint.x();
            y4=settledpoint.y();
        }
        else if(controlpoint==1)
        {
            x3=settledpoint.x();
            y3=settledpoint.y();
        }
        else if(controlpoint==2)
        {
            x2=settledpoint.x();
            y2=settledpoint.y();
        }
        else
        {
            x1=settledpoint.x();
            y1=settledpoint.y();
        }
    }

    QPoint leftup(x1,y1);
    QPoint leftdown(x2,y2);
    QPoint rightup(x3,y3);
    QPoint rightdown(x4,y4);
    QPoint center(xc,yc);

    //qDebug () << leftup << leftdown << rightup << rightdown;

    currentellipse->second.rect.leftup=leftup;
    currentellipse->second.rect.leftdown=leftdown;
    currentellipse->second.rect.rightup=rightup;
    currentellipse->second.rect.rightdown=rightdown;
    currentellipse->second.rect.center=center;

    delete painter;
    return true;
}

void myCanvas::drawRoundPoint(int xc,int yc,int x,int y,QPainter *painter)
{
    painter->drawPoint(xc+x,yc+y);
    painter->drawPoint(xc-x,yc+y);
    painter->drawPoint(xc+x,yc-y);
    painter->drawPoint(xc-x,yc-y);
    painter->drawPoint(xc+y,yc+x);
    painter->drawPoint(xc-y,yc+x);
    painter->drawPoint(xc+y,yc-x);
    painter->drawPoint(xc-y,yc-x);
}

void myCanvas::drawRoundPoint(int xc,int yc,int x,int y,QPainter *painter,QVector <QPoint> &pointset)
{
    painter->drawPoint(xc+x,yc+y);
    painter->drawPoint(xc-x,yc+y);
    painter->drawPoint(xc+x,yc-y);
    painter->drawPoint(xc-x,yc-y);
    painter->drawPoint(xc+y,yc+x);
    painter->drawPoint(xc-y,yc+x);
    painter->drawPoint(xc+y,yc-x);
    painter->drawPoint(xc-y,yc-x);
    pointset.push_back(QPoint(xc+x,yc+y));
    pointset.push_back(QPoint(xc-x,yc+y));
    pointset.push_back(QPoint(xc+x,yc-y));
    pointset.push_back(QPoint(xc-x,yc-y));
    pointset.push_back(QPoint(xc+y,yc+x));
    pointset.push_back(QPoint(xc-y,yc+x));
    pointset.push_back(QPoint(xc+y,yc-x));
    pointset.push_back(QPoint(xc-y,yc-x));
}

bool myCanvas::drawRound(int id,int xc,int yc,int r)
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
        graph.shape=Round;
        graph.penwidth=penwidth;
        graphset->insert(make_pair(id,graph));
        myRound round;
        round.id=id;
        round.xc=xc;
        round.yc=yc;
        round.r=r;
        roundset->insert(make_pair(id,round));
    }

    QPainter *painter=setPainter(drawpen);

    map <int,myRound>::iterator currentround;
    currentround=roundset->find(id);
    currentround->second.drawpointset.clear();

    int x=0,y=r;
    double d=1.25-(double)r;
    drawRoundPoint(xc,yc,x,y,painter,currentround->second.drawpointset);
    while(x<y)
    {
        if(d<0)
        {
            d=d+2*x+3;
            x++;
        }
        else
        {
            d=d+2*(x-y)+5;
            x++;
            y--;
        }
        drawRoundPoint(xc,yc,x,y,painter,currentround->second.drawpointset);
    }
    QWidget::update();

    int x1=xc-r,y1=yc+r;
    int x2=xc-r,y2=yc-r;
    int x3=xc+r,y3=yc+r;
    int x4=xc+r,y4=yc-r;

    if(oncontrolpoint&&controlshape==Round)
    {
        if(controlpoint==0)
        {
            x1=prevrect.leftup.x();
            y1=prevrect.leftup.y();
            x4=prevrect.rightdown.x();
            y4=prevrect.rightdown.y();
            x2=x1;
            y2=y4;
            x3=x4;
            y3=y1;
        }
        else if(controlpoint==1)
        {
            x2=prevrect.leftdown.x();
            y2=prevrect.leftdown.y();
            x3=prevrect.rightup.x();
            y3=prevrect.rightup.y();
            x1=x2;
            y1=y3;
            x4=x3;
            y4=y2;
        }
        else if(controlpoint==2)
        {
            x2=prevrect.leftdown.x();
            y2=prevrect.leftdown.y();
            x3=prevrect.rightup.x();
            y3=prevrect.rightup.y();
            x1=x2;
            y1=y3;
            x4=x3;
            y4=y2;
        }
        else
        {
            x1=prevrect.leftup.x();
            y1=prevrect.leftup.y();
            x4=prevrect.rightdown.x();
            y4=prevrect.rightdown.y();
            x2=x1;
            y2=y4;
            x3=x4;
            y3=y1;
        }
    }

    currentround->second.roundmatrix.transCoordinate(x1,y1);
    currentround->second.roundmatrix.transCoordinate(x2,y2);
    currentround->second.roundmatrix.transCoordinate(x3,y3);
    currentround->second.roundmatrix.transCoordinate(x4,y4);

    if(oncontrolpoint&&controlshape==Round)
    {
        if(controlpoint==0)
        {
            x4=settledpoint.x();
            y4=settledpoint.y();
        }
        else if(controlpoint==1)
        {
            x3=settledpoint.x();
            y3=settledpoint.y();
        }
        else if(controlpoint==2)
        {
            x2=settledpoint.x();
            y2=settledpoint.y();
        }
        else
        {
            x1=settledpoint.x();
            y1=settledpoint.y();
        }
    }

    QPoint leftup(x1,y1);
    QPoint leftdown(x2,y2);
    QPoint rightup(x3,y3);
    QPoint rightdown(x4,y4);
    QPoint center(xc,yc);

    currentround->second.rect.leftup=leftup;
    currentround->second.rect.leftdown=leftdown;
    currentround->second.rect.rightup=rightup;
    currentround->second.rect.rightdown=rightdown;
    currentround->second.rect.center=center;
    delete painter;
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

void myCanvas::drawCurve_Bezier(QVector <QPoint> &set,int n,QPainter *painter,QVector <QPoint> &pointset)
{

    doublePoint **p;
    p=new doublePoint*[n+1];
    for(int i=0;i<=n;i++)
        p[i]=new doublePoint[n+1];

    //int lastx=set[0].x(),lasty=set[0].y();

    double du=minStep_curve(set,n,Bezier); //计算u递增的步长

    for(double u=0;u<1+du;u=u+du) //u从0依据步长递增到1，结束为1+du是因为要确保绘制u=1时曲线的终点
    {
        for(int j=0;j<=n;j++) //依据控制点坐标初始化基本点信息
        {
            p[0][j].x=(double)set[j].x();
            p[0][j].y=(double)set[j].y();
        }

        for(int i=1;i<=n;i++) //不断循环迭代
        {
            for(int j=0;j<=n-i;j++)
            {
                p[i][j].x=(1-u)*p[i-1][j].x+u*p[i-1][j+1].x;
                p[i][j].y=(1-u)*p[i-1][j].y+u*p[i-1][j+1].y;
            }
        }

        int x=(int)round(p[n][0].x); //计算对应型值点的横坐标
        int y=(int)round(p[n][0].y); //计算对应型值点的纵坐标

        painter->drawPoint(x,y);
        pointset.push_back(QPoint(x,y));

        //paintSingleLine_Bresenham(lastx,lasty,x,y,painter);

        //lastx=x;
        //lasty=y;
    }

    for(int i=0;i<=n;i++)
        delete []p[i];
    delete []p;
}

void myCanvas::drawCurve_Bspline(QVector <QPoint> &set,int n,QPainter *painter,QVector <QPoint> &pointset)
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
    for(int i=0;i<n-k+1;i++) //确定绘制局部曲线所需要的控制点
    {
        controlset.clear();
        for(int j=0;j<=k;j++) //将这些控制点初始信息放入矩阵对应位置进行初始化
        {
            controlset.push_back(set[i+j]);
            p[0][i+j].x=(double)set[i+j].x();
            p[0][i+j].y=(double)set[i+j].y();
        }
        double du=minStep_curve(controlset,k,Bspline); //确定u的步长
        for(double u=(double)i+k;u<(double)i+k+1+du;u=u+du) //对于每一个u计算曲线型值点，结束为i+k+1+du是因为要确保绘制u=i+k+1时曲线的终点
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
            pointset.push_back(QPoint(x,y));
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

    map <int,myCurve>::iterator currentcurve;
    currentcurve=curveset->find(id);
    currentcurve->second.drawpointset.clear();

    QPainter *painter=setPainter(drawpen);

    if(method==Bezier)
        drawCurve_Bezier(pointset,pointnum-1,painter,currentcurve->second.drawpointset);
    else
        drawCurve_Bspline(pointset,pointnum-1,painter,currentcurve->second.drawpointset);

    QWidget::update();

    int xmin=pointset[0].x(),ymin=pointset[0].y(),xmax=pointset[0].x(),ymax=pointset[0].y();
    for(int i=1;i<pointnum;i++)
    {
        if(pointset[i].x()<xmin)
            xmin=pointset[i].x();
        if(pointset[i].x()>xmax)
            xmax=pointset[i].x();
        if(pointset[i].y()<ymin)
            ymin=pointset[i].y();
        if(pointset[i].y()>ymax)
            ymax=pointset[i].y();
    }
    QPoint leftup(xmin,ymax);
    QPoint leftdown(xmin,ymin);
    QPoint rightup(xmax,ymax);
    QPoint rightdown(xmax,ymin);
    QPoint center((int)round(((double)xmin+(double)xmax)/2),(int)round(((double)ymin+(double)ymax)/2));
    currentcurve->second.rect.leftup=leftup;
    currentcurve->second.rect.leftdown=leftdown;
    currentcurve->second.rect.rightup=rightup;
    currentcurve->second.rect.rightdown=rightdown;
    currentcurve->second.rect.center=center;

    delete painter;
    return true;
}

bool myCanvas::translate(int id,int dx,int dy)
{
    map <int,myGraph>::iterator current;
    current=graphset->find(id);
    if(current==graphset->end())
        return false;
    Shape shape=current->second.shape;
    if(shape==Line)
    {
        map <int,myLine>::iterator currentline;
        currentline=lineset->find(id);
        currentline->second.x0+=dx;
        currentline->second.y0+=dy;
        currentline->second.x1+=dx;
        currentline->second.y1+=dy;
    }
    else if(shape==Polygon)
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
    else if(shape==Ellipse)
    {
        map <int,myEllipse>::iterator currentellipse;
        currentellipse=ellipseset->find(id);
        currentellipse->second.ellipsematrix.translateEllipse(currentellipse->second.xc,currentellipse->second.yc,dx,dy);
        currentellipse->second.xc+=dx;
        currentellipse->second.yc+=dy;
    }
    else if(shape==Round)
    {
        map <int,myRound>::iterator currentround;
        currentround=roundset->find(id);
        currentround->second.roundmatrix.translateRound(currentround->second.xc,currentround->second.yc,dx,dy);
        currentround->second.xc+=dx;
        currentround->second.yc+=dy;
    }
    else if(shape==Curve)
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
    resetCanvas(width,height,true);
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
    else if(current->second.shape==Round)
    {
        map <int,myRound>::iterator currentround;
        currentround=roundset->find(id);
        int xc=currentround->second.xc;
        int yc=currentround->second.yc;
        currentround->second.roundmatrix.rotateRound(r,x,y,xc,yc);
        currentround->second.xc=xc;
        currentround->second.yc=yc;
    }
    else if(current->second.shape==Curve)
    {
        map <int,myCurve>::iterator currentcurve;
        currentcurve=curveset->find(id);
        currentcurve->second.curvematrix.rotateCurve(r,x,y,currentcurve->second.pointset,currentcurve->second.pointnum);
    }

    resetCanvas(width,height,true);
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
    else if(current->second.shape==Round)
    {
        map <int,myRound>::iterator currentround;
        currentround=roundset->find(id);
        int xc=currentround->second.xc;
        int yc=currentround->second.yc;
        int r0=currentround->second.r;
        currentround->second.roundmatrix.scaleRound(r,x,y,xc,yc,r0);
        currentround->second.xc=xc;
        currentround->second.yc=yc;
        currentround->second.r=r0;
    }
    else if(current->second.shape==Curve)
    {
        map <int,myCurve>::iterator currentcurve;
        currentcurve=curveset->find(id);
        int pointnum=currentcurve->second.pointnum;
        currentcurve->second.curvematrix.scaleCurve(r,x,y,pointnum,currentcurve->second.pointset);
    }
    resetCanvas(width,height,true);
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
    int p1=x0-x1,q1=x0-clipwindow->xmin; //计算p1，q1
    int p2=x1-x0,q2=clipwindow->xmax-x0; //计算p2，q2
    int p3=y0-y1,q3=y0-clipwindow->ymin; //计算p3，q3
    int p4=y1-y0,q4=clipwindow->ymax-y0; //计算p4，q4
    if(p1==0) //直线平行于x轴
    {
        if(q1<0||q2<0) //直线完全在窗口外面
            return false; //直接舍弃
        else
        {
            int ymin=max(clipwindow->ymin,min(y0,y1)); //计算裁剪后下端点
            int ymax=min(clipwindow->ymax,max(y0,y1)); //计算裁剪后上端点
            if(ymin>ymax) //下端点比上端点还要高，即直线在窗口外面
                return false; //直接舍弃
            else
            {
                y0=ymin;
                y1=ymax;
                return true;
            }
        }
    }
    else if(p3==0) //直线平行于x轴
    {
        if(q3<0||q4<0) //直线完全在窗口外面
            return false; //直接舍弃
        else
        {
            int xmin=max(clipwindow->xmin,min(x0,x1)); //计算裁剪后左端点
            int xmax=min(clipwindow->xmax,max(x0,x1));  //计算裁剪后右端点
            if(xmin>xmax) //左端点在右端点右边，即直线在窗口外面
                return false; //直接舍弃
            else
            {
                x0=xmin;
                x1=xmax;
                return true;
            }
        }
    }
    else //不存在pk=0，即直线不平行于坐标轴
    {
        double u1=(double)q1/(double)p1;
        double u2=(double)q2/(double)p2;
        double u3=(double)q3/(double)p3;
        double u4=(double)q4/(double)p4;
        double umin,umax; //定义裁剪后两个端点的参数
        if(p1<0)
        {
            if(p3<0) //此时u1和u3代表由外向内交点的参数
            {
                umin=max((double)0,max(u1,u3));
                umax=min((double)1,min(u2,u4));
            }
            else //此时u1和u4代表由外向内交点的参数
            {
                umin=max((double)0,max(u1,u4));
                umax=min((double)1,min(u2,u3));
            }
        }
        else
        {
            if(p3<0) //此时u2和u3代表由外向内交点的参数
            {
                umin=max((double)0,max(u2,u3));
                umax=min((double)1,min(u1,u4));
            }
            else //此时u2和u4代表由外向内交点的参数
            {
                umin=max((double)0,max(u2,u4));
                umax=min((double)1,min(u1,u3));
            }
        }
        if(umin>umax) //直线在窗口外面
            return false;
        else //根据裁剪后的端点参数依据参数方程更新端点
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
        //qDebug() << x0 << y0 << x1 << y1;
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
    resetCanvas(width,height,true);
    return true;
}

bool myCanvas::assertion()
{
    /*if(iscreatingshape)
    {
        QMessageBox message(QMessageBox::Warning,"invalid",tr("正在绘制图形!"),
QMessageBox::Ok,NULL);
        message.exec();
        return true;
    }*/
    if(iscreatingshape)
    {
        draw_on_real_board=true;
        drawboard=paintBoard;
        iscreatingshape=false;
        if(createshape==Polygon)
        {
            createPolygon_ui();
        }
        else if(createshape==Curve)
        {
            createCurve_ui();
        }
    }
    /*if(iseditingshape)
    {
        QMessageBox message(QMessageBox::Warning,"invalid",tr("正在编辑图形!"),
QMessageBox::Ok,NULL);
        message.exec();
        return true;
    }*/
    if(iseditingshape)
    {
        draw_on_real_board=true;
        drawboard=paintBoard;
        iseditingshape=false;
        if(centerdrawed)
        {
            centerdrawed=false;
            firstpointdrawed=false;
            resetCanvas(width,height,true);
        }
    }
    return false;
}

void myCanvas::setAction(Shape shape,editAction action)
{
    if(assertion())
        return;
    if(action==Create)
    {
        createshape=shape;
        if(selectid!=0)
        {
            selectid=0;
            drawoutline=true;
            onselectgraph=false;
            centerdrawed=false;
            firstpointdrawed=false;
            resetCanvas(width,height,true);
        }
        editid=0;
    }
    else
    {
        editshape=shape;
    }
    editaction=action;
}

int myCanvas::generateID()
{
    int i=1;
    for(;;i++)
    {
        map <int,myGraph>::iterator current;
        current=graphset->find(i);
        if(current==graphset->end())
            break;
    }
    return i;
}

QPainter* myCanvas::setPainter(QPen *pen)
{
    QPainter *painter;
    if(drawboard==paintBoard)
        painter=new QPainter(paintingboard);
    else if(drawboard==tempBoard)
        painter=new QPainter(&temppaintboard);
    else
        painter=new QPainter(&bufferboard);
    painter->setPen(*pen);
    return painter;
}

void myCanvas::changeSequence(int id,bool insert)
{
    if(insert)
    {
        list <int>::iterator current;
        for(current=sequence.begin();current!=sequence.end();current++)
        {
            if(*current==id)
            {
                qDebug() << "error in changeSequence: duplicate id insert";
                return;
            }
        }
        sequence.push_front(id);
    }
    else
    {
        bool found=false;
        list <int>::iterator current;
        for(current=sequence.begin();current!=sequence.end();current++)
        {
            if(*current==id)
            {
                found=true;
                sequence.erase(current);
                sequence.push_back(id);
                break;
            }
        }
        if(!found)
        {
            qDebug() << "error in changeSequence: not found the changing id";
        }
    }
}

void myCanvas::createLine_ui()
{
    int id=generateID();
    QPoint end=pointvector[1];
    QPoint begin=pointvector[0];
    drawLine(id,begin.x(),begin.y(),end.x(),end.y(),linealgorithm);
    pointvector.clear();
    changeSequence(id,true);
}

void myCanvas::createPolygon_ui()
{
    if(polygonpointnum<2)
    {
        pointvector.clear();
        polygonpointnum=0;
        ispolygonfirst=true;
        QWidget::update();
        return;
    }
    int *pointset=new int[polygonpointnum*2];
    int id=generateID();
    for(int i=0;i<polygonpointnum;i++)
    {
        QPoint point=pointvector[i];
        pointset[2*i]=point.x();
        pointset[2*i+1]=point.y();
    }
    drawPolygon(id,pointset,polygonpointnum,polygonalgorithm);
    polygonpointnum=0;
    ispolygonfirst=true;
    pointvector.clear();
    delete []pointset;
    changeSequence(id,true);
}

void myCanvas::drawtempClipWindow_ui(QPoint &point)
{
    /*QPainter *painter=new QPainter(&temppaintboard);
    painter->setPen(*clipwindowpen);
    paintDiscreteLine_Bresenham(cliplinewindow.xmin,cliplinewindow.ymin,point.x(),cliplinewindow.ymin,painter);
    paintDiscreteLine_Bresenham(cliplinewindow.xmin,cliplinewindow.ymin,cliplinewindow.xmin,point.y(),painter);
    paintDiscreteLine_Bresenham(point.x(),cliplinewindow.ymin,point.x(),point.y(),painter);
    paintDiscreteLine_Bresenham(cliplinewindow.xmin,point.y(),point.x(),point.y(),painter);*/
    QPainter *painter=setPainter(clipwindowpen);
    painter->drawLine(QPoint(cliplinewindow.xmin,cliplinewindow.ymin),QPoint(point.x(),cliplinewindow.ymin));
    painter->drawLine(QPoint(cliplinewindow.xmin,cliplinewindow.ymin),QPoint(cliplinewindow.xmin,point.y()));
    painter->drawLine(QPoint(point.x(),cliplinewindow.ymin),QPoint(point.x(),point.y()));
    painter->drawLine(QPoint(cliplinewindow.xmin,point.y()),QPoint(point.x(),point.y()));
    QWidget::update();
    delete painter;
}

void myCanvas::clipLine_ui()
{
    if(cliplinewindow.xmin==cliplinewindow.xmax||cliplinewindow.ymin==cliplinewindow.ymax)
        return;
    if(cliplinewindow.xmin>cliplinewindow.xmax)
        swapNum(cliplinewindow.xmin,cliplinewindow.xmax);
    if(cliplinewindow.ymin>cliplinewindow.ymax)
        swapNum(cliplinewindow.ymin,cliplinewindow.ymax);
    map <int,myLine>::iterator currentline;
    if(selectid==0)
    {
        for(currentline=lineset->begin();currentline!=lineset->end();) //遍历直线容器
        {
            int x0=currentline->second.x0;
            int y0=currentline->second.y0;
            int x1=currentline->second.x1;
            int y1=currentline->second.y1;
            bool remain=true;
            if(clipalgorithm==CohenSutherland)
                remain=clipLine_CohenSutherland(x0,y0,x1,y1,&cliplinewindow);
            else if(clipalgorithm==LiangBarsky)
                remain=clipLine_LiangBarsky(x0,y0,x1,y1,&cliplinewindow);
            if(remain) //裁剪后仍存在该直线
            {
                currentline->second.x0=x0;
                currentline->second.y0=y0;
                currentline->second.x1=x1;
                currentline->second.y1=y1;
                isrepaint=true;
                drawLine(currentline->first,x0,y0,x1,y1,currentline->second.method); //显式调用画线算法重绘
                isrepaint=false;
                currentline++;
            }
            else //裁剪后直线消失，从容器中移除
            {
                graphset->erase(currentline->first);
                currentline=lineset->erase(currentline);
            }
        }
    }
    else
    {
        currentline=lineset->find(selectid);
        if(currentline!=lineset->end())
        {
            int x0=currentline->second.x0;
            int y0=currentline->second.y0;
            int x1=currentline->second.x1;
            int y1=currentline->second.y1;
            bool remain=true;
            if(clipalgorithm==CohenSutherland)
                remain=clipLine_CohenSutherland(x0,y0,x1,y1,&cliplinewindow);
            else if(clipalgorithm==LiangBarsky)
                remain=clipLine_LiangBarsky(x0,y0,x1,y1,&cliplinewindow);
            if(remain)
            {
                currentline->second.x0=x0;
                currentline->second.y0=y0;
                currentline->second.x1=x1;
                currentline->second.y1=y1;
                editid=selectid;
            }
            else
            {
                graphset->erase(currentline->first);
                lineset->erase(currentline);
                selectid=0;
            }
        }
    }
    resetCanvas(width,height,true);
}

void myCanvas::drawtempLine_ui(int pivot,QPoint &point,bool fluent,QPen *pen)
{
    QPoint begin=pointvector[pivot];
    QPainter *painter=setPainter(pen);
    /*if(drawboard==paintBoard)
        painter=new QPainter(paintingboard);
    else if(drawboard==tempBoard)
        painter=new QPainter(&temppaintboard);
    else
        painter=new QPainter(&bufferboard);
    painter->setPen(*pen);*/
    if(fluent)
        paintSingleLine_Bresenham(begin.x(),begin.y(),point.x(),point.y(),painter);
    else
    {
        //paintDiscreteLine_Bresenham(begin.x(),begin.y(),point.x(),point.y(),painter);
        painter->drawLine(begin,point);
        QWidget::update();
    }
    delete painter;
    QPainter *painter2=setPainter(pointpen);
    paintPoint(begin.x(),begin.y(),painter2,solidpointpen);
    paintPoint(point.x(),point.y(),painter2,solidpointpen);
    delete painter2;
}

void myCanvas::drawtempEllipse_ui(QPoint &point)
{
    int xmin=pointvector[0].x(); //获取起始点横坐标
    int ymin=pointvector[0].y(); //获取起始点纵坐标
    int xmax=point.x();
    int ymax=point.y();
    if(xmin>xmax)
        swapNum(xmin,xmax);
    if(ymin>ymax)
        swapNum(ymin,ymax);
    int rx=(int)round((double)(xmax-xmin)/2);
    int ry=(int)round((double)(ymax-ymin)/2);
    int xc=rx+xmin;
    int yc=ry+ymin;
    int x=0,y=ry;
    /*QPainter *painter=new QPainter(&temppaintboard);
    painter->setPen(*drawpen);*/
    QPainter *painter=setPainter(drawpen);
    if(ry==0)
    {
        for(;x<=rx;x++)
        {
            painter->drawPoint(xc+x,yc);
            painter->drawPoint(xc-x,yc);
        }
        QWidget::update();
        delete painter;
        return;
    }
    double sqrx=(double)rx*rx;
    double sqry=(double)ry*ry;
    painter->drawPoint(xc+x,yc+y); //绘制第一个点及其对称点
    painter->drawPoint(xc-x,yc+y);
    painter->drawPoint(xc+x,yc-y);
    painter->drawPoint(xc-x,yc-y);
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
        painter->drawPoint(xc+x,yc+y);
        painter->drawPoint(xc-x,yc+y);
        painter->drawPoint(xc+x,yc-y);
        painter->drawPoint(xc-x,yc-y);
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
        painter->drawPoint(xc+x,yc+y);
        painter->drawPoint(xc-x,yc+y);
        painter->drawPoint(xc+x,yc-y);
        painter->drawPoint(xc-x,yc-y);
    }
    QWidget::update();
    delete painter;
}

void myCanvas::createEllipse_ui()
{
    int xmin=pointvector[0].x();
    int ymin=pointvector[0].y();
    int xmax=pointvector[1].x();
    int ymax=pointvector[1].y();
    if(xmin>xmax)
        swapNum(xmin,xmax);
    if(ymin>ymax)
        swapNum(ymin,ymax);
    int rx=(int)round((double)(xmax-xmin)/2);
    int ry=(int)round((double)(ymax-ymin)/2);
    int xc=rx+xmin;
    int yc=ry+ymin;
    int id=generateID();
    drawEllipse(id,xc,yc,rx,ry);
    pointvector.clear();
    changeSequence(id,true);
}

void myCanvas::drawtempRound_ui(QPoint &point)
{
    int xmin=pointvector[0].x();
    int ymin=pointvector[0].y();
    int xmax=point.x();
    int ymax=point.y();
    if(xmin>xmax)
        swapNum(xmin,xmax);
    if(ymin>ymax)
        swapNum(ymin,ymax);
    int rx=(int)round((double)(xmax-xmin)/2);
    int ry=(int)round((double)(ymax-ymin)/2);
    int xc=rx+xmin;
    int yc=ry+ymin;
    int r=max(rx,ry);
    QPainter *painter=new QPainter(&temppaintboard);
    painter->setPen(*drawpen);
    int x=0,y=r;
    double d=1.25-(double)r; //设置决策参数初始值
    drawRoundPoint(xc,yc,x,y,painter); //画第一个点
    while(x<y) //在1/8圆弧内
    {
        if(d<0)
        {
            d=d+2*x+3;
            x++;
        }
        else
        {
            d=d+2*(x-y)+5;
            x++;
            y--;
        }
        drawRoundPoint(xc,yc,x,y,painter);
    }
    QWidget::update();
    delete painter;
}

void myCanvas::createRound_ui()
{
    int xmin=pointvector[0].x();
    int ymin=pointvector[0].y();
    int xmax=pointvector[1].x();
    int ymax=pointvector[1].y();
    if(xmin>xmax)
        swapNum(xmin,xmax);
    if(ymin>ymax)
        swapNum(ymin,ymax);
    int rx=(int)round((double)(xmax-xmin)/2);
    int ry=(int)round((double)(ymax-ymin)/2);
    int xc=rx+xmin;
    int yc=ry+ymin;
    int r=max(rx,ry);
    int id=generateID();
    drawRound(id,xc,yc,r);
    pointvector.clear();
    changeSequence(id,true);
}

void myCanvas::paintGraphDot(QVector <QPoint> &drawpointset)
{
    QPainter *painter=setPainter(drawpen);
    int size=drawpointset.size();
    for(int i=0;i<size;i++)
        painter->drawPoint(drawpointset[i]);
    delete painter;
}

void myCanvas::editCanvas(QPoint &point,moveDirection direction,bool final)
{
    if(direction==Diagonal)
    {
        int dx=point.x()-pointvector[0].x();
        int dy=point.y()-pointvector[0].y();
        tempwidth=width+dx;
        if(tempwidth>maxwidth)
            tempwidth=maxwidth;
        else if(tempwidth<minwidth)
            tempwidth=minwidth;
        tempheight=height+dy;
        if(tempheight>maxheight)
            tempheight=maxheight;
        else if(tempheight<minheight)
            tempheight=minheight;
    }
    else if(direction==Vertical)
    {
        int dy=point.y()-pointvector[0].y();
        tempheight=height+dy;
        if(tempheight>maxheight)
            tempheight=maxheight;
        else if(tempheight<minheight)
            tempheight=minheight;
    }
    else if(direction==Horizontal)
    {
        int dx=point.x()-pointvector[0].x();
        tempwidth=width+dx;
        if(tempwidth>maxwidth)
            tempwidth=maxwidth;
        else if(tempwidth<minwidth)
            tempwidth=minwidth;
    }
    emit send_resCan_signal(); //发送修改画布大小信号
    statusinfo=tr("画布大小: ");
    statusinfo+=QString::number(tempwidth);
    statusinfo+='*';
    statusinfo+=QString::number(tempheight);
    statusinfo+=tr("像素");
    emit sendStatusbarinfo(statusinfo); //发送状态栏展示信息信号
    resetCanvasEdit(tempwidth,tempheight,final);

}

void myCanvas::drawtempCurve_ui(QPoint &point)
{
    QVector <QPoint> tempvector;
    if(curvealgorithm==Bezier)
    {
        QPainter *painter=setPainter(disclipen);
        for(int i=0;i<curvepointnum-2;i++)
            painter->drawLine(pointvector[i],pointvector[i+1]);
        painter->drawLine(pointvector[curvepointnum-1],point);
        painter->drawLine(pointvector[curvepointnum-2],point);
        delete painter;

        QPoint lastpoint=pointvector[curvepointnum-1];
        pointvector.pop_back();
        pointvector.push_back(point);
        pointvector.push_back(lastpoint);
        QPainter *painter2=setPainter(drawpen);
        drawCurve_Bezier(pointvector,curvepointnum,painter2,tempvector);
        delete painter2;
        QPainter *painter3=setPainter(pointpen);
        for(int i=0;i<=curvepointnum;i++)
            paintPoint(pointvector[i].x(),pointvector[i].y(),painter3,solidpointpen);
        delete painter3;
        pointvector.pop_back();
        pointvector.pop_back();
        pointvector.push_back(lastpoint);
    }
    else
    {
        QPainter *painter=setPainter(disclipen);
        for(int i=0;i<curvepointnum-1;i++)
        {
            painter->drawLine(pointvector[i],pointvector[i+1]);
        }
        painter->drawLine(pointvector[curvepointnum-1],point);
        delete painter;
        QVector <QPoint> tempvector;
        pointvector.push_back(point);
        QPainter *painter2=setPainter(drawpen);
        drawCurve_Bspline(pointvector,curvepointnum,painter2,tempvector);
        delete painter2;
        QPainter *painter3=setPainter(pointpen);
        for(int i=0;i<=curvepointnum;i++)
            paintPoint(pointvector[i].x(),pointvector[i].y(),painter3,solidpointpen);
        delete painter3;
        pointvector.pop_back();
    }
    QWidget::update();
}

void myCanvas::createCurve_ui()
{
    if(curvepointnum<2)
    {
        pointvector.clear();
        curvepointnum=0;
        curvelinedrawed=false;
        QWidget::update();
        return;
    }
    int id=generateID();
    drawCurve(id,pointvector,curvepointnum,curvealgorithm);
    pointvector.clear();
    curvepointnum=0;
    curvelinedrawed=false;
    changeSequence(id,true);
}

bool myCanvas::closeEnough_drawpointset(QVector <QPoint> &drawpointset,int pivotx,int pivoty)
{
    int size=drawpointset.size();
    for(int i=0;i<size;i++)
    {
        if(closeEnough_point(drawpointset[i],pivotx,pivoty,3,false))
            return true;
    }
    return false;
}

bool myCanvas::closeEnough_graph(int nowid,QPoint &point)
{
    map <int,myGraph>::iterator currentgraph;
    currentgraph=graphset->find(nowid);
    if(currentgraph==graphset->end())
    {
        qDebug() << "error in selectGraph: not found id in graphset";
        return false;
    }
    int x=point.x();
    int y=point.y();
    switch (currentgraph->second.shape)
    {
        case Line:
        {
            map <int,myLine>::iterator currentline;
            currentline=lineset->find(nowid);
            if(currentline==lineset->end())
            {
                qDebug() << "error in closeEnough_graph: not found id in lineset";
                break;
            }
            if(closeEnough_drawpointset(currentline->second.drawpointset,x,y))
            {
                return true;
            }
            break;
        }
        case Polygon:
        {
            map <int,myPolygon>::iterator currentpolygon;
            currentpolygon=polygonset->find(nowid);
            if(currentpolygon==polygonset->end())
            {
                qDebug() << "error in closeEnough_graph: not found id in polygonset";
                break;
            }
            if(closeEnough_drawpointset(currentpolygon->second.drawpointset,x,y))
            {
                return true;
            }
            break;
        }
        case Ellipse:
        {
            map <int,myEllipse>::iterator currentellipse;
            currentellipse=ellipseset->find(nowid);
            if(currentellipse==ellipseset->end())
            {
                qDebug() << "error in closeEnough_graph: not found id in ellipseset";
                break;
            }
            if(closeEnough_drawpointset(currentellipse->second.drawpointset,x,y))
            {
                return true;
            }
            break;
        }
        case Round:
        {
            map <int,myRound>::iterator currentround;
            currentround=roundset->find(nowid);
            if(currentround==roundset->end())
            {
                qDebug() << "error in closeEnough_graph: not found id in roundset";
                break;
            }
            if(closeEnough_drawpointset(currentround->second.drawpointset,x,y))
            {
                return true;
            }
            break;
        }
        case Curve:
        {
            map <int,myCurve>::iterator currentcurve;
            currentcurve=curveset->find(nowid);
            if(currentcurve==curveset->end())
            {
                qDebug() << "error in closeEnough_graph: not found id in curveset";
                break;
            }
            if(closeEnough_drawpointset(currentcurve->second.drawpointset,x,y))
            {
                return true;
            }
            break;
        }
        default:
            break;
    }
    return false;
}

void myCanvas::selectGraph(QPoint &point)
{
    int x=point.x();
    int y=point.y();
    bool select=false;
    list <int>::iterator current;
    current=sequence.begin();
    for(;current!=sequence.end();current++)
    {
        int nowid=*current;
        map <int,myGraph>::iterator currentgraph;
        currentgraph=graphset->find(nowid);
        if(currentgraph==graphset->end())
        {
            qDebug() << "error in selectGraph: not found id in graphset";
            continue;
        }
        switch (currentgraph->second.shape)
        {
            case Line:
            {
                map <int,myLine>::iterator currentline;
                currentline=lineset->find(nowid);
                if(currentline==lineset->end())
                {
                    qDebug() << "error in selectGraph: not found id in lineset";
                    break;
                }
                if(closeEnough_drawpointset(currentline->second.drawpointset,x,y))
                {
                    selectid=nowid;
                    select=true;
                    break;
                }
                break;
            }
            case Polygon:
            {
                map <int,myPolygon>::iterator currentpolygon;
                currentpolygon=polygonset->find(nowid);
                if(currentpolygon==polygonset->end())
                {
                    qDebug() << "error in selectGraph: not found id in polygonset";
                    break;
                }
                if(closeEnough_drawpointset(currentpolygon->second.drawpointset,x,y))
                {
                    selectid=nowid;
                    select=true;
                    break;
                }
                break;
            }
            case Ellipse:
            {
                map <int,myEllipse>::iterator currentellipse;
                currentellipse=ellipseset->find(nowid);
                if(currentellipse==ellipseset->end())
                {
                    qDebug() << "error in selectGraph: not found id in ellipseset";
                    break;
                }
                if(closeEnough_drawpointset(currentellipse->second.drawpointset,x,y))
                {
                    selectid=nowid;
                    select=true;
                    break;
                }
                break;
            }
            case Round:
            {
                map <int,myRound>::iterator currentround;
                currentround=roundset->find(nowid);
                if(currentround==roundset->end())
                {
                    qDebug() << "error in selectGraph: not found id in roundset";
                    break;
                }
                if(closeEnough_drawpointset(currentround->second.drawpointset,x,y))
                {
                    selectid=nowid;
                    select=true;
                    break;
                }
                break;
            }
            case Curve:
            {
                map <int,myCurve>::iterator currentcurve;
                currentcurve=curveset->find(nowid);
                if(currentcurve==curveset->end())
                {
                    qDebug() << "error in selectGraph: not found id in curveset";
                    break;
                }
                if(closeEnough_drawpointset(currentcurve->second.drawpointset,x,y))
                {
                    selectid=nowid;
                    select=true;
                    break;
                }
                break;
            }
            default:
                break;
        }
        if(select)
            break;
    }
    if(!select)
        selectid=0;
    else
        changeSequence(selectid,false);
    resetCanvas(width,height,true);
}

void myCanvas::paintPoint(int x,int y,QPainter *painter,QPen &pen)
{
    painter->setPen(pen);
    painter->drawPoint(x,y);
    painter->setPen(*pointpen);
    painter->drawLine(x-2,y-2,x-2,y+2);
    painter->drawLine(x+2,y-2,x+2,y+2);
    painter->drawLine(x-2,y-2,x+2,y-2);
    painter->drawLine(x-2,y+2,x+2,y+2);
}

void myCanvas::paintOutline_rect(Rectangle &rect,QPainter *painter,QPen &temppen)
{
    painter->setPen(*disclipen);
    painter->drawLine(rect.leftup,rect.leftdown);
    painter->drawLine(rect.leftup,rect.rightup);
    painter->drawLine(rect.leftdown,rect.rightdown);
    painter->drawLine(rect.rightup,rect.rightdown);
    paintPoint(rect.leftup.x(),rect.leftup.y(),painter,temppen);
    paintPoint(rect.leftdown.x(),rect.leftdown.y(),painter,temppen);
    paintPoint(rect.rightup.x(),rect.rightup.y(),painter,temppen);
    paintPoint(rect.rightdown.x(),rect.rightdown.y(),painter,temppen);
    paintPoint(rect.center.x(),rect.center.y(),painter,temppen);
}

void myCanvas::drawOutline(int nowid,Shape shape)
{
    if(!drawoutline)
        return;
    QPainter *painter=setPainter(pointpen);
    QPen temppen;
    temppen.setColor(QColor(255,255,255));
    temppen.setWidth(3);
    temppen.setStyle(Qt::SolidLine);
    temppen.setCapStyle(Qt::SquareCap);
    switch (shape)
    {
        case Line:
        {
            map <int,myLine>::iterator currentline;
            currentline=lineset->find(nowid);
            if(currentline==lineset->end())
            {
                qDebug() << "error in drawOutline: not found id in lineset";
                break;
            }
            /*painter->drawPoint(currentline->second.x0,currentline->second.y0);
            painter->drawPoint(currentline->second.x1,currentline->second.y1);*/

            //paintPoint(currentline->second.x0,currentline->second.y0,painter,temppen);
            //paintPoint(currentline->second.x1,currentline->second.y1,painter,temppen);
            paintOutline_rect(currentline->second.rect,painter,temppen);
            break;
        }
        case Polygon:
        {
            map <int,myPolygon>::iterator currentpolygon;
            currentpolygon=polygonset->find(nowid);
            if(currentpolygon==polygonset->end())
            {
                qDebug() << "error in drawOutline: not found id in polygonset";
                break;
            }
            for(int i=0;i<currentpolygon->second.pointnum;i++)
                //painter->drawPoint(currentpolygon->second.pointset[2*i],currentpolygon->second.pointset[2*i+1]);
                paintPoint(currentpolygon->second.pointset[2*i],currentpolygon->second.pointset[2*i+1],painter,temppen);

            paintOutline_rect(currentpolygon->second.rect,painter,temppen);
            break;
        }
        case Ellipse:
        {
            map <int,myEllipse>::iterator currentellipse;
            currentellipse=ellipseset->find(nowid);
            if(currentellipse==ellipseset->end())
            {
                qDebug() << "error in drawOutline: not found id in ellipseset";
                break;
            }
            //painter->drawPoint(currentellipse->second.xc,currentellipse->second.yc);

            //paintPoint(currentellipse->second.xc,currentellipse->second.yc,painter,temppen);
            paintOutline_rect(currentellipse->second.rect,painter,temppen);
            break;
        }
        case Round:
        {
            map <int,myRound>::iterator currentround;
            currentround=roundset->find(nowid);
            if(currentround==roundset->end())
            {
                qDebug() << "error in drawOutline: not found id in roundset";
                break;
            }
            //painter->drawPoint(currentround->second.xc,currentround->second.yc);

            //paintPoint(currentround->second.xc,currentround->second.yc,painter,temppen);
            paintOutline_rect(currentround->second.rect,painter,temppen);
            break;
        }
        case Curve:
        {
            map <int,myCurve>::iterator currentcurve;
            currentcurve=curveset->find(nowid);
            if(currentcurve==curveset->end())
            {
                qDebug() << "error in drawOutline: not found id in curveset";
                break;
            }
            int num=currentcurve->second.pointnum;
            for(int i=0;i<num-1;i++)
            {
                //painter->drawPoint(currentcurve->second.pointset[i]);
                painter->setPen(*disclipen);
                painter->drawLine(currentcurve->second.pointset[i],currentcurve->second.pointset[i+1]);
                paintPoint(currentcurve->second.pointset[i].x(),currentcurve->second.pointset[i].y(),painter,temppen);
            }
            paintPoint(currentcurve->second.pointset[num-1].x(),currentcurve->second.pointset[num-1].y(),painter,temppen);

            break;
        }
        default:
            break;
    }
    delete painter;
}

void myCanvas::translate_ui(QPoint &point)
{
    int dx=point.x()-lastpoint.x();
    int dy=point.y()-lastpoint.y();
    editid=selectid;
    translate(selectid,dx,dy);
}

void myCanvas::deleteGraph()
{
    if(assertion())
        return;
    if(selectid==0)
        return;
    QMessageBox message(QMessageBox::Warning,"确认删除",tr("确认删除所选图元?"),QMessageBox::Yes | QMessageBox::No,NULL);
    if(message.exec()==QMessageBox::Yes)
    {
        hasedited=true;
        map <int,myGraph>::iterator currentgraph;
        currentgraph=graphset->find(selectid);
        Shape shape=currentgraph->second.shape;
        graphset->erase(selectid);
        switch (shape)
        {
            case Line:
            {
                lineset->erase(selectid);
                break;
            }
            case Polygon:
            {
                polygonset->erase(selectid);
                break;
            }
            case Ellipse:
            {
                ellipseset->erase(selectid);
                break;
            }
            case Round:
            {
                roundset->erase(selectid);
                break;
            }
            case Curve:
            {
                curveset->erase(selectid);
                break;
            }
            default:
                break;
        }
        list <int>::iterator current;
        for(current=sequence.begin();current!=sequence.end();current++)
        {
            if(*current==selectid)
            {
                sequence.erase(current);
                break;
            }
        }
        selectid=0;
        resetCanvas(width,height,true);
    }
}

void myCanvas::paintCenterPoint()
{
    QPainter *painter=setPainter(centerpen);
    painter->drawPoint(centerpoint);
    QWidget::update();
    delete painter;
}

void myCanvas::closeEnough_selectcenter(QPoint &point)
{
    map <int,myGraph>::iterator currentgraph;
    currentgraph=graphset->find(selectid);
    Shape shape=currentgraph->second.shape;
    switch (shape)
    {
        case Line:
        {
            map <int,myLine>::iterator currentline;
            currentline=lineset->find(selectid);
            if(closeEnough_point(point,currentline->second.rect.center.x(),currentline->second.rect.center.y(),4,false))
            {
                centerpoint=currentline->second.rect.center;
                break;
            }
            break;
        }
        case Polygon:
        {
            map <int,myPolygon>::iterator currentpolygon;
            currentpolygon=polygonset->find(selectid);
            if(closeEnough_point(point,currentpolygon->second.rect.center.x(),currentpolygon->second.rect.center.y(),4,false))
            {
                centerpoint=currentpolygon->second.rect.center;
                break;
            }
            break;
        }
        case Ellipse:
        {
            map <int,myEllipse>::iterator currentellipse;
            currentellipse=ellipseset->find(selectid);
            if(closeEnough_point(point,currentellipse->second.rect.center.x(),currentellipse->second.rect.center.y(),4,false))
            {
                centerpoint=currentellipse->second.rect.center;
                break;
            }
            break;
        }
        case Round:
        {
            map <int,myRound>::iterator currentround;
            currentround=roundset->find(selectid);
            if(closeEnough_point(point,currentround->second.rect.center.x(),currentround->second.rect.center.y(),4,false))
            {
                centerpoint=currentround->second.rect.center;
                break;
            }
            break;
        }
        case Curve:
        {
            map <int,myCurve>::iterator currentcurve;
            currentcurve=curveset->find(selectid);
            if(closeEnough_point(point,currentcurve->second.rect.center.x(),currentcurve->second.rect.center.y(),4,false))
            {
                centerpoint=currentcurve->second.rect.center;
                break;
            }
            break;
        }
        default:
            break;
    }
}

int myCanvas::rotateAngle_ui(QPoint &point)
{
    if(centerpoint==lastpoint||lastpoint==point||centerpoint==point) //任意两个点重合，返回0度
        return 0;
    int x1=centerpoint.x();
    int y1=centerpoint.y();
    int x2=lastpoint.x()-x1;
    int y2=lastpoint.y()-y1;
    int x3=point.x()-x1;
    int y3=point.y()-y1;
    x1=0,y1=0;
    bool flip=false;
    if(y1==y2)
    {
        if(x2>x1)
        {
            if(y3>y1)
                flip=true;
        }
        else
        {
            if(y3<y1)
                flip=true;
        }
    }
    else if(x1==x2)
    {
        if(y2>y1)
        {
            if(x3<x1)
                flip=true;
        }
        else
        {
            if(x3>x1)
                flip=true;
        }
    }
    else if((x2>x1&&y2>y1)||(x2>x1&&y2<y1))
    {
        double k=(double)y2/(double)x2;
        if(y3>k*x3)
            flip=true;
    }
    else if((x2<x1&&y2>y1)||(x2<x1&&y2<y1))
    {
        double k=(double)y2/(double)x2;
        if(y3<k*x3)
            flip=true;
    }
    double a=sqrt((x3-x2)*(x3-x2)+(y3-y2)*(y3-y2));
    double b=sqrt(x2*x2+y2*y2);
    double c=sqrt(x3*x3+y3*y3);
    double cosA=(b*b+c*c-a*a)/(2*b*c); //利用余弦定理计算夹角
    double A=acos(cosA);
    int angle=(int)round(A*180/M_PI);
    if(!flip) //需要翻转
        angle=360-angle;
    return angle;
}

void myCanvas::rotate_ui(QPoint &point,bool first,bool final)
{
    int show=0;
    if(first)
        resetCanvas(width,height,true);
    else
    {
        int angle=rotateAngle_ui(point);
        if(lastangle!=0)
        {
            editid=selectid;
            rotate(selectid,centerpoint.x(),centerpoint.y(),lastangle);
        }
        if(angle!=0)
        {
            editid=selectid;
            rotate(selectid,centerpoint.x(),centerpoint.y(),angle);
        }
        lastangle=-angle;
        show=angle;
    }
    if(!final)
    {
        paintCenterPoint();
        int a1=centerpoint.x();
        int a2=centerpoint.y();
        int b1=point.x();
        int b2=point.y();
        QPen temppen;
        temppen.setColor(QColor(255,255,255));
        temppen.setWidth(3);
        temppen.setStyle(Qt::SolidLine);
        temppen.setCapStyle(Qt::SquareCap);
        QPainter *painter=setPainter(disclipen);
        painter->drawLine(a1,a2,b1,b2);
        disclipen->setColor(QColor(255,0,0));
        painter->setPen(*disclipen);
        painter->drawLine(centerpoint,lastpoint);
        disclipen->setColor(QColor(0,128,255));
        painter->setPen(*disclipen);
        map <int,myGraph>::iterator currentgraph;
        currentgraph=graphset->find(selectid);
        Shape shape=currentgraph->second.shape;
        switch (shape)
        {
            case Line:
            {
                map <int,myLine>::iterator currentline;
                currentline=lineset->find(selectid);
                painter->drawLine(b1,b2,currentline->second.x0,currentline->second.y0);
                painter->drawLine(b1,b2,currentline->second.x1,currentline->second.y1);
                paintPoint(currentline->second.x0,currentline->second.y0,painter,temppen);
                paintPoint(currentline->second.x1,currentline->second.y1,painter,temppen);
                break;
            }
            case Polygon:
            {
                map <int,myPolygon>::iterator currentpolygon;
                currentpolygon=polygonset->find(selectid);
                for(int i=0;i<currentpolygon->second.pointnum;i++)
                    painter->drawLine(b1,b2,currentpolygon->second.pointset[2*i],currentpolygon->second.pointset[2*i+1]);
                for(int i=0;i<currentpolygon->second.pointnum;i++)
                    paintPoint(currentpolygon->second.pointset[2*i],currentpolygon->second.pointset[2*i+1],painter,temppen);
                break;
            }
            case Ellipse:
            {
                map <int,myEllipse>::iterator currentellipse;
                currentellipse=ellipseset->find(selectid);
                int x1=(int)round((double)(currentellipse->second.rect.leftup.x()+currentellipse->second.rect.rightup.x())/(double)2);
                int y1=(int)round((double)(currentellipse->second.rect.leftup.y()+currentellipse->second.rect.rightup.y())/(double)2);
                int x2=(int)round((double)(currentellipse->second.rect.leftup.x()+currentellipse->second.rect.leftdown.x())/(double)2);
                int y2=(int)round((double)(currentellipse->second.rect.leftup.y()+currentellipse->second.rect.leftdown.y())/(double)2);
                int x3=(int)round((double)(currentellipse->second.rect.rightup.x()+currentellipse->second.rect.rightdown.x())/(double)2);
                int y3=(int)round((double)(currentellipse->second.rect.rightup.y()+currentellipse->second.rect.rightdown.y())/(double)2);
                int x4=(int)round((double)(currentellipse->second.rect.leftdown.x()+currentellipse->second.rect.rightdown.x())/(double)2);
                int y4=(int)round((double)(currentellipse->second.rect.leftdown.y()+currentellipse->second.rect.rightdown.y())/(double)2);
                painter->drawLine(b1,b2,x1,y1);
                painter->drawLine(b1,b2,x2,y2);
                painter->drawLine(b1,b2,x3,y3);
                painter->drawLine(b1,b2,x4,y4);
                paintPoint(x1,y1,painter,temppen);
                paintPoint(x2,y2,painter,temppen);
                paintPoint(x3,y3,painter,temppen);
                paintPoint(x4,y4,painter,temppen);
                break;
            }
            case Round:
            {
                map <int,myRound>::iterator currentround;
                currentround=roundset->find(selectid);
                int x1=(int)round((double)(currentround->second.rect.leftup.x()+currentround->second.rect.rightup.x())/(double)2);
                int y1=(int)round((double)(currentround->second.rect.leftup.y()+currentround->second.rect.rightup.y())/(double)2);
                int x2=(int)round((double)(currentround->second.rect.leftup.x()+currentround->second.rect.leftdown.x())/(double)2);
                int y2=(int)round((double)(currentround->second.rect.leftup.y()+currentround->second.rect.leftdown.y())/(double)2);
                int x3=(int)round((double)(currentround->second.rect.rightup.x()+currentround->second.rect.rightdown.x())/(double)2);
                int y3=(int)round((double)(currentround->second.rect.rightup.y()+currentround->second.rect.rightdown.y())/(double)2);
                int x4=(int)round((double)(currentround->second.rect.leftdown.x()+currentround->second.rect.rightdown.x())/(double)2);
                int y4=(int)round((double)(currentround->second.rect.leftdown.y()+currentround->second.rect.rightdown.y())/(double)2);
                painter->drawLine(b1,b2,x1,y1);
                painter->drawLine(b1,b2,x2,y2);
                painter->drawLine(b1,b2,x3,y3);
                painter->drawLine(b1,b2,x4,y4);
                paintPoint(x1,y1,painter,temppen);
                paintPoint(x2,y2,painter,temppen);
                paintPoint(x3,y3,painter,temppen);
                paintPoint(x4,y4,painter,temppen);
                break;
            }
            case Curve:
            {
                map <int,myCurve>::iterator currentcurve;
                currentcurve=curveset->find(selectid);
                painter->drawLine(QPoint(b1,b2),currentcurve->second.pointset[0]);
                painter->drawLine(QPoint(b1,b2),currentcurve->second.pointset[currentcurve->second.pointnum-1]);
                paintPoint(currentcurve->second.pointset[0].x(),currentcurve->second.pointset[0].y(),painter,temppen);
                paintPoint(currentcurve->second.pointset[currentcurve->second.pointnum-1].x(),currentcurve->second.pointset[currentcurve->second.pointnum-1].y(),painter,temppen);
                break;
            }
            default:
                break;
        }
        paintPoint(point.x(),point.y(),painter,temppen);
        paintPoint(lastpoint.x(),lastpoint.y(),painter,temppen);
        delete painter;
    }
    QWidget::update();
    statusinfo=tr("顺时针旋转角度: ");
    statusinfo+=QString::number(show);
    statusinfo+=tr("°");
    emit sendStatusbarinfo(statusinfo);
}

bool myCanvas::selectGraphPoint(QPoint &point)
{
    map <int,myGraph>::iterator currentgraph;
    currentgraph=graphset->find(selectid);
    Shape shape=currentgraph->second.shape;
    int dis=3;
    switch (shape)
    {
        case Line:
        {
            uiline=lineset->find(selectid);
            if(closeEnough_point(point,uiline->second.x0,uiline->second.y0,dis,false))
            {
                controlpoint=0;
                controlshape=Line;
                pivotpoint.setX(uiline->second.x0);
                pivotpoint.setY(uiline->second.y0);
                return true;
            }
            else if(closeEnough_point(point,uiline->second.x1,uiline->second.y1,dis,false))
            {
                controlpoint=1;
                controlshape=Line;
                pivotpoint.setX(uiline->second.x1);
                pivotpoint.setY(uiline->second.y1);
                return true;
            }
            break;
        }
        case Polygon:
        {
            uipolygon=polygonset->find(selectid);
            int pointnum=uipolygon->second.pointnum;
            int *set=uipolygon->second.pointset;
            for(int i=0;i<pointnum;i++)
            {
                if(closeEnough_point(point,set[2*i],set[2*i+1],dis,false))
                {
                    controlpoint=i;
                    controlshape=Polygon;
                    pivotpoint.setX(set[2*i]);
                    pivotpoint.setY(set[2*i+1]);
                    return true;
                }
            }
            break;
        }
        case Ellipse:
        {
            uiellipse=ellipseset->find(selectid);
            if(closeEnough_point(point,uiellipse->second.rect.leftup.x(),uiellipse->second.rect.leftup.y(),dis,false))
            {
                controlpoint=0; //左上角
                controlshape=Ellipse;
                pivotpoint.setX(uiellipse->second.rect.leftup.x()); //记录基准点（右下角当前坐标）
                pivotpoint.setY(uiellipse->second.rect.leftup.y());
                return true;
            }
            else if(closeEnough_point(point,uiellipse->second.rect.leftdown.x(),uiellipse->second.rect.leftdown.y(),dis,false))
            {
                controlpoint=1;
                controlshape=Ellipse;
                pivotpoint.setX(uiellipse->second.rect.leftdown.x());
                pivotpoint.setY(uiellipse->second.rect.leftdown.y());
                return true;
            }
            else if(closeEnough_point(point,uiellipse->second.rect.rightup.x(),uiellipse->second.rect.rightup.y(),dis,false))
            {
                controlpoint=2;
                controlshape=Ellipse;
                pivotpoint.setX(uiellipse->second.rect.rightup.x());
                pivotpoint.setY(uiellipse->second.rect.rightup.y());
                return true;
            }
            else if(closeEnough_point(point,uiellipse->second.rect.rightdown.x(),uiellipse->second.rect.rightdown.y(),dis,false))
            {
                controlpoint=3;
                controlshape=Ellipse;
                pivotpoint.setX(uiellipse->second.rect.rightdown.x());
                pivotpoint.setY(uiellipse->second.rect.rightdown.y());
                return true;
            }
            break;
        }
        case Round:
        {
            uiround=roundset->find(selectid);
            if(closeEnough_point(point,uiround->second.rect.rightup.x(),uiround->second.rect.rightup.y(),dis,false))
            {
                controlpoint=2;
                controlshape=Round;
                pivotpoint.setX(uiround->second.rect.rightup.x());
                pivotpoint.setY(uiround->second.rect.rightup.y());
                return true;
            }
            else if(closeEnough_point(point,uiround->second.rect.rightdown.x(),uiround->second.rect.rightdown.y(),dis,false))
            {
                controlpoint=3;
                controlshape=Round;
                pivotpoint.setX(uiround->second.rect.rightdown.x());
                pivotpoint.setY(uiround->second.rect.rightdown.y());
                return true;
            }
            else if(closeEnough_point(point,uiround->second.rect.leftup.x(),uiround->second.rect.leftup.y(),dis,false))
            {
                controlpoint=0;
                controlshape=Round;
                pivotpoint.setX(uiround->second.rect.leftup.x());
                pivotpoint.setY(uiround->second.rect.leftup.y());
                return true;
            }
            else if(closeEnough_point(point,uiround->second.rect.leftdown.x(),uiround->second.rect.leftdown.y(),dis,false))
            {
                controlpoint=1;
                controlshape=Round;
                pivotpoint.setX(uiround->second.rect.leftdown.x());
                pivotpoint.setY(uiround->second.rect.leftdown.y());
                return true;
            }
            break;
        }
        case Curve:
        {
            uicurve=curveset->find(selectid);
            int pointnum=uicurve->second.pointnum;
            for(int i=0;i<pointnum;i++)
            {
                if(closeEnough_point(point,uicurve->second.pointset[i].x(),uicurve->second.pointset[i].y(),dis,false))
                {
                    controlpoint=i;
                    controlshape=Curve;
                    pivotpoint.setX(uicurve->second.pointset[i].x());
                    pivotpoint.setY(uicurve->second.pointset[i].y());
                    return true;
                }
            }
            break;
        }
        default:
            break;
    }
    return false;
}

void myCanvas::editGraph_ui(QPoint &point)
{
    if(controlshape==Line)
    {
        int dx=point.x()-lastpoint.x();
        int dy=point.y()-lastpoint.y();
        int newx=pivotpoint.x()+dx;
        int newy=pivotpoint.y()+dy;
        if(controlpoint==0)
        {
            uiline->second.x0=newx;
            uiline->second.y0=newy;
        }
        else
        {
            uiline->second.x1=newx;
            uiline->second.y1=newy;
        }
    }
    else if(controlshape==Polygon)
    {
        int dx=point.x()-lastpoint.x();
        int dy=point.y()-lastpoint.y();
        int newx=pivotpoint.x()+dx;
        int newy=pivotpoint.y()+dy;
        uipolygon->second.pointset[2*controlpoint]=newx;
        uipolygon->second.pointset[2*controlpoint+1]=newy;
    }
    else if(controlshape==Ellipse)
    {
        int dx=point.x()-lastpoint.x();
        int dy=point.y()-lastpoint.y();
        int newx=pivotpoint.x()+dx;
        int newy=pivotpoint.y()+dy;
        int xmin,ymin;
        int xmax=newx,ymax=newy;
        if(controlpoint==0)
        {
            xmin=uiellipse->second.rect.rightdown.x();
            ymin=uiellipse->second.rect.rightdown.y();
        }
        else if(controlpoint==1)
        {
            xmin=uiellipse->second.rect.rightup.x();
            ymin=uiellipse->second.rect.rightup.y();
        }
        else if(controlpoint==2)
        {
            xmin=uiellipse->second.rect.leftdown.x();
            ymin=uiellipse->second.rect.leftdown.y();
        }
        else
        {
            xmin=uiellipse->second.rect.leftup.x();
            ymin=uiellipse->second.rect.leftup.y();
        }
        settledpoint.setX(xmin);
        settledpoint.setY(ymin);

        int xc=(int)round(((double)xmin+(double)xmax)/(double)2);
        int yc=(int)round(((double)ymin+(double)ymax)/(double)2);

        uiellipse->second.ellipsematrix.rotatePoint(xc,yc,xmin,ymin,xmax,ymax);

        if(controlpoint==0)
        {
            prevrect.leftup.setX(xmax);
            prevrect.leftup.setY(ymax);
            prevrect.rightdown.setX(xmin);
            prevrect.rightdown.setY(ymin);
        }
        else if(controlpoint==1)
        {
            prevrect.leftdown.setX(xmax);
            prevrect.leftdown.setY(ymax);
            prevrect.rightup.setX(xmin);
            prevrect.rightup.setY(ymin);
        }
        else if(controlpoint==2)
        {
            prevrect.rightup.setX(xmax);
            prevrect.rightup.setY(ymax);
            prevrect.leftdown.setX(xmin);
            prevrect.leftdown.setY(ymin);
        }
        else
        {
            prevrect.rightdown.setX(xmax);
            prevrect.rightdown.setY(ymax);
            prevrect.leftup.setX(xmin);
            prevrect.leftup.setY(ymin);
        }

        if(xmin>xmax)
            swapNum(xmin,xmax);
        if(ymin>ymax)
            swapNum(ymin,ymax);

        int rx=(int)round((double)(xmax-xmin)/2);
        int ry=(int)round((double)(ymax-ymin)/2);
        uiellipse->second.xc=xc;
        uiellipse->second.yc=yc;
        uiellipse->second.rx=rx;
        uiellipse->second.ry=ry;

        uiellipse->second.ellipsematrix.rebuildEllipsematrix(xc,yc);
    }
    else if(controlshape==Round)
    {
        int dx=point.x()-lastpoint.x();
        int dy=point.y()-lastpoint.y();
        int newx,newy;
        newx=pivotpoint.x()+dx;
        newy=pivotpoint.y()+dy;
        int xmin,ymin;
        int xmax=newx,ymax=newy;
        if(controlpoint==0)
        {
            xmin=uiround->second.rect.rightdown.x();
            ymin=uiround->second.rect.rightdown.y();
        }
        else if(controlpoint==1)
        {
            xmin=uiround->second.rect.rightup.x();
            ymin=uiround->second.rect.rightup.y();
        }
        else if(controlpoint==2)
        {
            xmin=uiround->second.rect.leftdown.x();
            ymin=uiround->second.rect.leftdown.y();
        }
        else
        {
            xmin=uiround->second.rect.leftup.x();
            ymin=uiround->second.rect.leftup.y();
        }
        settledpoint.setX(xmin);
        settledpoint.setY(ymin);

        QPoint point(xmax,ymax);
        QPoint projectpoint;
        if(settledpoint==pivotpoint)
        {
            int a1,b1,a2,b2;
            int angle=uiround->second.roundmatrix.getAngle();
            angle+=45;
            if(angle%180==0)
            {
                a1=settledpoint.x();
                b1=1;
                a2=a1;
                b2=2;
            }
            else if(angle%90==0)
            {
                a1=1;
                b1=settledpoint.y();
                a2=2;
                b2=b1;
            }
            else
            {
                a1=0;
                b1=0;
                double sinangle=sin(((double)angle/(double)180)*M_PI);
                double cosangle=cos(((double)angle/(double)180)*M_PI);
                double tanangle=sinangle/cosangle;
                a2=1;
                b2=(int)round(tanangle);
                a1+=settledpoint.x();
                b1+=settledpoint.y();
                a2+=settledpoint.x();
                b2+=settledpoint.y();
            }
            QPoint a(a1,b1);
            QPoint b(a2,b2);
            projectPoint(a,b,point,projectpoint);
        }
        else
            projectPoint(settledpoint,pivotpoint,point,projectpoint);

        xmax=projectpoint.x();
        ymax=projectpoint.y();

        int xc=(int)round(((double)xmin+(double)xmax)/(double)2);
        int yc=(int)round(((double)ymin+(double)ymax)/(double)2);

        uiround->second.roundmatrix.rotatePoint(xc,yc,xmin,ymin,xmax,ymax);

        if(controlpoint==0)
        {
            prevrect.leftup.setX(xmax);
            prevrect.leftup.setY(ymax);
            prevrect.rightdown.setX(xmin);
            prevrect.rightdown.setY(ymin);
        }
        else if(controlpoint==1)
        {
            prevrect.leftdown.setX(xmax);
            prevrect.leftdown.setY(ymax);
            prevrect.rightup.setX(xmin);
            prevrect.rightup.setY(ymin);
        }
        else if(controlpoint==2)
        {
            prevrect.rightup.setX(xmax);
            prevrect.rightup.setY(ymax);
            prevrect.leftdown.setX(xmin);
            prevrect.leftdown.setY(ymin);
        }
        else
        {
            prevrect.rightdown.setX(xmax);
            prevrect.rightdown.setY(ymax);
            prevrect.leftup.setX(xmin);
            prevrect.leftup.setY(ymin);
        }

        if(xmin>xmax)
            swapNum(xmin,xmax);
        if(ymin>ymax)
            swapNum(ymin,ymax);

        int rx=(int)round((double)(xmax-xmin)/2);
        int ry=(int)round((double)(ymax-ymin)/2);
        int r=max(rx,ry);

        uiround->second.xc=xc;
        uiround->second.yc=yc;
        uiround->second.r=r;

        uiround->second.roundmatrix.rebuildRoundmatrix(xc,yc);
    }
    else if(controlshape==Curve)
    {
        int dx=point.x()-lastpoint.x();
        int dy=point.y()-lastpoint.y();
        int newx=pivotpoint.x()+dx;
        int newy=pivotpoint.y()+dy;
        uicurve->second.pointset[controlpoint].setX(newx);
        uicurve->second.pointset[controlpoint].setY(newy);
    }
    editid=selectid;
    resetCanvas(width,height,true);
}

void myCanvas::projectPoint(QPoint &a,QPoint &b,QPoint &point,QPoint &projectpoint)
{
    int x0=a.x();
    int y0=a.y();
    int x1=b.x();
    int y1=b.y();
    if(x0==x1&&y0==y1)
    {
        qDebug() << "error in projectPoint: line is a dot";
        projectpoint=point;
    }
    else if(x0==x1)
    {
        projectpoint.setX(x0);
        projectpoint.setY(point.y());
    }
    else if(y0==y1)
    {
        projectpoint.setX(point.y());
        projectpoint.setY(y0);
    }
    else
    {
        double k,b;
        k=(double)(y1-y0)/(double)(x1-x0);
        b=(double)y0-k*(double)x0;
        double tx=(k*point.y()+point.x()-k*b)/(k*k+1);
        int x=(int)round(tx);
        int y=(int)round(k*tx+b);
        projectpoint.setX(x);
        projectpoint.setY(y);
    }
}

void myCanvas::copyInformation(bool first)
{
    Shape shape=scaleshape;
    if(first)
    {
        if(shape==Line)
        {
            prevline.x0=uiline->second.x0;
            prevline.y0=uiline->second.y0;
            prevline.x1=uiline->second.x1;
            prevline.y1=uiline->second.y1;
        }
        else if(shape==Polygon)
        {
            delete prevpolygon.pointset;
            int pointnum=uipolygon->second.pointnum;
            prevpolygon.pointset=new int[pointnum*2];
            for(int i=0;i<2*pointnum;i++)
                prevpolygon.pointset[i]=uipolygon->second.pointset[i];
            prevpolygon.pointnum=pointnum;
        }
        else if(shape==Ellipse)
        {
            prevellipse.rx=uiellipse->second.rx;
            prevellipse.ry=uiellipse->second.ry;
            prevellipse.xc=uiellipse->second.xc;
            prevellipse.yc=uiellipse->second.yc;
            prevellipse.ellipsematrix=uiellipse->second.ellipsematrix;
        }
        else if(shape==Round)
        {
            prevround.r=uiround->second.r;
            prevround.xc=uiround->second.xc;
            prevround.yc=uiround->second.yc;
            prevround.roundmatrix=uiround->second.roundmatrix;
        }
        else if(shape==Curve)
        {
            int pointnum=uicurve->second.pointnum;
            prevcurve.pointset.clear();
            for(int i=0;i<pointnum;i++)
                prevcurve.pointset.push_back(uicurve->second.pointset[i]);
            prevcurve.pointnum=pointnum;
        }
    }
    else
    {
        if(shape==Line)
        {
            uiline->second.x0=prevline.x0;
            uiline->second.y0=prevline.y0;
            uiline->second.x1=prevline.x1;
            uiline->second.y1=prevline.y1;
        }
        else if(shape==Polygon)
        {
            int pointnum=prevpolygon.pointnum;
            for(int i=0;i<2*pointnum;i++)
                uipolygon->second.pointset[i]=prevpolygon.pointset[i];
        }
        else if(shape==Ellipse)
        {
            uiellipse->second.rx=prevellipse.rx;
            uiellipse->second.ry=prevellipse.ry;
            uiellipse->second.xc=prevellipse.xc;
            uiellipse->second.yc=prevellipse.yc;
            uiellipse->second.ellipsematrix=prevellipse.ellipsematrix;
        }
        else if(shape==Round)
        {
            uiround->second.r=prevround.r;
            uiround->second.xc=prevround.xc;
            uiround->second.yc=prevround.yc;
            uiround->second.roundmatrix=prevround.roundmatrix;
        }
        else if(shape==Curve)
        {
            int pointnum=prevcurve.pointnum;
            uicurve->second.pointset.clear();
            for(int i=0;i<pointnum;i++)
               uicurve->second.pointset.push_back(prevcurve.pointset[i]);
            uicurve->second.pointnum=pointnum;
        }
    }
}

void myCanvas::scale_ui(QPoint &point,bool first,bool final)
{
    QPoint projectpoint;
    projectPoint(centerpoint,lastpoint,point,projectpoint);
    double show=1;
    if(first)
    {
        paintCenterPoint();
        map <int,myGraph>::iterator currentgraph;
        currentgraph=graphset->find(selectid);
        Shape shape=currentgraph->second.shape;
        if(shape==Line)
        {
            scaleshape=Line;
            uiline=lineset->find(selectid);
        }
        else if(shape==Polygon)
        {
            scaleshape=Polygon;
            uipolygon=polygonset->find(selectid);
        }
        else if(shape==Ellipse)
        {
            scaleshape=Ellipse;
            uiellipse=ellipseset->find(selectid);
        }
        else if(shape==Round)
        {
            scaleshape=Round;
            uiround=roundset->find(selectid);
        }
        else if(shape==Curve)
        {
            scaleshape=Curve;
            uicurve=curveset->find(selectid);
        }
        copyInformation(true);
        resetCanvas(width,height,true);
    }
    else
    {
        copyInformation(false);
        int pivotlength,nowlength;
        int a1=centerpoint.x()-lastpoint.x();
        int b1=centerpoint.y()-lastpoint.y();
        int a2=centerpoint.x()-projectpoint.x();
        int b2=centerpoint.y()-projectpoint.y();
        if(a1==0)
        {
            pivotlength=b1;
            nowlength=b2;
        }
        else if(b1==0)
        {
            pivotlength=a1;
            nowlength=a2;
        }
        else
        {
            if(abs(a1)>abs(b1))
            {
                pivotlength=a1;
                nowlength=a2;
            }
            else
            {
                pivotlength=b1;
                nowlength=b2;
            }
        }
        editid=selectid;
        double r=(double)nowlength/(double)pivotlength;
        scale(selectid,centerpoint.x(),centerpoint.y(),r);
        show=r;
    }
    if(!final)
    {
        QPen temppen;
        temppen.setColor(QColor(255,255,255));
        temppen.setWidth(3);
        temppen.setStyle(Qt::SolidLine);
        temppen.setCapStyle(Qt::SquareCap);
        QPainter *painter=setPainter(disclipen);
        painter->drawLine(centerpoint,lastpoint);
        painter->drawLine(centerpoint,projectpoint);
        paintPoint(lastpoint.x(),lastpoint.y(),painter,temppen);
        paintPoint(projectpoint.x(),projectpoint.y(),painter,temppen);
        delete painter;
        paintCenterPoint();
        QWidget::update();
    }
    statusinfo=tr("缩放倍数: ");
    if(show<0)
    {
        statusinfo+=tr("镜像");
        show=-show;
    }
    statusinfo+=QString::number(show);
    statusinfo+=tr("倍");
    emit sendStatusbarinfo(statusinfo);
}

bool myCanvas::closeWindow()
{
    assertion();
    if(hasedited)
    {
        QMessageBox message(QMessageBox::Warning,"保存画布",tr("当前画布有编辑未保存。是否保存再退出?"),QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No,NULL);
        int ret=message.exec();
        if(ret==QMessageBox::Yes)
        {
            saveFile();
            return true;
        }
        else if(ret==QMessageBox::Cancel)
            return false;
        else
            return true;
    }
    return true;
}
