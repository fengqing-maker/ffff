#if !defined(AFX_LANGUAGE_MANGER_N2345_H__FD8E1698_F985_4904_8249__INCLUDED_)
#define AFX_LANGUAGE_MANGER_N2345_H__FD8E1698_F985_4904_8249__INCLUDED_

#define LANGUAGE_VALUE 1 //0:为中文 1:为英文
// #define LANGUAGE_VALUE 1 //0:为中文 1:为英文

//自定义界面的资源ID
#define IDS_STR_CHINESE  _T("Chinese")
#define IDS_STR_ENGLISH  _T("English")

class CLanguageManger
{
public:
	~CLanguageManger();
	void   RealsePointOfLangManger();
	static CLanguageManger*  GetPointOfLangManger();
    //装入设置语言
	void LoadLanguage(const CString &szLangSel = _T(""));
	void LoadLanguageWay2();
	//加载所有语言类型
	void LoadAllLanguage();
    //从程序的资源文件产生资源
	bool MakeResource(int nID, CString szPathName);
    //改变语言环境
	/*void ChangeLang(const CString &szValue);*/
    //根据标识 szID到选定的语言文件中加载字符串
	void LoadString(CString &nID);
	//当对话框运行时获取其所有可得到的字符串，并保存到语言文件中每个控件用对话框ID和控件ID唯一标识
	void SetDialogStrings(CDialog *pDlg,UINT uDlgID);
    void SetWndStrings(CWnd *pWnd,UINT uWndID = 0);
	//设置菜单
	void SetMenuStrings(CMenu *pMainMenu);

	CString m_strCurLanguage;
	CString m_szLanguagePath;	//保存语言文件的路径
	CString m_szSettingPath;	//保存设置语言init文件路径
	CString	m_szCurPath;		//保存当前路径
protected:
	
	
private:
	CLanguageManger();
	static CLanguageManger *m_pLangManager;
};


#endif //endif(AFX_LANGUAGE_MANGER_N2345_H__FD8E1698_F985_4904_8249__INCLUDED_)