#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mycanvas.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canvas=new myCanvas(this);
    this->setCentralWidget(canvas);
}

MainWindow::~MainWindow()
{
    delete canvas;
    delete ui;
}

void MainWindow::commandline_init(string a,string b)
{
    openfile=a;
    savepath=b;
    canvas->loadSavePath(savepath);
    //qDebug() << "starting processing file...";
    processFile();
    //qDebug() << "processing file finished.";
    this->close();
}

bool MainWindow::processFile()
{
    ifstream fp(openfile,ios::in);
    if(!fp)
        return false;
    string commandline;
    bool match=false;
    regex reg1("\\s*resetCanvas\\s+\\d+\\s+\\d+\\s*");
    regex reg2("\\s*setColor\\s+\\d+\\s+\\d+\\s+\\d+\\s*");
    regex reg3("\\s*drawLine\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+DDA\\s*");
    regex reg4("\\s*drawLine\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+Bresenham\\s*");
    regex reg5("\\s*saveCanvas\\s+\\S+\\s*");
    regex reg6("\\s*drawPolygon\\s+-?[0-9]\\d*\\s+\\d+\\s+DDA\\s*");
    regex reg7("\\s*drawPolygon\\s+-?[0-9]\\d*\\s+\\d+\\s+Bresenham\\s*");
    regex reg8("\\s*drawEllipse\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+\\d+\\s+\\d+\\s*");
    regex reg9("\\s*translate\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s*");
    regex reg10("\\s*rotate\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s*");
    //regex reg11("\\s*scale\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+((\\d+\\.\\d*)|((\\.)?\\d+))\\s*");
    regex reg11("\\s*scale\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+(((-)?\\d+\\.\\d*)|((-)?(\\.)?\\d+))\\s*");
    regex reg12("\\s*clip\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+Cohen-Sutherland\\s*");
    regex reg13("\\s*clip\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+Liang-Barsky\\s*");
    regex reg14("\\s*drawCurve\\s+-?[0-9]\\d*\\s+\\d+\\s+Bezier\\s*");
    regex reg15("\\s*drawCurve\\s+-?[0-9]\\d*\\s+\\d+\\s+B-spline\\s*");
    while(getline(fp,commandline))
    {
        istrstream input(commandline.c_str());
        match=regex_match(commandline,reg1); //利用正则表达式判断是否匹配resetCanvas指令
        if(match) //匹配该指令形式
        {
            string temp;
            int width;
            int height;
            input >> temp >> width >> height; //依次读入resetCanvas字符串，宽度数据，高度数据
            canvas->resetCanvas_commandine(width,height); //调用myCanvas重置画布对外接口函数
            continue; //跳过后面的正则表达式匹配，直接进入下一次循环
        }
        match=regex_match(commandline,reg2);
        if(match)
        {
            string temp;
            int r,g,b;
            input >> temp >> r >> g >> b;
            canvas->setColor(r,g,b);
            continue;
        }
        match=regex_match(commandline,reg3);
        if(match)
        {
            string temp1,temp2;
            int id,x0,y0,x1,y1;
            input >> temp1 >> id >> x0 >> y0 >> x1 >> y1 >> temp2;
            canvas->drawLine(id,x0,y0,x1,y1,DDA);
            continue;
        }
        match=regex_match(commandline,reg4);
        if(match)
        {
            string temp1,temp2;
            int id,x0,y0,x1,y1;
            input >> temp1 >> id >> x0 >> y0 >> x1 >> y1 >> temp2;
            canvas->drawLine(id,x0,y0,x1,y1,Bresenham);
            continue;
        }
        match=regex_match(commandline,reg5);
        if(match)
        {
            string temp1,savefile;
            input >> temp1 >> savefile;
            canvas->saveToFile(savefile);
            continue;
        }
        match=regex_match(commandline,reg6);
        if(match)
        {
            string temp1,temp2;
            int id,n;
            input >> temp1 >> id >> n >> temp2;
            if(n<=0) //不是一个多边形
            {
                getline(fp,commandline);
                continue;
            }
            int *temppointset=new int[n*2]; //根据读取的多边形点数创建动态数组
            getline(fp,commandline); //读取下一行的点集数据内容
            istrstream tempinput(commandline.c_str());
            for(int i=0;i<n*2;i++)
                tempinput >> temppointset[i]; //将点集中每一个点信息存放到动态数组中
            canvas->drawPolygon(id,temppointset,n,DDA); //调用myCanvas绘制多边形的对外接口
            delete []temppointset; //释放临时创建的动态数组所占空间
            continue;
        }
        match=regex_match(commandline,reg7);
        if(match)
        {
            string temp1,temp2;
            int id,n;
            input >> temp1 >> id >> n >> temp2;
            int *temppointset=new int[n*2];
            getline(fp,commandline);
            istrstream tempinput(commandline.c_str());
            for(int i=0;i<n*2;i++)
                tempinput >> temppointset[i];
            canvas->drawPolygon(id,temppointset,n,Bresenham);
            delete []temppointset;
            continue;
        }
        match=regex_match(commandline,reg8);
        if(match)
        {
            string temp;
            int id,xc,yc,rx,ry;
            input >> temp >> id >> xc >> yc >> rx >> ry;
            canvas->drawEllipse(id,xc,yc,rx,ry);
            continue;
        }
        match=regex_match(commandline,reg9);
        if(match)
        {
            string temp;
            int id,dx,dy;
            input >> temp >> id >> dx >> dy;
            canvas->translate(id,dx,dy);
            continue;
        }
        match=regex_match(commandline,reg10);
        if(match)
        {
            string temp;
            int id,x,y,r;
            input >> temp >> id >> x >> y >> r;
            canvas->rotate(id,x,y,r);
            continue;
        }
        match=regex_match(commandline,reg11);
        if(match)
        {
            string temp;
            int id,x,y;
            double r;
            input >> temp >> id >> x >> y >> r;
            canvas->scale(id,x,y,r);
            continue;
        }
        match=regex_match(commandline,reg12);
        if(match)
        {
            string temp1,temp2;
            int id,xmin,ymin,xmax,ymax;
            input >> temp1 >> id >> xmin >> ymin >> xmax >> ymax >> temp2;
            clipWindow clipwindow;
            if(xmin>=xmax||ymin>=ymax)
                continue;
            clipwindow.xmin=xmin;
            clipwindow.ymin=ymin;
            clipwindow.xmax=xmax;
            clipwindow.ymax=ymax;
            canvas->clip(id,&clipwindow,CohenSutherland);
            continue;
        }
        match=regex_match(commandline,reg13);
        if(match)
        {
            string temp1,temp2;
            int id,xmin,ymin,xmax,ymax;
            input >> temp1 >> id >> xmin >> ymin >> xmax >> ymax >> temp2;
            clipWindow clipwindow;
            if(xmin>=xmax||ymin>=ymax)
                continue;
            clipwindow.xmin=xmin;
            clipwindow.ymin=ymin;
            clipwindow.xmax=xmax;
            clipwindow.ymax=ymax;
            canvas->clip(id,&clipwindow,LiangBarsky);
            continue;
        }
        match=regex_match(commandline,reg14);
        if(match)
        {
            string temp1,temp2;
            int id,n;
            input >> temp1 >> id >> n >> temp2;
            if(n<=0)
            {
                getline(fp,commandline);
                continue;
            }
            getline(fp,commandline); //读取下一行的点集数据内容
            istrstream tempinput(commandline.c_str());
            int x,y;
            QVector <QPoint> temppointset;
            for(int i=0;i<n;i++)
            {
                tempinput >> x >> y;
                QPoint point(x,y);
                temppointset.push_back(point);
            }
            canvas->drawCurve(id,temppointset,n,Bezier); //调用myCanvas绘制多边形的对外接口
            temppointset.clear();
            continue;
        }
        match=regex_match(commandline,reg15);
        if(match)
        {
            string temp1,temp2;
            int id,n;
            input >> temp1 >> id >> n >> temp2;
            if(n<=0)
            {
                getline(fp,commandline);
                continue;
            }
            getline(fp,commandline); //读取下一行的点集数据内容
            istrstream tempinput(commandline.c_str());
            int x,y;
            QVector <QPoint> temppointset;
            for(int i=0;i<n;i++)
            {
                tempinput >> x >> y;
                QPoint point(x,y);
                temppointset.push_back(point);
            }
            canvas->drawCurve(id,temppointset,n,Bspline); //调用myCanvas绘制多边形的对外接口
            temppointset.clear();
            continue;
        }
    }
    fp.close();
    return true;
}
