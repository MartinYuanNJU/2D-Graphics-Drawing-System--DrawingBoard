#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common.h"
#include "mycanvas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    myCanvas *canvas; //核心画布单元，内含绘图算法，图元存储，鼠标交互，绘图对外接口

    QLabel *infolabel; //状态栏信息显示内容

    void init_Menubar(); //初始化菜单栏
    void init_Toolbar(); //初始化工具栏
    void init_Statusbar(); //初始化状态栏

    //the DrawingBoard column's action
    QAction *about; //菜单栏中按下“关于”功能选项动作指示
    QAction *settings; //菜单栏中按下“偏好设置”功能或者工具栏按下设置按钮动作指示
    QAction *settings_toolbar;
    QAction *changecolor;
    QAction *changepenwidth;

    QAction *changecolor_menubar;
    QAction *changepenwidth_menubar;

    //the File column's actions
    QAction *newfile;
    QAction *openfile_ui;
    QAction *savefile;
    QAction *savefileas;
    QAction *newfile_menubar;
    QAction *openfile_menubar;
    QAction *savefile_menubar;
    QAction *savefileas_menubar;

    //the Create column's actions
    QAction *createline;
    QAction *createpolygon;
    QAction *createellipse;
    QAction *createround;
    QAction *createcurve;

    QAction *createline_menu;
    QAction *createpolygon_menu;
    QAction *createellipse_menu;
    QAction *createround_menu;
    QAction *createcurve_menu;

    QAction *selectgraph;
    QAction *deletegraph;
    QAction *rotategraph;
    QAction *scalegraph;

    QAction *selectgraph_menu;
    QAction *deletegraph_menu;
    QAction *rotategraph_menu;
    QAction *scalegraph_menu;

    //the Edit column's actions
    QAction *resetcanvasize; //菜单栏中按下“编辑->画布->重设画布大小”功能选项动作指示
    QAction *clearcanvas; //菜单栏中按下“编辑->画布->清空画布”功能选项动作指示
    QAction *clearcanvas_toolbar;
    QAction *clipline; //菜单栏中按下“编辑->图元->裁剪”功能选项动作指示

    string openfile; //存放输入的命令行文件路径
    string savepath; //存放保存文件的文件夹的路径
    bool processFile(); //文件输入接口，文件读取与指令处理

    void closeEvent(QCloseEvent *event);

private slots:
    void changeMainWindow();
    void showStatusbar(QString info);

public slots:
    void commandline_init();

    //the DrawingBoard column's action's function
    void showInformation(); //菜单栏中按下“关于”功能选项动作指示对应处理函数
    void setPreferences();
    void changeColor();
    void changePenwidth();

    //the File column's actions' functions
    void newFile();
    void openFile(); //菜单栏中按下“打开文件”功能选项动作指示对应处理函数
    void saveFile();
    void saveFileas();

    //the Create column's action's functions
    void createLine(); //菜单栏中按下“绘制->直线”功能选项动作指示对应处理函数
    void createPolygon();
    void createEllipse();
    void createRound();
    void createCurve();

    //the Edit column's actions
    void resetCanvasSize(); //菜单栏中按下“重设画布大小”功能选项动作指示对应处理函数
    void clearCanvas(); //菜单栏中按下“清空画布”功能选项动作指示对应处理函数
    void selectGraph();
    void clipLine(); //菜单栏中按下“编辑->直线->裁剪”功能选项动作指示
    void deleteGraph();
    void rotateGraph();
    void scaleGraph();

};

#endif // MAINWINDOW_H
