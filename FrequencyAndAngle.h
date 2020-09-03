// FrequencyAndAngle.h: interface for the CFrequencyAndAngle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREQUENCYANDANGLE_H__8AE8F729_0B65_43C2_8E24_F3533E09A72E__INCLUDED_)
#define AFX_FREQUENCYANDANGLE_H__8AE8F729_0B65_43C2_8E24_F3533E09A72E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WareDataSubItem.h"
//Ƶ��ͨ������
struct FreAngChannel
{
	long channelNum;            //ͨ����
	char channelName[33];       //ͨ������
	char channelFirstUint[9];   //��λ
	double a;                   //ͨ��ϵ����a��
	double b;                   //ͨ��ƫ����ֵ��b��
	double delayTime;           //in millisecondͨ��ʱ��
	long sampleMax;             //���������ֵ
	long sampleMin;             //������С��ֵ
	double dRatingOfSecondValue;   //���ζֵ
	long indexOfWareData;         //¼�����ݵ���ʼָ�����ļ��е�λ��
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
	//���޴˲���ֵ������SAMPLE_INVAILED_VALUE�����򷵻ز���ֵ
	//ע�⣺�˲���ֵ����ģ����������ֽ���ǰ�����ǿ�����������0��1
	//���������������nMaxSampCount���������Բ�ֵ�����ڿ����������Բ�ֵ��
	//���е�������ú�һ���������ֵ
	virtual short GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount);
	//��ȡָ��ͨ�����ݵ��ڴ��ָ��
	virtual DWORD GetChannelData(int index);
	//��ȡ��ǰ����Ĳ����ܲ������������-1��ʾ����ʧ��
	virtual float GetCircleCount();
	virtual float GetStartCircleCount();
	virtual float GetReturnCircleCount();
	//pFile ¼���ļ�ָ�룬nSubSize�����С��nCircleCount�ܲ�����nFilePos��ǰ�������ʼ��ַ
	virtual BOOL InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo);
	//��ȡ�����е�index��ͨ�������֣�����ɹ����ط���ֵ
	virtual BOOL GetChannelName(int index,CString & strChannelName);
	//��ȡ��ǰ�����ͨ�������������-1��ʾ����ʧ��
	virtual int GetChannelCount();
	//��ȡ��ǰ�����ȫ�̶β���ʱ�������������-1��ʾ����ʧ��
	virtual float GetSampleInterval();
	//��ȡ��ǰ�����ȫ�̶β����������������-1��ʾ����ʧ��
	virtual int GetSampleDots();
	//��ȡ��ǰ�����ָ��ͨ�������������-1��ʾ����ʧ��
	virtual int GetPointCountInCircle(int index=0);
	//��ȡ���ߵ�������Ϣ
	virtual BOOL GetCurve(Curve & curve,int nIndexOfChannel,char ps,
		COleDateTime startSampleTime,COleDateTime firstSampleTime,unsigned short nMillisecondOfStart,
		unsigned short nMillisecondOfFirst,UINT nCutCircleNum=0,UINT nCurveType=CURVE_DISPLAY);
	virtual void Release();
	
protected:
	double GetValueOfPoint(const FreAngChannel &channelData, long index);
	BOOL GetAllFreqChannelData(UINT & nPos);
	BYTE m_nChannelCount;                       //Ƶ�ʻ�ǲ�ͨ������NFR��
	FreAngChannel* m_pFreAngChannel;
	unsigned short m_nStaSamDots;               //ȫ����ʼ��Ƶ�ʲ�������
	unsigned short m_nRetSamDots;               //ȫ�̷��ض�Ƶ�ʲ������� 
	float m_nSamInterval;           	//ȫ��Ƶ�ʲ������
	WaveInfo m_WaveInfo;
};

#endif // !defined(AFX_FREQUENCYANDANGLE_H__8AE8F729_0B65_43C2_8E24_F3533E09A72E__INCLUDED_)
