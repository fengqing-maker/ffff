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
//�û���ʾ���ɫ����������
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
	//�����˵��б�
	CArray<DROPDOWNLIST,DROPDOWNLIST> m_DDBtnArray;
public:
	void AddDropDownBtn(UINT nIDBUTTON, UINT  nIDMENU);
	void RemoveDropDownBtn(UINT nIDBUTTON);
	//���ù�������ͼ��
    // 	������
    // 	nIDImageList��  λͼ��ID��
    //  clrMask:        Ҫ���ε���ɫ
    //  nType:  SIL_NORMAL      ���������ͼ��
	//          SIL_DISABLED    ����ʱ��ͼ��
	BOOL SetImageList(UINT nIDImageList,COLORREF clrMask, UINT nType);
};


