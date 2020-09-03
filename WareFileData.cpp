// WareFileData.cpp: implementation of the CWareFileData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WareFileData.h"
#include "SID_2FY.h"
#include "ProjectData.h"
#include "Frame.h"
#include "FrequencyAndAngle.h"
#include "dMessageBox.h"
#include "WareEventData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int nPointInCircle;
int nSampleDots;
int nSampleTotalTime;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern CMySID_2FYApp theApp;
CWareFileData::CWareFileData()
{
	m_arrPSubItem=NULL;
	memset(m_strFileHead,0,sizeof(m_strFileHead));
	memset(m_strStartType,0,sizeof(m_strStartType));
	m_milliSecondOfStart=0;
	m_milliSecondOfReturn=0;
	m_StartSampleTime=COleDateTime::GetCurrentTime();
	m_milliSecondOfFirst=0;
	m_FirstSampleTime=COleDateTime::GetCurrentTime();
	memset(m_fileData,0,sizeof(m_fileData));
	m_nCycleCount=0;
	m_nFileLength=MAX_WARE_FILE_LENGTH;
	m_pCurve = NULL;
	m_nCurveCount = 0;
	m_fileOk = TRUE;
}

CWareFileData::~CWareFileData()
{
    //lhz 2013-4-8 内存泄露
    Release();
}
// CWareFileData & CWareFileData::operator =(const CWareFileData & fileData)
// {
// 	int i;
// 	for (i=0;i<MAX_ANALOG_CHANNEL_COUNT;i++)
// 	{
// 		m_analogChannel[i]=fileData.m_analogChannel[i];
// 	}
// 	for (i=0;i<MAX_DIGITAL_CHANNEL_COUNT;i++)
// 	{
// 		m_digitalChannel[i]=fileData.m_digitalChannel[i];
// 	}
// 	m_currentChannelDelay=fileData.m_currentChannelDelay;
// 	memcpy(m_fileData,fileData.m_fileData,fileData.m_nFileLength);
// 	m_FirstSampleTime=fileData.m_FirstSampleTime;
// 	m_milliSecondOfFirst=fileData.m_milliSecondOfFirst;
// 	m_milliSecondOfStart=fileData.m_milliSecondOfStart;
// 	m_nChannelCount=fileData.m_nChannelCount;
// 	m_nCycleCount=fileData.m_nCycleCount;
// 	m_nDigitalCount=fileData.m_nDigitalCount;
// 	m_nDigitalPointCount=fileData.m_nDigitalPointCount;
// 	m_nDigitalPointInCircle=fileData.m_nDigitalPointInCircle;
// 	m_nFileLength=fileData.m_nFileLength;
// 	m_nIndexOfChannelData=fileData.m_nIndexOfChannelData;
// 	m_nPointCount=fileData.m_nPointCount;
// 	m_nStartOfDigitalData=fileData.m_nStartOfDigitalData;
// 	m_PointCountInCircle=fileData.m_PointCountInCircle;
// 	m_StartSampleTime=fileData.m_StartSampleTime;
// 	m_strDeviceIPAddr=fileData.m_strDeviceIPAddr;
// 	m_strFileSource=fileData.m_strFileSource;
// 	m_strStartType==fileData.m_strStartType;
// 	m_strUserCommpany=fileData.m_strUserCommpany;
// 	return *this;
// }
	BOOL CWareFileData::InitWareFileData()
{
	CString str, info;

	if(!CFrame::IsValidFile(m_fileData,m_nFileLength)) //
	{
		str.LoadString( IDS_FILE_CRC_ERROR );
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return FALSE;
	}
	if(m_nFileLength<85)
	{
        info.LoadString( IDS_FILELEN_ERROR );
		str.Format( info, m_nFileLength);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return FALSE;
	}
	unsigned char * data=m_fileData;
	UINT curPos=0,nTemp;
	memcpy(m_strFileHead,data,sizeof(m_strFileHead)-1);
	curPos+=16;

	m_nCopyCode = data[curPos];
	m_nCopyCode += data[curPos+1]<<8;
	curPos += sizeof(m_nCopyCode);
    //有返回值不判错？？？？？？？
	CFrame::GetDateTimeFromBuf(data+curPos,m_StartSampleTime,m_milliSecondOfStart);
	curPos += 8;

	CFrame::GetDateTimeFromBuf(data+curPos,m_ReturnSampleTime,m_milliSecondOfReturn);
	curPos += 8;
	COleDateTime tempTime = m_ReturnSampleTime- m_StartSampleTime;
	int tempMilli = m_milliSecondOfReturn -m_milliSecondOfStart;
	nTemp = sizeof(m_strStartType)-1;
	memcpy(m_strStartType,data+curPos,nTemp);
	curPos += nTemp;

	int totalTime = 0;
	totalTime = data[curPos+3];
	totalTime <<= 8;
	totalTime += data[curPos+2];
	totalTime <<= 8;
	totalTime += data[curPos+1];
	totalTime <<= 8;
	totalTime += data[curPos];
	m_nCycleCount = totalTime*1.0/CIRCLE_TIME;
	if (m_nCycleCount <= 0 || m_nCycleCount > 60000)
	{
		info.LoadString( IDS_WAVE_CLCYE_ERROR );
		str.Format( info,m_nCycleCount);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return FALSE;
	}
	curPos += 4;
	if (m_nCycleCount>3000)
	{
		m_diffMilliSecond = (m_nCycleCount-3000)*20;
	}
	else
	{
		m_diffMilliSecond = 0;
	}
	totalTime = data[curPos+1];
	totalTime <<= 8;
	totalTime += data[curPos];
	m_nBeforeStartCycleCount = totalTime*1.0/CIRCLE_TIME;//采样值启动段周波数
	if (m_nBeforeStartCycleCount<=0)
	{
		info.LoadString( IDS_WAVECLC_START_ERROR );
		str.Format( info,m_nBeforeStartCycleCount);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return FALSE;
	}
	curPos +=2;
	totalTime = data[curPos+1];
	totalTime <<= 8;
	totalTime += data[curPos];
	m_nAferReturnCycleCount = totalTime*1.0/CIRCLE_TIME;//采样值返回段周波数
	if (m_nAferReturnCycleCount<=0)
	{
		info.LoadString( IDS_WAVECLC_RETURN_ERROR );
		str.Format( info, m_nAferReturnCycleCount);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return FALSE;
	}
	curPos +=2;	
	totalTime = data[curPos+1];
	totalTime <<= 8;
	totalTime += data[curPos];
	m_nAvailStartCycleCount = totalTime*1.0/CIRCLE_TIME;//有效值启动段周波数
	if (m_nAvailStartCycleCount<=0)
	{
		info.LoadString( IDS_WAVECYC_ALL_ERROR );
		str.Format(info,m_nAvailStartCycleCount);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return FALSE;
	}
	curPos +=2;	
	totalTime = data[curPos+1];
	totalTime <<= 8;
	totalTime += data[curPos];
	m_nAvailReturnCycleCount = totalTime*1.0/CIRCLE_TIME;//有效值返回段周波数
	if (m_nAvailReturnCycleCount<=0)
	{
		info.LoadString( IDS_WAVECYC_ALL_ERROR2 );
		str.Format( info, m_nAvailReturnCycleCount);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return FALSE;
	}
	curPos +=2;	
	//启动时间加启动后所花的时间
	theApp.m_nReturnSecStart = tempTime.GetSecond()*1000 + tempMilli+m_nBeforeStartCycleCount *20;
	int nTimeInterval;
	nTimeInterval = m_milliSecondOfStart - m_nBeforeStartCycleCount *20;
	if(nTimeInterval >= 0)
	{
		m_FirstSampleTime = m_StartSampleTime;
		m_milliSecondOfFirst = nTimeInterval;
	}
	else
	{
		int second = (nTimeInterval%1000)?(nTimeInterval/1000-1):(nTimeInterval/1000);
		second = 0 - second;
		m_milliSecondOfFirst = second * 1000 + nTimeInterval;
		COleDateTimeSpan timeSpan(0,0,0,second);
		m_FirstSampleTime = m_StartSampleTime - timeSpan;
	}
	m_isUnitedFile=data[curPos] > 0 && data[curPos] < 0xff ? true : false;
	//lhz add by 2014-8-18
	m_fileOk = 0xff == data[curPos] ? FALSE:TRUE;
		
	curPos += 2;     //功能扩展预留
	UINT i,nSubItemLen,nSubItemPos;
	m_nSubItemCount = data[curPos];
	curPos += 1; 
	if (m_nSubItemCount>15)
	{
		return FALSE;
	}
	WaveInfo wv;
	wv.SumCycleCount = m_nCycleCount;
	wv.AvaRetCycleCount = m_nAvailReturnCycleCount;
	wv.AvaStaCycleCount = m_nAvailStartCycleCount;
	wv.SamRetCycleCount = m_nAferReturnCycleCount;
	wv.SamStaCycleCount = m_nBeforeStartCycleCount;
	wv.lbVer = m_nCopyCode;
	if(m_nSubItemCount > 0)
	{
		m_arrPSubItem = new CWareDataSubItem *[m_nSubItemCount];
		if(m_arrPSubItem == NULL)
		{
			info.LoadString( IDS_MEMORY_ERROR );
			CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
            box.MsgBoxEx();
			return FALSE;
		}
		memset(m_arrPSubItem,0,m_nSubItemCount*sizeof(CWareDataSubItem *));
	}
	theApp.m_freqIndex = 0;
	nSubItemPos = curPos + m_nSubItemCount*4;
	//先存的是所有子项木的长度，再存子项目的内容
	for(i = 0; i < m_nSubItemCount; i++)
	{
		nSubItemLen = data[curPos+3];
		nSubItemLen <<= 8;
		nSubItemLen += data[curPos+2];
		nSubItemLen <<= 8;
		nSubItemLen += data[curPos+1];
		nSubItemLen <<= 8;
		nSubItemLen += data[curPos];
		curPos += 4;
		switch(data[nSubItemPos])
		{
		case ANALOG_TYPE:     //模拟量子项
			m_arrPSubItem[i] = (CWareDataSubItem *)new CAnalogData;
			if(!m_arrPSubItem[i]->InitSubItemData(data,nSubItemLen,nSubItemPos,wv))
			{
				info.LoadString( IDS_ANALOG_ERROR );
				CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
                box.MsgBoxEx();
				m_arrPSubItem[i]->Release();
				return FALSE;
			}
			break;
		case DIGITAL_IN_TYPE:   //开入量
		case DIGITAL_OUT_TYPE:  //开出量
			m_arrPSubItem[i] = (CWareDataSubItem *)new CDigitalData;
			m_arrPSubItem[i]->SetDataType(data[nSubItemPos]);
			if(!m_arrPSubItem[i]->InitSubItemData(data,nSubItemLen,nSubItemPos,wv))
			{
				info.LoadString( IDS_DIGIT_ERROR );
				CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
                box.MsgBoxEx();
				m_arrPSubItem[i]->Release();
				return FALSE;
			}
			break;
		case PROTECT_PARA_TYPE:    //保护定值子项
			m_arrPSubItem[i] = (CWareDataSubItem *)new CProtectData;
			m_arrPSubItem[i]->SetDataType(data[nSubItemPos]);
			if(!m_arrPSubItem[i]->InitSubItemData(data,nSubItemLen,nSubItemPos,wv))
			{
				info.LoadString( IDS_PROTECTED_ERROR );
				CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
                box.MsgBoxEx();
				m_arrPSubItem[i]->Release();
				return FALSE;
			}
			break;
		case FREQUENCY_TYPE:        //频率子项
		case ANGLE_DIFF_TYPE:       //角差子项
			m_arrPSubItem[i] = (CWareDataSubItem *)new CFrequencyAndAngle;
			m_arrPSubItem[i]->SetDataType(data[nSubItemPos]);
			if(!m_arrPSubItem[i]->InitSubItemData(data,nSubItemLen,nSubItemPos,wv))
			{
				info.LoadString( IDS_ANGFRE_ERROR );
				CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
                box.MsgBoxEx();
				m_arrPSubItem[i]->Release();
				return FALSE;
			}
			m_nCycleCount = m_arrPSubItem[i]->GetCircleCount(); //2016-4-8 moidy 1111111111111
			break;
			//2016-10-20  pwq 事件子项
		case WARE_EVENT_TYPE:		
			{
				m_arrPSubItem[i] = (CWareDataSubItem *)new CWareEventData;
				m_arrPSubItem[i]->SetDataType(data[nSubItemPos]);
				if(!m_arrPSubItem[i]->InitSubItemData(data,nSubItemLen,nSubItemPos,wv))
				{
					info.LoadString(IDS_EVENTSUB_ERROR);
					//info= _T("事件子项解析失败");
					CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
					box.MsgBoxEx();
					m_arrPSubItem[i]->Release();
					return FALSE;
				}
			}
				break;
		}
		nSubItemPos += nSubItemLen;
	}
	return TRUE;
}



// Curve & CWareFileData::GetAnalogCurve(int indexOfAnalogChannel,Curve & curve,char ps)
// {
// 	ASSERT((indexOfAnalogChannel>=0) && (indexOfAnalogChannel<m_nChannelCount));
// 	ASSERT((curve.count==m_nPointCount));
// 	ASSERT(ps=='p'||ps=='P'||ps=='s'||ps=='S');
// 	double samplePeriod=(double)1000/FREQUENCY/CWareFileData::m_PointCountInCircle;
// 	COleDateTimeSpan span;
// 	span=m_StartSampleTime-m_FirstSampleTime;
// 	double startTime;
// 	startTime=span.GetSeconds()*1000+
// 		(m_milliSecondOfStart-m_milliSecondOfFirst);
// 	memset(curve.name,0,sizeof(curve.name));
// 	memset(curve.unit,0,sizeof(curve.name));
// 	memcpy(curve.name,m_analogChannel[indexOfAnalogChannel].channelName,
// 		strlen(m_analogChannel[indexOfAnalogChannel].channelName));
// // 	memcpy(curve.unit,m_analogChannel[indexOfAnalogChannel].channelSecondUint,
// // 		strlen(m_analogChannel[indexOfAnalogChannel].channelUint));
// 	curve.startTime=startTime+
// 				m_analogChannel[indexOfAnalogChannel].delayTime;
// 	curve.fmaxOfY=max(fabs(m_analogChannel[indexOfAnalogChannel].sampleMax),
// 				fabs(m_analogChannel[indexOfAnalogChannel].sampleMin));
// 	long cmpPs;
// 	cmpPs=abs(ps-m_analogChannel[indexOfAnalogChannel].ps);
// 	double mulit;
// 	if(cmpPs==0||cmpPs==0x20) mulit=1.0;
// 	else 
// 	{
// 		switch (ps)
// 		{
// 		case 'P':
// 		case 'p':
// 			mulit=m_analogChannel[indexOfAnalogChannel].primaryFactor/
// 				m_analogChannel[indexOfAnalogChannel].secondFactor;
// 			break;
// 		case 'S':
// 		case 's':
// 			mulit=m_analogChannel[indexOfAnalogChannel].secondFactor/
// 				m_analogChannel[indexOfAnalogChannel].primaryFactor;
// 			break;
// 		}
// 	}
// 	curve.minRange=10;
// 	if(ps=='p' || ps=='P')
// 	{
// 		curve.minRange=10*
// 			m_analogChannel[indexOfAnalogChannel].primaryFactor/
// 			m_analogChannel[indexOfAnalogChannel].secondFactor;
// 	}
// 	int j;
// 	for(j=0;j<m_nPointCount;j++)
// 	{
// 		curve.lpPoint[j].x=
// 			m_analogChannel[indexOfAnalogChannel].delayTime+samplePeriod*
// 			j;
// 		curve.lpPoint[j].y=mulit*
// 			GetValueOfPoint(
// 			m_analogChannel[indexOfAnalogChannel],j);
// 			}
// 	return curve;
// }

double CWareFileData::GetValueOfPoint(const AnalogChannel &channelData, long index)
{
	unsigned char * wareData=m_fileData+channelData.indexOfWareData;
	short u2;
	u2=wareData[index*2+1];
	u2<<=8;
	u2+=wareData[index*2];
	double re;
	re=(double)channelData.a*u2+channelData.b;
	return re;
}

// Curve & CWareFileData::GetDigitalCurve(Curve &curve, int indexOfDigitalChannel, int indexOfShow, int showCount)
// {
// 	ASSERT((indexOfDigitalChannel>=0) && (indexOfDigitalChannel<m_nDigitalCount));
// 	ASSERT((curve.count==m_nDigitalPointCount));
// 	double validateValue=3+(9-3)*indexOfShow/showCount;//开入量为1时对应的纵坐标值
// 	unsigned char bitOfChannel=1<<(indexOfDigitalChannel%8);
// 	int i,pos,indexOfByte=indexOfDigitalChannel/8;
// 	pos=indexOfByte+m_nStartOfDigitalData;
// 	for(i=0;i<m_nDigitalPointCount;i++)
// 	{
// 		curve.lpPoint[i].x=(double)i*((double)1000/FREQUENCY/(double)m_nDigitalPointInCircle);
// 		curve.lpPoint[i].y=(m_fileData[pos] & bitOfChannel)?validateValue:0;
// 		pos+=4;
// 	}
// 	return curve;
// }

int CWareFileData::OpenFile(CString strFilePath)
{
	CFileFind finder;
	if(!finder.FindFile(strFilePath))
	{
		return OPEN_WARE_FILE_NOT_EXIST;
	}
	FILE *wareFile = _wfopen(strFilePath,_T("rb"));
	if(wareFile == NULL)
	{
		return OPEN_WARE_FILE_OPEN_FAILED;
	}
	m_nFileLength = fread(m_fileData,sizeof(BYTE),sizeof(m_fileData),wareFile);
	fclose(wareFile);
	if(m_nFileLength == sizeof(m_fileData))
	{
		return OPEN_WARE_FILE_FILE_LENGTH_TOO_LONG;
	}
	if(!InitWareFileData())
	{
		return OPEN_WARE_FILE_GET_INFO_FIALED;
	}
	m_strFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind(_T('\\'))-1);
	return OPEN_WARE_FILE_SUCESS;
}

void CWareFileData::Release()
{
	int i;
	if(m_pCurve != NULL)
	{
		CCurveView::Release(m_pCurve,m_nCurveCount);
		m_nCurveCount = 0;
		m_pCurve = NULL;
	}
	if(m_arrPSubItem != NULL)
	{
		for(i=0;i<m_nSubItemCount;i++)
		{
			m_arrPSubItem[i]?delete m_arrPSubItem[i]:0;
		}
		delete [m_nSubItemCount] m_arrPSubItem;
		m_arrPSubItem = NULL;
		m_nSubItemCount = 0;
	}
}
BOOL CWareFileData::GetCurve(int curveIndex, int subItemIndex, int index, char ps,UINT nCutCircleNum/*=0*/,UINT nCurveType/* = CURVE_DISPLAY*/ )
{
	m_pCurve[curveIndex].nChannelIndex = index;
	m_pCurve[curveIndex].nSubItemIndex = subItemIndex;
	if(curveIndex < 0 || curveIndex >= m_nCurveCount)
	{
		return FALSE;
	}
	if(subItemIndex < 0 || subItemIndex >= m_nSubItemCount)
	{
		return FALSE;
	}
	if(m_arrPSubItem[subItemIndex]->GetDataType() != ANALOG_TYPE && (nCurveType==CURVE_ANALYSE_40A))
	{
		return FALSE;
	}
	if(nCurveType == CURVE_ANALYSE_40A && curveIndex == 1)
	{
		AnalogChannel *pAnalogChannel;
		pAnalogChannel = (AnalogChannel *)m_arrPSubItem[subItemIndex]->GetChannelData(index);
		int i,j,lineNum,count;     //进线编号
		double turnAngle;
		CString str;
		char strTemp[64] = {0} ;
		if (SUBLANG_CHINESE_SIMPLIFIED == theApp.m_pLangManger->m_strCurLanguage)
		{
			strcpy( strTemp, "线%d电压");
		}
		else if (IDS_STR_ENGLISH == theApp.m_pLangManger->m_strCurLanguage)
		{
			strcpy( strTemp, "Voltage Type of Line%s"); 
		}
		if(1 != sscanf(pAnalogChannel->channelName,strTemp,&lineNum))
		{
			m_pCurve[curveIndex].dTurnAngle=0;
			return m_arrPSubItem[subItemIndex]->GetCurve(m_pCurve[curveIndex],index,ps,m_StartSampleTime,
				m_FirstSampleTime,m_milliSecondOfStart,m_milliSecondOfFirst,nCutCircleNum,nCurveType);
		}
		if ( SUBLANG_CHINESE_SIMPLIFIED == theApp.m_pLangManger->m_strCurLanguage )
		{
			str.Format(_T("%d#进线电压类型"),lineNum);
		}else
		{
			str.Format( _T("Voltage Type of Line%d"), lineNum );
		}
			
		ProtectPara *pProtectPara;
		for(i=0;i<m_nSubItemCount;i++)
		{
			if(m_arrPSubItem[i]->GetDataType() != PROTECT_PARA_TYPE)
			{
				continue;
			}
			count = m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				pProtectPara = (ProtectPara *)m_arrPSubItem[i]->GetChannelData(j);
				if(str != pProtectPara->strParaName)
				{
					continue;
				}
				
				turnAngle = 0;
				if(0 == strcmp(pProtectPara->strParaValue,"Ub"))
				{
					turnAngle += 120;
				}
				else if(0 == strcmp(pProtectPara->strParaValue,"Uc"))
				{
					turnAngle += -120;
				}
				else if(0 == strcmp(pProtectPara->strParaValue,"Uab"))
				{
					turnAngle += -30;
				}
				else if(0 == strcmp(pProtectPara->strParaValue,"Ubc"))
				{
					turnAngle += 90;
				}
				else if(0 == strcmp(pProtectPara->strParaValue,"Uca"))
				{
					turnAngle += -150;
				}
				
				ASSERT(j+1 < count);
				pProtectPara = (ProtectPara *)m_arrPSubItem[i]->GetChannelData(j+1);
				double dAnalog;
				VERIFY(sscanf(pProtectPara->strParaValue,"%lf",&dAnalog));
				turnAngle += dAnalog;
				int i1=(int)turnAngle;
				turnAngle = i1%360+turnAngle-i1;
				if(DoubleCmp(turnAngle-180)>0)
				{
					turnAngle -= 360;
				}
				else if(DoubleCmp(180+turnAngle)<0)
				{
					turnAngle +=360;
				}
				m_pCurve[curveIndex].dTurnAngle = turnAngle;
				break;
			}
		}
	}
	
	return m_arrPSubItem[subItemIndex]->GetCurve(m_pCurve[curveIndex],index,ps,m_StartSampleTime,
		m_FirstSampleTime,m_milliSecondOfStart,m_milliSecondOfFirst,nCutCircleNum,nCurveType);
}

//用于导出Comtrade文件
BOOL CWareFileData::GetComtradeFile(CString strFileFolder)
{
	CFileFind fileFinder;
	CString str, info;
	if(fileFinder.FindFile(strFileFolder))
	{
		info.LoadString( IDS_FILE_EXIST );
		str.Format(info, strFileFolder);
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	CString strFileName,strDictory,strFilePath;
	strDictory = strFileFolder+_T("\\");
	CProjectData::ConvertPath(strDictory);
	if(!CreateDirectory(strDictory,NULL))
	{
		return FALSE;
	}
	strFileName = strFileFolder.Right(strFileFolder.GetLength() - strFileFolder.ReverseFind('\\')-1);
	strFilePath.Format(_T("%s%s.cfg"),strDictory,strFileName);
	
	int i,nAnalogCount=0,nDigitalCount=0,nMaxSampPointCount;
	for(i=0;i<m_nSubItemCount;i++)
	{
		switch (m_arrPSubItem[i]->GetDataType())
		{
		case ANALOG_TYPE:
			nAnalogCount += m_arrPSubItem[i]->GetChannelCount();
			break;
		case DIGITAL_IN_TYPE:
		case DIGITAL_OUT_TYPE:
			nDigitalCount += m_arrPSubItem[i]->GetChannelCount();
			break;
		}
	}
	if(!GetComfigFile(strFilePath,nAnalogCount,nDigitalCount,nMaxSampPointCount))
	{
		return FALSE;
	}
	strFilePath.Format(_T("%s%s.dat"),strDictory,strFileName);
	if(!GetDataFile(strFilePath,nAnalogCount,nDigitalCount,nMaxSampPointCount))
	{
		return FALSE;
	}
	strFilePath.Format(_T("%s%s.inf"),strDictory,strFileName);
	if(!GetInfoFile(strFilePath))
	{
		return FALSE;
	}
	return TRUE;
}



BOOL CWareFileData::GetComfigFile(CString strFilePath,int nAnalogCount, int nDigitalCount,
								  int & nMaxSampPointCount)
{
	FILE *file = _wfopen(strFilePath,_T("w"));
	CString info;
	if(file == NULL)
	{
		info.LoadString( IDS_CREATE_CONFIG );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
		
	}
	CString strFileData,strLineData,str;
	strFileData.Format(_T(",%s_%s,2001\r\n"),(CString)theApp.m_projectData.m_strDeviceType,(CString)m_strStartType);
	strLineData.Format(_T("%d,%dA,%dD\r\n"),nAnalogCount+nDigitalCount,nAnalogCount,nDigitalCount);
	strFileData += strLineData;
	int i,j,count,indexOfAnalog = 0,indexOfDigital=0,nMaxSampRate;
	nMaxSampRate=nMaxSampPointCount=0;
	for(i=0;i<m_nSubItemCount;i++)
	{
		if(m_arrPSubItem[i]->GetDataType() != ANALOG_TYPE) continue;
		count = m_arrPSubItem[i]->GetChannelCount();
		for(j=0;j<count;j++)
		{
			AnalogChannel *pAnalogChannel = (AnalogChannel *)m_arrPSubItem[i]->GetChannelData(j);
			ASSERT(pAnalogChannel != NULL);
			nMaxSampRate = max(nMaxSampRate,m_arrPSubItem[i]->GetPointCountInCircle(j)*50);
			nMaxSampPointCount = max(nMaxSampPointCount,m_arrPSubItem[i]->GetCircleCount()*
				m_arrPSubItem[i]->GetPointCountInCircle(j));
			m_nBeforeStartCycleCount = m_arrPSubItem[i]->GetStartCircleCount();
			m_nAferReturnCycleCount = m_arrPSubItem[i]->GetReturnCircleCount();
			nPointInCircle = m_arrPSubItem[i]->GetPointCountInCircle();
			nSampleDots = m_arrPSubItem[i]->GetSampleDots();
			nSampleTotalTime = m_arrPSubItem[i]->GetTotalTime();
			indexOfAnalog++;
			if(DoubleCmp(pAnalogChannel->secondFactor))
			{
				char cUint = pAnalogChannel->channelSecondUint[strlen(pAnalogChannel->channelSecondUint)-1];
				if(cUint == 'V' || cUint == 'v')
				{
					strLineData.Format(_T("%d,%s,,,%s,%f,%f,%f,%d,%d,%f,100.0,%c\r\n"),
						indexOfAnalog,(CString)pAnalogChannel->channelName,(CString)pAnalogChannel->channelSecondUint,
						pAnalogChannel->secondFactor*pAnalogChannel->a,
						pAnalogChannel->secondFactor*pAnalogChannel->b,pAnalogChannel->delayTime*1000,
						pAnalogChannel->sampleMin,pAnalogChannel->sampleMax,
						pAnalogChannel->primaryFactor/pAnalogChannel->secondFactor*100,pAnalogChannel->ps);
				}
				else
				{
					strLineData.Format(_T("%d,%s,,,%s,%f,%f,%f,%d,%d,%f,%f,%c\r\n"),
						indexOfAnalog,(CString)pAnalogChannel->channelName,(CString)pAnalogChannel->channelSecondUint,
						pAnalogChannel->secondFactor*pAnalogChannel->a,
						pAnalogChannel->secondFactor*pAnalogChannel->b,pAnalogChannel->delayTime*1000,
						pAnalogChannel->sampleMin,pAnalogChannel->sampleMax,
						pAnalogChannel->primaryFactor/pAnalogChannel->secondFactor*
						pAnalogChannel->dRatingOfSecondValue,pAnalogChannel->dRatingOfSecondValue,
						pAnalogChannel->ps);
				}
				
			}
			else
			{
				strLineData.Format(_T("%d,%s,,,%s,%f,%f,%f,%d,%d,1.0,1.0,%c\r\n"),
					indexOfAnalog,(CString)pAnalogChannel->channelName,(CString)pAnalogChannel->channelSecondUint,
					pAnalogChannel->secondFactor*pAnalogChannel->a,
					pAnalogChannel->secondFactor*pAnalogChannel->b,pAnalogChannel->delayTime*1000,
					pAnalogChannel->sampleMin,pAnalogChannel->sampleMax,pAnalogChannel->ps);
			}
			strFileData += strLineData;
		}
	}
	for(i=0;i<m_nSubItemCount;i++)
	{
		switch(m_arrPSubItem[i]->GetDataType())
		{
		case DIGITAL_IN_TYPE:
		case DIGITAL_OUT_TYPE:
			count = m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				nMaxSampRate = max(nMaxSampRate,m_arrPSubItem[i]->GetPointCountInCircle(j)*50);
				nMaxSampPointCount = max(nMaxSampPointCount,m_arrPSubItem[i]->GetCircleCount()*
					m_arrPSubItem[i]->GetPointCountInCircle(j));
				indexOfDigital++;
				m_arrPSubItem[i]->GetChannelName(j,str);
				strLineData.Format(_T("%d,%s,,,0\r\n"),indexOfDigital,str);
				strFileData += strLineData;
			}
			break;
		default:
			break;
		}
	}
	//电网频率
	strFileData += _T("50\r\n");
	strLineData.Format(_T("1\r\n%d,%d\r\n"),nMaxSampRate,nMaxSampPointCount);
	strFileData += strLineData;
	strLineData.Format(_T("%s.%03d000\r\n%s.%03d000\r\n"),m_FirstSampleTime.Format(_T("%d/%m/%Y,%H:%M:%S")),
		(int)m_milliSecondOfFirst,m_StartSampleTime.Format(_T("%d/%m/%Y,%H:%M:%S")),(int)m_milliSecondOfStart);
	strFileData += strLineData;
	strFileData += _T("binary\r\n");
 	// strFileData += _T("1\r\n"); 
	int lenStr = strFileData.GetLength()*2+2;
	char *pTempFileName = new char[lenStr];
	WideCharToMultiByte(CP_ACP,0,strFileData,-1,pTempFileName,lenStr,NULL,NULL);
	fwrite(pTempFileName,sizeof(char),strlen(pTempFileName)+1,file);
	fclose(file);
	delete [lenStr] pTempFileName;
	return TRUE;
}

BOOL CWareFileData::GetDataFile(CString strFilePath, int nAnalogCount, int nDigitalCount,
								int nMaxSampPointCount)
{
	const int dataSize=8 + nAnalogCount*2+((nDigitalCount%16)?(nDigitalCount/16*2+2):(nDigitalCount/16*2));
	int i,j,k,indexOfAnalog=0,indexOfDigital=0,count;
	BYTE *pSampData;
	pSampData = new BYTE[dataSize];
	int pos;
	
	CString info;
	FILE *file = _wfopen(strFilePath,_T("wb"));
	if(file == NULL)
	{
		info.LoadString( IDS_CREATEBASE_FILE );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	UINT returnStartTime  = (nSampleTotalTime - m_nAferReturnCycleCount*20)*1000;
	UINT returnStartPoint = nMaxSampPointCount - m_nAferReturnCycleCount*nPointInCircle;
	UINT rate = 20*1000/nPointInCircle ;
	for(i=0;i<nMaxSampPointCount;i++)
	{
		memset(pSampData,0,dataSize);
		UINT temp= i+1;
		memcpy(pSampData,&temp,sizeof(UINT));
		pos = sizeof(UINT);
		if (i<m_nBeforeStartCycleCount*nPointInCircle)
		{
			temp = i*rate;
		}
		else
		{
				temp =returnStartTime + ((i- returnStartPoint)*rate);
		}
		memcpy(pSampData+pos,&temp,sizeof(UINT));
		pos += sizeof(UINT);
		indexOfAnalog = indexOfDigital = 0;
		short sampValue=0,digitalSampValue=0;
		int nDigitalWordCount=0;
		for(j=0;j<m_nSubItemCount;j++)
		{
			switch(m_arrPSubItem[j]->GetDataType())
			{
			case ANALOG_TYPE:
				count = m_arrPSubItem[j]->GetChannelCount();
				for(k=0;k<count;k++)
				{
					if (i == m_nBeforeStartCycleCount*nPointInCircle || i ==m_nBeforeStartCycleCount*nPointInCircle-1)
					{
						sampValue = 0;
					}
					else
					{
						sampValue = m_arrPSubItem[j]->GetSampValue(k,i,nMaxSampPointCount);
					}
					temp = sizeof(sampValue);
					memcpy(pSampData+pos,&sampValue,temp);
					pos += temp;
				}
				break;
			case DIGITAL_IN_TYPE:
			case DIGITAL_OUT_TYPE:
				count = m_arrPSubItem[j]->GetChannelCount();
				for(k=0;k<count;k++)
				{
					if((indexOfDigital/16) > nDigitalWordCount)
					{
						memcpy(pSampData+8+nAnalogCount*2+nDigitalWordCount*2,&digitalSampValue,sizeof(digitalSampValue));
						nDigitalWordCount = indexOfDigital/16;
						digitalSampValue=0;
					}
					digitalSampValue |= m_arrPSubItem[j]->GetSampValue(k,i,nMaxSampPointCount)<< (indexOfDigital%16);
					indexOfDigital++;
				}
				break;
			default:
				break;
			} // of switch
		}// of for
		memcpy(pSampData+8+nAnalogCount*2+nDigitalWordCount*2,&digitalSampValue,sizeof(digitalSampValue));
		digitalSampValue = 0;
		fwrite(pSampData,sizeof(BYTE),dataSize,file);
	}
	delete [dataSize] pSampData;
	fclose(file);
	
	return TRUE;
}

BOOL CWareFileData::GetInfoFile(CString strFilePath)
{
	FILE *file = _wfopen(strFilePath,_T("wt"));
	CString info;
	if(file == NULL)
	{
		info.LoadString( IDS_CREATEBASE_FILE );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	CString strAnalogAppoint,strDigitalAppoint,str,strFileData;
	strAnalogAppoint = _T("Analog_Appoint_Channel=");
	strDigitalAppoint = _T("Digital_Appoint_Channel=");
	int i,j,k,countOfChannel,countOfAppoint,nChannelNum,indexOfAnalog=1,
		indexOfDigital=1,nDigitalAppoint=0,nAnalogAppoint=0;
	AnalogChannel *pAnalogChannel;
	for(i=0;i<m_nSubItemCount;i++)
	{
		switch(m_arrPSubItem[i]->GetDataType())
		{
		case ANALOG_TYPE:
			countOfChannel = m_arrPSubItem[i]->GetChannelCount();
			countOfAppoint = m_arrPSubItem[i]->GetAppointCount();
			nAnalogAppoint += countOfAppoint;
			for(j=0;j<countOfAppoint;j++)
			{
				nChannelNum = m_arrPSubItem[i]->GetAppointChannelIndex(j);
				for(k=0;k<countOfChannel;k++)
				{
					pAnalogChannel = (AnalogChannel *)m_arrPSubItem[i]->GetChannelData(k);
					ASSERT(pAnalogChannel!=NULL);
					if(nChannelNum == pAnalogChannel->channelNum)
					{
						str.Format(_T(" %d"),indexOfAnalog+k);
						strAnalogAppoint += str;
						break;
					}
				}
			}
			indexOfAnalog += countOfChannel;
			break;
		case DIGITAL_IN_TYPE:
		case DIGITAL_OUT_TYPE:
			countOfAppoint = m_arrPSubItem[i]->GetAppointCount();
			nDigitalAppoint += countOfAppoint;
			for(j=0;j<countOfAppoint;j++)
			{
				nChannelNum = m_arrPSubItem[i]->GetAppointChannelIndex(j);
				str.Format(_T(" %d"),indexOfDigital+nChannelNum);
				strDigitalAppoint += str;
				
			}
			indexOfDigital += m_arrPSubItem[i]->GetChannelCount();
			break;
		}//of switch
	}
	strFileData.Format(_T("[SZIDD Project]\r\nProject_Type %s\r\n\r\n[SZIDD Appoint_Channel]\r\nAnalog_Appoint_Count=%d\r\n%s\r\nDigital_Appoint_Count=%d\r\n%s\r\n"),
		(CString)theApp.m_projectData.m_strDeviceType,nAnalogAppoint,strAnalogAppoint,nDigitalAppoint,strDigitalAppoint);
	int lenData = strFileData.GetLength()*2+2;
	char *tempFileData = new char[lenData];
	WideCharToMultiByte(CP_ACP,0,strFileData,-1,tempFileData,lenData,NULL,NULL);
	fwrite(tempFileData,sizeof(char),strlen(tempFileData)+1,file);
	fclose(file);
	delete [] tempFileData;
	return TRUE;
	
}


int  CWareFileData::GetSubItemArrayCount() const
{
	//  int temp = m_SubItemPtrArray.GetCount();
	return m_nSubItemCount;
}




