#include "thread.h"
#include "QDebug"
#include "mainwindow.h"
#include "QFile"
#include "config.h"
#include<QtCore/QtCore>
#include "tinyxml.h"

#include"iMesImage.nsmap"
#include"soapH.h"
#include"soapStub.h"
#include"stdsoap2.h"
#include"stdsoap2.cpp"
#include"soapC.cpp"
#include"soapServer.cpp"
using namespace std;

QString g_text1 = "",g_operationalMode = "",g_text2 = "",g_text3 = "";
QString g_pcbNum = "",g_CheckBoardDate = "",g_WorkOrderID = "";
float g_PcbTotal=0,g_ErrCount=0;
bool g_flag = true, g_flag_vrs = true;
QVector<string>g_text_list;
QVector<string>g_Text_list;
iMesThread *cimesthread;
QString g_sTemp_WorkOrderID,g_sTemp_FileName1;

QDateTime current_date_time =QDateTime::currentDateTime();
//QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");    //current_date字符串结果为"2016.05.20 12:17:01.445 周五"
QString current_date =current_date_time.toString("yyyyMMdd");


iMesThread::iMesThread()
{
    EndSignal = false;
}

iMesThread::~iMesThread()
{
}

void iMesThread::run()
{
    SetiMes();
}

void iMesThread::SetiMes()
{
    QDateTime date_time =QDateTime::currentDateTime();
    QString Now_date_time =date_time.toString("yyyy-MM-dd hh:mm:ss");
    SOAP_SOCKET nMaster,nClient;
    struct soap soap_imes;
    soap_init(&soap_imes);
    soap_imes.fget = http_Get;   //回调函数，用于显示wsdl文件
    QString qstrip = Config().Get("COMMUNICATION","IP").toString();
    int qstrport = Config().Get("COMMUNICATION","Port").toInt();
    char*  chip;     //Qstring 转 char*
    QByteArray ba = qstrip.toLatin1();
    chip=ba.data();
    nMaster = soap_bind(&soap_imes,chip,qstrport,100);
    if(nMaster<0){
        soap_print_fault(&soap_imes,stderr);
        qDebug() <<"绑定端口失败：" << nMaster<<endl;
        g_text1 = "绑定端口失败";
        exit(0);
    }
    qDebug() <<"绑定端口成功：" << nMaster<<endl;
    //str = "iMes服务启动成功："+QString::number(nMaster);
    g_text1 = "iMes服务启动成功!";
    while(true){
        if(g_text1 != "")      //g_text1用于接收iMes下发的信息
        {
          emit signal_add_int(g_text1);
          g_text1 = "";
        }
        if(g_text2 != "")     //g_text2用于发送程序交互信息
        {
          emit signal_add_int(g_text2);
          g_text2 = "";
        }
        if(g_text3 != "")     //g_text3用于发送工单号和资料名给mainwindow
        {
          emit signal_for_mainwindow(g_text3);
          if(g_flag_vrs)
          {
              g_text3 = g_text3 + ",";
              Dealwith_MSG(g_text3);
              g_flag_vrs = false;
          }
          else
              Dealwith_MSG(g_text3);
          g_text3 = "";
        }
        if(EndSignal)
        {
            break;
        }
        nClient = soap_accept(&soap_imes);
        if(nClient<0){
            soap_print_fault(&soap_imes,stderr);
            qDebug() <<"信息接收错误：" << nClient<<endl;
            g_text1 = "信息接收错误："+QString::number(nClient);
            exit(0);
        }
        qDebug() <<"信息接收成功：" << nClient<<endl;
        g_text1 = Now_date_time+" 接收到信息："+QString::number(nClient);
        soap_serve(&soap_imes);
        soap_end(&soap_imes);
    }
}



/* Web service operation 'ns__uploadData' implementation, should return SOAP_OK or error code */
int ns__uploadData(struct soap*, char *macCode, char *wipEntityId, char **result)
{
    float NGrate = 0;
    QDateTime date_time =QDateTime::currentDateTime();
    QString Now_date_time =date_time.toString("yyyy-MM-dd hh:mm:ss");
    if (NULL == macCode || NULL == wipEntityId)
    {
        printf("Error:The two argument should not be NULL!\n");
        return SOAP_ERR;
    }
    else
    {
        int cmp;
        QString ID = wipEntityId,CCC,B;
        QString MacCode = macCode;
        g_text1 = Now_date_time+" 接收到iMes下发内容："+ID+", "+MacCode+";";
        cimesthread->WriteLog(g_text1);
        QString dir = "D:\\Decode\\";
        dir = dir+ID+"*.txt";

        QByteArray qByteArray = dir.toUtf8();   //QString 转化为 char*
        char* cStr = qByteArray.data();
        cimesthread->listFiles(cStr);  //第一步：根据iMes下发的工单号去查找二维码信息
        g_operationalMode = g_operationalMode.toUpper();
        cmp = g_operationalMode.compare("QJ");   //对比两个字符串是否相同
        if(!g_flag)
        {
            if (cmp!=0)   //检孔模式
            {
                if (g_ErrCount == 0)       //检孔模式整个工单OK
                {
                    (*result) = "1";
                }
                else if (g_ErrCount <= 5)    //检孔模式不良小于5个panel
                {
                    (*result) = "0";
                }
                else                     //检孔模式不良大于5个panel
                {
                    (*result) = "2";
                }
            }
            else  //全检模式
            {
                if (g_ErrCount == 0)      //  全检模式整个工单OK
                {
                    (*result) = "1_1";
                }
                else
                {
                    NGrate = g_ErrCount/g_PcbTotal;
                    QString strNGrate = QString("%1").arg(NGrate);    //float转QString
                    QString strErrCount = QString("%1").arg(g_ErrCount);
                    QString strPcbTotal = QString("%1").arg(g_PcbTotal);
                    CCC = "总板数："+strPcbTotal+"  "+"错误板数："+strErrCount;
                    cimesthread->WriteLog(CCC);
                    strNGrate = strNGrate+"%";
                    QByteArray qByteArray = strNGrate.toUtf8();   //QString 转化为 char*
                    char* CStr = qByteArray.data();
                    (*result) = CStr;
                }
            }
            g_ErrCount = 0;
            g_PcbTotal = 0;
            g_operationalMode = "";
            return SOAP_OK;
        }
        else
        {
          (*result) = "None";
          return SOAP_ERR;
        }
    }
}

/* Web service operation 'ns__DownloadData' implementation, should return SOAP_OK or error code */
int ns__DownloadData(struct soap*, char *data, char **value)
{
    QDateTime date_time =QDateTime::currentDateTime();
    QString Now_date_time =date_time.toString("yyyy-MM-dd hh:mm:ss");
    if (NULL == data)
        {
            g_text1 = "DownloadData接口数据下发错误";
            return SOAP_ERR;
        }
        else
        {
            QString str1(data),text;
            text = Now_date_time+" 接收到DownloadData接口下发内容："+str1;
            cimesthread->WriteLog(text);

            //将接收到的信息保存为xml文件
            QString XmlFile = QCoreApplication::applicationDirPath() + "//message.xml";
            QFile afile(XmlFile);
            if (!afile.open(QIODevice::ReadWrite |QIODevice::Truncate| QIODevice::Text))cout<<"Cannot save the received information as an xml file!"<<endl;
            QTextStream aStream(&afile);
            aStream<<str1;
            afile.close();

            //解析xml文档
            cimesthread->Read_Xml(XmlFile);



            (*value) = "OK";
            return SOAP_OK;
        }
}

/* Web service operation 'ns__test1' implementation, should return SOAP_OK or error code */
int ns__test1(struct soap*, std::string input, std::string &result)
{
    MainWindow object;
    QDateTime date_time =QDateTime::currentDateTime();
    QString Now_date_time =date_time.toString("yyyy-MM-dd hh:mm:ss");
    if (result == "")
        {
            qDebug()<<"Error:The third argument should not be NULL!"<<endl;
            return SOAP_ERR;
        }
        else
        {
            QString qStr = QString::fromStdString(input);
            qDebug()<<"接收到iMes下发内容："<<qStr<<endl;
            g_text1 = Now_date_time+" 接收到iMes下发内容："+qStr;
            result = "jiekou3";
            return SOAP_OK;
        }
}

int http_Get(struct soap* soapObject)              //回调函数，用于解决“HTTP GET method not implemented”问题
{
    FILE*fd = NULL;
    QString WsdlFile = QCoreApplication::applicationDirPath() + "//iMesImage.wsdl";
    QByteArray qByteArray = WsdlFile.toUtf8();   //QString 转化为 char*
    char* CStr = qByteArray.data();
    fd = fopen(CStr, "rb");       //open WSDL file to copy
    //QFile file("..//iMesImage.wsdl");
    if (!fd)
    {
       return 404;                                 //return HTTP not found error
    }
    soapObject->http_content = "text/xml";         //HTTP header with text /xml content
    soap_response(soapObject,SOAP_FILE);
    for(;;)
    {
       size_t r = fread(soapObject->tmpbuf,1, sizeof(soapObject->tmpbuf), fd);
       if (!r)
       {
         break;
       }
       if (soap_send_raw(soapObject, soapObject->tmpbuf, r))
       {
         break;                                   //cannot send, but little we can do about that
       }
    }
    //file.close();
    fclose(fd);
    soap_end_send(soapObject);
    return SOAP_OK;
}

void iMesThread::listFiles(const char* dir)
{
    QString AllFileName,S;
    intptr_t handle;
    QDateTime date_time =QDateTime::currentDateTime();
    QString Now_date_time =date_time.toString("yyyy-MM-dd hh:mm:ss");
    _finddata_t findData;

    handle = _findfirst(dir, &findData);    // 查找目录中的第一个文件
    if (handle == -1)
    {
        qDebug()<< "Failed to find first file!\n";
        g_text2 = Now_date_time+" 没有检测到对应工单的检板信息。";
        g_flag = false;
        return;
    }
    do
    {
        if (findData.attrib & _A_SUBDIR
            && strcmp(findData.name, ".") == 0    //strcmp函数判断两个字符串是否相等
            && strcmp(findData.name, "..") == 0
            )    // 是否是子目录并且不为"."或".."
            qDebug() << findData.name << "\t<dir>\n";
        else
        {
            AllFileName = findData.name;
            g_text_list.push_back(findData.name);
            WriteLog(AllFileName);
            QJorJK(AllFileName);   //判断检板模式
        }
    } while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件

    for (int i = 0 ;i<=g_text_list.size()-1;i++)
    {
        std::string s = g_text_list[i];
        S = QString::fromUtf8(s.c_str(),s.length());
        g_pcbNum = ReadTextLine(S,3);   //获取板号
        g_CheckBoardDate = ReadTextLine(S,2);   //获取检板日期
        ReadBoardResult(g_pcbNum,g_CheckBoardDate);
    }
    _findclose(handle);    // 关闭搜索句柄
}


QString iMesThread::QJorJK(QString files)
{
    files = "D:\\Decode\\"+files;
    QFile file(files);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout<<"Can't open the file!"<<endl;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        qDebug()<< line;
        QString pattern("[a-zA-Z]{2}");
        QRegExp rx(pattern);

        int pos = line.indexOf(rx);
        if ( pos >= 0 )
        {
            qDebug() << rx.capturedTexts();     // QStringList("a=100", "a", "100"),
            //qDebug() << rx.cap(0);              //   0: text matching pattern
            g_operationalMode = rx.cap(0);   //   1: text captured by the 1st
        }
    }
    file.close();
    return g_operationalMode;
}


void iMesThread::WriteLog(const QString& content)
{
    QString current_date_1 =current_date_time.toString("yyyy-MM-dd");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString Now_date_time =current_date_time.toString("yyyy-MM-dd HH:mm:ss");

    QString FileAdrr = QCoreApplication::applicationDirPath() + "//"+current_date_1+".log";
    QFile afile(FileAdrr);
    if (!afile.open(QIODevice::ReadWrite |QIODevice::Append| QIODevice::Text))cout<<"Can't open the Log file1!"<<endl;
    QTextStream aStream(&afile);
    aStream<<Now_date_time+" "+content<<"\n";
    afile.close();
}

QString iMesThread::ReadTextLine(QString filename, int line)
{
    QString strVec[1000];     //文本中总共有1000行
    int i = 0;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout<<"Can't open the file!"<<endl;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        qDebug()<< line;
        strVec[i] = line;
        i = i + 1;
    }
    qDebug()<<"这是第三行:"<<strVec[line - 1];
    return strVec[line - 1];
}

//给定板号和检板日期，去读取检板结果
void iMesThread::ReadBoardResult(QString boardNum, QString CheckBoardDate)
{
    QString A_vrs,B_vrs,T_true,B_true;
    int Num;
    QString tempAnaDataA,tempAnaDataB;
    BOOL flag1=0, flag2=0;

    //机器识别结果
    A_vrs = "D:\\car2\\"+CheckBoardDate+"\\"+ boardNum + "\\A.vrs";
    B_vrs = "D:\\car2\\"+CheckBoardDate+"\\"+ boardNum + "\\B.vrs";

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
    }
    fileA.close();

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
    }
    fileB.close();
    if (tempAnaDataA == "0" && tempAnaDataB == "0")
    {
        flag1 = 1;  //分析OK
    }
    else if(tempAnaDataA == "-1" || tempAnaDataB == "-1")
    {
        flag1 = 0;  //分析NG
    }

    //人工检测
    T_true = "D:\\car2\\"+CheckBoardDate+"\\"+ boardNum + "_t_True";
    B_true = "D:\\car2\\"+CheckBoardDate+"\\"+ boardNum + "_b_True";

    QFile A_file(T_true);
    QFile B_file(B_true);

    if ((A_file.size() == 8) && (B_file.size() == 8))
    {
        flag2 = 1;     //OK
    }
    else
    {
        flag2 = 0;     //NG
    }

    if (flag1 || flag2)
    {
        cout << "板OK" <<endl;
    }
    else
    {
        cout << "板NG" <<endl;
        g_ErrCount+=1;
    }
    g_PcbTotal+=1;
}


bool iMesThread::WorkOrdersChange(QString adress)
{
    QString str,temp;
    QFile file(adress);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
        {
           str = file.readLine();
        }
    }
    while(TRUE)
    {
        QString pattern("(\\d{8})");
        QRegExp rx(pattern);
        qDebug() << str;
        int pos = str.indexOf(rx);
        if ( pos >= 0 )
        {
            temp = rx.cap(0);
            qDebug() << rx.cap(0);              //   0: text matching pattern
        }

        if (g_WorkOrderID == "")g_WorkOrderID = temp;
        else if (g_WorkOrderID == temp)return FALSE;
        else
        {
            g_WorkOrderID = temp;
            return TRUE;
        }
        Sleep(2*1000);
    }
}



void iMesThread::Read_Xml(QString xml_path)
{
    QDateTime date_time =QDateTime::currentDateTime();
    QString Now_date_time =date_time.toString("yyyy-MM-dd hh:mm:ss");
    QByteArray qByteArray = xml_path.toUtf8();   //QString 转化为 char*
    QString tagCode,XmlInfo,eap_path,WorkOrderID,Process,MeterialNum,PCBNum,FileName,Top,Button;
    char* CStr = qByteArray.data();
    TiXmlDocument mydoc(CStr);      //xml文档对象
    bool loadOk=mydoc.LoadFile();   //加载文档
    if(!loadOk)
    {
        g_text1 = "解析xml文档失败";
        qDebug()<<"could not load the test file.Error:"<<mydoc.ErrorDesc()<<endl;
        exit(1);
    }

    TiXmlElement *RootElement=mydoc.RootElement();	//根元素, Info
    //qDebug()<< "[root name]" << RootElement->Value() <<"[wipEntityId]" << RootElement->Attribute("wipEntityId") <<"[macCode]" << RootElement->Attribute("macCode") <<"\n";
    QString wipEntityId = RootElement->Attribute("wipEntityId");
    QString macCode = RootElement->Attribute("macCode");

    TiXmlElement *pEle=RootElement;

    //遍历该结点
    for(TiXmlElement *StuElement = pEle->FirstChildElement();//第一个子元素
        StuElement != NULL;
        StuElement = StuElement->NextSiblingElement())//下一个兄弟元素
    {
        TiXmlAttribute *pAttr=StuElement->FirstAttribute();//第一个属性
        //qDebug()<< "[root name]" << pAttr->Value() <<"[tagCode]" << StuElement->Attribute("tagCode") <<"[tagValue]" << StuElement->Attribute("tagValue");
        //qDebug()<<StuElement->Attribute("tagCode")<<" "<<StuElement->Attribute("tagValue")<<endl;
        tagCode = StuElement->Attribute("tagCode");
        if(tagCode == "BC2005300_1000")
            WorkOrderID = StuElement->Attribute("tagValue");    //工单ID
        else if(tagCode == "BC2005300_1002")
            Process = StuElement->Attribute("tagValue");        //工序
        else if(tagCode == "BC2005300_1001")
            MeterialNum = StuElement->Attribute("tagValue");    //物资编码
        else if(tagCode == "BC2005300_1003")
            PCBNum = StuElement->Attribute("tagValue");         //数量
        else if(tagCode == "BC2005300_1005")
            FileName = StuElement->Attribute("tagValue");       //资料名称
        else if(tagCode == "BC2005300_1006")
            Top = StuElement->Attribute("tagValue");            //正面层面
        else if(tagCode == "BC2005300_1007")
            Button = StuElement->Attribute("tagValue");         //反面层面

        //输出子元素的值
        for(TiXmlElement *sonElement=StuElement->FirstChildElement();
            sonElement;
            sonElement=sonElement->NextSiblingElement())
        {
            qDebug()<<sonElement->FirstChild()->Value()<<endl;
        }
    }
    g_text1 = Now_date_time+" 接收到iMes下发内容：【wipEntityId】"+wipEntityId+"【macCode】"+macCode+"【工单ID】"+WorkOrderID+"【工序】"+Process+"【物资编码】"+\
            MeterialNum+"【板数量】"+PCBNum+"【资料名称】"+FileName+"【正面】"+Top+"【反面】"+Button;
    g_text3 = WorkOrderID+","+FileName;
    WriteLog(g_text1);

    QString iMesSetting_ini = QCoreApplication::applicationDirPath() + "/iMesSetting.ini";
    QFileInfo fi;
    fi.setFile(iMesSetting_ini);
    bool res = fi.isFile();
    if(res)
    {
        eap_path = Config().Get("PATH","EapPath").toString();
        if(eap_path != "")
            XmlInfo = eap_path + "//XmlInfo";
        else
            XmlInfo = QCoreApplication::applicationDirPath() + "//XmlInfo";
    }
    else
        XmlInfo = QCoreApplication::applicationDirPath() + "//XmlInfo";
    //将解析成功的xml文件信息写到外部文件供ImageServer的EAP读取
    WriteLog(XmlInfo);
    QFile afile(XmlInfo);
    if (!afile.open(QIODevice::ReadWrite |QIODevice::Truncate| QIODevice::Text))cout<<"Cannot write the parsed xml file information to an external file!"<<endl;
    QTextStream aStream(&afile);
    aStream<<FileName<<"\n";   //资料名称
    aStream<<Top<<"\n";        //正面层别
    aStream<<Button<<"\n";     //反面层别
    aStream<<WorkOrderID<<"\n";   //工单ID
    afile.close();
    WriteLog("写入外部XmlInfo文件成功！");
}


void iMesThread::Dealwith_MSG(QString MSG)
{
    QString a,b;
    QStringList list,list1;
    list = MSG.split(",");
    a = list[0];   //工单
    b = list[1];   //资料名

    if(list.size()>2)
    {
        g_sTemp_WorkOrderID = a;
        g_sTemp_FileName1 = b;
    }


    if(g_sTemp_WorkOrderID != "" && MSG != "")
    {

        if(g_sTemp_WorkOrderID != a)
        {
            QString file_path = Read_Config_InI("PATH","BoardResultPath");    //设置资料路径
            /*   按料号存储
            list1 = g_sTemp_FileName1.split(",");
            a = list[0];    //工单
            b = list[1];    //资料名
            */

            file_path = file_path+"\\"+current_date;
            //file_path = file_path+"\\20201011";
            FindFile(file_path);
        }
        g_sTemp_WorkOrderID = a;
        g_sTemp_FileName1 = b;
    }

}


bool iMesThread::FindFile(const QString &path)     //遍历的查找文件
{

    QDir dir(path);
    if (!dir.exists())
    {
        return false;
    }
    dir.setFilter(QDir::Dirs | QDir::Files);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    int i = 0;
    do {

        QFileInfo fileInfo = list.at(i);
        if (fileInfo.fileName() == "." | fileInfo.fileName() == "..")
        {
            i++;
            continue;
        }
        bool bisDir = fileInfo.isDir();
        if (bisDir)
        {
            FindFile(fileInfo.filePath());
        }
        else
        {
            //blog_file_paths_ << fileInfo.path();
            //blog_file_names_ << fileInfo.fileName();
            //qDebug() << QString(" %2  %3  ")
            //         .arg(fileInfo.path())
            //         .arg(fileInfo.fileName());

            if(fileInfo.filePath().right(4) == ".vrs")
            {
                qDebug() << QString(" %2 ").arg(fileInfo.filePath());
                ModifyVrs(fileInfo.filePath(),g_sTemp_WorkOrderID);
            }
            //nFiles++;
        }
        i++;
    } while (i < list.size());
    return true;
}


void iMesThread::ModifyVrs(QString filepath, QString content)
{
    qDebug() << "开始修改vrs文件，加入信息： "+content;
    QStringList strList;
    QFile readFile(filepath);
    if(!readFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
            qDebug()<<"Can't open the file!"<<endl;
    }
    QTextStream stream(&readFile);
    QString strAll = stream.readAll();
    strList = strAll.split("\n");
    if(strList.size()<= 6)
        return;
    QString a = strList[6];
    QString b = g_sTemp_FileName1;
    if(a == b)
    {
       stream.seek(readFile.size());//将当前读取文件指针移动到文件末尾
       stream<<content<<'\n';
    }

    readFile.close();
}


QString iMesThread::Read_Config_InI(QString __Key_root, QString __Key_value)
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











