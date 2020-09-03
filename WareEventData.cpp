// WareEventData.cpp: implementation of the CWareEventData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sid_2fy.h"
#include "WareFileData.h"
#include "WareEventData.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWareEventData::CWareEventData()
{
	m_nChannelCount = 0;
	m_pFilePos = NULL; 
	m_nDataTypeCode = WARE_EVENT_TYPE;
}

CWareEventData::~CWareEventData()
{
	Release();
}

void CWareEventData::Release()
{
	m_eventDescribe.clear();
	m_nChannelCount = 0;
	
	m_pFilePos = NULL; 
	m_nDataTypeCode = WARE_EVENT_TYPE;
}

CWareEventData::CWareEventData(const CWareEventData &currentData)
{
    m_eventDescribe.clear();
   	m_nDataTypeCode = WARE_EVENT_TYPE;
	CloneCopy(currentData);
}

CWareEventData& CWareEventData::operator=(const CWareEventData &currentData)
{
    CloneCopy(currentData);
	m_nDataTypeCode = WARE_EVENT_TYPE;
	return *this;
}

void CWareEventData::CloneCopy(const CWareEventData &currentData)
{
	m_nChannelCount = currentData.m_eventDescribe.size();
	m_eventDescribe.clear();
	for (UINT i = 0; i < m_nChannelCount; i++)
    {
		m_eventDescribe.push_back( currentData.m_eventDescribe[i] );
    }
	m_nDataTypeCode = WARE_EVENT_TYPE;
}


/*********************************************************************************************
��������InitSubItemData
������������ȡ¼���ļ��¼�����
������pFile:[in] ¼���ļ�ָ��  nSubSize:[in]�����С������֤��ֵ��ļ��Ƿ�Խ��
nFilePos:[in]��ǰ�������ʼ��ַ nCircleCount:[in] �ܲ���,Ĭ��ֵ0
*********************************************************************************************/
BOOL  CWareEventData::InitSubItemData( BYTE *pFile, UINT nSubSize,UINT nFilePos,WaveInfo waveInfo)
{
	ASSERT(m_pFilePos == NULL);
	if (m_nDataTypeCode != pFile[nFilePos] )//���ݴ����ֶ����ֽڣ����ֽ���ǰ��Ч
	{
			return FALSE;
	}
	m_nFilePos = nFilePos;
	m_pFilePos = pFile+nFilePos;	
	int nCurrPos = 1,nTempSubSize;//���ݴ���ռ2�ֽڣ����ֽ���Ч
	UINT nChannelCount = m_pFilePos[nCurrPos] + (m_pFilePos[nCurrPos + 1]<<8);//�¼���Ŀ
	nCurrPos += 2;
	nTempSubSize = (UINT)68*nChannelCount + 3;
	if(nTempSubSize != nSubSize)
	{
		return FALSE;
	}
    CEventDescribe event;
	for (UINT i = 0; i < nChannelCount; i++)
	{//�п�����Ӳ���������Ϣ
		memset(event.name, 0, NAME_LEN_IN_FILE + 1);
		memcpy(event.name, m_pFilePos + nCurrPos, NAME_LEN_IN_FILE);
		/*m_eventDescribe[i].name[NAME_LEN_IN_FILE] = '\0';*/
		nCurrPos += NAME_LEN_IN_FILE;
		int sizeRemark = 28;
		int sizetime = 8;
		if (!CFrame::GetDateTimeFromBuf(m_pFilePos + nCurrPos,event.eventTime,event.nMillisecond))
		{
			event.isVail = false;
		}else
		{
			event.isVail = true;
	   }

		nCurrPos += sizetime;
		//¼������//1:Ϊ�����¼� 2:Ϊ¼���¼�
		event.type = m_pFilePos[nCurrPos]; //¼������
		memset(event.m_strRemark, 0, sizeRemark + 1);
		memcpy(event.m_strRemark, m_pFilePos + nCurrPos+1, sizeRemark);
		nCurrPos=nCurrPos+sizeRemark;
		m_eventDescribe.push_back( event );
	}
    m_nChannelCount = m_eventDescribe.size();
	return TRUE;
}

void  CWareEventData::AddEventDes( const CEventDescribe &event )
{
	m_eventDescribe.push_back( event );
	m_nChannelCount++;
}

int  CWareEventData::GetChannelCount(   ) 
{
	
	return m_eventDescribe.size();
}
DWORD CWareEventData::GetChannelData(int index) 
{
	if ( m_eventDescribe.size() <= 0 || index > m_eventDescribe.size() || index < 0)
	{
		return NULL;
	}
	CEventDescribe *pEvent = &(m_eventDescribe[index]);
	//event = m_eventDescribe[index];
	return (DWORD)pEvent;
}


