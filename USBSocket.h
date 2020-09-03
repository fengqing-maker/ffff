// USBSocket.h: interface for the CUSBSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USBSOCKET_H__523B3AEF_B6DA_4FDB_8FF2_15784C8FFBEB__INCLUDED_)
#define AFX_USBSOCKET_H__523B3AEF_B6DA_4FDB_8FF2_15784C8FFBEB__INCLUDED_
#include "comSocket.h"
#include "USBMidWare.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUSBSocket :public CcomSocket 
{
public:
	CUSBSocket();
	virtual ~CUSBSocket();
	//创建一个端口号，或服务号为nPort的通过实体（socket）
	virtual bool CreateSocket(UINT nPort);
	virtual bool SendData(void * lpBuf,int nLength);
	virtual int  RecvData(void * lpBuf,int nLength,int nTimeOut);
	virtual void CloseSocket();
public:
	static CUSBMidWare * m_usbMidWare;
};

#endif // !defined(AFX_USBSOCKET_H__523B3AEF_B6DA_4FDB_8FF2_15784C8FFBEB__INCLUDED_)
