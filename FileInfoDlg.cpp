// FileInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include <boost/regex.hpp>
#include "SID_2FY.h"
#include "FileInfoDlg.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileInfoDlg dialog

extern CMySID_2FYApp theApp;

CFileInfoDlg::CFileInfoDlg(CString strInitialDir/*=_T("")*/,CString strFileType/*=_T("")*/,UINT nID_Icon/*=0*/,CWnd* pParent /*=NULL*/)
	: CDialog(CFileInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileInfoDlg)
	m_strFileHead = _T("");
	m_strFileName = _T("");
	m_strFileTail = _T("");
	m_strFileTailDis = _T("");
	m_nID_FilterIcon = nID_Icon;
	m_strFolderPath = strInitialDir;
	m_strFileType = strFileType;
	//}}AFX_DATA_INIT
}


void CFileInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileInfoDlg)
	DDX_Control(pDX, IDC_FILE_TYPE, m_ctlFileType);
	DDX_Control(pDX, IDC_FILE_LIST, m_listFileList);
	DDX_Text(pDX, IDC_FILE_HEAD, m_strFileHead);
	DDX_Text(pDX, IDC_FILE_NAME, m_strFileName);
	DDX_Text(pDX, IDC_FILE_TAIL, m_strFileTailDis);
	DDX_Text(pDX, IDC_FOLDER_PATH, m_strFolderPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CFileInfoDlg)
	ON_BN_CLICKED(IDC_SORT_FOLDER_BTN, OnSortFolderBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileInfoDlg message handlers

void CFileInfoDlg::OnSortFolderBtn() 
{
	// TODO: Add your control notification handler code here
	BROWSEINFO   bi;                         //BROWSEINFO结构体   
	TCHAR   Buffer[MAX_PATH]=_T("");   
	TCHAR   FullPath[MAX_PATH]=_T("");   
	bi.hwndOwner   =   m_hWnd;       //m_hWnd你的程序主窗口   
	bi.pidlRoot   =   NULL;   
	bi.pszDisplayName   =   Buffer;   //返回选择的目录名的缓冲区   
	bi.lpszTitle   =   _T("Selection");   //弹出的窗口的文字提示   
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS   ;   //只返回目录。其他标志看MSDN   
	bi.lpfn   =   NULL;                                                   //回调函数，有时很有用   
	bi.lParam   =   0;   
	bi.iImage   =   0;   
	ITEMIDLIST*   pidl   =   ::SHBrowseForFolder   (&bi);   //显示弹出窗口，ITEMIDLIST很重要   
	if(::SHGetPathFromIDList   (pidl,FullPath))           //在ITEMIDLIST中得到目录名的整个路径   
	{   
		//成功   
		m_strFolderPath=FullPath;
		LoadFolder();
		UpdateData(FALSE);
	}

	
}

void CFileInfoDlg::OnOK()
{
	// TODO: Add extra validation here
	UpdateData();
	boost::regex expression("^[^\\\\/:*?\"<>|]*([\\\\/:*?\"<>|]{1,})[^\\\\/:*?\"<>|]*$");
	boost::cmatch what;
	char tempFileName[255];
	WideCharToMultiByte(CP_ACP,0,m_strFileName,-1,tempFileName,255,NULL,NULL);
	CString info, title;
	title.LoadString( IDS_DLG_DEFTITLE );
	if(boost::regex_match(tempFileName, what, expression))//CString转string
	{
		info.LoadString( IDS_PRJNAME_ERROR );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	CFileFind finder;
	CString str;
	str =m_strFolderPath + m_strFileHead + m_strFileName + m_strFileTail;
	if(finder.FindFile(str))
	{
		CString strTemp;
		info.LoadString( IDS_WAVEFILE_INFO );
		strTemp.Format(info, str);
		CdMessageBox box(this, strTemp, title);
	 	box.MsgBoxEx();
		return;
	}
	else
	{
		CString strTemp;
		info.LoadString( IDS_SAVE_FILE );
		strTemp.Format(strTemp, str);
		CdMessageBox box(this, strTemp, title, MB_YESNO);
		if( IDNO == box.MsgBoxEx() )
		{
			return;
		}
	}
	CDialog::OnOK();
}

BOOL CFileInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	// TODO: Add extra initialization here
	m_imageList.Create(16,16,ILC_COLOR24|ILC_MASK,2,2);
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDER));
	if(m_nID_FilterIcon != 0)
	{
		VERIFY(m_imageList.Add(AfxGetApp()->LoadIcon(m_nID_FilterIcon)) != -1);
		ASSERT(m_strFileType != _T(""));
		m_ctlFileType.AddString(m_strFileType);
		m_ctlFileType.SelectString(0,m_strFileType);
	}
	
	m_listFileList.SetImageList(&m_imageList,LVSIL_SMALL);
	if(m_strFolderPath != _T(""))
	{
		LoadFolder();
	}
	if(m_strFileTail.GetLength() > 17)
	{
		m_strFileTailDis.Format(_T("%s..."),m_strFileTail.Left(17)); 
	}
	else
	{
		m_strFileTailDis = m_strFileTail;
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileInfoDlg::LoadFolder()
{
	m_listFileList.SetRedraw(FALSE);
	VERIFY(m_listFileList.DeleteAllItems());
	CString strFinderName;
	CString strFileTitle;
	CFileFind fileFind;
	VERIFY(fileFind.FindFile(m_strFolderPath+_T("\\*.*")));
	BOOL bNotLastFile;
	int index=-1;
	do 
	{
		bNotLastFile = fileFind.FindNextFile();
		if(!fileFind.IsDots())
		{
			index++;
			strFinderName = fileFind.GetFileName();
			strFileTitle = fileFind.GetFileTitle();
			if(fileFind.IsDirectory())
			{
				m_listFileList.InsertItem(index,strFinderName,0);
			}
			else
			{
				CString strExtend = strFinderName.Right(strFinderName.GetLength() - strFileTitle.GetLength());
				CString strFileType;
				m_ctlFileType.GetLBText(m_ctlFileType.GetCurSel(),strFileType);
				if(strExtend == strFileType)
				{
					m_listFileList.InsertItem(index,strFinderName,1);
				}
			}
		}
	} while (bNotLastFile);
	m_listFileList.SetRedraw(TRUE);
}
