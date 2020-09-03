// CurvePrintSet.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "CurvePrintSet.h"
#include "CustomizeWareSetDlg.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurvePrintSet dialog


CCurvePrintSet::CCurvePrintSet(CWareFileData *pWareFileData,BOOL bLabel/*=FALSE*/,int dotaTime,CWnd* pParent /*=NULL*/)
	: CDialog(CCurvePrintSet::IDD, pParent),m_pWareFileData(pWareFileData),m_bLabel(bLabel),
	m_dMinTime(-1),m_dMaxTime(-1),m_HPrintMinTime(40),m_VPrintMinTime(30),m_cPS('s'),m_nDotaTime(dotaTime)
{
	//{{AFX_DATA_INIT(CCurvePrintSet)
	m_dEndTime = 0.0;
	m_bDisChannelInfo = FALSE;
	m_bDisFileInfo = FALSE;
	m_bDisParaInfo = FALSE;
	m_dStartTime = 0.0;
	m_dTestTime = 0.0;
	m_strPrintChannel = _T("");
	m_bPrintPageNum = FALSE;
	//}}AFX_DATA_INIT
}


void CCurvePrintSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurvePrintSet)
	DDX_Control(pDX, IDC_COLOR_STYLE, m_ctlColorStyle);
	DDX_Control(pDX, IDC_DRAW_STYLE_COMBO, m_ctlDrawStyle);
	DDX_Text(pDX, IDC_END_TIME, m_dEndTime);
	DDV_MinMaxDouble(pDX, m_dEndTime, m_dMinTime,m_dMaxTime);
	DDX_Check(pDX, IDC_CHANNEL_INFO, m_bDisChannelInfo);
	DDX_Check(pDX, IDC_FILE_INFO, m_bDisFileInfo);
	DDX_Check(pDX, IDC_PARA_IFNO, m_bDisParaInfo);
	DDX_Text(pDX, IDC_START_TIME, m_dStartTime);
	DDV_MinMaxDouble(pDX, m_dStartTime, m_dMinTime,m_dMaxTime);
	DDX_Text(pDX, IDC_TEST_TIME, m_dTestTime);
	DDV_MinMaxDouble(pDX, m_dTestTime, m_dMinTime,m_dMaxTime);
	DDX_Text(pDX, IDC_PRINT_CHANNEL, m_strPrintChannel);
	DDX_Check(pDX, IDC_PAGE_NUM, m_bPrintPageNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurvePrintSet, CDialog)
	//{{AFX_MSG_MAP(CCurvePrintSet)
	ON_BN_CLICKED(IDC_PRINT_CHANNEL_BTN, OnPrintChannelBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurvePrintSet message handlers

void CCurvePrintSet::OnPrintChannelBtn() 
{
	// TODO: Add your control notification handler code here

	CCustomizeWareSetDlg dlg(m_pWareFileData,TRUE,NULL,0,TRUE);
	int i,nChannelType = -1,nSubItem;
	CString strTemp, info;
	if(IDOK == dlg.DoModal())
	{
		m_strPrintChannel = _T("");
		for(i=0; i<m_pWareFileData->m_nCurveCount; i++)
		{
			nSubItem = m_pWareFileData->m_pCurve[i].nSubItemIndex;
			switch(m_pWareFileData->m_arrPSubItem[nSubItem]->GetDataType())
			{
			case ANALOG_TYPE:
				if(nChannelType != ANALOG_TYPE)
				{
					info.LoadString( IDS_STR_ANALOG );
					m_strPrintChannel += info + _T(":\r\n");
					nChannelType = ANALOG_TYPE;
				}
				break;
			case DIGITAL_IN_TYPE:
				if(nChannelType != DIGITAL_IN_TYPE)
				{
					info.LoadString( IDS_DIGITAL_INPUT );
					m_strPrintChannel += info + _T(":\r\n");
					nChannelType = DIGITAL_IN_TYPE;
				}
				break;
			case DIGITAL_OUT_TYPE:
				if(nChannelType != DIGITAL_OUT_TYPE)
				{
					info.LoadString( IDS_DIGITAL_OUTPUT );
					m_strPrintChannel += info + _T(":\r\n");
					nChannelType = DIGITAL_OUT_TYPE;
				}
				break;
			default:
				break;
			}
			m_cPS = dlg.m_cPS;
			strTemp.Format(_T("%s  %s\r\n"),m_strPrintChannel,m_pWareFileData->m_pCurve[i].name);
			m_strPrintChannel = strTemp;
		}
		m_dMinTime = 0 - m_pWareFileData->m_pCurve[2].startTime;
		m_dMaxTime = m_pWareFileData->m_nCycleCount*20 - 
			m_pWareFileData->m_pCurve[2].startTime;
		GetDlgItem(IDC_END_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_TEST_TIME)->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}

extern CMySID_2FYApp theApp;
BOOL CCurvePrintSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	// TODO: Add extra initialization here
	CString str;
	str.LoadString(IDS_DRAW_WAY1);
	m_ctlDrawStyle.InsertString(0, str); //先纵向再横向
	str.LoadString(IDS_DRAW_WAY2);
	m_ctlDrawStyle.InsertString(1, str);
	m_ctlDrawStyle.SetCurSel(0);
	//
	str.LoadString( IDS_DRAW_COLOUR1 );
    m_ctlColorStyle.InsertString(0, str);
	str.LoadString( IDS_DRAW_COLOUR2 );
	m_ctlColorStyle.InsertString(1, str);
// 	m_ctlColorStyle.InsertString(0, _T("黑白打印"));
// 	m_ctlColorStyle.InsertString(1, _T("彩色打印"));
	m_ctlColorStyle.SetCurSel(0);
	m_bMultiColor = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

extern CMySID_2FYApp theApp; 
void CCurvePrintSet::OnOK() 
{
	// TODO: Add extra validation here
	CString str,strTemp;
	UpdateData();
	if (m_bLabel)
	{
		if (m_dStartTime>60800)
		{
			strTemp.LoadString( IDS_BEGINTIME_MAX );
			CdMessageBox box(theApp.m_pMainWnd, strTemp);
		    box.MsgBoxEx();
			return;
		}
		if (m_dEndTime>60800)
		{
			strTemp.LoadString( IDS_ENDTIME_MIN );
			CdMessageBox box(theApp.m_pMainWnd, strTemp);
		    box.MsgBoxEx();
			return;
		}
	}
    int selId = m_ctlDrawStyle.GetCurSel();
	m_ctlDrawStyle.GetLBText(selId, str);
//	m_bHPrint = str == _T("横向绘制") ? TRUE : FALSE;
	m_bHPrint = (1 == selId ? TRUE : FALSE);
	//m_ctlColorStyle.GetLBText(m_ctlColorStyle.GetCurSel(),str);
	//m_bMultiColor = str == _T("彩色打印") ? TRUE : FALSE;
	selId = m_ctlColorStyle.GetCurSel();
	m_bMultiColor = (1 == selId ? TRUE : FALSE);
	double dMinPrint = m_bHPrint ? m_HPrintMinTime : m_VPrintMinTime;
/*	str = m_bHPrint ? _T("横向打印") : _T("纵向打印");*/
    
	if(m_dEndTime - m_dStartTime < dMinPrint)
	{
		if (SUBLANG_CHINESE_SIMPLIFIED == theApp.m_pLangManger->m_strCurLanguage)
		{
			strTemp.Format(_T("%s时至少要打印%lfms时间长的曲线！"),str,dMinPrint);
		}
		else if (IDS_STR_ENGLISH == theApp.m_pLangManger->m_strCurLanguage)
		{
	        strTemp.Format(_T("%s print %lfms long at least！"),str,dMinPrint);
		}
		CdMessageBox box(this, strTemp);
		box.MsgBoxEx();
		return;
	}
	if(m_dTestTime < m_dStartTime || m_dTestTime > m_dEndTime)
	{
		str.LoadString(IDS_DRAW_START);
		CdMessageBox box(this, str);
		box.MsgBoxEx();
		return;
	}
	CDialog::OnOK();
}
