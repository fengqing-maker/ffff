// SID_2FYDoc.h : interface of the CMySID_2FYDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SID_2FYDOC_H__C934EFD1_A1FC_46EB_889D_881C799D9C98__INCLUDED_)
#define AFX_SID_2FYDOC_H__C934EFD1_A1FC_46EB_889D_881C799D9C98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMySID_2FYDoc : public CDocument
{
protected: // create from serialization only
	CMySID_2FYDoc();
	DECLARE_DYNCREATE(CMySID_2FYDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySID_2FYDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMySID_2FYDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMySID_2FYDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SID_2FYDOC_H__C934EFD1_A1FC_46EB_889D_881C799D9C98__INCLUDED_)
