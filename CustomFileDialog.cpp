// CustomFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include <boost/regex.hpp>
#include "SID_2FY.h"
#include "CustomFileDialog.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog

IMPLEMENT_DYNAMIC(CCustomFileDialog, CFileDialog)

CCustomFileDialog::CCustomFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}
BOOL CCustomFileDialog::OnFileNameOK()
{
	CString strFileName;
	strFileName = CFileDialog::GetFileTitle();
	boost::regex expression("^[^\\\\/:*?\"<>|]*([\\\\/:*?\"<>|]{1,})[^\\\\/:*?\"<>|]*$");
	boost::cmatch what;
	char tempFileName[255];
	WideCharToMultiByte(CP_ACP,0,strFileName,-1,tempFileName,255,NULL,NULL);
	if(boost::regex_match(tempFileName, what, expression))//CStringתstring
	{
		CString info;
		info.LoadString(IDS_PRJNAME_ERROR);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return TRUE;
	}
	return FALSE;
}

BEGIN_MESSAGE_MAP(CCustomFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CCustomFileDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

