// RealDataView.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "RealDataView.h"
#include "Frame.h"
#include "MainFrm.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CRealDataView::m_nDataType = 0;
//CEvent realEv;
CRealDataView* rView;
extern CMySID_2FYApp theApp;
extern LANGID SetThreadUILanguage(LANGID lcid);
DWORD WINAPI FreshRealThread(LPVOID lpParameter)
{
	//CRealDataView* rView=(CRealDataView*)lpParameter;
	DWORD LangId = GetUserDefaultLangID();
    if (IDS_STR_ENGLISH == theApp.m_pLangManger->m_strCurLanguage && MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) != LangId )
    {
		SetThreadUILanguage( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );  //win 7 下用
		//	SetThreadLocale( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );  //xp 下可以用此值
		//SetLanguageID(SUBLANG_ENGLISH_US);
    }else if (SUBLANG_CHINESE_SIMPLIFIED == theApp.m_pLangManger->m_strCurLanguage && MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED) != LangId)
    {
		SetThreadUILanguage( MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED) );  //win 7 下用
		
		//	SetThreadLocale( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );
	}

	while(1)
	{
		if (theApp.m_projectData.m_LinkType == CONNECT_BY_USB)
		{
//			::PostMessage((HWND)(AfxGetMainWnd()->GetSafeHwnd()),WM_SHOW_MESSAGE,19,NULL);
			if (!rView->LoadData(FALSE))   //更新数据
			{
				return 0;
			}
		}
		else
		{
			if (!rView->LoadData(FALSE))   //更新数据
			{
				return 0;
			}
			Sleep(800);
		}
				
	}
	return 0;
}

DWORD WINAPI LoadRealThread(LPVOID lpParameter)
//没用到
{
	//CRealDataView* rView=(CRealDataView*)lpParameter;
	
	while(1)
	{
		if (!rView->LoadData(TRUE))   //查看实测类型改变
		{
			return 0;
		}
	}
	//realEv.SetEvent();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CRealDataView

IMPLEMENT_DYNCREATE(CRealDataView, CFormView)

CRealDataView::CRealDataView()
	: CFormView(CRealDataView::IDD)
{
	//{{AFX_DATA_INIT(CRealDataView)
	//}}AFX_DATA_INIT
	m_nCount=MAX_REAL_COUNT;
//	m_nDataType = 0;
	memset(m_arrRealData,0,sizeof(m_arrRealData));
}

CRealDataView::~CRealDataView()
{
	WaitForSingleObject(m_bReadFinish.m_hObject,5000);      //等待通信完成（事件对象）
	TerminateThread(hThread,0);
}

void CRealDataView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRealDataView)
	DDX_Control(pDX, IDC_REAL_DATA_TYPE, m_ctlDataType);
	DDX_Control(pDX, IDC_REAL_LIST, m_listRealData);
//	DDX_Control(pDX, IDC_REFURBISH_FREQ, m_ctlRefurbishFreq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRealDataView, CFormView)
	//{{AFX_MSG_MAP(CRealDataView)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_REAL_DATA_TYPE, OnSelchangeRealDataType)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRealDataView diagnostics

#ifdef _DEBUG
void CRealDataView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRealDataView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRealDataView message handlers

void CRealDataView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	theApp.m_pLangManger->SetWndStrings(this, IDD);
	theApp.m_pLangManger->SetWndStrings(this, IDD);
	rView = this;
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	CRect rc;
	ScreenToClient(&rc);
	m_ctlDataType.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_ctlDataType.MoveWindow(rc.left,rc.top,rc.Width(),300);
	m_listRealData.GetClientRect(&rc);
	long width=rc.Width();
	m_listRealData.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	CString info;
	info.LoadString( IDS_SERIAL );
	m_listRealData.InsertColumn(0, info,LVCFMT_LEFT,50);
	info.LoadString( IDS_REAL_DATANAME );
	m_listRealData.InsertColumn(1, info,LVCFMT_LEFT,180);
	info.LoadString( IDS_REAL_DATATYPE );
	m_listRealData.InsertColumn(2, info,LVCFMT_LEFT,130);
	info.LoadString( IDS_MEASRUE_VALUE );
	m_listRealData.InsertColumn(3, info,LVCFMT_LEFT,140);
	info.LoadString( IDS_EXPAND_VALUE );
	m_listRealData.InsertColumn(4, info,LVCFMT_LEFT,width-520);

	hThread=CreateThread( NULL , 0 , FreshRealThread , this , 0,  NULL) ;
//	VERIFY(IDC_FREQ_TIMER == SetTimer(IDC_FREQ_TIMER,m_dRefurbishFreq*500,NULL));

}


void CRealDataView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==IDC_FREQ_TIMER)
	{
	//	UpdateData(FALSE);
	}
	CFormView::OnTimer(nIDEvent);
}

/***************************************************************************
函数名：LoadData
描述：加载实测数据
参数：bIsDelAllItem[in]:bIsDelAllItem=true时，一般是选择查看实测类型改变;
      =false时，一般是更新数据时所用
返回：成功：TRUE，否则FALSE
修改记录：
****************************************************************************/
BOOL CRealDataView::LoadData(BOOL bIsDelAllItem/* = FALSE*/)
{
	m_bReadFinish.ResetEvent();  //指定事件对象无信号
	int size=m_mapRealData.size(),i;
	if (!CFrame::GetRealData(&m_mapRealData))
	{
		m_bReadFinish.SetEvent();
		CString str;
		str.LoadString( IDS_CONNECT_ERROR );
		CdMessageBox box(this, str, _T(""), MB_OK);
        box.MsgBoxEx();
		CMainFrame::StopCommunication();
		return false;
	}
	m_bReadFinish.SetEvent();
	CString strDataType,str,info;
	if(bIsDelAllItem)//清除先前显示的所有项
	{
		m_listRealData.DeleteAllItems();
	}
	i=0;
	//int temp=m_mapRealData.size();
	if(size==m_mapRealData.size())
	{
		m_ctlDataType.GetLBText(m_nDataType,strDataType);
		info.LoadString( IDS_ALL_TYPE ); 
		if(strDataType == info)
		{
			for(m_tempItem=m_mapRealData.begin();m_tempItem!=m_mapRealData.end();m_tempItem++)
			{
				if(bIsDelAllItem)
				{
					str.Format(_T("%d"),i);
					m_listRealData.InsertItem(/*LVIF_TEXT|LVIF_IMAGE,*/i,str/*,0,0,-1,NULL*/);
					m_listRealData.SetItemText(i,1,(CString)(*m_tempItem).second.strName);
					m_listRealData.SetItemText(i,2,(CString)(*m_tempItem).second.strType);
				}
				m_listRealData.SetItemText(i,3,(CString)(*m_tempItem).second.strValue);
				m_listRealData.SetItemText(i,4,(CString)(*m_tempItem).second.strExtend);
				i++;
			}
		}
		else
		{
			for(m_tempItem=m_mapRealData.begin();m_tempItem!=m_mapRealData.end();m_tempItem++)
			{
				if(strDataType == (*m_tempItem).second.strType)
				{
					if(bIsDelAllItem)
					{
						str.Format(_T("%d"),i);
						m_listRealData.InsertItem(/*LVIF_TEXT|LVIF_IMAGE,*/i,str/*,0,0,-1,NULL*/);
						m_listRealData.SetItemText(i,1,(CString)(*m_tempItem).second.strName);
						m_listRealData.SetItemText(i,2,(CString)(*m_tempItem).second.strType);
					}
					
					m_listRealData.SetItemText(i,3,(CString)(*m_tempItem).second.strValue);
					m_listRealData.SetItemText(i,4,(CString)(*m_tempItem).second.strExtend);
					i++;
				}
			}
		}
	}
	else
	{
		//size=m_mapRealData.size();
		m_listRealData.DeleteAllItems();
		for(i=m_ctlDataType.GetCount()-1;i>=0;i--)
		{
			m_ctlDataType.DeleteString(i); 
		}
		strDataType.LoadString( IDS_ALL_TYPE );
		m_ctlDataType.AddString( strDataType );
		i=0;
		for(m_tempItem=m_mapRealData.begin();m_tempItem!=m_mapRealData.end();m_tempItem++)
		{//往控件上添加实测类型
			if(strDataType != (*m_tempItem).second.strType)
			{
				strDataType = (*m_tempItem).second.strType;
				m_ctlDataType.AddString(strDataType);
			}
			str.Format(_T("%d"),i);
			m_listRealData.InsertItem(/*LVIF_TEXT|LVIF_IMAGE,*/i,str/*,0,0,-1,NULL*/);
			str=(*m_tempItem).second.strName;
			m_listRealData.SetItemText(i,1,str);
			str=(*m_tempItem).second.strType;
			m_listRealData.SetItemText(i,2,str);
			str=(*m_tempItem).second.strValue;
			m_listRealData.SetItemText(i,3,str);
			str=(*m_tempItem).second.strExtend;
			m_listRealData.SetItemText(i,4,str);
			i++;
		}
		CString allTpyeStr; 
		allTpyeStr.LoadString( IDS_ALL_TYPE );
		m_ctlDataType.SelectString(0,allTpyeStr);
		m_nDataType = 0;
	}
	//UpdateData(FALSE);
	return TRUE;
}
/***************************************************************************
函数名：OnSelchangeRealDataType
描述：实测类型下拉列表选择变化时的响应函数
参数：无
返回：无
修改记录：
****************************************************************************/
void CRealDataView::OnSelchangeRealDataType() 
{
	// TODO: Add your control notification handler code here

	WaitForSingleObject(m_bReadFinish.m_hObject,5000);      //等待通信完成（事件对象）
	TerminateThread(hThread,0);
	m_nDataType = m_ctlDataType.GetCurSel();
	LoadData(TRUE);  //查看实测类型改变

	hThread = CreateThread( NULL , 0 , FreshRealThread, this , 0,  NULL) ;
	this->m_listRealData.SetFocus();
}

void CRealDataView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
	if(m_listRealData.GetSafeHwnd())
	{
		
		m_listRealData.SetWindowPos(   
			this,   10,   50,   
			cx-20,   cy-60,     
            SWP_NOOWNERZORDER   |   SWP_NOZORDER   |   SWP_SHOWWINDOW   );   
	}
}

BOOL CRealDataView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::OnScroll(nScrollCode, nPos, bDoScroll);
}


