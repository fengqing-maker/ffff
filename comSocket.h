// comSocket.h: interface for the CcomSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMSOCKET_H__F16666BC_1167_47BB_9BFF_3A798796E381__INCLUDED_)
#define AFX_COMSOCKET_H__F16666BC_1167_47BB_9BFF_3A798796E381__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//Socker�ӿ��ļ�
class CcomSocket  
{
public:
	CcomSocket();
	virtual ~CcomSocket();
	//����һ���˿ںţ�������ΪnPort��ͨ��ʵ�壨socket��
	virtual bool CreateSocket(UINT nPort);
	virtual bool SendData(void * lpBuf,int nLength);
	virtual int  RecvData(void * lpBuf,int nLength,int nTimeOut);
	virtual void CloseSocket();
	inline bool IsOpen() {return ((m_nPort != -1) ? true : false);}
protected:
	int  m_nPort;

};

//



#endif // !defined(AFX_COMSOCKET_H__F16666BC_1167_47BB_9BFF_3A798796E381__INCLUDED_)
