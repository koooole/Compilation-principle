#include "mywidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    myWidget w;

    w.resize(800, 700);// 设置窗口大小
    w.move(500,100);// 设置屏幕显示位置
    w.setWindowTitle("tiny"); //设置窗口名称

    w.show();

    return a.exec();
}
