#include "plcthread.h"
#include "QObject"     //继承了QObject 类的类，才具有信号槽的能力
#include<QtCore/QtCore>
#include <queue>
#include "mainwindow.h"
#include "QDebug"
#include "setupdialog.h"
#include "windows.h"
#include "thread.h"
#include "config.h"
#include "QMessageBox"
#include <QDialog>


using namespace std;
queue<QString> CodeList;
queue<long long>  CodeListTime;
QString g_PLCtext1 = "",g_PLCtext2 = "";    //g_PLCtext1给线程1用,g_PLCtext2给线程2用
iMesThread mes;
QString g_list = "";
BOOL Isfisrt = TRUE;

// ///////// 【子线程1】 将扫描到的二维码压入队列 ////////////
PlcThread::PlcThread()
{
}


PlcThread::~PlcThread()
{
}

void PlcThread::run()
{
    ReadCode();
}

void PlcThread::recMegFromMain(QString strCode)   //接收主程序信号处理函数
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    int x = QString::compare(strCode, g_list, Qt::CaseInsensitive);    // 对比两个字符串是否相等
    if (x != 0)
    {
        CodeList.push(strCode);
        CodeListTime.push(QDateTime::currentDateTime().toMSecsSinceEpoch());
        g_list = strCode;
        int j = CodeList.size();
        QString strTemp = QString::number(j);
        strTemp = current_date+"【"+strCode+"】入队后二维码队列大小: "+strTemp;
        mes.WriteLog(strTemp);
    }
    if (CodeList.size()>10000)   //清空队列，免得内存溢出
    {
        CodeList = queue<QString>();
        CodeListTime = queue<long long>();
    }
}

void PlcThread::ReadCode()
{
    while(true)
    {
        if(EndSignal)
            break; 
        emit PLCsignal_for_mainwindow("Image");
        sleep(4);  //秒为单位
    }
}


// ////////////////////// 新线程2    //////////////////
// 根据二维码查找板结果
PlcThreadNew::PlcThreadNew()
{}

PlcThreadNew::~PlcThreadNew()
{}

void PlcThreadNew::run()
{
    while(true)
        {
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy-MM-dd HH:mm:ss");
            QString	MsgStr;
            if (Isfisrt)
            {
                int timedelay = setup.readInI("TIME","Delay").toInt();
                sleep(timedelay);    //休眠60秒
                Isfisrt = FALSE;
            }

            int j = CodeList.size();
            mes.WriteLog("当前队列大小:" + QString::number(j));
            if(j > 0)
            {
                QString a=CodeList.front();   //取队列中第一个元素
                long long b = CodeListTime.front();
                mes.WriteLog("93从队列里读取de第一个值:" + a);
                bool boardResult = FindBoardResult(a,b);      //查找检板结果,不管OK或者NG都有结果
                if (boardResult)   // 将结果发送给主程序写入PLC中
                {
                    g_PLCtext2 = "O"+a;      //OK发送“1”
                    emit PLCNewSignal_for_mainwindow(g_PLCtext2);
                }
                else {
                    g_PLCtext2 = "N"+a;
                    emit PLCNewSignal_for_mainwindow(g_PLCtext2);
                }

                CodeList.pop();     //删除队列第一个
                CodeListTime.pop(); //删除队列与二维一致的时间
                int k = CodeList.size();
                QString strTemp = QString::number(k);
                strTemp = "【"+a+"】 出队后二维码队列大小: "+strTemp;
                mes.WriteLog(strTemp);
            }
            else{
                //二维码已空
                QString b = "二维码队列已空";
                //mes.WriteLog(b);
                int timedelay = setup.readInI("TIME","Delay").toInt();
                g_PLCtext2 = "S";   // 发送S启动定时器
                emit PLCNewSignal_for_mainwindow(g_PLCtext2);
                sleep(timedelay);    //休眠60秒
            }

            if(EndSignal)
            {
                break;
            }
            sleep(2);
        }
}

bool PlcThreadNew::FindBoardResult(QString QRCode,long long nTime)
{
    mes.WriteLog("132从队列里读取de第一个值:" + QRCode);
    /*
    if(QRCode.back() == QChar::Null)
        mes.WriteLog("QRCode.back接收到的二维码最后一位为nul:" + QRCode.back());
    if(QRCode.back() == 0x00)
        mes.WriteLog("0x00接收到的二维码最后一位为nul:" + QRCode.back());
    if(QRCode.back() == '\0')
        mes.WriteLog("\0接收到的二维码最后一位为nul:" + QRCode.back());
    if(QRCode.back() == '0')
        mes.WriteLog("0接收到的二维码最后一位为nul:" + QRCode.back());
    if(QRCode.back() == 0)
        mes.WriteLog("仅0接收到的二维码最后一位为nul:" + QRCode.back());

    const char *s = QRCode.toStdString().c_str();
    mes.WriteLog("转char接收到的二维码最后一位为nul:" + QRCode.back());
    */
    //int npos = QRCode.indexOf(QChar::Null);
    //if(npos > 0)
    //    QRCode.truncate(QRCode.indexOf(QChar::Null));
    mes.WriteLog("148从队列里读取de第一个值去空后:" + QRCode);
    QString tempAnaDataA,tempAnaDataB,CheckBoardDate;
    QString boardNum,MetrialNum;
    QString path = setup.readInI("PATH","QRCodePath");    // 二维码路径
    //path = path +"\\"+ QRCode.left(13) + ".txt";
    QTime current_time =QTime::currentTime();
    //long long nTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    long long tempTime;
    path = path +"/"+ QRCode.trimmed() + ".txt";   // 移除字符串两端的空白符
    mes.WriteLog("160准备打开的二维码文件:" + path);
    QFile QRcodefile(path);

    while (1) {
        if(QRcodefile.exists())
        {
            tempTime = QDateTime::currentDateTime().toMSecsSinceEpoch() - nTime;
            break;
        }
        else if(QDateTime::currentDateTime().toMSecsSinceEpoch() - nTime > 150 * 1000)
        {
            g_PLCtext2 = "二维码【"+QRCode +"】分析超时NG,给出NG信号";
            emit PLCNewSignal_for_mainwindow(g_PLCtext2);
            mes.WriteLog(g_PLCtext2);
            return false;
        }
        mes.WriteLog("查看二维码文件是否存在:" + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch() - nTime));
        sleep(1);
    }

    int Numm = 3;
    if (QRcodefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!QRcodefile.atEnd())
        {

            if (Numm == 1)
            {
                boardNum = QRcodefile.readLine();   //板号
                mes.WriteLog("找到二维信息文件:" + boardNum);
                break;
            }
            else if (Numm == 2)        //检板日期
                CheckBoardDate = QRcodefile.readLine();
            else if (Numm == 3)
                MetrialNum = QRcodefile.readLine();   //料号
            Numm--;
        }
        QRcodefile.close();
    }
    else
    {
        g_PLCtext2 = "打开二维码文件【"+QRCode+"】失败！";
        mes.WriteLog(g_PLCtext2);
        return FALSE;
    }


    //int hour = current_time.hour();//当前的小时
    //int minute = current_time.minute();//当前的分
    //int second = current_time.second();//当前的秒
    //int msec = current_time.msec();//当前的毫秒

    QString A_vrs,B_vrs,T_true,B_true,AI_T_true,AI_B_true;
    int Num;
    bool flag1=0, flag2=0;
    QString Filepath = setup.readInI("PATH","BoardResultPath");    // Car路径
    //机器识别结果
    CheckBoardDate = CheckBoardDate.remove(QChar('\n'),Qt::CaseInsensitive);
    boardNum = boardNum.remove(QChar('\n'),Qt::CaseInsensitive);
    A_vrs = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "\\A.vrs";
    B_vrs = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "\\B.vrs";

    // 23:50至0:10左右会出现找不到板结果的情况,读取的二维码文件内容,不需要判定日期,读到啥给啥
    //if((hour == 23 && minute>50) || (hour == 0 && minute < 15))
    //{
    //    QFile file(A_vrs);
     //   if (!file.exists())    // 不存在，则去前一天的文件夹里面找
     //   {
     //       int nCheckBoardDate = CheckBoardDate.toInt()-1;
     //       CheckBoardDate = QString::number(nCheckBoardDate);
     //       A_vrs = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "\\A.vrs";
     //       B_vrs = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "\\B.vrs";
     //   }
   // }

    QFile fileA(A_vrs);
    Num = 5;
    if (fileA.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!fileA.atEnd())
        {
            Num--;
            if (Num == 0)
            {
                tempAnaDataA = fileA.readLine();
                break;
            }
        }
        fileA.close();
    }


    QFile fileB(B_vrs);
    Num = 5;
    if (fileB.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!fileB.atEnd())
        {
            Num--;
            if (Num == 0)
            {
                tempAnaDataB = fileB.readLine();;
                break;
            }
        }
        fileB.close();
    }

    tempAnaDataA = tempAnaDataA.remove(QChar('\n'),Qt::CaseInsensitive);
    tempAnaDataB = tempAnaDataB.remove(QChar('\n'),Qt::CaseInsensitive);
    if (tempAnaDataA == "0" && tempAnaDataB == "0")
    {
        flag1 = 1;  //分析OK
        g_PLCtext2 = "二维码【"+QRCode +"】 分析OK 板号:"+boardNum;
        emit PLCNewSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return TRUE;
    }
    else if(tempAnaDataA == "-1" || tempAnaDataB == "-1")
    {
        flag1 = 0;  //分析NG
        g_PLCtext2 = "二维码【"+QRCode +"】分析NG 板号:"+boardNum;
        emit PLCNewSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return false;
    }

    //AI检测
    T_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_t_True";
    B_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_b_True";

    //人工检测
    AI_T_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_t_ai";
    AI_B_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_b_ai";

    // 23:50至0:10左右会出现找不到板结果的情况
    /*
    if((hour == 23 && minute>50) || (hour == 0 && minute < 15))
    {
        QFile file(T_true);
        if (!file.exists())    // 不存在，则去前一天的文件夹里面找
        {
            int nCheckBoardDate = CheckBoardDate.toInt()-1;
            CheckBoardDate = QString::number(nCheckBoardDate);
            T_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_t_True";
            B_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_b_True";

            AI_T_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_t_ai";
            AI_B_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_b_ai";
        }
    }
    */

    mes.WriteLog("T面过AI滤文件:" + AI_T_true);
    mes.WriteLog("B面过滤文件:" + AI_B_true);

    mes.WriteLog("T面过AI滤文件:" + T_true);
    mes.WriteLog("B面过滤文件:" + B_true);

    QFile AI_A_file(AI_T_true);
    QFile AI_B_file(AI_B_true);

    QFile A_file(T_true);
    QFile B_file(B_true);

    while (1) {
        if((A_file.exists() && B_file.exists()) || (AI_A_file.exists() && AI_B_file.exists()))
            break;
        else if((QDateTime::currentDateTime().toMSecsSinceEpoch() - nTime) > (150 * 1000 - tempTime))
        {
            g_PLCtext2 = "二维码【"+QRCode +"】过滤超时NG 板号:" + boardNum;
            emit PLCNewSignal_for_mainwindow(g_PLCtext2);
            mes.WriteLog(g_PLCtext2);
            return false;
        }
        sleep(1);
    }
    mes.WriteLog("T面过滤文件大小:" + QString::number(A_file.size()));
    mes.WriteLog("B面过滤文件大小:" + QString::number(B_file.size()));

    mes.WriteLog("T面AI过滤文件大小:" + QString::number(AI_A_file.size()));
    mes.WriteLog("B面AI过滤文件大小:" + QString::number(AI_B_file.size()));
    if(A_file.size() < 8 || A_file.size() > 4008 || B_file.size() < 8 || B_file.size() > 4008 || AI_A_file.size() < 8 || AI_A_file.size() > 4008 || AI_B_file.size() < 8 || AI_B_file.size() > 4008)
    {
        sleep(2);

        QFile A1_file(T_true);
        QFile B1_file(B_true);

        QFile AI_A1_file(AI_T_true);
        QFile AI_B1_file(AI_B_true);

        if (((A1_file.size() == 8) && (B1_file.size() == 8)) || ((AI_A1_file.size() == 8) && (AI_B1_file.size() == 8)))
        {
            flag2 = 1;     //OK
            g_PLCtext2 = "二维码【"+QRCode +"】分析OK 板号:"+boardNum;
            emit PLCNewSignal_for_mainwindow(g_PLCtext2);
            mes.WriteLog(g_PLCtext2);
            return true;
        }
        else
        {
            flag2 = 0;     //NG
            g_PLCtext2 = "二维码【"+QRCode +"】过滤NG 板号:"+boardNum + "A面大小:" + QString::number(A1_file.size()) + "B面大小" + QString::number(B1_file.size());
            emit PLCNewSignal_for_mainwindow(g_PLCtext2);
            mes.WriteLog(g_PLCtext2);

            g_PLCtext2 = "二维码【"+QRCode +"】AI过滤NG 板号:"+boardNum + "A面大小:" + QString::number(AI_A1_file.size()) + "B面大小" + QString::number(AI_B1_file.size());
            emit PLCNewSignal_for_mainwindow(g_PLCtext2);
            mes.WriteLog(g_PLCtext2);
            return false;
        }
    }

    if (((A_file.size() == 8) && (B_file.size() == 8)) ||((AI_A_file.size() == 8) && (AI_B_file.size() == 8)))
    {
        flag2 = 1;     //OK
        g_PLCtext2 = "二维码【"+QRCode +"】分析OK 板号:"+boardNum;
        emit PLCNewSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return true;
    }
    else
    {
        flag2 = 0;     //NG
        g_PLCtext2 = "二维码【"+QRCode +"】过滤NG 板号:"+boardNum + "A面大小:" + QString::number(A_file.size()) + "B面大小" + QString::number(B_file.size());
        emit PLCNewSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);

        flag2 = 0;     //NG
        g_PLCtext2 = "二维码【"+QRCode +"】AI过滤NG 板号:"+boardNum + "A面大小:" + QString::number(AI_A_file.size()) + "B面大小" + QString::number(AI_B_file.size());
        emit PLCNewSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return false;
    }
    mes.WriteLog("人工和机器识别结果都没有");
    return FALSE;   // 人工和机器识别结果都没有，则认为NG
}



// //////////////////// FastMode模式 子线程 ////////////
PlcThreadFastMode::PlcThreadFastMode(){}

PlcThreadFastMode::~PlcThreadFastMode(){}

void PlcThreadFastMode::run()
{
    QString path = setup1.readInI("PATH","QRCodePath"),a;    // 二维码路径
    while(true)
        {
            QStringList filelist = FindFileNoChild(path);
            if(filelist.size() > 0)
            {
                QString QRcodeFile,QRCode,tempStr;
                QString NewPathOk = path+"\\OK",NewPathNG = path+"\\NG";
                for(int i = 0;i<filelist.size();i++)
                {
                    QRcodeFile = filelist.at(i);
                    bool boardResult = FastModeFindBoardResult(QRcodeFile,a);   //查找检板结果,不管OK或者NG都有结果
                    if (boardResult)   // 将结果发送给主程序写入PLC中
                    {
                        QDir dir(NewPathOk);
                        if(!dir.exists())
                            dir.mkdir(NewPathOk);
                        g_PLCtext2 = "O"+a;      //OK发送“1”
                        emit PLCFastModeSignal_for_mainwindow(g_PLCtext2);
                        bool rec = QFile::rename(QRcodeFile,NewPathOk+"\\"+a+".txt");    // 给出信号将文件移走
                    }
                    else {
                        QDir dir(NewPathNG);
                        if(!dir.exists())
                            dir.mkdir(NewPathNG);
                        g_PLCtext2 = "N"+a;
                        emit PLCFastModeSignal_for_mainwindow(g_PLCtext2);
                        bool rec = QFile::rename(QRcodeFile,NewPathNG+"\\"+a+".txt");
                    }

                    if(FastModeEndSignal)
                    {
                        break;
                    }
                    sleep(2);
                }
            }
            else
                sleep(1);
        }
}

bool PlcThreadFastMode::FastModeFindBoardResult(QString QRcodeFile,QString &QRCode)
{
    QString tempAnaDataA,tempAnaDataB,CheckBoardDate;
    QString boardNum,MetrialNum,tempStr;

    QStringList a = QRcodeFile.split("/");
    tempStr = a.at(a.size()-1);
    a = tempStr.split(".");
    QRCode = a.at(0);
    QFile QRcodefile(QRcodeFile);
    int Numm = 3;
    if (QRcodefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!QRcodefile.atEnd())
        {

            if (Numm == 1)
            {
                boardNum = QRcodefile.readLine();   //板号
                break;
            }
            else if (Numm == 2)        //检板日期
                CheckBoardDate = QRcodefile.readLine();
            else if (Numm == 3)
                MetrialNum = QRcodefile.readLine();   //料号
            Numm--;
        }
        QRcodefile.close();
    }
    else
    {
        g_PLCtext2 = "打开二维码文件【"+QRcodeFile+"】失败！";
        mes.WriteLog(g_PLCtext2);
        return FALSE;
    }

    QTime current_time =QTime::currentTime();
    int hour = current_time.hour();//当前的小时
    int minute = current_time.minute();//当前的分
    int second = current_time.second();//当前的秒
    int msec = current_time.msec();//当前的毫秒

    QString A_vrs,B_vrs,T_true,B_true,AI_T_true,AI_B_true;
    int Num;
    bool flag1=0, flag2=0;
    QString Filepath = setup1.readInI("PATH","BoardResultPath");    // Car路径
    //机器识别结果
    CheckBoardDate = CheckBoardDate.remove(QChar('\n'),Qt::CaseInsensitive);
    boardNum = boardNum.remove(QChar('\n'),Qt::CaseInsensitive);
    A_vrs = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "\\A.vrs";
    B_vrs = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "\\B.vrs";

    // 23:50至0:10左右会出现找不到板结果的情况
    if((hour == 23 && minute>50) || (hour == 0 && minute < 15))
    {
        QFile file(A_vrs);
        if (!file.exists())    // 不存在，则去前一天的文件夹里面找
        {
            int nCheckBoardDate = CheckBoardDate.toInt()-1;
            CheckBoardDate = QString::number(nCheckBoardDate);
            A_vrs = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "\\A.vrs";
            B_vrs = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "\\B.vrs";
        }
    }

    QFile fileA(A_vrs);
    Num = 5;
    if (fileA.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!fileA.atEnd())
        {
            Num--;
            if (Num == 0)
            {
                tempAnaDataA = fileA.readLine();
                break;
            }
        }
        fileA.close();
    }


    QFile fileB(B_vrs);
    Num = 5;
    if (fileB.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!fileB.atEnd())
        {
            Num--;
            if (Num == 0)
            {
                tempAnaDataB = fileB.readLine();;
                break;
            }
        }
        fileB.close();
    }

    tempAnaDataA = tempAnaDataA.remove(QChar('\n'),Qt::CaseInsensitive);
    tempAnaDataB = tempAnaDataB.remove(QChar('\n'),Qt::CaseInsensitive);
    if (tempAnaDataA == "0" && tempAnaDataB == "0")
    {
        flag1 = 1;  //分析OK
        g_PLCtext2 = "二维码【"+QRCode +"】 分析OK 板号:"+boardNum;
        emit PLCFastModeSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return TRUE;
    }
    else if(tempAnaDataA == "-1" || tempAnaDataB == "-1")
    {
        flag1 = 0;  //分析NG
        g_PLCtext2 = "二维码【"+QRCode +"】分析NG 板号:"+boardNum;
        emit PLCFastModeSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return false;
    }

    //人工检测和AI检测
    T_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_t_True";
    B_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_b_True";

    AI_T_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_t_Ai";
    AI_B_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_b_Ai";
    // 23:50至0:10左右会出现找不到板结果的情况
    if((hour == 23 && minute>50) || (hour == 0 && minute < 15))
    {
        QFile file(T_true);
        if (!file.exists())    // 不存在，则去前一天的文件夹里面找
        {
            int nCheckBoardDate = CheckBoardDate.toInt()-1;
            CheckBoardDate = QString::number(nCheckBoardDate);
            T_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_t_True";
            B_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_b_True";

            AI_T_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_t_Ai";
            AI_B_true = Filepath+"\\"+CheckBoardDate+"\\"+ boardNum + "_b_Ai";
        }
    }

    //AI判定结果
    QFile AI_A_file(T_true);
    QFile AI_B_file(B_true);
    if ((AI_A_file.size() == 8) && (AI_B_file.size() == 8))
    {
        flag2 = 1;     //OK
        g_PLCtext2 = "二维码【"+QRCode +"】AI过滤OK 板号:"+boardNum;
        emit PLCFastModeSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return true;
    }

    //人工判定结果
    QFile A_file(T_true);
    QFile B_file(B_true);      
    if ((A_file.size() == 8) && (B_file.size() == 8))
    {
        flag2 = 1;     //OK
        g_PLCtext2 = "二维码【"+QRCode +"】人工过滤OK 板号:"+boardNum;
        emit PLCFastModeSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return true;
    }
    else
    {
        flag2 = 0;     //NG
        g_PLCtext2 = "二维码【"+QRCode +"】分析NG 板号:"+boardNum;
        emit PLCFastModeSignal_for_mainwindow(g_PLCtext2);
        mes.WriteLog(g_PLCtext2);
        return false;
    }
    mes.WriteLog("人工和机器识别结果都没有");
    return FALSE;   // 人工和机器识别结果都没有，则认为NG

}

// 查找指定文件夹下的指定类型文件，包含子文件夹
QStringList PlcThreadFastMode::FindFile(QString path)
{

    QStringList filelist;
    QDir dir(path);
    if(!dir.exists())   //判断路径是否存在
    {
        return filelist;
    }

    //获取所选文件类型过滤器
    QStringList filters;
    //filters<<QString("*.jpeg")<<QString("*.jpg")<<QString("*.png")<<QString("*.tiff")<<QString("*.gif")<<QString("*.bmp");
    filters<<QString("*.txt");
    //定义迭代器并设置过滤器
    QDirIterator dir_iterator(path,
        filters,
        QDir::Files | QDir::NoSymLinks,
        QDirIterator::Subdirectories);

    while(dir_iterator.hasNext())
    {
        dir_iterator.next();
        QFileInfo file_info = dir_iterator.fileInfo();
        QString absolute_file_path = file_info.absoluteFilePath();
        qDebug()<<absolute_file_path<<endl;
        filelist.append(absolute_file_path);
    }
    return filelist;
}

// 查找指定文件夹下的指定类型文件，不包含子文件夹
QStringList PlcThreadFastMode::FindFileNoChild(QString path)
{
    //判断路径是否存在
    QStringList filelist;
    QDir dir(path);
    if(!dir.exists())
    {
        return filelist;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();

    int file_count = list.count();

    for(int i=0; i<file_count;i++)
    {
        QFileInfo file_info = list.at(i);
        QString suffix = file_info.suffix();
        if(QString::compare(suffix, QString("txt"), Qt::CaseInsensitive) == 0)
        {
            QString absolute_file_path = file_info.absoluteFilePath();
            filelist.append(absolute_file_path);
        }
    }
    return filelist;
}

