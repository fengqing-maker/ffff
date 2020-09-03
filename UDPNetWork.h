// UDPNetWork.h: interface for the CUDPNetWork class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPNETWORK_H__B2C08032_EAC1_472F_AB8E_BD001A3895E8__INCLUDED_)
#define AFX_UDPNETWORK_H__B2C08032_EAC1_472F_AB8E_BD001A3895E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUDPNetWork  
{
public:
	CUDPNetWork();
	virtual ~CUDPNetWork();
	//ipaddr 32位的IP地址，网络字节序
	bool UDPInit();
	DWORD GetIPAddr()                     {return m_dwIPAddr;}
	CString GetIPAddrStr()                   {return m_strIPAddr;}
	bool  SetIPAddr(const CString & strIPAddr);
	void CableConfig(bool bEnable)        {m_bEnable = bEnable;}
	bool IsEnable()                       {return m_bEnable;}

protected:
	DWORD m_dwIPAddr;   ////ipaddr 32位的IP地址，网络字节序
	bool m_bEnable;
	CString m_strIPAddr;
};

#endif // !defined(AFX_UDPNETWORK_H__B2C08032_EAC1_472F_AB8E_BD001A3895E8__INCLUDED_)
