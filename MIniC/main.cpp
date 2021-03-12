#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.resize(1040, 700);// 设置窗口大小
    w.move(500,100);// 设置屏幕显示位置
    w.setWindowTitle("Mini C"); //设置窗口名称

    w.show();

    return a.exec();
}
