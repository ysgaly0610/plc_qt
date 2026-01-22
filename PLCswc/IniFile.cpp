// KmIniFile.cpp: implementation of the CKmIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "IniFile.h"
#include "QString"
#include "QFile"
#include "QDataStream"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

enum Excep { EXCEP_ONE, EXCEP_TWO };


typedef struct
{
        int nType;
        QString sName;
        QString sValue;
        QString sRemark;
}INI_RECORD;

#define INI_TYPE_SECTION	0
#define INI_TYPE_VAR		1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKmIniFile::CKmIniFile()
{

}

CKmIniFile::~CKmIniFile()
{
	ResetContent();
}

bool CKmIniFile::GetVar(const QString &strSection, const QString &strVarName, QString &strReturnValue)
{
	int i = SearchSection( strSection );
	if( i == -1 )
                return false;
	INI_RECORD* pRecord;
    //int nCount = m_FileContainer.GetSize();
	i++;
    //while(i < nCount)
	{
        //pRecord = (INI_RECORD *)m_FileContainer[ i++ ];
                assert(pRecord != NULL );
		if( pRecord->nType == INI_TYPE_SECTION )
                        return false;
		if( pRecord->sName == strVarName )
		{
			strReturnValue = pRecord->sValue;
                        return true;
		}
	}
        return false;
}

void CKmIniFile::SetVar(const QString &strSection, const QString &strVarName, const QString &strVarValue, QString& strVarRemark)
{
	INI_RECORD* pRecord = new INI_RECORD;
	pRecord->nType = INI_TYPE_VAR;
	pRecord->sName = strVarName;
	pRecord->sValue = strVarValue;
	pRecord->sRemark = strVarRemark;

	int i = SearchSection( strSection );
	if( i == -1 )
	{
		INI_RECORD* pSection = new INI_RECORD;
		pSection->nType = INI_TYPE_SECTION;
		pSection->sName = strSection;
        //m_FileContainer.Add( pSection );

        //m_FileContainer.Add( pRecord );
	}
    else;
        //m_FileContainer.InsertAt( i + 1, pRecord );
}

int CKmIniFile::SearchSection(const QString &strSection)
{
	INI_RECORD* pRecord;
    //int nCount = m_FileContainer.GetSize();
	int i = 0;
    //while(i < nCount)
	{
               // pRecord = (INI_RECORD *)m_FileContainer[ i++ ];
                assert( pRecord != NULL );
		if( pRecord->nType == INI_TYPE_SECTION && pRecord->sName == strSection )
		{
			return i-1;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////
//

void CKmIniFile::ResetContent()
{
    //int nCount = m_FileContainer.GetSize();
    //for(int i = 0; i < nCount; i++)
    //	delete (INI_RECORD *)m_FileContainer[ i ];
    //m_FileContainer.RemoveAll();
}

bool CKmIniFile::ReadFile(const QString &strFileName)
{
	ResetContent();

    QFile file(strFileName);
    if( !file.open(QIODevice::ReadWrite | QIODevice::Truncate ) )
                return false;

    QDataStream ar(&file);
	try
	{
                QString sSign;
		ar >> sSign;
		if( sSign != "iMes Setting File." )
                        return false;

		int index;
		INI_RECORD* pRecord;
        QString strFileLine, strSection, strVarName, strVarValue;

		int nRowCount, l;
        QString c;
		ar >> nRowCount;
		int j = 0;
		for(int iRow = 0; iRow < nRowCount; iRow++)
		{
			ar >> l;
            //strFileLine. GetBufferSetLength(l+1);
			for( j = 0; j < l; j++)
			{
				ar >> c;
                //c = 255 - c;
                //strFileLine.SetAt(j, c);
			}
            //strFileLine.SetAt(j, 0);
            //strFileLine.ReleaseBuffer();

            //strFileLine.TrimLeft();
            //if( strFileLine.GetLength() < 3 )
                //continue;
            /*
			if( strFileLine.GetAt(0) == '[' )
			{
				index = strFileLine.Find(']');
				if( index == -1 )
					continue;
				strSection = strFileLine.Mid(1, index - 1);
				strSection.TrimLeft();
				strSection.TrimRight();
				if( strSection.IsEmpty() )
					continue;

				// new section
				pRecord = new INI_RECORD;
				pRecord->nType = INI_TYPE_SECTION;
				pRecord->sName = strSection;
				m_FileContainer.Add( pRecord );
				continue;
			}

			if( strFileLine.Left(2) == "//" )
				continue;
			index = strFileLine.Find('=');
			if( index == -1 )
				continue;
			strVarName = strFileLine.Left( index );
			strVarName.TrimRight();
			if( strVarName.IsEmpty() )
				continue;
			
			// new var
			pRecord = new INI_RECORD;
			pRecord->nType = INI_TYPE_VAR;
			pRecord->sName = strVarName;
			pRecord->sValue = strFileLine.Mid( index + 1 );
            m_FileContainer.Add( pRecord );*/
		}
	}
    catch(Excep ex)
    //catch(EXCEPTION_POINTERS *pException)
	{
        ex;
        return false;
    }

    return true;
}

bool CKmIniFile::WriteFile(const QString &strFileName)
{

}

///////////////////////////////////////////////////////////////////////////
//

bool CKmIniFile::GetVarStr(const QString & strSection, const QString & strVarName, QString &strReturnValue)
{
	return GetVar(strSection, strVarName, strReturnValue);
}

bool CKmIniFile::GetVarDou(const QString & strSection, const QString & strVarName, double & fValue)
{
    QString  strReturnVar;
	if( GetVar(strSection, strVarName, strReturnVar) )
	{
        fValue = strReturnVar.toLong();
        return true;
	}
        return false;
}

bool CKmIniFile::GetVarInt(const QString & strSection, const QString & strVarName, int & iValue)
{
    QString  strReturnVar;
	if(GetVar(strSection,strVarName,strReturnVar))
	{
        iValue = strReturnVar.toInt();
        return true;
	}
        return false;
}

bool CKmIniFile::GetVarLon(const QString & strSection, const QString & strVarName, long & iValue)
{
    QString  strReturnVar;
	if(GetVar(strSection,strVarName,strReturnVar))
	{
        iValue = strReturnVar.toFloat();
        return true;
	}
        return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//

void CKmIniFile::SetVarStr(const QString & strSection, const QString & strVarName, const QString & strValue, QString strRemark)
{
	SetVar(strSection, strVarName, strValue, strRemark);
}

void CKmIniFile::SetVarDou(const QString & strSection, const QString & strVarName, const double fValue, QString strRemark)
{
    QString strVar;
    strVar = QString("%.6f").arg(fValue);
	SetVar(strSection, strVarName, strVar, strRemark);
}

void CKmIniFile::SetVarInt(const QString & strSection, const QString & strVarName, const int iValue, QString strRemark)
{
    QString strVar = QString("%d").arg(iValue);
	SetVar(strSection, strVarName, strVar, strRemark);
}

void CKmIniFile::SetVarLon(const QString & strSection, const QString & strVarName, const long lValue, QString strRemark)
{
	SetVarInt( strSection, strVarName, lValue, strRemark );
}
