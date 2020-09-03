// TestToolTip.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "TestToolTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestToolTip

IMPLEMENT_DYNCREATE(CTestToolTip, CFormView)

CTestToolTip::CTestToolTip()
	: CFormView(CTestToolTip::IDD)
{
	//{{AFX_DATA_INIT(CTestToolTip)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTestToolTip::~CTestToolTip()
{
}

void CTestToolTip::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestToolTip)
	DDX_Control(pDX, IDC_LIST1, m_ctlMyToolTip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestToolTip, CFormView)
	//{{AFX_MSG_MAP(CTestToolTip)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestToolTip diagnostics

#ifdef _DEBUG
void CTestToolTip::AssertValid() const
{
	CFormView::AssertValid();
}

void CTestToolTip::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestToolTip message handlers

void CTestToolTip::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CRect rectListCtrl;
	
	m_ctlMyToolTip.EnableToolTips(TRUE);
	
	m_ctlMyToolTip.SetRedraw(FALSE);
	
	m_ctlMyToolTip.GetClientRect(&rectListCtrl);
	int nColInterval = rectListCtrl.Width()/3;
	
	m_ctlMyToolTip.InsertColumn(0, _T("Column 1"), LVCFMT_LEFT, nColInterval);
	m_ctlMyToolTip.InsertColumn(1, _T("Column 2"), LVCFMT_LEFT, nColInterval);
	m_ctlMyToolTip.InsertColumn(2, _T("Column 3"), LVCFMT_LEFT, nColInterval);
	
	LVITEM lvi;
	CString strItem;
	for (int i = 0; i < 10000; i++) {
		
		lvi.mask = LVIF_IMAGE | LVIF_TEXT;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		strItem.Format(_T("Item (%d, %d)"), i, 0);
		lvi.pszText = (LPTSTR)(LPCTSTR)(strItem);
		
		m_ctlMyToolTip.InsertItem(&lvi);
		
		strItem = "This is the " + strItem;
		
		//m_ctlMyToolTip.SetItemToolTipText(i, 0, strItem);
		
		
		lvi.iSubItem = 1;
		strItem.Format(_T("Item (%d, %d)"), i, 1);
		lvi.pszText = (LPTSTR)(LPCTSTR)(strItem);
		
		m_ctlMyToolTip.SetItem(&lvi);
		
		strItem = "This is the " + strItem;	
		
		m_ctlMyToolTip.SetItemToolTipText(i, 1, strItem);
		
		
		lvi.iSubItem = 2;
		strItem.Format(_T("Item (%d, %d)"), i, 2);
		lvi.pszText = (LPTSTR)(LPCTSTR)(strItem);
		
		m_ctlMyToolTip.SetItem(&lvi);
		
		strItem = "This is the " + strItem;
		
		//m_ctlMyToolTip.SetItemToolTipText(i, 2, strItem);
		
		
		
		
	}
	
	
	m_ctlMyToolTip.SetRedraw(TRUE);
}
