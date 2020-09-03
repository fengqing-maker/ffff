// DigitalData.h: interface for the CDigitalData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGITALDATA_H__55A4E8D6_2421_4249_B0F3_6839D0082CC5__INCLUDED_)
#define AFX_DIGITALDATA_H__55A4E8D6_2421_4249_B0F3_6839D0082CC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//����������
class CDigitalData:public CWareDataSubItem
{
public:
	//���޴˲���ֵ������SAMPLE_INVAILED_VALUE�����򷵻ز���ֵ
	//ע�⣺�˲���ֵ����ģ����������ֽ���ǰ�����ǿ�����������0��1
	//���������������nMaxSampCount���������Բ�ֵ�����ڿ����������Բ�ֵ��
	//���е�������ú�һ���������ֵ
	virtual short GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount);
	//��ȡ��ǰ����Ĳ����ܲ������������-1��ʾ����ʧ��
	virtual float GetCircleCount();
	//pFile ¼���ļ�ָ�룬nSubSize�����С��nCircleCount�ܲ�����nFilePos��ǰ�������ʼ��ַ
	virtual BOOL InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo);
	//��ȡ�����е�index��ͨ�������֣�����ɹ����ط���ֵ
	virtual BOOL GetChannelName(int index,CString & strChannelName);
	//��ȡ��ǰ�����ͨ�������������-1��ʾ����ʧ��
	virtual int GetChannelCount();
	virtual float GetStartCircleCount();
	virtual float GetReturnCircleCount();
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
	CDigitalData();
	virtual ~CDigitalData();
private:
	BYTE m_nChannelCount;                        //������/������ͨ������NDI��
	BYTE m_nPointCountInCircle;                  //������/������ÿ�ܲ���������
	unsigned short m_nStartSampdots;             //������/��������ʼ�β������� 
	unsigned short m_nReturnSampdots;        	 //������/���������ضβ�������
	unsigned short m_nAvailInteralTime;    	     //ȫ�̿�����/����������ʱ����
	unsigned short m_nAvailStaSampdots;          //ȫ�̿�����/��������Ч��ʼ��������
	unsigned short m_nAvailRetSampdots;          //ȫ�̿�����/��������Ч���ضβ�������
	BYTE m_nAppointChannelCount;                 //ָ��������/������ͨ������Ŀ   
	BYTE *m_pAppointChannelNum;                  //ָ��������/������ͨ����ͨ����
	CString *m_pDigitalChannelName;              //ָ��������/������ͨ��������
	UINT m_nPosOfDigitalData;                    //���������������ݳ���
	BYTE m_nCircleCount;                         //���ܲ���
	WaveInfo m_WaveInfo;
};

#endif // !defined(AFX_DIGITALDATA_H__55A4E8D6_2421_4249_B0F3_6839D0082CC5__INCLUDED_)
