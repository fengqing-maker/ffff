// CreateProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include <boost/regex.hpp>
#include "SID_2FY.h"
#include "CreateProjectDlg.h"
#include "SetIP.h"
#include "dMessageBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateProjectDlg dialog

extern CMySID_2FYApp theApp;
CCreateProjectDlg::CCreateProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateProjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateProjectDlg)
	m_strPath = _T("");
	m_strProjectName = _T("");
	m_bLinkDevice = FALSE;
	m_bCreateResult = FALSE;
	m_nSel = CONNECT_BY_USB;
	//}}AFX_DATA_INIT
}


void CCreateProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateProjectDlg)
	DDX_Control(pDX, IDC_DEVICE_TYPE, m_listDeviceType);
	DDX_Text(pDX, IDC_LOCATION, m_strPath);
	DDX_Text(pDX, IDC_PROJECT_NAME, m_strProjectName);
	DDV_MaxChars(pDX, m_strProjectName, 30);
	DDX_Check(pDX, IDC_CREATE_LINK, m_bLinkDevice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateProjectDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateProjectDlg)
	ON_BN_CLICKED(IDC_PATH_BTN, OnPathBtn)
	ON_EN_CHANGE(IDC_PROJECT_NAME, OnChangeProjectName)
	ON_CBN_SELCHANGE(IDC_LINKTYPE, OnSelchangeLinkType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateProjectDlg message handlers

void CCreateProjectDlg::OnPathBtn() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	BROWSEINFO   bi;                         //BROWSEINFO结构体   
	TCHAR   Buffer[MAX_PATH]=_T("");   
	TCHAR   FullPath[MAX_PATH]=_T("");   
	bi.hwndOwner   =   m_hWnd;       //m_hWnd你的程序主窗口   
	bi.pidlRoot   =   NULL;   
	bi.pszDisplayName   =   Buffer;   //返回选择的目录名的缓冲区   
	bi.lpszTitle   =   _T("Selection");   //弹出的窗口的文字提示   
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS   ;   //只返回目录。其他标志看MSDN   
	bi.lpfn   =   NULL;                                                   //回调函数，有时很有用   
	bi.lParam   =   0;   
	bi.iImage   =   0;   
	ITEMIDLIST*   pidl   =   ::SHBrowseForFolder   (&bi);   //显示弹出窗口，ITEMIDLIST很重要   
	if(::SHGetPathFromIDList(pidl,FullPath))           //在ITEMIDLIST中得到目录名的整个路径   
	{   
		//成功   
		m_strPath=FullPath;
		if(m_strPath[m_strPath.GetLength()-1]!=TEXT('\\'))
		{
			m_strPath+=_T("\\")+m_strProjectName;
		}
		else
		{
			m_strPath+=m_strProjectName;
		}
		
		UpdateData(FALSE);
	}   
}

void CCreateProjectDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	CString info;
	if(m_strPath==_T("") || m_strProjectName==_T(""))
	{
		info.LoadString(IDS_CREATE_INFO);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	boost::regex expression("([\\d])\\1{2,}");
	boost::cmatch what;
	char tempFileName[255];
	WideCharToMultiByte(CP_ACP,0,m_strProjectName,-1,tempFileName,255,NULL,NULL);//CString转char[]
	if(boost::regex_match(tempFileName, what, expression))
	{
		info.LoadString(IDS_PRJNAME_ERROR);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	if (m_bLinkDevice)
	{
		m_nSel |= CONNECT_DEVICE;
	}
	else
	{
		m_nSel &= 0xFFFFFFFE;
	}
	//获取设备类型
	POSITION position;
	position=m_listDeviceType.GetFirstSelectedItemPosition();
	CString strDeviceType;
	strDeviceType=m_listDeviceType.GetItemText(m_listDeviceType.GetNextSelectedItem(position),0);
	CString netConnectStr;
	netConnectStr.LoadString( IDS_NETWORK_CONN );
	if (strDeviceType == DVI_DEVICE)
	{
		CComboBox* pLinkCombo = static_cast<CComboBox*>(GetDlgItem(IDC_LINKTYPE));
		int nTemp = pLinkCombo->GetCurSel();
		CString strSel ;
		pLinkCombo->GetLBText(nTemp , strSel);
		if (strSel == netConnectStr )
		{
			CString str;
			str.LoadString( IDS_DVI_CONNECT_ERROR );
			CdMessageBox box( theApp.m_pMainWnd, str);
	     	box.MsgBoxEx();
			return;
		}
	}
	BYTE ipAddr[4];
	::memset(ipAddr,0,sizeof(ipAddr));
	if (m_ipaddrDevice)
	{
		ipAddr[3] = (unsigned char)m_ipaddrDevice;
		ipAddr[2] = (unsigned char)(m_ipaddrDevice>>8);
		ipAddr[1] = (unsigned char)(m_ipaddrDevice>>16);
		ipAddr[0] = (unsigned char)(m_ipaddrDevice>>24);
		if(ipAddr[0]==0)
		{
		    info.LoadString(IDS_CREATE_IP);
		    CdMessageBox box(this, info);
		    box.MsgBoxEx();
			return;
		}
	}
	CProjectData project;
	if(project.CreateProject(strDeviceType,m_strProjectName,m_strPath,ipAddr,m_nSel))
	{
		if ((CString)theApp.m_projectData.m_strDeviceType != _T("") && m_bLinkDevice)
		{
			::memcpy(project.m_strDeviceType,theApp.m_projectData.m_strDeviceType,sizeof(theApp.m_projectData.m_strDeviceType));
		}
		else if (!m_bLinkDevice)
		{
			::memcpy(project.m_strDeviceType,"尚未进行装置连接",sizeof(theApp.m_projectData.m_strDeviceType));
		}
		theApp.m_projectData=project;
		m_bCreateResult = TRUE;
	}
	else
	{
		return;
	}
	CDialog::OnOK(); 
}

void CCreateProjectDlg::OnChangeProjectName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_strPath!=_T(""))
	{
		int index=m_strPath.ReverseFind(TEXT('\\'));
		m_strPath=m_strPath.Left(1+index);
		m_strPath+=m_strProjectName;
	}
	UpdateData(FALSE);
}

BOOL CCreateProjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	// TODO: Add extra initialization here
	UpdateData();
	
	CComboBox* pLinkCombo = static_cast<CComboBox*>(GetDlgItem(IDC_LINKTYPE));
	CString netConnectStr,usbConnectStr;
	netConnectStr.LoadString( IDS_NETWORK_CONN );
    usbConnectStr.LoadString( IDS_USB_CONN );
	pLinkCombo->AddString( netConnectStr );
	pLinkCombo->AddString( usbConnectStr );

	pLinkCombo->SetCurSel(0);
	m_imageList.Create(16,16,ILC_COLOR24|ILC_MASK,1,1);
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_DEVICE));
	m_ipaddrDevice = 0;
	m_listDeviceType.SetImageList(&m_imageList,LVSIL_SMALL);  //设备类型列表
	map <CString,CString>::iterator item;
	int i = 0;
	for(item = theApp.m_mapDiviceType.begin();item != theApp.m_mapDiviceType.end();item++)
	{
		m_listDeviceType.InsertItem(i++,item->first,0);	
	}
	SetWindowLong(m_listDeviceType.m_hWnd,GWL_STYLE,
		GetWindowLong(m_listDeviceType.m_hWnd,GWL_STYLE)|LVS_SINGLESEL ); 
	LVITEM lvItem;
	::memset(&lvItem,0,sizeof(lvItem));
	lvItem.mask=LVIF_STATE;
	lvItem.state=LVIS_SELECTED|LVIS_FOCUSED;
	m_listDeviceType.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED,-1);//LVIS_SELECTED|LVIS_FOCUSED ,LVIF_STATE
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateProjectDlg::OnSelchangeLinkType() 
{
	// TODO: Add your control notification handler code here 选择连接设备方式
	CComboBox* pLinkCombo = static_cast<CComboBox*>(GetDlgItem(IDC_LINKTYPE));
	int nTemp = pLinkCombo->GetCurSel();
	CString strSel ;
	pLinkCombo->GetLBText(nTemp , strSel);

	CString netConnectStr,usbConnectStr;
	netConnectStr.LoadString( IDS_NETWORK_CONN );
    usbConnectStr.LoadString( IDS_USB_CONN );

	if (strSel == netConnectStr )
	{
		CSetIP dlg;
		dlg.pParent = this;
		dlg.setParent = NULL;
		int ret = dlg.DoModal();
		if (m_bLinkDevice)
		{
			m_nSel = CONNECT_BY_NET|CONNECT_DEVICE;
		}
		else
		{
			m_nSel = CONNECT_BY_NET;
		}
	}
	if (strSel == usbConnectStr)
	{
		if (m_bLinkDevice)
		{
			m_nSel = CONNECT_BY_USB|CONNECT_DEVICE;
		}
		else
		{
			m_nSel = CONNECT_BY_USB;
		}
	}
}
