#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <string>

using namespace std;

namespace Ui {
class myWidget;
}

class myWidget : public QWidget
{
    Q_OBJECT

public:
    explicit myWidget(QWidget *parent = 0);
    ~myWidget();

    QString getImageSrc(string fileName);
    string projectFile;

private slots:
    void on_chiose_Button_clicked();

    void on_save_Button_clicked();

    void on_determine_Button_clicked();

    void on_NFA_Button_clicked();

    void on_DFA_Button_clicked();

    void on_MIN_DFA_Button_clicked();

    void on_reset_Button_clicked();

    void on_quit_clicked();

    void on_path_Button_clicked();

    void on_pushButton_clicked();

private:
    Ui::myWidget *ui;
};


#endif // MYWIDGET_H
