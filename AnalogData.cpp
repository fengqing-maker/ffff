// AnalogData.cpp: implementation of the CAnalogData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SID_2FY.h"
#include "AnalogData.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern CMySID_2FYApp theApp;

CAnalogData::CAnalogData()
{
	m_pAnalogChannel = NULL;
	m_pAppointChannelNum = NULL;
	m_nChannelCount = 0;
	m_nCircleCount = 0;
	m_nAppointChannelCount = 0;
	m_nDataTypeCode = ANALOG_TYPE;
}

CAnalogData::~CAnalogData()
{
	Release();
}

void CAnalogData::Release()
{
	m_pFilePos = NULL;
	m_nCircleCount = 0;
	m_nAvailIntervalTime = 0;
	m_nAvailStartSampdots = 0;
	m_nAvailReturnSampdots = 0;
	m_nStartSampdots = 0;
	m_nReturnSampdots = 0;
	if(m_pAnalogChannel !=NULL)
	{
		delete [m_nChannelCount] m_pAnalogChannel;
		m_nChannelCount = 0;
		m_pAnalogChannel = NULL;
	}
	if(m_pAppointChannelNum != NULL)
	{
		delete [m_nAppointChannelCount] m_pAppointChannelNum;
		m_nAppointChannelCount = 0;
		m_pAppointChannelNum = NULL;
	}
}
/*********************************************************************************************
函数名：InitSubItemData
描述：虚函数，初始化模拟量子项数据
参数：pFile[in]:录波文件指针,nSubSize[in]子项大小，nCircleCount[in]周波数，
      nFilePos[in]当前子项的起始地址
返回值：初始化成功TRUE，否则False
*********************************************************************************************/
BOOL CAnalogData::InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo)
{
	ASSERT(pFile[nFilePos] == m_nDataTypeCode);        
	ASSERT(m_pFilePos == NULL);
	m_WaveInfo = waveInfo;
	int pos=1;
	//m_nCircleCount = nCircleCount;
	CString str;
	m_nFilePos = nFilePos;
	m_pFilePos = pFile+m_nFilePos;
	m_nChannelCount = m_pFilePos[pos];   //模拟量通道数
	pos++;
	if (m_nChannelCount<=0)
	{
		return false;
	}
	m_nPointCountInCircle = m_pFilePos[pos];   //模拟量每周波采样点数
	pos++;
	CString formatStr;
	if (m_nPointCountInCircle<=0)             
	{
		formatStr.LoadString( IDS_ANG_CYSPOINT_ERROR );
		str.Format( formatStr, m_nPointCountInCircle);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}
	m_nStartSampdots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);   //交流量启动段采样点数
	pos +=2;
	if (m_nStartSampdots<=0)
	{
		formatStr.LoadString( IDS_ANG_STARTPOINT_ERROR );
		str.Format(formatStr, m_nStartSampdots);
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		return false;
	}
	//交流量返回段采样点数
	m_nReturnSampdots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	pos +=2;
	if (m_nReturnSampdots<=0)
	{
		formatStr.LoadString( IDS_ANG_AFTERPOINT_ERROR );
		str.Format(formatStr, m_nReturnSampdots);
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		return false;
	} 
	//全程交流有效段采样时间间隔
	m_nAvailIntervalTime =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	pos +=2;
	if (m_nAvailIntervalTime<=0||m_nAvailIntervalTime>1000)
	{	
		formatStr.LoadString( IDS_ANG_TIMEDIFF_ERROR );
		str.Format(formatStr, m_nAvailIntervalTime);
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		return false;
	}
	//全程交流有效启动段有效值采样点数
	m_nAvailStartSampdots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	pos +=2;
	if (m_nAvailStartSampdots<=0)
	{
	    formatStr.LoadString( IDS_ANG_ALLSTARTPOINT_ERROR );
		str.Format(formatStr, m_nAvailStartSampdots);
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		return false;
	}
    //全程交流有效返回段有效值采样点数
	m_nAvailReturnSampdots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	pos +=2;
	if (m_nAvailReturnSampdots<=0)
	{
		formatStr.LoadString( IDS_ANG_ALLAFTERPOINT_ERROR );
		str.Format(formatStr, m_nAvailReturnSampdots);
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		return false;
	}

	m_nCircleCount = (m_nStartSampdots + m_nReturnSampdots)/m_nPointCountInCircle;
	m_nAppointChannelCount = m_pFilePos[pos];      //指定模拟量通道的数目
	pos++;
	ASSERT(m_nAppointChannelCount <= 2*m_nChannelCount);
	if (m_nAppointChannelCount > 2*m_nChannelCount)
	{	
		formatStr.LoadString( IDS_ANG_APPOINTNUM_ERROR );
		str.Format(formatStr, m_nAppointChannelCount);
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		return false;
	}

	UINT i,nTempSubSize,nCurPos;
	nTempSubSize = (UINT)36 + m_nAppointChannelCount+ (UINT)m_nChannelCount * 
		(102+(m_nStartSampdots+m_nReturnSampdots+m_nAvailStartSampdots+m_nAvailReturnSampdots)*2);
	if(nTempSubSize != nSubSize)
	{
        formatStr.LoadString( IDS_ANG_SUBLEN_ERROR );
		str.Format(formatStr, nSubSize, nTempSubSize);
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		//if(nTempSubSize+(UINT)m_nChannelCount * (UINT)m_nPointCountInCircle*m_nCircleCount*2!= nSubSize)
			return FALSE;
	}
	//指定模拟量通道号
	m_pAppointChannelNum = new BYTE[m_nAppointChannelCount];
	m_pAnalogChannel = new AnalogChannel[m_nChannelCount];
	for(i=0;i<m_nAppointChannelCount;i++)
	{
		m_pAppointChannelNum[i]=m_pFilePos[14+i];
	}
	nCurPos = m_nAppointChannelCount + 36;
	return GetAllAnalogChannelData(nCurPos);
}
/***********************************************************************
函数名：GetAllAnalogChannelData
描述：获取模拟通道（0、1...）的数据
参数：开始位置nPos
返回值：
***********************************************************************/
BOOL CAnalogData::GetAllAnalogChannelData(UINT & nPos)
{
	UINT i,tempPos;
	double dCurTimeDelay=0;
	CString str;
	for (i=0;i<m_nChannelCount;i++)
	{
		tempPos = nPos;
		//初始化通道信息
		memset((void *)&m_pAnalogChannel[i],0,sizeof(m_pAnalogChannel[i]));
		//获取通道内容
		m_pAnalogChannel[i].channelNum=m_pFilePos[nPos++];
		if (m_pAnalogChannel[i].channelNum != i)
		{
			CString formatStr;
			formatStr.LoadString( IDS_ANG_CHNUM_ERROR );
			str.Format( formatStr,i,m_pAnalogChannel[i].channelNum);
			CdMessageBox box( theApp.m_pMainWnd, str);
		    box.MsgBoxEx();
		}
		memcpy(m_pAnalogChannel[i].channelName,m_pFilePos+nPos,32);
		nPos+=32;
		memcpy(m_pAnalogChannel[i].channelFirstUint,m_pFilePos+nPos,8);
		nPos+=8;
		memcpy(m_pAnalogChannel[i].channelSecondUint,m_pFilePos+nPos,8);
		nPos+=8;
		short u2=*((short *)(m_pFilePos+nPos));
		long u4;
		nPos+=2;
		m_pAnalogChannel[i].a=(double)u2/ADJUST_NUM;
		u2=*((short *)(m_pFilePos+nPos));		
		nPos+=2;
		m_pAnalogChannel[i].b=(double)u2/ADJUST_NUM;
		m_pAnalogChannel[i].delayTime=m_pFilePos[nPos++];
		m_pAnalogChannel[i].delayTime/=1000;
		u2=*((short *)(m_pFilePos+nPos));
		nPos+=2;
		m_pAnalogChannel[i].sampleMax=u2;
		u2 = *((short *)(m_pFilePos+nPos));
		nPos+=2;
		m_pAnalogChannel[i].sampleMin=u2;
		u4=*((long *)(m_pFilePos+nPos));
		nPos+=4;
		m_pAnalogChannel[i].primaryFactor=(double)u4/ADJUST_NUM;
		u4=*((long *)(m_pFilePos+nPos));
		nPos+=4;
		m_pAnalogChannel[i].secondFactor=(double)u4/ADJUST_NUM;
		u4=*((long *)(m_pFilePos+nPos));
		nPos+=4;
		m_pAnalogChannel[i].dRatingOfSecondValue = (double)u4/SECOND_RAITING_ADJUST_NUM;
		nPos += 32;     //功能扩展预留
		ASSERT((nPos - tempPos) == 102);
		m_pAnalogChannel[i].ps='s';
		m_pAnalogChannel[i].indexOfWareData=nPos;
		dCurTimeDelay+=m_pAnalogChannel[i].delayTime;
		m_pAnalogChannel[i].delayTime=dCurTimeDelay-m_pAnalogChannel[i].delayTime;
		nPos += (UINT)(m_nStartSampdots+m_nReturnSampdots+m_nAvailStartSampdots+m_nAvailReturnSampdots)*2;
	}
	return TRUE;
}

int CAnalogData::GetAppointChannelIndex(int nNum)
{
	if(nNum >= 0 && nNum < m_nAppointChannelCount)
	{
		return m_pAppointChannelNum[nNum];
	}
	else
	{
		return -1;
	}
}
int CAnalogData::GetAppointCount()
{
	if(m_pAnalogChannel !=NULL)
	{
		return m_nAppointChannelCount;
	}
	else
	{
		return -1;
	}
	
}
int CAnalogData::GetChannelCount()
{
	if(m_pAnalogChannel !=NULL)
	{
		return m_nChannelCount;
	}
	else
	{
		return -1;
	}
	
}
float CAnalogData::GetSampleInterval()
{
	if(m_pAnalogChannel !=NULL)
	{
		return m_nAvailIntervalTime;
	}
	else
	{
		return -1.0;
	}
}
int CAnalogData::GetSampleDots()
{
	if(m_pAnalogChannel !=NULL)
	{
		return m_nAvailReturnSampdots+m_nAvailStartSampdots;
	}
	else
	{
		return -1;
	}
}
BOOL CAnalogData::GetChannelName(int index,CString & strChannelName)
{
	if(index>=0 && index < m_nChannelCount)
	{
		strChannelName = m_pAnalogChannel[index].channelName;
		return TRUE;
	}
	else
	{
		strChannelName = _T("");
		return FALSE;
	}
}
BOOL CAnalogData::GetCurve(Curve & curve,int nIndexOfChannel,char ps,COleDateTime startSampleTime,
						   COleDateTime firstSampleTime,unsigned short nMillisecondOfStart,
						   unsigned short nMillisecondOfFirst,UINT nCutCircleNum/*=0*/,
						   UINT nCurveType/*=CURVE_DISPLAY*/)
{
	if(nIndexOfChannel<0 || nIndexOfChannel >= m_nChannelCount)
	{
		return FALSE;
	}
	if(!(ps=='p'||ps=='P'||ps=='s'||ps=='S'))
	{
		return FALSE;
	}
//	UINT nPointCount;
	UINT variFactor = AVAIL_FACTOR_U;
	///nPointCount = m_nAvailSampdots - 1000/FREQUENCY/m_nAvailIntervalTime*nCutCircleNum; //(m_nCircleCount-nCutCircleNum) * m_nPointCountInCircle;
	curve.nPointInCircle = m_nPointCountInCircle;
	curve.lpStaPoint = new Point[m_nAvailStartSampdots];
	curve.lpRetPoint = new Point[m_nAvailReturnSampdots];
	curve.lpStartSecPoint = new Point[m_nStartSampdots];
	curve.lpReturnSecPoint = new Point[m_nReturnSampdots];
	if(curve.lpStaPoint == NULL)
	{
		return FALSE;
	}
	curve.StaCount = m_nAvailStartSampdots;
	curve.RetCount = m_nAvailReturnSampdots;
	curve.startCount = m_nStartSampdots;
	curve.returnCount = m_nReturnSampdots;
	curve.bIsOverlap = true;
	double samplePeriod=(double)1000/FREQUENCY/m_nPointCountInCircle;		//每个采样点的时间
	COleDateTimeSpan span;
	curve.firstSampleTime = firstSampleTime;
	curve.milliSecondOfFirst = nMillisecondOfFirst+(double)10*nCutCircleNum+m_pAnalogChannel[nIndexOfChannel].delayTime;
	if(curve.milliSecondOfFirst >= 1000)
	{
		span =COleDateTimeSpan(0,0,0,1);
		curve.firstSampleTime += span;
		curve.milliSecondOfFirst -= 1000;
	}
	span=startSampleTime-curve.firstSampleTime;
	double startTime;
	startTime=span.GetSeconds()*1000+
		(nMillisecondOfStart-curve.milliSecondOfFirst);
	curve.name= (CString)m_pAnalogChannel[nIndexOfChannel].channelName;
	
	curve.startTime=startTime;
	
	curve.fmaxOfY=max(fabs(m_pAnalogChannel[nIndexOfChannel].sampleMax),
		fabs(m_pAnalogChannel[nIndexOfChannel].sampleMin));
	
	
	double mulit,d1;

	switch (ps)
	{
	case 'P':
	case 'p':
		mulit = m_pAnalogChannel[nIndexOfChannel].primaryFactor;
		curve.unit = (CString)m_pAnalogChannel[nIndexOfChannel].channelFirstUint;
		break;
	case 'S':
	case 's':
		mulit = m_pAnalogChannel[nIndexOfChannel].secondFactor;
		curve.unit=(CString)m_pAnalogChannel[nIndexOfChannel].channelSecondUint;
		break;
	}
	wchar_t cUint=curve.unit[curve.unit.GetLength()-1];
	if(cUint == TEXT('A') || cUint == TEXT('a'))
	{
		variFactor = AVAIL_FACTOR_I;
		curve.minRange=1;
		if(ps=='p' || ps=='P')
		{
			//表示m_pAnalogChannel[nIndexOfChannel].secondFactor不为零
			if(fabs(m_pAnalogChannel[nIndexOfChannel].secondFactor) > 1e-8)
			{
				curve.minRange=1*m_pAnalogChannel[nIndexOfChannel].primaryFactor/m_pAnalogChannel[nIndexOfChannel].secondFactor;
			}
		}
	}
	else
	{
		curve.minRange=10;
		if(ps=='p' || ps=='P')
		{
			//表示m_pAnalogChannel[nIndexOfChannel].secondFactor不为零
			if(fabs(m_pAnalogChannel[nIndexOfChannel].secondFactor) > 1e-8)
			{
				curve.minRange=10*
					m_pAnalogChannel[nIndexOfChannel].primaryFactor/
					m_pAnalogChannel[nIndexOfChannel].secondFactor;
			}
		}
	}
	
	int j,nCutIndex = m_nPointCountInCircle*nCutCircleNum/2;
	d1 = curve.dTurnAngle * m_nPointCountInCircle/360;
	int oldCutIndex = nCutIndex;
	nCutIndex = d1+0.5+nCutIndex;      //用平移得到转角
	int nPos=0;
	
	//int returnStart =  (m_WaveInfo.SumCycleCount - m_nReturnSampdots*1.0/m_nPointCountInCircle)*20;
	//int returnStart = theApp.m_nReturnSecStart;
	//int returnStart2 =  (m_WaveInfo.SumCycleCount - m_WaveInfo.AvaRetCycleCount)*20;
	CString		strSamplePoint(_T("")),strTemp;
	switch(nCurveType)
	{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
		case CURVE_DISPLAY:
			for (j=0 ; j<m_nStartSampdots;j++)
			{//启动段数据
				curve.lpStartSecPoint[j].x=m_pAnalogChannel[nIndexOfChannel].delayTime+samplePeriod*j;
				curve.lpStartSecPoint[j].y=mulit*GetValueOfPointOriginal(m_pAnalogChannel[nIndexOfChannel],j+nCutIndex);
			//	curve.lpStartSecPoint[j].y=mulit*GetValueOfPoint(m_pAnalogChannel[nIndexOfChannel],j+nCutIndex);
				strTemp.Format(_T("第%d个采样点数据 ：x=%0.4f  y=%0.4f\r\n"),j,curve.lpStartSecPoint[j].x,curve.lpStartSecPoint[j].y);
				strSamplePoint += strTemp;
			}
			for (j=0 ; j<m_nReturnSampdots;j++)
			{//返回段数据
				curve.lpReturnSecPoint[j].x=m_pAnalogChannel[nIndexOfChannel].delayTime+samplePeriod*j;
				curve.lpReturnSecPoint[j].y=mulit*GetValueOfPointOriginal(m_pAnalogChannel[nIndexOfChannel],j+m_nStartSampdots+nCutIndex);
				//curve.lpReturnSecPoint[j].y=mulit*GetValueOfPoint(m_pAnalogChannel[nIndexOfChannel],j+m_nStartSampdots+nCutIndex);
			}
			for(j=0;j<m_nAvailStartSampdots;j++)
			{//有效前段
				curve.lpStaPoint[j].x=m_pAnalogChannel[nIndexOfChannel].delayTime+m_nAvailIntervalTime*j;
				//curve.lpStaPoint[j].y=GetValueOfPoint(m_pAnalogChannel[nIndexOfChannel],j+m_nStartSampdots+m_nReturnSampdots+nCutIndex)/variFactor;
					//直接获取文件保存的值，不通过微调系数计算 //modify by 2018-03-15 zys（波形文件有效值显与输入值示失真问题）
				curve.lpStaPoint[j].y=GetValueOfPointOriginal(m_pAnalogChannel[nIndexOfChannel], j+m_nStartSampdots+m_nReturnSampdots+nCutIndex)/variFactor;
			}
			for(j=0;j<m_nAvailReturnSampdots;j++)
			{
				curve.lpRetPoint[j].x=m_pAnalogChannel[nIndexOfChannel].delayTime+m_nAvailIntervalTime*(j+m_nAvailStartSampdots);
				//curve.lpRetPoint[j].y=GetValueOfPoint(m_pAnalogChannel[nIndexOfChannel],j+m_nStartSampdots+m_nReturnSampdots+m_nAvailStartSampdots+nCutIndex)/variFactor;
				//直接获取文件保存的值，不通过微调系数计算 //modify by 2018-03-15 zys（波形文件有效值显与输入值示失真问题）
				curve.lpRetPoint[j].y = GetValueOfPointOriginal(m_pAnalogChannel[nIndexOfChannel], j+m_nStartSampdots+m_nReturnSampdots+m_nAvailStartSampdots+nCutIndex)/variFactor;
			}
			break;
		default:
			break;
	}
#ifdef OUTPUT_FILE
	char	Buff[100000] = {0};
	//把获取到的数据全部以文本形式提取
	FILE	*mFile = fopen("E:\us_ug.txt","a+");
	if (mFile == NULL)
	{
		AfxMessageBox(_T("文件打开失败"));
	}
	WideCharToMultiByte(CP_ACP,0,strSamplePoint,-1,Buff,100000,NULL,NULL);
	fwrite(Buff,strlen(Buff),1,mFile);
	fclose(mFile);
#endif

	return TRUE;
}
double CAnalogData::GetValueOfPoint(const AnalogChannel &channelData, long index)
{
	unsigned char * wareData=m_pFilePos+channelData.indexOfWareData;
	//2014-3-25 moidy 防止不同系统对于有不同的解析，文件组织时默认为先存低位，后存高位
	short u2;
	u2=wareData[index*2+1];
	u2<<=8;
	u2+=wareData[index*2];
	//short u2=*((short *)(wareData+index*2));
	double re;
	re=(double)channelData.a*u2+channelData.b;
	return re;
}

double CAnalogData::GetValueOfPointOriginal(const AnalogChannel &channelData, long index)
{
	unsigned char * wareData=m_pFilePos+channelData.indexOfWareData;
	//2014-3-25 moidy 防止不同系统对于有不同的解析，文件组织时默认为先存低位，后存高位
	short u2;
	u2=wareData[index*2+1];
	u2<<=8;
	u2+=wareData[index*2];
	//short u2=*((short *)(wareData+index*2));
	double re;
//	re=(double)channelData.a*u2+channelData.b;
//	return re;

	return (double)u2;
}

int CAnalogData::GetPointCountInCircle(int index/* =0 */)
{
	if(index>=0 && index < m_nChannelCount)
	{
		return m_nPointCountInCircle;
	}
	else
	{
		return -1;
	}
}

float CAnalogData::GetCircleCount()
{
	if(m_nCircleCount)
	{
		return m_nCircleCount;
	}
	else
	{
		return -1;
	}
}
float CAnalogData::GetStartCircleCount()
{
	
	if(m_nStartSampdots)
	{
		return m_nStartSampdots*1.0/m_nPointCountInCircle;
	}
	else
	{
		return -1;
	}
}
float CAnalogData::GetReturnCircleCount()
{
	if(m_nReturnSampdots)
	{
		return m_nReturnSampdots*1.0/m_nPointCountInCircle;
	}
	else
	{
		return -1;
	}
}
int CAnalogData::GetTotalTime()
{
	return ( m_nAvailStartSampdots + m_nAvailReturnSampdots)*m_nAvailIntervalTime; //2016-4-8 11111111
//	return m_WaveInfo.SumCycleCount*20; 
}
DWORD CAnalogData::GetChannelData(int index)
{
	if(index >= 0 && index <m_nChannelCount)
	{
		return (DWORD)&m_pAnalogChannel[index];
	}
	else
	{
		return NULL;
	}
}

/***************************************************************************
	函数名称：GetSampValue
	简要描述：若无此采样值，返回SAMPLE_INVAILED_VALUE，否则返回采样值
	     注意：此采样值如是模拟量，则低字节在前，如是开出量，则是0，1
	     如果采样点数不足nMaxSampCount将采用线性插值，对于开入量的线性插值，
         是中点或以上用后一个采样点的值
	参数：
	    indexOfChannel      [in]
		indexOfPoint        [in]
		nMaxSampCount       [in]
	返回值：
	    
	修改日志：无
****************************************************************************/
short CAnalogData::GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount)
{
	int nPointCount;
	nPointCount = m_nStartSampdots + m_nReturnSampdots;
	if(indexOfChannel <0 || indexOfChannel >= m_nChannelCount || 
		indexOfPoint <0 || indexOfPoint >= nMaxSampCount)
	{
		return (short)SAMPLE_INVAILED_VALUE;
	}
	unsigned char * wareData=m_pFilePos+m_pAnalogChannel[indexOfChannel].indexOfWareData;
	short re;
	if(nMaxSampCount == nPointCount)
	{
		//2014-3-25 moidy 不同的操作系统会导致取值错误
        re = ( wareData[indexOfPoint*2] + (((short)wareData[indexOfPoint*2+1])<<8) );
	//	re = *((short *)(wareData+indexOfPoint*2));
	}
	else
	{
		ASSERT(nPointCount<nMaxSampCount);
		double s1,s2,dIndex;
		int i1;
		dIndex = (double)indexOfPoint*nPointCount/nMaxSampCount;
		i1 = dIndex;
		if(DoubleCmp(dIndex-i1)==0)
		{
			re = *((short *)(wareData+i1*2));
		}
		else
		{
			s1 = (double)( wareData[i1*2] + (((short)wareData[i1*2+1])<<8) );
			s2 = (double)( wareData[i1*2+2] + (((short)wareData[i1*2+2+1])<<8) );
		//	s1 = (double)*((short *)(wareData+i1*2));
		//	s2 = (double)*((short *)(wareData+i1*2+2));
			double samp;
			samp = s1+(s2-s1)*(dIndex-i1);
			re = (short)samp;
		}
	}
	return re;
}