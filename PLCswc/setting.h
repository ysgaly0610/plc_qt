#ifndef SETTING_H
#define SETTING_H

#include <QString>
#include <QSettings>

class Setting
{
public:
    Setting();
    virtual ~Setting();

    void CreateIni_File(QString qstrFilePath, QString qstrFileName);
    void SetQRCodePath(QString qstrQrcode);
    QString GetIp();

    void SetBoardResultPath(QString qstrPort);
    void SetEapPath(QString qstrEap);
    void SetRunMode(QString mode);
    void SetTimeDelay(QString Sec);
    QString GetPort();

private:
    QString m_qstrFileName;
    QSettings *m_psetting;      //定义一个QSettings指针对象
};

#endif // SETTING_H
