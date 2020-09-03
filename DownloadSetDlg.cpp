// DownloadSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "DownloadSetDlg.h"
#include "InfoDlg.h"
#include "FileInfoDlg.h"
#include "Frame.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CMySID_2FYApp theApp;
UINT dlgRe;
CEvent selEvent;



CString fileName;
extern LANGID SetThreadUILanguage(LANGID lcid);

/////////////////////////////////////////////////////////////////////////////
// CDownloadSetDlg dialog

CDownloadSetDlg::CDownloadSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownloadSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownloadSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_fileBuf = new BYTE[MAX_WARE_FILE_LENGTH];
}
CDownloadSetDlg::~CDownloadSetDlg( )
{
   delete [] m_fileBuf;
    m_fileBuf = NULL;
}

void CDownloadSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloadSetDlg)
	DDX_Control(pDX, IDC_WARE_FILE_LIST, m_wareFileList);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownloadSetDlg, CDialog)
	//{{AFX_MSG_MAP(CDownloadSetDlg)
	ON_MESSAGE(WM_DOWNLOAD_FINISH,OnDownloadFinish)
	ON_MESSAGE(WM_FILE_SAVE_AS,OnFileSaveAs)
	ON_MESSAGE(WM_FILE_INFO,OnFileInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloadSetDlg message handlers

BOOL CDownloadSetDlg::OnInitDialog() 
{
	if(!CDialog::OnInitDialog())
	{
		return FALSE;
	}
	// TODO: Add extra initialization here
	//	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	// XListCtrl must have LVS_EX_FULLROWSELECT if combo boxes are used
	m_wareFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT |LVS_EX_GRIDLINES| LVS_EX_TRACKSELECT);

	// call EnableToolTips to enable tooltip display
	m_wareFileList.EnableToolTips(TRUE);
	CString info;
	info.LoadString( IDS_HAS_NONFILE );
	m_wareFileList.m_strNoItemText = info;
	InitListCtrl(m_wareFileList);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// 
void CDownloadSetDlg::InitListCtrl(CXListCtrl &list)
{
	CRect rect;
	list.GetWindowRect(&rect);

	int w = rect.Width() - 2;
	int colwidths[5] = { 10, 10, 25, 30, 25};	// 宽度的百分比
	TCHAR ** lpszHeaders = NULL;
	TCHAR *	lpszHeaders_ch[] = { _T("提取"),
		_T("序号"),
		_T("启动类型"),
		_T("启动时间"),
		_T("完成进度"),
		NULL };
	TCHAR *	lpszHeaders_en[] = { _T("down"),
		_T("serial"),
		_T("type"),
		_T("time"),
		_T("downing rate"),
		NULL };
	lpszHeaders = lpszHeaders_ch;
	if (IDS_STR_ENGLISH == theApp.m_pLangManger->m_strCurLanguage )
	{
		lpszHeaders = lpszHeaders_en;
	}

	int i;
	int total_cx = 0;
	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	// add columns
	for (i = 0; ; i++)
	{
		if (lpszHeaders[i] == NULL)
			break;
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = (i == 0) ? LVCFMT_LEFT : LVCFMT_CENTER;
		lvcolumn.pszText = lpszHeaders[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = (lpszHeaders[i+1] == NULL) ? w - total_cx - 2 : (w * colwidths[i]) / 100;
		total_cx += lvcolumn.cx;
		list.InsertColumn(i, &lvcolumn);
	}


	// create the image list from bitmap resource
	VERIFY(list.m_cImageList.Create(IDB_CHECKBOXES, 16, 3, RGB(255, 0, 255)));
	list.m_HeaderCtrl.SetImageList(&list.m_cImageList);

	// iterate through header items and attach the image list
	HDITEM hditem;

	for (i = 0; i < list.m_HeaderCtrl.GetItemCount(); i++)
	{
		hditem.mask = HDI_IMAGE | HDI_FORMAT;
		list.m_HeaderCtrl.GetItem(i, &hditem);
		hditem.fmt |=  HDF_IMAGE;
		if (i == 0)
			hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
		else
			hditem.iImage = XHEADERCTRL_NO_IMAGE;

		list.m_HeaderCtrl.SetItem(i, &hditem);
	}

	memset(&lvcolumn, 0, sizeof(lvcolumn));

	// set the format again - must do this twice or first column does not get set
	for (i = 0; ; i++)
	{
		if (lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM;
		lvcolumn.fmt = (i == 0) ? LVCFMT_LEFT : LVCFMT_CENTER;
		lvcolumn.iSubItem = i;
		list.SetColumn(i, &lvcolumn);
	}
	//服务器的指定发送端端口
	ServerFrame serverFrame;
	memset(&serverFrame,0,sizeof(serverFrame));
	serverFrame.nServerType=SERVER_WARE;
	serverFrame.nFlag=QUERY_WARE_SUMMARY;
	if(CFrame::SendAndRecvFrame(&serverFrame))
	{
		CString info;
		info.LoadString( IDS_DOWNFILE_ERROR1 );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		CDialog::OnOK();
		return;
	}
	m_nWareHead = 0;
	CFrame::GetAllWareHead(m_arrWareHead,m_nWareHead); 
	CString str;
	if(!IsWindow(m_wareFileList.m_hWnd))
	{
		AfxMessageBox(_T("尚未创建"));
	}
	for(i=0;i<m_nWareHead;i++)
	{
		if(m_arrWareHead[i].strWareStartType[0]==0)
		{
			continue;
		}
		str.Format(_T("%d"),i);
		m_wareFileList.InsertItem(i,_T(""));
		m_wareFileList.SetItemText(i,1,str);
		m_wareFileList.SetItemText(i,2,(CString)m_arrWareHead[i].strWareStartType);
		str.Format(_T("%s.%03d"),m_arrWareHead[i].happenTime.Format(_T("%Y-%m-%d %H:%M:%S")),m_arrWareHead[i].milliSecond);
		m_wareFileList.SetItemText(i,3,str);
		m_wareFileList.SetItemText(i, 4, str);
		m_wareFileList.SetProgress(i,4);
		m_wareFileList.SetCheckbox(i,0,0);
	}//of for
}


void CDownloadSetDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	TerminateThread(hThread,0);
	CDialog::OnCancel();
}

LRESULT CDownloadSetDlg::OnFileInfo(WPARAM wParam, LPARAM lParam)
{
	CString titleStr, info;
	titleStr.LoadString( IDS_DLG_DEFTITLE );
	info.LoadString( IDS_STR_COVER );
	CInfoDlg dlg( titleStr, info );
	dlgRe = dlg.DoModal();
	selEvent.SetEvent();
	return 0;
}

LRESULT CDownloadSetDlg::OnFileSaveAs(WPARAM wParam, LPARAM lParam)
{
	CFileInfoDlg fDlg(theApp.m_projectData.m_strWareFilePath,_T(".wave"),IDI_WARE_FILE);
	fDlg.m_strFileHead = m_arrWareHead[wParam].strWareStartType;
	fDlg.m_strFileTail.Format(_T("(%04d%02d%02d-%s-%03d).wave"),
		m_arrWareHead[wParam].happenTime.GetYear(),
		m_arrWareHead[wParam].happenTime.GetMonth(),m_arrWareHead[wParam].happenTime.GetDay(),
		m_arrWareHead[wParam].happenTime.Format(_T("%H%M%S")),m_arrWareHead[wParam].milliSecond);
	
	dlgRe=fDlg.DoModal();
	if(IDOK == dlgRe)
	{
		fileName = fDlg.m_strFolderPath + fDlg.m_strFileHead + fDlg.m_strFileName +
			fDlg.m_strFileTail;
	}
	selEvent.SetEvent();
	return 0;
}


LRESULT CDownloadSetDlg::OnDownloadFinish(WPARAM wParam,LPARAM lParam)
{
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	CDialog::OnOK();
	return 0;
}

DWORD WINAPI DownWaveFileThread(LPVOID lpParameter)
{
	CDownloadSetDlg* downFileDlg=(CDownloadSetDlg*)lpParameter;


	DWORD LangId = GetUserDefaultLangID();
	if (IDS_STR_ENGLISH == theApp.m_pLangManger->m_strCurLanguage && MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) != LangId )
	{
		SetThreadUILanguage( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );  //win 7 下用
		//	SetThreadLocale( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );  //xp 下可以用此值
		//SetLanguageID(SUBLANG_ENGLISH_US);
	}else if (SUBLANG_CHINESE_SIMPLIFIED == theApp.m_pLangManger->m_strCurLanguage && MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED) != LangId)
	{
		SetThreadUILanguage( MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED) );  //win 7 下用

		//	SetThreadLocale( MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );
	}

	//rView->LoadData(TRUE);
	UINT fileLength;
	int i,k;
	BOOL bIsNewWareFile;
	CString str, title, info;
	for(i=downFileDlg->m_nWareHead-1;i>=0;i--)
	{
		int index=theApp.m_projectData.m_nFileCount;
		if(index >= MAX_FILE_COUNT)
		{
			info.LoadString( IDS_MAXDOWN_FILENUM );
			str.Format(info, MAX_FILE_COUNT, index);
			CdMessageBox box(downFileDlg, str);
			box.MsgBoxEx();
			break;
		}
		bIsNewWareFile=TRUE;
		if(downFileDlg->m_wareFileList.GetCheckbox(i,0)==1)
		{
			memset(downFileDlg->m_fileBuf,0,sizeof(downFileDlg->m_fileBuf));
			bool writeFileSec=true;
			int newProgress=0;
			ServerFrame serverFrame;
			memset(&serverFrame,0,sizeof(serverFrame));
			serverFrame.nServerType=SERVER_WARE;
			serverFrame.nFlag=QUERY_WARE_DATA;
			serverFrame.queryFrame.wareData.nPieceNum=0;
			memcpy(serverFrame.queryFrame.wareData.strStartType,downFileDlg->m_arrWareHead[i].strWareStartType,
				sizeof(downFileDlg->m_arrWareHead[i].strWareStartType));
			CFrame::GetDateToBuf(serverFrame.queryFrame.wareData.happenTime,downFileDlg->m_arrWareHead[i].happenTime,
				downFileDlg->m_arrWareHead[i].milliSecond);
			do
			{
				if (newProgress>90)
				{
					Sleep(1);
				}
				newProgress=CFrame::GetBlock(downFileDlg->m_fileBuf,fileLength,&serverFrame);
				if(newProgress < 0 || newProgress > 100)
				{
					CString info;
					info.LoadString( IDS_DOWNFILE_ERROR2 );
					CdMessageBox box(downFileDlg, info);
					box.MsgBoxEx();
					break;
				}
				downFileDlg->m_wareFileList.UpdateProgress(i,4,newProgress);
			} while(newProgress != 100);
			if(newProgress != 100)
			{
				downFileDlg->m_wareFileList.DeleteProgress(i,4);
				info.LoadString( IDS_DOWN_FILE_ERROR3 );
				downFileDlg->m_wareFileList.SetItemText(i,4, info);
				Sleep(FILE_INTERVAL_TIME);
				continue;
			}
			if(!CFrame::IsValidFile(downFileDlg->m_fileBuf,fileLength))
			{
				downFileDlg->m_wareFileList.DeleteProgress(i,4);
				info.LoadString( IDS_FILE_BROKEN2 );
				downFileDlg->m_wareFileList.SetItemText(i,4, info);
				Sleep(FILE_INTERVAL_TIME);
				continue;
			}
			fileName.Format(_T("%s%s(%04d%02d%02d-%s-%03d).wave"),theApp.m_projectData.m_strWareFilePath,
				(CString)downFileDlg->m_arrWareHead[i].strWareStartType,downFileDlg->m_arrWareHead[i].happenTime.GetYear(),
				downFileDlg->m_arrWareHead[i].happenTime.GetMonth(),downFileDlg->m_arrWareHead[i].happenTime.GetDay(),
				downFileDlg->m_arrWareHead[i].happenTime.Format(_T("%H%M%S")),downFileDlg->m_arrWareHead[i].milliSecond);

			bIsNewWareFile = TRUE;
			CString strTemp,FileName;

			CFileFind fileFind;
			CString   tempStr;
			if(fileFind.FindFile(fileName))
			{
				CWnd *pt=(CDownloadSetDlg*)CWnd::FromHandle(downFileDlg->m_hWnd);
				pt->PostMessage(WM_FILE_INFO,(WPARAM)0,(LPARAM)0);
				selEvent.ResetEvent();
				WaitForSingleObject(selEvent.m_hObject,INFINITE);
				switch (dlgRe)
				{
				case IDYES://OK
					VERIFY(DeleteFile(fileName));
					FileName = fileName.Right(fileName.GetLength() - fileName.ReverseFind(_T('\\'))-1);
					index=theApp.m_projectData.m_nFileCount;
					for (k=0; k<index ;k++)
					{
						strTemp=theApp.m_projectData.m_arrfileTreeNode[k].strFileName;
						if (FileName == strTemp)
						{
							bIsNewWareFile = FALSE;
							break;
						}
					}
					//bIsNewWareFile = FALSE;
					break;
				case IDNO://另存
					{
						pt->PostMessage(WM_FILE_SAVE_AS,(WPARAM)i,(LPARAM)0);
						selEvent.ResetEvent();
						WaitForSingleObject(selEvent.m_hObject,INFINITE);
						if (dlgRe!=IDOK)
						{
							downFileDlg->m_wareFileList.DeleteProgress(i,4);
							tempStr.LoadString( IDS_CANCEL_SAVE );
							downFileDlg->m_wareFileList.SetItemText(i,4, tempStr);
							Sleep(FILE_INTERVAL_TIME);
							continue;
						}
						break;
					}
				case IDCANCEL:
					downFileDlg->m_wareFileList.DeleteProgress(i,4);
					tempStr.LoadString( IDS_CANCEL_SAVE );
					downFileDlg->m_wareFileList.SetItemText(i,4, tempStr);
					Sleep(FILE_INTERVAL_TIME);
					continue;
				}
			}
			FILE * wareFile=_wfopen(fileName,_T("wb"));
			if(wareFile==NULL)
			{
				downFileDlg->m_wareFileList.DeleteProgress(i,4);
				tempStr.LoadString( IDS_CREATE_WAVE_ERROR );
				downFileDlg->m_wareFileList.SetItemText(i,4, tempStr );
				fclose(wareFile);
				Sleep(FILE_INTERVAL_TIME);
				continue;
			}
			if(fileLength != fwrite(downFileDlg->m_fileBuf,sizeof(BYTE),fileLength,wareFile))
			{
				downFileDlg->m_wareFileList.DeleteProgress(i,4);
				tempStr.LoadString( IDS_WRITEEFILE_ERROR );
				downFileDlg->m_wareFileList.SetItemText(i,4,tempStr);
				fclose(wareFile);
				Sleep(FILE_INTERVAL_TIME);
				continue;
			}
			fclose(wareFile);
			if(bIsNewWareFile)
			{
				index=theApp.m_projectData.m_nFileCount;
				theApp.m_projectData.m_arrfileTreeNode[index].dtStartTime=downFileDlg->m_arrWareHead[i].happenTime;
				theApp.m_projectData.m_arrfileTreeNode[index].nMillisecond=downFileDlg->m_arrWareHead[i].milliSecond;
				strcpy(theApp.m_projectData.m_arrfileTreeNode[index].strWareType,
					downFileDlg->m_arrWareHead[i].strWareStartType);
				fileName = fileName.Right(fileName.GetLength() - fileName.ReverseFind('\\')-1);
				WideCharToMultiByte(CP_ACP,0,fileName,-1,
					theApp.m_projectData.m_arrfileTreeNode[index].strFileName,MAX_FILE_NAME_LENGTH,NULL,NULL);
				theApp.m_projectData.SaveTreeNodeToProject(index);
			}
			Sleep(FILE_INTERVAL_TIME);
		}
	}
	info.LoadString( IDS_DOWNFILE_INFO1 );
	CdMessageBox box(downFileDlg, info);
	box.MsgBoxEx();
	PostMessage(downFileDlg->m_hWnd,WM_DOWNLOAD_FINISH,NULL,NULL);
	return 0;
}

void CDownloadSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	hThread = CreateThread( NULL , 0 , DownWaveFileThread , this , 0,  NULL) ;

	if(!::SetThreadPriority(hThread,THREAD_PRIORITY_HIGHEST))
	{
		CString str;
		str.LoadString( IDS_SET_PRI_ERROR );
		CdMessageBox box( this, str);
	    box.MsgBoxEx();
	}

}