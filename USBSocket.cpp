// USBSocket.cpp: implementation of the CUSBSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBSocket.h"
#include "USBMidWare.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUSBMidWare * CUSBSocket::m_usbMidWare = NULL;

CUSBSocket::CUSBSocket()
{

}

CUSBSocket::~CUSBSocket()
{
	//lhz modiy 在进行父类析构时，子类已经析构完成，而在
	//CloseSocket(),有调用CUSBMidWare的函数已经无意义
	//CloseSocket();
}


//创建一个端口号，或服务号为nPort的通过实体（socket）
bool CUSBSocket::CreateSocket(UINT nPort)
{	
	ASSERT(m_nPort == -1);
	if(m_usbMidWare->RegServerPort(nPort))
	{
		m_nPort = nPort;
	}
	return m_nPort != -1;
}
bool CUSBSocket::SendData(void * lpBuf,int nLength)
{
	ASSERT(m_nPort>=0 && m_nPort<=0xff);
	ASSERT(lpBuf && nLength >0);
	return m_usbMidWare->SendData((BYTE)m_nPort,(BYTE *)lpBuf,nLength);
}
/***************************************************************************
返回值：
 0:      接收超时
-1:      接收缓冲区太小
>0:      接收到的数据字节数
***************************************************************************/
int  CUSBSocket::RecvData(void * lpBuf,int nLength,int nTimeOut)
{
	ASSERT(m_nPort>0 && m_nPort<0xff);
	ASSERT(lpBuf && nLength >0);
	int re = m_usbMidWare->RecvData(m_nPort,(BYTE *)lpBuf,nLength,nTimeOut);
	ASSERT(re != -1);    //不可能未注册此服务
	if(re < 0) 
	{
		ASSERT(re == -2);
		return -1;
	}
	return re;
}
void CUSBSocket::CloseSocket()
{
	if(m_nPort>0)
	{
		ASSERT(m_nPort>0 && m_nPort<0xff);
		ASSERT(m_usbMidWare->DelServerPort(m_nPort));
		m_nPort = -1;
	}
}
