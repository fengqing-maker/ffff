// FrequencyAndAngle.cpp: implementation of the CFrequencyAndAngle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SID_2FY.h"
#include "FrequencyAndAngle.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMySID_2FYApp theApp;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFrequencyAndAngle::CFrequencyAndAngle()
{
	
	m_pFreAngChannel = NULL;
	m_nChannelCount = 0;
	m_nSamInterval = 0;
	m_nStaSamDots = 0;
	m_nRetSamDots = 0;
	m_nDataTypeCode = FREQUENCY_TYPE;
}

CFrequencyAndAngle::~CFrequencyAndAngle()
{
	Release();
}
void CFrequencyAndAngle::Release()
{
	m_pFilePos = NULL;
	m_nSamInterval = 0;
	m_nStaSamDots = 0;
	m_nRetSamDots = 0;
	if(m_pFreAngChannel !=NULL)
	{
		delete [m_nChannelCount] m_pFreAngChannel;
		m_nChannelCount = 0;
		m_pFreAngChannel = NULL;
	}
}
/*********************************************************************************************
函数名：InitSubItemData
描述：虚函数，初始化频率子项/角差子项数据
参数：pFile[in]:录波文件指针,nSubSize[in]子项大小，nFilePos[in]当前子项的起始地址
返回值：初始化成功TRUE，否则False
*********************************************************************************************/
BOOL CFrequencyAndAngle::InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo/* =0 */)
{
	ASSERT(pFile[nFilePos] == m_nDataTypeCode);
	ASSERT(m_pFilePos == NULL);
	m_WaveInfo = waveInfo;
	int pos=1;
//	m_nCircleCount = nCircleCount;
	m_nFilePos = nFilePos;
	m_pFilePos = pFile+m_nFilePos;
	m_nChannelCount = m_pFilePos[pos];    //频率或角差通道数（NFR）
	pos++;
	CString str;
	if (m_nChannelCount< 0)
	{
		str.LoadString( IDS_ANGFRE_CHNUM_ERROR );
		CdMessageBox box( theApp.m_pMainWnd, str);
	 	box.MsgBoxEx();
		return FALSE;
	}
	//全程频率采样间隔
	m_nSamInterval =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	
	if (m_nSamInterval<=0)
	{
	    str.LoadString( IDS_ANGFRE_SMP_ERROR );
		CdMessageBox box( theApp.m_pMainWnd, str);
	 	box.MsgBoxEx();
		return FALSE;
	}
	pos += 2;
	
	//全程起始段频率采样点数
	m_nStaSamDots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	
	if (m_nStaSamDots<=0)
	{
		str.LoadString( IDS_ANGFRE_SAMP_POINT );
		CdMessageBox box( theApp.m_pMainWnd, str);
	 	box.MsgBoxEx();
		return FALSE;
	}
	pos += 2;
	//全程返回段频率采样点数
	m_nRetSamDots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	
	if (m_nRetSamDots<=0)
	{
		str.LoadString( IDS_ANGFRE_SAMP_POINT );
		CdMessageBox box( theApp.m_pMainWnd, str);
	 	box.MsgBoxEx();
		return FALSE;
	}
	pos += 2;
	UINT nTempSubSize,nCurPos;
	nTempSubSize = (UINT)8 + (UINT)m_nChannelCount * (86+(m_nStaSamDots+m_nRetSamDots)*2);
	if(nTempSubSize != nSubSize)
	{
		//if(nTempSubSize+(UINT)m_nChannelCount * (UINT)m_nPointCountInCircle*m_nCircleCount*2!= nSubSize)
			return FALSE;
	}
	/*m_pAppointChannelNum = new BYTE[m_nAppointChannelCount];*/
	m_pFreAngChannel = new FreAngChannel[m_nChannelCount];
// 	for(i=0;i<m_nAppointChannelCount;i++)
// 	{
// 		m_pAppointChannelNum[i]=m_pFilePos[4+i];
// 	}
	nCurPos = 8;
	return GetAllFreqChannelData(nCurPos);
}

BOOL CFrequencyAndAngle::GetAllFreqChannelData(UINT & nPos)
{
	UINT i,tempPos;
	double dCurTimeDelay=0;
	for (i=0;i<m_nChannelCount;i++)
	{
		tempPos = nPos;
		//初始化通道信息
		memset((void *)&m_pFreAngChannel[i],0,sizeof(m_pFreAngChannel[i]));
		//获取通道内容
		m_pFreAngChannel[i].channelNum=m_pFilePos[nPos++];
		memcpy(m_pFreAngChannel[i].channelName,m_pFilePos+nPos,32);
		nPos+=32;
		memcpy(m_pFreAngChannel[i].channelFirstUint,m_pFilePos+nPos,8);
		nPos+=8;
		short u2=*((short *)(m_pFilePos+nPos));
		long u4;
		nPos+=2;
		m_pFreAngChannel[i].a=(double)u2/ADJUST_NUM;
		u2=*((short *)(m_pFilePos+nPos));		
		nPos+=2;
		m_pFreAngChannel[i].b=(double)u2/ADJUST_NUM;
		m_pFreAngChannel[i].delayTime=m_pFilePos[nPos++];
		m_pFreAngChannel[i].delayTime/=1000;
		u2=*((short *)(m_pFilePos+nPos));
		nPos+=2;
		m_pFreAngChannel[i].sampleMax=u2;
		u2 = *((short *)(m_pFilePos+nPos));
		nPos+=2;
		m_pFreAngChannel[i].sampleMin=u2;
		u4=*((long *)(m_pFilePos+nPos));
		nPos+=4;
		m_pFreAngChannel[i].dRatingOfSecondValue = (double)u4/SECOND_RAITING_ADJUST_NUM;
		nPos += 32;     //功能扩展预留
		ASSERT((nPos - tempPos) == 86);
		m_pFreAngChannel[i].indexOfWareData=nPos;
		dCurTimeDelay+=m_pFreAngChannel[i].delayTime;
		m_pFreAngChannel[i].delayTime=dCurTimeDelay-m_pFreAngChannel[i].delayTime;
		nPos += (UINT)(m_nStaSamDots+m_nRetSamDots)*2;
	}
	return TRUE;
}

int CFrequencyAndAngle::GetChannelCount()
{
	if(m_pFreAngChannel !=NULL)
	{
		return m_nChannelCount;
	}
	else
	{
		return -1;
	}
	
}
float CFrequencyAndAngle::GetSampleInterval()
{
	if(m_pFreAngChannel !=NULL)
	{
		return m_nSamInterval;
	}
	else
	{
		return -1.0;
	}
}
int CFrequencyAndAngle::GetSampleDots()
{
	if(m_pFreAngChannel !=NULL)
	{
		return m_nStaSamDots+m_nRetSamDots;
	}
	else
	{
		return 0;
	}
}
float CFrequencyAndAngle::GetStartCircleCount()
{
	return 0;
}
float CFrequencyAndAngle::GetCircleCount()
{
	return   ((m_nStaSamDots + m_nRetSamDots)*m_nSamInterval*1.0)/20.0; ////1111111111
//	return 0;
}
float CFrequencyAndAngle::GetReturnCircleCount()
{
	return 0;
}
BOOL CFrequencyAndAngle::GetChannelName(int index,CString & strChannelName)
{
	if(index>=0 && index < m_nChannelCount)
	{
		strChannelName = m_pFreAngChannel[index].channelName;
		return TRUE;
	}
	else
	{
		strChannelName = _T("");
		return FALSE;
	}
}
BOOL CFrequencyAndAngle::GetCurve(Curve & curve,int nIndexOfChannel,char ps,COleDateTime startSampleTime,
						   COleDateTime firstSampleTime,unsigned short nMillisecondOfStart,
						   unsigned short nMillisecondOfFirst,UINT nCutCircleNum/*=0*/,
						   UINT nCurveType/*=CURVE_DISPLAY*/)
{
	if(nIndexOfChannel<0 || nIndexOfChannel >= m_nChannelCount)
	{
		return FALSE;
	}
//	UINT nPointCount;
//	nPointCount = m_nAllSamDots;//(m_nCircleCount-nCutCircleNum) * m_nPointCountInCircle;
//	curve.nPointInCircle = m_nPointCountInCircle;
	curve.lpStaPoint = new Point[m_nStaSamDots];
	curve.lpRetPoint = new Point[m_nRetSamDots];
	if(curve.lpStaPoint == NULL)
	{
		return FALSE;
	}
	curve.StaCount = m_nStaSamDots;
	curve.RetCount = m_nRetSamDots;
//	double samplePeriod=(double)1000/FREQUENCY/m_nPointCountInCircle;
	COleDateTimeSpan span;
	curve.firstSampleTime = firstSampleTime;
	curve.milliSecondOfFirst = nMillisecondOfFirst+(double)10*nCutCircleNum+
		m_pFreAngChannel[nIndexOfChannel].delayTime;
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
	curve.name= (CString)m_pFreAngChannel[nIndexOfChannel].channelName;
//	CString sDegug;sDegug.Format(_T("Frequence:%s"), curve.name);AfxMessageBox(sDegug);//DEBUG-TEST-ZYS
	curve.unit = (CString)m_pFreAngChannel[nIndexOfChannel].channelFirstUint;
	curve.startTime=startTime;
	
	curve.fmaxOfY=max(fabs(m_pFreAngChannel[nIndexOfChannel].sampleMax),
		fabs(m_pFreAngChannel[nIndexOfChannel].sampleMin));
	
	
	double d1;

	wchar_t cUint=curve.unit[curve.unit.GetLength()-1];

	curve.minRange=10;
	int j=0,nCutIndex = 1000/FREQUENCY/m_nSamInterval*nCutCircleNum/2;
	d1 = curve.dTurnAngle * (1000/FREQUENCY/m_nSamInterval)/360;
	int oldCutIndex = nCutIndex;
	nCutIndex = d1+0.5+nCutIndex;      //用平移得到转角
	
//	int returnStart =  (m_WaveInfo.SumCycleCount - m_WaveInfo.AvaRetCycleCount)*20; //111111111
	switch(nCurveType)
	{
	case CURVE_DISPLAY:
		curve.lpStaPoint[0].x=
				m_pFreAngChannel[nIndexOfChannel].delayTime; //2016-9-20 有效值为10ms计算一个点并且是算10ms后的数值，因此第0个点的数值=第一个点的数值
		if (cUint == 'z')
		{//是频率通道
			curve.lpStaPoint[0].y= 
				GetValueOfPoint(m_pFreAngChannel[nIndexOfChannel], nCutIndex)/100; //-1 2016-9-20 add 
		}
		else
		{//是主控插件相角差
			curve.lpStaPoint[j].y= 
				GetValueOfPoint(m_pFreAngChannel[nIndexOfChannel], nCutIndex)/10; //-1 2016-9-20 add 
		}
		for(j=1;j<m_nStaSamDots;j++)
		{
			curve.lpStaPoint[j].x=
				m_pFreAngChannel[nIndexOfChannel].delayTime+m_nSamInterval*j;
			
			if (cUint == 'z')
			{//是频率通道
				curve.lpStaPoint[j].y= 
					GetValueOfPoint(m_pFreAngChannel[nIndexOfChannel],j+nCutIndex-1)/100; //-1 2016-9-20 add 
			}
			else
			{//是主控插件相角差
				curve.lpStaPoint[j].y= 
					GetValueOfPoint(m_pFreAngChannel[nIndexOfChannel],j+nCutIndex-1)/10; //-1 2016-9-20 add 
			}
		}

		curve.lpRetPoint[0].x=
			m_pFreAngChannel[nIndexOfChannel].delayTime + m_nSamInterval*(j); //2016-9-20 有效值为10ms计算一个点并且是算10ms后的数值，因此第0个点的数值=第一个点的数值
		if (cUint == 'z')
		{//是频率通道
			curve.lpRetPoint[0].y= 
				GetValueOfPoint(m_pFreAngChannel[nIndexOfChannel], j+nCutIndex-1)/100; //-1 2016-9-20 add 
		}
		else
		{//是主控插件相角差
			curve.lpRetPoint[0].y= 
				GetValueOfPoint(m_pFreAngChannel[nIndexOfChannel], j+nCutIndex-1)/10; //-1 2016-9-20 add 
		}
		for(j=1;j<m_nRetSamDots;j++)
		{
			curve.lpRetPoint[j].x=
				m_pFreAngChannel[nIndexOfChannel].delayTime+m_nSamInterval*(j+m_nStaSamDots);
			
			if (cUint == 'z')
			{
				curve.lpRetPoint[j].y= 
					GetValueOfPoint(m_pFreAngChannel[nIndexOfChannel],j+m_nStaSamDots+nCutIndex-1)/100;
			}
			else
			{
				curve.lpRetPoint[j].y= 
					GetValueOfPoint(m_pFreAngChannel[nIndexOfChannel],j+m_nStaSamDots+nCutIndex-1)/10;
			}
		}
		break;
	default:
		break;
	}
	if (curve.unit == _T("Hz") || curve.unit == _T("HZ"))
	{
		theApp.m_freq[theApp.m_freqIndex++] = curve.lpStaPoint[0].y;
		theApp.m_freq[theApp.m_freqIndex++] = curve.lpRetPoint[0].y;
	}
	return TRUE;
}
double CFrequencyAndAngle::GetValueOfPoint(const FreAngChannel &channelData, long index)
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
int CFrequencyAndAngle::GetPointCountInCircle(int index/* =0 */)
{
	return 0;
}

DWORD CFrequencyAndAngle::GetChannelData(int index)
{
	if(index >= 0 && index <m_nChannelCount)
	{
		return (DWORD)&m_pFreAngChannel[index];
	}
	else
	{
		return NULL;
	}
}
//若无此采样值，返回SAMPLE_INVAILED_VALUE，否则返回采样值
short CFrequencyAndAngle::GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount)
{
	int nPointCount;
	nPointCount = m_nStaSamDots+m_nRetSamDots;
	if(indexOfChannel <0 || indexOfChannel >= m_nChannelCount || 
		indexOfPoint <0 || indexOfPoint >= nMaxSampCount)
	{
		return (short)SAMPLE_INVAILED_VALUE;
	}
	unsigned char * wareData=m_pFilePos+m_pFreAngChannel[indexOfChannel].indexOfWareData;
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