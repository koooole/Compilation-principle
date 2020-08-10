#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>

namespace Ui {
class myWidget;
}

class myWidget : public QWidget
{
    Q_OBJECT

public:
    explicit myWidget(QWidget *parent = 0);
    ~myWidget();

private slots:

    void on_choose_Button_clicked();

    void on_saveas_Button_clicked();

    void on_tree_Button_clicked();

    void on_reset_Button_clicked();

    void on_close_Button_clicked();

private:
    Ui::myWidget *ui;
    QString tinyfile;
    const QString treefile = "tree";
};

#endif // MYWIDGET_H
