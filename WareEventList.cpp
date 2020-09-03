// WareEventList.cpp : implementation file
//

#include "stdafx.h"
#include "WareEventList.h"
#include "MainFrm.h"
#include "WareEventData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWareEventList

CWareEventList::CWareEventList()
{
}

CWareEventList::~CWareEventList()
{
}


BEGIN_MESSAGE_MAP(CWareEventList, CListCtrl)
	//{{AFX_MSG_MAP(CWareEventList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWareEventList message handlers
void CWareEventList::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	
	*pResult = 0;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CView *pView = pMainFrm->GetActiveView();
	if (pView->IsKindOf(RUNTIME_CLASS(CCurveView))  )
	{
		/*��ǰ����¼����ͼ*/
		CEventDescribe CurEvent;
		/*��ȡ˫�����¼���Ϣ*/
		GetEventInfo(pNMItemActivate->iItem,CurEvent);
		/*�����¼����*/
		const CWareFileData *pWareData = ((CMainFrame *)AfxGetApp()->m_pMainWnd)->GetWaveFileDataPtr();
		//�����¼���������
		int nSubItem = pWareData->GetSubItemArrayCount();
		CWareDataSubItem *pSub = NULL;
		int i=0;
		for (;i<nSubItem;i++)
		{
		    pSub = pWareData->m_arrPSubItem[i];
			if (WARE_EVENT_TYPE == pSub->GetDataType())
			{
				break;
			}
		}
		if (pSub == NULL)
		{
			//AfxMessageBox(_T("�¼��������."));
			return;
		}
		//�¼�����
		int nEvent = pSub->GetChannelCount();
		CEventDescribe *pevent;
		i = 0;
		for (;i<nEvent ;i++)
		{
			pevent = (CEventDescribe *)pSub->GetChannelData(i);
			if ( NULL != pevent && strcmp(pevent->name,CurEvent.name) == 0
				&& pevent->nMillisecond == CurEvent.nMillisecond
				&& pevent->eventTime == CurEvent.eventTime)
			{
				/*�ҵ����������¼�����ָ��*/
				static_cast<CCurveView*>(pView)->ScrollByTime(pevent->eventTime, pevent->nMillisecond);
				return;
			}
		}
		CString Mstick;
	}
}
void CWareEventList::GetEventInfo(const int nItem,CEventDescribe &Event)
{
	CString strEventName = GetItemText(nItem,0);
	int len = strEventName.GetLength();
	WideCharToMultiByte(CP_ACP, 0, strEventName.GetBuffer( len ), strEventName.GetLength(),Event.name,NAME_LEN_IN_FILE + 1 , NULL, NULL );
	//memcpy(Event.name,strEventName.GetBuffer(),NAME_LEN_IN_FILE + 1);
	CString strEvnetType = GetItemText(nItem,3);
	if (strEvnetType == _T("¼���¼�"))
	{
		Event.type = LB_EVENT_TYPE;
	} 
	else if(strEvnetType == _T("�����¼�"))
	{
		Event.type = KZ_EVENT_TYPE;
	}
	else
	{
		Event.type = SVERROR_EVENT_TYPE;
	}
	CString EventTime = GetItemText(nItem,1);
	///*�ֽ��¼�ʱ��*/
	int nYear =0,nMonth = 0,nDay = 0, nHour = 0,nMin = 0,nSec = 0;
	len = EventTime.GetLength();
	swscanf(EventTime.GetBuffer( len ),_T("%d-%d-%d %d:%d:%d.%d"),&nYear,&nMonth,&nDay,&nHour,&nMin,&nSec,&Event.nMillisecond);
	Event.eventTime.SetDateTime(nYear,nMonth,nDay,nHour,nMin,nSec);
}
