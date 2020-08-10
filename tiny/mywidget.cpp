#include "mywidget.h"
#include "ui_mywidget.h"

#include "globals.h"
#include "parse.h"
#include "scan.h"
#include "util.h"

#include "QFileDialog"
#include "QDirIterator"
#include "QFile"
#include "QTextStream"
#include "QDebug"
#include "QMessageBox"

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

myWidget::myWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myWidget)
{
    ui->setupUi(this);
}

myWidget::~myWidget()
{
    delete ui;
}

//选择分析文件并输出内容
void myWidget::on_choose_Button_clicked()
{
    ui->tiny_textBrowser->clear();
    QString openfile = QFileDialog::getOpenFileName(this,
                                                    "Open Tiny File",
                                                    QDir::currentPath(),
                                                    "tiny(*.txt)");

    tinyfile = openfile;
    QFile rfile(tinyfile);

    bool isOk = rfile.open(QFile::ReadOnly|QFile::Text);
    if(isOk == true){
        QTextStream qin(&rfile);
        ui->tiny_textBrowser->setText(qin.readAll());

        ui->tree_textBrowser->setText("点击右边按钮可查看语法树。");
        qDebug() << "读取完成";
    }
    rfile.close();

}


void myWidget::on_saveas_Button_clicked()
{
    QString inputText = ui->tiny_textBrowser->toPlainText();
    if(inputText.isEmpty() == false){


        QString savefile = QFileDialog::getSaveFileName(this,
                                                        "Save Tiny File",
                                                        QDir::currentPath(),
                                                        "tiny(*.txt");

        QFile wfile(savefile);

        bool isOk = wfile.open(QFile::WriteOnly|QFile::Text);
        if(isOk == true){
            QTextStream qout(&wfile);
            qout << inputText;
        }
        wfile.close();
        qDebug()<< "写入完成";
    }
    else{
        QMessageBox msg;
        msg.about(this,"警告","输入为空，不允许保存！");
        //qDebug()<<"内容为空";
    }
}

void myWidget::on_tree_Button_clicked()
{
    if(tinyfile == (const char *)0){
        QMessageBox msg;
        msg.about(this,"警告","未加载源程序，不能生成语法树！");

    }
    else{
        ui->tree_textBrowser->clear();

        source = fopen(tinyfile.toLocal8Bit(),"r");
        listing = fopen(treefile.toLocal8Bit(),"w");

        TreeNode * syntaxTree = parse();
        fprintf(listing,"\n ******* Syntax tree ******* \n\n\n");
        printTree(syntaxTree);

        fclose(source);
        fclose(listing);

        QFile file(treefile);
        bool isOK = file.open(QFile::ReadOnly);
        if(isOK == true){
            QTextStream stream(&file);
            ui->tree_textBrowser->setText(stream.readAll());
        }
        file.close();
    }

}

void myWidget::on_reset_Button_clicked()
{
    ui->tiny_textBrowser->clear();
    ui->tree_textBrowser->clear();
    tinyfile = (const char *)0;
}

void myWidget::on_close_Button_clicked()
{
    ui->tiny_textBrowser->clear();
    ui->tree_textBrowser->clear();
    close();
}
