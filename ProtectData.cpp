// ProtectData.cpp: implementation of the CProtectData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SID_2FY.h"
#include "ProtectData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProtectData::CProtectData()
{
	m_pFilePos = NULL;
	m_pProPara = NULL;
	m_nParaCount = 0;
	m_nParaAreaNum = 0xFF; 
	m_nDataTypeCode = PROTECT_PARA_TYPE;
}

CProtectData::~CProtectData()
{
	Release();
}
BOOL CProtectData::InitSubItemData(BYTE *pFile, UINT nSubSize,
								   UINT nFilePos,WaveInfo waveInfo)
{	
	ASSERT(m_pFilePos == NULL);
	m_nFilePos = nFilePos;
	m_pFilePos = pFile+nFilePos;
	ASSERT(m_pFilePos[0] == m_nDataTypeCode);
	m_nParaAreaNum = m_pFilePos[1];         //定值区号
	m_nParaCount = *((unsigned short *)(m_pFilePos+2));  //定值数目（NBHDZ）
	UINT i,nTemp,nTempSubSize,nCurPos;
	nTempSubSize = (UINT)72*m_nParaCount + 4;
	if(nTempSubSize != nSubSize)
	{
		return FALSE;
	}
	if(m_pProPara != NULL)
	{
		return FALSE;
	}
	m_pProPara = new ProtectPara[m_nParaCount];
	if(m_pProPara == NULL)
	{
		m_nParaCount = 0;
		return FALSE;
	}
	nCurPos = 4;
	for(i=0;i<m_nParaCount;i++)
	{
		nTemp = sizeof(m_pProPara[i].strParaType)-1;
		memcpy(m_pProPara[i].strParaType,m_pFilePos+nCurPos,nTemp);
		nCurPos+=nTemp;
		nTemp = sizeof(m_pProPara[i].strParaName)-1;
		memcpy(m_pProPara[i].strParaName,m_pFilePos+nCurPos,nTemp);
		nCurPos+=nTemp;
		nTemp = sizeof(m_pProPara[i].strParaValue)-1;
		memcpy(m_pProPara[i].strParaValue,m_pFilePos+nCurPos,nTemp);
		nCurPos+=nTemp;
	}
	return TRUE;
}

void CProtectData::Release()
{
	m_pFilePos = NULL;
	m_nParaAreaNum = 0xFF;
	if(m_pProPara !=NULL)
	{
		delete [m_nParaCount] m_pProPara;
		m_nParaCount = 0;
		m_pProPara = NULL;
	}
}
//这里是获取保护定值的定值区
int CProtectData::GetAppointCount()
{
	if(m_pProPara != NULL)
	{
		return (int)m_nParaAreaNum;
	}
	else
	{
		return -1;
	}
}
int CProtectData::GetChannelCount()
{
	if(m_pProPara != NULL)
	{
		return m_nParaCount;
	}
	else
	{
		return -1;
	}
}
DWORD CProtectData::GetChannelData(int index)
{
	if(index >= 0 && index <m_nParaCount)
	{
		return (DWORD)&m_pProPara[index];
	}
	else
	{
		return NULL;
	}
}