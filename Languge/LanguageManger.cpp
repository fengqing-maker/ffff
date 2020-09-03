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
* ��������:CLanguageManger::LoadLanguage
* ˵��:	װ������������Է���2�����ݺ�LANGUAGE_VALUE��������Դ
* ����ֵ:�� 
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
* ��������:CLanguageManger::LoadLanguage
* ˵��:	װ������������� ��� szLangSelΪ����װ�����ԣ�������������
* ����ֵ:�� 
*********************************************************************/
void CLanguageManger::LoadLanguage(const CString &szLangSel /*= _T("")*/)
{
	//��������
	CString szSection = _T("Setting");
	CString szKey = _T("Language"),szLang;
	DWORD dwSize = 1000;

	if(!szLangSel.IsEmpty())	//��������
	{
		szLang = szLangSel;
	}
	else	//��ȡ��������
	{
		//��ȡ���õ�����
		BOOL ret = GetPrivateProfileString(szSection,szKey,_T("English"),szLang.GetBuffer(dwSize),dwSize,m_szSettingPath);
		szLang.ReleaseBuffer();
	}
    m_strCurLanguage = szLang;
	//�����Ƿ����
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

	//δ�ҵ��趨������
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
		//Ĭ��ʹ��Ӣ��
		m_szLanguagePath = szPath + szFileName[0];
		szLang = "English";
	}

	//������������
	WritePrivateProfileString(szSection, szKey, szLang, m_szSettingPath);
}

/*********************************************************************
* ��������:CMultiLanguageApp::MakeResource
* ˵��:	���δ�ҵ��������Դ�ļ�(�������ļ��������ļ���)���ӳ����в���
* ����ֵ:�� 
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
* ��������:CLanguageManger::LoadAllLanguage
* ˵��:	�������е���������
* ����ֵ:�� 
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
* ��������:CLanguageManger::LoadAllLanguage
* ˵��:	�ı����Ի���
* ������szValue��[in]����ı������
* ����ֵ:�� 
*********************************************************************/
//void CLanguageManger::ChangeLang(const CString &szValue) 
//{
//	LoadLanguage(szValue);
//
//	//�ı�˵�����
//
//	//ˢ�¶Ի����������ʾ
//	SetDialogStrings(this,IDD);
//	Invalidate(false);
//}

/*********************************************************************
* ��������:CLanguageManger::SetMenuStrings
* ˵��:	���ò˵���
* ������pMainMenu:���˵�
* ����ֵ:�� 
*********************************************************************/
void CLanguageManger::SetMenuStrings(CMenu *pMainMenu)
{
	int nCurPos = 0;
	CMenu* subMenu = pMainMenu->GetSubMenu(nCurPos);

	//�ļ��˵�
	//pMainMenu->ModifyMenu(nCurPos, MF_BYPOSITION, nCurPos, LoadString(_T("IDS_MENU_FILE")));
	//subMenu->ModifyMenu(ID_FILE_NEW,MF_BYCOMMAND, ID_FILE_NEW, LoadString(_T("IDS_MENU_FILE_NEW")));
	//subMenu->ModifyMenu(ID_FILE_OPEN,MF_BYCOMMAND, ID_FILE_OPEN, LoadString(_T("IDS_MENU_FILE_OPEN")));
	//subMenu->ModifyMenu(ID_FILE_CLOSE,MF_BYCOMMAND, ID_FILE_CLOSE, LoadString(_T("IDS_MENU_FILE_CLOSE")));
	//subMenu->ModifyMenu(ID_APP_EXIT,MF_BYCOMMAND, ID_APP_EXIT, LoadString(_T("IDS_MENU_FILE_EXIT")));

	//�༭�˵�
	//subMenu = m_menuMain.GetSubMenu();											
	//m_menuMain.ModifyMenu(nCurPos,MF_BYPOSITION,nCurPos,g_LoadString("IDS_MENU_EDIT"));
	//subMenu->ModifyMenu(ID_EDIT_CUT,MF_BYCOMMAND,ID_EDIT_CUT,g_LoadString("IDS_MENU_EDIT_CUT"));
	//subMenu->ModifyMenu(ID_EDIT_COPY,MF_BYCOMMAND,ID_EDIT_COPY,g_LoadString("IDS_MENU_EDIT_COPY"));
	//subMenu->ModifyMenu(ID_EDIT_PASTE,MF_BYCOMMAND,ID_EDIT_PASTE,g_LoadString("IDS_MENU_EDIT_PASTE"));
    ++nCurPos;
	//�����˵�
	subMenu = pMainMenu->GetSubMenu(++nCurPos);	
	//pMainMenu->ModifyMenu(nCurPos,MF_BYPOSITION,nCurPos,LoadString(_T("IDS_MENU_HELP")));
	//subMenu->ModifyMenu(ID_HELP,MF_BYCOMMAND,ID_HELP,LoadString(_T("IDS_MENU_HELP_HELP")));
	//subMenu->ModifyMenu(ID_APP_ABOUT,MF_BYCOMMAND,ID_APP_ABOUT,LoadString(_T("IDS_MENU_HELP_ABOUT")));

}

/*********************************************************************
* ��������:g_LoadString
* ˵��:	���ݱ�ʶ szID��ѡ���������ļ��м����ַ���
* ������strID��[in]�ַ�����ԴID 
               [out]���ض�Ӧ��Դ���ַ���
* ����ֵ��szID��Ӧ��Value��ʵ�ʶ�Ӧ���ַ�����
*********************************************************************/
void CLanguageManger::LoadString(CString &strID)
{
	
	DWORD dwSize = 1000;
	CString szID = strID;
	strID = _T("");
	GetPrivateProfileString(_T("String"), szID, _T("Not found"),
		strID.GetBuffer(dwSize),dwSize,m_szLanguagePath);
	strID.ReleaseBuffer();

	strID.Replace(_T("\\n"), _T("\n"));	//�滻�ػ��з���

	/*return szValue;*/
}

/*********************************************************************
* ��������:SetDialogStrings(CDialog *pDlg,UINT uDlgID)
* ˵��:	���Ի�������ʱ��ȡ�����пɵõ����ַ����������浽�����ļ���
* ÿ���ؼ��öԻ���ID�Ϳؼ�IDΨһ��ʶ
*
* ��ڲ���:
* CDialog *pDlg -- �Ի����ָ��
*  UINT uDlgID -- �öԻ����ID
* ����ֵ����
*********************************************************************/
void CLanguageManger::SetDialogStrings(CDialog *pDlg,UINT uDlgID)
{
	CString szSection = _T("String");
	CString szKey,szText;
	bool bSetText = 0;	//1:���ļ��������öԻ���
	//0:�ӶԻ���������浽�ļ�

	if(bSetText)	//1:���ļ��������öԻ���
	{
		CString szDefault = _T("");
		DWORD dwSize = 1000;
		wchar_t* pData = (wchar_t*)malloc(dwSize);

		//���Ի������
		szKey.Format(_T("IDD%d_Title"),uDlgID);
		if(GetPrivateProfileString(szSection,szKey,szDefault,
			pData,dwSize,m_szLanguagePath) != 0)
		{
			pDlg->SetWindowText(pData);
		}

		//д������ӿؼ��ı�������
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

		//�ͷ��ڴ�
		free(pData);
	}
	else	//0:�ӶԻ���������浽�ļ�
	{
		//д��Ի������
		szKey.Format(_T("IDD%d_Title"),uDlgID);
		pDlg->GetWindowText(szText);
		WritePrivateProfileString(szSection,szKey,szText,m_szLanguagePath);

		//д������ӿؼ��ı�������
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
* ��������:SetWndStrings(CDialog *pDlg,UINT uDlgID)
* ˵��:	����������ʱ��ȡ�����пɵõ����ַ����������浽�����ļ���
* ÿ���ؼ��öԻ���ID�Ϳؼ�IDΨһ��ʶ
*
* ��ڲ���:
* CWnd *pWnd -- ���ڵ�ָ��
*  UINT uDlgID --���ڶ�ӦID,Ĭ��ֵΪ0��ʾ�ؼ�ID�Լ���
* ����ֵ����
*********************************************************************/
void CLanguageManger::SetWndStrings(CWnd *pWnd,UINT uWndID/* = 0*/)
{
	ASSERT(NULL != pWnd);
	CString szSection = _T("String");
	CString szKey,szText;
	bool bSetText = 0;	//1:���ļ��������öԻ���
	//0:�ӶԻ���������浽�ļ�
    if (0 == uWndID)
    {
       uWndID = pWnd->GetDlgCtrlID();
    }
	if(bSetText)	//1:���ļ��������öԻ���
	{
		CString szDefault = _T("");
		DWORD dwSize = 1000;
		wchar_t* pData = (wchar_t*)malloc(dwSize);

		//���Ի������
		szKey.Format(_T("IDWND%d_Title"), uWndID);
		if(GetPrivateProfileString(szSection,szKey,szDefault,
			pData,dwSize,m_szLanguagePath) != 0)
		{
			pWnd->SetWindowText(pData);
		}

		//д������ӿؼ��ı�������
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

		//�ͷ��ڴ�
		free(pData);
	}
	else	//0:�ӶԻ���������浽�ļ�
	{
		//д��Ի������
		szKey.Format(_T("IDWND%d_Title"), uWndID);
		pWnd->GetWindowText(szText);
		WritePrivateProfileString(szSection,szKey,szText,m_szLanguagePath);

		//д������ӿؼ��ı�������
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