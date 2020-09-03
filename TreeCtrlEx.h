#if !defined(AFX_TREECTRLEX_H__9052D2D7_9AFF_4FB8_86AB_49EAE36AE394__INCLUDED_)
#define AFX_TREECTRLEX_H__9052D2D7_9AFF_4FB8_86AB_49EAE36AE394__INCLUDED_

#include "WareFileData.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx window
typedef map <CString,UINT> CStringUINT;
class CTreeCtrlEx : public CTreeCtrl
{
// Construction
public:
	CTreeCtrlEx();

// Attributes
public:

// Operations
public:
	virtual int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlEx)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	CStringUINT m_mapTreeItem;
	CStringUINT::iterator m_tempItem;
	BOOL SetWareFileData(CWareFileData *pWareFileData);
	virtual ~CTreeCtrlEx();
	
	// Generated message map functions
protected:
	CWareFileData *m_pWareFileData;
	//{{AFX_MSG(CTreeCtrlEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREECTRLEX_H__9052D2D7_9AFF_4FB8_86AB_49EAE36AE394__INCLUDED_)
