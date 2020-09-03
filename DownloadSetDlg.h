#if !defined(AFX_DOWNLOADSETDLG_H__E1D23CDA_6344_4331_9448_73450454181A__INCLUDED_)
#define AFX_DOWNLOADSETDLG_H__E1D23CDA_6344_4331_9448_73450454181A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DownloadSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDownloadSetDlg dialog
#include "XListCtrl.h"
#define WM_DOWNLOAD_FINISH (WM_USER+200)
#define WM_FILE_INFO (WM_USER+201)
#define WM_FILE_SAVE_AS (WM_USER+202)

class CDownloadSetDlg : public CDialog
{
// Construction
public:
	HANDLE hThread;
	BYTE   *m_fileBuf;    //录波文件最大1.5M
	BYTE m_nWareHead;                                  //当前设备中的录波文件数
	WareHead m_arrWareHead[MAX_DEVICE_SAVE_WARE_FILE_COUNT];  //临时存储设备中的录波事件概览
	CDownloadSetDlg(CWnd* pParent = NULL);   // standard constructor
    ~CDownloadSetDlg( );
// Dialog Data
	//{{AFX_DATA(CDownloadSetDlg)
	enum { IDD = IDD_DOWNLOAD_WARE_FILE };
	CXListCtrl	m_wareFileList;
	//}}AFX_DATA
	
	LRESULT OnFileSaveAs(WPARAM wParam,LPARAM lParam);
	LRESULT OnDownloadFinish(WPARAM wParam,LPARAM lParam);
	LRESULT OnFileInfo(WPARAM wParam,LPARAM lParam);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitListCtrl(CXListCtrl& list);
	// Generated message map functions
	//{{AFX_MSG(CDownloadSetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnFinishDownload();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNLOADSETDLG_H__E1D23CDA_6344_4331_9448_73450454181A__INCLUDED_)
