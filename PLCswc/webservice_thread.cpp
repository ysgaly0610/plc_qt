#include "webservice_thread.h"
#include <sstream>
//gsoap文件
#include"iMes.nsmap"
#include"soapH.h"
#include"soapStub.h"
#include"stdsoap2.h"
#include"stdsoap2.cpp"
#include"soapC.cpp"
#include"soapServer.cpp"

/*
webservice_thread::webservice_thread()
{

}
*/


QString Msg; // 存储客户端发送过来的数据

WebserviceThread::WebserviceThread(QObject *parent):QThread(parent){
    qRegisterMetaType<QVector<QString>>("QVector<QString>");
    runstate=0; //置1时停止网络循环
}

WebserviceThread::~WebserviceThread(){

}

void WebserviceThread::setParameters(int port){
    nPort=port;
}

int http_get_wbq(soap *);
void WebserviceThread::run(){
    struct soap wbq_soap;
    soap_init(&wbq_soap);
    wbq_soap.fget = http_get_wbq; // 网上有人说如果要传输的数据量大的话应该用http post
    int nMaster = (int)soap_bind(&wbq_soap,NULL,nPort,100); // 端口绑定
    if(nMaster<0)
        soap_print_fault(&wbq_soap,stderr);
    else{
        QVector<QString> link_state_1;
        link_state_1.append(QString("Socket connection successful: master socket = "));
        link_state_1.append(QString::number(nMaster, 10));
        emit linkState_send(link_state_1); // 发射初始连接后的状态信息

        for(int i=0;;i++){
            int nSlave = (int)soap_accept(&wbq_soap); // 端口监听，获取客户端连接信息
            if(nSlave<0){
                soap_print_fault(&wbq_soap,stderr);
                break;
            }

            QVector<QString> link_state_2;
            link_state_2.append(QString("Times "));
            link_state_2.append(QString::number(i, 10));
            link_state_2.append(QString(". Accepted connection from "));
            link_state_2.append(QString::number(int((wbq_soap.ip>>24)&0xFF), 10));
            link_state_2.append(QString("."));
            link_state_2.append(QString::number(int((wbq_soap.ip>>16)&0xFF), 10));
            link_state_2.append(QString("."));
            link_state_2.append(QString::number(int((wbq_soap.ip>>8)&0xFF), 10));
            link_state_2.append(QString("."));
            link_state_2.append(QString::number(int((wbq_soap.ip)&0xFF), 10));
            link_state_2.append(QString(": slave socket = "));
            link_state_2.append(QString::number(nSlave, 10));
            if(!runstate)
                emit linkState_send(link_state_2); // 发射客户端连接信息

            if(runstate)//点击断开连接后的下一次循环可以ping通，但无法调用服务端的sendMsg方法，即数据无法传送，不会造成数据丢失。
                break;

            if(soap_serve(&wbq_soap)!=SOAP_OK)
                soap_print_fault(&wbq_soap,stderr);
            soap_destroy(&wbq_soap);
            soap_end(&wbq_soap);

            QVector<QString> data = readXML(Msg); // 解析数据
            emit data_send(data); // 向主线程传递解析后的数据
            Msg="";
        }
    }
    soap_done(&wbq_soap);
}

void WebserviceThread::stopclick_accept(){
    runstate = 1;
}

/*
此功能是按自己格式解析的
*/
QVector<QString> WebserviceThread::readXML(QString Msg){
    QVector<QString> data;
    QStringList msglist = Msg.split(",");
    for(int i=0;i<msglist.size();i++){
        data.append(msglist[i]);
    }
    return data;
}

int ns__sendMsg(struct soap *soap, char* smsg, nullResponse* result)
{
    Msg = QString(smsg);
    return SOAP_OK;
}

/*
用于在网页页面正常显示信息（百度得到的解决方案）
*/
int http_get_wbq(struct soap *soap){
    soap_response(soap, SOAP_HTML); // HTTP response header with text/html
    soap_send(soap, "<HTML>My Web server is operational.</HTML>");
    soap_end_send(soap);
    return SOAP_OK;
}
