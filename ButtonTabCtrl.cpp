// ButtonTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CoolTabCtrl.h"
#include "ButtonTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define	ITEMBUTTON_HEIGHT		17
/////////////////////////////////////////////////////////////////////////////
// CBottonTabCtrl






/////////////////////////////////////////////////////////////////////////////
// CBottonTabCtrl message handlers
IMPLEMENT_DYNCREATE(CBottonTabCtrl, CWnd)

static CPen _penW(PS_SOLID,1 ,RGB(255,255,255));
static CPen _penB(PS_SOLID,1 ,RGB(0,0,0));
static CPen _penGray(PS_SOLID,1,GetSysColor(COLOR_GRAYTEXT));
static CPen _pen3DFace(PS_SOLID,1,GetSysColor(COLOR_3DFACE));
static CPen _pen3DLight(PS_SOLID,1,GetSysColor(COLOR_3DLIGHT));
static CPen _pen3DShadow(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
static CPen _pen3DDKShadow(PS_SOLID,1,GetSysColor(COLOR_3DDKSHADOW));

int DoubleRound(double d)
{
	int re=(int)d;
	double temp=d-re;
	if(temp>0.5)
	{
		return (re+1);
	}
	else
	{
		return re;
	}
}
CBottonTabCtrl::CBottonTabCtrl()
{
	m_nStyle = TCS_DOWN;
	m_nActivePage = -1;
	m_nBorder = 2;
	m_bEraseBkgnd = TRUE;
}

CBottonTabCtrl::~CBottonTabCtrl()
{
	POSITION pos;
	CPageItem* pItem;
	for(pos=m_PageList.GetHeadPosition();pos!=NULL;)
	{
		pItem=(CPageItem*)m_PageList.GetNext(pos);
		if(pItem)
		{	
			if(pItem->m_nStyle == 1 && pItem->m_pWnd) // 动态创建
			{
				pItem->m_pWnd->DestroyWindow();
				delete pItem->m_pWnd;
			}
			delete pItem;
			pItem=NULL;
		}
	}
	m_PageList.RemoveAll();
}


BEGIN_MESSAGE_MAP(CBottonTabCtrl, CWnd)
	//{{AFX_MSG_MAP(CBottonTabCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBottonTabCtrl message handlers

BOOL CBottonTabCtrl::Create(UINT wStyle, const CRect &rect, CWnd *pParentWnd, UINT nID)
{
	m_nStyle = wStyle & TCS_MASK;
	if(m_nStyle == 0)
		m_nStyle = TCS_UP;
	if (CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(LTGRAY_BRUSH), NULL),
						NULL,
						wStyle&~TCS_MASK | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						rect, 
						pParentWnd, 
						nID))
	{
		
		return TRUE;
	}
	return FALSE;
}

BOOL CBottonTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	if(m_bEraseBkgnd)
	{
		CRect rect;
		GetWindowRect(&rect);
		ScreenToClient(&rect);
		CBrush brush(GetSysColor(COLOR_3DFACE));
		pDC->FillRect(rect,&brush);
	}
	return TRUE;
}

void CBottonTabCtrl::OnPaint() 
{
	CPaintDC	dc(this);
	CPen	*pOldPen = dc.GetCurrentPen();
	CFont	*pOldFont = dc.SelectObject(&m_font);
	int		nOldBkMode = dc.SetBkMode(TRANSPARENT);
	CPageItem	*pItem;
	POSITION	pos,tempPos;
	int		nItemIndex = 0;
	
	DrawFrame(&dc);
	BOOL bIsFirstPage=TRUE;
	for(pos=m_PageList.GetHeadPosition();pos!=NULL;nItemIndex++)
	{
		UINT state=ITEM_STATE_NOMAL;
		pItem=(CPageItem*)m_PageList.GetNext(pos);
		tempPos=pos;
		if(pItem)
		{
			if(m_nActivePage==nItemIndex)
			{
				state=ITEM_STATE_ACTIVE;
				
			}
			else if(pos==NULL)  //表明是最后一项
			{
				state=ITEM_STATE_LAST_ITEM;
			}
			else
			{
				state=ITEM_STATE_NOMAL;
			}
			pItem->Draw(&dc,m_nStyle,state,bIsFirstPage);
			bIsFirstPage=FALSE;
			
		}

	}
	dc.SetBkMode(nOldBkMode);
	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
}


CBottonTabCtrl::CPageItem* CBottonTabCtrl::AddPage(CWnd *pWnd, LPCWSTR sText, UINT IconID)
{
	ASSERT(pWnd);
	ASSERT(IsWindow(pWnd->m_hWnd));

	CPageItem *pItem = NULL;
	pItem = new CPageItem();
	pItem->m_pWnd = pWnd;
	pItem->m_nStyle = 0;	//Window
	pItem->m_sText = sText;
	if(IconID)
		pItem->m_hIcon = AfxGetApp()->LoadIcon(IconID);
	else
		pItem->m_hIcon = NULL;

	CRect	rect;
	GetClientRect(rect);
	pWnd->MoveWindow(rect);

	m_PageList.AddTail(pItem);

	// 如果添加的Page index不等于m_nActivePage, 则隐藏
	if(m_nActivePage != m_PageList.GetCount())
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	return pItem;
}
void CBottonTabCtrl::GetClientRect(LPRECT lpRect)
{
	CWnd::GetClientRect(lpRect);
	if(m_nStyle&TCS_DOWN)
	{
		lpRect->left += 2;
		lpRect->right -= 2;
		lpRect->top += 2;
		lpRect->bottom -= ITEMBUTTON_HEIGHT;
	}
	else if(m_nStyle&TCS_UP)
	{
		lpRect->left += 2;
		lpRect->right -= 2;
		lpRect->top += ITEMBUTTON_HEIGHT;
		lpRect->bottom -= 2;
	}
}

///////取得需要占用的宽度/////
CPoint CBottonTabCtrl::CPageItem::m_arrPoint[6];

UINT CBottonTabCtrl::CPageItem::GetAreaWidth(CDC *pDC)
{
	UINT width = pDC->GetTextExtent(m_sText).cx+35;
	if(m_hIcon)
		width += 18;
	return (UINT)(width + (double)ITEMBUTTON_HEIGHT*2/1.732);
}

void CBottonTabCtrl::CPageItem::Draw(CDC *pDC, UINT nStyle, UINT state,BOOL bIsFirstLabel)
{
	CRect rect = m_rect;
	CPen * pOldPen;
	CBrush * pOldBrush;
	double dTemp=(double)m_rect.Height()/1.732;
	if(nStyle&TCS_DOWN)
	{
// 		if(bActive)
// 		{
// 			rect.top -= 2;
// 			CBrush brush(GetSysColor(COLOR_3DFACE));
// 			pDC->FillRect(rect,&brush);
// 			rect.top += 1;
// 		}
// 		else
// 			rect.bottom -= 1;
		pOldPen=pDC->SelectObject(&_penB);
		if(bIsFirstLabel)
		{
			m_arrPoint[4].x=rect.left;
			m_arrPoint[4].y=rect.top;
			m_arrPoint[3].x=DoubleRound((double)m_arrPoint[4].x+dTemp*2/3);
			m_arrPoint[3].y=DoubleRound((double)m_arrPoint[4].y+(double)rect.Height()*2/3);
			rect.left+=DoubleRound(dTemp/3);
		}
		m_arrPoint[0]=m_arrPoint[3];
		m_arrPoint[5]=m_arrPoint[4];
		m_arrPoint[1].x=DoubleRound((double)rect.left+dTemp*2/3);
		m_arrPoint[1].y=rect.bottom;
		m_arrPoint[2].x = (long)(rect.right-dTemp);
		m_arrPoint[2].y=rect.bottom;
		m_arrPoint[3].x=DoubleRound(m_arrPoint[2].x+dTemp/3);
		m_arrPoint[3].y=DoubleRound(m_arrPoint[2].y-dTemp/3);
		switch(state)
		{
		case ITEM_STATE_ACTIVE:
			{
				//此页最激活时
				m_arrPoint[4].x=rect.right;
				m_arrPoint[4].y=rect.top;
				CBrush tempBrush(RGB(255,255,255));
				pOldBrush=pDC->SelectObject(&tempBrush);
				pDC->Polygon(m_arrPoint,6);
				pOldPen=pDC->SelectObject(&_penW);
				pDC->MoveTo(m_arrPoint[4]);
				pDC->LineTo(m_arrPoint[5]);
				pDC->SelectObject(pOldPen);
				break;
			}
		case ITEM_STATE_NOMAL:
			{
				//triangle指两个页标签所夹的三角形
				m_arrPoint[4].x=DoubleRound((double)m_arrPoint[2].x-dTemp/3);
				m_arrPoint[4].y=rect.top;
				CBrush tempBrush(GetSysColor(COLOR_3DFACE));
				pOldBrush=pDC->SelectObject(&tempBrush);
				pDC->Polygon(m_arrPoint,6);
				break;
			}
		case ITEM_STATE_LAST_ITEM:
			{
				//最后一页
				m_arrPoint[4].x=rect.right;
				m_arrPoint[4].y=rect.top;
				CBrush tempBrush(GetSysColor(COLOR_3DFACE));
				pOldBrush=pDC->SelectObject(&tempBrush);
				pDC->Polygon(m_arrPoint,6);
				break;
			}
		}
		pDC->SelectObject(&_penGray);
		pDC->MoveTo(m_arrPoint[1]);
		pDC->LineTo(m_arrPoint[2]);
	}//end of TCS_DOWN
// 	else if(nStyle&TCS_UP)
// 	{
// 		CBrush brush(GetSysColor(COLOR_3DFACE));
// 		if(bActive)
// 		{
// 			rect.bottom += 1;
// 			pDC->FillRect(rect,&brush);
// 			rect.bottom -= 1;
// 		}
// 		else
// 		{
// 			rect.top += 2;
// 			pDC->FillRect(rect,&brush);
// 		}
// 		pDC->SelectObject(&_penW);
// 		pDC->MoveTo(rect.left,rect.bottom);
// 		pDC->LineTo(rect.left,rect.top + 2);
// 		pDC->LineTo(rect.left + 2,rect.top);
// 		pDC->LineTo(rect.right,rect.top);
// 
// 		pDC->SelectObject(&_pen3DShadow);
// 		pDC->MoveTo(rect.right - 1,rect.top);
// 		pDC->LineTo(rect.right - 1,rect.bottom);
// 			
// 		pDC->SelectObject(&_penB);
// 		pDC->MoveTo(rect.right,rect.top + 1);
// 		pDC->LineTo(rect.right,rect.bottom);	
// 	}
	///////////调整位置//////////
		rect.left += (long)(dTemp+5);
		rect.right -= (long)(dTemp+5);
		///////////显示图标//////////
// 		if(rect.Width() > 16 && m_hIcon != NULL)
// 		{
// 			::DrawIconEx(pDC->m_hDC,rect.left,rect.top + 3,m_hIcon,16,16,0,NULL,DI_NORMAL);
// 			rect.left += 18;
// 		}
		if (!m_sText.IsEmpty())
		{
			///////////显示文字//////////
			rect.top += 5;
			CString sText = m_sText;
			int l = sText.GetLength();
			int i;
			for(i=0;i<10 && pDC->GetTextExtent(sText).cx > rect.Width();i++,l-=2)
				sText = sText.Left(l-2);
			if(i > 0)
			{
				sText = sText.Left(l-2);
				sText += _T("...");
			}
			CFont *pOldFont,tempFont;
			LOGFONT longFont;
			pOldFont=pDC->GetCurrentFont();
			pOldFont->GetLogFont(&longFont);
			longFont.lfWeight|=FW_BOLD;
			tempFont.CreateFontIndirect(&longFont);
			if(state==ITEM_STATE_ACTIVE)
			{
				pDC->SelectObject(&tempFont);
				pDC->DrawText(sText, &rect, DT_LEFT /*| DT_VCENTER */| DT_SINGLELINE);
				pDC->SelectObject(pOldFont);
			}
			else
			{
				pDC->DrawText(sText, &rect, DT_LEFT /*| DT_VCENTER */| DT_SINGLELINE);
			}
			
		}
}

////调整尺寸////
void CBottonTabCtrl::AutoSize()
{
	UINT PageCount = m_PageList.GetCount();
	if(PageCount < 1) return;
	CPageItem	*pItem;
	POSITION	pos;
	CRect		rect,ClientRect,ItemRect;

	GetClientRect(ClientRect);
	ClientRect.DeflateRect(m_nBorder+1,m_nBorder+1);
	GetWindowRect(rect);
	ScreenToClient(rect);

	CDC* pDC = GetDC();
	CFont	*pOldFont = pDC->SelectObject(&m_font);

	if(m_nStyle&TCS_DOWN)
	{
		rect.left += 1;
		rect.right -= 6;
		rect.bottom -= 1;
		rect.top = rect.bottom - ITEMBUTTON_HEIGHT + 1;
	}
// 	else if(m_nStyle&TCS_UP)
// 	{
// 		rect.left += 2;
// 		rect.right -= 6;
// 		rect.bottom = rect.top + ITEMBUTTON_HEIGHT;
// 	}
	m_rectLabel=rect;
	ClientRect.bottom=m_rectLabel.top;
	ItemRect = rect;
	int AreaWidth = 0,ItemMaxWidth,ItemIndex=0;
	ItemMaxWidth = rect.Width()/m_PageList.GetCount();
	////////设置按钮初始宽度，并得到按钮所需占用的总宽度///////
	for(pos=m_PageList.GetHeadPosition();pos!=NULL;ItemIndex++)
	{
		pItem=(CPageItem*)m_PageList.GetNext(pos);
		if(pItem)
		{
			ItemMaxWidth=pItem->GetAreaWidth(pDC);
			AreaWidth += ItemMaxWidth;
			ItemRect.right = ItemRect.left+ItemMaxWidth;
			pItem->m_rect = ItemRect;
			ItemRect.left = DoubleRound((double)ItemRect.right-ITEMBUTTON_HEIGHT/1.732);
			if(pItem->m_pWnd)
				pItem->m_pWnd->MoveWindow(ClientRect);
		}
	}
	////////当需要的空间大于实际空间时进行调整////
// 	if(AreaWidth > rect.Width() && !bMonoSpace)
// 	{
// 		ItemRect = rect;
// 		int AreaWidth,MaxWidth = rect.Width()/PageCount;
// 		for(pos=m_PageList.GetHeadPosition();pos!=NULL;)
// 		{
// 			pItem=(CPageItem*)m_PageList.GetNext(pos);
// 			if(pItem)
// 			{
// 				AreaWidth = pItem->GetAreaWidth(pDC);
// 				ItemMaxWidth = (ItemMaxWidth < AreaWidth)?MaxWidth:AreaWidth;
// 				ItemRect.right = ItemRect.left+ItemMaxWidth;
// 				pItem->m_rect = ItemRect;
// 				ItemRect.left = ItemRect.right + 1;
// 			}
// 		}
// 	}
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
}

void CBottonTabCtrl::SetActivePage(int nIndex)
{
	if(nIndex == m_nActivePage)
		return;
	CPageItem *pOldItem,*pItem;
	if(m_nActivePage >= 0)
	{
		pOldItem = (CPageItem*)GetPageItem(m_nActivePage);
		if(pOldItem)
		{
			pOldItem->m_pWnd->ShowWindow(SW_HIDE);
		}
	}
	pItem = (CPageItem*)GetPageItem(nIndex);
	if(!pItem) return;
	m_nActivePage = nIndex;
	if(m_nStyle&TCS_ANIMATE)
		SetTimer(100,15,NULL);///为了在95或NT4下也有动画效果，没有使用AnimateWindow函数
	else
		pItem->m_pWnd->ShowWindow(SW_SHOW);
}

void* CBottonTabCtrl::GetPageItem(UINT nIndex)
{
	CPageItem *pItem = NULL;
	POSITION pos = m_PageList.FindIndex(nIndex);
	if(pos)
		pItem = (CPageItem*)m_PageList.GetAt(pos);
	return pItem;
}

void CBottonTabCtrl::UpdateWindow()
{
 	AutoSize();
 	if(m_nActivePage < 0)
 		SetActivePage(0);
 	else
 		SetActivePage(m_nActivePage);
 	Invalidate();
}

void CBottonTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT nItemIndex=0;
	POSITION pos;
	CPageItem *pItem;
	for(pos=m_PageList.GetHeadPosition();pos!=NULL;nItemIndex++)
	{
		pItem=(CPageItem*)m_PageList.GetNext(pos);
		if(pItem)
		{
			if(pItem->m_rect.PtInRect(point))
			{
				SetActivePage(nItemIndex);
				Invalidate();
				break;
			}
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CBottonTabCtrl::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CWnd::OnSizing(fwSide, pRect);
	m_bEraseBkgnd = FALSE;
	AutoSize();
}

void CBottonTabCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	m_bEraseBkgnd = TRUE;
	AutoSize();
	Invalidate();
}

void CBottonTabCtrl::DrawFrame(CDC *pDC)
{
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);
	switch(m_nBorder)
	{
	case 1:
			pDC->Draw3dRect(rect,RGB(0,0,0),RGB(0xff,0xff,0xff));
		break;
	case 2:
			pDC->Draw3dRect(rect, GetSysColor(COLOR_3DFACE), GetSysColor(COLOR_3DSHADOW));
			rect.DeflateRect(2, 2);
			pDC->Draw3dRect(rect,RGB(0,0,0),RGB(0xff,0xff,0xff));
		break;
	default:break;
	}
	CBrush brush(GetSysColor(COLOR_3DFACE));
	pDC->FillRect(m_rectLabel,&brush);

}



void CBottonTabCtrl::SetStyle(UINT style)
{
	m_nStyle = style;
}

UINT CBottonTabCtrl::GetStyle()
{
	return m_nStyle;
}

void CBottonTabCtrl::OnTimer(UINT nIDEvent) 
{
	static	step,times = 10;
	UINT	width;
	CRect	rect;
	GetClientRect(&rect);
	rect.DeflateRect(m_nBorder+1,m_nBorder+1);
	POSITION pos = m_PageList.FindIndex(m_nActivePage);
	if(pos)
	{
		CPageItem *pItem = (CPageItem*)m_PageList.GetAt(pos);
		if(pItem)
		{
			width = rect.Width();
			if(times == 10)
			{
				rect.right = 10;
				rect.left = rect.right - width + 1;
				step = width/times;
				times--;
				m_bEraseBkgnd = FALSE;
				pItem->m_pWnd->ShowWindow(SW_SHOW);
			}
			else
			{
				if(times <= 0)
				{
					times = 10;
					KillTimer(nIDEvent);
					m_bEraseBkgnd = TRUE;
				}
				else
				{
					rect.right = width - times*step;
					rect.left = rect.right - width + 1;
					times--;
				}
			}
			pItem->m_pWnd->MoveWindow(rect);
		}
	}
}

void CBottonTabCtrl::SetStyleAnimate(BOOL bEnable)
{
	UINT style = GetStyle();
	if(bEnable)
		style |= TCS_ANIMATE;
	else
		style &= ~TCS_ANIMATE;
	SetStyle(style);
}

void CBottonTabCtrl::SetStyleDirection(int Direction)
{
	UINT style = GetStyle();
	switch(Direction)
	{
	case TCS_DOWN:
		style &= ~TCS_UP;
		style |= TCS_DOWN;
		break;
	case TCS_UP:
	default:
		style &= ~TCS_DOWN;
		style |= TCS_UP;
		break;
	}
	SetStyle(style);
	UpdateWindow();
}

void CBottonTabCtrl::SetStyleMonoSpace(BOOL bEnable)
{
	UINT style = GetStyle();
// 	if(bEnable)
// 		style |= TCS_MONOSPACE;
// 	else
// 		style &= ~TCS_MONOSPACE;
	SetStyle(style);
	UpdateWindow();
}

BOOL CBottonTabCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// 将WM_NOTIFY消息转交给父窗口处理
	GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
	*pResult = 0;
	
	return TRUE;
}

int CBottonTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	LOGFONT logFont;
	ZeroMemory((void*)&logFont,sizeof(logFont));
	wcscpy(logFont.lfFaceName,_T("宋体"));
	logFont.lfHeight = -12;
	logFont.lfWeight = 400;
	logFont.lfCharSet = GB2312_CHARSET;
	logFont.lfOutPrecision = 3;
	logFont.lfClipPrecision = 2; 
	logFont.lfQuality = 1;
	logFont.lfPitchAndFamily = 2;
	m_font.CreateFontIndirect(&logFont);
	SetFont(&m_font);
	m_brush.CreateSolidBrush(RGB(255,255,255)); 
	return 0;
}

void CBottonTabCtrl::OnDestroy()
{
	CWnd::OnDestroy();
}



HBRUSH CBottonTabCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here 
    wchar_t   cTemp[20];   
	GetClassName(pWnd->GetSafeHwnd(),cTemp,sizeof(cTemp));   
	if   (CString(cTemp)==_T("Edit"))   
	{   

		static   HBRUSH   hbrEdit   =   ::CreateSolidBrush(RGB(255,   255,   255));   
		pDC->SetBkColor(RGB(255,   255,   255));   
		pDC->SetTextColor(RGB(0,   0,   0));   
        return   hbrEdit;
	}   
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
