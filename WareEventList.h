#if !defined(AFX_WAREEVENTLIST_H__45487878_CDF2_4A8E_913B_60783370C175__INCLUDED_)
#define AFX_WAREEVENTLIST_H__45487878_CDF2_4A8E_913B_60783370C175__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WareEventList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWareEventList window
struct CEventDescribe;
class CWareEventList : public CListCtrl
{
// Construction
public:
	CWareEventList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWareEventList)
	//}}AFX_VIRTUAL

// Implementation
private:
	void GetEventInfo(const int nItem,CEventDescribe &Event);
public:
	virtual ~CWareEventList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWareEventList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAREEVENTLIST_H__45487878_CDF2_4A8E_913B_60783370C175__INCLUDED_)
