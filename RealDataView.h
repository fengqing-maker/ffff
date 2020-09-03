#if !defined(AFX_REALDATAVIEW_H__B49FCB36_7743_4575_A466_F2B3D3B7E6E5__INCLUDED_)
#define AFX_REALDATAVIEW_H__B49FCB36_7743_4575_A466_F2B3D3B7E6E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RealDataView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRealDataView form view
#define MAX_REAL_COUNT 256
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#define IDC_FREQ_TIMER 0X01

//DWORD WINAPI FreshRealThread(LPVOID lpParameter);//����֡�洢�߳�

class CRealDataView : public CFormView
{
public:
	CRealDataView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRealDataView)

// Form Data
public:
	MapRealData m_mapRealData;
	MapRealData::iterator m_tempItem;
	//{{AFX_DATA(CRealDataView)
	enum { IDD = IDD_REAL_DATA_VIEW };
	CComboBox	m_ctlDataType;      //ʵ�����������б�
	CListCtrl	m_listRealData;
//	CComboBox	m_ctlRefurbishFreq;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
//	static double m_dRefurbishFreq;
	//����ʵ�������ݵ�����
	UINT m_nCount;
	RealData m_arrRealData[MAX_REAL_COUNT];
	BOOL LoadData(BOOL bIsDelAllItem = FALSE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRealDataView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetGrayTool();
	CEvent m_bReadFinish;//������ʶ�˴�ͨѶ�Ƿ����
	CString m_strDataType;
	HANDLE hThread;              //�߳�
 	static int m_nDataType;      //���ڴ洢m_cltDataType��ѡ�е�������ţ���һ��Ϊ0
	virtual ~CRealDataView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CRealDataView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeRealDataType();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REALDATAVIEW_H__B49FCB36_7743_4575_A466_F2B3D3B7E6E5__INCLUDED_)
