// FrequencyAndAngle.h: interface for the CFrequencyAndAngle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREQUENCYANDANGLE_H__8AE8F729_0B65_43C2_8E24_F3533E09A72E__INCLUDED_)
#define AFX_FREQUENCYANDANGLE_H__8AE8F729_0B65_43C2_8E24_F3533E09A72E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WareDataSubItem.h"
//频率通道数据
struct FreAngChannel
{
	long channelNum;            //通道号
	char channelName[33];       //通道名称
	char channelFirstUint[9];   //单位
	double a;                   //通道系数（a）
	double b;                   //通道偏移数值（b）
	double delayTime;           //in millisecond通道时滞
	long sampleMax;             //采样最大数值
	long sampleMin;             //采样最小数值
	double dRatingOfSecondValue;   //二次额定值
	long indexOfWareData;         //录波数据的起始指针在文件中的位置
	FreAngChannel & operator=(const AnalogChannel & channel)
	{
		channelNum=channel.channelNum;
		::memcpy(channelName,channel.channelName,sizeof(channelName));
		::memcpy(channelFirstUint,channel.channelFirstUint,sizeof(channelFirstUint));
		a=channel.a;
		b=channel.b;
		delayTime=channel.delayTime;
		sampleMax=channel.sampleMax;
		sampleMin=channel.sampleMin;
		indexOfWareData=channel.indexOfWareData;
		return *this;
	}
};

class CFrequencyAndAngle : public CWareDataSubItem  
{
public:
	CFrequencyAndAngle();
	virtual ~CFrequencyAndAngle();
public:
	//若无此采样值，返回SAMPLE_INVAILED_VALUE，否则返回采样值
	//注意：此采样值如是模拟量，则低字节在前，如是开出量，则是0，1
	//如果采样点数不足nMaxSampCount将采用线性插值，对于开入量的线性插值，
	//是中点或以上用后一个采样点的值
	virtual short GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount);
	//获取指向通道内容的内存的指针
	virtual DWORD GetChannelData(int index);
	//获取当前子项的采样周波数，如果返回-1表示操作失败
	virtual float GetCircleCount();
	virtual float GetStartCircleCount();
	virtual float GetReturnCircleCount();
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
	virtual int GetPointCountInCircle(int index=0);
	//获取曲线的数据信息
	virtual BOOL GetCurve(Curve & curve,int nIndexOfChannel,char ps,
		COleDateTime startSampleTime,COleDateTime firstSampleTime,unsigned short nMillisecondOfStart,
		unsigned short nMillisecondOfFirst,UINT nCutCircleNum=0,UINT nCurveType=CURVE_DISPLAY);
	virtual void Release();
	
protected:
	double GetValueOfPoint(const FreAngChannel &channelData, long index);
	BOOL GetAllFreqChannelData(UINT & nPos);
	BYTE m_nChannelCount;                       //频率或角差通道数（NFR）
	FreAngChannel* m_pFreAngChannel;
	unsigned short m_nStaSamDots;               //全程起始段频率采样点数
	unsigned short m_nRetSamDots;               //全程返回段频率采样点数 
	float m_nSamInterval;           	//全程频率采样间隔
	WaveInfo m_WaveInfo;
};

#endif // !defined(AFX_FREQUENCYANDANGLE_H__8AE8F729_0B65_43C2_8E24_F3533E09A72E__INCLUDED_)
