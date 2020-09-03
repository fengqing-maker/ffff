// USBMidWare.h: interface for the CUSBMidWare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USBMIDWARE_H__DE439323_5AF2_4747_832A_BD283257CB7B__INCLUDED_)
#define AFX_USBMIDWARE_H__DE439323_5AF2_4747_832A_BD283257CB7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
#include "midSerialLib.h"

//在区间[0,USBMW_MAX_SERVER_PORT]内的服务号所指定的是系统服务
//在区间[USBMW_MAX_SERVER_PORT+1,0xff]内的服务号所指定的是用户服务
#define USBMW_MAX_SERVER_PORT               9  
#define USBMW_MAX_SERVER_LENGTH             1500        //usb最长的服务数据的长度
#define USBMW_MAX_SERVER_HEAD_LENGTH        20          //最长的服务头信息的长度
using namespace std;


struct ServerEventNode
{
	CEvent srvEvent;       //用于等待服务数据
	BYTE   arrRecvBuf[USBMW_MAX_SERVER_LENGTH];       //接收缓冲区
	int    nBufLength;     //pRecvBuf缓冲区的总长度
	int    nDataLength;    //接收到的数据长度
	ServerEventNode()
	{
		nBufLength = sizeof(arrRecvBuf);
		nDataLength = 0;
	}
};

class CUSBMidWare  
{
public:
	CUSBMidWare();
	virtual ~CUSBMidWare();
	void USBMidWareInit(deviceInfo DF);
	bool RegServerPort(BYTE nPort);
	bool SendData(BYTE nPort,BYTE *pBuf,int nLength);
	int RecvData(BYTE nPort,BYTE *pBuf,int nLength,int nTimeOut);
	bool DelServerPort(BYTE nPort);
	inline bool IsConnect() {return m_bConnect;}
	bool ResetUSBMidWare();
	void CableConfig(bool bEnable);
	
	
	map<BYTE,ServerEventNode * > m_mapEvents;     //接收缓存区
	CRITICAL_SECTION m_mapEventCriSection;
protected:
	
	bool m_bConnect;    //是否连接
	static int WINAPI USBMidWareCallBack(int serviceNum,unsigned char* retData,int frame_size);
	
	CRITICAL_SECTION m_sendCriSection;
	
	deviceInfo m_devInfo;
	bool m_bEnable;   //是否可用
};

#endif // !defined(AFX_USBMIDWARE_H__DE439323_5AF2_4747_832A_BD283257CB7B__INCLUDED_)
