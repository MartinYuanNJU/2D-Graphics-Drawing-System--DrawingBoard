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
    string openfile; //存放输入的命令行文件路径
    string savepath; //存放保存文件的文件夹的路径
    bool processFile(); //文件输入接口，文件读取与指令处理

public slots:
    void commandline_init(string a,string b);
};

#endif // MAINWINDOW_H
