// AdoDatabase.cpp: implementation of the CAdoDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AdoDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//-----------------------------------------------------
// CAdoDatabase  实现 
//-----------------------------------------------------
void CAdoDatabase::Init()
{ 
	CoInitialize(NULL); 
	
	m_bIsOpen = false;
	ConnectString = _T("");
	m_lpConnection = NULL;
	m_lpConnection.CreateInstance("ADODB.Connection"); 
} 

CAdoDatabase::CAdoDatabase()
{
	Init();
} 

CAdoDatabase::CAdoDatabase(const char* lpConnStr)
{
	Init();
	ConnectString = lpConnStr; 
} 

CAdoDatabase::CAdoDatabase(const CString & szConnStr)
{
	Init();
	ConnectString = szConnStr; 
} 

bool CAdoDatabase::IsOpen()
{
	return m_bIsOpen;
} 

bool CAdoDatabase::Open(const char* lpConnStr)
{ 
	ConnectString = lpConnStr;
	return Open();
} 

bool CAdoDatabase::Open(const CString & szConnStr)
{
	ConnectString = szConnStr;
	return Open();
} 

bool CAdoDatabase::Open()
{
	if ( ConnectString == _T(""))
		return false; 
	try
	{
		m_bIsOpen = (m_lpConnection->Open(_bstr_t(ConnectString), _T(""), _T(""), NULL) == 0);
	}
	catch(_com_error &e)
	{
		m_bIsOpen = false; 
		throw e;
	}
	
	return m_bIsOpen; 
} 

//打开Microsoft Sql Server
bool CAdoDatabase::OpenMSSQL(const char* lpServer, const char* lpUserID, 
							 const char* lpPassword, const char* lpDataBase)
{ 
	ConnectString.Format(_T("driver={sql server};server=%s;uid=%s;pwd=%s;database=%s"),
		lpServer, lpUserID, lpPassword, lpDataBase);
    return Open(); 
	
} 


//打开Oracle数据库(客户端要先安装Oracle Client),lpDataSource指对应服名
bool CAdoDatabase::OpenOracle(const char* lpDataSource, const char* lpUserID, const char* lpPassword)
{ 
	ConnectString.Format(_T("Provider=OraOLEDB.Oracle;Data Source=%s;User Id=%s;Password=%s;"), 
		lpDataSource, lpUserID, lpPassword);       
	
    return Open();
} 

//打开Access数据库
bool CAdoDatabase::OpenAccess(const char* lpDbFile, const char* lpPassword)
{
	ConnectString.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;User Id=admin;Password=%s;") ,
		lpDbFile, lpPassword);
	return Open();
} 

void CAdoDatabase::Close()
{
	if (m_lpConnection != NULL && IsOpen())
	{
		m_lpConnection->Close(); 
		m_bIsOpen = false;
	}
} 

void CAdoDatabase::Release()
{
	Close(); 
	if (m_lpConnection != NULL)
	{
		m_lpConnection.Release();
		m_lpConnection = NULL;
	}
} 

CAdoDatabase::~CAdoDatabase()
{
	Release();
	CoUninitialize();  
} 

_RecordsetPtr CAdoDatabase::Execute(CString sql, long* recordsAffected)
{ 
	_variant_t d((long)0);  
	
	_RecordsetPtr lpRec = m_lpConnection->Execute(_bstr_t(sql),&d , adCmdText);
	
	if (recordsAffected != NULL)
	{
		*recordsAffected = d.lVal;
	}
	return lpRec;
} 

BOOL CAdoDatabase::RunSqlScript(_ConnectionPtr conn,LPCSTR sqlScriptFile, char separator/*=';'*/, char Remark/*='-'*/)
{
	
	BOOL   bRet   =   FALSE;   
	CFileFind   finder;   
	CString   ErrLong;   
	if   (finder.FindFile((CString)sqlScriptFile)   ==   TRUE)   
	{
		CFile   fSql;
		TCHAR   *buffer,*pSQL,*p;
		fSql.Open((CString)sqlScriptFile,CFile::modeRead); 
		UINT   nFileLength   =   fSql.GetLength();
		buffer   =   (TCHAR*)malloc((nFileLength+1)*sizeof(TCHAR));
		_tcsnset(buffer,   TCHAR('\0'),   nFileLength   +   1);
		UINT   nBytesRead   =   fSql.Read(buffer,nFileLength);   //把sql文件内容读入内存缓冲区
		fSql.Close(); 
		p   =   pSQL   =   buffer;
		BOOL   bRunOK   =   FALSE;
		BOOL   bInNote   =   FALSE;
		BOOL   bSkip   =   FALSE;
		CString   strSQL;
		TCHAR   ch;
		ErrLong   =   _T("");
		while   (p   <   (buffer   +   nFileLength))
		{
			//判断是否是注释行
			if   (bInNote)
			{
				if   (*p   ==   TCHAR('\x0a'))
					bInNote   =   FALSE;
			}
			else
			{
				if   ((*p   ==   Remark)   &&   (*(p+1)   ==   Remark))
				{
					bInNote   =   TRUE;
					p++;
				}
				else
				{
					//判断是否是sql语句结束标志
					if   (*p   ==   separator)
					{
						strSQL   =   _T("");
						bSkip   =   FALSE;
						while   (pSQL   <   p)
						{
							if   (bSkip   ==   FALSE)
							{
								if   ((*(pSQL)   ==   Remark)   &&(*(pSQL+1)   ==   Remark))
								{
									bSkip   =   TRUE;
									pSQL++;
								}
								else
								{
									ch   =   *pSQL;
									strSQL   =   strSQL+ch;
								}
							}
							else
							{
								if   (*pSQL   =   TCHAR('\x0a'))
								{
									bSkip   =   FALSE;
									ch   =   TCHAR(' ');
									strSQL   =   strSQL   +   ch;
								}
							}
							pSQL++;
						}
						pSQL   =   p   +   1;
						ErrLong   =   ErrLong   +   _T("--   SQL   Running   ...\n");
						ErrLong   =   ErrLong   +   strSQL;
						_variant_t   vRecords;
						try 
						{
							conn->CursorLocation   =   adUseClient;
							conn->Execute(_bstr_t((LPCTSTR)strSQL),   &vRecords,   adExecuteNoRecords);
							bRunOK   =   TRUE;
						}
						catch(_com_error&)
						{
							bRunOK   =   FALSE;
						}
						if   (bRunOK)
							ErrLong   =   ErrLong   +   _T("\n--   Successed!\n");
						else
						{
							ErrLong   =   ErrLong   +   _T("\n--   Failed!\n");
						}
					}
				}
			}
			p++;
		}
		free(buffer);
		bRet   =   bRunOK;
	}
	return   bRet;   
}

bool CAdoDatabase::Open(const CString & strConn, long nOption)
{
	ConnectString = strConn;
	try
	{
		m_bIsOpen = (m_lpConnection->Open(_bstr_t(ConnectString),_T(""),_T(""), nOption) == 0);
	}
	catch(_com_error &e)
	{
		m_bIsOpen = false; 
		throw e;
	}
	return m_bIsOpen; 
}
