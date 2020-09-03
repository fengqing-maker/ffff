#if !defined(AFX_WAREVIEW_H__06204239_E593_44FD_B5FE_662B8C6F952D__INCLUDED_)
#define AFX_WAREVIEW_H__06204239_E593_44FD_B5FE_662B8C6F952D__INCLUDED_
#define ID_CUSTOMIZE_WARE_DISPLAY 0X01
#define ID_SYSTEM_WARE_DISPLAY 0X02
#define ID_CLEAR_WARE_FILE 0X03
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WareView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWareView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
class CWareView : public CFormView
{
protected:
	CWareView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWareView)

// Form Data
public:
	//{{AFX_DATA(CWareView)
	enum { IDD = IDD_WARE };
	CListCtrl	m_wareFileList;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	CRect m_rectClient;
// 	CCurveWnd m_curveWnd;
// 	CCurveWnd m_curveWndCustomize;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWareView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWareView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CWareView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
// 	afx_msg void OnGetWareData();
// 	afx_msg void OnRclickWareFileList(NMHDR* pNMHDR, LRESULT* pResult);
// 	afx_msg void OnCustomsizeDisplay();
// 	afx_msg void OnClearFromTable();
// 	afx_msg void OnSaveWareFile();
// 	afx_msg void OnOpenWareFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAREVIEW_H__06204239_E593_44FD_B5FE_662B8C6F952D__INCLUDED_)
