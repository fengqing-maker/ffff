// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include <map>
#include "SID_2FY.h"
#include "MainFrm.h"
#include "CreateProjectDlg.h"
#include "SetProjectDlg.h"
#include "EventView.h"
#include "RealDataView.h"
#include "ParaView.h"
#include "DownloadSetDlg.h"
#include "CustomizeWareSetDlg.h"
#include "wareView.h"
#include "ProtectData.h"
#include "EventProgressDlg.h"
#include "CustomFileDialog.h"
#include "TestToolTip.h"
#include "UniteCurvDlg.h"
#include "hangePswDlg.h"
#include "PWDDlg.h"
#include "DefineScale.h"
#include "RealMonitorView.h"
#include "dMessageBox.h"
#include "WareEventData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CToolBarEx    CMainFrame::m_wndMainToolBar;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame
extern CMySID_2FYApp theApp;
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDC_NEW_PROJECT, OnNewProject)
	ON_COMMAND(IDC_PROJECT_WND, OnProject)
	ON_COMMAND(IDC_INFO_WND, OnInfoWnd)
	ON_COMMAND(IDC_OPEN_PROJECT, OnOpenProject)
	ON_NOTIFY(NM_RCLICK,IDC_TREECTRL,OnRclick)     
	ON_NOTIFY(NM_RCLICK,IDC_LIST_PARA_INFO,OnRclickParalist)
	ON_NOTIFY(NM_RCLICK,IDC_LIST_CHANNEL_INFO,OnRclickChanlist)
	ON_COMMAND(ID_SHOW_EVENT, OnShowEvent)
	ON_COMMAND(ID_GET_EVENT, OnGetEvent)
	ON_COMMAND(IDM_PROJECT_SET,OnSetProject)
	ON_COMMAND(IDM_DEVICE_CONNECT_TEST,OnTestDeviceConnection)
	ON_COMMAND(IDM_SHOW_CURVE,OnShowCurve)     //显示录波曲线
	ON_COMMAND(IDM_CURVE_UNITE,OnCurveUnite)
	ON_COMMAND(IDM_DELETE_OUT_TO_LIST,OnDeleteOutToList)
	ON_COMMAND(IDM_DELETE_FILE,OnDeleteFile)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREECTRL,OnSelchanged)
	ON_COMMAND(ID_GET_WARE_FILE, OnGetWareFile)
	ON_COMMAND(ID_OPEN_WARE_FILE, OnOpenWareFile)
	ON_COMMAND(ID_CURVE_H_BIGGER, OnCurveHBigger)
	ON_COMMAND(ID_CURVE_H_SMALLER, OnCurveHSmaller)
	ON_COMMAND(ID_CURVE_V_BIGGER, OnCurveVBigger)
	ON_COMMAND(ID_CURVE_V_SMALLER, OnCurveVSmaller)
	//ON_COMMAND(IDM_CURVE_ANALYSE,OnCurveAnalyse)
	ON_COMMAND(ID_OPEN_FILE, OnOpenFile)
	ON_COMMAND(IDM_READ_PARA_DATA,OnReadParaData)
	ON_COMMAND(IDM_IN_PARA_FILE,OnInParaFile)
	ON_COMMAND(IDM_OUT_PARA_DATA,OnOutParaData)
	ON_COMMAND(IDM_OUT_CHANNEL_DATA,OnOutChannelData)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_COMMAND(IDM_OUT_PARA_DATA_TO_EXCEL,OnOutParaToExcel)
	ON_COMMAND(IDM_OUT_REAL_DATA_TO_EXCEL,OnOutRealDataToExcel)
	ON_COMMAND(IDM_OUT_TO_COMTRADE,OnOutWareFileToComtrade)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_MODIFY_PSW, OnModifyPsw)
	ON_COMMAND(ID_MULTI_2, OnMulti2)
	ON_COMMAND(ID_MULTI_4, OnMulti4)
	ON_COMMAND(ID_MULTI_1, OnMulti1)
	ON_COMMAND(ID_IN_EVENT_FILE, OnInEventFile)
	ON_COMMAND(ID_OUT_EVENT, OnOutEvent)
	ON_COMMAND(IDC_RESTART, OnRestart)
	ON_COMMAND(IDM_RIGHT_BAR, OnRightBar)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_COMMAND(ID_DEFINE_SELF, OnDefineSelf)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	IDS_SUB_TIME,
	IDS_INDICATOR_TIME
// 	ID_INDICATOR_CAPS,
// 	ID_INDICATOR_NUM,
// 	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_nRClickWareFileIndex = -1;
	m_nHMulti = 1;
	m_nVMulti = 1;
	m_nMultiFactor = 4;
	m_bInProject  = FALSE;
	m_bStartSync = FALSE;
	m_bTelControl = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_BIG_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	m_wndToolBar.SetImageList(IDB_ENABLE,RGB(242,242,242),CToolBarEx::SIL_NORMAL);
	m_wndToolBar.SetImageList(IDB_DISABLE,RGB(242,242,242),CToolBarEx::SIL_DISABLED);
	if (!m_wndMainToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndMainToolBar.LoadToolBar(IDR_MAINTOOL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndMainToolBar.SetImageList(IDB_MAINTOOLBAR,RGB(242,242,242),CToolBarEx::SIL_NORMAL);
	m_wndMainToolBar.SetImageList(IDB_DISABLE_MAINTOOLBAR,RGB(242,242,242),CToolBarEx::SIL_DISABLED);
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(1,IDS_SUB_TIME,SBPS_DISABLED,230);
	m_wndStatusBar.SetPaneInfo(2,IDS_INDICATOR_TIME,SBPS_DISABLED,190);
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable

	//隐藏菜单
	//SetMenu(NULL);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_CURVE_H_BIGGER,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_CURVE_V_BIGGER,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_CURVE_H_SMALLER,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_CURVE_V_SMALLER,FALSE);
	CToolBarCtrl & toolBarCtrl = m_wndMainToolBar.GetToolBarCtrl();
	toolBarCtrl.EnableButton(ID_GET_EVENT,FALSE);
	toolBarCtrl.EnableButton(ID_GET_WARE_FILE,FALSE);
	toolBarCtrl.EnableButton(IDC_START_SYNC,FALSE);
	toolBarCtrl.EnableButton(IDM_UPDATE_PARA,FALSE);
	toolBarCtrl.EnableButton(IDC_RESTART,FALSE);
	toolBarCtrl.EnableButton(IDM_RIGHT_BAR,FALSE);
	toolBarCtrl.EnableButton(ID_CONNECT,FALSE);
	toolBarCtrl.EnableButton(ID_DISCONNECT,FALSE);
	/////////////创建控制条///////////////
    CString title;
	title.LoadString( IDS_PRJINFO );
	if (!m_wndLeftBar.Create(title, this, CSize(WIDTH_PROJECT,260),TRUE,IDC_LEFT_BAR))
	{
		TRACE0("Failed to create mybar\n");
		return -1;
	}
	title.LoadString( IDS_BOTTOM_TITLE );
	if (!m_wndBottonBar.Create(title, this,CSize(200,HEIGHT_INFO),TRUE,IDC_BOTTON_BAR))
	{
		TRACE0("Failed to create mybar\n");
		return -1;
	}
	/////////////这一段代码创建树型控件////////////
	if (!m_wndTree.Create(WS_CHILD|WS_VISIBLE|
		TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT,
		CRect(0, 0, 0, 0), &m_wndLeftBar, IDC_TREECTRL))
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;
	}
	m_wndTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_imageList.Create(32,32,ILC_COLOR24|ILC_MASK,5,5);
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_DEVICE));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_EVENT));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_REAL));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_PARA));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_WARE_FOLDER));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_WARE_FILE));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_REAL_MONITOR));
	m_wndTree.SetImageList(&m_imageList,TVSIL_NORMAL);
	SetWindowLong(m_wndTree.m_hWnd,GWL_STYLE,WS_VISIBLE | WS_TABSTOP | WS_CHILD 
		|TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES|TVS_SHOWSELALWAYS);

	
	//右边视图
	title.LoadString( IDS_RIGTH_VIEW );
	if (!m_wndRightBar.Create(title, this, CSize(WIDTH_OF_RIGHT_BAR,260),TRUE,IDC_RIGHT_BAR))
	{
		TRACE0("Failed to create mybar\n");
		return -1;
	}
	
	if(!m_wndRightTabCtrl.Create(TCS_DOWN|WS_CHILD|WS_VISIBLE,CRect(0,0,500,100),&m_wndRightBar,IDC_RIGHT_LIST_TAB))
		return -1;
	
	m_TabCtrl.Create(TCS_DOWN|WS_CHILD|WS_VISIBLE,CRect(0,0,500,100),&m_wndBottonBar,3);
	if(!m_listParaInfo.Create(WS_VSCROLL|WS_CHILD|WS_VISIBLE|LVS_ALIGNLEFT|LVS_REPORT,CRect(0,0,0,0),
		&m_TabCtrl,IDC_LIST_PARA_INFO))
		return -1;
	if(!m_editWareFileInfo.Create(WS_VSCROLL|WS_CHILD|ES_READONLY|WS_VISIBLE|ES_AUTOVSCROLL|ES_MULTILINE|ES_WANTRETURN,
		CRect(0,0,0,0),&m_TabCtrl,IDC_EDIT_FILE_INFO))
		return -1;
	if(!m_listChannelInfo.Create(WS_VSCROLL|WS_CHILD|WS_VISIBLE|LVS_ALIGNLEFT|LVS_REPORT,CRect(0,0,0,0),
		&m_TabCtrl,IDC_LIST_CHANNEL_INFO))
		return -1;
	//2016-10-20
	if(!m_listWareEvent.Create(WS_VSCROLL|WS_CHILD|WS_VISIBLE|LVS_ALIGNLEFT|LVS_REPORT,CRect(0,0,0,0),
		&m_TabCtrl,IDC_LIST_CHANNEL_INFO))
		return -1;

	if(!m_listSyncPara.Create(WS_VSCROLL|WS_CHILD|WS_VISIBLE|LVS_ALIGNLEFT|LVS_REPORT,CRect(0,0,0,0),
		&m_wndRightTabCtrl,IDC_SYNC_PARA_LIST))
		return -1;
	if(!m_listState.Create(WS_VSCROLL|WS_CHILD|WS_VISIBLE|LVS_ALIGNLEFT|LVS_REPORT,CRect(0,0,0,0),
		&m_wndRightTabCtrl,IDC_STATE_LIST))
		return -1;
	
	m_listSyncPara.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	m_listState.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	title.LoadString( IDS_SYN_PARA );
	m_wndRightTabCtrl.AddPage(&m_listSyncPara, title);
	title.LoadString( IDS_STATUS_INFO );
	m_wndRightTabCtrl.AddPage(&m_listState, title);
	m_wndRightTabCtrl.UpdateWindow();


	m_listParaInfo.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	m_editWareFileInfo.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	m_listChannelInfo.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	m_listWareEvent.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	CString info;
	info.LoadString( IDS_WAVE_FILE );
	m_TabCtrl.AddPage(&m_editWareFileInfo, info);
	info.LoadString( IDS_CHANEL_INFO ); 
	m_TabCtrl.AddPage(&m_listChannelInfo, info);
	info.LoadString( IDS_PRO_INFO );
	m_TabCtrl.AddPage(&m_listParaInfo,info);
	info.LoadString(IDS_WAVEEVENT_SUBITEM);
	m_TabCtrl.AddPage(&m_listWareEvent,info);

	m_TabCtrl.UpdateWindow();
	///////////////停靠控制///////////////////
 	m_wndRightBar.SetBarStyle(m_wndRightBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndLeftBar.SetBarStyle(m_wndLeftBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndBottonBar.SetBarStyle(m_wndBottonBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
// 	m_wndRightBar.EnableDocking(CBRS_ALIGN_RIGHT);

	
	m_wndRightBar.EnableDocking(CBRS_ALIGN_RIGHT);
	m_wndBottonBar.EnableDocking(CBRS_ALIGN_BOTTOM);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndLeftBar.EnableDocking(CBRS_ALIGN_LEFT);
	m_wndMainToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMainToolBar);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndLeftBar, AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndRightBar, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndBottonBar, AFX_IDW_DOCKBAR_BOTTOM);
	ShowControlBar(&m_wndRightBar, FALSE, FALSE);
	ShowControlBar(&m_wndBottonBar, FALSE, FALSE);

	
// 	DockControlBar(&m_wndRightBar, AFX_IDW_DOCKBAR_RIGHT);
// 	ShowControlBar(&m_wndRightBar, FALSE, FALSE);
// 	ShowControlBar(&m_wndBottonBar, FALSE, FALSE);
	
	
	m_listSyncPara.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	info.LoadString( IDS_PARAM_NAME );
	m_listSyncPara.InsertColumn(0, info,LVCFMT_LEFT,130);
	info.LoadString( IDS_PARAM_VALUE );
	m_listSyncPara.InsertColumn(1,info,LVCFMT_LEFT,WIDTH_OF_RIGHT_BAR-160);
	m_listState.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	info.LoadString( IDS_STATUS_TYPE );
	m_listState.InsertColumn(0, info,LVCFMT_LEFT,60);
	info.LoadString( IDS_STATUS_NAME );
	m_listState.InsertColumn(1, info,LVCFMT_LEFT,140);
	info.LoadString( IDS_STATUS_VALUE );
	m_listState.InsertColumn(2, info, LVCFMT_LEFT,WIDTH_OF_RIGHT_BAR-225);


	CRect rect;
	m_wndMainToolBar.GetWindowRect(&rect);
	rect.OffsetRect(1, 0);
	this->RecalcLayout();    //关键的一步，重新排列
	DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP, &rect);
	CRect rc;
	m_listChannelInfo.GetWindowRect(&rc);
	int width = rc.Width();
	m_listChannelInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	info.LoadString( IDS_CHANNEL_NAME );
	m_listChannelInfo.InsertColumn(0, info, LVCFMT_LEFT, 180);
	info.LoadString( IDS_CHANNEL_TYPE );
	m_listChannelInfo.InsertColumn(1, info, LVCFMT_LEFT, 100);
	info.LoadString( IDS_STR_CYCLE );
	m_listChannelInfo.InsertColumn(2, info, LVCFMT_LEFT, 150);
	info.LoadString( IDS_START_CYCLE );
	m_listChannelInfo.InsertColumn(3, info, LVCFMT_LEFT, 150);
	info.LoadString( IDS_END_CYCLE );
	m_listChannelInfo.InsertColumn(4, info, LVCFMT_LEFT,width - 300);
	m_listParaInfo.GetWindowRect(&rc);
	width = rc.Width();
	m_listParaInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	info.LoadString( IDS_PARA_TPYEANDINFO );
	m_listParaInfo.InsertColumn(0, info, LVCFMT_LEFT,180);
	info.LoadString( IDS_PARA_NAME);
	m_listParaInfo.InsertColumn(1, info, LVCFMT_LEFT,150);
	info.LoadString( IDS_PARA_VALUE );
	m_listParaInfo.InsertColumn(3, info, LVCFMT_LEFT,width - 400);	
	//add pwq 16.10.20  */
	m_listWareEvent.GetWindowRect(&rc);
	width = rc.Width();
	m_listWareEvent.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	info.LoadString( IDS_EVENT_NAME1 );
	m_listWareEvent.InsertColumn(0, info, LVCFMT_LEFT, 180);
	info.LoadString( IDS_EVENT_TIME );
	m_listWareEvent.InsertColumn(1, info, LVCFMT_LEFT, 100);
	info.LoadString( IDS_REMARK );
	m_listWareEvent.InsertColumn(2, info, LVCFMT_LEFT, 150);
	info.LoadString( IDS_EVENT_TYPE1 );
	m_listWareEvent.InsertColumn(3, info, LVCFMT_LEFT, width-150);
	

	CMenu *pMenu=GetMenu();
	pMenu->CheckMenuItem(ID_MULTI_2,MF_CHECKED);
	theApp.m_pLangManger->SetMenuStrings(pMenu);
    //lhz add 2013-4-8 解决new CFont时内存泄露
	m_editFont.CreateFont(15, // nHeight 
		 	0, // nWidth 
		 	0, // nEscapement 
		 	0, // nOrientation 
		 	FW_NORMAL, // nWeight 
		 	FALSE, // bItalic 
		 	FALSE, // bUnderline 
		 	0, // cStrikeOut 
		 	ANSI_CHARSET, // nCharSet 
		 	OUT_DEFAULT_PRECIS, // nOutPrecision 
		 	CLIP_DEFAULT_PRECIS, // nClipPrecision 
		 	DEFAULT_QUALITY, // nQuality 
		 	DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		 	_T("宋体")); // lpszFac 
	m_editWareFileInfo.SetFont(&m_editFont);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	m_strTitle.LoadString( IDS_MAIN_TILE );
	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers




void CMainFrame::OnNewProject() 
{
	// TODO: Add your command handler code here
	CCreateProjectDlg dlg;
	if(IDOK==dlg.DoModal() && dlg.m_bCreateResult)
	{
		LoadProjectData();  //加载界面
		ReleaseProject();
		CToolBarCtrl & toolBarCtrl = m_wndMainToolBar.GetToolBarCtrl();
		if(dlg.m_bLinkDevice)
		{
			toolBarCtrl.EnableButton(ID_GET_WARE_FILE,TRUE);  
//			toolBarCtrl.EnableButton(IDC_START_SYNC,TRUE);
//			toolBarCtrl.EnableButton(IDM_UPDATE_PARA,TRUE);
			toolBarCtrl.EnableButton(IDC_RESTART,TRUE);
			toolBarCtrl.EnableButton(ID_CONNECT,FALSE);
			toolBarCtrl.EnableButton(ID_DISCONNECT,TRUE);
		}
		else
		{
			toolBarCtrl.EnableButton(ID_GET_WARE_FILE,FALSE);
			toolBarCtrl.EnableButton(IDC_START_SYNC,FALSE);
			toolBarCtrl.EnableButton(IDM_UPDATE_PARA,FALSE);
			toolBarCtrl.EnableButton(IDC_RESTART,FALSE);
			toolBarCtrl.EnableButton(ID_CONNECT,TRUE);
			toolBarCtrl.EnableButton(ID_DISCONNECT,FALSE);
		}
		m_bInProject= TRUE;
		GetActiveDocument()->SetTitle((CString)theApp.m_projectData.m_strDeviceType);
		theApp.m_projectData.SaveProjectSetting();
	}
}
/*******************************************************************
  函数名：OnProject
  描述：显示隐藏工程视图区（导航树）
*******************************************************************/
void CMainFrame::OnProject() 
{
	// TODO: Add your command handler code here
	CMenu *pMenu=GetMenu();
	if(MF_CHECKED & pMenu->GetMenuState(IDC_PROJECT_WND,MF_BYCOMMAND))
	{
		pMenu->CheckMenuItem(IDC_PROJECT_WND,MF_UNCHECKED);
	}
	else
	{
		pMenu->CheckMenuItem(IDC_PROJECT_WND,MF_CHECKED);
	}
	ShowControlBar(&m_wndLeftBar, !m_wndLeftBar.IsVisible(), FALSE);
}

/*******************************************************************
  函数名：OnInfoWnd
  描述：显示隐藏信息视图区
*******************************************************************/
void CMainFrame::OnInfoWnd() 
{
	// TODO: Add your command handler code here
	
	ShowControlBar(&m_wndBottonBar, !m_wndBottonBar.IsVisible(), FALSE);
	
	/*CMenu *pMenu=GetMenu();
	if(MF_CHECKED & pMenu->GetMenuState(IDC_INFO_WND,MF_BYCOMMAND))
	{
		pMenu->CheckMenuItem(IDC_INFO_WND,MF_UNCHECKED);
	}
	else
	{
		pMenu->CheckMenuItem(IDC_INFO_WND,MF_CHECKED);
	}
	ShowControlBar(&m_wndBottonBar, !m_wndBottonBar.IsVisible(), FALSE);*/
}

/*******************************************************************
  函数名：OnOpenProject
  描述：打开工程
*******************************************************************/
void CMainFrame::OnOpenProject() 
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE,NULL,_T("*.prj"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("project files (*.prj)|*prj||"));
	if(IDOK==dlg.DoModal())
	{
		CProjectData project;
		CString path=dlg.GetPathName();

		if(project.OpenProject(path))
		{
			theApp.m_projectData=project;
			LoadProjectData();
			if(TestConnection())
			{
				CString info;
				info.LoadString( IDS_CONNECT_OK );
				CdMessageBox box(this, info);
	 	        box.MsgBoxEx();
				CToolBarCtrl & toolBarCtrl = m_wndMainToolBar.GetToolBarCtrl();
				toolBarCtrl.EnableButton(ID_DISCONNECT,TRUE);
				toolBarCtrl.EnableButton(ID_CONNECT,FALSE);
			}
			else
			{
				StopCommunication();
			}
			ReleaseProject();
			GetActiveDocument()->SetTitle((CString)theApp.m_projectData.m_strDeviceType);
			theApp.m_projectData.SaveProjectSetting();
		}
		m_bInProject= TRUE;
	}
}

/***********************************************************
功能：加载工程数据，包括导航树内容
***********************************************************/
void CMainFrame::LoadProjectData()
{
	VERIFY(m_wndTree.DeleteAllItems());
	HTREEITEM hProject = m_wndTree.InsertItem((CString)theApp.m_projectData.m_strProjectName,0,0,TVI_ROOT,TVI_FIRST);
	
	m_strDevice = theApp.m_projectData.m_strDeviceType;
	CString eventInfoStr;
	if (m_strDevice != DVI_DEVICE)
	{
	    eventInfoStr.LoadString( IDS_EVENT_INFO );
		m_wndTree.InsertItem(eventInfoStr,1,1,hProject,TVI_LAST);
		eventInfoStr.LoadString( IDS_REAL_PARA );
		m_wndTree.InsertItem( eventInfoStr,2,2,hProject,TVI_LAST);
        eventInfoStr.LoadString( IDS_RUN_WATCH );
		m_wndTree.InsertItem(eventInfoStr,6,6,hProject,TVI_LAST);
		eventInfoStr.LoadString( IDS_PROTECT_PARA );
		m_wndTree.InsertItem(eventInfoStr,3,3,hProject,TVI_LAST);
		eventInfoStr.LoadString( IDS_WAVE_FILE );
		HTREEITEM hWareFile=m_wndTree.InsertItem( eventInfoStr,4,4,hProject,TVI_LAST);
		int i=theApp.m_projectData.m_nFileCount-1;
		for(;i>=0;i--)
		{
			m_wndTree.InsertItem((CString)theApp.m_projectData.m_arrfileTreeNode[i].strFileName,5,5,hWareFile,TVI_LAST,i+1);
		}
	}
	else
	{	
		eventInfoStr.LoadString( IDS_PROTECT_PARA );
		m_wndTree.InsertItem(eventInfoStr,3,3,hProject,TVI_LAST);
	}
}

/**************************************************************
  函数：OnRclick
  描述；右击导航树上节点弹出不同菜单
  参数：pNMHDR 节点信息，pResult
  返回：
  修改记录：
**************************************************************/
void CMainFrame::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem= m_wndTree.GetSelectedItem();
	m_wndTree.SelectItem(hItem);
	CPoint point;
	GetCursorPos(&point);
	m_wndTree.ScreenToClient(&point);
	UINT nFlag;
	hItem=m_wndTree.HitTest(point,&nFlag);
	if ((hItem != NULL) && (TVHT_ONITEM & nFlag))
	{
		m_wndTree.Select(hItem, TVGN_CARET);
	}
	CString strParent,strSelect;
	CMenu PopupMenu;
	
	strParent=m_wndTree.GetItemText(m_wndTree.GetParentItem(hItem));
	strSelect=m_wndTree.GetItemText(hItem);
	m_wndTree.ClientToScreen(&point); 
	
	CString eventInfoStr,realDataStr,waveFileStr, paraStr, info;
	eventInfoStr.LoadString(IDS_EVENT_INFO );
	realDataStr.LoadString( IDS_REAL_PARA );
	waveFileStr.LoadString( IDS_WAVE_FILE );
	paraStr.LoadString( IDS_PROTECT_PARA );
	if(strParent==theApp.m_projectData.m_strProjectName)
	{
		if(strSelect==eventInfoStr)
		{
			PopupMenu.LoadMenu(IDR_MAINFRAME);
			PopupMenu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON ,
				point.x, point.y, this);
		}
		else if(strSelect== waveFileStr)
		{
			PopupMenu.LoadMenu(IDR_MAINFRAME);	
			PopupMenu.GetSubMenu(2)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON ,
				point.x, point.y, this);
		}
		else if(strSelect == realDataStr)
		{
			if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CRealDataView)))
			{
				PopupMenu.CreatePopupMenu();
				info.LoadString( IDS_EXPORTDATA );
				PopupMenu.AppendMenu(MF_ENABLED, IDM_OUT_REAL_DATA_TO_EXCEL, info );
				PopupMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x, point.y, this);
			}
			
		}
		else if(strSelect == paraStr)
		{
			if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CParaView)))
			{
				PopupMenu.CreatePopupMenu();
				info.LoadString( IDS_READ_SETPOINT );
				PopupMenu.AppendMenu(MF_ENABLED, IDM_READ_PARA_DATA, info);
				info.LoadString( IDS_EXPORT_PARA );
				PopupMenu.AppendMenu(MF_ENABLED, IDM_OUT_PARA_DATA_TO_EXCEL, info);
				info.LoadString( IDS_IMPORT_SETPOINT );
				PopupMenu.AppendMenu(MF_ENABLED, IDM_IN_PARA_FILE, info );
				PopupMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON , point.x, point.y, this);
			}
		}
	}
	else if(strParent== waveFileStr)
	{
		PopupMenu.CreatePopupMenu();
		int nSelectCount=m_wndTree.GetSelectedCount();
		if(nSelectCount == 1)
		{
			info.LoadString( IDS_SHOW_WAVE );
			PopupMenu.AppendMenu(MF_ENABLED, IDM_SHOW_CURVE, info);
			if(theApp.m_mapDiviceType.find(theApp.m_projectData.m_strDeviceType)->second != "CFrame_NoAnalyse")
			{
				//PopupMenu.AppendMenu(MF_ENABLED, IDM_CURVE_ANALYSE, _T("录波分析"));
				//PopupMenu.AppendMenu(MF_ENABLED, IDM_CURVE_UNITE, _T("波形合并"));
			}
//			PopupMenu.AppendMenu(MF_ENABLED, IDM_OUT_TO_COMTRADE, _T("按Comtrade文件导出"));
		}
		info.LoadString( IDS_DEL_LIST );
		PopupMenu.AppendMenu(MF_ENABLED, IDM_DELETE_OUT_TO_LIST, info);
		info.LoadString( IDS_DEL_DISK );
		PopupMenu.AppendMenu(MF_ENABLED, IDM_DELETE_FILE, info);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON , point.x, point.y, this);
		HTREEITEM paraItem=m_wndTree.GetParentItem(hItem);
		HTREEITEM tempItem=m_wndTree.GetNextItem(paraItem,TVGN_CHILD);
		int i=1;
		while((tempItem != NULL) && (tempItem != hItem))
		{
			i++;
			tempItem=m_wndTree.GetNextSiblingItem(tempItem);
		}
		if(tempItem == NULL)
		{
			m_nRClickWareFileIndex = -1;
		}
		else
		{
			m_nRClickWareFileIndex = theApp.m_projectData.m_nFileCount-i;
		}
	}
	else if(strParent==_T("") && strSelect ==theApp.m_projectData.m_strProjectName)
	{
		PopupMenu.CreatePopupMenu();
		info.LoadString( IDS_PRJ_SETTING );
		PopupMenu.AppendMenu(MF_ENABLED, IDM_PROJECT_SET, info );
		info.LoadString( IDS_TEST_CONNECT );
		PopupMenu.AppendMenu(MF_ENABLED, IDM_DEVICE_CONNECT_TEST, info );
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON , point.x, point.y, this);
	}
	*pResult = 0;
}

/**************************************************************
  函数：OnShowEvent
  描述；显示事件面板视图
  参数：
  返回：
  修改记录：
**************************************************************/
void CMainFrame::OnShowEvent() 
{
	// TODO: Add your command handler code here
	ReplaceView(RUNTIME_CLASS(CEventView));
	
}

/**************************************************************
  函数：OnGetEven
  描述；从装置中获取事件信息
  参数：
  返回：
  修改记录：
**************************************************************/
void CMainFrame::OnGetEvent() 
{
	// TODO: Add your command handler code here
	if(!TestConnection()) return;
	CView * pView=GetActiveView();
	if(!pView->IsKindOf(RUNTIME_CLASS(CEventView)))
	{
		ReplaceView(RUNTIME_CLASS(CEventView));
	}
	pView=GetActiveView();
	((CEventView *)pView)->GetDeviceEvent();
}

/**************************************************************
  函数：OnSetProject
  描述；工程设置，这里只能修改与装置通信的IP地址
  参数：
  返回：
  修改记录：
**************************************************************/
void CMainFrame::OnSetProject()
{
	CSetProjectDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		if(!theApp.m_projectData.SaveProjectSetting())
		{
			CString str;
			str.LoadString( IDS_PRJFILE_SAVE_ERROR );
			CdMessageBox box( this, str);
	        box.MsgBoxEx();
		}
	}
}

/**************************************************************
  函数：OnTestDeviceConnection
  描述；点击测试装置连接时触发的事件
  参数：
  返回：
  修改记录：
**************************************************************/
void CMainFrame::OnTestDeviceConnection()
{
	if(TestConnection())
	{
		CString info;
		info.LoadString( IDS_CONNECT_OK );
		CdMessageBox box(this, info);
	 	box.MsgBoxEx();
		CToolBarCtrl & toolBarCtrl = m_wndMainToolBar.GetToolBarCtrl();
		toolBarCtrl.EnableButton(ID_DISCONNECT,TRUE);
		toolBarCtrl.EnableButton(ID_CONNECT,FALSE);
		GetActiveView()->GetDocument()->SetTitle((CString)theApp.m_projectData.m_strDeviceType);
		theApp.m_projectData.SaveProjectSetting();
	}
}

/**************************************************************
  函数：OnDeleteFile
  描述；彻底删除录波文件
  参数：
  返回：
  修改记录：
**************************************************************/
void CMainFrame::OnDeleteFile()
{
	
	CString info;
	info.LoadString( IDS_DEL_WAVEFILE );
	CdMessageBox box(this, info, _T(""), MB_YESNO);
	if(IDNO == box.MsgBoxEx() )
	{
		return;
	}
	CString strFilePath;
	DWORD dwIndexOfFileCount;
	HTREEITEM hItem = m_wndTree.GetFirstSelectedItem();
	while(hItem)
	{
		dwIndexOfFileCount = m_wndTree.GetItemData(hItem)-1;
		strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,
			(CString)theApp.m_projectData.m_arrfileTreeNode[dwIndexOfFileCount].strFileName);
		if(!theApp.m_projectData.DeleteWareFileOutToProject(dwIndexOfFileCount))
		{
			info.LoadString( IDS_DELFILE_ERROR );
			CdMessageBox box(this, info);
	 	    box.MsgBoxEx();
			break;
		}
		DeleteFile(strFilePath);
		hItem= m_wndTree.GetNextSelectedItem(hItem);
	}
	LoadProjectData();
	info.LoadString( IDS_WAVE_FILE );
	ExpendTreeItem( info );
}
/**************************************************************
  函数：OnDeleteOutToList
  描述；删除录波文件列表
  参数：
  返回：
  修改记录：
**************************************************************/
void CMainFrame::OnDeleteOutToList()
{
	CString info;
	info.LoadString( IDS_DELWAVE_INFO2 );
	CdMessageBox box(this, info, _T(""), MB_YESNO);
	if(IDNO == box.MsgBoxEx() )
	{
		return;
	}
	CString strFilePath;
	DWORD dwIndexOfFileCount;
	HTREEITEM hItem = m_wndTree.GetFirstSelectedItem();
	while(hItem)
	{
		dwIndexOfFileCount = m_wndTree.GetItemData(hItem)-1;
		strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,
			(CString)theApp.m_projectData.m_arrfileTreeNode[dwIndexOfFileCount].strFileName);
		if(!theApp.m_projectData.DeleteWareFileOutToProject(dwIndexOfFileCount))
		{
			info.LoadString( IDS_DELFILE_ERROR );
			CdMessageBox box(this, info);
	 	    box.MsgBoxEx();
			break;
		}
		info.LoadString( IDS_WAVE_FILE );
		ExpendTreeItem( info );
		hItem= m_wndTree.GetNextSelectedItem(hItem);
	}
	LoadProjectData();
	info.LoadString( IDS_WAVE_FILE );
	ExpendTreeItem( info );	
	
}
/**************************************************************
  函数：OnShowCurve
  描述；显示录波曲线信息
  参数：无
  返回：无
  修改记录：
**************************************************************/
void CMainFrame::OnShowCurve()
{
	ReleaseProject();
	ShowControlBar(&m_wndBottonBar, TRUE, FALSE);
	if(m_nRClickWareFileIndex < 0 || m_nRClickWareFileIndex >= theApp.m_projectData.m_nFileCount)
	{
		return;
	}
	m_wareFileData.Release();
	CString strFilePath,strFileInfo;
	strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,
		(CString)theApp.m_projectData.m_arrfileTreeNode[m_nRClickWareFileIndex].strFileName);
	CString info;
	if ( TranErrorInfo( m_wareFileData.OpenFile(strFilePath) ) )
	{
		return;
	}

	CString strProParaType=_T(""),str;
	str.Format(_T("%.1f"),m_wareFileData.m_nCycleCount);
	if(m_wareFileData.m_isUnitedFile&&m_wareFileData.m_diffMilliSecond<=1500)
	{
		info.LoadString( IDS_BOTTOM_FIRST );
		strFileInfo.Format( info,
		(CString)m_wareFileData.m_strFileHead,(CString)m_wareFileData.m_strStartType,(1500+m_wareFileData.m_diffMilliSecond)/20,
		m_wareFileData.m_FirstSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfFirst,
		m_wareFileData.m_StartSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfStart);
	}
	else
	{
		info.LoadString( IDS_BOTTOM_FIRST_F2 );
		strFileInfo.Format( info , 
		(CString)m_wareFileData.m_strFileHead,(CString)m_wareFileData.m_strStartType,m_wareFileData.m_nCycleCount,
	 	m_wareFileData.m_FirstSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfFirst,
		 m_wareFileData.m_StartSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfStart,
		 m_wareFileData.m_ReturnSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfReturn);
	}
	//2014-8-18
	if ( FALSE == m_wareFileData.m_fileOk )
	{
		info.LoadString( IDS_FILEFLAG_ERROR );
		CdMessageBox box( this, str);
	    box.MsgBoxEx();
		strFileInfo+= info;
	}
	m_editWareFileInfo.SetWindowText(strFileInfo);
	int i,j,count;
	//strFileInfo = _T("保护定值：\r\n");
    //lhz modiy 2013-4-8 内存泄露问题,放在初始化赋字体
// 	CFont f;
// 	f = new CFont;
// 	m_editFont.CreateFont(15, // nHeight 
// 		0, // nWidth 
// 		0, // nEscapement 
// 		0, // nOrientation 
// 		FW_NORMAL, // nWeight 
// 		FALSE, // bItalic 
// 		FALSE, // bUnderline 
// 		0, // cStrikeOut 
// 		ANSI_CHARSET, // nCharSet 
// 		OUT_DEFAULT_PRECIS, // nOutPrecision 
// 		CLIP_DEFAULT_PRECIS, // nClipPrecision 
// 		DEFAULT_QUALITY, // nQuality 
// 		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
// 		_T("宋体")); // lpszFac 
//  m_editWareFileInfo.SetFont(f);
/*	m_editWareFileInfo.SetFont(&m_editFont);*/
    
//	delete f;
	int indexOfChannel=0,nPointInCircle,nRowOfPara=0,nRowOfWareEvent=0;
	float nStartCircleCount,nReturnCircleCount;
	ProtectPara * pProtectPara;
	CEventDescribe*pCEventDescribe;
	for(i=0;i<m_wareFileData.m_nSubItemCount;i++)
	{
		if ( !m_wareFileData.m_arrPSubItem[i] )
		{
			continue;
		}
		switch(m_wareFileData.m_arrPSubItem[i]->GetDataType())
		{
		case PROTECT_PARA_TYPE:  
			//当子项为保护定值时，指定通道总数的意义变成了返回装置所运行的定值区
			info.LoadString( IDS_ARARE_WAVE );
		    str.Format(info,
				m_wareFileData.m_arrPSubItem[i]->GetAppointCount());
			m_listParaInfo.InsertItem(nRowOfPara++,str);
			count = m_wareFileData.m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				pProtectPara = (ProtectPara *)m_wareFileData.m_arrPSubItem[i]->GetChannelData(j);
				ASSERT(pProtectPara);
				if(strProParaType != pProtectPara->strParaType)
				{
					strProParaType = pProtectPara->strParaType;
					m_listParaInfo.InsertItem(nRowOfPara,strProParaType);
				}
				else
				{
					m_listParaInfo.InsertItem(nRowOfPara,_T(""));
				}
				if ( (CString)pProtectPara->strParaName == _T("断路器合闸时间") || _T("Breaker Closing Time") == (CString)pProtectPara->strParaName  )
				{//
					sscanf(pProtectPara->strParaValue,"%dms",&m_CloseTimeSet);
				}
				if ((CString)pProtectPara->strParaName == _T("系统侧应转角") || _T("Busbar Turn Angle") == (CString)pProtectPara->strParaName)
				{
					sscanf(pProtectPara->strParaValue,"%d.0°",&m_SystemAngleSet);
				}
				if ((CString)pProtectPara->strParaName == _T("系统侧额定电压") || _T("Busbar Rated Volt.") == (CString)pProtectPara->strParaName )
				{
					sscanf(pProtectPara->strParaValue,"%f°",&m_fSVolSet);
				}
				if ((CString)pProtectPara->strParaName == _T("待并侧额定电压") || _T("Incomer Rated Volt.") == (CString)pProtectPara->strParaName )
				{
					sscanf(pProtectPara->strParaValue,"%f°",&m_fGVolSet);
				}
				m_listParaInfo.SetItemText(nRowOfPara,1,(CString)pProtectPara->strParaName);
				m_listParaInfo.SetItemText(nRowOfPara++,2,(CString)pProtectPara->strParaValue);
			}
			break;
		case ANALOG_TYPE:           //通道信息
		case DIGITAL_IN_TYPE:
		case DIGITAL_OUT_TYPE:
		case FREQUENCY_TYPE:
		case ANGLE_DIFF_TYPE:
			count = m_wareFileData.m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				if(m_wareFileData.m_arrPSubItem[i]->GetChannelName(j,str) &&
					(nStartCircleCount=m_wareFileData.m_arrPSubItem[i]->GetStartCircleCount()) != -1&&
					(nReturnCircleCount=m_wareFileData.m_arrPSubItem[i]->GetReturnCircleCount()) != -1 &&
					(nPointInCircle=m_wareFileData.m_arrPSubItem[i]->GetPointCountInCircle()) != -1)
				{
					m_listChannelInfo.InsertItem(/*LVIF_TEXT|LVIF_IMAGE,*/indexOfChannel,str/*,0,0,-1,NULL*/);
					switch(m_wareFileData.m_arrPSubItem[i]->GetDataType())
					{
						case ANALOG_TYPE:
							info.LoadString( IDS_STR_ANALOG );
							break;
						case DIGITAL_IN_TYPE:
							info.LoadString( IDS_DIGITAL_INPUT );
							break;
						case DIGITAL_OUT_TYPE:
							info.LoadString( IDS_DIGITAL_OUTPUT );
							break;
						case FREQUENCY_TYPE:
							info.LoadString( IDS_FREQUENCY );
							break;
						case ANGLE_DIFF_TYPE:
							info.LoadString( IDS_ANG_DIFF2 );
							break;
						default:
							info = _T(" ");
					}
					m_listChannelInfo.SetItemText(indexOfChannel,1, info);
					str.Format(_T("%d"),nPointInCircle);
					m_listChannelInfo.SetItemText(indexOfChannel,2,str);
					str.Format(_T("%.1f"),nStartCircleCount);
					m_listChannelInfo.SetItemText(indexOfChannel,3,str);
					str.Format(_T("%.1f"),nReturnCircleCount);
					m_listChannelInfo.SetItemText(indexOfChannel,4,str);
					indexOfChannel++;
				}
			}
			break;
			//2016-10-20 pwq
		case WARE_EVENT_TYPE:
			{
				count = m_wareFileData.m_arrPSubItem[i]->GetChannelCount();
				if (count < 0)
				{
					return;
				}
				for(j=0;j<count;j++)
				{
					pCEventDescribe = (CEventDescribe *)m_wareFileData.m_arrPSubItem[i]->GetChannelData(j);
					ASSERT(pCEventDescribe);
					CString temp;
					temp = pCEventDescribe->name;
					m_listWareEvent.InsertItem(nRowOfWareEvent, temp);
					temp.Format(_T("%s.%d"), /*(CString)(*/pCEventDescribe->eventTime.Format(_T("%Y-%m-%d %H:%M:%S")), 
						pCEventDescribe->nMillisecond/*, pCEventDescribe->nMstick*/);
					m_listWareEvent.SetItemText(nRowOfWareEvent, 1, temp);
					temp = pCEventDescribe->m_strRemark;
					if (temp.IsEmpty())
					{
						temp.LoadString( IDS_NO_REMARK );
					}
					m_listWareEvent.SetItemText(nRowOfWareEvent, 2, temp);	
					CString typeStr;
					if (LB_EVENT_TYPE == pCEventDescribe->type)
					{
						typeStr.LoadString( IDS_WAVE_EVENT );
					}
					else if (KZ_EVENT_TYPE == pCEventDescribe->type)
					{
						typeStr.LoadString( IDS_CONTROL_EVENT );
					}
					else if( SVERROR_EVENT_TYPE == pCEventDescribe->type )
					{
						typeStr.LoadString( IDS_SVERROR_EVENT );
					}else
					{
						typeStr.LoadString( IDS_UNKNOWTYPE_EVENT );
					}
					m_listWareEvent.SetItemText(nRowOfWareEvent,3,typeStr);
					++nRowOfWareEvent;
				}
			}
			break;
		}
	}
	
	CCustomizeWareSetDlg dlg(&m_wareFileData,FALSE); //2fy不显示通道选择
	
	if(IDOK == dlg.DoModal())
	{
		ReplaceView(RUNTIME_CLASS(CCurveView));
		if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CCurveView)))
		{
			CCurveView * pView=(CCurveView *)GetActiveView();
			CCurveView::m_isFirstShow = TRUE;
			pView->ChangeCurve(m_wareFileData.m_pCurve, m_wareFileData.m_nCurveCount, 
				        (bool)(m_wareFileData.m_isUnitedFile), m_wareFileData.m_diffMilliSecond,dlg.m_cPS);
			m_nHMulti = 1;
			m_nVMulti = 1;
		}
	}
 	else
	{
		ReleaseProject();
	}
	
	//显示曲线
}

/***********************************************************
功能：更新视图
输入：pViewClass；
输出：
***********************************************************/
BOOL CMainFrame::ReplaceView(CRuntimeClass *pViewClass)
{
	CCreateContext context;
	CDocument *pDoc = this->GetActiveDocument();
    CView *pActiveView = this->GetActiveView();
    pDoc->m_bAutoDelete = FALSE; //设置标志，这样当视图销毁时不会删除文档
    this->GetActiveView()->DestroyWindow();
    pDoc->m_bAutoDelete = TRUE; //设回默认的标志
	

    //创建新视图
    context.m_pNewViewClass = pViewClass;
    context.m_pCurrentDoc = pDoc;
    context.m_pNewDocTemplate = NULL;
    context.m_pLastView = NULL;
    context.m_pCurrentFrame = NULL;
    pActiveView=(CView *)CreateView(&context);
	SetActiveView(pActiveView);
	pActiveView->SendMessage(WM_INITIALUPDATE,0,0);
	RecalcLayout();
	CToolBarCtrl & toolBarCtrl = m_wndToolBar.GetToolBarCtrl();
	CToolBarCtrl & mainTool = m_wndMainToolBar.GetToolBarCtrl();
	
	if(!(pActiveView->IsKindOf(RUNTIME_CLASS(CCurveView))))
	{
		ShowControlBar(&m_wndBottonBar, FALSE, FALSE); //录波界面显示底部的信息视图
	}

	if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CCurveView)))
	{
		
		//toolBarCtrl.EnableButton(IDM_RIGHT_BAR,FALSE);
		toolBarCtrl.EnableButton(ID_CURVE_H_BIGGER,TRUE);
		toolBarCtrl.EnableButton(ID_CURVE_V_BIGGER,TRUE);
		m_wndStatusBar.SetPaneStyle(1,SBPS_NORMAL );
		m_wndStatusBar.SetPaneStyle(2,SBPS_NORMAL );
	}
	else
	{
		//toolBarCtrl.EnableButton(IDM_RIGHT_BAR,FALSE);
		toolBarCtrl.EnableButton(ID_CURVE_H_BIGGER,FALSE);
		toolBarCtrl.EnableButton(ID_CURVE_V_BIGGER,FALSE);
		toolBarCtrl.EnableButton(ID_CURVE_H_SMALLER,FALSE);
		toolBarCtrl.EnableButton(ID_CURVE_V_SMALLER,FALSE);
		m_wndStatusBar.SetPaneStyle(1,SBPS_DISABLED );
		m_wndStatusBar.SetPaneStyle(2,SBPS_DISABLED );
	}
	if(!(pActiveView->IsKindOf(RUNTIME_CLASS(CRealMonitorView))))
	{
		ShowControlBar(&m_wndRightBar, FALSE, FALSE);
		mainTool.EnableButton(IDM_RIGHT_BAR,FALSE); 
		mainTool.EnableButton(IDM_UPDATE_PARA,FALSE); 
	}
	else
	{
		ShowControlBar(&m_wndRightBar, TRUE, TRUE);
		mainTool.EnableButton(IDM_RIGHT_BAR,TRUE); 
		mainTool.EnableButton(IDM_UPDATE_PARA,TRUE); 
		m_wndStatusBar.SetPaneStyle(1,SBPS_NORMAL );
		m_wndStatusBar.SetPaneStyle(2,SBPS_NORMAL );
	}
	return TRUE;
}

/****************************************************************
函数名：OnSelchanged
功能：点击工程视图区中的树点结时触发
输入：
返回：空
****************************************************************/
void CMainFrame::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CString strParent,strSelect;
	strParent=m_wndTree.GetItemText(m_wndTree.GetParentItem(pNMTreeView->itemNew.hItem));
	strSelect=m_wndTree.GetItemText(pNMTreeView->itemNew.hItem);
	CString eventInfoStr, realDataStr, wavefileStr, paraStr, synStr;
	eventInfoStr.LoadString( IDS_EVENT_INFO );
    realDataStr.LoadString( IDS_REAL_PARA );
	wavefileStr.LoadString( IDS_WAVE_FILE );
	paraStr.LoadString( IDS_PROTECT_PARA );
    synStr.LoadString( IDS_RUN_WATCH );
	if(strParent==theApp.m_projectData.m_strProjectName)
	{
		if(strSelect== eventInfoStr)
		{
			OnShowEvent();
		}
		else if(strSelect == wavefileStr)
		{
			//OnShowCurve();
		}
		else if(strSelect == realDataStr)
		{
			if(TestConnection())
			{
				ReplaceView(RUNTIME_CLASS(CRealDataView));
			}
		}
		else if(strSelect == synStr )
		{
			if(TestConnection())
			{
				ReplaceView(RUNTIME_CLASS(CRealMonitorView));
				ShowControlBar(&m_wndLeftBar, FALSE, FALSE);
				ShowControlBar(&m_wndRightBar,TRUE, TRUE);
			}
		}
		else if(strSelect == paraStr )
		{
			ReplaceView(RUNTIME_CLASS(CParaView));        //
		}
	}
	else if(strParent == wavefileStr )
	{
		int i=1;
		HTREEITEM paraItem=m_wndTree.GetParentItem(pNMTreeView->itemNew.hItem);
		HTREEITEM tempItem=m_wndTree.GetNextItem(paraItem,TVGN_CHILD);
		while((tempItem != NULL) && (tempItem != pNMTreeView->itemNew.hItem))
		{
			i++;
			tempItem=m_wndTree.GetNextSiblingItem(tempItem);
		}
		if(tempItem == NULL)
		{
			m_nRClickWareFileIndex = -1;
		}
		else
		{
			m_nRClickWareFileIndex = theApp.m_projectData.m_nFileCount-i;
			CString strStatus;
			strStatus.Format(_T("%s:%s"),(CString)theApp.m_projectData.m_arrfileTreeNode[m_nRClickWareFileIndex].strWareType,
				theApp.m_projectData.m_arrfileTreeNode[m_nRClickWareFileIndex].dtStartTime.Format(_T("%Y-%m-%d %H:%M:%S")));
			m_wndStatusBar.SetPaneText(0,strStatus);
		}
	}
	*pResult = 0;
}
/****************************************************************
函数名：OnGetWareFile
功能：提取录波文件
输入：
返回：空
****************************************************************/
void CMainFrame::OnGetWareFile() 
{
	// TODO: Add your command handler code here
	if(!TestConnection())  return;  
	CString str;
	if(theApp.m_projectData.m_nFileCount >= MAX_FILE_COUNT)
	{
		CString info;
		info.LoadString( IDS_MAXDOWN_FILENUM );
		str.Format( info, MAX_FILE_COUNT, theApp.m_projectData.m_nFileCount );
		CdMessageBox box(this, str);
		return;
	}
	CDownloadSetDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		LoadProjectData();
		str.LoadString( IDS_WAVE_FILE );
		ExpendTreeItem( str );
	}
}

void CMainFrame::OnOpenWareFile() 
{
	// TODO: Add your command handler code here
	CString str, info;
	if(theApp.m_projectData.m_nFileCount >= MAX_FILE_COUNT)
	{
		info.LoadString( IDS_MAX_FILENUM );
		str.Format( info , MAX_FILE_COUNT, theApp.m_projectData.m_nFileCount);
		CdMessageBox box(this, str);
	 	box.MsgBoxEx();
		return;
	}
	CString	filename[20];
	POSITION	AA;
	unsigned short dialogFileName[2000];
	memset(dialogFileName,0,sizeof(dialogFileName));
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ENABLEHOOK|OFN_ALLOWMULTISELECT,_T("wave files (*.wave)|*wave||"));
	dlg.m_ofn.lpstrFile=dialogFileName;
	dlg.m_ofn.nMaxFile=2000;
	if(IDOK==dlg.DoModal())
	{
		int i=0;
		AA=dlg.GetStartPosition();
		filename[i]=dlg.GetNextPathName(AA);
		CString path,strFileName;
		path=filename[i].Left(filename[i].ReverseFind(TEXT('\\'))+1);
		strFileName=filename[i].Right(filename[i].GetLength()-filename[i].ReverseFind(TEXT('\\'))-1);
		CString strProFilePath;
		strProFilePath.Format(_T("%s"),theApp.m_projectData.m_strWareFilePath);
		if(path == strProFilePath)
		{
			while(1)
			{	
				bool flag=false;
				int j;
				for(j=0;j<theApp.m_projectData.m_nFileCount;j++)
				{
					if(strFileName == theApp.m_projectData.m_arrfileTreeNode[j].strFileName)
					{
						flag=true;
						break;
					}
				}
				if(!flag)
				{
					if(!theApp.m_projectData.AddWareFileToProject(strFileName))
					{
						info.LoadString( IDS_FILE_BROKEN );
						CdMessageBox box(this, info);
	                 	box.MsgBoxEx();
					}
				}
				else
				{
					info.LoadString( IDS_STR_FILEEX );
					CdMessageBox box(this, info);
	 	            box.MsgBoxEx();
				}
				i++;
				if(AA==NULL)
					break;
				filename[i]=dlg.GetNextPathName(AA);
				strFileName=filename[i].Right(filename[i].GetLength()-filename[i].ReverseFind(TEXT('\\'))-1);
			}
		}
		else
		{
			while(1)
			{	
				CFileFind fileFind;
				int temp=1;
				strProFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,strFileName);
				//文件名相同时
// 				while(fileFind.FindFile(strProFilePath))
// 				{
// 					strFileName.Format(_T("%s_%d.wave"),strFileName.Left(strFileName.GetLength()-5),temp);
// 					strProFilePath.Format(_T("%s_%d.wave"),strProFilePath.Left(strProFilePath.GetLength()-5),temp);
// 					temp++;
// 				}
				if (fileFind.FindFile(strProFilePath))
				{
					info.LoadString( IDS_STR_FILEEX );
					CdMessageBox box(this, info);
	 	            box.MsgBoxEx();
					return;
				}
				FILE * wareFile;
				wareFile = _wfopen(filename[i],_T("rb"));
				if(wareFile == NULL)
				{
					info.LoadString( IDS_STR_OPENE );
					CdMessageBox box(this, info);
	 	            box.MsgBoxEx();
					return;
				}
				fseek(wareFile,0,SEEK_END);
				int filelength=ftell(wareFile);
				BYTE *fileBuf=new BYTE[filelength+10];
				memset(fileBuf,0,filelength+10);
				fseek(wareFile,0,SEEK_SET);
				temp=fread(fileBuf,sizeof(BYTE),filelength+10,wareFile);
				fclose(wareFile);
				ASSERT(filelength==temp);
				wareFile = _wfopen(strProFilePath,_T("wb"));
				if(wareFile == NULL)
				{
					info.LoadString( IDS_STR_OPENE );
					CdMessageBox box(this, info);
					box.MsgBoxEx();
					delete [] fileBuf;
					return;
				}
				if(filelength != fwrite(fileBuf,sizeof(BYTE),filelength,wareFile))
				{
					info.LoadString( IDS_STR_CPE ); 
					CdMessageBox box(this, info);
					box.MsgBoxEx();
					fclose(wareFile);
					delete [] fileBuf;
					return;
				}
				delete [] fileBuf;
				fclose(wareFile);
				if(!theApp.m_projectData.AddWareFileToProject(strFileName))
				{
					::DeleteFile(strProFilePath);
				}
				i++;
				if(AA==NULL)
					break;
				filename[i]=dlg.GetNextPathName(AA);
				strFileName=filename[i].Right(filename[i].GetLength()-filename[i].ReverseFind(TEXT('\\'))-1);
			}
		}
		LoadProjectData();
		info.LoadString( IDS_WAVE_FILE );
		ExpendTreeItem( info );
	}

}

/****************************************************************
函数名：OnCurveHBigger
功能：水平放大波形曲线
输入：
返回：空
****************************************************************/
void CMainFrame::OnCurveHBigger() 
{
	// TODO: Add your command handler code here
	CToolBarCtrl & toolBarCtrl = m_wndToolBar.GetToolBarCtrl();
	if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CCurveView)))
	{
		CCurveView * pView = (CCurveView *)GetActiveView();
		if(m_nHMulti<MAX_H_MULTI_NUM)
		{
			if(m_nHMulti == 1)
			{
				toolBarCtrl.EnableButton(ID_CURVE_H_SMALLER,TRUE);
			}
			if(m_nHMulti+1<= MAX_H_MULTI_NUM)
			{
				m_nHMulti ++;
			}
			pView->m_dNewMulti = pView->m_dMinMulti*(1+(m_nHMulti-1)*m_nMultiFactor);
			::RedrawWindow(pView->m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
			if(m_nHMulti ==MAX_H_MULTI_NUM)
			{
				toolBarCtrl.EnableButton(ID_CURVE_H_BIGGER,FALSE);
			}
		}
		else
		{
			toolBarCtrl.EnableButton(ID_CURVE_H_BIGGER,FALSE);
		}
	}
	
}

/****************************************************************
函数名：OnCurveHSmaller
功能：水平缩小波形曲线
输入：
返回：空
****************************************************************/
void CMainFrame::OnCurveHSmaller() 
{
	// TODO: Add your command handler code here
	CToolBarCtrl & toolBarCtrl = m_wndToolBar.GetToolBarCtrl();
	if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CCurveView)))
	{
		CCurveView * pView = (CCurveView *)GetActiveView();
		if(m_nHMulti>1)
		{
			if(m_nHMulti == MAX_H_MULTI_NUM)
			{
				toolBarCtrl.EnableButton(ID_CURVE_H_BIGGER,TRUE);
			}
			if (m_nHMulti-1 >= 1)
			{
				m_nHMulti--;
			}
			
			pView->m_dNewMulti = pView->m_dMinMulti*(1+(m_nHMulti-1)*m_nMultiFactor);
			::RedrawWindow(pView->m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
			if(m_nHMulti ==1)
			{
				toolBarCtrl.EnableButton(ID_CURVE_H_SMALLER,FALSE);
			}
		}
		else
		{
			toolBarCtrl.EnableButton(ID_CURVE_H_SMALLER,FALSE);
		}
	}
}

/****************************************************************
函数名：OnCurveVBigger
功能：垂直放大波形曲线
输入：
返回：空
****************************************************************/
void CMainFrame::OnCurveVBigger() 
{
	// TODO: Add your command handler code here
	
	CToolBarCtrl & toolBarCtrl = m_wndToolBar.GetToolBarCtrl();
	if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CCurveView)))
	{
		CCurveView * pView = (CCurveView *)GetActiveView();
		if(m_nVMulti<5)
		{
			if(m_nVMulti == 1)
			{
				toolBarCtrl.EnableButton(ID_CURVE_V_SMALLER,TRUE);
			}
			if(m_nVMulti + m_nMultiFactor<= 5)
			{
				m_nVMulti += m_nMultiFactor;
			}
			if(!toolBarCtrl.IsButtonEnabled(ID_CURVE_V_SMALLER))
			{
				toolBarCtrl.EnableButton(ID_CURVE_V_SMALLER,TRUE);
			}
			switch(m_nVMulti)
			{
			case 1:
				pView->SetCoorHeight(20,60);
				break;
			case 2:
				pView->SetCoorHeight(40,80);
				break;
			case 3:
				pView->SetCoorHeight(40,100);
				break;
			case 4:
				pView->SetCoorHeight(40,120);
				break;
			case 5:
				pView->SetCoorHeight(40,160);
				break;
			}
			::RedrawWindow(pView->m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
			if(m_nVMulti == 5)
			{
				toolBarCtrl.EnableButton(ID_CURVE_V_BIGGER,FALSE);
			}
		}
		else
		{
			toolBarCtrl.EnableButton(ID_CURVE_V_BIGGER,FALSE);
		}
	}
}

/****************************************************************
函数名：OnCurveVSmaller
功能：垂直缩小波形曲线
输入：
返回：空
****************************************************************/
void CMainFrame::OnCurveVSmaller() 
{
	// TODO: Add your command handler code here
	
	CToolBarCtrl & toolBarCtrl = m_wndToolBar.GetToolBarCtrl();
	if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CCurveView)))
	{
		CCurveView * pView = (CCurveView *)GetActiveView();
		if(m_nVMulti>1)
		{
			if(m_nVMulti == 5)
			{
				toolBarCtrl.EnableButton(ID_CURVE_V_BIGGER,TRUE);
			}
			if (m_nVMulti - m_nMultiFactor>=1)
			{
				m_nVMulti -= m_nMultiFactor;
			}		
			switch(m_nVMulti)
			{
			case 1:
				pView->SetCoorHeight(20,60);
				break;
			case 2:
				pView->SetCoorHeight(40,80);
				break;
			case 3:
				pView->SetCoorHeight(40,100);
				break;
			case 4:
				pView->SetCoorHeight(40,120);
				break;
			case 5:
				pView->SetCoorHeight(40,160);
				break;
			}
			::RedrawWindow(pView->m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
			if(m_nVMulti == 1)
			{
				toolBarCtrl.EnableButton(ID_CURVE_V_SMALLER,FALSE);
			}
		}
		else
		{
			toolBarCtrl.EnableButton(ID_CURVE_V_SMALLER,FALSE);
		}
	}
}

BOOL CMainFrame::TranErrorInfo(int errorId)
{
	CString info;
	switch(errorId)
	{
	case OPEN_WARE_FILE_FILE_LENGTH_TOO_LONG:
		info.LoadString( IDS_WAREFILE_INFO3 );
		break;
	case OPEN_WARE_FILE_NOT_EXIST:
		{
			info.LoadString( IDS_WAREFILE_INFO4 );
			CdMessageBox box(this, info);
			box.MsgBoxEx();
			OnDeleteOutToList();
			break;
		}
	case OPEN_WARE_FILE_OPEN_FAILED:
		info.LoadString( IDS_OPEN_WAVEF );
		break;
	case OPEN_WARE_FILE_GET_INFO_FIALED:
		info.LoadString( IDS_SPLIT_ERROR );
		break;
	case OPEN_WARE_FILE_SUCESS:
	default:
		break;
	}
	if (OPEN_WARE_FILE_SUCESS != errorId && OPEN_WARE_FILE_NOT_EXIST != errorId )
	{
		CdMessageBox box(this, info);
		box.MsgBoxEx();
	}
	if (info.GetLength() > 0)
	{
		
		return TRUE;
	}
	return FALSE;
}

/****************************************************************
函数名：OnCurveAnalyse
功能：波形分析
输入：
返回：空
****************************************************************/
// void CMainFrame::OnCurveAnalyse()
// {
// 	ReleaseProject();
// 	if(m_nRClickWareFileIndex < 0 || m_nRClickWareFileIndex >= theApp.m_projectData.m_nFileCount)
// 	{
// 		return;
// 	}
// 	m_wareFileData.Release();
// 	CString strFileInfo,strFilePath;
// 	strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,
// 		(CString)theApp.m_projectData.m_arrfileTreeNode[m_nRClickWareFileIndex].strFileName);
// 	if ( TranErrorInfo( m_wareFileData.OpenFile(strFilePath) ) )
// 	{
// 		return;
// 	}
// 	if(m_wareFileData.m_isUnitedFile&&m_wareFileData.m_diffMilliSecond<=1500)
// 	{
// 		strFileInfo.Format(_T("录波文件信息：\r\n文件来源：%s\r\n录波启动类型：%s\r\n采样总周波数：%.1f\r\n第一个采样点时间：%s.%d\r\n录波启动时间：%s.%d"),
// 			(CString)m_wareFileData.m_strFileHead,(CString)m_wareFileData.m_strStartType,(1500+m_wareFileData.m_diffMilliSecond)/20,
// 			m_wareFileData.m_FirstSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfFirst,
// 			m_wareFileData.m_StartSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfStart);
// 	}
// 	else
// 	{
// 		strFileInfo.Format(_T("录波文件信息：\r\n文件来源：%s\r\n录波启动类型：%s\r\n采样总周波数：%.1f\r\n第一个采样点时间：%s.%d\r\n录波启动时间：%s.%d\r\n返回段采样启动时间：%s.%d"),
// 			(CString)m_wareFileData.m_strFileHead,(CString)m_wareFileData.m_strStartType,m_wareFileData.m_nCycleCount,
// 			m_wareFileData.m_FirstSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfFirst,
// 			m_wareFileData.m_StartSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfStart,
// 			m_wareFileData.m_ReturnSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_wareFileData.m_milliSecondOfReturn);
// 	}
// 	m_editWareFileInfo.SetWindowText(strFileInfo);
// 	int i,j,count;
// 	CString strProParaType=_T(""),str;
// 	strFileInfo = _T("保护定值：\r\n");
// 	CFont * f; 
// 	f = new CFont; 
// 	f->CreateFont(15, // nHeight 
// 		0, // nWidth 
// 		0, // nEscapement 
// 		0, // nOrientation 
// 		FW_NORMAL, // nWeight 
// 		FALSE, // bItalic 
// 		FALSE, // bUnderline 
// 		0, // cStrikeOut 
// 		ANSI_CHARSET, // nCharSet 
// 		OUT_DEFAULT_PRECIS, // nOutPrecision 
// 		CLIP_DEFAULT_PRECIS, // nClipPrecision 
// 		DEFAULT_QUALITY, // nQuality 
// 		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
// 		_T("宋体")); // lpszFac 
// 	m_editWareFileInfo.SetFont(f);
// 	int indexOfChannel=0,nPointInCircle,nRowOfPara=0;
// 	float nCircleCount;
// 	ProtectPara * pProtectPara;
// 	for(i=0;i<m_wareFileData.m_nSubItemCount;i++)
// 	{
// 		switch(m_wareFileData.m_arrPSubItem[i]->GetDataType())
// 		{
// 		case PROTECT_PARA_TYPE:
// 			str.Format(_T("录波时装置运行在%d号定值区"),
// 				m_wareFileData.m_arrPSubItem[i]->GetAppointCount());
// 			m_listParaInfo.InsertItem(nRowOfPara++,str);
// 			count = m_wareFileData.m_arrPSubItem[i]->GetChannelCount();
// 			for(j=0;j<count;j++)
// 			{
// 				pProtectPara = (ProtectPara *)m_wareFileData.m_arrPSubItem[i]->GetChannelData(j);
// 				ASSERT(pProtectPara);
// 				if(strProParaType != pProtectPara->strParaType)
// 				{
// 					strProParaType = pProtectPara->strParaType;
// 					m_listParaInfo.InsertItem(nRowOfPara,strProParaType);
// 				}
// 				else
// 				{
// 					m_listParaInfo.InsertItem(nRowOfPara,_T(""));
// 				}
//              m_listParaInfo.SetItemText(nRowOfPara,1,(CString)pProtectPara->strParaName);
// 				m_listParaInfo.SetItemText(nRowOfPara++,2,(CString)pProtectPara->strParaValue);
// 			}
// 			break;
// 		case ANALOG_TYPE:
// 		case DIGITAL_IN_TYPE:
// 		case DIGITAL_OUT_TYPE:
// 			count = m_wareFileData.m_arrPSubItem[i]->GetChannelCount();
// 			for(j=0;j<count;j++)
// 			{
// 				if(m_wareFileData.m_arrPSubItem[i]->GetChannelName(j,str) &&
// 					(nCircleCount=m_wareFileData.m_arrPSubItem[i]->GetCircleCount()) != -1 &&
// 					(nPointInCircle=m_wareFileData.m_arrPSubItem[i]->GetPointCountInCircle(j)) != -1)
// 				{
// 					m_listChannelInfo.InsertItem(/*LVIF_TEXT|LVIF_IMAGE,*/indexOfChannel,str/*,0,0,-1,NULL*/);
// 					switch(m_wareFileData.m_arrPSubItem[i]->GetDataType())
// 					{
// 					case ANALOG_TYPE:
// 						m_listChannelInfo.SetItemText(indexOfChannel,1,_T("模拟量"));
// 						break;
// 					case DIGITAL_IN_TYPE:
// 						m_listChannelInfo.SetItemText(indexOfChannel,1,_T("开入量"));
// 						break;
// 					case DIGITAL_OUT_TYPE:
// 						m_listChannelInfo.SetItemText(indexOfChannel,1,_T("开出量"));
// 						break;
// 					}
// 					str.Format(_T("%d"),nCircleCount);
// 					m_listChannelInfo.SetItemText(indexOfChannel,2,str);
// 					str.Format(_T("%d"),nPointInCircle);
// 					m_listChannelInfo.SetItemText(indexOfChannel,3,str);
// 					indexOfChannel++;
// 				}
// 			}
// 			break;
// 		}
// 	}
// }

/*****************************************************************
    描述：用于展开工程视图中树状上含有strItemText文本的结点项，
	      如果没有找了，返回FASLE
    参数：
	    strItemText:           
	返回值：
	    TRUE      成功
		FALSE     失败
	修改日志：无
*****************************************************************/
BOOL CMainFrame::ExpendTreeItem(CString strItemText)
{
	HTREEITEM hRoot=m_wndTree.GetRootItem();
	m_wndTree.Expand(hRoot,TVE_EXPAND);
	HTREEITEM hChildItem= m_wndTree.GetNextItem(hRoot,TVGN_CHILD);
	ASSERT(hChildItem != NULL);
	CString str=m_wndTree.GetItemText(hChildItem);
	while(hChildItem != NULL && str != strItemText)
	{
		hChildItem = m_wndTree.GetNextSiblingItem(hChildItem);
		str=m_wndTree.GetItemText(hChildItem);
	}
	if(hChildItem == NULL)
	{
		return FALSE;
	}
	else
	{
		m_wndTree.Expand(hChildItem,TVE_EXPAND);
		return TRUE;
	}
}

void CMainFrame::OnOpenFile() 
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE,NULL,_T("*.prj"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("project files (*.prj)|*prj|wave files (*.wave)|*wave||"));
	if(IDOK==dlg.DoModal())
	{
		CString path=dlg.GetPathName();
		OpenNewFile(path);
	}
}

/***************************************************
功能：用于新工程的创建成功后，
      或重新打开别的工程时，析构原来已经打开的工程
****************************************************/
void CMainFrame::ReleaseProject()
{
	ReplaceView(RUNTIME_CLASS(CWareView));
	m_listParaInfo.DeleteAllItems();
	m_editWareFileInfo.SetWindowText(_T(""));
	m_listChannelInfo.DeleteAllItems();
	m_listWareEvent.DeleteAllItems();
}

/***************************************************
功能：显示或隐藏工具栏
****************************************************/
void CMainFrame::OnViewToolbar() 
{
	// TODO: Add your command handler code here
	CMenu *pMenu=GetMenu();
	if(MF_CHECKED & pMenu->GetMenuState(ID_VIEW_TOOLBAR,MF_BYCOMMAND))
	{
		pMenu->CheckMenuItem(IDC_PROJECT_WND,MF_UNCHECKED);
	}
	else
	{
		pMenu->CheckMenuItem(IDC_PROJECT_WND,MF_CHECKED);
	}
	ShowControlBar(&m_wndMainToolBar, !m_wndMainToolBar.IsVisible(), FALSE);
	ShowControlBar(&m_wndToolBar, !m_wndToolBar.IsVisible(), FALSE);
}



BOOL CMainFrame::GetExcelFile(const CString & strFileName,CComboBox * pComboBox,
							  CListCtrl * pList,BOOL bIsSaveSelected/*=FALSE*/,BOOL bAllInOneTable/*=FALSE*/)
{
	long i,j,colCount,nTypeCol;
	HDITEM hdi;   
	TCHAR lpBuffer[256];   
	bool fFound =false;
	hdi.mask=HDI_TEXT;   
	hdi.pszText=lpBuffer;   
	hdi.cchTextMax=256;   
	CFileFind fileFinder;
	CString info;
	if(fileFinder.FindFile(strFileName))
	{
		if(!DeleteFile(strFileName))
		{
			CString strDeleteFailed;
			info.LoadString( IDS_FILEUSED_INFO );
			strDeleteFailed.Format(info, strFileName);
			CdMessageBox box(theApp.m_pMainWnd, strDeleteFailed, _T(""), MB_OK);
	 	    box.MsgBoxEx();
			return FALSE;
		}
	}

	CAdoDatabase database;
	CAdoRecordset record;
	CString strConn;
	strConn.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Extended Properties=\"Excel 8.0;HDR=Yes;IMEX=0\";"),strFileName);
//	strConn.Format(_T("driver={sql server};server=SZIDD-RD-067;uid=sa;pwd=;database=master"));
	if(!database.Open(strConn,adModeUnknown))
	{
		return FALSE;
	}
	CString serialStr, typeStr ;//2014-11-13 add 序号字符串
	serialStr.LoadString( IDS_SERIAL );				//序号
    typeStr.LoadString( IDS_STRING_TYPE );			//类型

	CString sql,strSheetName,strItemText,strColName,str;
	map <CString,int> mapSheet;
	int index[100];
	long sheetCount = pComboBox->GetCount();				//获取定值类型组合框里面项的数目
	colCount = pList->GetHeaderCtrl()->GetItemCount();		//获取列表当前有多少列
	if (sheetCount == 0)
	{
		database.Close();
		CString str;
		str.LoadString( IDS_EXPORT_SETPOINT_EMPTY );
		CdMessageBox box(theApp.m_pMainWnd, str, _T(""), MB_OK);
	 	box.MsgBoxEx();
		return FALSE;
	}
	memset(index,0,sizeof(index));
	for(i=1;i<sheetCount;i++)					//决定要建多少个表
	{
		pComboBox->GetLBText(i,strSheetName);				
		strSheetName.Remove('-');//修复在英文版本时由与字符命名原因在导出定值或者实测数据崩溃问题
		strSheetName.Remove('_');
		mapSheet.insert(map <CString,int>::value_type(strSheetName,i));			//添加每一个表格以及表格名称
		sql.Format(_T("CREATE TABLE [%s]([%s] TEXT"),strSheetName, serialStr);	//创建一个表%s，然后表项%s
		for(j=1;j<colCount;j++)			//标头项进行一个循环
		{
			pList->GetHeaderCtrl()->GetItem(j,&hdi);		//获取标头的属性信息
			strColName = hdi.pszText;						//获取每个标头的文本信息
			if(-1 != strColName.Find(typeStr,0))			//如果在标头中获取到包含“类型”的标头		
			{
				nTypeCol = j;								//获得定值类型的标头索引
				continue;
			}
			str.Format(_T("%s,[%s] TEXT"),sql,strColName);	//对列表添加标头项
			sql = str;
		}
		sql += _T(")");			//因为上面也是半边括号
		sql.Remove('.');
// 		sql.Remove('-');//将引起导出实测数据EXCEL崩溃
// 		sql.Remove('_');
		database.Execute(sql);				//创建表格
	}
	CString strError;
	int *pIndex;
	if(bIsSaveSelected)
	{
		POSITION pos=pList->GetFirstSelectedItemPosition();			//获取当前点击的位置
		while (pos)
		{
			int nItem = pList->GetNextSelectedItem(pos);			//获取列表点击的索引号
			pIndex=NULL;
			strSheetName = pList->GetItemText(nItem,nTypeCol);		//获取列表某行的最后一个
			strSheetName.Remove('-');
			strSheetName.Remove('_');
			sql.Format(_T("select * from [%s]"),strSheetName);		//应该是为去数据库中查找数据做准备
			pIndex = &index[mapSheet.find(strSheetName)->second];	//获取当前这种类型的数据的表格索引值
			sql.Remove('.');
// 			sql.Remove('-');
// 			sql.Remove('_');
			if(!record.Open(sql,database,adLockOptimistic))			//打开记录集
			{
				database.Close();
				return FALSE;
			}
			record.AddNew();										//增加记录
			ASSERT(pIndex != NULL);
			str.Format(_T("%d"),*pIndex);
			record[serialStr] = str;								//类型这个位置 = 索引号
			(*pIndex)++;
			for(i=1;i<colCount;i++)									//标头数目做一个循环，然后把标头的每一项都进行了复制
			{
				if(nTypeCol == i) continue;
				pList->GetHeaderCtrl()->GetItem(i,&hdi);
				strItemText = pList->GetItemText(nItem,i);
				record[(CString)hdi.pszText] = strItemText;
			}
			info.LoadString( IDS_ADD_FAIL );			//所有类型
			strError.Format(info,strSheetName);			//所有类型+表格名称（每一种类型就是一个名称）
			try
			{
				record.Update();						//添加到记录集
			}
			catch (...)
			{
				CdMessageBox box(theApp.m_pMainWnd, strError, _T(""), MB_OK);
	 	        box.MsgBoxEx();
			}
			record.Close();
		}
	}
	else
	{
		long count = pList->GetItemCount();
		for(i=0;i<count;i++)
		{
			pIndex=NULL;
			strSheetName = pList->GetItemText(i,nTypeCol);
			strSheetName.Remove('-');
			strSheetName.Remove('_');
			sql.Format(_T("select * from [%s]"),strSheetName);
			pIndex = &index[mapSheet.find(strSheetName)->second];
			CString strshow;
			strshow.Format(_T("%d"), *pIndex);
// 			AfxMessageBox(strshow);
			sql.Remove('.');
// 			sql.Remove('-');
// 			sql.Remove('_');
// 			AfxMessageBox(sql);
			if(!record.Open(sql,database,adLockOptimistic))
			{
				database.Close();
				return FALSE;
			}
			record.AddNew();
			ASSERT(pIndex != NULL);
			str.Format(_T("%d"),*pIndex);
			record[serialStr] = str;
			(*pIndex)++;
			for(j=1;j<colCount;j++)
			{
				if(nTypeCol == j) continue;
				pList->GetHeaderCtrl()->GetItem(j,&hdi);
				strItemText = pList->GetItemText(i,j);
				record[(CString)hdi.pszText] = strItemText;
			}
			info.LoadString( IDS_ADD_FAIL ); 
			strError.Format(info,strSheetName);
			try
			{
				record.Update();
			}
			catch (...)
			{
				CdMessageBox box(theApp.m_pMainWnd, strError, _T(""), MB_OK);
	 	        box.MsgBoxEx();
			}
			record.Close();
		}
	}
	if(bAllInOneTable)
	{
		pComboBox->GetLBText(0,strSheetName);
		strSheetName.Remove('-');
		strSheetName.Remove('_');
		int nIndexOfSheet=0;
		sql.Format(_T("CREATE TABLE [%s]([%s] TEXT"),strSheetName, serialStr);
		sql.Remove('.');
// 		sql.Remove('-');
// 		sql.Remove('_');
		for(j=1;j<colCount;j++)
		{
			pList->GetHeaderCtrl()->GetItem(j,&hdi);
			strColName = hdi.pszText;
			str.Format(_T("%s,[%s] TEXT"),sql,strColName);
			sql = str;
		}
		sql += _T(")");
		database.Execute(sql);
		sql.Format(_T("select * from [%s]"),strSheetName);
		sql.Remove('.');
// 		sql.Remove('-');
// 		sql.Remove('_');
		if(!record.Open(sql,database,adLockOptimistic))
		{
			database.Close();
			return FALSE;
		}
		if(bIsSaveSelected)
		{
			POSITION pos=pList->GetFirstSelectedItemPosition();
			while (pos)
			{
			
				int nItem = pList->GetNextSelectedItem(pos);
				record.AddNew();
				str.Format(_T("%d"),nIndexOfSheet++);
				record[serialStr] = str;
				for(j=1;j<colCount;j++)
				{
					pList->GetHeaderCtrl()->GetItem(j,&hdi);
					strItemText = pList->GetItemText(i,j);
					record[(CString)hdi.pszText] = strItemText;
				}
				info.LoadString( IDS_ADD_FAIL );
				strError.Format(info,strSheetName);
				try
				{
					record.Update();
				}
				catch (...)
				{
					CdMessageBox box(theApp.m_pMainWnd, strError, _T(""), MB_OK);
	 	            box.MsgBoxEx();
				}
			}

		}
		else
		{
			long count = pList->GetItemCount();
			for(i=0;i<count;i++)
			{
				record.AddNew();
				ASSERT(pIndex != NULL);
				str.Format(_T("%d"),nIndexOfSheet++);
				record[serialStr] = str;
				for(j=1;j<colCount;j++)
				{
					pList->GetHeaderCtrl()->GetItem(j,&hdi);
					strItemText = pList->GetItemText(i,j);
					record[(CString)hdi.pszText] = strItemText;
				}
				info.LoadString( IDS_ADD_FAIL );
				strError.Format(info,strSheetName);
				try
				{
					record.Update();
				}
				catch (...)
				{
					CdMessageBox box(theApp.m_pMainWnd, strError, _T(""), MB_OK);
	 	            box.MsgBoxEx();
				}
			}
		}
		record.Close();
	}
	database.Close();
	return TRUE;
}

/**************************************************************
  函数：OnInParaFile
  描述；导入装置定值文件
  参数：无
  返回：无
  修改记录：
**************************************************************/
void CMainFrame::OnInParaFile()
{
	ASSERT(GetActiveView()->IsKindOf(RUNTIME_CLASS(CParaView)));
	CParaView * pView = (CParaView *)GetActiveView();
	pView->ReadParaFile();
}

/**************************************************************
  函数：OnReadParaData
  描述；读取装置定值（导航树右键菜单的读取装置能进）
  参数：
  返回：
  修改记录：
**************************************************************/
void CMainFrame::OnReadParaData()
{
	
	ASSERT(GetActiveView()->IsKindOf(RUNTIME_CLASS(CParaView)));
	CParaView * pView = (CParaView *)GetActiveView();
	pView->ReadDevicePara();
}


void CMainFrame::OnInEventFile() 
{
	// TODO: Add your command handler code here
	ASSERT(GetActiveView()->IsKindOf(RUNTIME_CLASS(CEventView)));
	CEventView * pView = (CEventView *)GetActiveView();
	pView->ReadEventFile();
	
}

void CMainFrame::OnOutEvent() 
{
	// TODO: Add your command handler code here
	
	ASSERT(GetActiveView()->IsKindOf(RUNTIME_CLASS(CEventView)));
	CEventView * pView = (CEventView *)GetActiveView();
	pView->OutEventToExcel();
	
}

/*******************************************************************
  函数名：OnOutParaToExcel
  描述：导出保护定值到Excel文件中
*******************************************************************/
void CMainFrame::OnOutParaToExcel()
{
	ASSERT(GetActiveView()->IsKindOf(RUNTIME_CLASS(CParaView)));
	CParaView * pView = (CParaView *)GetActiveView();
	CCustomFileDialog dlg(FALSE,NULL,_T("*.xls"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Worksheet Files (*.xls)|*.xls||"));
	if(IDOK==dlg.DoModal())
	{
		CString strFilePath, info, title,str;
		title.LoadString( IDS_EXPORTPARA );
		strFilePath = dlg.GetPathName();
		if(!CMainFrame::GetExcelFile(strFilePath,&pView->m_ctlParaType,&pView->m_listPara,FALSE,TRUE))
		{
			str.LoadString( IDS_EXPORTPARA_INFO );
		}
		else
		{
			info.LoadString( IDS_EXPORTPARA_OK );
			str.Format(info, strFilePath);
		}
		CdMessageBox box(this, str, title);
		box.MsgBoxEx();
	}
}

/*******************************************************************
  函数名：OnOutRealDataToExcel
  描述：导出实测数据到Excel文件中
*******************************************************************/
void CMainFrame::OnOutRealDataToExcel()
{
	ASSERT(GetActiveView()->IsKindOf(RUNTIME_CLASS(CRealDataView)));
	CRealDataView * pView = (CRealDataView *)GetActiveView();
	CCustomFileDialog dlg(FALSE,NULL,_T("*.xls"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Worksheet Files (*.xls)|*.xls||"));
	if(IDOK==dlg.DoModal())
	{
		CString strFilePath, info, str, title;;
		strFilePath = dlg.GetPathName();
		pView->KillTimer(IDC_FREQ_TIMER);
		title.LoadString( IDS_EXPORTDATA );
		if(!CMainFrame::GetExcelFile(strFilePath,&pView->m_ctlDataType,&pView->m_listRealData,FALSE))
		{
			str.LoadString( IDS_EXPORTDATA_INFO );
		}
		else
		{	info.LoadString( IDS_EXPORTDATA_INFO2 );
			str.Format(info, strFilePath);
		}
		CdMessageBox box(this, str, title);
		box.MsgBoxEx();
//		pView->SetTimer(IDC_FREQ_TIMER,pView->m_dRefurbishFreq*1000,NULL);
	}
}

/*******************************************************************
  函数名：OnOutWareFileToComtrade
  描述：按comtrade标准文件格式将录波文件导出
*******************************************************************/
void CMainFrame::OnOutWareFileToComtrade()
{
	CCustomFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Comtrade File(输入Comtrade的文件夹名)|*||"));
	if(IDOK==dlg.DoModal())
	{
		CString strFolderPath,strFilePath,info;
		strFolderPath =dlg.GetPathName();
		if(m_nRClickWareFileIndex < 0 || m_nRClickWareFileIndex >= theApp.m_projectData.m_nFileCount)
		{
			return;
		}
		m_wareFileData.Release();
		strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,
			(CString)theApp.m_projectData.m_arrfileTreeNode[m_nRClickWareFileIndex].strFileName);
		if ( TranErrorInfo( m_wareFileData.OpenFile(strFilePath) ) )
		{
			return;
		}
		if(!m_wareFileData.GetComtradeFile(strFolderPath))
		{
	     	info.LoadString( IDS_EXPORTCOMTRADE_ERROR );
		
		}
		else
		{
			info.LoadString( IDS_EXPORTCOMTRADE_INFO );
		}
		CdMessageBox box(this, info);
		box.MsgBoxEx();
	}
}


BOOL CMainFrame::TestConnection()
{
	BOOL bIsConnection = theApp.m_projectData.TestConnection();
	if (!bIsConnection)
	{
		StopCommunication();
	}
	else
	{
		OpenCommunication();
	}
	return bIsConnection;
}

void CMainFrame::CurveDisSet()
{
	CView * pView = GetActiveView();
	if(pView->IsKindOf(RUNTIME_CLASS(CCurveView)))
	{
		CCurveView * pCurveView = (CCurveView *)pView;
		CCustomizeWareSetDlg dlg(&m_wareFileData,TRUE,pCurveView->m_pCurve,pCurveView->m_nCurveCount);
		if(IDOK ==dlg.DoModal())
		{
			CCurveView::m_isFirstShow = TRUE;
			pCurveView->ChangeCurve(m_wareFileData.m_pCurve,m_wareFileData.m_nCurveCount,(bool)m_wareFileData.m_isUnitedFile,m_wareFileData.m_diffMilliSecond,dlg.m_cPS);
			
			m_nHMulti = 1;
			m_nVMulti = 1;
			CToolBarCtrl & toolBarCtrl = m_wndToolBar.GetToolBarCtrl();
			toolBarCtrl.EnableButton(ID_CURVE_H_BIGGER,TRUE);
			toolBarCtrl.EnableButton(ID_CURVE_V_BIGGER,TRUE);
			m_wndStatusBar.SetPaneStyle(1,SBPS_NORMAL );
			m_wndStatusBar.SetPaneStyle(2,SBPS_NORMAL );
		}
	}
	
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	// TODO: Add your message handler code here
	CView * pView = GetActiveView();
	switch (nIndex)
	{
	case 0:
		if(pView->IsKindOf(RUNTIME_CLASS(CCurveView)))
		{
			pPopupMenu->EnableMenuItem(ID_FILE_PRINT_SETUP,MF_ENABLED|MF_BYCOMMAND);
		}
		else
		{
			pPopupMenu->EnableMenuItem(ID_FILE_PRINT_SETUP,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		}
		break;
	case 1:
		if(pView->IsKindOf(RUNTIME_CLASS(CEventView)))
		{
			pPopupMenu->EnableMenuItem(ID_SHOW_EVENT,MF_ENABLED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_GET_EVENT,MF_ENABLED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_IN_EVENT_FILE,MF_ENABLED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OUT_EVENT,MF_ENABLED|MF_BYCOMMAND);
		}
		else	
		{
			pPopupMenu->EnableMenuItem(ID_SHOW_EVENT,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_GET_EVENT,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_IN_EVENT_FILE,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OUT_EVENT,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		}
		break;
	case 2:
		if (!m_bInProject || m_strDevice == DVI_DEVICE)
		{
			pPopupMenu->EnableMenuItem(ID_GET_WARE_FILE,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OPEN_WARE_FILE,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		}
		else
		{
			pPopupMenu->EnableMenuItem(ID_GET_WARE_FILE,MF_ENABLED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OPEN_WARE_FILE,MF_ENABLED|MF_BYCOMMAND);
		}
		break;
	default:
		break;
	}
}
/*******************************************************************
  函数名：OnCurveUnite
  描述：波形合并
*******************************************************************/
void CMainFrame::OnCurveUnite()
{
	if(m_nRClickWareFileIndex < 0 || m_nRClickWareFileIndex >= theApp.m_projectData.m_nFileCount)
	{
		return;
	}
	m_wareFileData.Release();
	CString strFileName;
	strFileName.Format(_T("%s"),(CString)theApp.m_projectData.m_arrfileTreeNode[m_nRClickWareFileIndex].strFileName);
	CUniteCurvDlg dlg(strFileName);
	dlg.DoModal();
	if(dlg.isUnited)
	{
		LoadProjectData();
		ExpendTreeItem(_T("录波文件"));
	}
}



void CMainFrame::OnModifyPsw() 
{
	// TODO: Add your command handler code here
	ChangePswDlg dlg;
	dlg.DoModal();
	if (dlg.m_bCorrect)
	{
		CString info;
		info.LoadString( IDS_PWDC_INFO );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
	}
}

void CMainFrame::OnMulti2() 
{
	// TODO: Add your command handler code here
	CMenu *pMenu=GetMenu();
	pMenu->CheckMenuItem(ID_MULTI_2,MF_CHECKED);
	pMenu->CheckMenuItem(ID_MULTI_1,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_MULTI_4,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_DEFINE_SELF,MF_UNCHECKED);
	m_nMultiFactor = 4;
}

void CMainFrame::OnMulti4() 
{
	// TODO: Add your command handler code here
	m_nMultiFactor = 8;
	CMenu *pMenu=GetMenu();
	pMenu->CheckMenuItem(ID_MULTI_4,MF_CHECKED);
	pMenu->CheckMenuItem(ID_MULTI_1,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_MULTI_2,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_DEFINE_SELF,MF_UNCHECKED);
}

void CMainFrame::OnMulti1() 
{
	// TODO: Add your command handler code here
	m_nMultiFactor = 2;
	CMenu *pMenu=GetMenu();
	pMenu->CheckMenuItem(ID_MULTI_1,MF_CHECKED);
	pMenu->CheckMenuItem(ID_MULTI_2,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_MULTI_4,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_DEFINE_SELF,MF_UNCHECKED);
}


void CMainFrame::OnRestart() 
{
	// TODO: Add your command handler code here
	BYTE ret = 1;
	CPWDDlg dlg;
	dlg.DoModal();
	if (dlg.m_bCorrect)
	{
		ret = CFrame::TelControl(EnumReset);
		theApp.m_eRestart.SetEvent();
	}
	else
	{
		return;
	}
	CString info;
	if(ret == 0)
	{
		info.LoadString( IDS_DEVS_RESTART_OK );
	}
	else
	{
		info.LoadString( IDS_DEVS_RESTART_ERROR );
	}
	CdMessageBox box( this, info );
	box.MsgBoxEx();
}



void CMainFrame::OnRightBar() 
{
	// TODO: Add your command handler code here
	if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CRealMonitorView)))
	{
		ShowControlBar(&m_wndRightBar, !m_wndRightBar.IsVisible(), FALSE);
	}
}



void CMainFrame::StopCommunication()
{
	// TODO: Add your command handler code here
    int	static  count = 0;
	CToolBarCtrl & toolBarCtrl = m_wndMainToolBar.GetToolBarCtrl();
	toolBarCtrl.EnableButton(ID_GET_WARE_FILE,FALSE);
	toolBarCtrl.EnableButton(IDC_START_SYNC,FALSE);
	toolBarCtrl.EnableButton(IDM_UPDATE_PARA,FALSE);
	toolBarCtrl.EnableButton(IDC_RESTART,FALSE);
	toolBarCtrl.EnableButton(ID_CONNECT,TRUE);
	toolBarCtrl.EnableButton(ID_DISCONNECT,FALSE);
	CUSBSocket::m_usbMidWare->CableConfig(false);
	CNetSocket::m_udpNetWork.CableConfig(false);
		
} 

void CMainFrame::OpenCommunication()
{
	// TODO: Add your command handler code here
	CToolBarCtrl & toolBarCtrl = m_wndMainToolBar.GetToolBarCtrl();
	toolBarCtrl.EnableButton(ID_CONNECT,FALSE); 
	toolBarCtrl.EnableButton(ID_DISCONNECT,TRUE);
	if (m_strDevice != DVI_DEVICE)
	{
		toolBarCtrl.EnableButton(ID_GET_WARE_FILE,TRUE);
		toolBarCtrl.EnableButton(IDC_RESTART,TRUE);
		if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CRealMonitorView)))
		{
			toolBarCtrl.EnableButton(IDM_UPDATE_PARA,TRUE);
		}
	}
		
} 

void CMainFrame::OnConnect() 
{
	// TODO: Add your command handler code here
	CUSBSocket::m_usbMidWare->CableConfig(TRUE);
	CNetSocket::m_udpNetWork.CableConfig(TRUE);
	if(TestConnection())
	{
		if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CRealMonitorView)))
		{
			((CRealMonitorView *)GetActiveView())->StartCommunication();
		}
		else if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CParaView)))
		{
		}
		else
		{
			ReplaceView(RUNTIME_CLASS(CWareView));
		}
	}
}

void CMainFrame::OnDisconnect() 
{
	//GetDlgItem(ID_CONNECT)->EnableWindow( FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_CONNECT,FALSE);
	StopCommunication();
	Sleep( 500 );
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_CONNECT,TRUE);
	//GetDlgItem(ID_CONNECT)->EnableWindow( TRUE );
}


void CMainFrame::ClearSyncList()
{
	int i;
	for(i=0;i<20;i++)
	{
		m_listSyncPara.SetItemText(i,1,_T(""));
	}
}

void CMainFrame::OnDefineSelf() 
{
	// TODO: Add your command handler code here
	CDefineScale dlg;
	if (IDOK==dlg.DoModal())
	{
		CMenu *pMenu=GetMenu();
		pMenu->CheckMenuItem(ID_MULTI_1,MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_MULTI_2,MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_MULTI_4,MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_DEFINE_SELF,MF_CHECKED);
		m_nMultiFactor = 2*(CDefineScale::m_curSel);
	}
	
}

void CMainFrame::ShowCurve()
{
	OnShowCurve();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
// 	m_wndMainToolBar.SetWindowPos(NULL,0,0,618,58,NULL);
// 	m_wndToolBar.ShowWindow(SW_HIDE);
// 	m_wndToolBar.SetWindowPos(NULL,620,0,240,58,NULL);
// 	m_wndToolBar.ShowWindow(SW_SHOW);
	// TODO: Add your message handler code here
	
}

/*************************************************************************
函数名：OnOutParaData
描述：导出录波曲线保护定值到Excel文件中    
参数：
*************************************************************************/
void CMainFrame::OnOutParaData()
{
	CCustomFileDialog dlg(FALSE,NULL,_T("*.xls"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Worksheet Files (*.xls)|*.xls||"));
	if(IDOK==dlg.DoModal())
	{
		CString strFilePath, info, title;
		strFilePath = dlg.GetPathName();
		CString str;
		title.LoadString( IDS_EXPORTPARA ); 
		if(!CMainFrame::GetParaFile(strFilePath,&m_listParaInfo))
		{
			str.LoadString( IDS_EXPORTPARA_INFO );
		}
		else
		{
			info.LoadString( IDS_EXPORTPARA_OK );
			str.Format(info, strFilePath);
		}
		CdMessageBox box(this, str, title);
	 	box.MsgBoxEx();
	}
	
}

/*************************************************************************
函数名：OnOutChannelData
描述：导出录波曲线通道信息到Excel文件中    
参数：
*************************************************************************/
void CMainFrame::OnOutChannelData()
{
	CCustomFileDialog dlg(FALSE,NULL,_T("*.xls"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Worksheet Files (*.xls)|*.xls||"));
	if(IDOK==dlg.DoModal())
	{
		CString strFilePath, info, title;;
		strFilePath = dlg.GetPathName();
		CString str;
		title.LoadString( IDS_EXPORTCHANALE );
		if(!CMainFrame::GetParaFile(strFilePath,&m_listChannelInfo))
		{
			str.LoadString( IDS_EXPORTPARA_INFO );
		}
		else
		{
			info.LoadString( IDS_EXPORTCHANEL_OK );
			str.Format(info, strFilePath);
		}
		CdMessageBox box(this, str, title);
		box.MsgBoxEx();
	}
	
}

/*************************************************************************
函数名：
描述：视图显示录波时，下方的信息视图显示相应的录波信息，
      当右击定值列表时，响应的函数     
参数：pNMHDR：选中的列信息，
*************************************************************************/
void CMainFrame::OnRclickParalist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point;
	GetCursorPos(&point);
	m_listParaInfo.ScreenToClient(&point);
	UINT nFlag;
	m_listParaInfo.HitTest(point,&nFlag);
	m_listParaInfo.ClientToScreen(&point);
	CMenu PopupMenu;
	PopupMenu.CreatePopupMenu();
	PopupMenu.AppendMenu(MF_ENABLED, IDM_OUT_PARA_DATA, _T("导出录波定值"));
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON , point.x, point.y, this);
}

/*************************************************************************
函数名：
描述：视图显示录波时，下方的信息视图显示相应的录波信息，
      当右击通道信息列表时，响应的函数     
参数：pNMHDR：选中的列信息，
*************************************************************************/
void CMainFrame::OnRclickChanlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point;
	GetCursorPos(&point);
	m_listChannelInfo.ScreenToClient(&point);
	UINT nFlag;
	m_listChannelInfo.HitTest(point,&nFlag);
	m_listChannelInfo.ClientToScreen(&point);
	CMenu PopupMenu;
	PopupMenu.CreatePopupMenu();
	PopupMenu.AppendMenu(MF_ENABLED, IDM_OUT_CHANNEL_DATA, _T("导出通道信息"));
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON , point.x, point.y, this);
}

BOOL CMainFrame::GetParaFile(const CString & strFileName,CListCtrl * pList)
{
	long i,j,colCount;//,nTypeCol; 修改warning
	HDITEM hdi;   
	TCHAR lpBuffer[256];   
	bool fFound =false;
	hdi.mask=HDI_TEXT;   
	hdi.pszText=lpBuffer;   
	hdi.cchTextMax=256;   
	CFileFind fileFinder;
	CString info;
	if(fileFinder.FindFile(strFileName))
	{
		if(!DeleteFile(strFileName))
		{
			CString strDeleteFailed;
			info.LoadString( IDS_FILEUSED_INFO );
			strDeleteFailed.Format(info, strFileName);
			CdMessageBox box(theApp.m_pMainWnd, strDeleteFailed, _T(""), MB_OK);
	 	    box.MsgBoxEx();
			return FALSE;
		}
	}
	
	CAdoDatabase database;
	CAdoRecordset record;
	CString strConn;
	strConn.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Extended Properties=\"Excel 8.0;HDR=Yes;IMEX=0\";"),strFileName);
	
	if(!database.Open(strConn,adModeUnknown))
	{
		return FALSE;
	}
	CString sql,strSheetName,strItemText,strColName,str,strError;
	map <CString,int> mapSheet;
	int index[100];
	colCount = pList->GetHeaderCtrl()->GetItemCount();
	memset(index,0,sizeof(index));
	long count = pList->GetItemCount();
	strSheetName = _T("sheet");
	pList->GetHeaderCtrl()->GetItem(0,&hdi);
	strColName = hdi.pszText;
	sql.Format(_T("CREATE TABLE [%s]([%s] TEXT"),strSheetName,strColName);
	for(j=1;j<colCount;j++)
	{
		pList->GetHeaderCtrl()->GetItem(j,&hdi);
		strColName = hdi.pszText;
		str.Format(_T("%s,[%s] TEXT"),sql,strColName);
		sql = str;
	}
	sql += _T(")");
	database.Execute(sql);
	
	sql.Format(_T("select * from [%s]"),strSheetName);
	if(!record.Open(sql,database,adLockOptimistic))
	{
		database.Close();
		return FALSE;
	}
	int nIndexOfSheet = 0;
	for(i=0;i<count;i++)
	{
		record.AddNew();
		for(j=0;j<colCount;j++)
		{
			pList->GetHeaderCtrl()->GetItem(j,&hdi);
			strItemText = pList->GetItemText(i,j);
			record[(CString)hdi.pszText] = strItemText;
		}
		info.LoadString( IDS_ADD_FAIL );
		strError.Format(info, strSheetName);
		try
		{
			record.Update();
		}
		catch (...)
		{
			CdMessageBox box(theApp.m_pMainWnd, strError, _T(""), MB_OK);
	 	    box.MsgBoxEx();
		}
	}
	return TRUE;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	TCHAR filePath[200]={0}; 
	int pathLen = DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath)); 
	DragFinish(hDropInfo); 
	OpenNewFile(filePath);

//	CFrameWnd::OnDropFiles(hDropInfo);
}

void CMainFrame::OpenNewFile(CString path)
{
	//char path_buffer[_MAX_PATH];//lhz 2012-2-9修改warming，此变量定义未使用
	//拆分打开文件的路径
    char drive[_MAX_DRIVE];  //盘符
    char dir[_MAX_DIR];      //路径
    char fname[_MAX_FNAME];  //文件名
    char ext[_MAX_EXT];      //扩展名
    char file[_MAX_EXT];     //文件路径

	//把宽字符转换成字符串数组
	WideCharToMultiByte(CP_ACP,0,path,-1,file,_MAX_EXT,NULL,NULL);
	_splitpath( file, drive, dir, fname, ext );
	CString strExt = ext;
	CString info;
	//打开工程文件
	if(strExt == _T(".prj"))
	{
		CProjectData project;
		if(project.OpenProject(path))
		{
			
			if(theApp.m_mapDiviceType.find(project.m_strDeviceType) == theApp.m_mapDiviceType.end())
			{
				CString str;
				str.LoadString( IDS_PRJFILE_BROKEN );
				CdMessageBox box( this, str);
	            box.MsgBoxEx();
				return;
			}
			theApp.m_projectData=project;
			LoadProjectData();
			ReleaseProject();
			if (TestConnection())
			{
				CString info;
				info.LoadString( IDS_CONNECT_OK );
				CdMessageBox box(this, info);
	 	        box.MsgBoxEx();
				CToolBarCtrl & toolBarCtrl = m_wndMainToolBar.GetToolBarCtrl();
				toolBarCtrl.EnableButton(ID_DISCONNECT,TRUE);
				toolBarCtrl.EnableButton(ID_CONNECT,FALSE);
			}
			GetActiveDocument()->SetTitle((CString)theApp.m_projectData.m_strDeviceType);
			theApp.m_projectData.SaveProjectSetting();
			m_bInProject = TRUE;
		}
	}
	else if(strExt == _T(".wave"))//打开录波文件，只有已经打开了工程文件之后才能打开录波文件
	{
		if(theApp.m_projectData.m_strDeviceType[0] == 0)
		{
			info.LoadString( IDS_STR_PRJINFO );
			CdMessageBox box(this, info);
	 	        box.MsgBoxEx();
			return;
		}
		CString str;
		if(theApp.m_projectData.m_nFileCount >= MAX_FILE_COUNT)
		{
			info.LoadString( IDS_MAX_FILENUM );
			str.Format( info, MAX_FILE_COUNT, theApp.m_projectData.m_nFileCount);
			CdMessageBox box(this, str);
	 	    box.MsgBoxEx();
			return;
		}
		CString strFileName;
		strFileName = fname;
		strFileName += ext;
		CString strProFilePath,strProFileName=strFileName;
		strProFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,strFileName);
		if(path == strProFilePath)
		{
			int i;
			for(i=0;i<theApp.m_projectData.m_nFileCount;i++)
			{
				if(strFileName == theApp.m_projectData.m_arrfileTreeNode[i].strFileName)
				{
					break;
				}
			}
			if(i<theApp.m_projectData.m_nFileCount)
			{
				info.LoadString( IDS_OPENFILE_INFO );
				CdMessageBox box(this, info);
	 	        box.MsgBoxEx();
			}
			else
			{
				if(!theApp.m_projectData.AddWareFileToProject(strFileName))
				{
					info.LoadString( IDS_FILE_BROKEN );
					CdMessageBox box(this, info);
	 	            box.MsgBoxEx();
				}
			}
		}
		else
		{
			CFileFind fileFind;
			int temp=1;
			//文件名相同时
			while(fileFind.FindFile(strProFilePath))
			{
				strFileName.Format(_T("%s_%d.wave"),strFileName.Left(strFileName.GetLength()-5),temp);
				strProFilePath.Format(_T("%s_%d.wave"),strProFilePath.Left(strProFilePath.GetLength()-5),temp);
				temp++;
			}
			FILE * wareFile;
			wareFile = _wfopen(path,_T("rb"));
			if(wareFile == NULL)
			{
				info.LoadString( IDS_STR_OPENE );
				CdMessageBox box(this, info);
	 	        box.MsgBoxEx();
				return;
			}
			fseek(wareFile,0,SEEK_END);
			int filelength=ftell(wareFile);
			BYTE *fileBuf=new BYTE[filelength+10];
			memset(fileBuf,0,filelength+10);
			fseek(wareFile,0,SEEK_SET);
			temp=fread(fileBuf,sizeof(BYTE),filelength+10,wareFile);
			fclose(wareFile);
			ASSERT(filelength==temp);
			wareFile = _wfopen(strProFilePath,_T("wb"));
			if(wareFile == NULL)
			{
				info.LoadString( IDS_STR_OPENE );
				CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
	 	        box.MsgBoxEx();
				delete [] fileBuf;
				return;
			}
			if(filelength != fwrite(fileBuf,sizeof(BYTE),filelength,wareFile))
			{
				info.LoadString( IDS_STR_CPE ); 
				CdMessageBox box(this, info);
	 	        box.MsgBoxEx();
				fclose(wareFile);
				delete [] fileBuf;
				return;
			}
			delete [] fileBuf;
			fclose(wareFile);
			if(!theApp.m_projectData.AddWareFileToProject(strFileName))
			{
				info.LoadString( IDS_FILE_BROKEN );
				CdMessageBox box(this, info);
	 	        box.MsgBoxEx();
			}
			LoadProjectData();
			info.LoadString( IDS_WAVE_FILE );
			ExpendTreeItem( info );
		}//of else
	}		

}
