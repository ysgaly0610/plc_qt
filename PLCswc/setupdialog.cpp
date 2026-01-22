#include "setupdialog.h"
#include "ui_setupdialog.h"
#include "QDebug"
#include "QFileDialog"
#include "QTime"
#include <setting.h>
#include <QMessageBox>

SetUpDialog::SetUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetUpDialog)
{
    ui->setupUi(this);
    QString QRCodePath = Read_Config_InI("PATH","QRCodePath");    //二维码路径
    QString BoardResultPath = Read_Config_InI("PATH","BoardResultPath");      //资料路径
    QString EapPath = Read_Config_InI("PATH","EapPath");    // EAP路径
    QString Timedelay = Read_Config_InI("TIME","Delay");    // 延时
    ui->lineEdit->setText(QRCodePath);
    ui->lineEdit_2->setText(BoardResultPath);
    ui->lineEdit_3->setText(EapPath);
    ui->lineEdit_4->setText(Timedelay);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");   //将按钮改成中文
}

SetUpDialog::~SetUpDialog()
{
    delete ui;
}

void SetUpDialog::on_pushButton_clicked()    //二维码读取路径
{
    QString file_path = QFileDialog::getExistingDirectory(this,"请选择二维码读取路径...","./");
    if(file_path.isEmpty())
    {
        return;
    }
    else
    {
        qDebug() << file_path << endl;
        ui->lineEdit->setText(file_path);
    }
}

void SetUpDialog::on_pushButton_2_clicked()    //检板结果资料路径
{
    QString file_path1 = QFileDialog::getExistingDirectory(this,"请选择检板结果路径...","./");
    if(file_path1.isEmpty())
    {
        return;
    }
    else
    {
        qDebug() << file_path1 << endl;
        ui->lineEdit_2->setText(file_path1);
    }
}

void SetUpDialog::on_pushButton_3_clicked()    //EAP路径
{
    QString file_path3 = QFileDialog::getExistingDirectory(this,"请选择EAP路径...","./");
    if(file_path3.isEmpty())
    {
        return;
    }
    else
    {
        qDebug() << file_path3 << endl;
        ui->lineEdit_3->setText(file_path3);
    }
}

void SetUpDialog::on_buttonBox_accepted()    //【确定】按钮
{
    QString str = ui->lineEdit_4->text();
    int tmpInt = str.toInt();
    if(ui->lineEdit->text().isEmpty() || ui->lineEdit_2->text().isEmpty() || tmpInt < 0)
    {
       QMessageBox::information(NULL,"提示","格式错误！");
       return;
    }
    else {
        Setting().SetQRCodePath(ui->lineEdit->text());
        Setting().SetBoardResultPath(ui->lineEdit_2->text());
        Setting().SetEapPath(ui->lineEdit_3->text());
        Setting().SetTimeDelay(ui->lineEdit_4->text());
    }

}

QString SetUpDialog::Read_Config_InI(QString __Key_root, QString __Key_value)
{
    QSettings *Config_ini_Read = NULL;       //分配打开ini文件的内存
    QString iniFileName = QCoreApplication::applicationDirPath() + "/iMesSetting.ini";
    Config_ini_Read = new QSettings(iniFileName,QSettings::IniFormat);
    if(Config_ini_Read == NULL)
        return NULL ;
    QString Key_value = "/" + __Key_root + "/" + __Key_value ;
    QString value = Config_ini_Read->value(Key_value).toString();
    delete Config_ini_Read;
    return value;
}

QString SetUpDialog::readInI(QString Key, QString value)
{
    QString tempStr = Read_Config_InI(Key,value);
    return tempStr;
}


