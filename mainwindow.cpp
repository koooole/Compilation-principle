/***************************************************
 * File: mainwindow.cpp
 * Function: Mini C 编译器的界面设计与点击实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/7/10
 ****************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stdio.h"

#include "globals.h"
#include "parse.h"
#include "scan.h"
#include "util.h"
#include "symtab.h"
#include "analyze.h"
#include "cgen.h"

#include "QFileDialog"
#include "QDirIterator"
#include "QFile"
#include "QTextStream"
#include "QDebug"
#include "QMessageBox"
#include "QMenu"
#include "QMenuBar"
#include "QAction"
#include "QString"
#include <QStandardItemModel>
#include <QTableView>
#include <QTextCursor>

/* 分配全局变量 */
int lineno = 0;           // 源文件中的行数
FILE * source;            // 读取源代码的句柄
FILE * Syntax_listing;    // 用于保存生成的语法树（保存到tree文件）的句柄
FILE * Token_listing;     // 用于保存生成的词法分析（保存到token文件）的句柄
FILE * Analyze_listing;   // 用于保存生成的语义分析结果文本文件（保存到symtab文件，包括报错的情况）的句柄
FILE * code;              // 用于保存生成的TM模拟器的代码文本文件的句柄

QString cfile = "";   // 当前打开文件的文件名

/* 分配和设置跟踪标志 */
int EchoSource = FALSE;     // EchoSource = TRUE时，源程序在解析过程中以行号回显到listing文件
int TraceScan = FALSE;      // TraceScan = TRUE时，当分析器识别出每个token时，将token信息打印到listing文件中
int TraceParse = FALSE;     // TraceParse = TRUE时，语法树以线性形式打印到listing文件中(使用缩进表示子树)
int TraceAnalyze = FALSE;   // TraceAnalyze = TRUE时，将符号表插入和查找报告打印到listing文件
int TraceCode = FALSE;      // TraceCode = TRUE时，在代码生成时将注释写入TM代码文件
int Error = FALSE;          // Error为真时，如果发生错误，防止进一步通过

int ParseFlag = FALSE;      // 判断分析文件是否已经打开

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QMenuBar *mBar = menuBar(); // 添加菜单栏
    QMenu *pFile = mBar->addMenu("文件"); //添加菜单
    // QMenu *pHelp = mBar->addMenu("帮助");

    // 添加菜单项，添加动作
    QAction *pOpen = pFile->addAction("打开");
    pFile->addSeparator(); // 添加分割线
    QAction *pSave = pFile->addAction("保存");
    pFile->addSeparator();
    QAction *pClose = pFile->addAction("关闭");
    pFile->addSeparator();

    // 菜单项信号与槽相连
    connect(pOpen, &QAction::triggered,this,&MainWindow::open_file);
    connect(pSave, &QAction::triggered,this,&MainWindow::save_file);
    connect(pClose, &QAction::triggered,this,&MainWindow::close_all);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 打开源文件，并输出源文件内容
void MainWindow::open_file()
{

    QString openfile = QFileDialog::getOpenFileName(this,
                                                    "Open File",
                                                    QDir::currentPath(),
                                                    "*.txt");

    QString filetext;

    QFile rfile(openfile);
    bool isOk = rfile.open(QFile::ReadOnly|QFile::Text);
    if(isOk == true){
        QTextStream qin(&rfile);
        filetext = qin.readAll();
        qDebug() << "读取完成";
    }
    rfile.close();

    if(filetext.isNull() == false){
        cfile = openfile;

        lineno = 0;
        ParseFlag = FALSE;
        EOF_flag = FALSE;

        QFile::remove(QDir::currentPath() + QString::fromStdString("/tree"));
        QFile::remove(QDir::currentPath() + QString::fromStdString("/token"));
        QFile::remove(QDir::currentPath() + QString::fromStdString("/symtab"));
        QFile::remove(QDir::currentPath() + QString::fromStdString("/code.tm"));

        ui->C_textEdit->clear();
        ui->C_textEdit->setText(filetext);

    }
}

// 保存源文件
void MainWindow::save_file(){

    QString inputText = ui->C_textEdit->toPlainText();
    if(inputText.isEmpty() == false){

        QString savefile = QFileDialog::getSaveFileName(this,
                                                        "Save File",
                                                        QDir::currentPath(),
                                                        "*.txt");

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

// 关闭软件
void MainWindow::close_all(){

    ui->C_textEdit->clear();
    ui->analysis_textBrowser->clear();

    QFile::remove(QDir::currentPath() + QString::fromStdString("/tree"));
    QFile::remove(QDir::currentPath() + QString::fromStdString("/token"));
    QFile::remove(QDir::currentPath() + QString::fromStdString("/symtab"));

    close();
    qDebug()<< "关闭成功";
}

// 生成词法分析、语法分析、语义分析和代码生成文件
void MainWindow::GenParseFile(){

    source = fopen(cfile.toLocal8Bit(),"r");
    Syntax_listing = fopen(treefile.toLocal8Bit(),"w");
    Token_listing = fopen(tokenfile.toLocal8Bit(),"w");
    Analyze_listing = fopen(symtabfile.toLocal8Bit(),"w");

    fprintf(Token_listing,"%-14s%-8s\n","  Token","       Type");
    fprintf(Token_listing,"%-14s%-8s\n","---------","    --------------");

    TreeNode * syntaxTree = parse();

    fclose(Token_listing);

    fprintf(Syntax_listing," ---------- Syntax tree ---------- \n");

    printTree(syntaxTree);

    fclose(Syntax_listing);

    if (! Error)
    {
        if (!TraceAnalyze)
            fprintf(Analyze_listing,"\nBuilding Symbol Table...\n");
        buildSymtab(syntaxTree);

        if (!TraceAnalyze)
            fprintf(Analyze_listing,"\nChecking Types...\n");
        typeCheck(syntaxTree);

        if (!TraceAnalyze){
            if(Error)
                fprintf(Analyze_listing,"\nType Checking Finished, but had error \n");
            else
                fprintf(Analyze_listing,"\nType Checking Finished successfully \n");
        }

        hashRelease();

    }

    if (! Error)
        {

            code = fopen(codefile.toLocal8Bit(),"w");
            if (code == NULL)
            {
                printf("Unable to open %s\n",codefile);
            exit(1);
            }
            QByteArray arr = codefile.toUtf8();
            char* cf = arr.data();
            codeGen(syntaxTree,cf);
            fclose(code);
        }

    fclose(source);
    fclose(Analyze_listing);
}

// 词法分析
void MainWindow::on_morphology_pushButton_clicked()
{
    if(cfile == (const char *)0){
        QMessageBox msg;
        msg.about(this,"警告","未加载源程序，不能生成语法树！");

    }
    else{

        ui->analysis_textBrowser->clear();

        if(ParseFlag == FALSE){
           GenParseFile();
           ParseFlag = TRUE;
        }

        QFile file(tokenfile);
        bool isOK = file.open(QFile::ReadOnly);
        if(isOK == true){
            QTextStream stream(&file);
            ui->analysis_textBrowser->setText(stream.readAll().toLocal8Bit());
        }
        file.close();
    }
}

// 语法分析
void MainWindow::on_grammar_pushButton_clicked()
{
    if(cfile == (const char *)0){
        QMessageBox msg;
        msg.about(this,"警告","未加载源程序，不能生成语法树！");

    }
    else{
        ui->analysis_textBrowser->clear();

        if(ParseFlag == FALSE){
           GenParseFile();
           ParseFlag = TRUE;
        }

        QFile file(treefile);
        bool isOK = file.open(QFile::ReadOnly);
        if(isOK == true){
            QTextStream stream(&file);
            ui->analysis_textBrowser->setText(stream.readAll());
        }
        file.close();
    }
}

// 语义分析
void MainWindow::on_analyze_pushButton_clicked()
{
    if(cfile == (const char *)0){
        QMessageBox msg;
        msg.about(this,"警告","未加载源程序，不能生成语法树！");

    }
    else{
        ui->analysis_textBrowser->clear();

        if(ParseFlag == FALSE){
           GenParseFile();
           ParseFlag = TRUE;
        }

        QFile file(symtabfile);
        bool isOK = file.open(QFile::ReadOnly);
        if(isOK == true){
            QTextStream stream(&file);
            ui->analysis_textBrowser->setText(stream.readAll());
        }
        file.close();
    }
}

// 代码生成
void MainWindow::on_codegen_pushButton_clicked()
{
    if(cfile == (const char *)0){
        QMessageBox msg;
        msg.about(this,"警告","未加载源程序，不能生成语法树！");

    }
    else{
        ui->analysis_textBrowser->clear();

        if(ParseFlag == FALSE){
           GenParseFile();
           ParseFlag = TRUE;
        }

        QFile file(codefile);
        bool isOK = file.open(QFile::ReadOnly);
        if(isOK == true){
            QTextStream stream(&file);
            ui->analysis_textBrowser->setText(stream.readAll());
        }
        file.close();
    }
}


// 重置
void MainWindow::on_clear_pushButton_clicked()
{
    ui->C_textEdit->clear();
    ui->analysis_textBrowser->clear();

    QFile::remove(QDir::currentPath() + QString::fromStdString("/tree"));
    QFile::remove(QDir::currentPath() + QString::fromStdString("/token"));
    QFile::remove(QDir::currentPath() + QString::fromStdString("/symtab"));
    QFile::remove(QDir::currentPath() + QString::fromStdString("/code.tm"));

    cfile =  (const char *)0;
    lineno = 0;
    ParseFlag = FALSE;
    EOF_flag = FALSE;

    EchoSource = FALSE;
    TraceScan = FALSE;
    TraceParse = FALSE;
    TraceAnalyze = FALSE;
    TraceCode = FALSE;
    Error = FALSE;

}







