#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QObject>
#include <thread>

int http_Get(struct soap* soapObject);
class iMesThread : public QThread
{
    Q_OBJECT    //添加宏定义,Q_OBJEC是类与类之间通信的前提

public:
    iMesThread();
    ~iMesThread();
    bool EndSignal;
    void listFiles(const char* dir);
    QString QJorJK(QString files);
    void WriteLog(const QString& content);
    QString ReadTextLine(QString filename, int line);
    void ReadBoardResult(QString boardNum, QString CheckBoardDate);
    bool WorkOrdersChange(QString adress);
    void Read_Xml(QString xml_path);
    void Dealwith_MSG(QString MSG);
    bool FindFile(const QString &path);
    void ModifyVrs(QString filepath, QString content);
    QString Read_Config_InI(QString __Key_root, QString __Key_value);



private:
    void run();   //任务处理线程 重写run函数
    void SetiMes();

private slots:     //自定义信号槽函数

signals:
    void signal_add_int(QString);   //线程信号函数
    void signal_for_mainwindow(QString);
    void signal_for_threadvrs(QString str);

};
#endif // THREAD_H
