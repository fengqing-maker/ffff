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

//ģ����ͨ����Ϣ
struct AnalogChannel
{
	long channelNum;               //ͨ����
	char channelName[33];          //ͨ������
	char channelSecondUint[9];     //һ��ֵ��λ
	char channelFirstUint[9];      //����ֵ��λ
	double a;                      //ͨ��ϵ����a��
	double b;                      //ͨ��ƫ����ֵ��b��
	double delayTime;              //in millisecondͨ��ʱ��
	long sampleMax;                //���������ֵ
	long sampleMin;                //������С��ֵ
	double primaryFactor;          //һ�β���,ͨ�����һ������
	double secondFactor;           //ͨ����ȶ�������
	double dRatingOfSecondValue;   //���ζֵ 
	char ps;                       //
	long indexOfWareData;          //¼�����ݵ���ʼָ�����ļ��е�λ��
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
//ģ��������
class CAnalogData:public CWareDataSubItem
{
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
	virtual int GetTotalTime();
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
	virtual int GetAppointCount();
	//��ȡ��nNum��ָ��ͨ����ͨ�����,�������-1��ʾ����ʧ��
	virtual int GetAppointChannelIndex(int nNum);
	//��ȡindexͨ����ÿ��ͨ��������,�������-1��ʾ����ʧ��
	virtual int GetPointCountInCircle(int index=0);
	//��ȡ���ߵ�������Ϣ
	virtual BOOL GetCurve(Curve & curve,int nIndexOfChannel,char ps,
		COleDateTime startSampleTime,COleDateTime firstSampleTime,unsigned short nMillisecondOfStart,
		unsigned short nMillisecondOfFirst,UINT nCutCircleNum=0,UINT nCurveType=CURVE_DISPLAY);
	virtual void Release();
	CAnalogData();
	virtual ~CAnalogData();

protected:
	double GetValueOfPoint(const AnalogChannel &channelData, long index);
	double GetValueOfPointOriginal(const AnalogChannel &channelData, long index);//ֱ�ӻ�ȡ�ļ������ֵ����ͨ��΢��ϵ������ //modify by 2018-03-15 zys�������ļ���Чֵ��������ֵʾʧ�����⣩
	BOOL GetAllAnalogChannelData(UINT & nPos);

protected:
	unsigned short m_nAvailStartSampdots;      //ȫ�̽�����Ч��������Чֵ��������
	unsigned short m_nAvailReturnSampdots;     //ȫ�̽�����Ч���ض���Чֵ��������
	BYTE m_nChannelCount;                     //ģ����ͨ����
	BYTE m_nPointCountInCircle;               //ģ����ÿ�ܲ��������� 
	BYTE m_nAppointChannelCount;               //ָ��ģ����ͨ������Ŀ
	unsigned short m_nStartSampdots;          //�����������β�������
	unsigned short m_nReturnSampdots;         //���������ضβ������� 
    float m_nAvailIntervalTime;      //ȫ�̽�����Ч�β���ʱ����
	BYTE *m_pAppointChannelNum;
	AnalogChannel * m_pAnalogChannel;          //ģ����ͨ����Ϣ
	BYTE m_nCircleCount;                      
	WaveInfo m_WaveInfo;
};

#endif // !defined(AFX_ANALOGDATA_H__B6E33523_13CD_4EBC_AA21_B2E7BDAEB4D2__INCLUDED_)
