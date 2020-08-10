#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    w.resize(1050, 800);// 设置窗口大小
    w.move(500,100);// 设置屏幕显示位置
    w.setWindowTitle("SLR1"); //设置窗口名称

    return a.exec();
}
