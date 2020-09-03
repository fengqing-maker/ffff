// DigitalData.cpp: implementation of the CDigitalData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SID_2FY.h"
#include "DigitalData.h"
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

CDigitalData::CDigitalData()
{
	m_pDigitalChannelName = NULL;
	m_pAppointChannelNum = NULL;
	m_nCircleCount = 0;
	m_nChannelCount = 0;
	m_nAppointChannelCount = 0;
	m_nDataTypeCode = DIGITAL_IN_TYPE;
}

CDigitalData::~CDigitalData()
{
	Release();
}

void CDigitalData::Release()
{
	this->m_pFilePos= NULL;
	m_nCircleCount = 0;
	m_nAvailInteralTime = 0;
	m_nAvailStaSampdots = 0;
	m_nAvailRetSampdots = 0;
	m_nStartSampdots = 0;
	m_nReturnSampdots = 0;
	if(m_pDigitalChannelName !=NULL)
	{
		delete [m_nChannelCount] m_pDigitalChannelName;
		m_nChannelCount = 0;
		m_pDigitalChannelName = NULL;
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
描述：虚函数，初始化开出量/开入量子项数据
参数：pFile[in]:录波文件指针,nSubSize[in]子项大小，nFilePos[in]当前子项的起始地址
返回值：初始化成功TRUE，否则False
*********************************************************************************************/
BOOL CDigitalData::InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo)
{
	ASSERT(pFile[nFilePos] == m_nDataTypeCode);
	ASSERT(m_pFilePos == NULL);
	m_WaveInfo = waveInfo;
	int pos=1;
	CString str;
	//m_nCircleCount = nCircleCount;
	m_nFilePos = nFilePos;
	m_pFilePos = pFile+m_nFilePos;
	m_nChannelCount = m_pFilePos[pos];   //开入量/开出量通道数（NDI）
	CString formatStr;
	if (m_nChannelCount<=0)
	{
		formatStr.LoadString( IDS_DIGITAL_CHNUM_ERROR );
		str.Format(formatStr,m_nChannelCount);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}
	pos++;
	m_nPointCountInCircle = m_pFilePos[pos];    //开入量/开出量每周波采样点数
	if (m_nPointCountInCircle<=0)
	{
		formatStr.LoadString( IDS_DIGIT_CYSPOINT_ERROR );
		str.Format(formatStr,m_nPointCountInCircle);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}
	pos++;
	//开入量/开出量起始段采样点数
	m_nStartSampdots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);  
	if (m_nStartSampdots<=0)
	{
		formatStr.LoadString( IDS_DIGIT_STARTPOINT_ERROR );
		str.Format(formatStr,m_nStartSampdots);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}
	pos +=2;
	//开入量/开出量返回段采样点数
	m_nReturnSampdots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	pos +=2;
	if (m_nReturnSampdots<=0)
	{
		formatStr.LoadString( IDS_DIGIT_AFTERPOINT_ERROR );
		str.Format( formatStr,m_nReturnSampdots);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}
	//全程开入量/开出量采样时间间隔
	m_nAvailInteralTime =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	pos +=2;
	if (m_nAvailInteralTime<=0)
	{
        formatStr.LoadString( IDS_DIGIT_TIMEDIFF_ERROR );
		str.Format( formatStr, m_nAvailInteralTime);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}
	//全程开入量/开出量有效起始采样点数
	m_nAvailStaSampdots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	pos +=2;
	if (m_nAvailStaSampdots<=0)
	{
		formatStr.LoadString( IDS_DIGIT_ALLSTARTPOINT_ERROR );
		str.Format(formatStr,m_nAvailStaSampdots);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}
	//全程开入量/开出量有效返回段采样点数
	m_nAvailRetSampdots =(unsigned short)m_pFilePos[pos]+(unsigned short)(m_pFilePos[pos+1]<<8);
	pos +=2;
	if (m_nAvailRetSampdots<=0)
	{
		formatStr.LoadString( IDS_DIGIT_ALLAFTERPOINT_ERROR );
		str.Format(formatStr,m_nAvailRetSampdots);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}

	m_nCircleCount = (m_nStartSampdots + m_nReturnSampdots)/m_nPointCountInCircle;

	m_nAppointChannelCount = m_pFilePos[pos];      //指定开入量/开出量通道的数目
	if (m_nAppointChannelCount > m_nChannelCount)
	{
		formatStr.LoadString( IDS_DIGIT_APPOINTNUM_ERROR );
		str.Format(formatStr,m_nAppointChannelCount);
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		return false;
	}
	ASSERT(m_nAppointChannelCount <= m_nChannelCount);
	UINT i,nTempSubSize,nCurPos,nByte;
	nByte = ((m_nChannelCount%8) ? (m_nChannelCount/8+1) : (m_nChannelCount/8));
	nTempSubSize = 36 + m_nAppointChannelCount + m_nChannelCount * 32 + 
		(m_nStartSampdots+m_nReturnSampdots+m_nAvailStaSampdots+m_nAvailRetSampdots)*nByte;
	if(nTempSubSize != nSubSize)
	{
		if(nTempSubSize + m_nCircleCount*m_nPointCountInCircle*nByte!= nSubSize)
			return FALSE;
	}
	//指定开入量/开出量通道的通道号
	m_pAppointChannelNum = new BYTE[m_nAppointChannelCount];
	m_pDigitalChannelName = new CString[m_nChannelCount];
	for(i=0;i<m_nAppointChannelCount;i++)
	{
		m_pAppointChannelNum[i]=m_pFilePos[14+i];
	}
	nCurPos = m_nAppointChannelCount + 36;
	char tempChannelName[33];
	for(i=0;i<m_nChannelCount;i++)
	{
		::memset(tempChannelName,0,sizeof(tempChannelName));
		::memcpy(tempChannelName,m_pFilePos+nCurPos,sizeof(tempChannelName)-1);
		if(m_nDataTypeCode == DIGITAL_IN_TYPE) 
		{
			m_pDigitalChannelName[i].Format(_T("%s(DI)"),(CString)tempChannelName);
		}
		else
		{
			m_pDigitalChannelName[i].Format(_T("%s(DO)"),(CString)tempChannelName);
		}
		
		nCurPos += 32;
	}
	m_nPosOfDigitalData = nCurPos;     //数字量子项总内容长度
	return TRUE;
}



int CDigitalData::GetAppointChannelIndex(int nNum)
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
int CDigitalData::GetAppointCount()
{
	if(m_pDigitalChannelName != NULL)
	{
		return m_nAppointChannelCount;
	}
	else
	{
		return -1;
	}
	
}
int CDigitalData::GetChannelCount()
{
	if(m_pDigitalChannelName != NULL)
	{
		return m_nChannelCount;
	}
	else
	{
		return -1;
	}
	
}
float CDigitalData::GetSampleInterval()
{
	if(m_pDigitalChannelName !=NULL)
	{
		return m_nAvailInteralTime;
	}
	else
	{
		return -1.0;
	}
}
int CDigitalData::GetSampleDots()
{
	if(m_pDigitalChannelName !=NULL)
	{
		return m_nAvailStaSampdots+m_nAvailRetSampdots;
	}
	else
	{
		return -1;
	}
}
float CDigitalData::GetStartCircleCount()
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
float CDigitalData::GetReturnCircleCount()
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
BOOL CDigitalData::GetChannelName(int index,CString & strChannelName)
{
	if(index>=0 && index < m_nChannelCount)
	{
		strChannelName = m_pDigitalChannelName[index];
		return TRUE;
	}
	else
	{
		strChannelName = _T("");
		return FALSE;
	}
}
int CDigitalData::GetPointCountInCircle(int index/* =0 */)
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
BOOL CDigitalData::GetCurve(Curve & curve,int nIndexOfChannel,char ps,COleDateTime startSampleTime,
							COleDateTime firstSampleTime,unsigned short nMillisecondOfStart,
							unsigned short nMillisecondOfFirst,UINT nCutCircleNum/*=0*/,UINT nCurveType/*=CURVE_DISPLAY*/)
{
	ASSERT(nCutCircleNum<m_nCircleCount);
	if(nIndexOfChannel<0 || nIndexOfChannel>= m_nChannelCount)
	{
		return FALSE;
	}
//	UINT nPointCount;
//	nPointCount = m_nAvailSampdots - 1000/FREQUENCY/m_nAvailInteralTime*nCutCircleNum; //(m_nCircleCount-nCutCircleNum) * m_nPointCountInCircle;
	curve.lpStaPoint = new Point[m_nAvailStaSampdots];
	curve.lpRetPoint = new Point[m_nAvailRetSampdots];
	curve.lpStartSecPoint = new Point[m_nStartSampdots];
	curve.lpReturnSecPoint = new Point[m_nReturnSampdots];
	if(curve.lpStaPoint == NULL)
	{
		return FALSE;
	}
	curve.nPointInCircle = m_nPointCountInCircle;
	curve.StaCount = m_nAvailStaSampdots;
	curve.RetCount = m_nAvailRetSampdots;
	curve.startCount = m_nStartSampdots;
	curve.returnCount = m_nReturnSampdots;
	curve.bIsDigital = true;
	curve.bIsOverlap = true;
	double validateValue=9.0;//开入量为1时对应的纵坐标值
	unsigned char bitOfChannel=1<<(nIndexOfChannel%8);
	int i,pos,indexOfByte=nIndexOfChannel/8,nByteOfAllBit = 
		(m_nChannelCount%8)?(m_nChannelCount/8+1):(m_nChannelCount/8);
	pos=indexOfByte+m_nPosOfDigitalData+nByteOfAllBit*nCutCircleNum*m_nPointCountInCircle/2;
	curve.name = m_pDigitalChannelName[nIndexOfChannel];
// 	CString cDebug;cDebug.Format(_T("Digital:%s"), curve.name);AfxMessageBox(cDebug);//DEBUG-TEST-ZYS
	COleDateTimeSpan span;
	curve.firstSampleTime = firstSampleTime;
	curve.milliSecondOfFirst = nMillisecondOfFirst+10*nCutCircleNum;
	if(curve.milliSecondOfFirst >= 1000)
	{
		span =COleDateTimeSpan(0,0,0,1);
		curve.firstSampleTime += span;
		curve.milliSecondOfFirst -= 1000;
	}
	span=startSampleTime-curve.firstSampleTime;
	curve.startTime=span.GetSeconds()*1000+
		(nMillisecondOfStart-curve.milliSecondOfFirst);
	for(i=0;i<m_nStartSampdots;i++)
	{
		curve.lpStartSecPoint[i].x = (double)(i+1)*((double)1000/FREQUENCY/(double)m_nPointCountInCircle);
		curve.lpStartSecPoint[i].y=(m_pFilePos[pos] & bitOfChannel)?validateValue:0;
		pos+=nByteOfAllBit;
	}
	//int returnPos = m_WaveInfo.SumCycleCount*20 - m_nReturnSampdots*1.0/m_nPointCountInCircle*20;
	//int returnPos = theApp.m_nReturnSecStart;
	for(i=0;i<m_nReturnSampdots;i++)
	{
		curve.lpReturnSecPoint[i].x = (double)i*((double)1000/FREQUENCY/(double)m_nPointCountInCircle);
		curve.lpReturnSecPoint[i].y=(m_pFilePos[pos] & bitOfChannel)?validateValue:0;
		pos+=nByteOfAllBit;
	}
	for(i=0;i<m_nAvailStaSampdots;i++)
	{
		curve.lpStaPoint[i].x = (double)i*m_nAvailInteralTime;
		curve.lpStaPoint[i].y=(m_pFilePos[pos] & bitOfChannel)?validateValue:0;
		pos+=nByteOfAllBit;
	}
// 	returnPos = m_WaveInfo.SumCycleCount*20 - m_WaveInfo.AvaRetCycleCount*20;
	for(i=0;i<m_nAvailRetSampdots;i++)
	{
		curve.lpRetPoint[i].x = (double)(i+m_nAvailStaSampdots)*m_nAvailInteralTime;
		curve.lpRetPoint[i].y=(m_pFilePos[pos] & bitOfChannel)?validateValue:0;
		pos+=nByteOfAllBit;
	}
	curve.valueOfCellY = 1.0;
	return TRUE;
}

float CDigitalData::GetCircleCount()
{
	if(m_nCircleCount)
	{
		return (int)m_nCircleCount;
	}
	else
	{
		return -1;
	}
}
//若无此采样值，返回SAMPLE_INVAILED_VALUE，否则返回采样值
//注意：此采样值如是模拟量，则低字节在前，如是开出量，则是0，1
//如果采样点数不足nMaxSampCount将采用线性插值，对于开入量的线性插值，
//是中点或以上用后一个采样点的值
short CDigitalData::GetSampValue(int indexOfChannel,int indexOfPoint,int nMaxSampCount)
{
	int nPointCount;
	nPointCount = m_nCircleCount * m_nPointCountInCircle;//m_nAvailSampdots;
	if(indexOfChannel <0 || indexOfChannel >= m_nChannelCount || 
		indexOfPoint <0 || indexOfPoint >= nMaxSampCount)
	{
		return (short)SAMPLE_INVAILED_VALUE;
	}
	unsigned char bitOfChannel=1<<(indexOfChannel%8);
	int pos,indexOfByte=indexOfChannel/8,nByteOfAllBit = 
		(m_nChannelCount%8)?(m_nChannelCount/8+1):(m_nChannelCount/8);
	pos=indexOfByte+m_nPosOfDigitalData;// + nByteOfAllBit*m_nCircleCount * m_nPointCountInCircle;
	if(nPointCount == nMaxSampCount)
	{
		pos+=indexOfPoint*nByteOfAllBit;
	}
	else
	{
		int i1;
		double dIndex;
		dIndex = indexOfPoint*nPointCount/nMaxSampCount;
		i1=dIndex;
		if(DoubleCmp(dIndex-i1-0.5f) >=0)
		{
			i1++;
		}
		pos += i1*nByteOfAllBit;
	}
	short u2=(m_pFilePos[pos] & bitOfChannel)?1:0;
	return u2;
}
