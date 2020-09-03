#if !defined(AFX_EVENTVIEW_H__8F4A2F5C_2F2E_461B_B3FB_7ABA512EAB9C__INCLUDED_)
#define AFX_EVENTVIEW_H__8F4A2F5C_2F2E_461B_B3FB_7ABA512EAB9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "AdoRecordset.h"	// Added by ClassView
#define ID_EVENT_DELETE_ALL 0X101
#define ID_EVENT_DELETE 0X102
#define IDD_EVENT_PROGRESS 0X103
#define ID_EVENT_OUT_TO_EXCEL 0X104
class CEventView : public CFormView
{
protected:
	CEventView();           // protected constructor used by dynamic creation
	BOOL QueryData();
	DECLARE_DYNCREATE(CEventView)

// Form Data
public:
	//{{AFX_DATA(CEventView)
	enum { IDD = IDD_EVENT };
	CComboBox	m_ctlEventType;
	CListCtrl	m_listEvent;
	COleDateTime	m_timeEnd;
	COleDateTime	m_timeStart;
	CString	m_strEventName;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	VOID OutEventToExcel();
	VOID ReadEventFile();
	int m_nEventCounts;
	CImageList m_imageList;
	CString m_strOrderBy;
	void UpdateEventType();
	BOOL GetDeviceEvent();
	CAdoRecordset m_recordset;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	long m_lSortedColumn;
	virtual ~CEventView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CEventView)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSortBtn();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickEventList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEventDelete();
	afx_msg void OnEventDeleteAll();
	afx_msg void OnEventOutToExcel();
	afx_msg void OnColumnclickEventList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInputEvent();
	afx_msg void OnGetEvent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTVIEW_H__8F4A2F5C_2F2E_461B_B3FB_7ABA512EAB9C__INCLUDED_)
