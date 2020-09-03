// SetIP.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "SetIP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetIP dialog


CSetIP::CSetIP(CWnd* pParent /*=NULL*/)
	: CDialog(CSetIP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetIP)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetIP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetIP)
	DDX_Control(pDX, IDC_IPADDRESS, m_IP);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetIP, CDialog)
	//{{AFX_MSG_MAP(CSetIP)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetIP message handlers

void CSetIP::OnOK() 
{
	// TODO: Add extra validation here
	if (pParent)
	{
		m_IP.GetAddress(pParent->m_ipaddrDevice);
	}
	if (setParent)
	{
		m_IP.GetAddress(setParent->m_setIP);
	}
	CDialog::OnOK();
}

BOOL CSetIP::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (setParent != NULL)
	{
		m_IP.SetAddress(setParent->m_setIP);
	}
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
