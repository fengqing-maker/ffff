// PWDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "PWDDlg.h"
#include "Md5Class.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPWDDlg dialog

extern char *MD5String(char* szString);
extern CMySID_2FYApp theApp;

CPWDDlg::CPWDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPWDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPWDDlg)
	m_nPassWord = _T("");
	m_bCorrect = FALSE;
	//}}AFX_DATA_INIT
}


void CPWDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPWDDlg)
	DDX_Text(pDX, IDC_PSW, m_nPassWord);
	DDV_MaxChars(pDX, m_nPassWord, 16);
	//}}AFX_DATA_MAP
}

BOOL CPWDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CPWDDlg, CDialog)
	//{{AFX_MSG_MAP(CPWDDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPWDDlg message handlers

void CPWDDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	// TODO: Add extra validation here
	UpdateData();
	CString str, info;
	if(m_nPassWord.GetLength() == 0)
	{
		info.LoadString( IDS_PWD_INFO2 );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	char lstrPsw[32];
	VERIFY(::WideCharToMultiByte(CP_ACP,0,m_nPassWord,-1,lstrPsw,32,NULL,NULL));
	char *pStr = MD5String(lstrPsw);
	//fwprintf(theApp.m_traceFile,_T("%s==%s"),theApp.m_strCurrentPSW,pStr);
	if(strcmp(theApp.m_strCurrentPSW,pStr))
	{
		info.LoadString( IDS_PWD_ERROR );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		free(pStr);
		SetDlgItemText(IDC_PSW,_T(""));
		return;
	}
	free(pStr);
	m_bCorrect = TRUE;
	CDialog::OnOK();
}
