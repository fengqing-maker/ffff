#if !defined(AFX_STATUSBAREX_H__9082DA42_461A_4A40_9FE6_E5A5174F3E10__INCLUDED_)
#define AFX_STATUSBAREX_H__9082DA42_461A_4A40_9FE6_E5A5174F3E10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatusBarEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx window

class CStatusBarEx : public CStatusBar
{
// Construction
public:
	CStatusBarEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusBarEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTextColor(int index,COLORREF color);
	virtual ~CStatusBarEx();

	// Generated message map functions
protected:
	COLORREF *m_pColor;
	//{{AFX_MSG(CStatusBarEx)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSBAREX_H__9082DA42_461A_4A40_9FE6_E5A5174F3E10__INCLUDED_)
