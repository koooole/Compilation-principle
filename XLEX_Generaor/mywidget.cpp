#include "mywidget.h"
#include "ui_mywidget.h"
#include "mylex.h"


#include <iostream>
#include <stdio.h>
#include <string>

#include "QFileDialog"
#include "QFile"
#include "QString"
#include "QDebug"
#include "QMessageBox"
#include <QProcess>
#include <QDir>
#include "QTextBrowser"
#include "QInputDialog"
#include "QLineEdit"

using namespace std;

myWidget::myWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myWidget)
{
    projectFile = "..\\XLEX_Generaor";

    ui->setupUi(this);

    ui->regxText->setPlaceholderText("请在此处输入正则表达式或者右边选择文件");


    QMessageBox message;
    message.about(this,"提示","1.请确保电脑安装Graphviz（Graphviz为画图软件）;\n"
                            "2.如果你是首次打开该应用，请先点击左上角按钮设置Graphvi系统路径;\n"
                            "3.如果路径设置错误将不能获得正确结果，但不提示;\n"
                            "4.如果不满足以上条件将无法正常显示结果，但不影响转换得到的c语言词法分析程序。\n");

}

myWidget::~myWidget()
{
    delete ui;
}

//选择文件并输出内容到lineEdit
void myWidget::on_chiose_Button_clicked()
{
    ui->regxText->clear();
    QString path = QFileDialog::getOpenFileName(this,
                                                    tr("选择文件"),
                                                    tr("D:\\qt\Experiment_2\\XLEX_Generaor\\reText"),
                                                    "all file(*.*);;"
                                                    "text(*.txt);;"
                                                    );

    if(path.isEmpty() == false){
        //文件对象
        QFile file(path);

        //打开文件，只读
        bool isOk = file.open(QIODevice::ReadOnly);
        if(isOk == true){

            QByteArray array1;
            while(file.atEnd() == false){

                //按行读取文本并输出到文本框
                array1 = file.readLine();
                ui->regxText->setText(array1);
            }
        }
        file.close();
        //qDebug()<<"读取完成";
    }
}

//保存lineEdit内容
void myWidget::on_save_Button_clicked()
{
    QString inputText = ui->regxText->text();
    if(inputText.isEmpty() == false){
        QString filepath = QFileDialog::getSaveFileName(this,"save","D:\\qt\Experiment_2\\XLEX_Generaor\\reText\\", "TXT(*.txt)");
        if(filepath.isEmpty() == false){

            //文件对象
            QFile file;
            file.setFileName(filepath);
            bool isOk = file.open(QIODevice::WriteOnly);
            if(isOk == true){
                file.write(inputText.toStdString().data());
            }

            file.close();
        }
        //qDebug()<<"写入完成";
    }
    else{
        QMessageBox msg;
        msg.about(this,"提示","输入为空，不允许保存！");
        //qDebug()<<"内容为空";
    }
}

//将正在表达式进行转化（NFA DFA DFA最小化）和输出词法分析程序
void myWidget::on_determine_Button_clicked()
{

    Lex lexTest;
    lexTest.setDot();
    ui->textBrowser->clear();

    string regxInput = ui->regxText->text().toStdString();

    lexTest.getNFA(regxInput);
    lexTest.generateNFADotString(lexTest.lexNFA.NFAGraph);

    lexTest.getDFA();
    lexTest.generateDFADotString(lexTest.lexDFA.DFAGraph,0);

    lexTest.minimizeDFA();
    lexTest.generateDFADotString(lexTest.lexDFA.DFAGraph,1);

    lexTest.generateCCode(lexTest.lexDFA.DFAGraph);

}

//获取状态图
QString myWidget::getImageSrc(string fileName){
    string resultString = projectFile + string("\\images\\") + fileName;
    cout << "当前图片路径为" << resultString << endl;
    return QString::fromStdString(resultString);
}

void myWidget::on_NFA_Button_clicked()
{
    //输出NFA
    QTextImageFormat format;
    format.setName(getImageSrc("nfa.jpg"));
    ui->textBrowser->clear();
    ui->textBrowser->textCursor().insertImage(format);
}

void myWidget::on_DFA_Button_clicked()
{
    //输出DFA
    QTextImageFormat format;
    format.setName(getImageSrc("dfa.jpg"));
    ui->textBrowser->clear();
    ui->textBrowser->textCursor().insertImage(format);
}

void myWidget::on_MIN_DFA_Button_clicked()
{
    //输出最小化DFA
    QTextImageFormat format;
    format.setName(getImageSrc("mindfa.jpg"));
    ui->textBrowser->clear();
    ui->textBrowser->textCursor().insertImage(format);
}

void myWidget::on_pushButton_clicked()
{
    //c语言程序代码
    ui->textBrowser->clear();

    QFile file("D:\\qt\\Experiment_2\\XLEX_Generaor\\cCode.txt");
    bool isOk = file.open(QIODevice::ReadOnly|QIODevice::Text);
    if(isOk == true){
        QByteArray arr;
        while(file.atEnd() == false){

            //按行读取文本
            arr = file.readLine();
            ui->textBrowser->moveCursor(QTextCursor::End);
            ui->textBrowser->textCursor().insertText(arr);
        }
    }
    file.close();
}

void myWidget::on_reset_Button_clicked()
{
    //重置
    ui->regxText->clear();
    ui->textBrowser->clear();

    //删除生成的dot、图片和cCode
    QFile fileTemp1("D:\\qt\\Experiment_2\\XLEX_Generaor\\dots\\dfa.dot");
    fileTemp1.remove();
    QFile fileTemp2("D:\\qt\\Experiment_2\\XLEX_Generaor\\dots\\mindfa.dot");
    fileTemp2.remove();
    QFile fileTemp3("D:\\qt\\Experiment_2\\XLEX_Generaor\\dots\\nfa.dot");
    fileTemp3.remove();
    QFile fileTemp4("D:\\qt\\Experiment_2\\XLEX_Generaor\\images\\dfa.jpg");
    fileTemp4.remove();
    QFile fileTemp5("D:\\qt\\Experiment_2\\XLEX_Generaor\\images\\mindfa.jpg");
    fileTemp5.remove();
    QFile fileTemp6("D:\\qt\\Experiment_2\\XLEX_Generaor\\images\\nfa.jpg");
    fileTemp6.remove();
    QFile fileTemp7("D:\\qt\\Experiment_2\\XLEX_Generaor\\cCode.txt");
    fileTemp7.remove();
}

void myWidget::on_quit_clicked()
{
    //删除生成的dot、图片和cCode
    QFile fileTemp1("D:\\qt\\Experiment_2\\XLEX_Generaor\\dots\\dfa.dot");
    fileTemp1.remove();
    QFile fileTemp2("D:\\qt\\Experiment_2\\XLEX_Generaor\\dots\\mindfa.dot");
    fileTemp2.remove();
    QFile fileTemp3("D:\\qt\\Experiment_2\\XLEX_Generaor\\dots\\nfa.dot");
    fileTemp3.remove();
    QFile fileTemp4("D:\\qt\\Experiment_2\\XLEX_Generaor\\images\\dfa.jpg");
    fileTemp4.remove();
    QFile fileTemp5("D:\\qt\\Experiment_2\\XLEX_Generaor\\images\\mindfa.jpg");
    fileTemp5.remove();
    QFile fileTemp6("D:\\qt\\Experiment_2\\XLEX_Generaor\\images\\nfa.jpg");
    fileTemp6.remove();
    QFile fileTemp7("D:\\qt\\Experiment_2\\XLEX_Generaor\\cCode.txt");
    fileTemp7.remove();

    //关闭窗口
    close();
}


//环境变量路径
void myWidget::on_path_Button_clicked()
{
    bool isOK;
    QString text = QInputDialog::getText(NULL, "Path", "input path:\n如：D:\\qt\\dot\\bin（需处理转义字符）", QLineEdit::Normal, "your path", &isOK);
    if(isOK && !text.isEmpty()){
        //文件对象
        QFile file("D:\\qt\\Experiment_2\\XLEX_Generaor\\path.txt");

        //打开文件，只写
        bool isok = file.open(QIODevice::WriteOnly);
        if(isok == true){
            file.write(text.toStdString().data());
            cout<<text.toStdString()<<endl;
        }
        file.close();
    }
}

