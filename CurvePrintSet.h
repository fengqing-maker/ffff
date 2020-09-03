#if !defined(AFX_CURVEPRINTSET_H__662D8645_EE2F_41AC_870A_6C70F30F47FE__INCLUDED_)
#define AFX_CURVEPRINTSET_H__662D8645_EE2F_41AC_870A_6C70F30F47FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurvePrintSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurvePrintSet dialog

class CCurvePrintSet : public CDialog
{
// Construction
public:
	BOOL m_bMultiColor;
	char m_cPS;
	BOOL m_bHPrint;
	double m_dMaxTime;
	double m_dMinTime;
	CCurvePrintSet(CWareFileData *pWareFileData,BOOL bLabel=FALSE,int dotaTime=0,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCurvePrintSet)
	enum { IDD = IDD_PRINT_SET };
	CComboBox	m_ctlColorStyle;
	CComboBox	m_ctlDrawStyle;
	double	m_dEndTime;
	BOOL	m_bDisChannelInfo;
	BOOL	m_bDisFileInfo;
	BOOL	m_bDisParaInfo;
	double	m_dStartTime;
	double	m_dTestTime;
	CString	m_strPrintChannel;
	BOOL	m_bPrintPageNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurvePrintSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCurvePrintSet)
	afx_msg void OnPrintChannelBtn();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
	CWareFileData * m_pWareFileData;
	const BOOL m_bLabel;
	const BOOL m_nDotaTime;
	const int m_HPrintMinTime;   //横向打印时最少打印的时间长度
	const int m_VPrintMinTime;   //纵向打印时阳少打印的时间长度
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVEPRINTSET_H__662D8645_EE2F_41AC_870A_6C70F30F47FE__INCLUDED_)
