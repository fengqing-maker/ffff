#if !defined(AFX_CUSTOMIZEWARESETDLG_H__64628CDA_5608_439E_A2DC_F6C22476CB2C__INCLUDED_)
#define AFX_CUSTOMIZEWARESETDLG_H__64628CDA_5608_439E_A2DC_F6C22476CB2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomizeWareSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomizeWareSetDlg dialog
#include "WareFileData.h"
#include "TreeCtrlEx.h"
#include <list>
class CCustomizeWareSetDlg : public CDialog
{
// Construction
public:
	UINT m_nAngleShowCount;
	UINT m_nFrequencyShowCount;
	
	BOOL IsAppointChannel(const CString & strItemText);
	const BOOL m_bShowDialog;
	const Curve * m_pCurve;
	const int m_nCurveCount;
	const BOOL m_bPrint;
	BOOL AddToDisWithStr(const CString & str,UINT nItemType);
	CWareFileData * m_pWareFileData;
	char m_cPS;
	UINT m_nDigitalInShowCount;
	UINT m_nDigitalOutShowCount;
	UINT m_nAnalogShowCount;
	CCustomizeWareSetDlg(CWareFileData * pWareFileData,BOOL bShowDialog=FALSE,Curve * pOldCurve=NULL,int nCurveCount=0,
		BOOL bPrint=FALSE,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomizeWareSetDlg)
	enum { IDD = IDD_CUSTOMIZE_WARE_SET };
	CComboBox	m_comBoxValueType;
	CTreeCtrlEx	m_fileTree;
	CTreeCtrlEx	m_displayTree;
	UINT m_nIndexOfWareFile;
//	CurveSetting m_curveSetting;
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomizeWareSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void MoveTreeItem(CTreeCtrlEx &sourceTree, CTreeCtrlEx &desTree,BOOL bAddToDis = TRUE);

	// Generated message map functions
	//{{AFX_MSG(CCustomizeWareSetDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkFileData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDisplayData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddBtn();
	afx_msg void OnSubBtn();
	afx_msg void OnAppointAddBtn();
	afx_msg void OnMoveUpBtn();
	afx_msg void OnMoveDownBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void AddCurveToDisplay(map<UINT,CString> mapToString);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMIZEWARESETDLG_H__64628CDA_5608_439E_A2DC_F6C22476CB2C__INCLUDED_)
