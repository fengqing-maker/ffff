#if !defined(AFX_SETIP_H__1C8639B4_4AE0_40DB_A759_DB27F6EE4766__INCLUDED_)
#define AFX_SETIP_H__1C8639B4_4AE0_40DB_A759_DB27F6EE4766__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetIP.h : header file
//
#include "CreateProjectDlg.h"
#include "Setprojectdlg.h"
/////////////////////////////////////////////////////////////////////////////
// CSetIP dialog

class CSetIP : public CDialog
{
// Construction
public:
	CSetIP(CWnd* pParent = NULL);   // standard constructor
	class CCreateProjectDlg *pParent;    //父窗口是创建工程对话框
	class CSetProjectDlg *setParent;     //父窗口是设置工程对话框
// Dialog Data
	//{{AFX_DATA(CSetIP)
	enum { IDD = IDD_IP_SET };
	CIPAddressCtrl	m_IP;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetIP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetIP)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETIP_H__1C8639B4_4AE0_40DB_A759_DB27F6EE4766__INCLUDED_)
