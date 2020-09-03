// StatusBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "StatusBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx

CStatusBarEx::CStatusBarEx()
{
	m_pColor=NULL;
}

CStatusBarEx::~CStatusBarEx()
{
}


BEGIN_MESSAGE_MAP(CStatusBarEx, CStatusBar)
	//{{AFX_MSG_MAP(CStatusBarEx)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx message handlers

void CStatusBarEx::OnPaint() 
{
	//如果用户没有设置颜色，就调用CStatusBar::OnPaint();
	if(m_pColor==NULL)
	{
		CStatusBar::OnPaint();
		return;
	}
		
	PAINTSTRUCT ps;
	CDC* pDC = BeginPaint(&ps);	 
	
	
	// TODO: Add your message handler code here
	//自定义绘制CStatusBar的每一个文本框
	CRect   rect;
	COLORREF	hell	= GetSysColor(COLOR_3DHILIGHT);
	COLORREF	dunkel	= GetSysColor(COLOR_3DSHADOW);
	int oldMode=pDC->SetBkMode(TRANSPARENT);
	CFont font,*pOldFont;
	font.CreatePointFont(100,_T("宋体"));
	pOldFont=pDC->SelectObject(&font);
	int i;
	for(i=0;i<m_nCount;i++)
	{
		GetItemRect(i,&rect);
		pDC->FillSolidRect(rect,::GetSysColor(COLOR_BTNFACE));
		pDC->Draw3dRect(rect, dunkel, hell);
		rect=CRect(rect.left,rect.top,rect.right,rect.bottom);
		COLORREF oldColor=pDC->SetTextColor(m_pColor[i]);
		rect.top +=2;
		pDC->DrawText(GetPaneText(i),&rect,DT_LEFT);
		pDC->SetTextColor(oldColor);
	}
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldFont);
	
	EndPaint(&ps);
	// Do not call CStatusBar::OnPaint() for painting messages
}


void CStatusBarEx::SetTextColor(int index,COLORREF color)
{
	int i;
	if(m_pColor==NULL)
	{
		m_pColor=new COLORREF[m_nCount];
		for(i=0;i<m_nCount;i++)
		{
			m_pColor[i]=RGB(0,0,0);
		}
	}
	m_pColor[index]=color;
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
}
