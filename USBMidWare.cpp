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
	m_bEnable = false;    //初始化为可用状态
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
函数名：ResetUSBMidWare
描述：重新设置usb连接、可用等
参数：
返回值：
记录：
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
// nPort <= USBMW_MAX_SERVER_PORT表示系统服务，将注册失败
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
		//已经注册过了
		return TRUE;
	}
}

//nLength不得超过USBMW_MAX_SERVER_LENGTH
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
		//未注册此类型的服务
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
返回值：
 0:      接收超时
-1:      未注册此类型的服务
-2:      接收缓冲区太小
-3：     系统缓冲区太小
>0:      接收到的数据字节数
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
		//未注册此类型的服务
		return -1;
	}
	pSrvEventNode = iter->second;
	LeaveCriticalSection(&m_mapEventCriSection);
	int re;
	if(WAIT_TIMEOUT ==WaitForSingleObject(pSrvEventNode->srvEvent.m_hObject,nTimeOut))
	//不释放pSrvEventNode->srvEvent.m_hObjec????????
	{
		//接收超时
		/*m_bConnect = false;*/
		re = 0;
	}
	if(pSrvEventNode->nDataLength < 0)
	{
		//系统缓冲区缓冲区太小
		re = -3;
	}
	if(pSrvEventNode->nDataLength > nLength)
	{
		//接收缓冲区太小
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
		//未注册此类型的服务
		return false;
	}
	delete iter->second;
	m_mapEvents.erase(iter);
	LeaveCriticalSection(&m_mapEventCriSection);
	return true;
}

/*************************************************************************************
函数名称：USBMidWareCallBack
简要描述：中间层回调函数
参数：
    serviceNum             服务类型号
	retData                接收到的服务数据缓冲区，不包含服务帧头
	frame_size             retData数据区长度
返回值：
	用户自己行定义
调用环境：中间层调用
修改日志：无
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