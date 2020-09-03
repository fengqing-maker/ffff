// XListCtrl.h  Version 1.3
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XLISTCTRL_H
#define XLISTCTRL_H

#include "XHeaderCtrl.h"
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
#include "XComboList.h"
#include "InPlaceEdit.h"
#endif
#define IDC_LIST_EDIT           0X01
#define IDM_COMBOX_MENU         0X02
#define IDM_COMBOX_MENU_LAST    0X10    //最多允许有13项菜单

#define IDC_LIST_BUTTON         0X1000   
extern UINT NEAR WM_XLISTCTRL_COMBO_SELECTION;
extern UINT NEAR WM_XLISTCTRL_CHECKBOX_CLICKED;
extern UINT NEAR WM_EDIT_CHANGE;
extern UINT NEAR WM_MENU_COMBO_SELECTION;
extern UINT NEAR WM_XLISTCTRL_BUTTON_CLICKED;
///////////////////////////////////////////////////////////////////////////////
// Style bits for use with SetExtendedStyleX()

// none defined yet

///////////////////////////////////////////////////////////////////////////////
// CXListCtrl data

struct XLISTCTRLDATA
{
	// ctor
	XLISTCTRLDATA()
	{
/*		pButton = NULL;*/
		Release();
	}
	~XLISTCTRLDATA()
	{
		Release();
	}
	void Release()
	{	
// 		if(pButton!=NULL)
// 		{
// 			pButton->DestroyWindow();
// 			delete pButton;
// 		}
		bEditAndCombox       = FALSE;
		nWidthOfCombox       = 0;
		bEdit                = FALSE;
		bEditActive          = FALSE;
		bEnabled             = TRUE;
		crText               = ::GetSysColor(COLOR_WINDOWTEXT);
		crBackground         = ::GetSysColor(COLOR_WINDOW);
		bShowProgress        = FALSE;
		nProgressPercent     = 0;
		strProgressMessage   = _T("");
		bShowProgressMessage = TRUE;
		nCheckedState        = -1;
		bBold                = FALSE;
		nImage               = -1;
#ifndef NO_XLISTCTRL_TOOL_TIPS
		strToolTip           = _T("");
#endif
		bCombo               = FALSE;
		bComboIsClicked      = FALSE;
		nComboListHeight     = 10;
		nInitialComboSel     = 0;
		psa.RemoveAll();
		dwItemData           = 0;
		bButton              = FALSE;
		bButtonActive        = FALSE;
	}
	BOOL            bEditAndCombox;         // TRUE:表示表项前面下拉菜单，又有文本框
	int             nWidthOfCombox;          // 下拉菜单的宽度第一项的宽度
	BOOL			bEnabled;				// TRUE = enabled, FALSE = disabled (gray text)
	BOOL			bBold;					// TRUE = display bold text
	int				nImage;					// index in image list, else -1
#ifndef NO_XLISTCTRL_TOOL_TIPS
	CString			strToolTip;				// tool tip text for cell
#endif

	// for combo
	BOOL			bCombo;					// TRUE = display combobox
	BOOL			bComboIsClicked;		// TRUE = downarrow is clicked
	CStringArray 	psa;					// pointer to string array for combo listbox
	int				nComboListHeight;		// combo listbox height (in rows)
	int				nInitialComboSel;		// initial combo listbox selection (0 = first)
    
	// for Edit
	BOOL            bEdit;                  //TRUE＝Display Edit
	BOOL            bEditActive;
	// for color
	COLORREF	crText;
	COLORREF	crBackground;

	// for progress
	BOOL		bShowProgress;				// true = show progress control
	int			nProgressPercent;			// 0 - 100
	CString		strProgressMessage;			// custom message for progress indicator -
											// MUST INCLUDE %d
	BOOL		bShowProgressMessage;		// TRUE = display % message, or custom message
											// if one is supplied
	// for checkbox
	int			nCheckedState;				// -1 = don't show, 0 = unchecked, 1 = checked

	// for Button
	//CListButton *    pButton;                    //NULL表示本表项不是按钮
	BOOL	    bButton;
	BOOL        bButtonActive;
	CString     strButtonText;
	DWORD		dwItemData;					// pointer to app's data

};


///////////////////////////////////////////////////////////////////////////////
// CXListCtrl class

class CXListCtrl : public CListCtrl
{
// Construction
	DECLARE_DYNAMIC(CXListCtrl)
public:
	CXListCtrl();
	virtual ~CXListCtrl();

// Attributes
public:

// Operations
public:
	DWORD   m_dwEditCtrlStyle;
	int		CountCheckedItems(int nSubItem);
	BOOL	DeleteAllItems();
	BOOL	DeleteItem(int nItem);
	void	DeleteProgress(int nItem, int nSubItem);
	BOOL	GetBold(int nItem, int nSubItem);
	int		GetCheckbox(int nItem, int nSubItem);
	int		GetColumns();
	CString	GetComboText(int iItem, int iSubItem);
	int		GetCurSel();
	BOOL	GetEnabled(int nItem);
	DWORD	GetExtendedStyleX() { return m_dwExtendedStyleX; }
	int		GetHeaderCheckedState(int nSubItem);
	DWORD	GetItemData(int nItem);
	BOOL	GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& rect);
	int		InsertItem(int nItem, LPCTSTR lpszItem);
	int		InsertItem(int nItem, 
					   LPCTSTR lpszItem, 
					   COLORREF crText, 
					   COLORREF crBackground);
	int		InsertItem(const LVITEM* pItem);
	BOOL	SetBold(int nItem, int nSubItem, BOOL bBold);
	BOOL	SetComboBox(int nItem, 
						int nSubItem, 
						BOOL bEnableCombo, 
						CStringArray *psa,
						int nComboListHeight,
						int nInitialComboSel);
	BOOL	SetCheckbox(int nItem, int nSubItem, int nCheckedState);
	BOOL	SetCurSel(int nItem);
	BOOL	SetEnabled(int nItem, BOOL bEnable);
	DWORD	SetExtendedStyleX(DWORD dwNewStyle) 
	{
		DWORD dwOldStyle = m_dwExtendedStyleX;
		m_dwExtendedStyleX = dwNewStyle;
		return dwOldStyle;
	}

	BOOL	SetHeaderCheckedState(int nSubItem, int nCheckedState);
	int		SetItem(const LVITEM* pItem);
	BOOL	SetItemData(int nItem, DWORD dwData);
	BOOL	SetItemImage(int nItem, int nSubItem, int nImage);
	BOOL	SetItemText(int nItem, int nSubItem, LPCTSTR lpszText); 
	BOOL	SetItemText(int nItem, 
						int nSubItem, 
						LPCTSTR lpszText,
						COLORREF crText, 
						COLORREF crBackground);
	BOOL	SetProgress(int nItem, 
						int nSubItem, 
						BOOL bShowProgressText = TRUE, 
						LPCTSTR lpszProgressText = NULL);
	BOOL	SetEdit(int nItem,int nSubItem,LPCTSTR lpszText,COLORREF crText, 
						COLORREF crBackground,BOOL bEdit = TRUE);
	void	UpdateProgress(int nItem, int nSubItem, int nPercent);
	void	UpdateSubItem(int nItem, int nSubItem);

#ifndef NO_XLISTCTRL_TOOL_TIPS
	void DeleteAllToolTips();
	BOOL SetItemToolTipText(int nItem, int nSubItem, LPCTSTR lpszToolTipText);
	CString GetItemToolTipText(int nItem, int nSubItem);
	virtual int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListCtrl)
	public:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetButtom(int nItem,int nSubItem,LPCTSTR lpszText);
	void DeleteSubItem(int nItem,int nSubItem);
	BOOL SetEditAndComboxInOneCell(int nItem,int nSubItem,CStringArray *psa,int nInitialComboSel,
		CString strEdit,int nWidthOfFirst);
	//bReadOnly = FALSE表示list可写，也就是Edit、Combox、CheckBox都可用
	//bReadOnly = TRUE 表示list只读，也就是Edit、Combox、CheckBox都不可用
	void SetListReadOnly(BOOL bReadOnly = TRUE);
	CString m_strNoItemText;
	int m_nEditSubItem;
	CXHeaderCtrl	m_HeaderCtrl;
	CImageList		m_cImageList;	// Image list for the header control

protected:
	CInPlaceEdit* CXListCtrl::ShowInPlaceEdit(int iRowIndex,int iColumnIndex,const CRect & rcCell, CString& rstrCurSelection);
	void ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& robCellRect);
	void CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect);
	void DrawCheckbox(int nItem, 
					  int nSubItem, 
					  CDC *pDC, 
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rect, 
					  XLISTCTRLDATA *pCLD);
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	void DrawComboBox(int nItem, 
					  int nSubItem, 
					  CDC *pDC, 
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rect, 
					  XLISTCTRLDATA *pCLD);
	void UnpressComboButton();
#endif
	BOOL DrawEdit(int nItem, 
		int nSubItem, 
		CDC *pDC, 
		COLORREF crText,
		COLORREF crBkgnd,
		CRect& rect, 
		XLISTCTRLDATA *pCLD);
	
	int DrawImage(int nItem, 
				  int nSubItem, 
				  CDC* pDC, 
				  COLORREF crText,
				  COLORREF crBkgnd,
				  CRect rect,
				  XLISTCTRLDATA *pXLCD);
	void DrawProgress(int nItem, 
					  int nSubItem, 
					  CDC *pDC, 
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rect, 
					  XLISTCTRLDATA *pCLD);
	void DrawText(int nItem, 
				  int nSubItem, 
				  CDC *pDC, 
				  COLORREF crText,
				  COLORREF crBkgnd,
				  CRect& rect, 
				  XLISTCTRLDATA *pCLD);
	void GetDrawColors(int nItem,
					   int nSubItem,
					   COLORREF& colorText,
					   COLORREF& colorBkgnd);
	void SubclassHeaderControl();

	BOOL			m_bHeaderIsSubclassed;
	DWORD			m_dwExtendedStyleX;

	COLORREF		m_cr3DFace;
	COLORREF		m_cr3DHighLight;
	COLORREF		m_cr3DShadow;
	COLORREF		m_crBtnFace;
	COLORREF		m_crBtnShadow;
	COLORREF		m_crBtnText;
	COLORREF		m_crGrayText;
	COLORREF		m_crHighLight;
	COLORREF		m_crHighLightText;
	COLORREF		m_crWindow;
	COLORREF		m_crWindowText;

#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	BOOL			m_bComboIsClicked;
	int				m_nComboItem;
	int				m_nComboSubItem;
	CRect			m_rectComboButton;
	CRect			m_rectComboList;
	CXComboList *	m_pListBox;
	CInPlaceEdit *  m_pEdit;
	CFont			m_ListboxFont;
	BOOL			m_bFontIsCreated;
	CString			m_strInitialComboString;
#endif


	// Generated message map functions
protected:
	void DrawButton(int nItem,int nSubItem,CDC *pDC,CRect & rect,XLISTCTRLDATA * pXLCD);
	int m_nClickSubItem;
	int m_nClickItem;
	//{{AFX_MSG(CXListCtrl)
	afx_msg BOOL OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnComboxMenu(UINT nID);
	//}}AFX_MSG
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnComboEscape(WPARAM, LPARAM);
	afx_msg LRESULT OnComboReturn(WPARAM, LPARAM);
	afx_msg LRESULT OnComboKeydown(WPARAM, LPARAM);
	afx_msg LRESULT OnComboLButtonUp(WPARAM, LPARAM);
#endif

#ifndef NO_XLISTCTRL_TOOL_TIPS
	virtual afx_msg BOOL OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
#endif

	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bReadOnly;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XLISTCTRL_H
