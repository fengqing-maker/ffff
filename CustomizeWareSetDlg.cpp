// CustomizeWareSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include <map>
using namespace std;
#include "SID_2FY.h"
#include "CustomizeWareSetDlg.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomizeWareSetDlg dialog

extern CMySID_2FYApp theApp;
//实现构造函数 对参数进行赋值
CCustomizeWareSetDlg::CCustomizeWareSetDlg(CWareFileData * pWareFileData,BOOL bShowDialog/*=FALSE*/,
										   Curve *pCurve/*=NULL*/,int nCurveCount/*=0*/,BOOL bPrint/*=FALSE*/,
										   CWnd* pParent/* = NULL*/)
	: CDialog(CCustomizeWareSetDlg::IDD, pParent),m_bShowDialog(bShowDialog),m_pCurve(pCurve),m_nCurveCount(nCurveCount),
	m_bPrint(bPrint)
{
	//{{AFX_DATA_INIT(CCustomizeWareSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nAnalogShowCount=0;
	m_nDigitalInShowCount=0;
	m_nDigitalOutShowCount=0;
	m_nFrequencyShowCount=0;
	m_nAngleShowCount=0;
	m_pWareFileData = pWareFileData;
	m_cPS='s';
}

//对话框与控件之间的数据传递
void CCustomizeWareSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomizeWareSetDlg)
	//1次值还是2次值
	DDX_Control(pDX, IDC_VALUE_TYPE, m_comBoxValueType);
	//文件中的信息通道
	DDX_Control(pDX, IDC_FILE_DATA, m_fileTree);
	//被选中的信息通道
	DDX_Control(pDX, IDC_DISPLAY_DATA, m_displayTree);
	//}}AFX_DATA_MAP
}

//监听按钮的点击
BEGIN_MESSAGE_MAP(CCustomizeWareSetDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomizeWareSetDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_DATA, OnDblclkFileData)
	ON_NOTIFY(NM_DBLCLK, IDC_DISPLAY_DATA, OnDblclkDisplayData)
	ON_BN_CLICKED(IDC_ADD_BTN, OnAddBtn)
	ON_BN_CLICKED(IDC_SUB_BTN, OnSubBtn)
	ON_BN_CLICKED(IDC_APPOINT_ADD_BTN, OnAppointAddBtn)
	ON_BN_CLICKED(IDC_MOVE_UP_BTN, OnMoveUpBtn)
	ON_BN_CLICKED(IDC_MOVE_DOWN_BTN, OnMoveDownBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomizeWareSetDlg message handlers

void CCustomizeWareSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	theApp.m_projectData.m_dwFrequencyDef = theApp.m_projectData.m_dwAngleDiffDef = theApp.m_projectData.m_dwDigtalInDef
		= theApp.m_projectData.m_dwDigtalOutDef =theApp.m_projectData.m_dwAnalogDef =0;
	long indexOfps;
	BOOL bReGetCurve;
	//获取节点的索引值
	indexOfps=m_comBoxValueType.GetCurSel();
	CString info;
	//如果一项没有选择
	if(indexOfps== CB_ERR)
	{
		info.LoadString(IDS_SEL_VALUETYPE);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	else if(indexOfps==0)
	{
		m_cPS='p';
	}
	else
	{
		m_cPS='s';
	}
	if(m_nDigitalInShowCount==0 && m_nAnalogShowCount==0 && m_nDigitalOutShowCount == 0
		&& m_nFrequencyShowCount == 0 && m_nAngleShowCount == 0) 
	{
        info.LoadString(IDS_SEL_CHANNAL);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	if(m_nAnalogShowCount !=2 || m_nFrequencyShowCount != 2 ) 
	{
		info.LoadString( IDS_SUBITEM_SELE_ERROR );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	CCurveView::Release(m_pWareFileData->m_pCurve,m_pWareFileData->m_nCurveCount);
	int count,index,curveIndex,subItemIndex;
	CString str;
	m_pWareFileData->m_nCurveCount = m_nDigitalInShowCount+m_nAnalogShowCount+m_nDigitalOutShowCount+m_nFrequencyShowCount+m_nAngleShowCount;
	m_pWareFileData->m_pCurve = new Curve[m_pWareFileData->m_nCurveCount];
	ASSERT(m_pWareFileData->m_pCurve!=NULL);
	count=0;
	HTREEITEM hAnalogChannel=m_displayTree.GetChildItem(m_displayTree.GetRootItem());
	HTREEITEM hDigitalInChannel=m_displayTree.GetChildItem(
		m_displayTree.GetNextSiblingItem(m_displayTree.GetRootItem()));
	HTREEITEM hDigitalOutChannel=m_displayTree.GetChildItem(
		m_displayTree.GetNextSiblingItem(m_displayTree.GetNextSiblingItem(m_displayTree.GetRootItem())));
	HTREEITEM hFreqChannel=m_displayTree.GetChildItem(
		m_displayTree.GetNextSiblingItem(m_displayTree.GetNextSiblingItem(m_displayTree.GetNextSiblingItem(m_displayTree.GetRootItem()))));
	HTREEITEM hAngleChannel=m_displayTree.GetChildItem(
		m_displayTree.GetNextSiblingItem(m_displayTree.GetNextSiblingItem(m_displayTree.GetNextSiblingItem(m_displayTree.GetNextSiblingItem(m_displayTree.GetRootItem())))));
	curveIndex=-1;
	info.LoadString(IDS_GET_WAVEF);

	if(m_nAngleShowCount>0)
	{
		//有主控插件相角差
		do 
		{
			str=m_displayTree.GetItemText(hAngleChannel);
			m_displayTree.m_tempItem = m_displayTree.m_mapTreeItem.find(str);
			ASSERT(m_displayTree.m_tempItem != m_displayTree.m_mapTreeItem.end());
			subItemIndex = m_displayTree.m_tempItem->second >>16;						/*获取子项号*/
			index = m_displayTree.m_tempItem->second & 0x0000FFFF;						/*获取通道序号*/
			theApp.m_projectData.m_dwAngleDiffDef |= 0x00000001 << index;
			curveIndex++;
			bReGetCurve = m_pWareFileData->GetCurve(curveIndex,subItemIndex,index,m_cPS);
			if(!bReGetCurve)
			{
				CdMessageBox box(this, info);
				box.MsgBoxEx();

				goto MarkEndDialog;
			}
			hAngleChannel = m_displayTree.GetNextSiblingItem(hAngleChannel);
		} while (hAngleChannel!=NULL);
	}
	if(m_nAnalogShowCount>0)
	{//模拟量通道取采样值
		do 
		{
			str=m_displayTree.GetItemText(hAnalogChannel);
			m_displayTree.m_tempItem = m_displayTree.m_mapTreeItem.find(str);
			ASSERT(m_displayTree.m_tempItem != m_displayTree.m_mapTreeItem.end());
			subItemIndex = m_displayTree.m_tempItem->second >>16;
			index = m_displayTree.m_tempItem->second & 0x0000FFFF;
			theApp.m_projectData.m_dwAnalogDef |= 0x00000001 << index;
			curveIndex++;
			bReGetCurve = m_pWareFileData->GetCurve(curveIndex,subItemIndex,index,m_cPS);
			if(!bReGetCurve)
			{
				CdMessageBox box(this, info);
		        box.MsgBoxEx();
				goto MarkEndDialog;
			}
			hAnalogChannel = m_displayTree.GetNextSiblingItem(hAnalogChannel);
		} while (hAnalogChannel!=NULL);
	}
	if(m_nDigitalInShowCount>0)
	{//开入取采样点
		do 
		{
			str=m_displayTree.GetItemText(hDigitalInChannel);
			m_displayTree.m_tempItem = m_displayTree.m_mapTreeItem.find(str);
			ASSERT(m_displayTree.m_tempItem != m_displayTree.m_mapTreeItem.end());
			subItemIndex = m_displayTree.m_tempItem->second >>16;
			index = m_displayTree.m_tempItem->second & 0x0000FFFF;
			theApp.m_projectData.m_dwDigtalInDef |= 0x00000001 << index;
			curveIndex++;
			bReGetCurve = m_pWareFileData->GetCurve(curveIndex,subItemIndex,index,m_cPS);
			if(!bReGetCurve)
			{
				  CdMessageBox box(this, info);
		          box.MsgBoxEx();
				goto MarkEndDialog;
			}
			hDigitalInChannel = m_displayTree.GetNextSiblingItem(hDigitalInChannel);
		} while (hDigitalInChannel!=NULL);
	}
	if(m_nDigitalOutShowCount>0)
	{//开出取采样点
		do 
		{
			str=m_displayTree.GetItemText(hDigitalOutChannel);
			m_displayTree.m_tempItem = m_displayTree.m_mapTreeItem.find(str);
			ASSERT(m_displayTree.m_tempItem != m_displayTree.m_mapTreeItem.end());
			subItemIndex = m_displayTree.m_tempItem->second >>16;
			index = m_displayTree.m_tempItem->second & 0x0000FFFF;
			theApp.m_projectData.m_dwDigtalOutDef |= 0x00000001 << index;
			curveIndex++;
			bReGetCurve = m_pWareFileData->GetCurve(curveIndex,subItemIndex,index,m_cPS);
			if(!bReGetCurve)
			{
				CdMessageBox box(this, info);
		        box.MsgBoxEx();
				goto MarkEndDialog;
			}
			hDigitalOutChannel = m_displayTree.GetNextSiblingItem(hDigitalOutChannel);
		} while (hDigitalOutChannel!=NULL);
	}
	if(m_nFrequencyShowCount>0)
	{//频率
		theApp.m_freqIndex = 0;
		do 
		{
			str=m_displayTree.GetItemText(hFreqChannel);
			m_displayTree.m_tempItem = m_displayTree.m_mapTreeItem.find(str);
			ASSERT(m_displayTree.m_tempItem != m_displayTree.m_mapTreeItem.end());
			subItemIndex = m_displayTree.m_tempItem->second >>16;
			index = m_displayTree.m_tempItem->second & 0x0000FFFF;
			theApp.m_projectData.m_dwFrequencyDef |= 0x00000001 << index;
			curveIndex++;
			bReGetCurve = m_pWareFileData->GetCurve(curveIndex,subItemIndex,index,m_cPS);
			if(!bReGetCurve)
			{
				CdMessageBox box(this, info);
				box.MsgBoxEx();
				goto MarkEndDialog;
			}
			hFreqChannel = m_displayTree.GetNextSiblingItem(hFreqChannel);
		} while (hFreqChannel!=NULL);
	}
	if(!theApp.m_projectData.SaveProjectSetting())
	{
		info.LoadString( IDS_WAVESET_INFO );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
	}
MarkEndDialog:CDialog::OnOK();
}

BOOL CCustomizeWareSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	// TODO: Add extra initialization here
	CRect rc;
	UINT data;
// 	m_comBoxValueType.AddString(_T("一次值"));
// 	m_comBoxValueType.AddString(_T("二次值"));
	CString info;
	info.LoadString( IDS_ONE_VALUE );
    m_comBoxValueType.InsertString( 0, info);
	info.LoadString( IDS_TWICE_VALUE );
	m_comBoxValueType.InsertString(1, info);
	m_comBoxValueType.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_comBoxValueType.MoveWindow(rc.left,rc.top,rc.Width(),100);
	//m_comBoxValueType.SelectString(0,_T("二次值"));
	m_comBoxValueType.SetCurSel( 1 );

	map<UINT,CString> mapTemp;
    info.LoadString( IDS_STR_ANALOG );
	HTREEITEM hAnalog = m_fileTree.InsertItem( info, TVI_ROOT );
	info.LoadString( IDS_DIGITAL_INPUT );
	HTREEITEM hDigitalIn=m_fileTree.InsertItem( info, TVI_ROOT, hAnalog );
	info.LoadString( IDS_DIGITAL_OUTPUT );
	HTREEITEM hDigitalOut=m_fileTree.InsertItem( info,TVI_ROOT,hDigitalIn );
	info.LoadString( IDS_FREQUENCY );
	HTREEITEM hFrequency=m_fileTree.InsertItem( info,TVI_ROOT,hDigitalOut );
    info.LoadString( IDS_ANG_DIFF );
	HTREEITEM hAngle=m_fileTree.InsertItem( info,TVI_ROOT,hFrequency );
	CString str,strTemp;
	int i,j,count,indexOfAnalog=0,indexOfDI=0,indexOfDO=0,indexOfFreq=0,indexOfAngle=0;
	info.LoadString(IDS_NAME_DIFF);
	for(i=0;i<m_pWareFileData->m_nSubItemCount;i++)
	{
		switch(m_pWareFileData->m_arrPSubItem[i]->GetDataType())
		{
		case ANALOG_TYPE:
			count = m_pWareFileData->m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				VERIFY(m_pWareFileData->m_arrPSubItem[i]->GetChannelName(j,strTemp));
				str.Format(_T("%02d:%s"),indexOfAnalog,strTemp);
				data = i;
				data <<= 16;
				data |= j;
				if(m_fileTree.m_mapTreeItem.find(str)==m_fileTree.m_mapTreeItem.end())
				{
					m_fileTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				if(m_displayTree.m_mapTreeItem.find(str)==m_displayTree.m_mapTreeItem.end())
				{
					m_displayTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				m_fileTree.InsertItem(str,hAnalog,TVI_LAST);
				indexOfAnalog++;
			}
			break;
		case DIGITAL_IN_TYPE:
			count = m_pWareFileData->m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				VERIFY(m_pWareFileData->m_arrPSubItem[i]->GetChannelName(j,strTemp));
				str.Format(_T("%02d:%s"),indexOfDI+j,strTemp);
				data = i;
				data <<= 16;
				data |= j;
				if(m_fileTree.m_mapTreeItem.find(str)==m_fileTree.m_mapTreeItem.end())
				{
					m_fileTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
	              	box.MsgBoxEx();
					return FALSE;
				}
				if(m_displayTree.m_mapTreeItem.find(str)==m_displayTree.m_mapTreeItem.end())
				{
					m_displayTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				m_fileTree.InsertItem(str,hDigitalIn,TVI_LAST);
			}
			indexOfDI += count;
			break;
		case DIGITAL_OUT_TYPE:
			count = m_pWareFileData->m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				VERIFY(m_pWareFileData->m_arrPSubItem[i]->GetChannelName(j,strTemp));
				str.Format(_T("%02d:%s"),indexOfDO+j,strTemp);
				UINT data;
				data = i;
				data <<= 16;
				data |= j;
				if(m_fileTree.m_mapTreeItem.find(str)==m_fileTree.m_mapTreeItem.end())
				{
					m_fileTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				if(m_displayTree.m_mapTreeItem.find(str)==m_displayTree.m_mapTreeItem.end())
				{
					m_displayTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				m_fileTree.InsertItem(str,hDigitalOut,TVI_LAST);
			}
			indexOfDO += count;
			break;
		case FREQUENCY_TYPE:
			count = m_pWareFileData->m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				VERIFY(m_pWareFileData->m_arrPSubItem[i]->GetChannelName(j,strTemp));
				str.Format(_T("%02d:%s"),indexOfFreq+j,strTemp);
				UINT data;
				data = i;
				data <<= 16;
				data |= j;
				if(m_fileTree.m_mapTreeItem.find(str)==m_fileTree.m_mapTreeItem.end())
				{
					m_fileTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				if(m_displayTree.m_mapTreeItem.find(str)==m_displayTree.m_mapTreeItem.end())
				{
					m_displayTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				m_fileTree.InsertItem(str,hFrequency,TVI_LAST);
			}
			indexOfFreq += count;
			break;
		case ANGLE_DIFF_TYPE:
			count = m_pWareFileData->m_arrPSubItem[i]->GetChannelCount();
			for(j=0;j<count;j++)
			{
				VERIFY(m_pWareFileData->m_arrPSubItem[i]->GetChannelName(j,strTemp));
				str.Format(_T("%02d:%s"),indexOfAngle+j,strTemp);
				UINT data;
				data = i;
				data <<= 16;
				data |= j;
				if(m_fileTree.m_mapTreeItem.find(str)==m_fileTree.m_mapTreeItem.end())
				{
					m_fileTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				if(m_displayTree.m_mapTreeItem.find(str)==m_displayTree.m_mapTreeItem.end())
				{
					m_displayTree.m_mapTreeItem.insert(map <CString,UINT>::value_type(str,data));
					mapTemp.insert(map<UINT,CString>::value_type(data,str));
				}
				else
				{
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
					return FALSE;
				}
				m_fileTree.InsertItem(str,hAngle,TVI_LAST);
			}
			indexOfAngle += count;
			break;
		}
	}
	info.LoadString( IDS_STR_ANALOG );
	
	hAnalog = m_displayTree.InsertItem( info, TVI_ROOT);
	info.LoadString( IDS_DIGITAL_INPUT );
	hDigitalIn=m_displayTree.InsertItem( info, TVI_ROOT, hAnalog);
	info.LoadString( IDS_DIGITAL_OUTPUT );
	hDigitalOut=m_displayTree.InsertItem( info, TVI_ROOT, hDigitalIn);
	info.LoadString( IDS_FREQUENCY );
	hFrequency=m_displayTree.InsertItem( info,TVI_ROOT,hDigitalOut);
    info.LoadString( IDS_ANG_DIFF );
	hAngle=m_displayTree.InsertItem( info,TVI_ROOT,hFrequency);
	m_displayTree.Expand(hAnalog,TVE_EXPAND);
	m_displayTree.Expand(hDigitalIn,TVE_EXPAND);
	m_displayTree.Expand(hDigitalOut,TVE_EXPAND);
	m_displayTree.SetWareFileData(m_pWareFileData);
	m_fileTree.SetWareFileData(m_pWareFileData);
	if(m_pCurve != NULL)
	{
		AddCurveToDisplay(mapTemp);
	}
	OnAppointAddBtn();
	if(m_bPrint)
	{
		info.LoadString( IDS_PRINT_TITLE );
		SetWindowText( info );
		info.LoadString( IDS_PRINT_CHANNELIFNO );
		GetDlgItem(IDC_DIS_CHANNEL_INFO)->SetWindowText( info );
		info.LoadString( IDS_SET );
		GetDlgItem(IDOK)->SetWindowText( info );
	}
	UpdateData(FALSE);
	if (m_bShowDialog == FALSE)
	{
		OnOK();
	}
 	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomizeWareSetDlg::OnDblclkFileData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelectItem = m_fileTree.GetSelectedItem();
	CRect rcItem;
	CPoint ptPoint;
	m_fileTree.GetItemRect(hSelectItem,&rcItem,FALSE);
	::GetCursorPos(&ptPoint);
	m_fileTree.ScreenToClient(&ptPoint);
	if(!rcItem.PtInRect(ptPoint))
	{
		return;
	}

	MoveTreeItem(m_fileTree,m_displayTree);
	*pResult = 1;
}

void CCustomizeWareSetDlg::OnDblclkDisplayData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelectItem = m_displayTree.GetSelectedItem();
	CRect rcItem;
	CPoint ptPoint;
	m_displayTree.GetItemRect(hSelectItem,&rcItem,FALSE);
	::GetCursorPos(&ptPoint);
	m_displayTree.ScreenToClient(&ptPoint);
	if(!rcItem.PtInRect(ptPoint))
	{
		return;
	}
	MoveTreeItem(m_displayTree,m_fileTree,FALSE);
	
	*pResult = 1;
}

void CCustomizeWareSetDlg::OnAddBtn() 
{
	// TODO: Add your control notification handler code here
	LRESULT re=0;
	if(m_fileTree.GetSelectedItem() == NULL)
	{
		CString info;
		info.LoadString(IDS_SEL_CHANNAL);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	MoveTreeItem(m_fileTree,m_displayTree);
}

void CCustomizeWareSetDlg::OnSubBtn() 
{
	// TODO: Add your control notification handler code here
	LRESULT re=0;
	if(m_displayTree.GetSelectedItem() == NULL)
	{
		CString info;
		info.LoadString(IDS_NOT_SHOWCHANANL);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	MoveTreeItem(m_displayTree,m_fileTree,FALSE);
}



void CCustomizeWareSetDlg::OnAppointAddBtn() 
{
	// TODO: Add your control notification handler code here
	LONG nDefaultShow = theApp.m_projectData.m_dwAnalogDef+theApp.m_projectData.m_dwDigtalInDef+theApp.m_projectData.m_dwDigtalOutDef
		+ theApp.m_projectData.m_dwFrequencyDef+theApp.m_projectData.m_dwAngleDiffDef;
	int countOfChannel,countOfAppoint,nChannelNum,i,j,k,indexOfAnalog=0,indexOfDI=0,indexOfDO=0;
	AnalogChannel *pAnalogChannel;
	CString str;
	if (nDefaultShow != 0)
	{
		int temp;
		for (i = 0; i < 32 ;i++)
		{ 
			temp = 1<<i;
			if (theApp.m_projectData.m_dwAnalogDef&temp)
			{
				pAnalogChannel = (AnalogChannel *)m_pWareFileData->m_arrPSubItem[0]->GetChannelData(i);
				ASSERT(pAnalogChannel!=NULL);
				str.Format(_T("%02d:%s"),i,(CString)pAnalogChannel->channelName);
				AddToDisWithStr(str,ANALOG_TYPE);
			}
		}
		for (i = 0; i < 32 ;i++)
		{ 
			temp = 1<<i;
			if (theApp.m_projectData.m_dwDigtalInDef&temp)
			{
				CString strChannelName;
				VERIFY(m_pWareFileData->m_arrPSubItem[1]->GetChannelName(i,strChannelName));
				str.Format(_T("%02d:%s"),i,strChannelName);
				AddToDisWithStr(str,DIGITAL_IN_TYPE);
			}
		}
		for (i = 0; i < 32 ;i++)
		{ 
			temp = 1<<i;
			if (theApp.m_projectData.m_dwDigtalOutDef&temp)
			{
				CString strChannelName;
				VERIFY(m_pWareFileData->m_arrPSubItem[2]->GetChannelName(i,strChannelName));
				str.Format(_T("%02d:%s"),i,strChannelName);
				AddToDisWithStr(str,DIGITAL_OUT_TYPE);
			}
		}
		for (i = 0; i < 32 ;i++)
		{ 
			temp = 1<<i;
			if (theApp.m_projectData.m_dwFrequencyDef&temp)
			{
				CString strChannelName;
				VERIFY(m_pWareFileData->m_arrPSubItem[4]->GetChannelName(i,strChannelName));
				str.Format(_T("%02d:%s"),i,strChannelName);
				AddToDisWithStr(str,FREQUENCY_TYPE);
			}
		}
		for (i = 0; i < 32 ;i++)
		{ 
			temp = 1<<i;
			if (theApp.m_projectData.m_dwAngleDiffDef&temp)
			{
				CString strChannelName;
				VERIFY(m_pWareFileData->m_arrPSubItem[5]->GetChannelName(i,strChannelName));
				str.Format(_T("%02d:%s"),i,strChannelName);
				AddToDisWithStr(str,ANGLE_DIFF_TYPE);
			}
		}
	}

	else
	{
		for(i=0;i<m_pWareFileData->m_nSubItemCount;i++)
		{
			switch(m_pWareFileData->m_arrPSubItem[i]->GetDataType())
			{
			case ANALOG_TYPE:
				countOfChannel = m_pWareFileData->m_arrPSubItem[i]->GetChannelCount();
				countOfAppoint = m_pWareFileData->m_arrPSubItem[i]->GetAppointCount();
				for(j=0;j<countOfAppoint;j++)
				{
					nChannelNum = m_pWareFileData->m_arrPSubItem[i]->GetAppointChannelIndex(j);
					for(k=0;k<countOfChannel;k++)
					{
						pAnalogChannel = (AnalogChannel *)m_pWareFileData->m_arrPSubItem[i]->GetChannelData(k);
						ASSERT(pAnalogChannel!=NULL);
						if(nChannelNum == pAnalogChannel->channelNum)
						{
							str.Format(_T("%02d:%s"),(indexOfAnalog+k),(CString)pAnalogChannel->channelName);
							AddToDisWithStr(str,ANALOG_TYPE);
							break;
						}
					}
					if (k>=countOfChannel)
					{
						CString formatInfo;
						formatInfo.LoadString( IDS_CHANAL_NOEXIST );
						str.Format( formatInfo, j+1, nChannelNum);
						CdMessageBox box(this, str);
		                box.MsgBoxEx();
					}
				}
				indexOfAnalog += countOfChannel;
				break;
			case DIGITAL_IN_TYPE:
				countOfChannel = m_pWareFileData->m_arrPSubItem[i]->GetChannelCount();
				countOfAppoint = m_pWareFileData->m_arrPSubItem[i]->GetAppointCount();
				for(j=0;j<countOfAppoint;j++)
				{
					nChannelNum = m_pWareFileData->m_arrPSubItem[i]->GetAppointChannelIndex(j);
					CString strChannelName;
					VERIFY(m_pWareFileData->m_arrPSubItem[i]->GetChannelName(nChannelNum,strChannelName));
					str.Format(_T("%02d:%s"),(indexOfDI+nChannelNum),strChannelName);
					AddToDisWithStr(str,DIGITAL_IN_TYPE);
				}
				indexOfDI += countOfChannel;
				break;
			case DIGITAL_OUT_TYPE:
				countOfChannel = m_pWareFileData->m_arrPSubItem[i]->GetChannelCount();
				countOfAppoint = m_pWareFileData->m_arrPSubItem[i]->GetAppointCount();
				for(j=0;j<countOfAppoint;j++)
				{
					nChannelNum = m_pWareFileData->m_arrPSubItem[i]->GetAppointChannelIndex(j);
					CString strChannelName;
					VERIFY(m_pWareFileData->m_arrPSubItem[i]->GetChannelName(nChannelNum,strChannelName));
					str.Format(_T("%02d:%s"),(indexOfDO+nChannelNum),strChannelName);
					AddToDisWithStr(str,DIGITAL_OUT_TYPE);
				}
				indexOfDO += countOfChannel;
				break;
			default:
				break;
				
			}
		}

	}
	::RedrawWindow(m_displayTree.m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
}

BOOL CCustomizeWareSetDlg::AddToDisWithStr(const CString & str,UINT nItemType)
{
	HTREEITEM hAnalog = m_displayTree.GetRootItem();
	HTREEITEM hDigitalIn =m_displayTree.GetNextSiblingItem(hAnalog);
	HTREEITEM hDigitalOut =m_displayTree.GetNextSiblingItem(hDigitalIn);
	HTREEITEM hFrequency =m_displayTree.GetNextSiblingItem(hDigitalOut);
	HTREEITEM hAngleDiff =m_displayTree.GetNextSiblingItem(hFrequency);
	
	HTREEITEM hFileAnalog=m_fileTree.GetRootItem();
	HTREEITEM hFileDigitalIn=m_fileTree.GetNextSiblingItem(hFileAnalog);
	HTREEITEM hFileDigitalOut=m_fileTree.GetNextSiblingItem(hFileDigitalIn);
	HTREEITEM hFileFrequency=m_fileTree.GetNextSiblingItem(hFileDigitalOut);
	HTREEITEM hFileAngleDiff=m_fileTree.GetNextSiblingItem(hFileFrequency);

	HTREEITEM hStrItemInFile;
	CString strItemText;
	HTREEITEM hChild,hParent;
	UINT *pShowCount;
	switch(nItemType)
	{
	case ANALOG_TYPE:
		pShowCount = &m_nAnalogShowCount;
		hParent = hAnalog;
		hChild = m_displayTree.GetChildItem(hAnalog);
		hStrItemInFile = m_fileTree.GetChildItem(hFileAnalog);
		break;
	case DIGITAL_IN_TYPE:
		pShowCount = &m_nDigitalInShowCount;
		hParent = hDigitalIn;
		hChild = m_displayTree.GetChildItem(hDigitalIn);
		hStrItemInFile = m_fileTree.GetChildItem(hFileDigitalIn);
		break;
	case DIGITAL_OUT_TYPE:
		pShowCount = &m_nDigitalOutShowCount;
		hParent = hDigitalOut;
		hChild = m_displayTree.GetChildItem(hDigitalOut);
		hStrItemInFile = m_fileTree.GetChildItem(hFileDigitalOut);
		break;
	case FREQUENCY_TYPE:
		pShowCount = &m_nFrequencyShowCount;
		hParent = hFrequency;
		hChild = m_displayTree.GetChildItem(hFrequency);
		hStrItemInFile = m_fileTree.GetChildItem(hFileFrequency);
		break;
	case ANGLE_DIFF_TYPE:
		pShowCount = &m_nAngleShowCount;
		hParent = hAngleDiff;
		hChild = m_displayTree.GetChildItem(hAngleDiff);
		hStrItemInFile = m_fileTree.GetChildItem(hFileAngleDiff);
		break;
	default:
		break;
	}
	while(hStrItemInFile!=NULL)
	{
		strItemText = m_fileTree.GetItemText(hStrItemInFile);
		if(str > strItemText)
		{
			hStrItemInFile = m_fileTree.GetNextSiblingItem(hStrItemInFile);
			continue;
		}
		else if(str != strItemText)
		{
			hStrItemInFile = NULL;   //未找到str的选项
		}
		break;
	}
	if(hStrItemInFile != NULL)
	{
		VERIFY(m_fileTree.DeleteItem(hStrItemInFile));
		m_displayTree.InsertItem(str,hParent,TVI_LAST);
		(*pShowCount)++;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CCustomizeWareSetDlg::IsAppointChannel(const CString &strItemText)
{
	int i,nSubItemIndex,nChannelIndex,nAppointCount;
	map<CString,UINT>::iterator Iter;
	Iter = m_fileTree.m_mapTreeItem.find(strItemText);
	if(Iter == m_fileTree.m_mapTreeItem.end())
	{
		return FALSE;
	}
	nSubItemIndex = Iter->second>>16;
	nChannelIndex = Iter->second & 0x0000FFFF;
	switch (m_pWareFileData->m_arrPSubItem[nSubItemIndex]->GetDataType())
	{
	case ANALOG_TYPE:
		nChannelIndex = ((AnalogChannel *)m_pWareFileData
			->m_arrPSubItem[nSubItemIndex]->GetChannelData(nChannelIndex))->channelNum;
		nAppointCount = m_pWareFileData->m_arrPSubItem[nSubItemIndex]->GetAppointCount();
		for(i=0; i<nAppointCount; i++)
		{
			if(nChannelIndex == m_pWareFileData->m_arrPSubItem[nSubItemIndex]->GetAppointChannelIndex(i))
			{
				return TRUE;
			}
		}
		return FALSE;
	case DIGITAL_IN_TYPE:
	case DIGITAL_OUT_TYPE:
		nAppointCount = m_pWareFileData->m_arrPSubItem[nSubItemIndex]->GetAppointCount();
		for(i=0; i<nAppointCount; i++)
		{
			if(nChannelIndex == m_pWareFileData->m_arrPSubItem[nSubItemIndex]->GetAppointChannelIndex(i))
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	return FALSE;
}

void CCustomizeWareSetDlg::OnMoveUpBtn() 
{
	// TODO: Add your control notification handler code here
	CString strSelItemText,strProItemText,strTemp;
	HTREEITEM hProvious,hParentItem,hSelItem = m_displayTree.GetSelectedItem();
	CString info;
	if(hSelItem == NULL)
	{
		info.LoadString(IDS_SEL_UPCHANANL);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	strSelItemText = m_displayTree.GetItemText(hSelItem);
	m_fileTree.m_tempItem = m_fileTree.m_mapTreeItem.find(strSelItemText);
	if(m_fileTree.m_tempItem == m_fileTree.m_mapTreeItem.end())
	{
		info.LoadString(IDS_UPCHANANL);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	int nSubItemIndex,nChannelIndex;
	nSubItemIndex = m_fileTree.m_tempItem->second>>16;
	nChannelIndex = m_fileTree.m_tempItem->second & 0x0000FFFF;
	hParentItem = m_displayTree.GetParentItem(hSelItem);
	hProvious = m_displayTree.GetNextItem(hSelItem,TVGN_PREVIOUS);
	strProItemText = m_displayTree.GetItemText(hProvious);
	if(hProvious == NULL)
	{
			info.LoadString(IDS_MOVE_CHANANLINFO);
			CdMessageBox box(this, info);
		    box.MsgBoxEx();
		return;
	}
	VERIFY(m_displayTree.DeleteItem(hProvious));
	m_displayTree.InsertItem(strProItemText,hParentItem,hSelItem);
}

void CCustomizeWareSetDlg::OnMoveDownBtn() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CString strSelItemText,strNextItemText,strTemp;
	HTREEITEM hNextItem,hParentItem,hSelItem = m_displayTree.GetSelectedItem();
	CString info;
	if(hSelItem == NULL)
	{
		info.LoadString(IDS_MOVE_DOWNINFO);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	strSelItemText = m_displayTree.GetItemText(hSelItem);
	m_fileTree.m_tempItem = m_fileTree.m_mapTreeItem.find(strSelItemText);
	if(m_fileTree.m_tempItem == m_fileTree.m_mapTreeItem.end())
	{
		info.LoadString(IDS_MOVEDOWN_ERROR);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	int nSubItemIndex,nChannelIndex;
	nSubItemIndex = m_fileTree.m_tempItem->second>>16;
	nChannelIndex = m_fileTree.m_tempItem->second & 0x0000FFFF;
	hParentItem = m_displayTree.GetParentItem(hSelItem);
	hNextItem = m_displayTree.GetNextItem(hSelItem,TVGN_NEXT);
	strNextItemText = m_displayTree.GetItemText(hNextItem);
	if(hNextItem == NULL)
	{
		info.LoadString(IDS_MOVEDOWN_INFO1);
		strTemp.Format(info, strSelItemText);
		CdMessageBox box(this, strTemp);
		box.MsgBoxEx();
		return;
	}
	VERIFY(m_displayTree.DeleteItem(hSelItem));
	m_displayTree.InsertItem(strSelItemText,hParentItem,hNextItem);
	hSelItem = m_displayTree.GetNextItem(hNextItem,TVGN_NEXT);
	m_displayTree.SelectItem(hSelItem);
}

void CCustomizeWareSetDlg::AddCurveToDisplay(map<UINT,CString> mapToString)
{
	ASSERT(m_pCurve!=NULL);
	UINT key;
	CString strTemp;
	int i;
	map<UINT,CString>::iterator Iter;
	for(i=0;i<m_nCurveCount;i++)
	{
		if(m_pCurve[i].nSubItemIndex >= 0)
		{
			key = m_pCurve[i].nSubItemIndex << 16;
			key |= m_pCurve[i].nChannelIndex;
			Iter = mapToString.find(key);
			if(Iter == mapToString.end())
			{
				CString info;
				info.LoadString( IDS_ERROR_CHANANL );
				CdMessageBox box(this, info);
		        box.MsgBoxEx();
				return;
			}
			AddToDisWithStr(Iter->second,m_pWareFileData->m_arrPSubItem[m_pCurve[i].nSubItemIndex]->GetDataType());
		}
	}
}

void CCustomizeWareSetDlg::MoveTreeItem(CTreeCtrlEx &sourceTree, CTreeCtrlEx &desTree,BOOL bAddToDis/* = TRUE*/)
{
	HTREEITEM hSelectItem = sourceTree.GetSelectedItem();
	HTREEITEM hDesAnalog = desTree.GetRootItem();
	HTREEITEM hDesDigitalIn =desTree.GetNextSiblingItem(hDesAnalog);
	HTREEITEM hDesDigitalOut =desTree.GetNextSiblingItem(hDesDigitalIn);
	HTREEITEM hDesFrequency =desTree.GetNextSiblingItem(hDesDigitalOut);
	HTREEITEM hDesAngleDiff =desTree.GetNextSiblingItem(hDesFrequency);
	HTREEITEM hSourceAnalog=sourceTree.GetRootItem();
	HTREEITEM hSourceDigitalIn=sourceTree.GetNextSiblingItem(hSourceAnalog);
	HTREEITEM hSourceDigitalOut=sourceTree.GetNextSiblingItem(hSourceDigitalIn);
	HTREEITEM hSourceFrequency=sourceTree.GetNextSiblingItem(hSourceDigitalOut);
	HTREEITEM hSourceAngleDiff=sourceTree.GetNextSiblingItem(hSourceFrequency);
	CString strSelect=sourceTree.GetItemText(hSelectItem);
	CString str,strTemp,strItemText;
	list<CString>::iterator iterStr;
	HTREEITEM hTempItem,hBrotherItem;//hBrotherItem是hTempItem的哥哥，如果hTempItem没有哥哥，则为NULL
	CString info, strAnalog, strDi, strDo, strFre, strAngDiff;
	strAnalog.LoadString( IDS_STR_ANALOG );
	strDi.LoadString( IDS_DIGITAL_INPUT );
	strDo.LoadString( IDS_DIGITAL_OUTPUT );
	strFre.LoadString( IDS_FREQUENCY );
	strAngDiff.LoadString( IDS_ANG_DIFF );
	if(strSelect == strAnalog)
	{
		hTempItem = sourceTree.GetNextItem(hSourceAnalog,TVGN_CHILD);
		while(hTempItem != NULL)
		{
			strItemText = sourceTree.GetItemText(hTempItem);
			hBrotherItem = sourceTree.GetNextItem(hTempItem,TVGN_PREVIOUS);
			VERIFY(sourceTree.DeleteItem(hTempItem));
			HTREEITEM hInsertPos=TVI_FIRST;
			HTREEITEM hChild=desTree.GetChildItem(hDesAnalog);
			while(hChild!=NULL)
			{
				str=desTree.GetItemText(hChild);
				if(strItemText<str)
				{
					desTree.InsertItem(strItemText,hDesAnalog,hInsertPos);
					break;
				}
				hInsertPos=hChild;
				hChild=desTree.GetNextSiblingItem(hChild);
			}
			if(hChild==NULL)
			{
				desTree.InsertItem(strItemText,hDesAnalog,TVI_LAST);
			}
			bAddToDis ? m_nAnalogShowCount++ : m_nAnalogShowCount--;
			hTempItem = hBrotherItem == NULL ? sourceTree.GetNextItem(hSourceAnalog,TVGN_CHILD) : 
				sourceTree.GetNextItem(hBrotherItem,TVGN_NEXT);
		}
		if(!bAddToDis)
		{
			ASSERT( m_nAnalogShowCount!=0 || m_nAnalogShowCount==0);
		}
	}
	else if(strSelect == strDi)
	{
		hTempItem = sourceTree.GetNextItem(hSourceDigitalIn,TVGN_CHILD);
		while(hTempItem != NULL)
		{
			strItemText = sourceTree.GetItemText(hTempItem);
			hBrotherItem = sourceTree.GetNextItem(hTempItem,TVGN_PREVIOUS);
			VERIFY(sourceTree.DeleteItem(hTempItem));
			HTREEITEM hInsertPos=TVI_FIRST;
			HTREEITEM hChild=desTree.GetChildItem(hDesDigitalIn);
			while(hChild!=NULL)
			{
				str=desTree.GetItemText(hChild);
// 				if(strItemText<str)
// 				{
// 					desTree.InsertItem(strItemText,hDesDigitalIn,hInsertPos);
// 					break;
// 				}
				if(strItemText<str)
				{
					desTree.InsertItem(strItemText,hDesDigitalIn,hInsertPos);
					break;
				}
				hInsertPos=hChild;
				hChild=desTree.GetNextSiblingItem(hChild);
			}
			if(hChild==NULL)
			{
				desTree.InsertItem(strItemText,hDesDigitalIn,TVI_LAST);
			}
			bAddToDis ? m_nDigitalInShowCount++ : m_nDigitalInShowCount--;
			hTempItem = hBrotherItem == NULL ? sourceTree.GetNextItem(hSourceDigitalIn,TVGN_CHILD) : 
				sourceTree.GetNextItem(hBrotherItem,TVGN_NEXT);
		}
	}
	else if(strSelect == strDo)
	{
		hTempItem = sourceTree.GetNextItem(hSourceDigitalOut,TVGN_CHILD);
		while(hTempItem != NULL)
		{
			strItemText = sourceTree.GetItemText(hTempItem);
			hBrotherItem = sourceTree.GetNextItem(hTempItem,TVGN_PREVIOUS);
			VERIFY(sourceTree.DeleteItem(hTempItem));
			HTREEITEM hInsertPos=TVI_FIRST;
			HTREEITEM hChild=desTree.GetChildItem(hDesDigitalOut);
			while(hChild!=NULL)
			{
				str=desTree.GetItemText(hChild);
// 				if(strItemText<str)
// 				{
// 					desTree.InsertItem(strItemText,hDesDigitalOut,hInsertPos);
// 					break;
// 				}
				if(strItemText<str)
				{
					desTree.InsertItem(strItemText,hDesDigitalOut,hInsertPos);
					break;
				}
				hInsertPos=hChild;
				hChild=desTree.GetNextSiblingItem(hChild);
			}
			if(hChild==NULL)
			{
				desTree.InsertItem(strItemText,hDesDigitalOut,TVI_LAST);
			}
			bAddToDis ? m_nDigitalOutShowCount++ : m_nDigitalOutShowCount--;
			hTempItem = hBrotherItem == NULL ? sourceTree.GetNextItem(hSourceDigitalOut,TVGN_CHILD) : 
				sourceTree.GetNextItem(hBrotherItem,TVGN_NEXT);
		}
	}
	else if( strSelect == strFre )
	{
		hTempItem = sourceTree.GetNextItem(hSourceFrequency,TVGN_CHILD);
		while(hTempItem != NULL)
		{
			strItemText = sourceTree.GetItemText(hTempItem);
			hBrotherItem = sourceTree.GetNextItem(hTempItem,TVGN_PREVIOUS);
			VERIFY(sourceTree.DeleteItem(hTempItem));
			HTREEITEM hInsertPos=TVI_FIRST;
			HTREEITEM hChild=desTree.GetChildItem(hDesFrequency);
			while(hChild!=NULL)
			{
				str=desTree.GetItemText(hChild);
// 				if(strItemText<str)
// 				{
// 					desTree.InsertItem(strItemText,hDesDigitalOut,hInsertPos);
// 					break;
// 				}
				if(strItemText<str)
				{
					desTree.InsertItem(strItemText,hDesFrequency,hInsertPos);
					break;
				}
				hInsertPos=hChild;
				hChild=desTree.GetNextSiblingItem(hChild);
			}
			if(hChild==NULL)
			{
				desTree.InsertItem(strItemText,hDesFrequency,TVI_LAST);
			}
			bAddToDis ? m_nFrequencyShowCount++ : m_nFrequencyShowCount--;
			hTempItem = hBrotherItem == NULL ? sourceTree.GetNextItem(hSourceFrequency,TVGN_CHILD) : 
				sourceTree.GetNextItem(hBrotherItem,TVGN_NEXT);
		}
	}
	else if( strSelect == strAngDiff )
	{
		hTempItem = sourceTree.GetNextItem(hSourceAngleDiff,TVGN_CHILD);
		while(hTempItem != NULL)
		{
			strItemText = sourceTree.GetItemText(hTempItem);
			hBrotherItem = sourceTree.GetNextItem(hTempItem,TVGN_PREVIOUS);
			VERIFY(sourceTree.DeleteItem(hTempItem));
			HTREEITEM hInsertPos=TVI_FIRST;
			HTREEITEM hChild=desTree.GetChildItem(hDesAngleDiff);
			while(hChild!=NULL)
			{
				str=desTree.GetItemText(hChild);
// 				if(strItemText<str)
// 				{
// 					desTree.InsertItem(strItemText,hDesDigitalOut,hInsertPos);
// 					break;
// 				}
				if(strItemText<str)
				{
					desTree.InsertItem(strItemText,hDesAngleDiff,hInsertPos);
					break;
				}
				hInsertPos=hChild;
				hChild=desTree.GetNextSiblingItem(hChild);
			}
			if(hChild==NULL)
			{
				desTree.InsertItem(strItemText,hDesAngleDiff,TVI_LAST);
			}
			bAddToDis ? m_nAngleShowCount++ : m_nAngleShowCount--;
			hTempItem = hBrotherItem == NULL ? sourceTree.GetNextItem(hSourceAngleDiff,TVGN_CHILD) : 
				sourceTree.GetNextItem(hBrotherItem,TVGN_NEXT);
		}
	}
	else if(sourceTree.GetParentItem(sourceTree.GetSelectedItem())==hSourceAnalog)//双点模拟量的子项
	{
		VERIFY(sourceTree.DeleteItem(hSelectItem));
		HTREEITEM hInsertPos=TVI_FIRST;
		HTREEITEM hChild=desTree.GetChildItem(hDesAnalog);
		while(hChild!=NULL)
		{
			str=desTree.GetItemText(hChild);
// 			if(strSelect<str)
// 			{
// 				desTree.InsertItem(strSelect,hDesAnalog,hInsertPos);
// 				break;
// 			}
			if(strSelect<str)
			{
				desTree.InsertItem(strSelect,hDesAnalog,hInsertPos);
				break;
			}
			hInsertPos=hChild;
			hChild=desTree.GetNextSiblingItem(hChild);
		}
		if(hChild==NULL)
		{
			desTree.InsertItem(strSelect,hDesAnalog,TVI_LAST);
		}
		bAddToDis ? m_nAnalogShowCount++ : m_nAnalogShowCount--;
	}
	else if(sourceTree.GetParentItem(sourceTree.GetSelectedItem())==hSourceDigitalIn)
	{
		VERIFY(sourceTree.DeleteItem(hSelectItem));
		HTREEITEM hInsertPos=TVI_FIRST;
		HTREEITEM hChild=desTree.GetChildItem(hDesDigitalIn);
		while(hChild!=NULL)
		{
			str=desTree.GetItemText(hChild);
// 			if(strSelect<str)
// 			{
// 				desTree.InsertItem(strSelect,hDesDigitalIn,hInsertPos);
// 				break;
// 			}
			if(strSelect<str)
			{
				desTree.InsertItem(strSelect,hDesDigitalIn,hInsertPos);
				break;
			}
			hInsertPos=hChild;
			hChild=desTree.GetNextSiblingItem(hChild);
		}
		if(hChild==NULL)
		{
			desTree.InsertItem(strSelect,hDesDigitalIn,TVI_LAST);
		}
		bAddToDis ? m_nDigitalInShowCount++ : m_nDigitalInShowCount--;
	}
	else if(sourceTree.GetParentItem(sourceTree.GetSelectedItem())==hSourceDigitalOut)
	{
		VERIFY(sourceTree.DeleteItem(hSelectItem));
		HTREEITEM hInsertPos=TVI_FIRST;
		HTREEITEM hChild=desTree.GetChildItem(hDesDigitalOut);
	
		while(hChild!=NULL)
		{
			str=desTree.GetItemText(hChild);
// 			if(strSelect<str)
// 			{
// 				desTree.InsertItem(strSelect,hDesDigitalOut,hInsertPos);
// 				break;
// 			}
			if(strSelect<str)
			{
				desTree.InsertItem(strSelect,hDesDigitalOut,hInsertPos);
				break;
			}
			hInsertPos=hChild;
			hChild=desTree.GetNextSiblingItem(hChild);
		}
		if(hChild==NULL)
		{
			desTree.InsertItem(strSelect,hDesDigitalOut,TVI_LAST);
		}
		bAddToDis ? m_nDigitalOutShowCount++ : m_nDigitalOutShowCount--;
	}
	else if(sourceTree.GetParentItem(sourceTree.GetSelectedItem())==hSourceFrequency)
	{
		VERIFY(sourceTree.DeleteItem(hSelectItem));
		HTREEITEM hInsertPos=TVI_FIRST;
		HTREEITEM hChild=desTree.GetChildItem(hDesFrequency);
	
		while(hChild!=NULL)
		{
			str=desTree.GetItemText(hChild);
// 			if(strSelect<str)
// 			{
// 				desTree.InsertItem(strSelect,hDesDigitalOut,hInsertPos);
// 				break;
// 			}
			if(strSelect<str)
			{
				desTree.InsertItem(strSelect,hDesFrequency,hInsertPos);
				break;
			}
			hInsertPos=hChild;
			hChild=desTree.GetNextSiblingItem(hChild);
		}
		if(hChild==NULL)
		{
			desTree.InsertItem(strSelect,hDesFrequency,TVI_LAST);
		}
		bAddToDis ? m_nFrequencyShowCount++ : m_nFrequencyShowCount--;
	}
	else if(sourceTree.GetParentItem(sourceTree.GetSelectedItem())==hSourceAngleDiff)
	{
		VERIFY(sourceTree.DeleteItem(hSelectItem));
		HTREEITEM hInsertPos=TVI_FIRST;
		HTREEITEM hChild=desTree.GetChildItem(hDesAngleDiff);
	
		while(hChild!=NULL)
		{
			str=desTree.GetItemText(hChild);
// 			if(strSelect<str)
// 			{
// 				desTree.InsertItem(strSelect,hDesDigitalOut,hInsertPos);
// 				break;
// 			}
			if(strSelect<str)
			{
				desTree.InsertItem(strSelect,hDesAngleDiff,hInsertPos);
				break;
			}
			hInsertPos=hChild;
			hChild=desTree.GetNextSiblingItem(hChild);
		}
		if(hChild==NULL)
		{
			desTree.InsertItem(strSelect,hDesAngleDiff,TVI_LAST);
		}
		bAddToDis ? m_nAngleShowCount++ : m_nAngleShowCount--;
	}
	sourceTree.SelectItem(NULL);
	::RedrawWindow(desTree.m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
}