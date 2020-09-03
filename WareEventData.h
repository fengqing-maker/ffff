// WareEventData.h: interface for the CWareEventData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAREEVENTDATA_H__7D830C68_2A58_4F09_B86A_DC23ABC4943B__INCLUDED_)
#define AFX_WAREEVENTDATA_H__7D830C68_2A58_4F09_B86A_DC23ABC4943B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WareDataSubItem.h"
#define NAME_LEN_IN_FILE  32                //�ļ���������ռ�ĳ���
#define WARE_EVENT_TYPE           0X05        //�¼����� 
struct CEventDescribe
{
	
	unsigned short nMillisecond;	//����
	LONG           nEventPosX;      //��ʱ����Ľ�����ʾ������X
	double         diffMsTime;		//������ʱ�������ʱ�䣬�ɺ��뼶nMillisecond�������,��λms
	bool           isVail;          //�¼�ʱ���Ƿ���Ч
	BYTE           type;			//1:Ϊ�����¼� 2:Ϊ¼���¼�
	char           m_strRemark[100];
	char           name[NAME_LEN_IN_FILE + 1];
	COleDateTime   eventTime;
    CEventDescribe()
	{
		memset(name, 0, NAME_LEN_IN_FILE + 1);
		nMillisecond = 0;
		diffMsTime = 0;
		nEventPosX = 0;
		isVail = false;
	}
	CEventDescribe& CopyObject( const CEventDescribe& copyObject )
	{
		memcpy(name, copyObject.name, NAME_LEN_IN_FILE + 1);
		memcpy(m_strRemark, copyObject.m_strRemark, sizeof(m_strRemark));
		eventTime = copyObject.eventTime;
		nMillisecond = copyObject.nMillisecond;
		diffMsTime = copyObject.diffMsTime;
		type = copyObject.type;
		isVail = copyObject.isVail;
		nEventPosX = copyObject.nEventPosX;
		return *this;
	}
	CEventDescribe(const CEventDescribe& copyObject)
	{
		CopyObject( copyObject );
	}
	CEventDescribe& operator=(const CEventDescribe& copyObject)
	{
		return CopyObject( copyObject );
	}
	
};
class CWareEventData : public CWareDataSubItem  
{
public:
	CWareEventData();
	CWareEventData(const CWareEventData &currentData);
	virtual ~CWareEventData();
	virtual BOOL InitSubItemData( BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo);
	//��ȡ��ǰ����Ķ�ֵ�������������-1��ʾ����ʧ��
	virtual int GetChannelCount(  ) ;
	////��ȡ��ǰ����ı�����ֵ���������������-1��ʾ����ʧ��
	//virtual int GetAppointCount();
	//��ȡָ���ڴ��ָ��
	//virtual DWORD GetChannelData(int index) const;
	virtual	DWORD GetChannelData(int index) ;
	void  AddEventDes( const CEventDescribe &event );
	virtual void Release();
	CWareEventData& operator=(const CWareEventData &currentData);
	void CloneCopy(const CWareEventData &currentData);
    
	//void AddEvent();
protected:
private:
	vector<CEventDescribe> m_eventDescribe; //����
	//unsigned short m_nWareEventCount;               //¼���¼��ĸ���
	//CEventDescribe *m_pWareEven;                   //�洢¼���¼���ָ��
};

#endif // !defined(AFX_WAREEVENTDATA_H__7D830C68_2A58_4F09_B86A_DC23ABC4943B__INCLUDED_)
