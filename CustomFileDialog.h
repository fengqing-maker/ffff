#if !defined(AFX_CUSTOMFILEDIALOG_H__43B08D54_8A9E_4602_9D5C_E2D312DE0F6A__INCLUDED_)
#define AFX_CUSTOMFILEDIALOG_H__43B08D54_8A9E_4602_9D5C_E2D312DE0F6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog dialog
#include <Afxdlgs.h>
class CCustomFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CCustomFileDialog)
public:
	CCustomFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual BOOL OnFileNameOK();
protected:
	//{{AFX_MSG(CCustomFileDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMFILEDIALOG_H__43B08D54_8A9E_4602_9D5C_E2D312DE0F6A__INCLUDED_)
