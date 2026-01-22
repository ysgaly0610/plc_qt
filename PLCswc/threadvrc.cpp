#include "threadvrc.h"
#include "windows.h"
#include "thread.h"
#include "QDir"
#include "QDebug"
#include <setting.h>
#include "QCoreApplication"

QString g_sTemp_WorkOrderID = "";
QString g_sTemp_FileName1 = "";

CThreadVRC *cThreadvrs_s = new CThreadVRC();
iMesThread *iMesImage_vrs_s = new iMesThread();

CThreadVRC::CThreadVRC()
{

    QObject::connect(iMesImage_vrs_s,SIGNAL(signal_for_threadvrs(QString)),cThreadvrs_s,SLOT(Dealwith_MSG(QString)),Qt::BlockingQueuedConnection);
}

CThreadVRC::~CThreadVRC()
{
}



void CThreadVRC::Dealwith_MSG(QString MSG)
{
    qDebug() << "开始处理接收到的信息: "+MSG;
    QString a,b;
    QStringList list,list1;
    if(MSG.right(3) == "txt")
    {
        MSG = MSG.left(sizeof (MSG)-3);
        list = MSG.split(",");
        a = list[0];    //工单
        b = list[1];    //资料名
        g_sTemp_WorkOrderID = a;
        g_sTemp_FileName1 = MSG;
    }
    else {
        list = MSG.split(",");
        a = list[0];    //工单
        b = list[1];    //资料名
        g_sTemp_WorkOrderID = a;

    }

    if(g_sTemp_WorkOrderID != "" && MSG != "")
    {

        if(g_sTemp_WorkOrderID != a)
        {
            QString file_path = Read_Config_InI("PATH","BoardResultPath");    //设置资料路径
            list1 = g_sTemp_FileName1.split(",");
            a = list[0];    //工单
            b = list[1];    //资料名
            file_path = file_path+"\\"+b;
            FindFile(file_path);
        }
        g_sTemp_WorkOrderID = a;
        g_sTemp_FileName1 = MSG;
    }

}


bool CThreadVRC::FindFile(const QString &path)     //遍历的查找文件
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


void CThreadVRC::ModifyVrs(QString filepath, QString content)
{
    qDebug() << "开始修改vrs文件，加入信息： "+content;
    QString strAll;
    QStringList strList;
    QFile readFile(filepath);
    if(readFile.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        QTextStream stream(&readFile);
        strAll=stream.readAll();
    }
    readFile.close();
    QFile writeFile(filepath);
    if(writeFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
            QTextStream stream(&writeFile);
            strList=strAll.split("\n");
            for(int i=0;i<strList.count();i++)
            {
                if(i==strList.count()-1)
                {
                    //最后一行不需要换行
                    stream<<"Image";
                    stream<<content;
                }
                else
                {
                    stream<<"Image111"<<'\n';
                }

                if(strList.at(i).contains("iface eth0 inet static"))
                {
                    QString tempStr=strList.at(i+1);
                    tempStr.replace(0,tempStr.length(),"        address 192.168.1.111");
                    stream<<tempStr<<'\n';
                    tempStr=strList.at(i+2);
                    tempStr.replace(0,tempStr.length(),"        netmask 255.255.255.0");
                    stream<<tempStr<<'\n';
                    tempStr=strList.at(i+3);
                    tempStr.replace(0,tempStr.length(),"        network 192.168.1.0");
                    stream<<tempStr<<'\n';
                    tempStr=strList.at(i+4);
                    tempStr.replace(0,tempStr.length(),"        geteway 192.168.1.1");
                    stream<<tempStr<<'\n';
                    i+=4;
                }
            }
    }
    writeFile.close();
}


QString CThreadVRC::Read_Config_InI(QString __Key_root, QString __Key_value)
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

