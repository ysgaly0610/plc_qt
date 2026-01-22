#include "changepassworddlg.h"
#include "ui_changepassworddlg.h"
#include "QPushButton"

ChangePasswordDlg::ChangePasswordDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePasswordDlg)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");   //将按钮改成中文
}

ChangePasswordDlg::~ChangePasswordDlg()
{
    delete ui;
}

void ChangePasswordDlg::on_buttonBox_accepted()
{

}
