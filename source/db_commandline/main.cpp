#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if(argc==3)
        w.commandline_init(string(argv[1]),string(argv[2]));
    else
        qDebug() << "wrong input.";

    return 0;
}
