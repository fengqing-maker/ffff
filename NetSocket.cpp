// NetSocket.cpp: implementation of the CNetSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetSocket.h"
#include "SID_2FY.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern CMySID_2FYApp theApp;
CUDPNetWork CNetSocket::m_udpNetWork;
CNetSocket::CNetSocket()
{
	m_socket=INVALID_SOCKET;
}

CNetSocket::~CNetSocket()
{
	CloseSocket();
}

//创建一个端口号，或服务号为nPort的通过实体（socket）
bool CNetSocket::CreateSocket(UINT nPort)
{	
	m_nPort = nPort;
	m_socket=::socket(AF_INET,SOCK_DGRAM,0);
	CString info;
	if(INVALID_SOCKET==m_socket)
	{
		info.LoadString( IDS_SOCKET_ERROR1 );
		CdMessageBox box( theApp.m_pMainWnd, info );
		box.MsgBoxEx();
		return false;
	}

	m_wEvent=::WSACreateEvent();
	if(::WSAEventSelect(m_socket,m_wEvent,FD_READ))
	{
		info.LoadString( IDS_SOCKETEVENT_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}

	m_sendSockAddr.sin_family=AF_INET;
	m_sendSockAddr.sin_port=htons(nPort);
	m_sendSockAddr.sin_addr.S_un.S_addr = CNetSocket::m_udpNetWork.GetIPAddr();

	m_bindSockAddr.sin_port=htons(nPort);
	m_bindSockAddr.sin_family=AF_INET;
	m_bindSockAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);

	if(bind(m_socket,(sockaddr *)&m_bindSockAddr,sizeof(m_bindSockAddr)))
	{
		
		return false;
	}
	return true;
}
/*******************************************************
函数名：SendData
描述：用网口连接设备时，发送数据函数
参数：lpBuf[in]:发送的内容；nLength[in]:发送数据长度
返回值：成功返回true，失败返回false
*******************************************************/
bool CNetSocket::SendData(void * lpBuf,int nLength)
{
	if(!m_udpNetWork.IsEnable()) return false;
	m_sendSockAddr.sin_addr.S_un.S_addr = m_udpNetWork.GetIPAddr();
	if(SOCKET_ERROR==sendto(m_socket,(char *)lpBuf,
		nLength,0,(sockaddr *)&m_sendSockAddr,sizeof(m_sendSockAddr)))
	{
		//请数据失败！
		return false;
	}
	return true;
}
/***************************************************************************
返回值：
 0:      接收超时
-1:      接收失败
>0:      接收到的数据字节数
***************************************************************************/
int  CNetSocket::RecvData(void * lpBuf,int nLength,int nTimeOut)
{
	
	sockaddr_in recvSock;
 	int sockLen = sizeof(recvSock);
	int nRecvData;
	DWORD index=::WSAWaitForMultipleEvents(1,&m_wEvent,TRUE,nTimeOut,FALSE);  //进入临界区
	//TRACE(_T("recv wait on %d port\n"),m_nPort);
	if(index==WSA_WAIT_FAILED||index==WSA_WAIT_TIMEOUT)
		return 0;

	WSANETWORKEVENTS wnetEvent;
	::WSAEnumNetworkEvents(m_socket,m_wEvent,&wnetEvent);
	if(wnetEvent.lNetworkEvents & FD_READ)
	{
		if(wnetEvent.iErrorCode[FD_READ_BIT]!=0)
		{
			return -1;
		}
		nRecvData=recvfrom(m_socket,(char *)lpBuf,nLength,
			0,(sockaddr *)&recvSock,&sockLen);
		if(SOCKET_ERROR==nRecvData)
		{
			//接收失败！
			return -1;
		}
		//TRACE(_T("recv seccuss on %d port\n"),m_nPort);
		return nRecvData;
	}
	return -1;

}
void CNetSocket::CloseSocket()
{
	if(m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		WSACloseEvent(m_wEvent);
		m_socket=INVALID_SOCKET;
	}
}
