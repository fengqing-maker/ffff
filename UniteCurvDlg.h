#if !defined(AFX_UNITECURVDLG_H__D342C470_1B6F_406A_88E6_D21495053F68__INCLUDED_)
#define AFX_UNITECURVDLG_H__D342C470_1B6F_406A_88E6_D21495053F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UniteCurvDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUniteCurvDlg dialog

class CUniteCurvDlg : public CDialog
{
// Construction
public:
	int m_defalutCheck;
	CString m_defalutFile;
	bool isUnited;
	CString m_newFileName;
	int m_appointCurveMilliSecond;
	int m_diffMilliSecond;
	bool DoWaveFileUnite(CString waveFile1,CString waveFile2);
	int m_nFileCount;
	CString m_curSel;
	COleDateTime m_appointCurveStartTime;
	CString m_filePath;
	CUniteCurvDlg(CString fileName,CWnd* pParent = NULL);   // standard constructor

	//当前打开的录波文件信息类
	CWareFileData m_wareFileData;
// Dialog Data
	//{{AFX_DATA(CUniteCurvDlg)
	enum { IDD = IDD_UNITE_CURVE };
	CListCtrl	m_uniteCurveList;
	CString	m_showFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUniteCurvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUniteCurvDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCurveList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnAutoSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITECURVDLG_H__D342C470_1B6F_406A_88E6_D21495053F68__INCLUDED_)
