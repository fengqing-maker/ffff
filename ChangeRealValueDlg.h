#if !defined(AFX_CHANGEREALVALUEDLG_H__95503361_296C_4C65_9389_F14C73BCFE3A__INCLUDED_)
#define AFX_CHANGEREALVALUEDLG_H__95503361_296C_4C65_9389_F14C73BCFE3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeRealValueDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeRealValueDlg dialog

class CChangeRealValueDlg : public CDialog
{
// Construction
public:
	double m_dSecondValue;
	int m_nChannelIndex;
	void SetBufCRC(BYTE * fileBuf,int nFileLength);
	CChangeRealValueDlg(int nSubItem,int nChannelIndex,int nPointIndex,CWareFileData * pWareFileData,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeRealValueDlg)
	enum { IDD = IDD_CHANGE_REAL_VALUE };
	CString	m_strChannelName;
	int		m_nSampleNum;
	int		m_nNewSampleValue;
	int		m_nOldSampleValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeRealValueDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nSubItem;
	CWareFileData * m_pWareFileData;

	// Generated message map functions
	//{{AFX_MSG(CChangeRealValueDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEREALVALUEDLG_H__95503361_296C_4C65_9389_F14C73BCFE3A__INCLUDED_)
