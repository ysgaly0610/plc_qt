#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "actutltypelib.h"
#include "actprogtypelib.h"
#include "thread.h"
#include <setupdialog.h>
#include "QCloseEvent"
#include "plcthread.h"
#include "windows.h"
#include "QTimer"
#include "passworddlg.h"
#include "changepassworddlg.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace ActUtlTypeLib;
using namespace ActProgTypeLib;
extern QString global_string;


class MainWindow : public QMainWindow
{
    //信号与槽的时候需要
    Q_OBJECT   //构造的函数，按F4可以看到对应的.cpp是空的

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString m_return_edit;
    QString m_retval1;            //获取数值显示控件
    QString m_retval2;
    ActUtlType utl;   //两个控件类的对象实例化
    ActProgType prog;
    //friend class iMesThread;   //声明友元
    void ConnectPLC(QString Addr);
    void SetDevicePLC(bool OkorNG, QString QRcode);
    void fastmode();
    void testFunction();


private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void Setvalue2PLC(QString ClearCode);
    void Initial();
    void Read_Xml(QString xml_path);
    void on_pushButton_4_clicked();
    void closeEvent(QCloseEvent *event);    // 重写closeEvent的申明
    QString Dec2Bin(int nDec);
    QString Bin2Dec(QString bin);






private:
    Ui::MainWindow *ui;
    iMesThread iMesImage;
    SetUpDialog *childWindow;
    PasswordDlg *passwordDlg;
    ChangePasswordDlg *changepasswordDlg;
    PlcThread Inlist;
    PlcThreadNew BoardResult;
    PlcThread *test;
    PlcThreadFastMode fast_mode;
    int g_Time;
    QTimer *timer;

private slots:     //自定义信号槽函数
    void set_lineEdit_text(QString);
    void set_workID(QString string);
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_lineEdit_5_editingFinished();
    void set_textBrowser_text(QString string);

    void on_lineEdit_6_textEdited(const QString &arg1);
    void on_radioButton_2_clicked();
    void showTimelimit();
    void on_radioButton_clicked();

    void on_pushButton_Modify_Password_clicked();
    void on_checkBox_stateChanged(int arg1);


signals:
    void signal_end_iMes(QString);   //信号函数
    //给子线程发消息
    void sengMsgToThread(QString);
};
#endif // MAINWINDOW_H
