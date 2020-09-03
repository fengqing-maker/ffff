// ChangeRealValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "ChangeRealValueDlg.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeRealValueDlg dialog


CChangeRealValueDlg::CChangeRealValueDlg(int nSubItem,int nChannelIndex,int nPointIndex,
										 CWareFileData * pWareFileData,CWnd* pParent/* = NULL*/)
	: CDialog(CChangeRealValueDlg::IDD, pParent),m_pWareFileData(pWareFileData),m_nChannelIndex(nChannelIndex)
	,m_nSubItem(nSubItem)
{
	//{{AFX_DATA_INIT(CChangeRealValueDlg)
	m_strChannelName = _T("");
	m_nSampleNum = nPointIndex;
	m_nNewSampleValue = 0;
	m_nOldSampleValue = 0;
	//}}AFX_DATA_INIT
}


void CChangeRealValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeRealValueDlg)
	DDX_Text(pDX, IDC_CHANNEL_NAME, m_strChannelName);
	DDX_Text(pDX, IDC_SAMPLE_NUM, m_nSampleNum);
	DDX_Text(pDX, IDC_NEW_S_VALUE, m_nNewSampleValue);
	DDX_Text(pDX, IDC_OLD_S_VALUE, m_nOldSampleValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeRealValueDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeRealValueDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeRealValueDlg message handlers

BOOL CChangeRealValueDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	short *pSrcChannel;
	pSrcChannel = (short *)(m_pWareFileData->m_arrPSubItem[m_nSubItem]->GetChannelDataAddr(m_nChannelIndex));
	m_pWareFileData->m_arrPSubItem[m_nSubItem]->GetChannelName(m_nChannelIndex,m_strChannelName);
	m_nOldSampleValue = pSrcChannel[m_nSampleNum];
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeRealValueDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	BYTE * FileBuf;
	FileBuf = new BYTE[m_pWareFileData->m_nFileLength+10];
	memcpy(FileBuf,m_pWareFileData->m_fileData,m_pWareFileData->m_nFileLength);
	short * pDesChannel,*pSrcChannel;
	pSrcChannel = (short *)(m_pWareFileData->m_arrPSubItem[m_nSubItem]->GetChannelDataAddr(m_nChannelIndex));
	pDesChannel = (short *)(FileBuf + (DWORD)pSrcChannel - (DWORD)m_pWareFileData->m_fileData);
	pDesChannel[m_nSampleNum] = pSrcChannel[m_nSampleNum] =  m_nNewSampleValue;
	SetBufCRC(FileBuf,m_pWareFileData->m_nFileLength);
	FILE  *file =  _wfopen(m_pWareFileData->m_strFilePath,_T("wb"));
	CString info;
	if(file == NULL)
	{
        info.LoadString(IDS_ERROR_OPENWAVE);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	if(m_pWareFileData->m_nFileLength != fwrite(FileBuf,sizeof(BYTE),m_pWareFileData->m_nFileLength,file))
	{
		info.LoadString(IDS_ERROR_WRITEWF);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		fclose(file);
		return;
	}
	fclose(file);
	delete [] FileBuf;
	FileBuf = NULL;
	info.LoadString(IDS_OK_WRITEVALUE);
	CdMessageBox box(this, info);
	box.MsgBoxEx();

	CDialog::OnOK();
}
void CChangeRealValueDlg::SetBufCRC(BYTE* fileBuf, int nFileLength)
{
	UINT cks2=0;
	
	int i=0,top=nFileLength-4;
	for(i=0;i<top;i++)
	{
		cks2 += fileBuf[i];
	}
	*((UINT *)(fileBuf+nFileLength-4)) = cks2;
}