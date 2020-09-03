#include "stdafx.h"
#include "..\resource.h"
#include "LanguageManger.h"
#include "..\SID_2FY.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLanguageManger *CLanguageManger::m_pLangManager = NULL;
CLanguageManger::CLanguageManger()
{

}

CLanguageManger::~CLanguageManger()
{

}
CLanguageManger* CLanguageManger::GetPointOfLangManger()
{
    if (NULL == m_pLangManager)
    {
		m_pLangManager = new CLanguageManger();
    }
	return m_pLangManager;
}
void CLanguageManger::RealsePointOfLangManger()
{
	if (NULL != m_pLangManager)
	{
		delete m_pLangManager;
		m_pLangManager = NULL;
	}
}
/*********************************************************************
* 函数名称:CLanguageManger::LoadLanguage
* 说明:	装入或者设置语言方法2，根据宏LANGUAGE_VALUE，加载资源
* 返回值:无 
*********************************************************************/
void CLanguageManger::LoadLanguageWay2()
{
     int langId = LANGUAGE_VALUE;
	 switch (langId)
	 {
		 case 0:
             m_strCurLanguage = IDS_STR_CHINESE;
			 break;
		 case 1:
			 m_strCurLanguage = IDS_STR_ENGLISH;
			 break;
		 default:
			 m_strCurLanguage = IDS_STR_CHINESE;
			 break;
	 }

}
/*********************************************************************
* 函数名称:CLanguageManger::LoadLanguage
* 说明:	装入或者设置语言 如果 szLangSel为空则装入语言，否则设置语言
* 返回值:无 
*********************************************************************/
void CLanguageManger::LoadLanguage(const CString &szLangSel /*= _T("")*/)
{
	//查找语言
	CString szSection = _T("Setting");
	CString szKey = _T("Language"),szLang;
	DWORD dwSize = 1000;

	if(!szLangSel.IsEmpty())	//保存语言
	{
		szLang = szLangSel;
	}
	else	//读取语言设置
	{
		//获取设置的语言
		BOOL ret = GetPrivateProfileString(szSection,szKey,_T("English"),szLang.GetBuffer(dwSize),dwSize,m_szSettingPath);
		szLang.ReleaseBuffer();
	}
    m_strCurLanguage = szLang;
	//查找是否存在
	CFileFind find;
	BOOL bRet = find.FindFile(m_szCurPath + _T("Language\\*.ini"));
	bool bFound = false;
	CString szEng = _T("");
	while(bRet)
	{
		bRet = find.FindNextFile();
		if(find.IsDots() || find.IsDirectory()) continue;

		CString szValue;
		CString szFilePath = find.GetFilePath();
		if(GetPrivateProfileString(szSection,szKey,_T(""),szValue.GetBuffer(dwSize),
			dwSize,szFilePath) != 0)
		{
			szValue.ReleaseBuffer();
			if(szValue == szLang)
			{
				m_szLanguagePath = szFilePath;
				bFound = true;
				break;
			}
		}
	}
	find.Close();

	//未找到设定的语言
	if(!bFound)
	{
		int nLangID[2] = {IDR_LANG_ENG,IDR_LANG_CH};
		CString szFileName[2] = {_T("\\Multi_eng.ini"), _T("\\Multi_ch.ini")};
		CString szPath = m_szCurPath + _T("Language");
		CreateDirectory(szPath,NULL);
		for(int i = 0;i < 2;i++)
		{
			MakeResource(nLangID[i],szPath + szFileName[i]);
		}
		//默认使用英文
		m_szLanguagePath = szPath + szFileName[0];
		szLang = "English";
	}

	//保存语言设置
	WritePrivateProfileString(szSection, szKey, szLang, m_szSettingPath);
}

/*********************************************************************
* 函数名称:CMultiLanguageApp::MakeResource
* 说明:	如果未找到所需的资源文件(如设置文件，语言文件等)，从程序中产生
* 返回值:无 
*********************************************************************/
bool CLanguageManger::MakeResource(int nID, CString szPathName)
{
	HRSRC hSrc = FindResource(NULL,MAKEINTRESOURCE(nID),_T("LANGUAGE"));
	if(hSrc == NULL)	return false;

	HGLOBAL hGlobal = LoadResource(NULL,hSrc);
	if(hGlobal == NULL)	return false;

	LPVOID lp = LockResource(hGlobal);
	DWORD dwSize = SizeofResource(NULL,hSrc);

	CFile file;
	if(file.Open(szPathName,CFile::modeCreate|CFile::modeWrite))
	{
		file.Write(lp,dwSize);
		file.Close();
	}
	FreeResource(hGlobal);

	return true;
}

/*********************************************************************
* 函数名称:CLanguageManger::LoadAllLanguage
* 说明:	加载所有的语言类型
* 返回值:无 
*********************************************************************/
void CLanguageManger::LoadAllLanguage()
{
	CString szKey = _T("Language"),szSection = _T("Setting");

	CFileFind find;
	BOOL ret = find.FindFile(m_szCurPath + _T("Language\\*.ini"));
	while(ret)
	{
		ret = find.FindNextFile();
		if(find.IsDots() || find.IsDirectory()) continue;

		CString szValue;
		DWORD dwSize = 100;
		if(GetPrivateProfileString(szSection,szKey, _T(""),
			szValue.GetBuffer(dwSize),dwSize,find.GetFilePath()) != 0)
		{
			/*m_cmbLang.AddString(szValue);*/
		}
		szValue.ReleaseBuffer();
	}
	find.Close();

	DWORD dwSize = 1000;
	CString szLang;
	GetPrivateProfileString(szSection, szKey, _T("English"), szLang.GetBuffer(dwSize), dwSize, m_szSettingPath);
	szLang.ReleaseBuffer();

	/*m_cmbLang.SelectString(-1,szLang);*/
}

/*********************************************************************
* 函数名称:CLanguageManger::LoadAllLanguage
* 说明:	改变语言环境
* 参数：szValue：[in]输入改变的语言
* 返回值:无 
*********************************************************************/
//void CLanguageManger::ChangeLang(const CString &szValue) 
//{
//	LoadLanguage(szValue);
//
//	//改变菜单语言
//
//	//刷新对话框的语言显示
//	SetDialogStrings(this,IDD);
//	Invalidate(false);
//}

/*********************************************************************
* 函数名称:CLanguageManger::SetMenuStrings
* 说明:	设置菜单项
* 参数：pMainMenu:主菜单
* 返回值:无 
*********************************************************************/
void CLanguageManger::SetMenuStrings(CMenu *pMainMenu)
{
	int nCurPos = 0;
	CMenu* subMenu = pMainMenu->GetSubMenu(nCurPos);

	//文件菜单
	//pMainMenu->ModifyMenu(nCurPos, MF_BYPOSITION, nCurPos, LoadString(_T("IDS_MENU_FILE")));
	//subMenu->ModifyMenu(ID_FILE_NEW,MF_BYCOMMAND, ID_FILE_NEW, LoadString(_T("IDS_MENU_FILE_NEW")));
	//subMenu->ModifyMenu(ID_FILE_OPEN,MF_BYCOMMAND, ID_FILE_OPEN, LoadString(_T("IDS_MENU_FILE_OPEN")));
	//subMenu->ModifyMenu(ID_FILE_CLOSE,MF_BYCOMMAND, ID_FILE_CLOSE, LoadString(_T("IDS_MENU_FILE_CLOSE")));
	//subMenu->ModifyMenu(ID_APP_EXIT,MF_BYCOMMAND, ID_APP_EXIT, LoadString(_T("IDS_MENU_FILE_EXIT")));

	//编辑菜单
	//subMenu = m_menuMain.GetSubMenu();											
	//m_menuMain.ModifyMenu(nCurPos,MF_BYPOSITION,nCurPos,g_LoadString("IDS_MENU_EDIT"));
	//subMenu->ModifyMenu(ID_EDIT_CUT,MF_BYCOMMAND,ID_EDIT_CUT,g_LoadString("IDS_MENU_EDIT_CUT"));
	//subMenu->ModifyMenu(ID_EDIT_COPY,MF_BYCOMMAND,ID_EDIT_COPY,g_LoadString("IDS_MENU_EDIT_COPY"));
	//subMenu->ModifyMenu(ID_EDIT_PASTE,MF_BYCOMMAND,ID_EDIT_PASTE,g_LoadString("IDS_MENU_EDIT_PASTE"));
    ++nCurPos;
	//帮助菜单
	subMenu = pMainMenu->GetSubMenu(++nCurPos);	
	//pMainMenu->ModifyMenu(nCurPos,MF_BYPOSITION,nCurPos,LoadString(_T("IDS_MENU_HELP")));
	//subMenu->ModifyMenu(ID_HELP,MF_BYCOMMAND,ID_HELP,LoadString(_T("IDS_MENU_HELP_HELP")));
	//subMenu->ModifyMenu(ID_APP_ABOUT,MF_BYCOMMAND,ID_APP_ABOUT,LoadString(_T("IDS_MENU_HELP_ABOUT")));

}

/*********************************************************************
* 函数名称:g_LoadString
* 说明:	根据标识 szID到选定的语言文件中加载字符串
* 参数：strID：[in]字符串资源ID 
               [out]返回对应资源的字符串
* 返回值：szID对应的Value（实际对应的字符串）
*********************************************************************/
void CLanguageManger::LoadString(CString &strID)
{
	
	DWORD dwSize = 1000;
	CString szID = strID;
	strID = _T("");
	GetPrivateProfileString(_T("String"), szID, _T("Not found"),
		strID.GetBuffer(dwSize),dwSize,m_szLanguagePath);
	strID.ReleaseBuffer();

	strID.Replace(_T("\\n"), _T("\n"));	//替换回换行符号

	/*return szValue;*/
}

/*********************************************************************
* 函数名称:SetDialogStrings(CDialog *pDlg,UINT uDlgID)
* 说明:	当对话框运行时获取其所有可得到的字符串，并保存到语言文件中
* 每个控件用对话框ID和控件ID唯一标识
*
* 入口参数:
* CDialog *pDlg -- 对话框的指针
*  UINT uDlgID -- 该对话框的ID
* 返回值：无
*********************************************************************/
void CLanguageManger::SetDialogStrings(CDialog *pDlg,UINT uDlgID)
{
	CString szSection = _T("String");
	CString szKey,szText;
	bool bSetText = 0;	//1:从文件读，设置对话框
	//0:从对话框读，保存到文件

	if(bSetText)	//1:从文件读，设置对话框
	{
		CString szDefault = _T("");
		DWORD dwSize = 1000;
		wchar_t* pData = (wchar_t*)malloc(dwSize);

		//读对话框标题
		szKey.Format(_T("IDD%d_Title"),uDlgID);
		if(GetPrivateProfileString(szSection,szKey,szDefault,
			pData,dwSize,m_szLanguagePath) != 0)
		{
			pDlg->SetWindowText(pData);
		}

		//写入各个子控件的标题文字
		CWnd* pWnd = pDlg->GetWindow(GW_CHILD);
		while(pWnd != NULL)
		{
			szKey.Format(_T("IDD%d_%d"),uDlgID,pWnd->GetDlgCtrlID());
			if(GetPrivateProfileString(szSection,szKey,szDefault,
				pData,dwSize,m_szLanguagePath) != 0)
			{
				pWnd->SetWindowText(pData);
			}

			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}

		//释放内存
		free(pData);
	}
	else	//0:从对话框读，保存到文件
	{
		//写入对话框标题
		szKey.Format(_T("IDD%d_Title"),uDlgID);
		pDlg->GetWindowText(szText);
		WritePrivateProfileString(szSection,szKey,szText,m_szLanguagePath);

		//写入各个子控件的标题文字
		CWnd* pWnd = pDlg->GetWindow(GW_CHILD);
		while(pWnd != NULL)
		{
			szKey.Format(_T("IDD%d_%d"),uDlgID,pWnd->GetDlgCtrlID());
			pWnd->GetWindowText(szText);
			WritePrivateProfileString(szSection,szKey,szText,m_szLanguagePath);

			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
	}
}

/*********************************************************************
* 函数名称:SetWndStrings(CDialog *pDlg,UINT uDlgID)
* 说明:	当窗口运行时获取其所有可得到的字符串，并保存到语言文件中
* 每个控件用对话框ID和控件ID唯一标识
*
* 入口参数:
* CWnd *pWnd -- 窗口的指针
*  UINT uDlgID --窗口对应ID,默认值为0表示控件ID自己算
* 返回值：无
*********************************************************************/
void CLanguageManger::SetWndStrings(CWnd *pWnd,UINT uWndID/* = 0*/)
{
	ASSERT(NULL != pWnd);
	CString szSection = _T("String");
	CString szKey,szText;
	bool bSetText = 0;	//1:从文件读，设置对话框
	//0:从对话框读，保存到文件
    if (0 == uWndID)
    {
       uWndID = pWnd->GetDlgCtrlID();
    }
	if(bSetText)	//1:从文件读，设置对话框
	{
		CString szDefault = _T("");
		DWORD dwSize = 1000;
		wchar_t* pData = (wchar_t*)malloc(dwSize);

		//读对话框标题
		szKey.Format(_T("IDWND%d_Title"), uWndID);
		if(GetPrivateProfileString(szSection,szKey,szDefault,
			pData,dwSize,m_szLanguagePath) != 0)
		{
			pWnd->SetWindowText(pData);
		}

		//写入各个子控件的标题文字
		CWnd* pWndChild = pWnd->GetWindow(GW_CHILD);
		while(pWndChild != NULL)
		{
			szKey.Format(_T("IDWND%d_%d"), uWndID, pWndChild->GetDlgCtrlID());
			if(GetPrivateProfileString(szSection,szKey,szDefault,
				pData,dwSize,m_szLanguagePath) != 0)
			{
				pWndChild->SetWindowText(pData);
			}

			pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
		}

		//释放内存
		free(pData);
	}
	else	//0:从对话框读，保存到文件
	{
		//写入对话框标题
		szKey.Format(_T("IDWND%d_Title"), uWndID);
		pWnd->GetWindowText(szText);
		WritePrivateProfileString(szSection,szKey,szText,m_szLanguagePath);

		//写入各个子控件的标题文字
		CWnd* pWndChild = pWnd->GetWindow(GW_CHILD);
		while(pWndChild != NULL)
		{
			szKey.Format(_T("IDWND%d_%d"), uWndID, pWndChild->GetDlgCtrlID());
			pWndChild->GetWindowText(szText);
			WritePrivateProfileString(szSection, szKey, szText, m_szLanguagePath);

			pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
		}
	}
}