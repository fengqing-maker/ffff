// NetSocket.h: interface for the CNetSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETSOCKET_H__D807C773_9729_4ED6_AF74_74BDDB5310AE__INCLUDED_)
#define AFX_NETSOCKET_H__D807C773_9729_4ED6_AF74_74BDDB5310AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "comSocket.h"
#include <winsock2.h>
#include "UDPNetWork.h"
class CNetSocket  :public CcomSocket
{
public:
	CNetSocket();
	virtual ~CNetSocket();
	//创建一个端口号，或服务号为nPort的通过实体（socket）
	virtual bool CreateSocket(UINT nPort);
	virtual bool SendData(void * lpBuf,int nLength);
	virtual int  RecvData(void * lpBuf,int nLength,int nTimeOut);
	virtual void CloseSocket();
protected:
	SOCKET m_socket;
	sockaddr_in m_bindSockAddr;
	sockaddr_in m_sendSockAddr;
	WSAEVENT m_wEvent;
public:
	static CUDPNetWork m_udpNetWork;
};

#endif // !defined(AFX_NETSOCKET_H__D807C773_9729_4ED6_AF74_74BDDB5310AE__INCLUDED_)
