// SID_2FY.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "SID_2FY.h"
#include "MainFrm.h"
#include "SID_2FYDoc.h"
#include "WareView.h"
#include "EventView.h"
#include "FileInfoDlg.h"
#include "InPlaceEdit.h"
#include "Md5Class.h"
#include <AFXPRIV.H>
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYApp

char strOmnipotencePSW[20]="szidd";
char strDefaultPSW[20]="SZZN";

BEGIN_MESSAGE_MAP(CMySID_2FYApp, CWinApp)
	//{{AFX_MSG_MAP(CMySID_2FYApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYApp construction

CMySID_2FYApp::CMySID_2FYApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	//当前录波文件的个数
	m_nWareDataCount=0;
	//语言设置
	m_pLangManger = CLanguageManger::GetPointOfLangManger();
	m_eRestart.ResetEvent();
	m_freq[0] = 0;
	m_freq[1] = 0;
	m_freq[2] = 0;
	m_freq[3] = 0;
	m_freqIndex = 0;
}
CMySID_2FYApp::~CMySID_2FYApp()
{
	CInPlaceEdit::DeleteInstance();
	//lhz add 修改内存泄露问题 
	m_mapDiviceType.clear();
	delete CUSBSocket::m_usbMidWare;
	//end
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CMySID_2FYApp object

CMySID_2FYApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYApp initialization
bool IsWin7()  
{  
    OSVERSIONINFOEX osvi;  
    BOOL bOsVersionInfoEx;  
    
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));  
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);  
    bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);  
    
    // win7的系统版本为NT6.1  
    if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId &&    
        osvi.dwMajorVersion == 6 &&   
        osvi.dwMinorVersion == 1 )  
    {  
        return true;      
    }  
    else  
    {  
        return false;  
    }  
}  
//手工修改用户界面（UI）的现实语言
typedef LANGID (WINAPI *FP_SetThreadUILanguage)(LANGID LangId);
LANGID SetThreadUILanguage(LANGID lcid)
{
    HMODULE hKernel32 = GetModuleHandle(_T("Kernel32.dll"));
    FARPROC pFn = GetProcAddress(hKernel32, "SetThreadUILanguage");
    
    FP_SetThreadUILanguage pSetThreadUILanguage = (FP_SetThreadUILanguage)pFn;
    if (pSetThreadUILanguage)
    {
        return pSetThreadUILanguage(lcid);
    }
    return 0;
}

BOOL CMySID_2FYApp::InitInstance()
{
	AfxEnableControlContainer();
	if(!AfxOleInit())
	{
		CString info;
		info.LoadString( IDS_ERROR_OLE );
		CdMessageBox box(this->m_pMainWnd, info);
        box.MsgBoxEx();

		return FALSE;
	}

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	//语言设置
	//获取当前路径
	CString szCurPath(_T(""));
	GetModuleFileName(NULL,szCurPath.GetBuffer(MAX_PATH),MAX_PATH);	
	szCurPath.ReleaseBuffer();
	m_pLangManger->m_szCurPath = szCurPath.Left(szCurPath.ReverseFind('\\') + 1);
	//设置文件在当前路径下 
	m_pLangManger->m_szSettingPath = m_pLangManger->m_szCurPath + _T("setting.ini");
	m_pLangManger->LoadLanguageWay2();
	DWORD LangId = GetUserDefaultLangID();
    if (IDS_STR_ENGLISH == m_pLangManger->m_strCurLanguage && MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) != LangId )
    {
		SetThreadUILanguage( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );  //win 7 下用
	//	SetThreadLocale( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );  //xp 下可以用此值
		//SetLanguageID(SUBLANG_ENGLISH_US);
    }else if (SUBLANG_CHINESE_SIMPLIFIED == m_pLangManger->m_strCurLanguage && MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED) != LangId)
    {
		SetThreadUILanguage( MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED) );  //win 7 下用

	//	SetThreadLocale( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );
	}
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMySID_2FYDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWareView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// Dispatch commands specified on the command line
	//m_nCmdShow=SW_MAXIMIZE;
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	
	if(!InitPasswordFile())
	{
		return FALSE;
	}
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	m_pMainWnd->UpdateWindow();
	//套接字版本的引入
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		return FALSE;
	}
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
		WSACleanup( );
		return FALSE; 
	}
	deviceInfo dt;
	dt.pid = _T("6119");
	dt.vid = _T("03EB");
	CUSBSocket::m_usbMidWare = new CUSBMidWare();
	CUSBSocket::m_usbMidWare->USBMidWareInit(dt);
	if(!CNetSocket::m_udpNetWork.UDPInit())
	{
		return FALSE;
	}
	//2012=10-19 修改，测试需要添加SID-2CM
    //m_mapDiviceType.insert(map <CString,CString>::value_type(_T("SID-2CM"),_T("CFrame_NoAnalyse")));
	//end 2012=10-19
/*	m_mapDiviceType.insert(map <CString,CString>::value_type(_T("SID-40B"),_T("CFrame_NoAnalyse")));*/
	m_mapDiviceType.insert(map <CString,CString>::value_type(_T("SID-2FY"),_T("CFrame_NoAnalyse")));
//	m_mapDiviceType.insert(map <CString,CString>::value_type(_T("SID-2AF"),_T("CFrame_NoAnalyse")));
	m_mapDiviceType.insert(map <CString,CString>::value_type(_T("SID-DVI"),_T("CFrame_ViewPara")));
	m_mapDiviceType.insert(map <CString,CString>::value_type(_T("SID-2AS(B)"),_T("CFrame_NoAnalyse")));
	m_mapDiviceType.insert(map <CString,CString>::value_type(_T("SID-2AF(B)"),_T("CFrame_NoAnalyse")));
	m_mapDiviceType.insert(map <CString,CString>::value_type(_T("SID-2CM(B)"),_T("CFrame_NoAnalyse")));
		
	
	
	// enable File Manager drag/drop an project or wave file open
	m_pMainWnd->DragAcceptFiles();
	//初始化MessageBox
    InitMessageBox();
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL CAboutDlg::OnInitDialog();
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	return TRUE;  
}

// App command to run the dialog
void CMySID_2FYApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYApp message handlers

BOOL CMySID_2FYApp::InitPasswordFile()
{
	char *pStr =MD5String(strOmnipotencePSW);
	strcpy(m_strOmnipotencePSW,pStr);
	free(pStr);
	CString str;
	FILE * file = NULL;
	long nFileLength;

	
	GetModuleFileName(NULL,m_strAppPath.GetBuffer(MAX_PATH),MAX_PATH);
	m_strAppPath.ReleaseBuffer();
	int position = m_strAppPath.ReverseFind('\\');
	m_strAppPath = m_strAppPath.Left(position+1);

	m_strPSWFilePath= m_strAppPath + _T("\\SID_2FY.PSW");
	CFileFind finder;
	if(finder.FindFile(m_strPSWFilePath))
	{
		file = _wfopen(m_strPSWFilePath,_T("rb"));
		if(file == NULL)
		{
			CString formatStr;
			formatStr.LoadString(IDS_ERROR_PWDFILE);
            str.Format(formatStr, m_strPSWFilePath);
			CdMessageBox box(this->m_pMainWnd, str);
		    box.MsgBoxEx();
			return FALSE;
		}
		fseek(file,0,SEEK_END);
		nFileLength = ftell(file);
		if(nFileLength == 33)
		{
			fseek(file,0,SEEK_SET);
			fread(m_strCurrentPSW,sizeof(char),33,file);
			fclose(file);
			file = NULL;
			return TRUE;
		}
		else
		{
			fclose(file);
			file = NULL;
			//CString lang = theApp.m_pLangManger->m_strCurLanguage;
			CString formatStr;
			formatStr.LoadString(IDS_STRING_PWDW);
		    str.Format(formatStr, m_strPSWFilePath,m_strPSWFilePath);
		//	str.Format(_T("文件：%s\r\n与装置的密钥文件同名，但却并非本装置的密钥文件，\r\n所以本软件获取密钥失败，无法继续运行\r\n建议解决方案：\r\n\t1. 将本上位机软件单独放入一个目录下再运行\r\n\t2. 删除文件：%s （如果此文件不重要）"),m_strPSWFilePath,m_strPSWFilePath);
			CdMessageBox box(this->m_pMainWnd, str);
		    box.MsgBoxEx();
			return FALSE;
		}
	}
	else
	{
		file = _wfopen(m_strPSWFilePath,_T("wb"));
		if(file == NULL)
		{
			CString info;
			info.LoadString(IDS_STRING_PWDCF);
			CdMessageBox box(this->m_pMainWnd, info);
		    box.MsgBoxEx();
			return FALSE;
		}
		pStr = MD5String(strDefaultPSW);
		strcpy(m_strCurrentPSW,pStr);
		free(pStr);
		int nStrLen = strlen(m_strCurrentPSW);
		ASSERT(nStrLen == 32);
		nStrLen++;
		//2014-5-21 modiy
		//free(pStr);
		if(nStrLen == fwrite(m_strCurrentPSW,sizeof(char),nStrLen,file))
		{
			fclose(file);
			file = NULL;
			return TRUE;
		}
		else
		{
			fclose(file);
			file = NULL;
			CString info;
			info.LoadString( IDS_WRITE_PWDF );
			CdMessageBox box(this->m_pMainWnd, info);
		    box.MsgBoxEx();
			return FALSE;	
		}
	}
}

BOOL CMySID_2FYApp::SetCurPassWord(const char *strPSW)
{
	ASSERT(strPSW);
	if(strlen(strPSW) != strlen(m_strCurrentPSW))
	{
		return FALSE;
	}
	FILE *file = _wfopen(m_strPSWFilePath,_T("wb+"));
	if(file == NULL)
	{
		return FALSE;
	}
	int nStrLen = strlen(strPSW)+1;
	if(nStrLen == fwrite(strPSW,sizeof(char),nStrLen,file))
	{
		fclose(file);
		strcpy(m_strCurrentPSW,strPSW);
		return TRUE;
	}
	else
	{
		fclose(file);
		return FALSE;
	}
}

int CMySID_2FYApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pLangManger->RealsePointOfLangManger();
	return CWinApp::ExitInstance();
}
