#ifndef PLCTHREAD_H
#define PLCTHREAD_H

#include <QThread>
#include <QObject>
#include <thread>
#include "setupdialog.h"

// ///////// 【子线程】 将扫描到的二维码压入队列 ////////////
class PlcThread : public QThread
{
    Q_OBJECT    //添加宏定义,Q_OBJEC是类与类之间通信的前提

public:
    explicit PlcThread();
    ~PlcThread();
    void ReadCode();
    bool EndSignal;
    void ConnectPLC();


private:
    void run();   //任务处理线程 重写run函数
    int changefromHex_to_ascii(QString str);

public slots:     //自定义信号槽函数
    //接收主线程的消息
    void recMegFromMain(QString);

signals:
    void PLCsignal_for_mainwindow(QString);
};




// ///////// 【子线程】 不断地进行查找检板结果 ////////////
class PlcThreadNew: public QThread
{
    Q_OBJECT    //添加宏定义,Q_OBJEC是类与类之间通信的前提

public:
    PlcThreadNew();
    ~PlcThreadNew();
    void SetDevice(bool OkorNG, QString QRcode);
    bool EndSignal;

private:
    void run();   //任务处理线程 重写run函数
    bool FindBoardResult(QString QRCode,long long nTime);
    SetUpDialog setup;

private slots:     //自定义信号槽函数

signals:
    void PLCNewSignal_for_mainwindow(QString);
};



// ///////// 【子线程】 快速机模式子线程 ////////////
class PlcThreadFastMode: public QThread
{
    Q_OBJECT    //添加宏定义,Q_OBJEC是类与类之间通信的前提
public:
    PlcThreadFastMode();
    ~PlcThreadFastMode();
    void FastModeSetDevice(bool OkorNG, QString QRcode);
    bool FastModeEndSignal;
    SetUpDialog setup1;

private:
    void run();   //任务处理线程 重写run函数
    bool FastModeFindBoardResult(QString QRCodeFile,QString &QRcode);
    QStringList FindFile(QString path);
    QStringList FindFileNoChild(QString path);

private slots:     //自定义信号槽函数

signals:
    void PLCFastModeSignal_for_mainwindow(QString);
};

#endif // PLCTHREAD_H
