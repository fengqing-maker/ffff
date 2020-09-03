// ComService.cpp: implementation of the CComService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SID_2FY.h"
#include "ComService.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CComService::CComService()
{
	m_nPort = -1;
	m_frameCount=1;
	m_pComSock = NULL;
}

CComService::~CComService()
{
	if(m_pComSock != NULL)
	{
		delete m_pComSock;
		m_pComSock = NULL;
	}
}

bool CComService::OpenService(ComType comType,int nPort)
{
	bool bRe=false;
	switch(comType)
	{
	case EN_USBMW:
		if(m_pComSock != NULL)
		{
			delete m_pComSock;
			m_pComSock = NULL;
		}
		m_pComSock = new CUSBSocket;
		if(m_pComSock->CreateSocket(nPort))
		{
			m_comType = EN_USBMW;
			m_nPort = nPort;
			bRe=true;
		}
		else
		{
			delete m_pComSock;
			m_pComSock = NULL;
		}
		break;
	case EN_NETUDP:
		if(m_pComSock != NULL)
		{
			delete m_pComSock;
		}
		m_pComSock = new CNetSocket;
		if(m_pComSock->CreateSocket(nPort))
		{
			m_comType = EN_NETUDP;
			m_nPort = nPort;
			bRe=true;
		}
		else
		{
			delete m_pComSock;
			m_pComSock = NULL;
		}
		break;
	default:
		ASSERT(FALSE);
	}
	return bRe;
}


bool CComService::RecvData(UINT nTimeOut)
{
	ASSERT(m_pComSock && m_pComSock->IsOpen());
	m_nRecvFrameSize = m_pComSock->RecvData(m_recvFrame,sizeof(m_recvFrame),nTimeOut);
	return (m_nRecvFrameSize > 0) ? true:false;
}

bool CComService::SendData()
{
	ASSERT(m_pComSock && m_pComSock->IsOpen());
	return m_pComSock->SendData(m_sendFrame,m_nSendFrameSize);
}