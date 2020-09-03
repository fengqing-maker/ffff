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

//������[0,USBMW_MAX_SERVER_PORT]�ڵķ������ָ������ϵͳ����
//������[USBMW_MAX_SERVER_PORT+1,0xff]�ڵķ������ָ�������û�����
#define USBMW_MAX_SERVER_PORT               9  
#define USBMW_MAX_SERVER_LENGTH             1500        //usb��ķ������ݵĳ���
#define USBMW_MAX_SERVER_HEAD_LENGTH        20          //��ķ���ͷ��Ϣ�ĳ���
using namespace std;


struct ServerEventNode
{
	CEvent srvEvent;       //���ڵȴ���������
	BYTE   arrRecvBuf[USBMW_MAX_SERVER_LENGTH];       //���ջ�����
	int    nBufLength;     //pRecvBuf���������ܳ���
	int    nDataLength;    //���յ������ݳ���
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
	
	
	map<BYTE,ServerEventNode * > m_mapEvents;     //���ջ�����
	CRITICAL_SECTION m_mapEventCriSection;
protected:
	
	bool m_bConnect;    //�Ƿ�����
	static int WINAPI USBMidWareCallBack(int serviceNum,unsigned char* retData,int frame_size);
	
	CRITICAL_SECTION m_sendCriSection;
	
	deviceInfo m_devInfo;
	bool m_bEnable;   //�Ƿ����
};

#endif // !defined(AFX_USBMIDWARE_H__DE439323_5AF2_4747_832A_BD283257CB7B__INCLUDED_)
