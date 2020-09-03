#if !defined(AFX_DEFINESCALE_H__88013E59_1AA1_4A21_BDC5_30FFC955DAFC__INCLUDED_)
#define AFX_DEFINESCALE_H__88013E59_1AA1_4A21_BDC5_30FFC955DAFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefineScale.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDefineScale dialog

class CDefineScale : public CDialog
{
// Construction
public:
	static int m_curSel;
	CDefineScale(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefineScale)
	enum { IDD = IDD_DEFINE_SCALE };
	CComboBox	m_comboScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefineScale)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineScale)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFINESCALE_H__88013E59_1AA1_4A21_BDC5_30FFC955DAFC__INCLUDED_)
