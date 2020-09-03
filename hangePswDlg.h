#if !defined(AFX_HANGEPSWDLG_H__03684048_BF29_4D7D_B67C_AD5AB6C53967__INCLUDED_)
#define AFX_HANGEPSWDLG_H__03684048_BF29_4D7D_B67C_AD5AB6C53967__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// hangePswDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ChangePswDlg dialog

class ChangePswDlg : public CDialog
{
// Construction
public:
	BOOL m_bCorrect;
	ChangePswDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ChangePswDlg)
	enum { IDD = IDD_MODIFY_PSW };
	CString	m_nOriginalPsw;
	CString	m_nModifyPsw1;
	CString	m_nModifyPsw2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ChangePswDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ChangePswDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HANGEPSWDLG_H__03684048_BF29_4D7D_B67C_AD5AB6C53967__INCLUDED_)
