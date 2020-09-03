#pragma once
#if !defined CDMESSAGEBOX_2134_HEARD_1
#define  CDMESSAGEBOX_2134_HEARD_1

#include "afxtempl.h"
LRESULT CALLBACK SetHook(int nCode,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK HookWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void CreateButton(HWND hWndOwner);
CRect SizeToContent(HWND hWndOwner,CSize ChildSize);
int SetOut(WPARAM wParam);
void SetIcon(HWND Owner,HICON hIcon);
void UnInstallHook(HHOOK m_gHook);
void SetCaption(HWND Owner);
// CdMessageBox
extern void InitMessageBox();

class CdMessageBox : public CWnd
{
	DECLARE_DYNAMIC(CdMessageBox)

public:
	CdMessageBox(CWnd*pParentWnd,LPCTSTR prompt,LPCTSTR title = _T(""), DWORD dwStyle = MB_OK);
	HHOOK InstallHook();
	virtual ~CdMessageBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	int MsgBoxEx(void);

protected:
	CRect SizeToContent(HWND hWndOwner,HWND hChild,CSize ChildSize);
public:

	
	void SetMsgIcon(HICON hIcon);
	void SetCaptions(UINT nID,CString strCaption);
};


#endif //CDMESSAGEBOX_2134_HEARD_1