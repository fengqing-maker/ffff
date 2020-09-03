// ProtectData.h: interface for the CProtectData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTECTDATA_H__31843B00_9C79_40F3_83C8_D50948DA1E4E__INCLUDED_)
#define AFX_PROTECTDATA_H__31843B00_9C79_40F3_83C8_D50948DA1E4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//保护定值
struct ProtectPara
{
	char strParaType[21];   //保护定值类型
	char strParaName[33];   //保护定值名称
	char strParaValue[21];  //保护定值数值
	ProtectPara()
	{
		::memset(strParaName,0,sizeof(strParaName));
		::memset(strParaType,0,sizeof(strParaType));
		::memset(strParaValue,0,sizeof(strParaValue));
	}
	ProtectPara & operator =(const ProtectPara & proPara)
	{
		::memcpy(strParaValue,proPara.strParaValue,sizeof(strParaValue));
		::memcpy(strParaName,proPara.strParaName,sizeof(strParaName));
		::memcpy(strParaType,proPara.strParaType,sizeof(strParaType));
		return *this;
	}
	
};

//录波保护定值子项：该子项主要用于描述保护动作时的保护定值。
class CProtectData:public CWareDataSubItem
{
public:
	virtual BOOL InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo);

	//获取当前子项的定值个数，如果返回-1表示操作失败
	virtual int GetChannelCount();
	//获取当前子项的保护定值区，如果返回-1表示操作失败
	virtual int GetAppointCount();
	//获取指向内存的指针
	virtual DWORD GetChannelData(int index);
	virtual void Release();
	CProtectData();
	virtual ~CProtectData();


private:
	BYTE m_nParaAreaNum;                       //录波文件中保护定值的定值区号
	unsigned short m_nParaCount;               //保护定值的个数
	ProtectPara *m_pProPara;                   //存储保护定值的指针
};

#endif // !defined(AFX_PROTECTDATA_H__31843B00_9C79_40F3_83C8_D50948DA1E4E__INCLUDED_)
