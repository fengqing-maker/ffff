#pragma once


// CToolBarEx
//
// Copyright ? wzb 2005
// E-mail:wzb13@163.com

#include <afxtempl.h>

typedef struct tagDROPDOWNLIST
{
	UINT nIDButton;
	UINT nIDMenu;
}DROPDOWNLIST;
//用户显示真彩色工具栏的类
class CToolBarEx : public CToolBar
{
	DECLARE_DYNAMIC(CToolBarEx)

public:
	CToolBarEx();
	virtual ~CToolBarEx();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	enum{SIL_NORMAL,SIL_HOT,SIL_DISABLED};

protected:
	afx_msg void OnDropDownBtn( NMHDR * pNotifyStruct, LRESULT * result );
	DECLARE_MESSAGE_MAP()
protected:
	UINT m_dwToolBarFlag;
	BOOL m_bDropDownStyle;
	//下拉菜单列表
	CArray<DROPDOWNLIST,DROPDOWNLIST> m_DDBtnArray;
public:
	void AddDropDownBtn(UINT nIDBUTTON, UINT  nIDMENU);
	void RemoveDropDownBtn(UINT nIDBUTTON);
	//设置工具栏的图标
    // 	参数：
    // 	nIDImageList：  位图的ID号
    //  clrMask:        要屏蔽的颜色
    //  nType:  SIL_NORMAL      正常情况下图标
	//          SIL_DISABLED    禁用时的图标
	BOOL SetImageList(UINT nIDImageList,COLORREF clrMask, UINT nType);
};


