#if !defined(AFX_TESTTOOLTIP_H__D74935D5_1D88_4D79_A539_E0002BB3D274__INCLUDED_)
#define AFX_TESTTOOLTIP_H__D74935D5_1D88_4D79_A539_E0002BB3D274__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestToolTip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestToolTip form view
#include "ToolTipListCtrl.h"
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CTestToolTip : public CFormView
{
protected:
	CTestToolTip();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTestToolTip)

// Form Data
public:
	//{{AFX_DATA(CTestToolTip)
	enum { IDD = IDD_FORMVIEW };
	CToolTipListCtrl	m_ctlMyToolTip;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestToolTip)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTestToolTip();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTestToolTip)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTTOOLTIP_H__D74935D5_1D88_4D79_A539_E0002BB3D274__INCLUDED_)
