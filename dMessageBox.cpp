//***********************IN THE NAME OF GOD********************//
//This file is created for free using for Visual C++ users.
//You can use this class without any conditions except using the name:
//KUASE Corp 2005.
//You can use this classe by:
//1-#inlcude "dMessageBox.h"
//2-for example CdMessageBox*box=new CdMessageBox(...)
//3-If you want to use your own icon use box->SetMsgIcon(...)
//4-Call the int result=MsgBoxEx() function to show the messagebox
//5-If the return value is -n it means the checkbox is checked by 
//user. by using abs(...) function you get the positive value.
//Created by KUASE Corp 2004-2005 using Microsoft Foundation Classes
//(MFC).
//send me your comments to :MiladC@gmail.com
//Thank you.

#include "stdafx.h"
#include "dMessageBox.h"
#include ".\dmessagebox.h"
#include "afxtempl.h"
#include "resource.h"
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#define IDC_CHECKBOX 1000

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//Global variables. So we can use them everywhere
//In this .cpp file.
HHOOK g_hHook;
WNDPROC oldProc;
CWnd *OwnerWnd;
CString m_strPrompt;
CString strCheck;
CString m_strTitle;
DWORD dw_style;
HICON m_hIcon;
CString m_strCaption;
bool m_bChecked;
UINT m_nID;
CArray<CString,CString> strar;
CButton *btn = NULL;
// CdMessageBox

IMPLEMENT_DYNAMIC(CdMessageBox, CWnd)
CdMessageBox::CdMessageBox(CWnd*pParentWnd,LPCTSTR prompt, LPCTSTR title/* = _T("")*/, DWORD dwStyle/* = MB_OK*/)
{
	//global variables  load here
	OwnerWnd=pParentWnd;
	m_strPrompt=prompt;
//	strCheck=CheckPtr;
	if (_T("") != title)
	{
		m_strTitle=title;
	}else
	{
		m_strTitle.LoadString( AFX_IDS_APP_TITLE );
	}
	dw_style = dwStyle;
	m_bChecked=false;

	//Install the hook so we can get messages from messagebox
	InstallHook();
}
CdMessageBox::~CdMessageBox()
{
	if (NULL != btn )
	{
		delete btn;
		btn = NULL;
	}
}
BEGIN_MESSAGE_MAP(CdMessageBox, CWnd)
END_MESSAGE_MAP()

HHOOK CdMessageBox::InstallHook(void)
{
	//Installs a hook procedure that monitors messages before the 
	//system sends them to the destination window procedure.
	//For more information, see the CallWndProc hook procedure.
	return g_hHook = SetWindowsHookEx(WH_CALLWNDPROC,
                               (HOOKPROC)SetHook,//procedure we use.
                               NULL,
                               GetCurrentThreadId());//retrieves the thread identifier of the calling thread
}
LRESULT CALLBACK SetHook(int nCode,WPARAM wParam,LPARAM lParam)
{
if (nCode==HC_ACTION)//Checks if an action is beeing active
{
	CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;//variable from CWPSTRUCT
	//that contains informations about window messages

	if (pwp->message==WM_INITDIALOG)//If window is being created so...

		//let us handle the message box
	oldProc=(WNDPROC)SetWindowLong(pwp->hwnd,GWL_WNDPROC,(LONG)HookWndProc); 
}
//Let the OS handle it!
return CallNextHookEx(g_hHook,nCode,wParam,lParam);
}
LRESULT CALLBACK HookWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//Let OS to perform basic operation
	LRESULT rc = CallWindowProc( oldProc, hWnd, uMsg, wParam, lParam );

	//If MessageBox wants to load, do something
	if (uMsg==WM_INITDIALOG)
	{
	   if (m_hIcon)
		 SetIcon(hWnd,m_hIcon);
	   CreateButton(hWnd);	

	   if (m_strCaption)
		 SetCaption(hWnd);
	}
	if (uMsg==WM_COMMAND&&wParam==IDC_CHECKBOX)
		//If user clicked the check box handle it!
		SetOut(wParam);
	if (uMsg==WM_NCDESTROY)
		//On exit Uninstall the hook procedure
		//If we don't do that every message box will have 
		//A check box on it!
		UnInstallHook(g_hHook);
return rc;
}
/*************************************************************************
函数名:InitMessageBox
函数描述：初始化
函数参数：无
函数返回：无
**************************************************************************/
void InitMessageBox()
{
    strar.SetSize(8,-1); 
	CString info;
	info.LoadString( IDS_STR_OK );
	strar[IDOK]  = info;
	info.LoadString(IDS_STR_CANCEL);
	strar[IDCANCEL] = info;
	info.LoadString(IDS_STR_ABORT);
    strar[IDABORT] = info;
	info.LoadString( IDS_STR_RETRY );
	strar[IDRETRY] = info;
	info.LoadString( IDS_STR_IGNORE );
	strar[IDIGNORE] = info;
	info.LoadString( IDS_STR_YES );
    strar[IDYES] = info;
    info.LoadString( IDS_STR_NO );
	strar[IDNO] = info;
}

int CdMessageBox::MsgBoxEx(void)
{
	//The main function
	int out=::MessageBox(OwnerWnd->m_hWnd ,m_strPrompt,m_strTitle,dw_style);
	if (m_bChecked)
		out=-out;
	return out;
}
void CreateButton(HWND hWndOwner)
{
		//Get the height and width we need for this text
		CDC*pDC=CWnd::FromHandle(hWndOwner)->GetDC();
		CSize size=pDC->GetTextExtent(strCheck); 
		
		//This function make the appropriate size for the dialog
		//Checks that checkbox width won't be wider that dialog
		//Width.
		CRect rc=SizeToContent(hWndOwner,size);

		//Get the default system font from its static text
		HWND hStatic=GetDlgItem(hWndOwner,65535);
		HFONT sysFont=(HFONT)::SendMessage(hStatic,WM_GETFONT,0,0); 

		//Create the button with a appropriate rect
		CRect lpRect(CPoint(rc.Width()-rc.Width()+13,rc.Height()-size.cy*2 ),size);
		if (NULL != btn )
		{
           delete btn;
		   btn = NULL;
		}
		btn =new CButton;
		btn->Create(strCheck,WS_VISIBLE|BS_AUTOCHECKBOX |BS_CHECKBOX,lpRect,CWnd::FromHandle(hWndOwner),1000);  

		//Set the system font for the checkbox
		btn->SetFont(CFont::FromHandle(sysFont),TRUE);
}
CRect SizeToContent(HWND hWndOwner,CSize ChildSize)
{
	WINDOWPLACEMENT pl;
	::GetWindowPlacement(hWndOwner,&pl);
	CRect rc=pl.rcNormalPosition; 

	//make sure the dialog is wide enough for the check box
	if (ChildSize.cx<rc.Width() )
		::MoveWindow(hWndOwner,rc.left,rc.top,rc.Width(),rc.Height()+ChildSize.cy+7,TRUE);       
	return rc;
}
int SetOut(WPARAM wParam)
{
	//if it is true change it to false and if not...
	m_bChecked=!m_bChecked;
	return m_bChecked;
}
void CdMessageBox::SetMsgIcon(HICON hIcon)
{
	m_hIcon=hIcon;
}
void SetIcon(HWND Owner,HICON hIcon)
{
	//This function sets the user icon for the messagebox
	HWND hStatic=GetDlgItem(Owner,20);
	if (hStatic)
	{
	CStatic*st=(CStatic*)CStatic::FromHandle(hStatic);
	st->SetIcon(hIcon); 
	}
}
void UnInstallHook(HHOOK m_gHook)
{
	//Uninstall the hook!
	UnhookWindowsHookEx(m_gHook);
}
void SetCaption(HWND Owner)
{
	if (dw_style<=16)
	{
// 		SetDlgItemText(Owner,2,strar[IDOK]);
// 		//SetDlgItemText(Owner,2,strar[IDCANCEL]);
// 		SetDlgItemText(Owner,IDYES,strar[IDYES]);
// 		SetDlgItemText(Owner,IDNO,strar[IDNO]);
		for (int i = IDOK; i <= IDNO; i++)
		{
            SetDlgItemText(Owner, i, strar[i]);
		}
		if (MB_OK == dw_style )
		{
            SetDlgItemText(Owner, 2, strar[IDOK]);    
		}
		return;
	}
	for (UINT nID=1;nID<8;nID++)
	{
		if (!strar[nID].IsEmpty()) 
	SetDlgItemText(Owner,nID,strar[nID]);
	}
}
void CdMessageBox::SetCaptions(UINT nID,CString strCaption)
{
	strar.SetSize(8,-1); 
	strar[nID]=strCaption; 
}
