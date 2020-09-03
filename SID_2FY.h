// SID_2FY.h : main header file for the SID_2FY application
//

#if !defined(AFX_SID_2FY_H__81FC99DE_2DA7_4F47_BCD6_FA1788A5C147__INCLUDED_)
#define AFX_SID_2FY_H__81FC99DE_2DA7_4F47_BCD6_FA1788A5C147__INCLUDED_
#include <map>
using namespace std;
#include "ProjectData.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#define LAW_PC_PORT 9002
#include "resource.h"       // main symbols
#include "WareFileData.h"
#include "./Languge/LanguageManger.h"
/////////////////////////////////////////////////////////////////////////////
// CMySID_2FYApp:
// See SID_2FY.cpp for the implementation of this class
//

class CMySID_2FYApp : public CWinApp
{
public:
	CLanguageManger *m_pLangManger;
	int m_nReturnSecStart;
	CString m_strAppPath;
	CProjectData m_projectData;   //当前工程信息，有模拟量显示通道、
	CAdoDatabase m_adoDatabase;
	map <CString,CString> m_mapDiviceType;       //支持的设备类型
	map <CString,CString>::iterator m_tempItem;
	char m_strOmnipotencePSW[40];
	CString m_strPSWFilePath;
	char m_strCurrentPSW[40];
	BOOL SetCurPassWord(const char *strPSW);
	BOOL InitPasswordFile();
	CMySID_2FYApp();
	virtual ~CMySID_2FYApp();
// 	CView* m_pOldView;
// 	CView* m_pNewView;
// 	CView* SwitchView(CView* pNewView);
	int m_nWareDataCount;           //当前有多少过临时录波文件
	CEvent ev[6];
	CEvent m_eRestart;
	float  m_freq[4];
	int m_freqIndex;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySID_2FYApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMySID_2FYApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SID_2FY_H__81FC99DE_2DA7_4F47_BCD6_FA1788A5C147__INCLUDED_)
