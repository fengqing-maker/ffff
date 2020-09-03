// WareView.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "WareView.h"
#include "DownloadSetDlg.h"
#include "CustomizeWareSetDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWareView

IMPLEMENT_DYNCREATE(CWareView, CFormView)
extern CMySID_2FYApp theApp;
CWareView::CWareView()
	: CFormView(CWareView::IDD)
{
	//{{AFX_DATA_INIT(CWareView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWareView::~CWareView()
{
}

void CWareView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWareView, CFormView)
	//{{AFX_MSG_MAP(CWareView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWareView diagnostics

#ifdef _DEBUG
void CWareView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWareView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWareView message handlers

void CWareView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}
void CWareView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	UpdateData(FALSE);

}


