// WareFileData.h: interface for the CWareFileData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAREFILEDATA_H__0B94AA67_1998_4993_8219_CFB4D5FDD0CA__INCLUDED_)
#define AFX_WAREFILEDATA_H__0B94AA67_1998_4993_8219_CFB4D5FDD0CA__INCLUDED_
#include "AnalogData.h"	// Added by ClassView
#include "DigitalData.h"	// Added by ClassView
#include "ProtectData.h"	// Added by ClassView
#include "FrequencyAndAngle.h"	// Added by ClassView
#include "Frame.h"
#define MAX_ANALOG_CHANNEL_COUNT 24
#define MAX_DIGITAL_CHANNEL_COUNT 24

#define OPEN_WARE_FILE_SUCESS                0X00
#define OPEN_WARE_FILE_NOT_EXIST             0X01
#define OPEN_WARE_FILE_OPEN_FAILED           0X02
#define OPEN_WARE_FILE_FILE_LENGTH_TOO_LONG  0X03
#define OPEN_WARE_FILE_GET_INFO_FIALED       0X04

#define ANALOG_TYPE 0XAB
#define DIGITAL_IN_TYPE 0XDE
#define DIGITAL_OUT_TYPE 0XDF
#define PROTECT_PARA_TYPE 0X01
#define FREQUENCY_TYPE 0XC0		//Ƶ������
#define ANGLE_DIFF_TYPE 0XC1	//�ǲ�����
#define WARE_EVENT_TYPE 0X05    //¼���¼�����

#define SECOND_MIN_RANGE 10    
#define ADJUST_NUM 1000    //����ϵ����������λ��û�и�������������ȡ���������������õ�������
#define SECOND_RAITING_ADJUST_NUM 100   //���ζֵ�ĵ���ϵ��
#define FREQUENCY 50    //Ƶ��Ϊ50Hz

// #define FREQUENCY 100    //Ƶ��Ϊ50Hz DEGBU-TEST-ZYS

#define CIRCLE_TIME 20
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CurveView.h"

class CWareFileData  
{
public:
	CString m_strFileName;
	int m_nSubItemCount;                 //������Ŀ
	CWareDataSubItem ** m_arrPSubItem;  //¼����������
	UINT m_nCurveCount;
	Curve * m_pCurve;    //���ļ���Ϣ
	void Release();

	BOOL GetComtradeFile(CString strFileFolder);
	BOOL GetCurve(int curveIndex,int subItemIndex,int index,char ps,UINT nCutCircleNum=0,UINT nCurveType = CURVE_DISPLAY);
	/*����ֵ��
	*OPEN_WARE_FILE_SUCESS:                  �򿪲�����¼���ļ��ɹ���
	*OPEN_WARE_FILE_NOT_EXIST                ¼���ļ�������
	*OPEN_WARE_FILE_OPEN_FAILED              ��¼���ļ�ʧ�ܣ����ܱ�ĳ�������ʹ����
	*OPEN_WARE_FILE_FILE_LENGTH_TOO_LONG     ¼���ļ�̫��
	*OPEN_WARE_FILE_GET_INFO_FIALED          ����¼���ļ�ʧ��
	*/
	int OpenFile(CString strFilePath);
    //¼������ǰ¼�������ܲ�,��20����ֵ����ǰʱ��
	float m_nBeforeStartCycleCount;
    //�������ض�¼�������ܲ�,��20����ֵ���غ�ʱ��
	float m_nAferReturnCycleCount;
	//��Чֵ�������ܲ���������20��Чֵ������ʱ��
	float m_nAvailStartCycleCount;
	//��Чֵ���ض��ܲ���������20��Чֵ���ض�ʱ��
	float m_nAvailReturnCycleCount;
	CWareFileData & operator=(const CWareFileData &fileData);
/*	Curve & GetDigitalCurve(Curve & curve,int indexOfDigitalChannel,int indexOfShow,int showCount);*/
	//��ȡģ����ͨ��channelData�ĵ�index�㴦�Ĳ���ֵ��ע��������a*x+bֵ
	double GetValueOfPoint(const AnalogChannel & channelData,long index);
	//ps��ʾҪ��ȡ����p/sֵ
	//����¼���ļ�
	BOOL InitWareFileData();
	int  GetSubItemArrayCount() const;	
	CPtrArray        m_SubItemPtrArray;//¼����������
	float m_nCycleCount; //�������ܲ���������20Ϊ��ʱ��
	unsigned short m_nCopyCode;  //�汾��
	BOOL m_isUnitedFile;          //��Ϊ���κϲ�����ļ�������ǣ�����ֵΪ1,����Ϊ0  
	int m_diffMilliSecond;

	unsigned char m_fileData[MAX_WARE_FILE_LENGTH];   //�ļ���ɴ�1m�ֽ�
	UINT m_nFileLength;                               //�ļ�����
	char m_strStartType[33];                          //¼����������
	char m_strFileHead[17];                           //�ļ�ͷ
	unsigned short m_milliSecondOfFirst;              //
	COleDateTime m_FirstSampleTime;                   //��һ���ܲ���ʼʱ�䣿��
	unsigned short m_milliSecondOfStart;              //¼������������
	COleDateTime m_StartSampleTime;                   //¼������ʱ��
	unsigned short m_milliSecondOfReturn;             //��������ʱ�������
	COleDateTime m_ReturnSampleTime;                  //��������ʱ��
	BOOL         m_fileOk;                            //2014-8-18�ļ��Ƿ�Ϊ�̻�ok��
	CWareFileData();
	virtual ~CWareFileData();

protected:
	//����comtrade�ļ��е�.inf�ļ�
	BOOL GetInfoFile(CString strFilePath);
	//����comtrade�ļ��е������ļ���.dat�ļ���
	BOOL GetDataFile(CString strFilePath,int nAnalogCount,int nDigitalCount,int nMaxSampPointCount);
	//����comtrade�ļ��е������ļ���.cfg��
	BOOL GetComfigFile(CString strFilePath,int nAnalogCount,int nDigitalCount,
		int & nMaxSampPointCount);
};

#endif // !defined(AFX_WAREFILEDATA_H__0B94AA67_1998_4993_8219_CFB4D5FDD0CA__INCLUDED_)
