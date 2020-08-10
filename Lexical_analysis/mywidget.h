#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QString>
#include <QStandardItemModel>

#include<iostream>
#include<cstring>
using namespace std;

void addNew(int,string,int);

namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MyWidget *ui;
    QString path = NULL;
    //QStandardItemModel *model;

};

#endif // MYWIDGET_H
