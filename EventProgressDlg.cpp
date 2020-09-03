// EventProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "EventProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventProgressDlg dialog


CEventProgressDlg::CEventProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventProgressDlg)
	m_strTypeInfo = _T("");
	//}}AFX_DATA_INIT
}


void CEventProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventProgressDlg)
	DDX_Control(pDX, IDC_EVENT_PROGRESS, m_ctlProgress);
	DDX_Text(pDX, IDC_TYPE_INFO, m_strTypeInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CEventProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventProgressDlg message handlers

void CEventProgressDlg::SetEvevntType(CString str,BOOL bIsShowProgress/*=FALSE*/,int nUp/*=0*/,int nLow/*=0*/)
{
	m_strTypeInfo = str;
	
	if(bIsShowProgress)
	{
		m_ctlProgress.ShowWindow(SW_SHOW);
		m_ctlProgress.SetRange32(nLow,nUp);
		m_ctlProgress.SetPos(nLow);
	}
	UpdateData(FALSE);
}

void CEventProgressDlg::SetProgressPos(int pos)
{
	m_ctlProgress.SetPos(pos);
	UpdateData(FALSE);
}

void CEventProgressDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	DestroyWindow();
}

void CEventProgressDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
}
extern CMySID_2FYApp theApp;
BOOL CEventProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	return TRUE;
}