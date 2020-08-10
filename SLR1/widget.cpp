#include "widget.h"
#include "ui_widget.h"
#include "SLR1.h"
#include "SLR1.cpp"

#include "string"
#include "QString"
#include "QFileDialog"
#include "QDir"
#include "QDebug"
#include "QMessageBox"
#include "QInputDialog"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


// 打开文件
void Widget::on_open_pushButton_clicked()
{
    if(waitto_analyse_sentence.isEmpty()){
        openfile = QFileDialog::getOpenFileName(this,
                                                tr("open"),
                                                QDir::currentPath(),
                                                "*.txt;;"
                                                        );
    }

    QString file;
    QFile rfile(openfile);
    bool isOk = rfile.open(QFile::ReadOnly|QFile::Text);
    if(isOk == true){
        QTextStream qin(&rfile);
        file = qin.readAll();

    }
    rfile.close();

    if(file.isNull() == false){

        ui->textEdit->clear();

        SLR grammar(file.toStdString());

        qDebug() << file<<"读取完成";

        string text = grammar.genprintP(); // 扩展式
        ui->textEdit->setPlainText(QString::fromStdString(text));

        grammar.getFollow();
        grammar.dfa();
        grammar.getTable();

        state_str = QString::fromStdString(grammar.printS()); // dfa
        state_str += QString::fromStdString(grammar.printGO());

        gra_str = QString::fromStdString(grammar.printTable()); // 状态表

        check_str = QString::fromStdString(grammar.parser(waitto_analyse_sentence.toStdString())); // 是否属于SLR(1)文法
    }
}

// 保存文件
void Widget::on_save_pushButton_clicked()
{

    QString inputText = ui->textEdit->toPlainText();
    if(inputText.isEmpty() == false){
        QString savefile= QFileDialog::getSaveFileName(this,
                                                  tr("Save"),
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

// 重置
void Widget::on_reset_pushButton_clicked()
{
    ui->textEdit->clear();
    ui->analyze_textBrowser->clear();
    ui->state_textBrowser->clear();
    openfile = (const char *)0;
    state_str = (const char *)0;
    check_str = (const char *)0;
    gra_str = (const char *)0;
    waitto_analyse_sentence = (const char *)0;

}

// 分析
void Widget::on_analys_pushButton_clicked()
{
    if(state_str == (const char *)0){
        QMessageBox msg;
        msg.about(this,"警告","请先打开分析文件或输入语法!");
    }
    else{
        ui->state_textBrowser->setPlainText(state_str);
    }
}


// 检查是否为SLR(0)语法
void Widget::on_cheak_pushButton_clicked()
{
    if(openfile == (const char *)0){
        QMessageBox msg;
        msg.about(this,"警告","未分析文件!");
    }
    else{
        bool isOK;
        waitto_analyse_sentence = QInputDialog::getText(NULL, "分析语句输入框", "例：id * id + id(注意记号之间需要空格)",
                                                        QLineEdit::Normal, "id * id + id", &isOK);
        if(isOK && !waitto_analyse_sentence.isEmpty()){
            on_open_pushButton_clicked();
            ui->analyze_textBrowser->clear();
            ui->analyze_textBrowser->setPlainText(check_str);
        }
    }
}

// 查看分析表
void Widget::on_view_pushButton_clicked()
{
    ui->analyze_textBrowser->clear();
    if(gra_str == (const char *)0){
        QMessageBox msg;
        msg.about(this,"警告","未分析文件!");
    }
    else{
        ui->analyze_textBrowser->setPlainText(gra_str);
    }

}

