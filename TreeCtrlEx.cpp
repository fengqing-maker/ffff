// TreeCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "TreeCtrlEx.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx

CTreeCtrlEx::CTreeCtrlEx()
{
	m_pWareFileData = NULL;
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx message handlers

void CTreeCtrlEx::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CTreeCtrl::PreSubclassWindow();
	
	EnableToolTips(TRUE);
}

//显示提示信息
int CTreeCtrlEx::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	RECT rect;
	UINT nFlags;
	HTREEITEM hitem = HitTest( point, &nFlags );
	CString strItemText;
//	char str[80];	
	if(hitem != NULL)
	{
		strItemText = GetItemText(hitem);
		if(m_mapTreeItem.find(strItemText) == m_mapTreeItem.end())
		{
			return -1;
		}
		CString info;
		int subItemIndex,index,nPointInCircle;
		subItemIndex = (*m_mapTreeItem.find(strItemText)).second>>16;
		index = (*m_mapTreeItem.find(strItemText)).second & 0x0000FFFF;
		nPointInCircle = m_pWareFileData->m_arrPSubItem[subItemIndex]->GetPointCountInCircle(index);
		if(nPointInCircle == -1)
		{
			info.LoadString( IDS_ANALYSZFILE_ERROR );
			CdMessageBox box( (CWnd *const) this, info);
	 	    box.MsgBoxEx();
			return -1;
		}
		GetItemRect(hitem,&rect,TRUE);
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT)hitem;
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = rect;
		return pTI->uId;
	}
	return -1;
}

BOOL CTreeCtrlEx::SetWareFileData(CWareFileData *pWareFileData)
{
	if(pWareFileData == NULL)
	{
		return FALSE;
	}
	else
	{
		m_pWareFileData = pWareFileData;
		return TRUE;
	}
}
BOOL CTreeCtrlEx::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// 需要处理ANSI和UNICODE两种格式
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText,strItemText;
	UINT nID = pNMHDR->idFrom;
	
	// 不必处理树自己发出的ToolTip消息
	if( nID == (UINT)m_hWnd &&
		(( pNMHDR->code == TTN_NEEDTEXTA && 
		pTTTA->uFlags & TTF_IDISHWND ) ||
		( pNMHDR->code == TTN_NEEDTEXTW && 
		pTTTW->uFlags & TTF_IDISHWND ) ) )
		return FALSE;
	
	// 得到鼠标位置
	const MSG* pMessage;
	CPoint pt;
	pMessage = GetCurrentMessage();
	ASSERT ( pMessage );
	pt = pMessage->pt;
	ScreenToClient( &pt );
	
	UINT nFlags;
	HTREEITEM hitem = HitTest( pt, &nFlags );
	if( nFlags & TVHT_ONITEM)
	{
		strItemText = GetItemText(hitem);
		if(m_mapTreeItem.find(strItemText) == m_mapTreeItem.end())
		{
			return -1;
		}
		CString info;
		int subItemIndex,index,nPointInCircle;
		subItemIndex = (*m_mapTreeItem.find(strItemText)).second>>16;
		index = (*m_mapTreeItem.find(strItemText)).second & 0x0000FFFF;
		nPointInCircle = m_pWareFileData->m_arrPSubItem[subItemIndex]->GetPointCountInCircle(index);
		if(nPointInCircle == -1)
		{
			info.LoadString( IDS_ANALYSZFILE_ERROR );
			CdMessageBox box(this, info);
	 	    box.MsgBoxEx();
			return -1;
		}
		strTipText.Format(_T("此通道每周波采样：%d点"),nPointInCircle);
	}
#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, 80);
	else
		_mbstowcsz(pTTTW->szText, strTipText, 80);
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, 80);
	else
		lstrcpyn(pTTTW->szText, strTipText, 80);
#endif
	*pResult = 0;
	
	return TRUE; // 消息处理完毕
}
