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
#define FREQUENCY_TYPE 0XC0		//频率子项
#define ANGLE_DIFF_TYPE 0XC1	//角差子项
#define WARE_EVENT_TYPE 0X05    //录波事件子项

#define SECOND_MIN_RANGE 10    
#define ADJUST_NUM 1000    //调整系数，由于下位机没有浮点数，所以提取上来的数除以它得到浮点数
#define SECOND_RAITING_ADJUST_NUM 100   //二次额定值的调整系数
#define FREQUENCY 50    //频率为50Hz

// #define FREQUENCY 100    //频率为50Hz DEGBU-TEST-ZYS

#define CIRCLE_TIME 20
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CurveView.h"

class CWareFileData  
{
public:
	CString m_strFileName;
	int m_nSubItemCount;                 //子项数目
	CWareDataSubItem ** m_arrPSubItem;  //录波子项数据
	UINT m_nCurveCount;
	Curve * m_pCurve;    //波文件信息
	void Release();

	BOOL GetComtradeFile(CString strFileFolder);
	BOOL GetCurve(int curveIndex,int subItemIndex,int index,char ps,UINT nCutCircleNum=0,UINT nCurveType = CURVE_DISPLAY);
	/*返回值：
	*OPEN_WARE_FILE_SUCESS:                  打开并解析录波文件成功！
	*OPEN_WARE_FILE_NOT_EXIST                录波文件不存在
	*OPEN_WARE_FILE_OPEN_FAILED              打开录波文件失败，可能别的程序正在使用它
	*OPEN_WARE_FILE_FILE_LENGTH_TOO_LONG     录波文件太长
	*OPEN_WARE_FILE_GET_INFO_FIALED          解析录波文件失败
	*/
	int OpenFile(CString strFilePath);
    //录波启动前录波多少周波,乘20采样值启动前时间
	float m_nBeforeStartCycleCount;
    //采样返回段录波多少周波,乘20采样值返回后时间
	float m_nAferReturnCycleCount;
	//有效值启动段周波数，乘以20有效值启动段时间
	float m_nAvailStartCycleCount;
	//有效值返回段周波数，乘以20有效值返回段时间
	float m_nAvailReturnCycleCount;
	CWareFileData & operator=(const CWareFileData &fileData);
/*	Curve & GetDigitalCurve(Curve & curve,int indexOfDigitalChannel,int indexOfShow,int showCount);*/
	//获取模拟量通道channelData的第index点处的采样值，注意这里是a*x+b值
	double GetValueOfPoint(const AnalogChannel & channelData,long index);
	//ps表示要获取的是p/s值
	//解析录波文件
	BOOL InitWareFileData();
	int  GetSubItemArrayCount() const;	
	CPtrArray        m_SubItemPtrArray;//录波子项数据
	float m_nCycleCount; //采样总周波数，乘以20为总时间
	unsigned short m_nCopyCode;  //版本号
	BOOL m_isUnitedFile;          //否为波形合并后的文件，如果是，则标记值为1,否则为0  
	int m_diffMilliSecond;

	unsigned char m_fileData[MAX_WARE_FILE_LENGTH];   //文件最长可达1m字节
	UINT m_nFileLength;                               //文件长度
	char m_strStartType[33];                          //录波启动类型
	char m_strFileHead[17];                           //文件头
	unsigned short m_milliSecondOfFirst;              //
	COleDateTime m_FirstSampleTime;                   //第一个周波开始时间？？
	unsigned short m_milliSecondOfStart;              //录波启动毫秒数
	COleDateTime m_StartSampleTime;                   //录波启动时间
	unsigned short m_milliSecondOfReturn;             //动作返回时间毫秒数
	COleDateTime m_ReturnSampleTime;                  //动作返回时间
	BOOL         m_fileOk;                            //2014-8-18文件是否为固化ok的
	CWareFileData();
	virtual ~CWareFileData();

protected:
	//生成comtrade文件中的.inf文件
	BOOL GetInfoFile(CString strFilePath);
	//生成comtrade文件中的数据文件（.dat文件）
	BOOL GetDataFile(CString strFilePath,int nAnalogCount,int nDigitalCount,int nMaxSampPointCount);
	//生成comtrade文件中的配制文件（.cfg）
	BOOL GetComfigFile(CString strFilePath,int nAnalogCount,int nDigitalCount,
		int & nMaxSampPointCount);
};

#endif // !defined(AFX_WAREFILEDATA_H__0B94AA67_1998_4993_8219_CFB4D5FDD0CA__INCLUDED_)
