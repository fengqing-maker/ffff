// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "InfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog
extern CMySID_2FYApp theApp;

CInfoDlg::CInfoDlg(CString strCaption,CString strInfo,CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfoDlg)
	m_strInfo = strInfo;
	m_strCaption = strCaption;
	//}}AFX_DATA_INIT
	m_nRe = 0;
}


void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDlg)
	DDX_Text(pDX, IDC_INFO, m_strInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CInfoDlg)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers

BOOL CInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	// TODO: Add extra initialization here
	SetWindowText(m_strCaption);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInfoDlg::OnYes() 
{
	// TODO: Add your control notification handler code here
	m_nRe = IDYES;
	CDialog::OnOK();
}

void CInfoDlg::OnNo() 
{
	// TODO: Add your control notification handler code here
	m_nRe = IDNO;
	CDialog::OnOK();
}


int CInfoDlg::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	int re = CDialog::DoModal();
	return m_nRe?m_nRe:re;
}
