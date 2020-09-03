// MyView.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PreView.h"

/////////////////////////////////////////////////////////////////////////////
// CMyView

IMPLEMENT_DYNCREATE(CMyView, CScrollView)

CMyView::CMyView()
{
}

CMyView::~CMyView()
{
}


BEGIN_MESSAGE_MAP(CMyView, CScrollView)
	//{{AFX_MSG_MAP(CMyView)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyView drawing

void CMyView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CMyView diagnostics

#ifdef _DEBUG
void CMyView::AssertValid() const
{
	CView::AssertValid();
}

void CMyView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyView message handlers

void CMyView::OnFilePrintPreview() 
{
	CPrintPreviewState* pState = new CPrintPreviewState;

	if (!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this,
							RUNTIME_CLASS(CPreView), pState))
	{
		TRACE0("Error: DoPrintPreview failed.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		delete pState;      // preview failed to initialize, delete State now
	}	
}


BOOL CMyView::DoPrintPreview(UINT nIDResource, CMyView * pPrintView, 
							 CRuntimeClass * pPreviewViewClass, CPrintPreviewState * pState)
{
	ASSERT_VALID_IDR(nIDResource);
	ASSERT_VALID(pPrintView);
	ASSERT(pPreviewViewClass != NULL);
	ASSERT(pPreviewViewClass->IsDerivedFrom(RUNTIME_CLASS(CPreView)));
	ASSERT(pState != NULL);

	CFrameWnd* pParent;
	CWnd* pNaturalParent = pPrintView->GetParentFrame();
	pParent = DYNAMIC_DOWNCAST(CFrameWnd, pNaturalParent);
	if (pParent == NULL || pParent->IsIconic())
		pParent = (CFrameWnd*)AfxGetThread()->m_pMainWnd;

	ASSERT_VALID(pParent);
	ASSERT_KINDOF(CFrameWnd, pParent);

	CCreateContext context;
	context.m_pCurrentFrame = pParent;
	context.m_pCurrentDoc = GetDocument();
	context.m_pLastView = this;

	// Create the preview view object
	CPreView* pView = (CPreView*)pPreviewViewClass->CreateObject();
	if (pView == NULL)
	{
		TRACE0("Error: Failed to create preview view.\n");
		return FALSE;
	}
	ASSERT_KINDOF(CPreView, pView);
	pView->m_pPreviewState = pState;        // save pointer

	pParent->OnSetPreviewMode(TRUE, pState);    // Take over Frame Window

#ifdef _MAC
	if (nIDResource == AFX_IDD_PREVIEW_TOOLBAR)
	{
		HINSTANCE hInst = AfxFindResourceHandle(
			MAKEINTRESOURCE(AFX_IDD_PREVIEW_TOOLBAR), RT_DIALOG);
		HRSRC hResource = FindResource(hInst,
			MAKEINTRESOURCE(AFX_IDD_PREVIEW_TOOLBAR), RT_DIALOG);

		HGLOBAL hdt = NULL;
		if (hResource != NULL)
			hdt = LoadResource(hInst, hResource);

		DLGTEMPLATE* pdt = NULL;
		if (hdt != NULL)
			pdt = (DLGTEMPLATE*)LockResource(hdt);

		if (pdt != NULL)
		{
			CRect rectParent;
			pParent->GetClientRect(&rectParent);

			int cxToolbar = MulDiv(LOWORD(GetDialogBaseUnits()), pdt->cx, 4);

			if (cxToolbar > rectParent.Width())
				nIDResource = AFX_IDD_PREVIEW_SHORTTOOLBAR;

			UnlockResource(hdt);
			FreeResource(hdt);
		}
	}
#endif

	// Create the toolbar from the dialog resource
	pView->m_pToolBar = new CDialogBar;

	//**********************************
    nIDResource = IDD_MY_PREVIEWDIALOG;
    //**********************************

	if (!pView->m_pToolBar->Create(pParent, MAKEINTRESOURCE(nIDResource),
		                           CBRS_TOP, AFX_IDW_PREVIEW_BAR))
	{
		TRACE0("Error: Preview could not create toolbar dialog.\n");
		pParent->OnSetPreviewMode(FALSE, pState);   // restore Frame Window
		delete pView->m_pToolBar;       // not autodestruct yet
		pView->m_pToolBar = NULL;
		pView->m_pPreviewState = NULL;  // do not delete state structure
		delete pView;
		return FALSE;
	}
	pView->m_pToolBar->m_bAutoDelete = TRUE;    // automatic cleanup

	// Create the preview view as a child of the App Main Window.  This
	// is a sibling of this view if this is an SDI app.  This is NOT a sibling
	// if this is an MDI app.

	if (!pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), pParent, AFX_IDW_PANE_FIRST, &context))
	{
		TRACE0("Error: couldn't create preview view for frame.\n");
		pParent->OnSetPreviewMode(FALSE, pState);   // restore Frame Window
		pView->m_pPreviewState = NULL;  // do not delete state structure
		delete pView;
		return FALSE;
	}

	// Preview window shown now

	pState->pViewActiveOld = pParent->GetActiveView();
	CMyView* pActiveView = (CMyView*)pParent->GetActiveFrame()->GetActiveView();
	if (pActiveView != NULL)
		pActiveView->OnActivateView(FALSE, pActiveView, pActiveView);

	if (!pView->SetPrintView(pPrintView))
	{
		pView->OnPreviewClose();
		return TRUE;            // signal that OnEndPrintPreview was called
	}

	pParent->SetActiveView(pView);  // set active view - even for MDI

	// update toolbar and redraw everything
	pView->m_pToolBar->SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE);
	pParent->RecalcLayout();            // position and size everything
	pParent->UpdateWindow();

	return TRUE;
}

void CMyView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, 
								POINT point, 
								CPreView* pView) 
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pView);
	CRect rc;
	GetParentFrame()->GetWindowRect(&rc);
	if (pView->m_pPrintView != NULL)
		pView->m_pPrintView->OnEndPrinting(pDC, pInfo);
	
	CFrameWnd* pParent;
	CWnd* pNaturalParent = pView->GetParentFrame();
	
	pParent = DYNAMIC_DOWNCAST(CFrameWnd, pNaturalParent);
	if (pParent == NULL || pParent->IsIconic())
		pParent = (CFrameWnd*)AfxGetThread()->m_pMainWnd;
	
	
	ASSERT_VALID(pParent);
	ASSERT_KINDOF(CFrameWnd, pParent);

	// restore the old main window
	pParent->OnSetPreviewMode(FALSE, pView->m_pPreviewState);

	// Force active view back to old one
	pParent->SetActiveView(pView->m_pPreviewState->pViewActiveOld);
	if (pParent != GetParentFrame())
		OnActivateView(TRUE, this, this);   // re-activate view in real frame
	pView->DestroyWindow();     // destroy preview view
			// C++ object will be deleted in PostNcDestroy

	// restore main frame layout and idle message
	pParent->MoveWindow(&rc);
	pParent->RecalcLayout();
	pParent->SendMessage(WM_SETMESSAGESTRING, (WPARAM)AFX_IDS_IDLEMESSAGE, 0L);
	pParent->UpdateWindow();
}
