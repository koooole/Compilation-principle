/***************************************************
 * File: mainwindow.h
 * Function: Mini C 编译器的界面设计与点击实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/7/10
 ****************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

extern QString cfile;  // 当前打开文件的文件名

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open_file();
    void save_file();
    void close_all();

    void GenParseFile();
    void on_morphology_pushButton_clicked();
    void on_grammar_pushButton_clicked();
    void on_analyze_pushButton_clicked();
    void on_codegen_pushButton_clicked();
    void on_clear_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    const QString treefile = "tree";      // 语法树文件
    const QString tokenfile = "token";    // 词法分析文件
    const QString symtabfile = "symtab";  // 语义分析文件
    const QString codefile = "code.tm";   // 代码生成文件
};

#endif // MAINWINDOW_H
