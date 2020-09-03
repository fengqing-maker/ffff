// comSocket.cpp: implementation of the CcomSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CcomSocket::CcomSocket()
{
	m_nPort = -1;
}

CcomSocket::~CcomSocket()
{
	if(IsOpen())
	{
		CloseSocket();
	}
}


//创建一个端口号，或服务号为nPort的通过实体（socket）
bool CcomSocket::CreateSocket(UINT nPort)
{	
	return false;
}
bool CcomSocket::SendData(void * lpBuf,int nLength)
{

	return false;
}
int  CcomSocket::RecvData(void * lpBuf,int nLength,int nTimeOut)
{

	return 0;
}
void CcomSocket::CloseSocket()
{
	return;
}
