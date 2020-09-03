// WareEventData.h: interface for the CWareEventData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAREEVENTDATA_H__7D830C68_2A58_4F09_B86A_DC23ABC4943B__INCLUDED_)
#define AFX_WAREEVENTDATA_H__7D830C68_2A58_4F09_B86A_DC23ABC4943B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WareDataSubItem.h"
#define NAME_LEN_IN_FILE  32                //文件中名字所占的长度
#define WARE_EVENT_TYPE           0X05        //事件子项 
struct CEventDescribe
{
	
	unsigned short nMillisecond;	//毫秒
	LONG           nEventPosX;      //在时间轴的界面显示的像素X
	double         diffMsTime;		//与启动时间的相差的时间，由毫秒级nMillisecond计算得来,单位ms
	bool           isVail;          //事件时间是否有效
	BYTE           type;			//1:为保护事件 2:为录波事件
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
	//获取当前子项的定值个数，如果返回-1表示操作失败
	virtual int GetChannelCount(  ) ;
	////获取当前子项的保护定值区个数，如果返回-1表示操作失败
	//virtual int GetAppointCount();
	//获取指向内存的指针
	//virtual DWORD GetChannelData(int index) const;
	virtual	DWORD GetChannelData(int index) ;
	void  AddEventDes( const CEventDescribe &event );
	virtual void Release();
	CWareEventData& operator=(const CWareEventData &currentData);
	void CloneCopy(const CWareEventData &currentData);
    
	//void AddEvent();
protected:
private:
	vector<CEventDescribe> m_eventDescribe; //描述
	//unsigned short m_nWareEventCount;               //录波事件的个数
	//CEventDescribe *m_pWareEven;                   //存储录波事件的指针
};

#endif // !defined(AFX_WAREEVENTDATA_H__7D830C68_2A58_4F09_B86A_DC23ABC4943B__INCLUDED_)
