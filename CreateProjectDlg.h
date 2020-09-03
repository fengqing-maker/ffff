#if !defined(AFX_CREATEPROJECTDLG_H__87CBC698_DCE6_497A_BDEC_745F9A9916A6__INCLUDED_)
#define AFX_CREATEPROJECTDLG_H__87CBC698_DCE6_497A_BDEC_745F9A9916A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateProjectDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CCreateProjectDlg dialog

class CCreateProjectDlg : public CDialog
{
// Construction
public:
	int m_nSel;
	BOOL m_bCreateResult;                 //TRUE����ʾ�������̳ɹ���FALSE����ʾʧ��
	CImageList m_imageList;
	CCreateProjectDlg(CWnd* pParent = NULL);   // standard constructor
	
	unsigned long	m_ipaddrDevice;
// Dialog Data
	//{{AFX_DATA(CCreateProjectDlg)
	enum { IDD = IDD_CREATE_PROJECT_DLG };
	CListCtrl	m_listDeviceType;
	CString	m_strPath;
	CString	m_strProjectName;
	BOOL	m_bLinkDevice;         //�Ƿ񴴽�ʱ�����豸
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateProjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateProjectDlg)
	afx_msg void OnPathBtn();
	virtual void OnOK();
	afx_msg void OnChangeProjectName();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLinkType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEPROJECTDLG_H__87CBC698_DCE6_497A_BDEC_745F9A9916A6__INCLUDED_)
