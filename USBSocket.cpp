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
	//lhz modiy �ڽ��и�������ʱ�������Ѿ�������ɣ�����
	//CloseSocket(),�е���CUSBMidWare�ĺ����Ѿ�������
	//CloseSocket();
}


//����һ���˿ںţ�������ΪnPort��ͨ��ʵ�壨socket��
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
����ֵ��
 0:      ���ճ�ʱ
-1:      ���ջ�����̫С
>0:      ���յ��������ֽ���
***************************************************************************/
int  CUSBSocket::RecvData(void * lpBuf,int nLength,int nTimeOut)
{
	ASSERT(m_nPort>0 && m_nPort<0xff);
	ASSERT(lpBuf && nLength >0);
	int re = m_usbMidWare->RecvData(m_nPort,(BYTE *)lpBuf,nLength,nTimeOut);
	ASSERT(re != -1);    //������δע��˷���
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
