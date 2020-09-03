// DefineScale.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "DefineScale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefineScale dialog
int CDefineScale::m_curSel = 0;

CDefineScale::CDefineScale(CWnd* pParent /*=NULL*/)
	: CDialog(CDefineScale::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefineScale)
	//}}AFX_DATA_INIT
}


void CDefineScale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineScale)
	DDX_Control(pDX, IDC_COMBO_SCALE, m_comboScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefineScale, CDialog)
	//{{AFX_MSG_MAP(CDefineScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineScale message handlers

BOOL CDefineScale::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str;
	for (int i = 0 ;i < 10;i++)
	{
		str.Format(_T("%d"),i+1);
		m_comboScale.AddString(str);
	}
	m_comboScale.SetCurSel(m_curSel);
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDefineScale::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	m_curSel = m_comboScale.GetCurSel();
	CDialog::OnOK();
}
