// KmIniFile.h: interface for the CKmIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KMINIFILE_H__D3332372_7E76_4999_B96C_B3FEEA7D1A96__INCLUDED_)
#define AFX_KMINIFILE_H__D3332372_7E76_4999_B96C_B3FEEA7D1A96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QString"

class CKmIniFile  
{
public:
	CKmIniFile();
	virtual ~CKmIniFile();

private:

public:
	//创建函数
	void ResetContent();
    bool ReadFile(const QString &strFileName);
    bool WriteFile(const QString &strFileName);

	//得到变量整数型数值
    bool GetVarInt(const QString &, const QString & ,int &);
    bool GetVarLon(const QString &, const QString & ,long &);

	//得到变量字符串型数值
    bool GetVarStr(const QString &, const QString & ,QString & );

	//得到变量字符串型数值
    bool GetVarDou(const QString &, const QString & ,double & );

	//重新设置变量整数型数值
    void SetVarInt(const QString &, const QString & , const int, QString strRemark = "" );
    void SetVarLon(const QString &, const QString & , const long, QString strRemark = "" );

	//重新设置变量字符串型数值
    void SetVarStr(const QString &, const QString &, const QString &, QString strRemark = "");

	//重新设置变量浮点数值
    void SetVarDou(const QString &, const QString &, const double, QString strRemark = "");

private:
    int SearchSection(const QString& strSection);
    void SetVar( const QString& strSection, const QString& strVarName, const QString& strVar, QString& strRemark );
    bool GetVar( const QString& strSection, const QString& strVarName, QString& strReturnValue );
};

#endif // !defined(AFX_KMINIFILE_H__D3332372_7E76_4999_B96C_B3FEEA7D1A96__INCLUDED_)
