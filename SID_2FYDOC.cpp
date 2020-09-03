// SID_2FYDoc.cpp : implementation of the CMySID_2FYDoc class
//

#include "stdafx.h"
#include "SID_2FY.h"

#include "SID_2FYDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYDoc
extern CMySID_2FYApp theApp;
IMPLEMENT_DYNCREATE(CMySID_2FYDoc, CDocument)

BEGIN_MESSAGE_MAP(CMySID_2FYDoc, CDocument)
	//{{AFX_MSG_MAP(CMySID_2FYDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYDoc construction/destruction

CMySID_2FYDoc::CMySID_2FYDoc()
{
	// TODO: add one-time construction code here

}

CMySID_2FYDoc::~CMySID_2FYDoc()
{
}

BOOL CMySID_2FYDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYDoc serialization

void CMySID_2FYDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYDoc diagnostics

#ifdef _DEBUG
void CMySID_2FYDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMySID_2FYDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYDoc commands

void CMySID_2FYDoc::SetTitle(LPCTSTR lpszTitle) 
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::SetTitle((CString)theApp.m_projectData.m_strDeviceType);
}
