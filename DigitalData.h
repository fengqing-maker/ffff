// DigitalData.h: interface for the CDigitalData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGITALDATA_H__55A4E8D6_2421_4249_B0F3_6839D0082CC5__INCLUDED_)
#define AFX_DIGITALDATA_H__55A4E8D6_2421_4249_B0F3_6839D0082CC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//数字量数据
class CDigitalData:public CWareDataSubItem
{
public:
	//若无此采样值，返回SAMPLE_INVAILED_VALUE，否则返回采样值
	//注意：此采样值如是模拟量，则低字节在前，如是开出量，则是0，1
	//如果采样点数不足nMaxSampCount将采用线性插值，对于开入量的线性插值，
	//是中点或以上用后一个采样点的值
	virtual short GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount);
	//获取当前子项的采样周波数，如果返回-1表示操作失败
	virtual float GetCircleCount();
	//pFile 录波文件指针，nSubSize子项大小，nCircleCount周波数，nFilePos当前子项的起始地址
	virtual BOOL InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo);
	//获取子项中第index个通道的名字，如果成功返回非零值
	virtual BOOL GetChannelName(int index,CString & strChannelName);
	//获取当前子项的通道数，如果返回-1表示操作失败
	virtual int GetChannelCount();
	virtual float GetStartCircleCount();
	virtual float GetReturnCircleCount();
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
	CDigitalData();
	virtual ~CDigitalData();
private:
	BYTE m_nChannelCount;                        //开入量/开出量通道数（NDI）
	BYTE m_nPointCountInCircle;                  //开入量/开出量每周波采样点数
	unsigned short m_nStartSampdots;             //开入量/开出量起始段采样点数 
	unsigned short m_nReturnSampdots;        	 //开入量/开出量返回段采样点数
	unsigned short m_nAvailInteralTime;    	     //全程开入量/开出量采样时间间隔
	unsigned short m_nAvailStaSampdots;          //全程开入量/开出量有效起始采样点数
	unsigned short m_nAvailRetSampdots;          //全程开入量/开出量有效返回段采样点数
	BYTE m_nAppointChannelCount;                 //指定开入量/开出量通道的数目   
	BYTE *m_pAppointChannelNum;                  //指定开入量/开出量通道的通道号
	CString *m_pDigitalChannelName;              //指定开入量/开出量通道的名称
	UINT m_nPosOfDigitalData;                    //数字量子项总内容长度
	BYTE m_nCircleCount;                         //总周波数
	WaveInfo m_WaveInfo;
};

#endif // !defined(AFX_DIGITALDATA_H__55A4E8D6_2421_4249_B0F3_6839D0082CC5__INCLUDED_)
