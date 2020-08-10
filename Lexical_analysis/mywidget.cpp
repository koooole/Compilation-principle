#include "mywidget.h"
#include "ui_mywidget.h"
#include <QString>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QStandardItemModel>
#include <QTableView>
#include <QTextCursor>

#include<iostream>
#include<cstring>
using namespace std;

extern Scanner(const char *arr, int);
QStandardItemModel *model = new QStandardItemModel();

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    //tableview添加表头

    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,QStringLiteral("行号"));
    model->setHeaderData(1,Qt::Horizontal,QStringLiteral("表达式"));
    model->setHeaderData(2,Qt::Horizontal,QStringLiteral("标识记号"));

    ui->tableView->setModel(model);

    //设置列宽
    ui->tableView->setColumnWidth(0,65);
    ui->tableView->setColumnWidth(1,225);
    ui->tableView->setColumnWidth(2,140);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter); //表头信息居中

    /*
    //添加新数据项
    for(int i=0; i < 3; i++){
        //（默认行， 列， text）
        model->setItem(i, 0, new QStandardItem(QString("%0").arg(i)));
        model->item(i,0)->setTextAlignment(Qt::AlignCenter);
        model->setItem(i, 1, new QStandardItem(QString("aa")));
        model->item(i,1)->setTextAlignment(Qt::AlignCenter);
        model->setItem(i, 2, new QStandardItem(QString("11")));
        model->item(i,2)->setTextAlignment(Qt::AlignCenter);

    //删除所有数据项
    model->removeRows(0,model->rowCount());
    }
    */
}

static int display_row=0;

// 全局函数
void addNew(int row, string str,  int tag){

    //0关键字， 1特殊符号， 2标识符， 3数， 4串， 5注释
    QString tag1;
    switch (tag) {
    case 0:{
        tag1 = QStringLiteral("关键字");
        break;}
    case 1:{
        tag1 = QStringLiteral("特殊符号");
        break;}
    case 2:{
        tag1 = QStringLiteral("标识符");
        break;}
    case 3:{
        tag1 = QStringLiteral("数");
        break;}
    case 4:{
        tag1 = QStringLiteral("串");
        break;}
    case 5:{
        tag1 = QStringLiteral("注释");
        break;}
    default: break;
    }
    QString str1 = QString::fromStdString(str);

    //qDebug() << str1 <<  tag1;

    model->setItem(display_row, 0, new QStandardItem(QString("%0").arg(row)));
    model->item(display_row,0)->setTextAlignment(Qt::AlignCenter);
    model->setItem(display_row, 1, new QStandardItem(QString(str1)));
    model->item(display_row,1)->setTextAlignment(Qt::AlignCenter);
    model->setItem(display_row, 2, new QStandardItem(QString(tag1)));
    model->item(display_row,2)->setTextAlignment(Qt::AlignCenter);
    display_row++;

    return;
}


MyWidget::~MyWidget()
{

    delete ui;
}


void MyWidget::on_pushButton_clicked()
{

    path = QFileDialog::getOpenFileName(this,
                                                    tr("open"),
                                                    "D:\\qt\\Lexical_analysis\\TestText",
                                                    "all file(*.*);;"
                                                    "souce(*.cpp *.h);;"
                                                    "text(*.txt);;"
                                                    );
    ui->lineEdit->setText(path);
}

void MyWidget::on_pushButton_2_clicked()
{

    if(path.isEmpty() == false){
        //文件对象
        QFile file(path);

        //打开文件，只读
        bool isOk = file.open(QIODevice::ReadOnly);
        if(isOk == true){

            QByteArray array1;
            int row = 1;
            while(file.atEnd() == false){

                //按行读取文本并输出到文本框
                array1 = file.readLine();


                ui->textEdit->moveCursor(QTextCursor::End);
                ui->textEdit->textCursor().insertText(array1);

                //词法分析
                // 统一结束符 qt打开文件换行标志为“\r\n”，最后一行没有“\r\n”
                array1.append('\r');

                //qDebug()<<row;
                //qDebug() << array1;

                Scanner(array1,row);

                row++;

            }
            QByteArray array2 ;
            ui->textEdit->moveCursor(QTextCursor::End);
            ui->textEdit->textCursor().insertText(array2.append('\n'));

        }
        file.close();
        qDebug()<<"分析结束";
    }
}


void MyWidget::on_pushButton_4_clicked()
{
    close();
}

void MyWidget::on_pushButton_3_clicked()
{
    ui->textEdit->setText("");
    model->removeRows(0,model->rowCount());
    display_row = 0;
    ui->lineEdit->setText("");
    path = "";
}

void MyWidget::on_pushButton_5_clicked()
{
    ui->lineEdit->setText("");
    path = "";
    qDebug() << path;
}
