// AnalogData.h: interface for the CAnalogData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALOGDATA_H__B6E33523_13CD_4EBC_AA21_B2E7BDAEB4D2__INCLUDED_)
#define AFX_ANALOGDATA_H__B6E33523_13CD_4EBC_AA21_B2E7BDAEB4D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "WareFileData.h"
#include "WareDataSubItem.h"

#define AVAIL_FACTOR_I 100
#define AVAIL_FACTOR_U 10

//模拟量通道信息
struct AnalogChannel
{
	long channelNum;               //通道号
	char channelName[33];          //通道名称
	char channelSecondUint[9];     //一次值单位
	char channelFirstUint[9];      //二次值单位
	double a;                      //通道系数（a）
	double b;                      //通道偏移数值（b）
	double delayTime;              //in millisecond通道时滞
	long sampleMax;                //采样最大数值
	long sampleMin;                //采样最小数值
	double primaryFactor;          //一次侧变比,通道变比一次因子
	double secondFactor;           //通道变比二次因子
	double dRatingOfSecondValue;   //二次额定值 
	char ps;                       //
	long indexOfWareData;          //录波数据的起始指针在文件中的位置
	AnalogChannel & operator=(const AnalogChannel & channel)
	{
		channelNum=channel.channelNum;
		::memcpy(channelName,channel.channelName,sizeof(channelName));
		::memcpy(channelSecondUint,channel.channelSecondUint,sizeof(channelSecondUint));
		::memcpy(channelFirstUint,channel.channelFirstUint,sizeof(channelFirstUint));
		a=channel.a;
		b=channel.b;
		delayTime=channel.delayTime;
		sampleMax=channel.sampleMax;
		sampleMin=channel.sampleMin;
		primaryFactor=channel.primaryFactor;
		secondFactor=channel.secondFactor;
		ps=channel.ps;
		indexOfWareData=channel.indexOfWareData;
		return *this;
	}
};
//模拟量子项
class CAnalogData:public CWareDataSubItem
{
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
	virtual int GetTotalTime();
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
	CAnalogData();
	virtual ~CAnalogData();

protected:
	double GetValueOfPoint(const AnalogChannel &channelData, long index);
	double GetValueOfPointOriginal(const AnalogChannel &channelData, long index);//直接获取文件保存的值，不通过微调系数计算 //modify by 2018-03-15 zys（波形文件有效值显与输入值示失真问题）
	BOOL GetAllAnalogChannelData(UINT & nPos);

protected:
	unsigned short m_nAvailStartSampdots;      //全程交流有效启动段有效值采样点数
	unsigned short m_nAvailReturnSampdots;     //全程交流有效返回段有效值采样点数
	BYTE m_nChannelCount;                     //模拟量通道数
	BYTE m_nPointCountInCircle;               //模拟量每周波采样点数 
	BYTE m_nAppointChannelCount;               //指定模拟量通道的数目
	unsigned short m_nStartSampdots;          //交流量启动段采样点数
	unsigned short m_nReturnSampdots;         //交流量返回段采样点数 
    float m_nAvailIntervalTime;      //全程交流有效段采样时间间隔
	BYTE *m_pAppointChannelNum;
	AnalogChannel * m_pAnalogChannel;          //模拟量通道信息
	BYTE m_nCircleCount;                      
	WaveInfo m_WaveInfo;
};

#endif // !defined(AFX_ANALOGDATA_H__B6E33523_13CD_4EBC_AA21_B2E7BDAEB4D2__INCLUDED_)
