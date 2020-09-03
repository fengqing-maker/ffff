// ToolBarEx.cpp : 实现文件
//

#include "stdafx.h"
#include "ToolBarEx.h"
#include "MainFrm.h"
#include "SID_2FY.h"
// CToolBarEx

IMPLEMENT_DYNAMIC(CToolBarEx, CToolBar)
CToolBarEx::CToolBarEx()
{
	m_bDropDownStyle=FALSE;
	m_dwToolBarFlag = 0x0F;
}

CToolBarEx::~CToolBarEx()
{
	m_DDBtnArray.RemoveAll();
}


BEGIN_MESSAGE_MAP(CToolBarEx, CToolBar)
	//消息反射
	ON_NOTIFY_REFLECT(TBN_DROPDOWN,OnDropDownBtn)
END_MESSAGE_MAP()



// CToolBarEx 消息处理程序
void CToolBarEx::OnDropDownBtn( NMHDR * pNotifyStruct, LRESULT * result )
{
	LPNMTOOLBAR pnmtb=(LPNMTOOLBAR)pNotifyStruct;
	UINT nIDMenu=NULL;
	//
	for(int i=0;i<m_DDBtnArray.GetSize();i++)
		if(((DROPDOWNLIST)m_DDBtnArray[i]).nIDButton==pnmtb->iItem)
			nIDMenu=((DROPDOWNLIST)m_DDBtnArray[i]).nIDMenu;
	if(!nIDMenu)
		return;
	CMenu menu;
	menu.LoadMenu(nIDMenu);
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup);
	
	CRect rc;
	SendMessage(TB_GETRECT, pnmtb->iItem, (LPARAM)&rc);
	ClientToScreen(&rc);
	
	pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
		rc.left, rc.bottom, this, &rc);
}

void CToolBarEx::AddDropDownBtn(UINT nIDBUTTON, UINT  nIDMENU)
{
	if(!m_bDropDownStyle)
	{
		DWORD dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
		GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle);
		m_bDropDownStyle=TRUE;
	}
	DROPDOWNLIST DropdownBtn={nIDBUTTON,nIDMENU};
	DWORD dwStyle =GetButtonStyle(CommandToIndex(nIDBUTTON));
	dwStyle |= TBSTYLE_DROPDOWN;
	SetButtonStyle(CommandToIndex(nIDBUTTON), dwStyle);
	m_DDBtnArray.Add(DropdownBtn);
}
void CToolBarEx::RemoveDropDownBtn(UINT nIDBUTTON)
{
	for(int i=0;i<m_DDBtnArray.GetSize();i++)
		if(((DROPDOWNLIST)m_DDBtnArray[i]).nIDButton==nIDBUTTON)
		{
				DWORD dwStyle =GetButtonStyle(CommandToIndex(nIDBUTTON));
				dwStyle &=~TBSTYLE_DROPDOWN;
				SetButtonStyle(CommandToIndex(nIDBUTTON), dwStyle);
				m_DDBtnArray.RemoveAt(i);
		}
	if(m_DDBtnArray.GetSize()==0)
	{
		DWORD dwExStyle =NULL;
		GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle);
		m_bDropDownStyle=FALSE;
	}
}


BOOL CToolBarEx::SetImageList(UINT nIDImageList,COLORREF clrMask, UINT nType)
{
	CImageList imgList;
	CBitmap bm;
	BITMAP bminfo;
	HBITMAP   hbm   =   (HBITMAP)::LoadImage(AfxGetInstanceHandle(),   
		MAKEINTRESOURCE(nIDImageList),   
		IMAGE_BITMAP,   
		0,0,   //   cx,cy   
		LR_DEFAULTCOLOR  );  // LR_CREATEDIBSECTION   |   LR_LOADMAP3DCOLORS  
	//int error=GetLastError();
	bm.Attach(hbm);  
	bm.GetBitmap(&bminfo);
	if(!imgList.Create(bminfo.bmHeight,bminfo.bmHeight,ILC_COLOR32|ILC_MASK,bminfo.bmWidth/bminfo.bmHeight,0))
	{
		TRACE0("Unable to create image list!");
		bm.DeleteObject();
		return FALSE;
	}
	imgList.Add(&bm,clrMask);
	switch(nType)
	{
	case SIL_NORMAL:
		GetToolBarCtrl().SetImageList(&imgList);
		break;
	case SIL_HOT:
		GetToolBarCtrl().SetHotImageList(&imgList);
		break;
	case SIL_DISABLED:
		GetToolBarCtrl().SetDisabledImageList(&imgList);
		break;
	default:
		TRACE0("No this type!");
	}
	bm.DeleteObject();
	imgList.Detach();
	return TRUE;
}
void CToolBarEx::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
// 	UINT & dwFlag=((CMainFrame *)GetParentFrame())->m_dwToolBarFlag;
// 	if(dwFlag == m_dwToolBarFlag)
// 	{
// 		return;
// 	}
// 	CToolBarCtrl & toolBarCtrl=GetToolBarCtrl();
// 	CToolBar::OnUpdateCmdUI(pTarget,bDisableIfNoHndler);
// 	if(dwFlag & TOOLBAR_CURVE_H_BIGGER)
// 	{
// 		if(!toolBarCtrl.IsButtonEnabled(ID_CURVE_H_BIGGER))
// 		{
// 			toolBarCtrl.EnableButton(ID_CURVE_H_BIGGER,TRUE);
// 		}
// 	}
// 	else
// 	{
// 		if(toolBarCtrl.IsButtonEnabled(ID_CURVE_H_BIGGER))
// 		{
// 			
// 			toolBarCtrl.EnableButton(ID_CURVE_H_BIGGER,FALSE);
// 		}
// 	}
// 	if(dwFlag & TOOLBAR_CURVE_H_SMALLER)
// 	{
// 		if(!toolBarCtrl.IsButtonEnabled(ID_CURVE_H_SMALLER))
// 		{
// 			toolBarCtrl.EnableButton(ID_CURVE_H_SMALLER,TRUE);
// 		}
// 	}
// 	else
// 	{
// 		if(toolBarCtrl.IsButtonEnabled(ID_CURVE_H_SMALLER))
// 		{
// 			
// 			toolBarCtrl.EnableButton(ID_CURVE_H_SMALLER,FALSE);
// 		}
// 	}
// 	if(dwFlag & TOOLBAR_CURVE_V_BIGGER)
// 	{
// 		if(!toolBarCtrl.IsButtonEnabled(ID_CURVE_V_BIGGER))
// 		{
// 			toolBarCtrl.EnableButton(ID_CURVE_V_BIGGER,TRUE);
// 		}
// 	}
// 	else
// 	{
// 		if(toolBarCtrl.IsButtonEnabled(ID_CURVE_V_BIGGER))
// 		{
// 			
// 			toolBarCtrl.EnableButton(ID_CURVE_V_BIGGER,FALSE);
// 		}
// 	}
// 	if(dwFlag & TOOLBAR_CURVE_V_SMALLER)
// 	{
// 		if(!toolBarCtrl.IsButtonEnabled(ID_CURVE_V_SMALLER))
// 		{
// 			toolBarCtrl.EnableButton(ID_CURVE_V_SMALLER,TRUE);
// 		}
// 	}
// 	else
// 	{
// 		if(toolBarCtrl.IsButtonEnabled(ID_CURVE_V_SMALLER))
// 		{
// 			
// 			toolBarCtrl.EnableButton(ID_CURVE_V_SMALLER,FALSE);
// 		}
// 	}
// 	m_dwToolBarFlag = dwFlag;
}