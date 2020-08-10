#include "mywidget.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyWidget w;



    w.resize(1000, 750);// 设置窗口大小
    w.move(500,100);// 设置屏幕显示位置
    w.setWindowTitle("Lexical analysis"); //设置窗口名称

    w.show();

    return a.exec();
}
