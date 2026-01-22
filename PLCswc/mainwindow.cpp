#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "actutltypelib.h"
#include "actprogtypelib.h"
#include <QDebug>  //打印
#include <QDialog>
#include "QMessageBox"
#include "windows.h"
#include "thread.h"
#include "stdio.h"
#include "config.h"
#include "QFileInfo"
#include "plcthread.h"
#include "QQueue"
#include <QFileDialog>
#include "QDirIterator"

using namespace ActUtlTypeLib;
using namespace ActProgTypeLib;
BOOL IsFirst = true;
BOOL g_bIsFirst = true;
//PasswordDlg passwordDlg;
//ChangePasswordDlg changepasswordDlg;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建线程
    test = new PlcThread;  //分配空间
    timer = new QTimer(this);
    timer->stop();
    timer->setInterval (1000) ;//设置定时周期，单位：毫秒
    g_Time = childWindow->readInI("TIME","Delay").toInt();
    connect(timer,SIGNAL(timeout()),this,SLOT(showTimelimit()));
    Initial();
    //connect(sender, signal, receiver, slot); connect(信号发出者，信号发送者发出的信号，信号接收者，接收对象在接收到信号之后所需要调用的函数——槽函数)
    //信号槽要求信号和槽的参数一致，所谓一致，是参数类型一致。

    connect(&iMesImage,SIGNAL(signal_add_int(QString)),this,SLOT(set_lineEdit_text(QString)),Qt::QueuedConnection);
    connect(&iMesImage,SIGNAL(signal_for_mainwindow(QString)),this,SLOT(set_workID(QString)),Qt::QueuedConnection);    //工单
    connect(&Inlist,SIGNAL(PLCsignal_for_mainwindow(QString)),this,SLOT(set_textBrowser_text(QString)),Qt::QueuedConnection);
    connect(&BoardResult,SIGNAL(PLCNewSignal_for_mainwindow(QString)),this,SLOT(set_textBrowser_text(QString)),Qt::QueuedConnection);
    connect(&fast_mode,SIGNAL(PLCFastModeSignal_for_mainwindow(QString)),this,SLOT(set_textBrowser_text(QString)),Qt::QueuedConnection);
    //我们用“转到槽”的方法建立槽函数时Qt已经将我们的连接做好了而我又在构造函数中对槽函数进行了连接，所以导致槽函数执行了两次。

    //主程序发数据给子线程
    connect(this, &MainWindow::sengMsgToThread, test, &PlcThread::recMegFromMain);
}

MainWindow::~MainWindow()
{
    iMesImage.terminate();     //终止线程
    iMesImage.wait();
    Inlist.terminate();
    Inlist.wait();
    BoardResult.terminate();
    BoardResult.wait();
    delete ui;
}

void MainWindow::Initial()
{
    QTime current_time =QTime::currentTime();
    int hour = current_time.hour();//当前的小时
    int minute = current_time.minute();//当前的分
    int second = current_time.second();//当前的秒
    int msec = current_time.msec();//当前的毫秒

    QFileInfo fi;
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit_2->setReadOnly(true);
    QString config_path = QCoreApplication::applicationDirPath() + "/Config.ini";
    fi.setFile(config_path);
    bool res = fi.isFile();
    if(res)
    {
        QString qstrip = Config().Get("COMMUNICATION","IP").toString();
        QString qstrport = Config().Get("COMMUNICATION","Port").toString();
        QString RunMode = Config().Get("MODE","Mode").toString();
        QString FastMode = Config().Get("MODE","FastMode").toString();
        ui->lineEdit_5->setText(qstrip);
        ui->lineEdit_6->setText(qstrport);
        if (RunMode == "1")
            ui->radioButton_2->setChecked(true);
        else if (RunMode == "0"){
            ui->radioButton->setChecked(true);
        }
        if (FastMode == "1")
        {
            ui->checkBox->setChecked(true);
            ui->radioButton->setEnabled(false);
            ui->radioButton_2->setEnabled(false);
            ui->pushButton->setEnabled(false);
        }
    }
    else
    {
        ui->lineEdit_5->setText("127.0.0.1");
        ui->lineEdit_6->setText("8090");
        Config().Set("COMMUNICATION","IP","127.0.0.1");
        Config().Set("COMMUNICATION","Port","8090");
        Config().Set("MODE","Mode","2");    // 2为无状态
        Config().Set("MODE","FastMode","2");    // 2为无状态
    }
    g_bIsFirst = false;
}

void MainWindow::on_pushButton_clicked()    //连接收板机按钮
{
    long lRet;
    QString m_return_edit = "";   //界面上的控件，用于显示操作返回值，返回0代表操作成功
    try {
        //ActProgType Control
        if (ui->radioButton->isChecked()){           //ActProgType模式
            //lRet = prog.Close();
            lRet = prog.Open();
            qDebug() << lRet << endl;
        }
        else if (ui->radioButton_2->isChecked())
        {
            utl.SetActLogicalStationNumber(1);   //此处的参数1就是我们在MX COMPONENT中设置向导里面设置的逻辑站号1，实际设置的数字是几这里就是几
            lRet = utl.Open();
            ui->textBrowser->insertPlainText("UTL模式：" + QString::number(lRet) + "\n");
            ui->textBrowser->moveCursor(QTextCursor::End);
            //int lvalue;
            //utl.GetDevice("D10210",lvalue);
            //ui->textBrowser->insertPlainText("D10210：" + QString::number(lvalue) + "\n");
            //ui->textBrowser->moveCursor(QTextCursor::End);
            //qDebug() << lRet << endl;
            /*
            //获取CPU类型
            QString str;
            str = str.setNum(lRet,16).toUpper();//转为十六进制
            ui->textBrowser->setText(QString("0x%1").arg(str) + "\n");
            ui->textBrowser->insertPlainText("UTL模式打开CPU结果:" + str + "\n");
            ui->textBrowser->moveCursor(QTextCursor::End);

            QString szCpu;
            int sValue;
            utl.GetCpuType(szCpu,sValue);
            ui->textBrowser->insertPlainText("UTL模式获取CPU类型:" + szCpu + "返回数值:" + QString(sValue) + "\n");
            ui->textBrowser->moveCursor(QTextCursor::End);
            */
            //qDebug() << lRet << endl;
        }
        else
        {
            QString dlgTitle="warning 消息框";
            QString strInfo="请选择通信模式！";
            QMessageBox::warning(this,dlgTitle,strInfo);
            return;
        }
        if (lRet == 0)
        {
            ui->textBrowser->insertPlainText("收板机连接成功！\n");
            ui->textBrowser->moveCursor(QTextCursor::End);
            iMesImage.WriteLog("收板机连接成功！");


        }
        else
        {
            ui->textBrowser->insertPlainText("收板机连接失败:" + QString::number(lRet) + "请检查通讯是否正常···");
            ui->textBrowser->moveCursor(QTextCursor::End);
            iMesImage.WriteLog("收板机连接失败:" + QString::number(lRet) + "请检查通讯是否正常···");
        }

    } catch (EXCEPTION_POINTERS *pException) {
        QString dlgTitle="warning 消息框";
        QString strInfo="设备连接失败！";
        QMessageBox::warning(this,dlgTitle,strInfo);
    }
}

void MainWindow::on_pushButton_2_clicked()   //开始按钮
{
    QString FastMode = Config().Get("MODE","FastMode").toString();
    if(FastMode == "1")    // 快速机模式
    {
        fastmode();
        if (IsFirst)    //是否第一次开启线程？
        {
            fast_mode.FastModeEndSignal = false;
            fast_mode.start();
            IsFirst = false;
            ui->textBrowser->insertPlainText("已开启【快速模式】分板线程\n");
            ui->textBrowser->moveCursor(QTextCursor::End);
            iMesImage.WriteLog("已开启【快速模式】分板线程");
        }
        ConnectPLC("D10210");
    }
    else {
        if (IsFirst)    //是否第一次开启线程？
        {
            Inlist.EndSignal = false;
            Inlist.start();
            BoardResult.EndSignal = false;
            BoardResult.start();
            IsFirst = false;
            ui->textBrowser->insertPlainText("已开启分板线程\n");
            ui->textBrowser->moveCursor(QTextCursor::End);
            iMesImage.WriteLog("已开启分板线程");
            timer->start();
        }
        ConnectPLC("D10210");
    }
}



void MainWindow::ConnectPLC(QString Addr)
{
    long lRet = -1;
    int nValue = -1;
    QString strCode,str;
    QString m_devicename_edit = Addr;
    QString FastMode = Config().Get("MODE","FastMode").toString();
    if (FastMode == "1")
    {
        lRet = utl.GetDevice(m_devicename_edit, nValue);
        //ui->textBrowser->insertPlainText("FastMode连接模式状态：" + QString::number(lRet) + "\n");
    }
    else
    {
        if(ui->radioButton->isChecked()){     // prog连接模式
            lRet = prog.GetDevice(m_devicename_edit, nValue);
            //ui->textBrowser->insertPlainText("prog连接模式状态：" + QString::number(lRet) + "\n");
        }
        else{                                //utl连接模式
            lRet = utl.GetDevice(m_devicename_edit, nValue);
            ui->textBrowser->insertPlainText("UTL连接模式状态：" + QString::number(lRet) + "起始地址：" + m_devicename_edit + "回传变量：" + QString::number(nValue) + "\n");
            QString Dec2Bin = QString::number(nValue,2);
            QString szName;
            int sValue;
            utl.GetCpuType(szName,sValue);
            ui->textBrowser->insertPlainText("CPU类型：" + szName + "\n");
            //utl.SetDevice(m_devicename_edit, m_nSetValue);
        }
    }

    if(lRet == 0x00){	// 返回值为0，说明获取成功 并且D10210的值为1再去读取寄存地址的值
        str = QString::number(nValue);
        ui->lineEdit->setText(m_devicename_edit);
        ui->lineEdit_2->setText(str);	// Device Value
        if(1)
        {
            QString sDecode;
            for (int i = 0;i<7;i++)
            {
                QString addNum = QString::number(i);
                QString m_Device = "D1020" + addNum;
                if (FastMode == "1")
                {
                    lRet = utl.GetDevice(m_Device, nValue);
                }
                else
                {
                    if(ui->radioButton->isChecked())
                        lRet = prog.GetDevice(m_Device, nValue);
                    else
                        lRet = utl.GetDevice(m_Device, nValue);
                }

                sDecode += QString::number(nValue) + ",";
                QString Dec2Bin = QString::number(nValue,2);    //将寄存器地址值十进制数转为2进制
                while (TRUE)     // 不足位数，前面补0
                {
                    if (Dec2Bin.size()<16)
                        Dec2Bin = "0"+Dec2Bin;
                    else {
                        break;
                    }
                }
                QString LowBit = Dec2Bin.left(8);      //截取前面低八位
                QString HighBit = Dec2Bin.right(8);    //截取后面高八位
                QString strHighBit = Bin2Dec(HighBit); //2进制数转10进制
                QString strLowBit = Bin2Dec(LowBit);
                // 10进制转ASCII
                int dec = strHighBit.toInt();          // QString2Int
                char ch = dec;
                QString decToStr = QString(ch);
                strCode = strCode + decToStr;
                int dec2 = strLowBit.toInt();
                char ch1 = dec2;
                QString decToStr1 = QString(ch1);
                strCode = strCode + decToStr1;

            }

            ui->textBrowser->insertPlainText("接收到读码器二维码：" +strCode+"::" + QString::number(strCode.size()) + "\n");
            //二维码错误1
            //if(strCode.indexOf(QChar::Null) == 13)
            //    strCode.truncate(strCode.indexOf(QChar::Null));
            //QString sExp = R"(\0)";
            //strCode = strCode.split(sExp).at(0);
            std::string strc = strCode.toStdString();
            strCode = strc.c_str();
            //iMesImage.WriteLog("qudiao去掉杠灵：" + strCode);
            strCode = strCode.trimmed();
            //iMesImage.WriteLog("去空格" + strCode);
            if (strCode.size() < 13)
            {
                ui->textBrowser->insertPlainText("二维码错误！："+strCode+"\n");
                //iMesImage.WriteLog("二维码错误！："+strCode + "::" + QString::number(strCode.size()));
            }
            else if (strCode.contains("    ",Qt::CaseSensitive)) {    // 空的字符串不加入二维码队列
                qDebug()<< strCode<<endl;
                iMesImage.WriteLog("二维码错误,包含多个空格！："+strCode);
            }
            else
            {
                strCode = strCode.simplified();
                if(strCode != "")
                {
                    emit sengMsgToThread(strCode.trimmed());    // 将收到的二维码发送给子线程，压入list
                    iMesImage.WriteLog("305接收到读码器二维码："+strCode);
                }
            }
            ui->textBrowser->moveCursor(QTextCursor::End);
        }
    }
    else {
        str = QString::number(nValue);
        ui->lineEdit->setText(m_devicename_edit);
        ui->lineEdit_2->setText(str);	// Device Value
        ui->textBrowser->insertPlainText("分板线程开启失败！\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
        iMesImage.WriteLog("分板线程开启失败！");
    }
}

void MainWindow::on_pushButton_3_clicked()   //停止按钮
{
    //Setvalue();
    Inlist.EndSignal = true;
    Inlist.quit();
    IsFirst = TRUE;

    fast_mode.FastModeEndSignal = true;
    fast_mode.quit();

    ui->textBrowser->insertPlainText("分板离线模式已关闭……\n");
    ui->textBrowser->moveCursor(QTextCursor::End);
    iMesImage.WriteLog("分板离线模式已关闭……");

}


void MainWindow::on_pushButton_4_clicked()
{
    testFunction();
    //启动一个线程用于执行函数SetiMes.
    iMesImage.EndSignal = false;
    iMesImage.start();
}

//【断开】按钮
void MainWindow::on_pushButton_5_clicked()
{
    iMesImage.EndSignal = true;
    iMesImage.quit();
    ui->textBrowser_2->insertPlainText("iMes线程已停止……\n");
    ui->textBrowser_2->moveCursor(QTextCursor::End);
}

// 设置
void MainWindow::on_pushButton_6_clicked()
{
    passwordDlg = new PasswordDlg();
    if(passwordDlg->exec() == QDialog::Accepted)
    {
        childWindow = new SetUpDialog();
        childWindow->setModal(true);    //设置成模态对话框
        childWindow->show();
    }
    else
    {
        passwordDlg->setModal(true);    //设置成模态对话框
        passwordDlg->show();    // 会重复显示
        return;
    }
}


void MainWindow::Setvalue2PLC(QString ClearCode)       //给寄存器设置值
{
    QString FastMode = Config().Get("MODE","FastMode").toString();
    ui->textBrowser->insertPlainText("给出明码信息："+ClearCode+"\n");
    ui->textBrowser->moveCursor(QTextCursor::End);
    long lRet;
    QVector<uint>v;
    QQueue<int> nCode;
    QString	MsgStr,sHigth,sLow,tempStr;
    m_return_edit = "";
    m_retval1 = "";            //获取数值显示控件
    m_retval2 = "";            //此控件用于显示PLC型号，此函数没用
    QString m_devicename_edit = "D10211";
    //将二维码字符串逐字符压入向量V中
    for (int i = 0;i < ClearCode.length(); i++)
    {
        QString chr = ClearCode.mid(i,1);
        //ui->textBrowser->insertPlainText("给出明码信息字符："+chr+"\n");
        v.append(chr.toUcs4());    //ASCII转10进制
    }

    for (int i=0;i<v.size();i +=2)
    {
        sHigth = Dec2Bin(v[i]);   //高位  10进制转2进制
        if ((i+1)<v.size())
            sLow = Dec2Bin(v[i+1]);   //低位
        else
            sLow = "00000000";

        QString strDec = Bin2Dec(sLow+sHigth);    // 高低位换位后，2进制转10进制
        nCode.append(strDec.toInt());   //压入队列
    }

    for (int i = 1;i<8;i++)   //(int i = 1; i < 7; i++)
    {
        int lValue = nCode.dequeue();   //删除队列第一个元素，并返回这个元素
        MsgStr = QString::number(lValue);
        QString addNum = QString::number(i);
        QString m_Device = "D1021" + addNum;
        if (FastMode == "1")
        {
            lRet = utl.SetDevice(m_Device, lValue);    // 快速机模式
        }
        else {
            if(ui->radioButton->isChecked())
                lRet = prog.SetDevice(m_Device, lValue);
            else
                lRet = utl.SetDevice(m_Device, lValue);
        }

        if(lRet == 0x00){	// 返回值为0，说明设置成功。
            tempStr = m_Device+" 写入地址成功！";
            qDebug()<< tempStr<<endl;
            //ui->textBrowser->insertPlainText(tempStr);
            iMesImage.WriteLog(tempStr+MsgStr);
        }
        else{
            tempStr = m_Device+" 写入地址失败！";
            qDebug()<< tempStr<<endl;
            ui->textBrowser->insertPlainText(tempStr);
            iMesImage.WriteLog(tempStr+MsgStr);
        }
    }
}


void MainWindow::set_lineEdit_text(QString string)     //iMes信息交互显示框
{
    string = string + "\n";
    ui->textBrowser_2->insertPlainText(string);
    ui->textBrowser_2->moveCursor(QTextCursor::End);
}

void MainWindow::set_textBrowser_text(QString string)     //PLC信息交互显示框
{
    QString FastMode = Config().Get("MODE","FastMode").toString();
    if (FastMode != "1")
        ui->pushButton_2->clicked(true);   // 每接收一条消息，去扫描一次读码器
    QString temp = string.left(1);
    if (temp == "O")
    {
        SetDevicePLC(true, string.right(string.length()-1));
    }
    else if (temp == "N")
        SetDevicePLC(false, string.right(string.length()-1));
    else if (string == "Image")
        qDebug()<< string <<endl;
    else if (string == "S")     // S为启动定时器
        timer->start();
    else
    {
        string = string + "\n";
        ui->textBrowser->insertPlainText(string);
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
}

void MainWindow::set_workID(QString string)     //工单
{
    QStringList list = string.split(",");
    QString a = list[0];
    QString b = list[1];
    ui->lineEdit_3->setText(a);
    ui->lineEdit_4->setText(b);
}

#include "tinyxml.h"

void MainWindow::Read_Xml(QString xml_path)
{
    TiXmlDocument mydoc("E:\\Qt\\build-PLC-Desktop_Qt_5_14_2_MinGW_32_bit-Debug\\debug\\123.xml");//xml文档对象
    bool loadOk=mydoc.LoadFile();//加载文档
    if(!loadOk)
    {
        qDebug()<<"could not load the test file.Error:"<<mydoc.ErrorDesc()<<endl;
        exit(1);
    }

    TiXmlElement *RootElement=mydoc.RootElement();	//根元素, Info
    qDebug()<< "[root name]" << RootElement->Value() <<"[wipEntityId]" << RootElement->Attribute("wipEntityId") <<"[macCode]" << RootElement->Attribute("macCode") <<"\n";

    TiXmlElement *pEle=RootElement;

    //遍历该结点
    for(TiXmlElement *StuElement = pEle->FirstChildElement();//第一个子元素
        StuElement != NULL;
        StuElement = StuElement->NextSiblingElement())//下一个兄弟元素
    {
        // StuElement->Value() 节点名称
        qDebug()<< StuElement->Value() <<" ";
        TiXmlAttribute *pAttr=StuElement->FirstAttribute();//第一个属性
        qDebug()<< "[root name]" << pAttr->Value() <<"[tagCode]" << StuElement->Attribute("tagCode") <<"[tagValue]" << StuElement->Attribute("tagValue");
        while( NULL != pAttr) //输出所有属性
        {
            qDebug()<<pAttr->Name()<<":"<<pAttr->Value()<<" ";
            pAttr=pAttr->Next();
        }
        qDebug()<<endl;

        //输出子元素的值
        for(TiXmlElement *sonElement=StuElement->FirstChildElement();
            sonElement;
            sonElement=sonElement->NextSiblingElement())
        {
            qDebug()<<sonElement->FirstChild()->Value()<<endl;
        }
    }
}


void MainWindow::on_lineEdit_5_editingFinished()
{
    QString SetIp = ui->lineEdit_5->text();
    Config().Set("COMMUNICATION","IP",SetIp);
}


void MainWindow::on_lineEdit_6_textEdited(const QString &arg1)    //与textChanged()不同，当以编程方式更改文本时，例如，通过调用setText()不会发出此信号。
{
    QString SetPort = ui->lineEdit_6->text();
    Config().Set("COMMUNICATION","Port",SetPort);
}


// 重写closeEvent函数
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this,tr("退出程序"),QString(tr("确认退出程序")),QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::No)
    {
        event->ignore(); // 忽略退出信号，程序继续进行
    }
    else if(button==QMessageBox::Yes)
    {
        iMesImage.EndSignal = true;
        iMesImage.quit();
        Inlist.EndSignal = true;
        Inlist.quit();
        BoardResult.EndSignal = true;
        BoardResult.quit();

        event->accept(); // 接受退出信号，程序退出
    }
}


void MainWindow::on_radioButton_2_clicked()
{
    Config().Set("MODE","Mode","1");
}

void MainWindow::on_radioButton_clicked()
{
    Config().Set("MODE","Mode","0");
}


//给分板机信号
void MainWindow::SetDevicePLC(bool OkorNG, QString QRcode)
{
    long lRet;
    int nValue;
    QString	MsgStr,str;
    QString device = "D10221";
    Setvalue2PLC(QRcode);
    QString FastMode = Config().Get("MODE","FastMode").toString();

    if (OkorNG)   //OK写入1
    {
        nValue = 1;
        if (FastMode == "1")
        {
            lRet = utl.SetDevice(device, nValue);
        }
        else
        {
            if(ui->radioButton->isChecked()){     // prog连接模式
                lRet = prog.SetDevice(device, nValue);
            }
            else{                                //utl连接模式
                lRet = utl.SetDevice(device, nValue);
            }
        }
    }
    else {      //NG写入2
        nValue = 2;
        if (FastMode == "1")
        {
            lRet = utl.SetDevice(device, nValue);
        }
        else
        {
            if(ui->radioButton->isChecked()){     // prog连接模式
                lRet = prog.SetDevice(device, nValue);
            }
            else{                                //utl连接模式
                lRet = utl.SetDevice(device, nValue);
            }
        }

    }
    OkorNG?MsgStr = "OK":MsgStr ="NG";
    if(lRet == 0x00){	// 返回值为0，说明设置成功。
        //信号给出成功
        str = QString::number(nValue);
        MsgStr = QRcode+"信号给出成功！"+MsgStr;
        ui->textBrowser->insertPlainText(MsgStr+"\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
        ui->lineEdit->setText(device);
        ui->lineEdit_2->setText(str);
        iMesImage.WriteLog(MsgStr);
    }
    else{
        //信号给出失败
        MsgStr = QRcode+"信号给出失败！读写PLC问题";
        str = QString::number(nValue);
        ui->textBrowser->insertPlainText(MsgStr+"\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
        ui->lineEdit->setText(device);
        ui->lineEdit_2->setText(str);
        iMesImage.WriteLog(MsgStr);
    }

    try {  //清空要板信号
        QString m_Device = "D10210";
        nValue = 1;
        lRet = utl.SetDevice(m_Device,nValue);
        ui->textBrowser->insertPlainText("清空要板信号：D10210\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
    } catch (EXCEPTION_POINTERS *pException) {
        qDebug()<< pException <<endl;
    }
}



QString MainWindow::Dec2Bin(int nDec){  //十进制转二进制
    int nYushu, nShang;
    QString strBin, strTemp;
    //TCHAR buf[2];
    bool bContinue = true;
    while ( bContinue )
    {
        nYushu = nDec % 2;
        nShang = nDec / 2;
        strBin=QString::number(nYushu)+strBin; //qDebug()<<strBin;
        strTemp = strBin;
        //strBin.Format("%s%s", buf, strTemp);
        nDec = nShang;
        if ( nShang == 0 )
            bContinue = false;
    }
    int nTemp = strBin.length() % 4;
    switch(nTemp)
    {
    case 1:
        //strTemp.Format(_T("000%s"), strBin);
        strTemp = "000"+strBin;
        strBin = strTemp;
        break;
    case 2:
        //strTemp.Format(_T("00%s"), strBin);
        strTemp = "00"+strBin;
        strBin = strTemp;
        break;
    case 3:
        //strTemp.Format(_T("0%s"), strBin);
        strTemp = "0"+strBin;
        strBin = strTemp;
        break;
    default:
        break;
    }
    return strBin;
}

QString MainWindow::Bin2Dec(QString bin)    // 二进制转十进制
{
    QString decimal;
    int nDec = 0;
    int nLen,i,j,k;
    nLen = bin.length();
    for (i = 0 ;i < nLen; i++) {
        if (bin[nLen-i-1] == "0")
            continue;
        else
        {
            k =1;
            for (j = 0;j < i;j++) {
                k = k*2;
            }
            nDec += k;
        }
    }
    decimal = QString::number(nDec);
    return decimal;
}

void MainWindow::showTimelimit()
{
    if(g_Time >= 0)
    {
        QString num = QString::number(g_Time);
        ui->label_9->setText(num+"S");
        g_Time = g_Time - 1; //注意字符类型
    }
    else
    {
        g_Time = childWindow->readInI("TIME","Delay").toInt();
    }
}


void MainWindow::on_pushButton_Modify_Password_clicked()
{
    changepasswordDlg = new ChangePasswordDlg;
    changepasswordDlg->setModal(true);    //设置成模态对话框
    changepasswordDlg->show();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1 && !g_bIsFirst)
    {  // 选中
        QString dlgTitle="warning";
        QString strInfo="是否切换到快速机模式？";
        QMessageBox::warning(this,dlgTitle,strInfo);
        Config().Set("MODE","FastMode","1");
        ui->radioButton_2->setAutoExclusive(false);
        ui->radioButton_2->setChecked(false);   // 快速机模式取消两个radioButton
        ui->radioButton->setChecked(false);
        ui->radioButton->setEnabled(false);
        ui->radioButton_2->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->radioButton_2->setAutoExclusive(true);
        Config().Set("MODE","Mode","2");   // 2为无状态
    }
    else {   // 取消选中
        ui->radioButton->setEnabled(true);
        ui->radioButton_2->setEnabled(true);
        ui->pushButton->setEnabled(true);
        if(arg1 == 0)
            Config().Set("MODE","FastMode","0");
    }
}

void MainWindow::fastmode()
{
    long lRet;
    QString m_return_edit = "";   //界面上的控件，用于显示操作返回值，返回0代表操作成功
    try {
        //ActUtlType模式

        utl.SetActLogicalStationNumber(1);   //此处的参数1就是我们在MX COMPONENT中设置向导里面设置的逻辑站号1，实际设置的数字是几这里就是几
        lRet = utl.Open();

        if (lRet == 0)
        {
            ui->textBrowser->insertPlainText("【快速模式】收板机连接成功！\n");
            ui->textBrowser->moveCursor(QTextCursor::End);
            iMesImage.WriteLog("【快速模式】收板机连接成功！");
        }
        else
        {
            ui->textBrowser->insertPlainText("【快速模式】收板机连接失败！请检查通讯是否正常···");
            ui->textBrowser->moveCursor(QTextCursor::End);
            iMesImage.WriteLog("【快速模式】收板机连接失败！请检查通讯是否正常···");
        }

    } catch (EXCEPTION_POINTERS *pException) {
        QString dlgTitle="warning";
        QString strInfo="【快速模式】设备连接失败！";
        QMessageBox::warning(this,dlgTitle,strInfo);
    }
}


// 便利所有文件（包括子文件夹）
void MainWindow::testFunction()
{
    //判断路径是否存在
    QString path = "D:\\tt";
        QDir dir(path);
        if(!dir.exists())
        {
            return;
        }

        //获取所选文件类型过滤器
        QStringList filters;
        //filters<<QString("*.jpeg")<<QString("*.jpg")<<QString("*.png")<<QString("*.tiff")<<QString("*.gif")<<QString("*.bmp");
        filters<<QString("*.csv");
        //定义迭代器并设置过滤器
        QDirIterator dir_iterator(path,
            filters,
            QDir::Files | QDir::NoSymLinks,
            QDirIterator::Subdirectories);
        QStringList string_list;
        while(dir_iterator.hasNext())
        {
            dir_iterator.next();
            QFileInfo file_info = dir_iterator.fileInfo();
            QString absolute_file_path = file_info.absoluteFilePath();
            qDebug()<<absolute_file_path<<endl;
            string_list.append(absolute_file_path);
        }
}
