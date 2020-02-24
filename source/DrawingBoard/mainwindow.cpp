#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mycanvas.h"
#include "resetcanvassize.h"
#include "preferences.h"
#include "setpaintcolor.h"
#include "setpenwidth.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1255,730);
    canvas=new myCanvas(this);
    this->setCentralWidget(canvas);
    connect(canvas,SIGNAL(send_resCan_signal()),this,SLOT(changeMainWindow()));

    connect(canvas,SIGNAL(sendStatusbarinfo(QString)),this,SLOT(showStatusbar(QString)));

    init_Menubar();
    init_Toolbar();
    init_Statusbar();
}

MainWindow::~MainWindow()
{
    delete canvas;
    delete ui;
}

void MainWindow::init_Menubar()
{
    QMenu *info=menuBar()->addMenu(tr("About"));
    about=new QAction(tr("About DrawingBoard"));
    connect(about,&QAction::triggered,this,&MainWindow::showInformation);
    info->addAction(about);
    settings=new QAction(tr("Preferences"));
    connect(settings,&QAction::triggered,this,&MainWindow::setPreferences);
    info->addAction(settings);

    QMenu *file=menuBar()->addMenu(tr("画布"));
    newfile_menubar=new QAction(tr("新建"),this);
    connect(newfile_menubar,&QAction::triggered,this,&MainWindow::newFile);
    file->addAction(newfile_menubar);
    openfile_menubar=new QAction(tr("打开"),this);
    connect(openfile_menubar,&QAction::triggered,this,&MainWindow::openFile);
    file->addAction(openfile_menubar);
    savefile_menubar=new QAction(tr("保存"),this);
    connect(savefile_menubar,&QAction::triggered,this,&MainWindow::saveFile);
    file->addAction(savefile_menubar);
    savefileas_menubar=new QAction(tr("另存为"),this);
    connect(savefileas_menubar,&QAction::triggered,this,&MainWindow::saveFileas);
    file->addAction(savefileas_menubar);

    QMenu *create=menuBar()->addMenu(tr("创建"));
    createline_menu=new QAction(tr("直线"));
    connect(createline_menu,&QAction::triggered,this,&MainWindow::createLine);
    create->addAction(createline_menu);
    createpolygon_menu=new QAction(tr("多边形"));
    connect(createpolygon_menu,&QAction::triggered,this,&MainWindow::createPolygon);
    create->addAction(createpolygon_menu);
    createellipse_menu=new QAction(tr("椭圆"));
    connect(createellipse_menu,&QAction::triggered,this,&MainWindow::createEllipse);
    create->addAction(createellipse_menu);
    createround_menu=new QAction(tr("圆"));
    connect(createround_menu,&QAction::triggered,this,&MainWindow::createRound);
    create->addAction(createround_menu);
    createcurve_menu=new QAction(tr("曲线"));
    connect(createcurve_menu,&QAction::triggered,this,&MainWindow::createCurve);
    create->addAction(createcurve_menu);

    QMenu *edit=menuBar()->addMenu(tr("编辑"));
    QMenu *canvasinfo=edit->addMenu(tr("画布"));
    resetcanvasize=new QAction(tr("重设画布大小"),this);
    connect(resetcanvasize,&QAction::triggered,this,&MainWindow::resetCanvasSize);
    canvasinfo->addAction(resetcanvasize);
    clearcanvas=new QAction(tr("清空画布"),this);
    connect(clearcanvas,&QAction::triggered,this,&MainWindow::clearCanvas);
    canvasinfo->addAction(clearcanvas);
    QMenu *graph=edit->addMenu(tr("图元"));
    selectgraph_menu=new QAction(tr("选择图元"),this);
    connect(selectgraph_menu,&QAction::triggered,this,&MainWindow::selectGraph);
    graph->addAction(selectgraph_menu);
    rotategraph_menu=new QAction(tr("旋转图元"),this);
    connect(rotategraph_menu,&QAction::triggered,this,&MainWindow::rotateGraph);
    graph->addAction(rotategraph_menu);
    scalegraph_menu=new QAction(tr("缩放图元"),this);
    connect(scalegraph_menu,&QAction::triggered,this,&MainWindow::scaleGraph);
    graph->addAction(scalegraph_menu);
    deletegraph_menu=new QAction(tr("删除图元"),this);
    connect(deletegraph_menu,&QAction::triggered,this,&MainWindow::deleteGraph);
    graph->addAction(deletegraph_menu);
    QMenu *brush=edit->addMenu(tr("画笔"));
    changecolor_menubar=new QAction(tr("画笔颜色"),this);
    connect(changecolor_menubar,&QAction::triggered,this,&MainWindow::changeColor);
    brush->addAction(changecolor_menubar);
    changepenwidth_menubar=new QAction(tr("画笔粗细"),this);
    connect(changepenwidth_menubar,&QAction::triggered,this,&MainWindow::changePenwidth);
    brush->addAction(changepenwidth_menubar);

    /*QMenu *linedit=edit->addMenu(tr("直线"));
    clipline=new QAction(tr("裁剪"),this);
    connect(clipline,&QAction::triggered,this,&MainWindow::clipLine);
    linedit->addAction(clipline);*/

}

void MainWindow::init_Toolbar()
{
    newfile=new QAction(QIcon(":/picture/new"),tr("新建"),ui->mainToolBar);
    newfile->setStatusTip(tr("新建画布"));
    ui->mainToolBar->addAction(newfile);
    connect(newfile,&QAction::triggered,this,&MainWindow::newFile);

    openfile_ui=new QAction(QIcon(":/picture/open"),tr("打开"),ui->mainToolBar);
    openfile_ui->setStatusTip(tr("打开文件"));
    ui->mainToolBar->addAction(openfile_ui);
    connect(openfile_ui,&QAction::triggered,this,&MainWindow::openFile);

    savefile=new QAction(QIcon(":/picture/save"),tr("保存"),ui->mainToolBar);
    savefile->setStatusTip(tr("保存画布"));
    ui->mainToolBar->addAction(savefile);
    connect(savefile,&QAction::triggered,this,&MainWindow::saveFile);

    savefileas=new QAction(QIcon(":/picture/saveas"),tr("另存为"),ui->mainToolBar);
    savefileas->setStatusTip(tr("另存为"));
    ui->mainToolBar->addAction(savefileas);
    connect(savefileas,&QAction::triggered,this,&MainWindow::saveFileas);


    QToolBar *toolbar1=addToolBar("CreateShape");

    createline=new QAction(QIcon(":/picture/line"),tr("直线"),toolbar1);
    createline->setStatusTip(tr("绘制直线"));
    toolbar1->addAction(createline);
    connect(createline,&QAction::triggered,this,&MainWindow::createLine);

    createpolygon=new QAction(QIcon(":/picture/polygon"),tr("多边形"),toolbar1);
    createpolygon->setStatusTip(tr("绘制多边形"));
    toolbar1->addAction(createpolygon);
    connect(createpolygon,&QAction::triggered,this,&MainWindow::createPolygon);

    createellipse=new QAction(QIcon(":/picture/ellipse"),tr("椭圆"),toolbar1);
    createellipse->setStatusTip(tr("绘制椭圆"));
    toolbar1->addAction(createellipse);
    connect(createellipse,&QAction::triggered,this,&MainWindow::createEllipse);

    createround=new QAction(QIcon(":/picture/round"),tr("圆"),toolbar1);
    createround->setStatusTip(tr("绘制圆"));
    toolbar1->addAction(createround);
    connect(createround,&QAction::triggered,this,&MainWindow::createRound);

    createcurve=new QAction(QIcon(":/picture/curve"),tr("曲线"),toolbar1);
    createcurve->setStatusTip(tr("绘制曲线"));
    toolbar1->addAction(createcurve);
    connect(createcurve,&QAction::triggered,this,&MainWindow::createCurve);


    QToolBar *toolbar2=addToolBar("EditShape");

    selectgraph=new QAction(QIcon(":/picture/select"),tr("选择"),toolbar2);
    selectgraph->setStatusTip(tr("选择图元"));
    toolbar2->addAction(selectgraph);
    connect(selectgraph,&QAction::triggered,this,&MainWindow::selectGraph);

    rotategraph=new QAction(QIcon(":/picture/rotate"),tr("旋转"),toolbar2);
    rotategraph->setStatusTip(tr("旋转已选图元"));
    toolbar2->addAction(rotategraph);
    connect(rotategraph,&QAction::triggered,this,&MainWindow::rotateGraph);

    scalegraph=new QAction(QIcon(":/picture/scale"),tr("缩放"),toolbar2);
    scalegraph->setStatusTip(tr("缩放已选图元"));
    toolbar2->addAction(scalegraph);
    connect(scalegraph,&QAction::triggered,this,&MainWindow::scaleGraph);

    clipline=new QAction(QIcon(":/picture/clip"),tr("裁剪"),toolbar2);
    clipline->setStatusTip(tr("直线裁剪"));
    toolbar2->addAction(clipline);
    connect(clipline,&QAction::triggered,this,&MainWindow::clipLine);

    deletegraph=new QAction(QIcon(":/picture/delete"),tr("删除"),toolbar2);
    deletegraph->setStatusTip(tr("删除选中图元"));
    toolbar2->addAction(deletegraph);
    connect(deletegraph,&QAction::triggered,this,&MainWindow::deleteGraph);

    clearcanvas_toolbar=new QAction(QIcon(":/picture/clear"),tr("清空"),toolbar2);
    clearcanvas_toolbar->setStatusTip(tr("清空画布"));
    toolbar2->addAction(clearcanvas_toolbar);
    connect(clearcanvas_toolbar,&QAction::triggered,this,&MainWindow::clearCanvas);


    QToolBar *toolbar3=addToolBar("Settings");

    settings_toolbar=new QAction(QIcon(":/picture/settings"),tr("偏好设置"),toolbar3);
    settings_toolbar->setStatusTip(tr("偏好设置"));
    toolbar3->addAction(settings_toolbar);
    connect(settings_toolbar,&QAction::triggered,this,&MainWindow::setPreferences);

    changecolor=new QAction(QIcon(":/picture/palette"),tr("颜色"),toolbar3);
    changecolor->setStatusTip(tr("修改颜色"));
    toolbar3->addAction(changecolor);
    connect(changecolor,&QAction::triggered,this,&MainWindow::changeColor);

    changepenwidth=new QAction(QIcon(":/picture/pen2"),tr("画笔"),toolbar3);
    changepenwidth->setStatusTip(tr("修改画笔粗细"));
    toolbar3->addAction(changepenwidth);
    connect(changepenwidth,&QAction::triggered,this,&MainWindow::changePenwidth);

}

void MainWindow::init_Statusbar()
{
    infolabel=new QLabel();
    statusBar()->addWidget(infolabel);
}

void MainWindow::changeMainWindow()
{
    int width=canvas->getWidth();
    int height=canvas->getHeight();
    this->resize(width+25,height+40+50);
}

void MainWindow::commandline_init()
{
    getline(cin,openfile);
    getline(cin,savepath);
    canvas->loadSavePath(savepath);
    qDebug() << "starting processing file...";
    processFile();
    qDebug() << "processing file finished.";
    this->close();
}

void MainWindow::openFile()
{
    canvas->handleFileAction(Open);
}

void MainWindow::newFile()
{
    canvas->handleFileAction(New);
}

void MainWindow::saveFile()
{
    canvas->handleFileAction(Save);
}

void MainWindow::saveFileas()
{
    canvas->handleFileAction(Saveas);
}

void MainWindow::showInformation()
{
    QMessageBox::about(this,"DrawingBoard","          version 4.2\ncreated by YuanXiang");
}

void MainWindow::setPreferences()
{
    if(canvas->assertion())
        return;
    Information information;
    canvas->getInformation(information);
    Preferences *dialog=new Preferences();
    dialog->loadInfo(canvas,information);
    dialog->exec();
}

void MainWindow::changeColor()
{
    if(canvas->assertion())
        return;
    Information information;
    canvas->getInformation(information);
    setPaintColor *dialog=new setPaintColor();
    dialog->loadInfo(canvas,information);
    dialog->exec();
}

void MainWindow::changePenwidth()
{
    if(canvas->assertion())
        return;
    setPenwidth *widget=new setPenwidth();
    widget->loadInfo(canvas,canvas->getPenwidth());
    widget->setWindowModality(Qt::ApplicationModal);
    widget->show();
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
    regex reg11("\\s*scale\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+((\\d+\\.\\d*)|((\\.)?\\d+))\\s*");
    regex reg12("\\s*clip\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+Cohen-Sutherland\\s*");
    regex reg13("\\s*clip\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+-?[0-9]\\d*\\s+Liang-Barsky\\s*");
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
            canvas->resetCanvas(width,height,false); //调用myCanvas重置画布对外接口函数
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
    }
    fp.close();
    return true;
}

void MainWindow::createLine()
{
    canvas->setAction(Line,Create);
}

void MainWindow::createPolygon()
{
    canvas->setAction(Polygon,Create);
}

void MainWindow::createEllipse()
{
    canvas->setAction(Ellipse,Create);
}

void MainWindow::createRound()
{
    canvas->setAction(Round,Create);
}

void MainWindow::createCurve()
{
    canvas->setAction(Curve,Create);
}

void MainWindow::resetCanvasSize()
{
    if(canvas->assertion())
        return;
    ResetCanvasSize *dialog=new ResetCanvasSize();
    dialog->loadInfo(canvas->getWidth(),canvas->getHeight(),canvas);
    dialog->exec();
}

void MainWindow::clearCanvas()
{
    QMessageBox message(QMessageBox::Warning,"confirm",tr("确认清空画布?"),
QMessageBox::Yes | QMessageBox::No,NULL);
    if(message.exec()==QMessageBox::Yes)
    {
        canvas->assertion();
        canvas->resetCanvas(canvas->getWidth(),canvas->getHeight(),false);
    }
}

void MainWindow::selectGraph()
{
    canvas->setAction(NoneShape,Select);
}

void MainWindow::clipLine()
{
    canvas->setAction(Line,Clip);
}

void MainWindow::deleteGraph()
{
    canvas->deleteGraph();
}

void MainWindow::rotateGraph()
{
    canvas->setAction(NoneShape,Rotate);
}

void MainWindow::scaleGraph()
{
    canvas->setAction(NoneShape,Scale);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(canvas->closeWindow())
        event->accept();
    else
        event->ignore();
}

void MainWindow::showStatusbar(QString info)
{
    infolabel->setText(info);
    infolabel->repaint();
}
