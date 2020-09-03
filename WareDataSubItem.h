// WareDataSubItem.h: interface for the CWareDataSubItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAREDATASUBITEM_H__627A15BE_D457_468B_8493_418BD646695C__INCLUDED_)
#define AFX_WAREDATASUBITEM_H__627A15BE_D457_468B_8493_418BD646695C__INCLUDED_
#include "CurveView.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define CURVE_DISPLAY     0X01
#define CURVE_ANALYSE_40A 0X02   //曲线用于40A,40B录波分析的
#define SAMPLE_INVAILED_VALUE (-32768)     //无效采样值


class CWareDataSubItem  
{
public:
	//若无此采样值，返回SAMPLE_INVAILED_VALUE，否则返回采样值
	//注意：此采样值如是模拟量，则低字节在前，如是开出量，则是0，1
	//如果采样点数不足nMaxSampCount将采用线性插值，对于开入量的线性插值，
	//用中点或以用后一个采样点的值
	virtual short GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount);
	//获取当前子项的采样周波数，如果返回-1表示操作失败
	virtual float GetCircleCount();
	//获取当前子项的有效起始段的采样周波数
	virtual float GetStartCircleCount();
	//获取当前子项的有效返回段的采样周波数
	virtual float GetReturnCircleCount();
	virtual int GetTotalTime();
	//获取指向通道内容的内存的指针
	virtual DWORD GetChannelData(int index);
	BYTE GetDataType();
	BOOL SetDataType(BYTE nTypeCode);
	CWareDataSubItem();
	virtual ~CWareDataSubItem();
	//pFile 录波文件指针，nSubSize子项大小，nCircleCount周波数，nFilePos当前子项的起始地址
	virtual BOOL InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo);
	//获取子项中第index个通道的名字，如果成功返回非零值
	virtual BOOL GetChannelName(int index,CString & strChannelName);
	//获取当前子项的通道数，如果返回-1表示操作失败
	virtual int GetChannelCount();
	//获取当前子项的全程段采样时间间隔，如果返回-1表示操作失败
	virtual float GetSampleInterval();
	//获取当前子项的全程段采样点数，如果返回-1表示操作失败
	virtual int GetSampleDots();
	//获取当前子项的指定通道数，如果返回-1表示操作失败
	virtual int GetAppointCount();
	//获取第nNum个指定通道的通道序号,如果返回-1表示操作失败
	virtual int GetAppointChannelIndex(int nNum);
	//获取index通道的每周通采样点数,如果返回-1表示操作失败
	virtual int GetPointCountInCircle(int index=0);
	//获取曲线的数据信息
	virtual BOOL GetCurve(Curve & curve,int nIndexOfChannel,char ps,
		COleDateTime startSampleTime,COleDateTime firstSampleTime,unsigned short nMillisecondOfStart,
		unsigned short nMillisecondOfFirst,UINT nCutCircleNum=0,UINT nCurveType=CURVE_DISPLAY);


	virtual void Release();
protected:
	UINT m_nSubItemLength;
	UINT m_nFilePos;
	BYTE m_nDataTypeCode;    //数据类型
	BYTE * m_pFilePos;

	// pwq 2016-10-20
	UINT m_nChannelCount;	//通道数
};

#endif // !defined(AFX_WAREDATASUBITEM_H__627A15BE_D457_468B_8493_418BD646695C__INCLUDED_)
