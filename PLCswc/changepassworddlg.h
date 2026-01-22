#ifndef CHANGEPASSWORDDLG_H
#define CHANGEPASSWORDDLG_H

#include <QDialog>

namespace Ui {
class ChangePasswordDlg;
}

class ChangePasswordDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePasswordDlg(QWidget *parent = nullptr);
    ~ChangePasswordDlg();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ChangePasswordDlg *ui;
};

#endif // CHANGEPASSWORDDLG_H
