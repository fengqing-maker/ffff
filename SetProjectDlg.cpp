// SetProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "SetProjectDlg.h"
#include "SetIP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetProjectDlg dialog

extern CMySID_2FYApp theApp;
CSetProjectDlg::CSetProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetProjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetProjectDlg)
	m_strDBFileName = theApp.m_projectData.m_strDBFileName;
	m_strProjectName =theApp.m_projectData.m_strProjectName;
	m_strProjectType = theApp.m_projectData.m_strDeviceType;
	m_setIP = 0;
	//}}AFX_DATA_INIT
}


void CSetProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetProjectDlg)
	DDX_Control(pDX, IDC_COMBO_LINK, m_comboLink);
	DDX_Text(pDX, IDC_DB_FILENAME, m_strDBFileName);
	DDX_Text(pDX, IDC_PROJECT_NAME, m_strProjectName);
	DDX_Text(pDX, IDC_PROJECT_TYPE, m_strProjectType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetProjectDlg, CDialog)
	//{{AFX_MSG_MAP(CSetProjectDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_LINK, OnSelchangeComboLink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetProjectDlg message handlers
void CSetProjectDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	theApp.m_projectData.m_arrIPAddr[0] = (unsigned char)(m_setIP>>24);
	theApp.m_projectData.m_arrIPAddr[1] = (unsigned char)(m_setIP>>16);
	theApp.m_projectData.m_arrIPAddr[2] = (unsigned char)(m_setIP>>8);
	theApp.m_projectData.m_arrIPAddr[3] = (unsigned char)(m_setIP);
// 	BYTE ipAddr[4];
// 	m_ipAddrDevice.GetAddress(ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);
// 	if(0!=memcmp(theApp.m_projectData.m_arrIPAddr,ipAddr,sizeof(ipAddr)))
// 	{
// 		memcpy(theApp.m_projectData.m_arrIPAddr,ipAddr,sizeof(ipAddr));
// 	}
	CDialog::OnOK();
}

BOOL CSetProjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	CEdit  *pEdit1  =  (CEdit*)m_comboLink.GetWindow(GW_CHILD); 
	//m_combo1用你的控件变量代替就OK了。。 
	if  (pEdit1)  
        pEdit1->SetReadOnly();
	m_setIP = (unsigned long)(theApp.m_projectData.m_arrIPAddr[0]<<24)+(unsigned long)(theApp.m_projectData.m_arrIPAddr[1]<<16)
		+(unsigned long)(theApp.m_projectData.m_arrIPAddr[2]<<8)+(unsigned long)theApp.m_projectData.m_arrIPAddr[3];

	CString netConnectStr,usbConnectStr;
	netConnectStr.LoadString( IDS_NETWORK_CONN );
    usbConnectStr.LoadString( IDS_USB_CONN );

	if (m_strProjectType == DVI_DEVICE)
	{
		m_comboLink.AddString( usbConnectStr );
	}
	else
	{
		m_comboLink.AddString( netConnectStr );
		m_comboLink.AddString( usbConnectStr );
	}
	// TODO: Add extra initialization here
// 	m_ipAddrDevice.SetAddress(theApp.m_projectData.m_arrIPAddr[0],theApp.m_projectData.m_arrIPAddr[1],
// 		theApp.m_projectData.m_arrIPAddr[2],theApp.m_projectData.m_arrIPAddr[3]);
	if (theApp.m_projectData.m_LinkType&CONNECT_BY_NET)
	{
		m_comboLink.SetCurSel(1);
	}
	else if (theApp.m_projectData.m_LinkType&CONNECT_BY_USB)
	{
		m_comboLink.SetCurSel(0);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetProjectDlg::OnSelchangeComboLink() 
{
	// TODO: Add your control notification handler code here
	int nTemp = m_comboLink.GetCurSel();
	CString str;
	m_comboLink.GetLBText(nTemp,str);
	CString netConnectStr,usbConnectStr;
	netConnectStr.LoadString( IDS_NETWORK_CONN );
    usbConnectStr.LoadString( IDS_USB_CONN );

	if (str == netConnectStr )
	{
		CSetIP dlg;
		dlg.setParent = this;
		dlg.pParent = NULL;
		//dlg.m_IP = theApp.m_projectData.m_arrIPAddr;
		dlg.DoModal();
		theApp.m_projectData.m_LinkType = CONNECT_BY_NET;
	}
	else if (str == usbConnectStr )
	{
		theApp.m_projectData.m_LinkType = CONNECT_BY_USB;
	}
}
