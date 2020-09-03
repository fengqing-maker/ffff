// ProtectData.h: interface for the CProtectData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTECTDATA_H__31843B00_9C79_40F3_83C8_D50948DA1E4E__INCLUDED_)
#define AFX_PROTECTDATA_H__31843B00_9C79_40F3_83C8_D50948DA1E4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//������ֵ
struct ProtectPara
{
	char strParaType[21];   //������ֵ����
	char strParaName[33];   //������ֵ����
	char strParaValue[21];  //������ֵ��ֵ
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

//¼��������ֵ�����������Ҫ����������������ʱ�ı�����ֵ��
class CProtectData:public CWareDataSubItem
{
public:
	virtual BOOL InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo);

	//��ȡ��ǰ����Ķ�ֵ�������������-1��ʾ����ʧ��
	virtual int GetChannelCount();
	//��ȡ��ǰ����ı�����ֵ�����������-1��ʾ����ʧ��
	virtual int GetAppointCount();
	//��ȡָ���ڴ��ָ��
	virtual DWORD GetChannelData(int index);
	virtual void Release();
	CProtectData();
	virtual ~CProtectData();


private:
	BYTE m_nParaAreaNum;                       //¼���ļ��б�����ֵ�Ķ�ֵ����
	unsigned short m_nParaCount;               //������ֵ�ĸ���
	ProtectPara *m_pProPara;                   //�洢������ֵ��ָ��
};

#endif // !defined(AFX_PROTECTDATA_H__31843B00_9C79_40F3_83C8_D50948DA1E4E__INCLUDED_)
