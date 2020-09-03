// hangePswDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "hangePswDlg.h"
#include "Md5Class.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ChangePswDlg dialog


extern char *MD5String(char* szString);
extern CMySID_2FYApp theApp;

ChangePswDlg::ChangePswDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ChangePswDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ChangePswDlg)
	m_nOriginalPsw = _T("");
	m_nModifyPsw1 = _T("");
	m_nModifyPsw2 = _T("");
	m_bCorrect = FALSE;
	//}}AFX_DATA_INIT
}


void ChangePswDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ChangePswDlg)
	DDX_Text(pDX, IDC_ORIGINAL_PSW, m_nOriginalPsw);
	DDV_MaxChars(pDX, m_nOriginalPsw, 16);
	DDX_Text(pDX, IDC_MODIFY_PSW, m_nModifyPsw1);
	DDV_MaxChars(pDX, m_nModifyPsw1, 16);
	DDX_Text(pDX, IDC_MODIFY_CONFIRM, m_nModifyPsw2);
	DDV_MaxChars(pDX, m_nModifyPsw2, 16);
	//}}AFX_DATA_MAP
}

BOOL ChangePswDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	return TRUE;
}

BEGIN_MESSAGE_MAP(ChangePswDlg, CDialog)
	//{{AFX_MSG_MAP(ChangePswDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ChangePswDlg message handlers

void ChangePswDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	char lstrPsw[32];
	CString str, info, title;
	if(m_nModifyPsw2 != m_nModifyPsw1)
	{
		title.LoadString( AFX_IDS_APP_TITLE );
		info.LoadString( IDS_PWDC_INFO3 );
		CdMessageBox box(this, info, title);
	 	 box.MsgBoxEx();
		return;
	}
	
	VERIFY(::WideCharToMultiByte(CP_ACP,0,m_nOriginalPsw,-1,lstrPsw,32,NULL,NULL));
	char *pStr = MD5String(lstrPsw);
	if(strcmp(theApp.m_strCurrentPSW,pStr) && strcmp(theApp.m_strOmnipotencePSW,pStr))
	{
		info.LoadString( IDS_PWDC_OLDW );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		free(pStr);
		m_nOriginalPsw = _T("");
		UpdateData(FALSE);
		return;
	}
	free(pStr);
	VERIFY(::WideCharToMultiByte(CP_ACP,0,m_nModifyPsw1,-1,lstrPsw,32,NULL,NULL));
	pStr = MD5String(lstrPsw);
	if(!theApp.SetCurPassWord(pStr))
	{
		free(pStr);
		info.LoadString( IDS_PWDC_ERROR );
		CdMessageBox box(this, info);
	 	box.MsgBoxEx();
		return;
	}
	free(pStr);
	m_bCorrect = true;
	CDialog::OnOK();
}
