#if !defined(AFX_BUTTONTABCTRL_H__E8CB08CB_0F35_4D95_AF68_1E12FFAA124A__INCLUDED_)
#define AFX_BUTTONTABCTRL_H__E8CB08CB_0F35_4D95_AF68_1E12FFAA124A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBottonTabCtrl window
#define ITEM_STATE_ACTIVE         0x0001     //该项为激发态
#define ITEM_STATE_LAST_ITEM      0x0002     //该项为最后一项
#define ITEM_STATE_NOMAL          0x0003     //该项是除前三项的一个普通相项

class CBottonTabCtrl : public CWnd
{
	DECLARE_DYNCREATE(CBottonTabCtrl)
public:
	class CPageItem
	{
		friend class CBottonTabCtrl;
		
		CWnd*		m_pWnd;		///窗口,可以为NULL
		UINT		m_nStyle;	///窗口类型，0:普通，1:动态创建
		CString		m_sText;	///文字
		HICON		m_hIcon;	///图标
		
	public:
		CRect		       m_rect;	
		static CPoint      m_arrPoint[6];
	public:
		void SetPageStyle(UINT nStyle)
		{
			m_nStyle = nStyle;
		}
		
		UINT GetAreaWidth(CDC *pDC);
		void Draw(CDC *pDC,UINT nStyle,UINT state,BOOL bIsFirstLabel);
	};
	
		// Construction
	public:
		CBottonTabCtrl();
		
		// Attributes
	public:
		// Operations
	public:
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CBottonTabCtrl)
	protected:
		virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		//}}AFX_VIRTUAL
		
		// Implementation
	public:
		CBrush m_brush;
		void SetStyleMonoSpace(BOOL bEnable = TRUE);
		void SetStyleDirection(int Direction);		///Direction取值:TCS_UP,TCS_DOWN
		void SetStyleAnimate(BOOL bEnable = TRUE);	///允许动画bEnable = TRUE,否则为FALSE
		void UpdateWindow();
		BOOL m_bEraseBkgnd;
		UINT GetStyle();
		void SetStyle(UINT style);
		void DrawFrame(CDC *pDC);
		void* GetPageItem(UINT nIndex);
		void SetActivePage(int nIndex);
		CPageItem* AddPage(CWnd *pWnd,LPCWSTR sText,UINT IconID = NULL);
		void AutoSize();
		void GetClientRect(LPRECT lpRect );
		BOOL Create(UINT wStyle, const CRect & rect, CWnd * pParentWnd, UINT nID);
		virtual ~CBottonTabCtrl();
		
		// Generated message map functions
	protected:
		CRect m_rectLabel;
		//{{AFX_MSG(CBottonTabCtrl)
		afx_msg void OnPaint();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnDestroy();	
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	private:
		CFont		m_font;
		int			m_nActivePage;
		UINT		m_nStyle;
		UINT		m_nBorder;
	CPtrList	    m_PageList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONTABCTRL_H__E8CB08CB_0F35_4D95_AF68_1E12FFAA124A__INCLUDED_)
