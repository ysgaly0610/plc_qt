#include "setting.h"
#include <QtCore>
#include <QFile>


Setting::Setting()
{
    m_qstrFileName = QCoreApplication::applicationDirPath() + "/iMesSetting.ini";

    //"iMesSetting.ini"配置文件，文件存在则打开，不存在则创建
    m_psetting = new QSettings(m_qstrFileName,QSettings::IniFormat);     //实例化对象

}

Setting::~Setting()
{
    delete m_psetting;    //释放对象的堆内存
    m_psetting = NULL;
}

void Setting::SetQRCodePath(QString qstrQrcode)
{
    m_psetting->setValue("/PATH/QRCodePath",qstrQrcode);     //写INI文件
}
/*
QString Setting::GetIp()
{
    return m_psetting->value("/PATH/ip").toString();
}
*/
void Setting::SetBoardResultPath(QString qstrPort)
{
    m_psetting->setValue("/PATH/BoardResultPath",qstrPort);
}

void Setting::SetEapPath(QString qstrEap)
{
    m_psetting->setValue("/PATH/EapPath",qstrEap);
}

void Setting::SetTimeDelay(QString qstrSec)
{
    m_psetting->setValue("/TIME/Delay",qstrSec);
}

/*
QString Setting::GetPort()
{
    return m_psetting->value("/PATH/port").toString();
}
*/

void Setting::SetRunMode(QString mode)
{
    m_psetting->setValue("/MODE/mode",mode);
}
