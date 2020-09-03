// USBMidWare.cpp: implementation of the CUSBMidWare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBMidWare.h"
#include "USBSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUSBMidWare::CUSBMidWare()
{
	m_bEnable = false;    //��ʼ��Ϊ����״̬
}

CUSBMidWare::~CUSBMidWare()
{
	map<BYTE,ServerEventNode*>::iterator iter;
	for(iter = m_mapEvents.begin();iter != m_mapEvents.end();iter++)
	{
		delete iter->second;
	}
	m_mapEvents.erase(m_mapEvents.begin(),m_mapEvents.end());
	DeleteCriticalSection(&m_sendCriSection);
	DeleteCriticalSection(&m_mapEventCriSection);
}


void CUSBMidWare::USBMidWareInit(deviceInfo DF)
{
	
	m_bConnect = false;
	m_devInfo.pid = DF.pid;
	m_devInfo.vid = DF.vid;
	CableConfig(true);
	ASSERT(CUSBMidWare::m_mapEvents.begin() == CUSBMidWare::m_mapEvents.end());
	InitializeCriticalSection(&m_sendCriSection);
	InitializeCriticalSection(&m_mapEventCriSection);
}
/****************************************************
��������ResetUSBMidWare
��������������usb���ӡ����õ�
������
����ֵ��
��¼��
*****************************************************/
bool CUSBMidWare::ResetUSBMidWare()
{
	if(!m_bEnable) return false;
	if (m_bConnect)
	{
		CableConfig(false);
		CableConfig(true);
	}
	else
	{	
		CableConfig(true);
	}
	m_bConnect = false;
	if (InitIntermediate(CUSBMidWare::USBMidWareCallBack,m_devInfo) != 1 )
	{
		CableConfig(false);
		m_bConnect = false;
	}
	else
	{
		m_bConnect = true;
	}
	
	return m_bConnect;
}
// nPort <= USBMW_MAX_SERVER_PORT��ʾϵͳ���񣬽�ע��ʧ��
bool CUSBMidWare::RegServerPort(BYTE nPort)
{
	if(nPort <= USBMW_MAX_SERVER_PORT) return false;
	if(m_mapEvents.end() ==	m_mapEvents.find(nPort))
	{
		ServerEventNode *pSrvEvent = new ServerEventNode;
		EnterCriticalSection(&m_mapEventCriSection);
		m_mapEvents.insert(map<BYTE,ServerEventNode*>::value_type(nPort,pSrvEvent));
		LeaveCriticalSection(&m_mapEventCriSection);
		return true;
	}
	else
	{
		//�Ѿ�ע�����
		return TRUE;
	}
}

//nLength���ó���USBMW_MAX_SERVER_LENGTH
bool CUSBMidWare::SendData(BYTE nPort,BYTE *pBuf,int nLength)
{ 
	int Priority = 3;//default priority
	switch (nPort)
	{
	case 20:
	case 25:
		Priority = 3;
		break;
	case 23:
	case 21:
	case 26:
		Priority = 2;
		break;
	case 22:
		Priority = 1;
		break;
	}
	if(!m_bEnable) return false;
	if(!IsConnect()  && !ResetUSBMidWare())
	{
		return false;
	}
	if(nLength >= USBMW_MAX_SERVER_LENGTH)
	{
		return false;
	}
	ServerEventNode *pSrvEventNode;
	EnterCriticalSection(&m_mapEventCriSection);
	map<BYTE,ServerEventNode*>::iterator iter = m_mapEvents.find(nPort);
	if(m_mapEvents.end() ==	iter) 
	{
		LeaveCriticalSection(&m_mapEventCriSection);
		//δע������͵ķ���
		return false;
	}
	pSrvEventNode = iter->second;
	LeaveCriticalSection(&m_mapEventCriSection);
	BYTE tempBuf[USBMW_MAX_SERVER_LENGTH+USBMW_MAX_SERVER_HEAD_LENGTH];
	tempBuf[2] = nPort;
	*(int *)(tempBuf+3) = 0;
	memcpy(tempBuf+7,pBuf,nLength);
	*(unsigned short*)(tempBuf) = nLength + 5;
	
	EnterCriticalSection(&m_sendCriSection);
	Interface(tempBuf,Priority);
	LeaveCriticalSection(&m_sendCriSection);

	pSrvEventNode->srvEvent.ResetEvent();
	return true;
}
/***************************************************************************
����ֵ��
 0:      ���ճ�ʱ
-1:      δע������͵ķ���
-2:      ���ջ�����̫С
-3��     ϵͳ������̫С
>0:      ���յ��������ֽ���
***************************************************************************/
int CUSBMidWare::RecvData(BYTE nPort,BYTE *pBuf,int nLength,int nTimeOut)
{
	if(!m_bEnable)
	{
		return 0;
	}
	if(!IsConnect()  && !ResetUSBMidWare())
	{
		return 0;
	}
	ServerEventNode *pSrvEventNode;
	EnterCriticalSection(&m_mapEventCriSection);
	map<BYTE,ServerEventNode*>::iterator iter = m_mapEvents.find(nPort);
	if(iter == m_mapEvents.end())
	{
		LeaveCriticalSection(&m_mapEventCriSection);
		//δע������͵ķ���
		return -1;
	}
	pSrvEventNode = iter->second;
	LeaveCriticalSection(&m_mapEventCriSection);
	int re;
	if(WAIT_TIMEOUT ==WaitForSingleObject(pSrvEventNode->srvEvent.m_hObject,nTimeOut))
	//���ͷ�pSrvEventNode->srvEvent.m_hObjec????????
	{
		//���ճ�ʱ
		/*m_bConnect = false;*/
		re = 0;
	}
	if(pSrvEventNode->nDataLength < 0)
	{
		//ϵͳ������������̫С
		re = -3;
	}
	if(pSrvEventNode->nDataLength > nLength)
	{
		//���ջ�����̫С
		re = -2;
	}
	else
	{
		re = pSrvEventNode->nDataLength;
		memcpy(pBuf,pSrvEventNode->arrRecvBuf,re);
		
	}
	pSrvEventNode->nDataLength = 0;
	return re;
}
bool CUSBMidWare::DelServerPort(BYTE nPort)
{
	ASSERT(m_mapEvents.begin() != m_mapEvents.end());
	EnterCriticalSection(&m_mapEventCriSection);
	map<BYTE,ServerEventNode*>::iterator iter = m_mapEvents.find(nPort);
	if(iter == m_mapEvents.end())
	{
		LeaveCriticalSection(&m_mapEventCriSection);
		//δע������͵ķ���
		return false;
	}
	delete iter->second;
	m_mapEvents.erase(iter);
	LeaveCriticalSection(&m_mapEventCriSection);
	return true;
}

/*************************************************************************************
�������ƣ�USBMidWareCallBack
��Ҫ�������м��ص�����
������
    serviceNum             �������ͺ�
	retData                ���յ��ķ������ݻ�����������������֡ͷ
	frame_size             retData����������
����ֵ��
	�û��Լ��ж���
���û������м�����
�޸���־����
*************************************************************************************/
int WINAPI CUSBMidWare::USBMidWareCallBack(int serviceNum,unsigned char* retData,int frame_size)
{
	ASSERT(serviceNum > USBMW_MAX_SERVER_PORT && serviceNum <= 0xff);
	retData += 5;
	frame_size -= 5;
	ASSERT(frame_size > 0);
	BYTE nPort = (BYTE)serviceNum;
	ServerEventNode * pSrvEventNode;
	EnterCriticalSection(&CUSBSocket::m_usbMidWare->m_mapEventCriSection);
	map<BYTE,ServerEventNode*>::iterator iter = CUSBSocket::m_usbMidWare->m_mapEvents.find(nPort);
	if(iter == CUSBSocket::m_usbMidWare->m_mapEvents.end())
	{
		LeaveCriticalSection(&CUSBSocket::m_usbMidWare->m_mapEventCriSection);
		return 0;
	}
	pSrvEventNode = iter->second;
	LeaveCriticalSection(&CUSBSocket::m_usbMidWare->m_mapEventCriSection);
	if(frame_size > pSrvEventNode->nBufLength)
	{
		pSrvEventNode->nDataLength = 0 - frame_size;
	}
	else
	{
		memcpy(pSrvEventNode->arrRecvBuf,retData,frame_size);
		pSrvEventNode->nDataLength = frame_size;
	}
	ASSERT(pSrvEventNode->srvEvent.m_hObject != NULL);
	pSrvEventNode->srvEvent.SetEvent();
	return 0;
}

void CUSBMidWare::CableConfig(bool bEnable)
{
	if(!bEnable)
	{
		CloseMidSerial();
		m_bConnect = false;
	}
	m_bEnable = bEnable;
}