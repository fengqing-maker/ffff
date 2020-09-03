#if !defined(AFX_FILEINFODLG_H__2CF91561_92B0_4D01_AD3F_AA4854A7DD44__INCLUDED_)
#define AFX_FILEINFODLG_H__2CF91561_92B0_4D01_AD3F_AA4854A7DD44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileInfoDlg dialog

class CFileInfoDlg : public CDialog
{
// Construction
public:
	CString m_strFileType;
	UINT m_nID_FilterIcon;
	void LoadFolder();
	CImageList m_imageList;
	CFileInfoDlg(CString strInitialDir=_T(""),CString strFileType=_T(""),UINT nID_Icon=0,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileInfoDlg)
	enum { IDD = IDD_FILE_INFO_DLG };
	CComboBox	m_ctlFileType;
	CListCtrl	m_listFileList;
	CString	m_strFileHead;
	CString	m_strFileName;
	CString	m_strFileTail;
	CString m_strFileTailDis;
	CString	m_strFolderPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileInfoDlg)
	afx_msg void OnSortFolderBtn();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEINFODLG_H__2CF91561_92B0_4D01_AD3F_AA4854A7DD44__INCLUDED_)
