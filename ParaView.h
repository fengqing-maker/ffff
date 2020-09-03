#if !defined(AFX_PARAVIEW_H__9EBB2908_FD58_4A0E_A8BA_3D888D264A28__INCLUDED_)
#define AFX_PARAVIEW_H__9EBB2908_FD58_4A0E_A8BA_3D888D264A28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParaView.h : header file
#include	<map>
using namespace  std;
//

/////////////////////////////////////////////////////////////////////////////
// CParaView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "XListCtrl.h"
#include "afxwin.h"
#define MAX_PARA_COUNT 500
class CParaView : public CFormView
{
protected:
	CParaView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CParaView)

// Form Data
public:
	//{{AFX_DATA(CParaView)
	enum { IDD = IDD_PARA_VIEW };
	CComboBox	m_ctlParaType;
	CXListCtrl	m_listPara;
	CComboBox	m_ctlParaArea;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	int m_nCurIndex;
	int m_nParasEachType[10];
	BOOL m_bSystemPara;
	VOID ReadParaFile();
	VOID ReadDevicePara();
	int typeCount;
	void CompareParaType(CString * strArray, CString str);
	void LoadParaTypeData(CString strParaType);
	void LoadData();
	static CString GetParaDataValueString(ParaData & paraData);
	static unsigned short SetParaDataValue(double dValue,unsigned short nDecBit);
	static double GetParaDataValue(unsigned short nValue,unsigned short nDecBit);


	void	OnReadXLSDate(CString );
		
    BYTE m_nDisParaArea;
	BYTE m_nCurParaArea;
	BYTE m_nParaAreaCount;
	unsigned short m_nParaCount;
	ParaData m_arrParaData[MAX_PARA_COUNT];
	CString strParaType;
	CString m_nPsw;
	//2018-11-13 CZZ
	BYTE	m_nSetParaArea;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParaView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CParaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CParaView)
	afx_msg void OnSetCurParaAreaBtn();
	afx_msg void OnSetParaBtn();
	afx_msg void OnSelchangeParaType();
	afx_msg void OnSelchangeParaArea();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnReadPara();
	afx_msg void OnReadParaFile();
	//2018-11-12 CZZ
	BOOL	OnAnalyseExcelDate(CString strExcelPath);
	void	OnIntToAlphater(int nRow,int nColumn,CString &strCurrntRow);


	afx_msg void OnDeviceReset();
	//}}AFX_MSG
	afx_msg LRESULT OnEditChange(WPARAM nItem, LPARAM nSubItem);           //当改变数值形定值时触发
	afx_msg LRESULT OnComboxSelection(WPARAM nItem, LPARAM nSubItem);
	DECLARE_MESSAGE_MAP()
private:
	CString GetErrorInfo( int errorNum );
public:
	CComboBox m_ctlSetArea;
	CString m_strSetArea;
	afx_msg void OnCbnSelchangeComboSetarea();
	//定义解析EXCEL文件的容器缓存空间
	void	OnShowExcelDate(CStringArray *arrCurrentParaDate,int nRow);
	BOOL	m_bIputExcelFile;
	int		OnCalculateParaValue(CString strParavalue,int nPointCount);
	void	OnCalulateString(int nRow,int nColumn);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAVIEW_H__9EBB2908_FD58_4A0E_A8BA_3D888D264A28__INCLUDED_)
