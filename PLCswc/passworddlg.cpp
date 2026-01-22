#include "passworddlg.h"
#include "ui_passworddlg.h"
#include "QMessageBox"
#include "qpushbutton.h"

PasswordDlg::PasswordDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordDlg)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");   //将按钮改成中文
    //setWindowTitle("密码");
}

PasswordDlg::~PasswordDlg()
{
    delete ui;
}

void PasswordDlg::on_buttonBox_accepted()
{
    QString password = ui->lineEdit->text();
    if(password == tr("yafeilinux") || password == tr("image"))
        {
           accept();
        } else {
           QMessageBox::warning(this, tr("Waring"),
                                 tr("密码错误!"),
                                 QMessageBox::Yes);
           reject();
        }
}
