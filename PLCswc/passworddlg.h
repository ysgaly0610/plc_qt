#ifndef PASSWORDDLG_H
#define PASSWORDDLG_H

#include <QDialog>

namespace Ui {
class PasswordDlg;
}

class PasswordDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDlg(QWidget *parent = nullptr);
    ~PasswordDlg();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::PasswordDlg *ui;
    QString CurrentPassword;
};

#endif // PASSWORDDLG_H
