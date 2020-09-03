#if !defined(AFX_SETPROJECTDLG_H__958A3A1C_7CD0_4389_872C_4BC374C4D821__INCLUDED_)
#define AFX_SETPROJECTDLG_H__958A3A1C_7CD0_4389_872C_4BC374C4D821__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetProjectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetProjectDlg dialog

class CSetProjectDlg : public CDialog
{
// Construction
public:
	unsigned long m_setIP;
	CSetProjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetProjectDlg)
	enum { IDD = IDD_SET_PROJECT_DLG };
	CComboBox	m_comboLink;
//	CIPAddressCtrl	m_ipAddrDevice;
	CString	m_strDBFileName;
	CString	m_strProjectName;
	CString	m_strProjectType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetProjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetProjectDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboLink();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPROJECTDLG_H__958A3A1C_7CD0_4389_872C_4BC374C4D821__INCLUDED_)
