#ifndef WEBSERVICE_THREAD_H
#define WEBSERVICE_THREAD_H

#include<QtSql/QSql>
#include<QtSql/qsqlquerymodel.h>
#include<QtSql/QSqlQuery>
#include<QtSql/qsqldatabase.h>
#include<QSqlError>
#include<QStandardItem>

#include <QThread>
#include <QDebug>
#include <QVector>
#include <QMetaType>
#include <QWaitCondition>

/*
class webservice_thread
{
public:
    webservice_thread();
};
*/


class WebserviceThread: public QThread{
    Q_OBJECT

public:
    WebserviceThread(QObject *parent=0);
    ~WebserviceThread();
    void run();
    void setParameters(int);

signals:
    void linkState_send(QVector<QString>); // 向主线程发送连接状态
    void data_send(QVector<QString>); // 向主线程发送从客户端接收的数据

private slots:
    void stopclick_accept(); // 改变runstate

private:
    QVector<QString> readXML(QString);
    int runstate; // 用于终止循环
    int nPort;
};
#endif // WEBSERVICE_THREAD_H
