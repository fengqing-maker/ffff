// WareDataSubItem.cpp: implementation of the CWareDataSubItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SID_2FY.h"
#include "WareDataSubItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWareDataSubItem::CWareDataSubItem()
{
	m_pFilePos = NULL;
	m_nSubItemLength = 0;
	m_nFilePos = 0;
	m_nDataTypeCode = 0;
}

CWareDataSubItem::~CWareDataSubItem()
{
	Release();
}
int CWareDataSubItem::GetAppointChannelIndex(int nNum)
{
	return -1;
}
int CWareDataSubItem::GetAppointCount()
{
	return -1;
}
int CWareDataSubItem::GetChannelCount()
{
	return -1;
}
float CWareDataSubItem::GetSampleInterval()
{
	return -1.0;
}
int CWareDataSubItem::GetSampleDots()
{
	return -1;
}
BOOL CWareDataSubItem::GetChannelName(int index,CString & strChannelName)
{
	return FALSE;
}
BOOL CWareDataSubItem::GetCurve(Curve & curve,int nIndexOfChannel,char ps,
					  COleDateTime startSampleTime,COleDateTime firstSampleTime,unsigned short nMillisecondOfStart,
		unsigned short nMillisecondOfFirst,UINT nCutCircleNum/*=0*/,UINT nCurveType/*=CURVE_DISPLAY*/)
{
	return FALSE;
}
int CWareDataSubItem::GetPointCountInCircle(int index/* =0 */)
{
	return FALSE;
}
BOOL CWareDataSubItem::InitSubItemData(BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo)
{
	return FALSE;
}
void CWareDataSubItem::Release()
{
	m_pFilePos = NULL;
	m_nSubItemLength = 0;
	m_nFilePos = 0;
	m_nDataTypeCode = 0;
}

BYTE CWareDataSubItem::GetDataType()
{
	return m_nDataTypeCode;
}

BOOL CWareDataSubItem::SetDataType(BYTE nTypeCode)
{
	if(nTypeCode == DIGITAL_IN_TYPE || nTypeCode == DIGITAL_OUT_TYPE)
	{
		m_nDataTypeCode = nTypeCode;
		Release();
		return TRUE;
	}
	if(nTypeCode == FREQUENCY_TYPE || nTypeCode == ANGLE_DIFF_TYPE)
	{
		m_nDataTypeCode = nTypeCode;
		Release();
		return TRUE;
	}
	return FALSE;
}


DWORD CWareDataSubItem::GetChannelData(int index)
{

	return 0;
}

float CWareDataSubItem::GetCircleCount()
{
	return -1;
}

float CWareDataSubItem::GetStartCircleCount()
{
	return -1;
}
float CWareDataSubItem::GetReturnCircleCount()
{
	return -1;
}
short CWareDataSubItem::GetSampValue(int indexOfChannel, int indexOfPoint,int nMaxSampCount)
{
	return (short)SAMPLE_INVAILED_VALUE;
}

int CWareDataSubItem::GetTotalTime()
{
	return -1;
}

