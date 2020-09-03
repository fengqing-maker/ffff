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
#define CURVE_ANALYSE_40A 0X02   //��������40A,40B¼��������
#define SAMPLE_INVAILED_VALUE (-32768)     //��Ч����ֵ


class CWareDataSubItem  
{
public:
	//���޴˲���ֵ������SAMPLE_INVAILED_VALUE�����򷵻ز���ֵ
	//ע�⣺�˲���ֵ����ģ����������ֽ���ǰ�����ǿ�����������0��1
	//���������������nMaxSampCount���������Բ�ֵ�����ڿ����������Բ�ֵ��
	//���е�����ú�һ���������ֵ
	virtual short GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount);
	//��ȡ��ǰ����Ĳ����ܲ������������-1��ʾ����ʧ��
	virtual float GetCircleCount();
	//��ȡ��ǰ�������Ч��ʼ�εĲ����ܲ���
	virtual float GetStartCircleCount();
	//��ȡ��ǰ�������Ч���ضεĲ����ܲ���
	virtual float GetReturnCircleCount();
	virtual int GetTotalTime();
	//��ȡָ��ͨ�����ݵ��ڴ��ָ��
	virtual DWORD GetChannelData(int index);
	BYTE GetDataType();
	BOOL SetDataType(BYTE nTypeCode);
	CWareDataSubItem();
	virtual ~CWareDataSubItem();
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
protected:
	UINT m_nSubItemLength;
	UINT m_nFilePos;
	BYTE m_nDataTypeCode;    //��������
	BYTE * m_pFilePos;

	// pwq 2016-10-20
	UINT m_nChannelCount;	//ͨ����
};

#endif // !defined(AFX_WAREDATASUBITEM_H__627A15BE_D457_468B_8493_418BD646695C__INCLUDED_)
