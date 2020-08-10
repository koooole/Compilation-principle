#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:

    void on_open_pushButton_clicked();

    void on_save_pushButton_clicked();

    void on_reset_pushButton_clicked();

    void on_analys_pushButton_clicked();

    void on_cheak_pushButton_clicked();

    void on_view_pushButton_clicked();

private:
    Ui::Widget *ui;
    QString openfile;
    QString check_str;
    QString gra_str;
    QString state_str;
    QString waitto_analyse_sentence;
};

#endif // WIDGET_H
