#if !defined(AFX_EVENTPROGRESSDLG_H__CB919B40_ECD8_406A_B824_555DA6A16474__INCLUDED_)
#define AFX_EVENTPROGRESSDLG_H__CB919B40_ECD8_406A_B824_555DA6A16474__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventProgressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventProgressDlg dialog

class CEventProgressDlg : public CDialog
{
// Construction
public:
	void SetProgressPos(int pos);
	void SetEvevntType(CString str,BOOL bIsShowProgress=FALSE,int nUp=0,int nLow=0);
	CEventProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventProgressDlg)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	m_ctlProgress;
	CString	m_strTypeInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventProgressDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTPROGRESSDLG_H__CB919B40_ECD8_406A_B824_555DA6A16474__INCLUDED_)
