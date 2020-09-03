#if !defined(AFX_PWDDLG_H__2CB79CA9_C245_475C_A44E_CE98D9C8F0C6__INCLUDED_)
#define AFX_PWDDLG_H__2CB79CA9_C245_475C_A44E_CE98D9C8F0C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PWDDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPWDDlg dialog

class CPWDDlg : public CDialog
{
// Construction
public:
	BOOL m_bCorrect;
	CPWDDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPWDDlg)
	enum { IDD = IDD_PASSWORD };
	CString	m_nPassWord;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPWDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPWDDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWDDLG_H__2CB79CA9_C245_475C_A44E_CE98D9C8F0C6__INCLUDED_)
