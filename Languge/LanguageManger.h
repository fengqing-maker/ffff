#if !defined(AFX_LANGUAGE_MANGER_N2345_H__FD8E1698_F985_4904_8249__INCLUDED_)
#define AFX_LANGUAGE_MANGER_N2345_H__FD8E1698_F985_4904_8249__INCLUDED_

#define LANGUAGE_VALUE 1 //0:Ϊ���� 1:ΪӢ��
// #define LANGUAGE_VALUE 1 //0:Ϊ���� 1:ΪӢ��

//�Զ���������ԴID
#define IDS_STR_CHINESE  _T("Chinese")
#define IDS_STR_ENGLISH  _T("English")

class CLanguageManger
{
public:
	~CLanguageManger();
	void   RealsePointOfLangManger();
	static CLanguageManger*  GetPointOfLangManger();
    //װ����������
	void LoadLanguage(const CString &szLangSel = _T(""));
	void LoadLanguageWay2();
	//����������������
	void LoadAllLanguage();
    //�ӳ������Դ�ļ�������Դ
	bool MakeResource(int nID, CString szPathName);
    //�ı����Ի���
	/*void ChangeLang(const CString &szValue);*/
    //���ݱ�ʶ szID��ѡ���������ļ��м����ַ���
	void LoadString(CString &nID);
	//���Ի�������ʱ��ȡ�����пɵõ����ַ����������浽�����ļ���ÿ���ؼ��öԻ���ID�Ϳؼ�IDΨһ��ʶ
	void SetDialogStrings(CDialog *pDlg,UINT uDlgID);
    void SetWndStrings(CWnd *pWnd,UINT uWndID = 0);
	//���ò˵�
	void SetMenuStrings(CMenu *pMainMenu);

	CString m_strCurLanguage;
	CString m_szLanguagePath;	//���������ļ���·��
	CString m_szSettingPath;	//������������init�ļ�·��
	CString	m_szCurPath;		//���浱ǰ·��
protected:
	
	
private:
	CLanguageManger();
	static CLanguageManger *m_pLangManager;
};


#endif //endif(AFX_LANGUAGE_MANGER_N2345_H__FD8E1698_F985_4904_8249__INCLUDED_)