#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>

namespace Ui {
class SetUpDialog;
}

class SetUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetUpDialog(QWidget *parent = nullptr);
    ~SetUpDialog();
    QString readInI(QString Key, QString value);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_buttonBox_accepted();
    QString Read_Config_InI(QString __Key_root, QString __Key_value);

    void on_pushButton_3_clicked();

    //void on_lineEdit_4_cursorPositionChanged(int arg1, int arg2);

private:
    Ui::SetUpDialog *ui;
};

#endif // SETUPDIALOG_H
