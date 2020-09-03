#if !defined(AFX_PREVIEW_H__03894546_1C39_11D4_B336_00104B13D514__INCLUDED_)
#define AFX_PREVIEW_H__03894546_1C39_11D4_B336_00104B13D514__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PreView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreView view

#include "afxpriv.h"

class CMyView;

class CPreView : public CScrollView
{

    DECLARE_DYNCREATE(CPreView)

protected:
	CPreView();           // protected constructor used by dynamic creation
	BOOL SetPrintView(CMyView* pPrintView);

// Attributes
public:
	CMyView* m_pOrigView;
	CMyView* m_pPrintView;
	CPreviewDC* m_pPreviewDC;  // Output and attrib DCs Set, not created
	CDC m_dcPrint;             // Actual printer DC

// Operations
	void SetZoomState(UINT nNewState, UINT nPage, CPoint point);
	void SetCurrentPage(UINT nPage, BOOL bClearRatios);

	// Returns TRUE if in a page rect. Returns the page index
	// in nPage and the point converted to 1:1 screen device coordinates
	BOOL FindPageRect(CPoint& point, UINT& nPage);

	// Returns .cx/.cy as the numerator/denominator pair for the ratio
	// using CSize for convenience
	virtual CSize CalcScaleRatio(CSize windowSize, CSize actualSize);

	virtual void PositionPage(UINT nPage);
	virtual void OnDisplayPageNumber(UINT nPage, UINT nPagesDisplayed);

// Operations
public:
	void ClosePreview();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
	void DoZoom(UINT nPage, CPoint point);
	void SetScaledSize(UINT nPage);
	CSize CalcPageDisplaySize();

	CPrintPreviewState* m_pPreviewState; // State to restore
	CDialogBar* m_pToolBar; // Toolbar for preview

	struct PAGE_INFO
	{
		CRect rectScreen; // screen rect (screen device units)
		CSize sizeUnscaled; // unscaled screen rect (screen device units) 打印纸对应到屏幕坐标的长宽
		CSize sizeScaleRatio; // scale ratio (cx/cy)
		CSize sizeZoomOutRatio; // scale ratio when zoomed out (cx/cy)
	};

	PAGE_INFO* m_pPageInfo; // Array of page info structures

	//***********************************************************
 	      //PAGE_INFO m_pageInfoArray[2]; // Embedded array for the default implementation

	 // 每屏可显示10页预览页
    PAGE_INFO m_pageInfoArray[10]; // Embedded array for the default implementation
    //***********************************************************

	BOOL m_bPageNumDisplayed;// Flags whether or not page number has yet
								// been displayed on status line
	UINT m_nZoomOutPages; // number of pages when zoomed out
	UINT m_nZoomState;
	UINT m_nMaxPages; // for sanity checks
	UINT m_nCurrentPage;
	UINT m_nPages;   // 预览每屏显示的页数

	int m_nSecondPageOffset; // used to shift second page position
	int m_nUpdownPageOffset; // 上下页之间的距离
	int m_nPreviewLine;      // 预览页行数
	int m_nLine;      // 行数


	HCURSOR m_hMagnifyCursor;

	CSize m_sizePrinterPPI; // printer pixels per inch
	CPoint m_ptCenterPoint;
	CPrintInfo* m_pPreviewInfo;

// Implementation
protected:
	virtual ~CPreView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPreView)
	afx_msg void OnPreviewClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNumPageChange();
	afx_msg void OnNextPage();
	afx_msg void OnPrevPage();
	afx_msg void OnPreviewPrint();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnThreePage();
	afx_msg void OnUpdateThreePage(CCmdUI* pCmdUI);
	afx_msg void OnSixPage();
	afx_msg void OnUpdateSixPage(CCmdUI* pCmdUI);
	afx_msg void OnFind();
	afx_msg void OnUpdateNumPageChange(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextPage(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrevPage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);	
	afx_msg void OnFileNew();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

    friend class CMyView;
	friend BOOL CALLBACK _AfxPreviewCloseProc(CFrameWnd* pFrameWnd);

};

// Zoom States
#define ZOOM_OUT    0
#define ZOOM_MIDDLE 1
#define ZOOM_IN     2


class CMyView : public CScrollView
{
protected:
	CMyView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyView)

// Attributes
public:

// Operations
public:
	BOOL DoPrintPreview(UINT nIDResource, CMyView* pPrintView,
		                CRuntimeClass* pPreviewViewClass, CPrintPreviewState* pState);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreView* pView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyView)
	afx_msg void OnFilePrintPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    friend class CPreView;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEW_H__03894546_1C39_11D4_B336_00104B13D514__INCLUDED_)
