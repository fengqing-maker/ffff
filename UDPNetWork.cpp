// UDPNetWork.cpp: implementation of the CUDPNetWork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UDPNetWork.h"
#include <winsock2.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUDPNetWork::CUDPNetWork()
{
	m_bEnable = false;
}

CUDPNetWork::~CUDPNetWork()
{

}

bool CUDPNetWork::UDPInit()
{
	//套接字版本的引入
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		return false;
	}
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
		WSACleanup( );
		return false; 
	}
	return true;
}


bool  CUDPNetWork::SetIPAddr(const CString & strIPAddr)
{
	char tempFileName[255];
	WideCharToMultiByte(CP_ACP,0,strIPAddr,-1,tempFileName,255,NULL,NULL);
	DWORD dwTemp = inet_addr(tempFileName);
	if(m_dwIPAddr == INADDR_NONE)
	{

		return false;
	}
	else
	{
		m_dwIPAddr = dwTemp;
		m_strIPAddr = strIPAddr;
		return true;
	}
}