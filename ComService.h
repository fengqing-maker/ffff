// ComService.h: interface for the CComService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMSERVICE_H__2EEAF0C4_DF95_4D77_8C8D_C3C11374AE47__INCLUDED_)
#define AFX_COMSERVICE_H__2EEAF0C4_DF95_4D77_8C8D_C3C11374AE47__INCLUDED_

#include "comSocket.h"	// Added by ClassView
#include "frame.h"
#include "NetSocket.h"
#include "USBMidWare.h"
#include "USBSocket.h"
#include "UDPNetWork.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ComType{EN_USBMW,EN_NETUDP};

class CComService  
{
public:
	CComService();
	virtual ~CComService();
	bool OpenService(ComType comType,int nPort);
	bool SendData();
	bool RecvData(UINT nTimeOut);
	ComType GetComType(){return m_comType;}
friend class CFrame;

protected:
	CcomSocket *m_pComSock;
	BYTE m_sendFrame[1200];   //发送缓存数组
	int m_nSendFrameSize;
	BYTE m_recvFrame[1200];   //接收缓存数组
	short m_nRecvFrameSize;
	int  m_nPort;
	ComType m_comType;   //连接方式
	unsigned short m_frameCount;

};

#endif // !defined(AFX_COMSERVICE_H__2EEAF0C4_DF95_4D77_8C8D_C3C11374AE47__INCLUDED_)
