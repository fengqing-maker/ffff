// ParaView.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "ParaView.h"
#include "Frame.h"
#include "PWDDlg.h"
#include "hangePswDlg.h"
#include "MainFrm.h"
#include "dMessageBox.h"
#include "excel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PASSWORD "SZZN"
/////////////////////////////////////////////////////////////////////////////
// CParaView

extern CMySID_2FYApp theApp;
IMPLEMENT_DYNCREATE(CParaView, CFormView)

CParaView::CParaView()
	: CFormView(CParaView::IDD)
	, m_strSetArea(_T(""))
{
	//{{AFX_DATA_INIT(CParaView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nCurParaArea=0;
	m_nParaAreaCount=0;
	m_nParaCount=0;
	m_nDisParaArea=0;
	typeCount=0;
	m_nPsw=PASSWORD;
	m_bSystemPara = FALSE;
	m_nCurIndex = 0;
	ZeroMemory(m_nParasEachType,sizeof(m_nParasEachType));
	m_nSetParaArea = 0x00;
	m_bIputExcelFile = FALSE;
}

CParaView::~CParaView()
{
}

void CParaView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParaView)
	DDX_Control(pDX, IDC_PARA_TYPE, m_ctlParaType);
	DDX_Control(pDX, IDC_PARA_LIST, m_listPara);
	DDX_Control(pDX, IDC_PARA_AREA, m_ctlParaArea);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_SETAREA, m_ctlSetArea);
	DDX_CBString(pDX, IDC_COMBO_SETAREA, m_strSetArea);
}


BEGIN_MESSAGE_MAP(CParaView, CFormView)
	//{{AFX_MSG_MAP(CParaView)
	ON_BN_CLICKED(IDC_SET_CUR_PARA_AREA_BTN, OnSetCurParaAreaBtn)
	ON_BN_CLICKED(IDC_SET_PARA_BTN, OnSetParaBtn)
	ON_CBN_SELCHANGE(IDC_PARA_TYPE, OnSelchangeParaType)
	ON_CBN_SELCHANGE(IDC_PARA_AREA, OnSelchangeParaArea)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_READ_PARA, OnReadPara)
	ON_BN_CLICKED(IDC_READ_PARA_FILE, OnReadParaFile)
	ON_BN_CLICKED(IDC_DEVICE_RESET, OnDeviceReset)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_EDIT_CHANGE, OnEditChange)
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_COMBO_SELECTION, OnComboxSelection)
	ON_CBN_SELCHANGE(IDC_COMBO_SETAREA, OnCbnSelchangeComboSetarea)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParaView diagnostics

#ifdef _DEBUG
void CParaView::AssertValid() const
{
	CFormView::AssertValid();
}

void CParaView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParaView message handlers
CString CParaView::GetErrorInfo( int errorNum )
{
	CString str;
    switch( OPERATION_RESULT_SUCCESS )
	{
    case OPERATION_RESULT_SUCCESS:
		break;
	case OPERATION_RESULT_OVERFLOW:
		str.LoadString( IDS_OPER_OVERFLAG );
	    break;
	case OPERATION_RESULT_TRANS_ERROR:
		str.LoadString( IDS_OPER_TRAN_ERROR );
		break;
	case OPERATION_RESULT_DEVICE_LOCK:
	    str.LoadString( IDS_OPER_DEVS_LOCK );
		break;
	case OPERATION_RESULT_FORMAT_ERROR:
		str.LoadString( IDS_OPER_FORMAT_ERROR );
		break;
    case OPERATION_RESULT_TIMEOUT:
		str.LoadString( IDS_OPER_TIMEOUT );
		break;
	case OPERATION_RESULT_MODULE_ERROR:
		str.LoadString( IDS_OPER_WORKMODE );
		break;
	}
	return str;
}

void CParaView::OnSetCurParaAreaBtn() 
{
	// TODO: Add your control notification handler code here	
	CPWDDlg dlg;
	CString str, title, info;
	title.LoadString( IDS_DLG_DEFTITLE );
	while(IDOK==dlg.DoModal())
	{
		if(dlg.m_nPassWord!=m_nPsw)
		{	
			str.LoadString( IDS_PWD_ERROR_INPUT );
			CdMessageBox box(this, str, title, MB_OK);
			box.MsgBoxEx();
			dlg.m_nPassWord=_T("");
		}
		else	
		{
			wchar_t strIndexArea[10];
			m_ctlParaArea.GetLBText(m_ctlParaArea.GetCurSel(),strIndexArea);
			int areaIndex;
			swscanf(strIndexArea,_T("%d"),&areaIndex);

	        info.LoadString( IDS_SET_AREA );
	        str.Format(info, areaIndex);
	        
	        CdMessageBox box(this, str, title, MB_YESNO);
	        if(IDNO == box.MsgBoxEx() )
			{
				return;
			}
			ServerFrame serverFrame;
			serverFrame.nServerType = SERVER_PARA;
			serverFrame.nFlag = QUERY_PARA_AREA_TRANSFER;
			serverFrame.queryFrame.nQueryOrSaveParaArea = areaIndex;
			BOOL retValue = CFrame::SetCurParaArea(areaIndex);
			if ( FALSE == retValue )
			{
				str.LoadString( IDS_CONNECT_ERROR );
				CdMessageBox box( this, str);
	            box.MsgBoxEx();
			}else if ( OPERATION_RESULT_SUCCESS == retValue)
			{
				m_nCurParaArea = (BYTE)areaIndex;
			}else
			{
                CString errorInfo = GetErrorInfo( retValue );
				if ( errorInfo.GetLength() > 0 )
				{
                     str.LoadString( IDS_CHANGEAREA_ERROR );
					 str += ',' + errorInfo;
					 CdMessageBox box( this, str);
	                 box.MsgBoxEx();
				}
			}
			break;
		}
	}
}

void CParaView::OnSetParaBtn() 
{
	// TODO: Add your control notification handler code here
	CPWDDlg dlg;
	CString strParaType;
	CString str;
	while(IDOK==dlg.DoModal())
	{
		if(dlg.m_bCorrect)
		{
			unsigned short *pBuf=new unsigned short[m_nParaCount];
			ZeroMemory(pBuf,sizeof(unsigned short)*m_nParaCount);
			unsigned short i;
			for(i=0;i<m_nParaCount;i++)
			{
				pBuf[i]=m_arrParaData[i].nParaValue;
			}
	        CString info;
			OnCbnSelchangeComboSetarea();
			CString	strOptParaArea;
			if (m_nSetParaArea == 0xff)
			{
				strOptParaArea = (_T("定值设置操作对象：系统定值；"));
			}
			else
			{
				strOptParaArea.Format(_T("定值操作对象：%d号定值区；"),m_nSetParaArea + 1);
			}
			BOOL retSaveValue = CFrame::SetParaData(m_nSetParaArea,m_nParaCount,pBuf);
			if ( FALSE == retSaveValue )
			{
				str.LoadString( IDS_CONNECT_ERROR );
				CdMessageBox box( this, str);
				box.MsgBoxEx();
			}
			else if ( OPERATION_RESULT_SUCCESS == retSaveValue )
			{
				info.LoadString( IDS_SETPARA_OK );
				CdMessageBox box(this, info, _T(""), MB_YESNO);
				if(IDYES == box.MsgBoxEx() )			
				{
					BOOL retValue = CFrame::SaveParaArea(m_nSetParaArea);
					if ( FALSE == retValue )
					{
						str.LoadString( IDS_CONNECT_ERROR );
						CdMessageBox box( this, str);
						box.MsgBoxEx();
					}
					else if ( OPERATION_RESULT_SUCCESS == retValue )
					{
						str = theApp.m_projectData.m_strDeviceType;
						if (str != DVI_DEVICE)
						{
							info.LoadString( IDS_SOLIDIFY_OK );
							CdMessageBox box2( this, info);
							box2.MsgBoxEx();
							m_nCurIndex = 0;
							LoadData();
							m_ctlParaType.GetLBText(m_ctlParaType.GetCurSel(),strParaType);
							info.LoadString(  IDS_ALL_TYPE  );
							if(strParaType != info) 
							{
								LoadParaTypeData(strParaType);
							}
						}
						else
						{
							((CMainFrame*)GetParentFrame())->StopCommunication();
							str.LoadString( IDS_SOLIDIFY_OK );
							info.LoadString(IDS_DEVS_RESTART);
							str += _T(',') + info;
							CdMessageBox box2( this, str);
							box2.MsgBoxEx();
						}
					}
					else
					{
						CString errorInfo = GetErrorInfo( retValue );
						if ( errorInfo.GetLength() > 0 )
						{
							str.LoadString( IDS_SOLIDIFY_INFO );
							str += ',' + errorInfo;
							CdMessageBox box2( this, str);
							box.MsgBoxEx();
						}
					}
				}
			}
			else 
			{
	             CString errorInfo = GetErrorInfo( retSaveValue );
				 if ( errorInfo.GetLength() > 0 )
				 {
					 str.LoadString( IDS_SETPARA_ERROR );
					 str += ',' + errorInfo;
					 CdMessageBox box2( this, str);
					 box2.MsgBoxEx();
				}
			}
			delete [] pBuf;
			break;
		}
	}
}

void CParaView::OnSelchangeParaType() 
{
	// TODO: Add your control notification handler code here
	CString strParaType;
	int temp = m_ctlParaType.GetCurSel();
	m_nCurIndex = 0;
	for (int i =0 ;i < temp;i++)
	{
		m_nCurIndex += m_nParasEachType[i];
	}
	m_ctlParaType.GetLBText(temp,strParaType);
	LoadParaTypeData(strParaType);
	this->m_listPara.SetFocus();
}

void CParaView::OnSelchangeParaArea() 
{
	// TODO: Add your control notification handler code here
	wchar_t strIndex[20];
	m_ctlParaArea.GetLBText(m_ctlParaArea.GetCurSel(),strIndex);
	CString str, typeStr ;
	str.Format(_T("%s"), strIndex);
	typeStr.LoadString( IDS_SYS_SETPOINT );
	if (str == typeStr)			//如果是系统地址
	{
//		GetDlgItem(IDC_SET_CUR_PARA_AREA_BTN)->EnableWindow(FALSE);
		m_nDisParaArea = 0xFF;
	}
	else
	{
		int index;
		swscanf(strIndex,_T("%d"),&index);
		m_nDisParaArea = index - 1;
	}
	LoadData();
	this->m_listPara.SetFocus();
}

void CParaView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	theApp.m_pLangManger->SetWndStrings(this, IDD);
	CString strDevice;
	strDevice = theApp.m_projectData.m_strDeviceType;
	if (strDevice != DVI_DEVICE)
	{
		GetDlgItem(IDC_DEVICE_RESET)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_DEVICE_RESET)->ShowWindow(SW_SHOW);
	}
	// TODO: Add your specialized code here and/or call the base class
//	GetDlgItem(IDC_SET_CUR_PARA_AREA_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_SET_PARA_BTN)->EnableWindow(FALSE);
	CRect rc;
	CString info;
	m_listPara.GetClientRect(&rc);
	long width=rc.Width();
	m_listPara.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	info.LoadString( IDS_PARA_SERIAL );
	m_listPara.InsertColumn(0, info,LVCFMT_LEFT,50);
    info.LoadString( IDS_PARA_TYPE );
	m_listPara.InsertColumn(1, info,LVCFMT_LEFT,120);
	info.LoadString( IDS_PARA_NAME);
	m_listPara.InsertColumn(2, info,LVCFMT_LEFT,150);
	info.LoadString( IDS_PARA_DATA );
	m_listPara.InsertColumn(3, info,LVCFMT_LEFT,170);
	info.LoadString( IDS_PARA_RANGE );
	m_listPara.InsertColumn(4, info,LVCFMT_LEFT,(width-510 >0)?(width-510):0);
	m_ctlParaType.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_ctlParaType.MoveWindow(rc.left,rc.top,rc.Width(),300);
	m_ctlParaArea.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_ctlParaArea.MoveWindow(rc.left,rc.top,rc.Width(),300);
	//
	m_ctlSetArea.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_ctlSetArea.MoveWindow(rc.left,rc.top,rc.Width(),300);
	OnReadPara();
}

void CParaView::LoadData()
{	
	m_bIputExcelFile = FALSE;
	ZeroMemory(m_nParasEachType,sizeof(m_nParasEachType));
	m_nParaCount=MAX_PARA_COUNT;
	CString info;
	if (FALSE == CFrame::GetAllPara(m_nDisParaArea,m_arrParaData,m_nParaCount))
	{
        info.LoadString( IDS_READ_SETTING_FAILED );
		CdMessageBox box( this, info);
		box.MsgBoxEx();
		return;
	}
	m_listPara.SetRedraw(FALSE);
	unsigned short i,j;
	int nParaIndex=0;
	CString str,strParaType,strOldParaType=_T(""),strRange,strFormat;

// 	if(m_ctlParaType.GetCount()>0)
// 	{
// 		m_ctlParaType.GetLBText(m_ctlParaType.GetCurSel(),strOldParaType);
// 	}
	for(i=m_ctlParaType.GetCount()-1;((short)i)>=0;i--)
	{
		m_ctlParaType.DeleteString(i);
	}
	strParaType.LoadString( IDS_ALL_TYPE );
	//strParaType=_T("所有类型");
	m_ctlParaType.AddString(strParaType);
	m_listPara.DeleteAllItems();
	for(i=0;i<m_nParaCount;i++)
	{
		ASSERT(m_arrParaData[i].nParaNum == i);
		if(strParaType != m_arrParaData[i].strTypeName)
		{
			strParaType=m_arrParaData[i].strTypeName;
			m_ctlParaType.AddString(strParaType);
			nParaIndex++;
		}
		m_nParasEachType[nParaIndex]++;
		str.Format(_T("%u"),i+1);
		m_listPara.InsertItem(i,str);
		m_listPara.SetItemText(i,1,(CString)m_arrParaData[i].strTypeName);
		m_listPara.SetItemText(i,2,(CString)m_arrParaData[i].strPareName);
		switch (m_arrParaData[i].nTranFormat)
		{
		case PARA_TYPE_DATA:
			{
				str = GetParaDataValueString(m_arrParaData[i]);
				m_listPara.SetEdit(i,3,str,RGB(0,0,0),RGB(255,255,255));
				double dMax,dMin;
				dMax=GetParaDataValue(m_arrParaData[i].nParaMaxValue,
					m_arrParaData[i].differData.dataValue.nDecBitNum);
				dMin=GetParaDataValue(m_arrParaData[i].nParaMinValue,
					m_arrParaData[i].differData.dataValue.nDecBitNum);
				info.LoadString( IDS_DATA_RANGE );
				info += _T(": %%.%dlf%%s——%%.%dlf%%s");
				strFormat.Format(info,m_arrParaData[i].differData.dataValue.nDecBitNum,
					m_arrParaData[i].differData.dataValue.nDecBitNum);
				str.Format(strFormat,
					dMin,(CString)m_arrParaData[i].differData.dataValue.strUnit,
					dMax,(CString)m_arrParaData[i].differData.dataValue.strUnit);
				strFormat.Format(_T("%%.%dlf%%s——%%.%dlf%%s"),m_arrParaData[i].differData.dataValue.nDecBitNum,
					m_arrParaData[i].differData.dataValue.nDecBitNum);
				str.Format(strFormat,
					dMin,(CString)m_arrParaData[i].differData.dataValue.strUnit,
					dMax,(CString)m_arrParaData[i].differData.dataValue.strUnit);
				
				m_listPara.SetItemText(i,4,str,RGB(0,0,0),RGB(255,255,255));
				break;
			}
		case PARA_TYPE_OPTION:
			{
				CStringArray strItem;
				strRange = _T("");
				for(j=m_arrParaData[i].nParaMinValue;j<=m_arrParaData[i].nParaMaxValue;j++)
				{
					strItem.Add(m_arrParaData[i].differData.arrStrOption[j]);
					strRange += (CString)m_arrParaData[i].differData.arrStrOption[j]+_T("、");
					
				}
				strRange = strRange.Left(strRange.ReverseFind(TEXT('、')));
				m_listPara.SetComboBox(i,3,TRUE,&strItem,3,
					m_arrParaData[i].nParaValue-m_arrParaData[i].nParaMinValue);
				m_listPara.SetItemText(i,4,strRange);
				m_listPara.SetItemToolTipText(i,4,strRange);
				break;
			}
		}
	}
	info.LoadString( IDS_ALL_TYPE );
	strOldParaType == _T("") ? m_ctlParaType.SelectString(0, info) :
	     m_ctlParaType.SelectString(0,strOldParaType);
	m_listPara.SetRedraw(TRUE);
	if(strOldParaType != _T("") && strOldParaType != info)
	{
		LoadParaTypeData(strOldParaType);
	}
	
}

LRESULT CParaView::OnEditChange(WPARAM nItem, LPARAM nSubItem)
{
	if (m_bIputExcelFile)
	{
		CString	strChange = m_listPara.GetItemText(nItem,nSubItem);
		int nCurrent = OnCalculateParaValue(strChange,m_arrParaData[nItem].differData.dataValue.nIntBitNum);
		if (nCurrent > m_arrParaData[nItem].nParaMaxValue || nCurrent < m_arrParaData[nItem].nParaMinValue)
		{
			OnCalulateString(nItem,nSubItem);
		}
		else
		{
			m_arrParaData[nItem].nParaValue = nCurrent;
			CString strNew;
			strNew.Format(_T("%s%s"),strChange,(CString)m_arrParaData[nItem].differData.dataValue.strUnit);
			m_listPara.SetEdit(nItem,nSubItem,strNew,RGB(0,0,0),RGB(255,255,255));
		}
	} 
	else
	{
		CString str=m_listPara.GetItemText(nItem,nSubItem);
		wchar_t strNum[10];
		m_listPara.GetItemText(nItem,0,strNum,sizeof(strNum));
		int nNum;
		swscanf(strNum,_T("%d"),&nNum);
		nNum = m_nCurIndex + nNum -1;
		double tempD1,dMax,dMin;
		swscanf(str,_T("%lf"),&tempD1);
		dMax=GetParaDataValue(m_arrParaData[nNum].nParaMaxValue,
			m_arrParaData[nNum].differData.dataValue.nDecBitNum);
		dMin=GetParaDataValue(m_arrParaData[nNum].nParaMinValue,
			m_arrParaData[nNum].differData.dataValue.nDecBitNum);
		if(tempD1 >= dMin && tempD1 <= dMax)
		{
			m_arrParaData[nNum].nParaValue = SetParaDataValue(tempD1,
				m_arrParaData[nNum].differData.dataValue.nDecBitNum);
			CString newStr = GetParaDataValueString(m_arrParaData[nNum]);
			m_listPara.SetEdit(nItem,nSubItem,newStr,RGB(0,0,0),RGB(255,255,255));
		}
		else
		{
			CString oldStr = GetParaDataValueString(m_arrParaData[nNum]);
			CString formateStr;
			formateStr.LoadString( IDS_OVERRANGE_INFO );
			str.Format(formateStr, dMin,dMax,tempD1);
			CdMessageBox box(this, str);
			box.MsgBoxEx();
			m_listPara.SetEdit(nItem,nSubItem,oldStr,RGB(0,0,0),RGB(255,255,255));
		}
	}
	return 0;
}
LRESULT CParaView::OnComboxSelection(WPARAM nItem, LPARAM nSubItem)
{
	wchar_t selectStr[33];
	m_listPara.GetItemText(nItem,nSubItem,selectStr,sizeof(selectStr));
	unsigned short i;
	wchar_t strNum[10];
	m_listPara.GetItemText(nItem,0,strNum,sizeof(strNum));
	int nNum;
	swscanf(strNum,_T("%d"),&nNum);
	nNum = m_nCurIndex + nNum - 1;
	char tempSelectStr[255];
	WideCharToMultiByte(CP_ACP,0,selectStr,-1,tempSelectStr,255,NULL,NULL);
	for(i=m_arrParaData[nNum].nParaMinValue;i<=m_arrParaData[nNum].nParaMaxValue;i++)
	{
		
		if(!strcmp(tempSelectStr,m_arrParaData[nNum].differData.arrStrOption[i]))
		{
			break;
		}
	}
	ASSERT(i<=m_arrParaData[nNum].nParaMaxValue);
	m_arrParaData[nNum].nParaValue = i;
	return 0;
}
double CParaView::GetParaDataValue(unsigned short nValue,unsigned short nDecBit)
{
	double re;
	re=(double)nValue;
	unsigned short i;
	for(i=0;i<nDecBit;i++)
	{
		re /= 10;
	}
	return re;
}

unsigned short CParaView::SetParaDataValue(double dValue, unsigned short nDecBit)
{
	unsigned short re,i;
	for(i=0;i<nDecBit;i++)
	{
		dValue *= 10;
	}
	re = (unsigned short)dValue;
	return re;
}

CString CParaView::GetParaDataValueString(ParaData &paraData)
{
	ASSERT(paraData.nTranFormat == PARA_TYPE_DATA);
	CString str;
	double d1;	
	d1 = GetParaDataValue(paraData.nParaValue,
		paraData.differData.dataValue.nDecBitNum);
	CString formatStr;
	formatStr.Format(_T("%%.%dlf"),paraData.differData.dataValue.nDecBitNum);
	str.Format(formatStr,d1);
	int lenStr;
	if(paraData.differData.dataValue.nDecBitNum != 0)
	{
		lenStr=1 + paraData.differData.dataValue.nDecBitNum +
			paraData.differData.dataValue.nIntBitNum;
	}
	else
	{
		lenStr = paraData.differData.dataValue.nIntBitNum;
	}
	str=str.Right(lenStr);
	str += paraData.differData.dataValue.strUnit;
	return str;
}

void CParaView::LoadParaTypeData(CString strParaType)
{
	m_listPara.SetRedraw(FALSE);
	UINT i,j,index;
	CString str,strRange, info;
	m_listPara.DeleteAllItems();
	info.LoadString( IDS_ALL_TYPE );
	if( strParaType != info )
	{
		index=0;
		for(i=0;i<m_nParaCount;i++)
		{
			ASSERT(m_arrParaData[i].nParaNum == i);
			if(strParaType != m_arrParaData[i].strTypeName)
			{
				continue;
			}
			str.Format(_T("%u"),index+1);
			m_listPara.InsertItem(index,str);
			m_listPara.SetItemText(index,1,(CString)m_arrParaData[i].strTypeName);
			m_listPara.SetItemText(index,2,(CString)m_arrParaData[i].strPareName);
			switch (m_arrParaData[i].nTranFormat)
			{
			case PARA_TYPE_DATA:
				{
					str = GetParaDataValueString(m_arrParaData[i]);
					m_listPara.SetEdit(index,3,str,RGB(0,0,0),RGB(255,255,255));
					double dMax,dMin;
					dMax=GetParaDataValue(m_arrParaData[i].nParaMaxValue,
						m_arrParaData[i].differData.dataValue.nDecBitNum);
					dMin=GetParaDataValue(m_arrParaData[i].nParaMinValue,
						m_arrParaData[i].differData.dataValue.nDecBitNum);
					info.LoadString( IDS_DATA_RANGE );
				    info += _T(": %.3lf%s——%.3lf%s");
					str.Format( info,
						dMin,(CString)m_arrParaData[i].differData.dataValue.strUnit,
						dMax,(CString)m_arrParaData[i].differData.dataValue.strUnit);
					str.Format(_T("%.3lf%s——%.3lf%s"),
						dMin,(CString)m_arrParaData[i].differData.dataValue.strUnit,
						dMax,(CString)m_arrParaData[i].differData.dataValue.strUnit);
					
					m_listPara.SetItemText(index,4,str,RGB(0,0,0),RGB(255,255,255));
					break;
				}
			case PARA_TYPE_OPTION:
				{
					CStringArray strItem;
					strRange = _T("");
					for(j=m_arrParaData[i].nParaMinValue;j<=m_arrParaData[i].nParaMaxValue;j++)
					{
						strItem.Add(m_arrParaData[i].differData.arrStrOption[j]);
						strRange += (CString)m_arrParaData[i].differData.arrStrOption[j]+_T("、");
						
					}
					strRange = strRange.Left(strRange.ReverseFind(TEXT('、')));
					m_listPara.SetComboBox(index,3,TRUE,&strItem,3,
						m_arrParaData[i].nParaValue-m_arrParaData[i].nParaMinValue);
					m_listPara.SetItemText(index,4,strRange);
					m_listPara.SetItemToolTipText(index,4,strRange);
					break;
				}
			}
			index++;
		}
	}
	else
	{
		index = 0;
		for(i=0;i<m_nParaCount;i++)
		{
			ASSERT(m_arrParaData[i].nParaNum == i);
			str.Format(_T("%u"),index+1);
			m_listPara.InsertItem(index,str);
			m_listPara.SetItemText(index,1,(CString)m_arrParaData[i].strTypeName);
			m_listPara.SetItemText(index,2,(CString)m_arrParaData[i].strPareName);
			switch (m_arrParaData[i].nTranFormat)
			{
			case PARA_TYPE_DATA:
				{
					str = GetParaDataValueString(m_arrParaData[i]);
					m_listPara.SetEdit(index,3,str,RGB(0,0,0),RGB(255,255,255));
					double dMax,dMin;
					dMax=GetParaDataValue(m_arrParaData[i].nParaMaxValue,
						m_arrParaData[i].differData.dataValue.nDecBitNum);
					dMin=GetParaDataValue(m_arrParaData[i].nParaMinValue,
						m_arrParaData[i].differData.dataValue.nDecBitNum);
					info.LoadString( IDS_DATA_RANGE );
					info += _T(": %.3lf%s——%.3lf%s");
					str.Format(info,
						dMin,(CString)m_arrParaData[i].differData.dataValue.strUnit,
						dMax,(CString)m_arrParaData[i].differData.dataValue.strUnit);
					str.Format(_T("%.3lf%s——%.3lf%s"),
						dMin,(CString)m_arrParaData[i].differData.dataValue.strUnit,
						dMax,(CString)m_arrParaData[i].differData.dataValue.strUnit);
					
					m_listPara.SetItemText(index,4,str,RGB(0,0,0),RGB(255,255,255));
					break;
				}
			case PARA_TYPE_OPTION:
				{
					CStringArray strItem;
					strRange = _T("");
					for(j=m_arrParaData[i].nParaMinValue;j<=m_arrParaData[i].nParaMaxValue;j++)
					{
						strItem.Add(m_arrParaData[i].differData.arrStrOption[j]);
						strRange += (CString)m_arrParaData[i].differData.arrStrOption[j]+_T("、");
						
					}
					strRange = strRange.Left(strRange.ReverseFind(TEXT('、')));
					m_listPara.SetComboBox(index,3,TRUE,&strItem,3,
						m_arrParaData[i].nParaValue-m_arrParaData[i].nParaMinValue);
					m_listPara.SetItemText(index,4,strRange);
					m_listPara.SetItemToolTipText(index,4,strRange);
					break;
				}
			}
			index++;
		}
	}
	m_listPara.SetRedraw(TRUE);
}

void CParaView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(m_listPara.GetSafeHwnd())
	{
		m_listPara.SetWindowPos(   
			this,   10,   120,   
			cx-20,   cy-130,     
			SWP_NOOWNERZORDER   |   SWP_NOZORDER   |   SWP_SHOWWINDOW   );   
	}
}

/**************************************************************
  函数：OnReadPara
  描述；读取装置定值（视图按钮的响应函数）
  参数：
  返回：
  修改记录：
**************************************************************/
void CParaView::OnReadPara() 
{
	// TODO: Add your control notification handler code here
//	CPWDDlg dlg;
//	dlg.DoModal();
	CString str, sysTypeStr;
	str = theApp.m_projectData.m_strDeviceType;
	if (str != DVI_DEVICE)
	{
		if (!CFrame::GetParaArea(m_nCurParaArea,m_nParaAreaCount))
		{
			((CMainFrame*)GetParentFrame())->StopCommunication();
			str.LoadString( IDS_CONNECT_ERROR );
			CdMessageBox box( this, str);
			box.MsgBoxEx();
			return;
		}
	}
	else
	{
		m_nParaAreaCount = 0;
	}
	m_nCurParaArea = 0xff;
	m_ctlParaArea.ResetContent();
	m_ctlSetArea.ResetContent();
	sysTypeStr.LoadString( IDS_SYS_SETPOINT );
	m_ctlParaArea.AddString( sysTypeStr );
	//2018-11-12	CZZ
	m_ctlSetArea.InsertString(0,sysTypeStr);
	BYTE i;
	for(i=0;i<m_nParaAreaCount;i++)
	{
		str.Format(_T("%d"),i + 1);
		m_ctlParaArea.AddString(str);
		//
		m_ctlSetArea.InsertString(i+1,str);
	}
	m_ctlSetArea.SetCurSel(1);
	m_ctlParaArea.SelectString(0, sysTypeStr);
	m_nDisParaArea=m_nCurParaArea;
//	GetDlgItem(IDC_SET_CUR_PARA_AREA_BTN)->EnableWindow(FALSE);
	LoadData();
	GetDlgItem(IDC_SET_PARA_BTN)->EnableWindow(TRUE);
}

void CParaView::OnReadParaFile() 
{
	// TODO: Add your control notification handler code here
	CString path;
	CString tempStr, info;
//	int channelNum;
	CFileDialog dlg(TRUE,NULL,_T("请选择文件"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("定值文件(*.set)|*.set|表格文件(*.xls)|*xls||"));
	if(IDOK==dlg.DoModal())
	{
		CProjectData project;
		path=dlg.GetPathName();
	}
	else
	{
		return;
	}
	//2018-11-12 CZZ
	CString	strFileExt = dlg.GetFileExt();
	if (strFileExt.Find(_T("xls"),0) != -1)
	{
		if (!OnAnalyseExcelDate(path))
		{
			AfxMessageBox(_T("解析错误"));
			return;
		}
		m_ctlParaArea.ResetContent();
		m_bIputExcelFile = TRUE;
		GetDlgItem(IDC_SET_PARA_BTN)->EnableWindow(TRUE);
	}
	else if (strFileExt.Find(_T("set"),0) != -1)
	{
		m_bIputExcelFile = FALSE;
		tempStr = path.Right(path.GetLength()-path.ReverseFind(TEXT('_'))-1);
		tempStr = tempStr.Left(tempStr.ReverseFind(TEXT('.')));
		// 		sscanf(tempStr,"%d.set",&channelNum);
		// 		tempStr.Format(_T("%d"),channelNum);
		m_ctlParaArea.ResetContent();
		m_ctlParaArea.InsertString(0,tempStr);
		m_ctlParaArea.SelectString(0,tempStr);
		FILE *dataFile;
		dataFile=_wfopen(path,_T("rb"));
		if(dataFile==NULL)
		{
			tempStr.LoadString( IDS_OPENFILE_ERROR );
			CdMessageBox box( this, tempStr);
			box.MsgBoxEx();
			return;
		}
		fseek(dataFile,0,SEEK_END);
		int filelength=ftell(dataFile);
		if (filelength<70)
		{
			tempStr.LoadString( IDS_FILELEN_TOOSHORT );
			CdMessageBox box( this, tempStr);
			box.MsgBoxEx();
			fclose(dataFile);
			return;
		}
		BYTE *fileBuf=new BYTE[filelength+10];
		memset(fileBuf,0,filelength+10);
		fseek(dataFile,0,SEEK_SET);
		int temp=fread(fileBuf,sizeof(BYTE),filelength+10,dataFile);
		fclose(dataFile);
		if(filelength!=temp)
		{
			delete [] fileBuf;
			return;
		}
 		m_listPara.DeleteAllItems();
 		ASSERT(m_listPara.GetItemCount() == 0);
	//	GetDlgItem(IDC_SET_CUR_PARA_AREA_BTN)->EnableWindow(FALSE);
		GetDlgItem(IDC_SET_PARA_BTN)->EnableWindow(TRUE);
		m_ctlParaType.ResetContent();
		tempStr.LoadString( IDS_ALL_TYPE );
		m_ctlParaType.AddString( tempStr );
		int curPos=0,paraNum,i=0,nTranFormat;
		CString str,strFormat,strType[20];
		paraNum = fileBuf[curPos] + (int)(fileBuf[curPos+1]<<8);
		curPos +=2 ;
		while(paraNum == i)
		{
			if (i>60)
			{
				str = tempStr;
			}
			memset(&m_arrParaData[i],0,sizeof(&m_arrParaData[i]));
			m_arrParaData[i].nParaNum = i;
			str.Format(_T("%d"),i+1);
			m_listPara.InsertItem(i,str);
			nTranFormat = fileBuf[curPos];
			m_arrParaData[i].nTranFormat = nTranFormat;
			curPos++;
			memcpy(m_arrParaData[i].strTypeName,fileBuf+curPos,20);
			curPos += 20;
			str=m_arrParaData[i].strTypeName;
			m_listPara.SetItemText(i,1,str);
			CompareParaType(strType,str);
			memcpy(m_arrParaData[i].strPareName,fileBuf+curPos,32);
			curPos += 32;
			str=m_arrParaData[i].strPareName;
			m_listPara.SetItemText(i,2,str);
			m_arrParaData[i].nParaValue = fileBuf[curPos] +(unsigned short)(fileBuf[curPos+1]<<8);
			curPos += 2;
			m_arrParaData[i].nParaMaxValue = fileBuf[curPos] +(unsigned short)(fileBuf[curPos+1]<<8);
			curPos += 2;
			m_arrParaData[i].nParaMinValue = fileBuf[curPos] +(unsigned short)(fileBuf[curPos+1]<<8);
			curPos += 2;

			switch (nTranFormat)
			{
			case PARA_TYPE_DATA:
				{
					m_arrParaData[i].differData.dataValue.nIntBitNum = fileBuf[curPos];
					curPos++;
					m_arrParaData[i].differData.dataValue.nDecBitNum = fileBuf[curPos];
					curPos++;
					memcpy(m_arrParaData[i].differData.dataValue.strUnit,fileBuf + curPos,8);
					curPos += 8;

					str = GetParaDataValueString(m_arrParaData[i]);
					m_listPara.SetEdit(i,3,str,RGB(0,0,0),RGB(255,255,255));
					double dMax,dMin;
					dMax=GetParaDataValue(m_arrParaData[i].nParaMaxValue,
						m_arrParaData[i].differData.dataValue.nDecBitNum);
					dMin=GetParaDataValue(m_arrParaData[i].nParaMinValue,
						m_arrParaData[i].differData.dataValue.nDecBitNum);
					info.LoadString( IDS_DATA_RANGE );
					info += _T(": %%.%dlf%%s——%%.%dlf%%s");
					strFormat.Format( info, m_arrParaData[i].differData.dataValue.nDecBitNum,
						m_arrParaData[i].differData.dataValue.nDecBitNum);
					str.Format(strFormat,
						dMin,(CString)m_arrParaData[i].differData.dataValue.strUnit,
						dMax,(CString)m_arrParaData[i].differData.dataValue.strUnit);
					strFormat.Format(_T("%%.%dlf%%s——%%.%dlf%%s"),m_arrParaData[i].differData.dataValue.nDecBitNum,
						m_arrParaData[i].differData.dataValue.nDecBitNum);
					str.Format(strFormat,
						dMin,(CString)m_arrParaData[i].differData.dataValue.strUnit,
						dMax,(CString)m_arrParaData[i].differData.dataValue.strUnit);
					
					m_listPara.SetItemText(i,4,str,RGB(0,0,0),RGB(255,255,255));
					break;
				}
			case PARA_TYPE_OPTION:
				{
					ASSERT(m_arrParaData[i].nParaMaxValue - m_arrParaData[i].nParaMinValue <=15);
					int inter=0;
					for(int j=m_arrParaData[i].nParaMinValue;j<=m_arrParaData[i].nParaMaxValue;j++)
					{
						memcpy(m_arrParaData[i].differData.arrStrOption[j],fileBuf+curPos+inter,32);
						inter += 32;
					}
					
					CStringArray strItem;
					str = _T("");
					for(j=m_arrParaData[i].nParaMinValue;j<=m_arrParaData[i].nParaMaxValue;j++)
					{
						strItem.Add(m_arrParaData[i].differData.arrStrOption[j]);
						str += (CString)m_arrParaData[i].differData.arrStrOption[j]+_T("、");
						
					}
					str = str.Left(str.ReverseFind(TEXT('、')));
					m_listPara.SetComboBox(i,3,TRUE,&strItem,3,
						m_arrParaData[i].nParaValue-m_arrParaData[i].nParaMinValue);
					m_listPara.SetItemText(i,4,str);
						m_listPara.SetItemToolTipText(i,4,str);
					curPos+=512;
					break;
				}
			}
			m_nParaCount = paraNum+1;
			i++;
			paraNum = fileBuf[curPos] + (int)(fileBuf[curPos+1]<<8);
			curPos +=2 ;
		}
	}
	else
	{
		AfxMessageBox(_T("选择文件格式出错，应该是*.xls或者*.set文件！"));
		return;
	}
}

void CParaView::CompareParaType(CString *strArray, CString str)
{
	BOOL flag = FALSE;
	if (typeCount==0)
	{			
		strArray[typeCount]=str;
		m_ctlParaType.AddString(str);
		typeCount++;
	}
	else
	{
		for (int j=0;j<typeCount;j++)
		{
			if(strArray[j]!=str)
			{
				flag = TRUE;
			}
			else
			{
				flag = FALSE;
			}
		}
		if (flag)
		{
			strArray[typeCount]=str;
			m_ctlParaType.AddString(str);
			typeCount++;
		}
	}
}

VOID CParaView::OnDeviceReset() 
{
	// TODO: Add your control notification handler code here
	
	CPWDDlg dlg;
	CString strParaType;
	CString str;
	while(IDOK==dlg.DoModal())
	{
		if(dlg.m_bCorrect)
		{
			BYTE ret = CFrame::DeviceReset();
			if ( FALSE == ret )
			{
				((CMainFrame*)GetParentFrame())->StopCommunication();
				str.LoadString( IDS_CONNECT_ERROR );
				CdMessageBox box( this, str);
				box.MsgBoxEx();
			}else if ( OPERATION_RESULT_SUCCESS == ret)
			{
				((CMainFrame*)GetParentFrame())->StopCommunication();
				str.LoadString( IDS_RESET_SETTING );
				CdMessageBox box( this, str);
		        box.MsgBoxEx();
			}else
			{
				CString errorInfo = GetErrorInfo( ret );
				if ( errorInfo.GetLength() > 0 )
				{
                      str.LoadString( IDS_RESET_FAILURE );
					  str += _T(",") + errorInfo;
					  CdMessageBox box( this, str);
		              box.MsgBoxEx();
				}
			}
			break;
		}
	}
}
VOID CParaView::ReadDevicePara()
{
	OnReadPara();
}

VOID CParaView::ReadParaFile()
{
	OnReadParaFile();
}

//2018-11-12 CZZ
//解析选中Excel文件中的数据
BOOL CParaView::OnAnalyseExcelDate(CString strExcelPath)
{
	//添加表格处理标量
	_Application	Myapp;
	Workbooks		Mybooks;
	Sheets			Mysheets;
	_Workbook		Mybook;
	_Worksheet		Mysheet;
	Range			Myrange;
	LPDISPATCH		MyDispatch;

	//打开Excel应用
	if (!Myapp.CreateDispatch(_T("Excel.Application")))
	{
		return FALSE;
	}
	Mybooks = Myapp.GetWorkbooks();
	//打开Excel
	COleVariant	MyOlevariant((long)DISP_E_PARAMNOTFOUND,VT_ERROR);
	MyDispatch = Mybooks.Open(strExcelPath,MyOlevariant,MyOlevariant,MyOlevariant,MyOlevariant
		,MyOlevariant,MyOlevariant,MyOlevariant,MyOlevariant,MyOlevariant
		,MyOlevariant,MyOlevariant,MyOlevariant,MyOlevariant,MyOlevariant);
	Mybook.AttachDispatch(MyDispatch);
	//获取里面的表的数目
	Mysheets = Mybook.GetWorksheets();
	int	nSheetCount = Mysheets.GetCount();
	int nIndex = 1;
	CString		strSheetName(_T(""));
	for (nIndex; nIndex <= nSheetCount ;nIndex++)					//完成表的锁定
	{
		Mysheet = Mysheets.GetItem(COleVariant((short)nIndex));
		strSheetName = Mysheet.GetName();
		if (strSheetName.Find(_T("所有类型"),0) != -1)
		{
			break;
		}
	}
	if (nIndex > nSheetCount)
	{
		return FALSE;
	}
	//清空原来的列表
	m_listPara.DeleteAllItems();
	for(int i=m_ctlParaType.GetCount()-1;((short)i)>=0;i--)	//清空类型
	{
		m_ctlParaType.DeleteString(i);
	}
	m_nParaCount = 0;
	//清空原来的数据缓存空间
	memset(m_arrParaData,0,sizeof(struct ParaData) * MAX_PARA_COUNT);
	//准备拿取数据
	CString		strCurrentPos,strRow;
	BOOL	bExitFlag = TRUE,bExitParaType;
	CStringArray	strArrayParaDate;
	for (int nRowIndex = 2;;nRowIndex++)
	{
		strArrayParaDate.RemoveAll();
		bExitFlag = TRUE;
		strRow.Format(_T("%d"),nRowIndex-1);
		for (int nColumn = 1;nColumn <= 5 ;nColumn++)
		{
			OnIntToAlphater(nRowIndex,nColumn,strCurrentPos);
			Myrange = Mysheet.GetRange(COleVariant(strCurrentPos),COleVariant(strCurrentPos));
			//数据转换
			COleVariant	rValue = COleVariant(Myrange.GetValue2());
			rValue.ChangeType(VT_BSTR);
			if ((CString)rValue.bstrVal == _T("") && nColumn == 1)
			{
				bExitFlag = FALSE;
				break;
			}
			strArrayParaDate.Add((CString)rValue.bstrVal);
			if ((CString)rValue.bstrVal != _T("") && nColumn == 1 && nRowIndex == 2)
			{
				m_ctlParaType.InsertString(0,_T("所有类型"));
 				m_ctlParaType.SetCurSel(0);
//  				m_ctlParaArea.InsertString(0,_T("1"));
// 				m_ctlParaArea.SetCurSel(0);
			}
			//统计定值类型有哪些
			if (nColumn == 2)
			{
				bExitParaType = FALSE;
				for (int nParaCount = 0; nParaCount < m_ctlParaType.GetCount() ;nParaCount++)
				{
					CString	strPara(_T(""));
					m_ctlParaType.GetLBText(nParaCount,strPara);
					if (strPara == (CString)rValue.bstrVal)
					{
						bExitParaType = TRUE;
						break;
					}
				}
				if (!bExitParaType)
				{
					m_ctlParaType.InsertString(nParaCount,(CString)rValue.bstrVal);
				}
			}
		}
		if (!bExitFlag)
		{
			break;
		}
		OnShowExcelDate(&strArrayParaDate,nRowIndex - 2);
	}
	Mybook.SetSaved(TRUE);
	Mybook.DetachDispatch();
	Mybooks.Close();
	Myapp.Quit();
	return TRUE;
}

//2018-11-12 CZZ
void CParaView::OnIntToAlphater( int nRow,int nColumn,CString &strCurrntRow )
{
	strCurrntRow.Format(_T("%c%d"),nColumn+64,nRow);
}

//2018-11-13 CZZ
void CParaView::OnCbnSelchangeComboSetarea()
{
	// TODO: Add your control notification handler code here
	wchar_t strIndex[20];
	m_ctlSetArea.GetLBText(m_ctlSetArea.GetCurSel(),strIndex);
	CString str, typeStr ;
	str = (CString)strIndex;
	typeStr.LoadString( IDS_SYS_SETPOINT );
	if (str == typeStr)			//如果是系统地址
	{
		//		GetDlgItem(IDC_SET_CUR_PARA_AREA_BTN)->EnableWindow(FALSE);
		m_nSetParaArea = 0xFF;
	}
	else
	{
		int index;
		swscanf(strIndex,_T("%d"),&index);
		m_nSetParaArea = index - 1;
	}
	this->m_listPara.SetFocus();
}
//2018-11-13 CZZ 显示表格中的数据
void CParaView::OnShowExcelDate( CStringArray *arrCurrentParaDate,int nRow )
{
	m_nParaCount++;
	CString   strIndex;
	strIndex.Format(_T("%d"),nRow + 1);
	m_listPara.InsertItem(nRow,strIndex);
	m_listPara.SetItemText(nRow,1,arrCurrentParaDate->GetAt(1));
	m_listPara.SetItemText(nRow,2,arrCurrentParaDate->GetAt(2));
	m_listPara.SetItemText(nRow,4,arrCurrentParaDate->GetAt(4));
	m_arrParaData[nRow].nParaNum = nRow;
	WideCharToMultiByte(CP_ACP,0,arrCurrentParaDate->GetAt(2),-1,m_arrParaData[nRow].strPareName,sizeof(m_arrParaData[nRow].strPareName),0,0);		//定值名称
	WideCharToMultiByte(CP_ACP,0,arrCurrentParaDate->GetAt(1),-1,m_arrParaData[nRow].strTypeName,sizeof(m_arrParaData[nRow].strTypeName),0,0);		//定值类型
	//判断是选项型还是数值型数据
	CString  strParaValue = arrCurrentParaDate->GetAt(4);
	CStringArray	arrOptString;
	if (strParaValue.Find(_T("、"),0) != -1)			//选项型数据
	{
		m_arrParaData[nRow].nTranFormat = PARA_TYPE_OPTION;
		arrOptString.RemoveAll();
		m_arrParaData[nRow].nParaMinValue = 0;
		int nPos = strParaValue.Find(_T("、"),0);
		int nParaOptCount = 0;
		while (nPos != -1)
		{
			WideCharToMultiByte(CP_ACP,0,strParaValue.Left(nPos),-1,m_arrParaData[nRow].differData.arrStrOption[nParaOptCount],\
				sizeof(m_arrParaData[nRow].differData.arrStrOption[nParaOptCount]),0,0);
			if (strParaValue.Left(nPos) == arrCurrentParaDate->GetAt(2))
			{
				m_arrParaData[nRow].nParaValue = nParaOptCount;
			}
			nParaOptCount++;
			arrOptString.Add(strParaValue.Left(nPos));
			strParaValue = strParaValue.Right(strParaValue.GetLength() - 1 - nPos);
			nPos = strParaValue.Find(_T("、"),0);
			if (nPos == -1)
			{
				WideCharToMultiByte(CP_ACP,0,strParaValue,-1,m_arrParaData[nRow].differData.arrStrOption[nParaOptCount],\
					sizeof(m_arrParaData[nRow].differData.arrStrOption[nParaOptCount]),0,0);
				arrOptString.Add(strParaValue);
				if (strParaValue.Left(nPos) == arrCurrentParaDate->GetAt(2))
				{
					m_arrParaData[nRow].nParaValue = nParaOptCount;
				}
			}
		}
		m_arrParaData[nRow].nParaMaxValue = nParaOptCount;
		m_listPara.SetComboBox(nRow,3,TRUE,&arrOptString,3,m_arrParaData[nRow].nParaValue - m_arrParaData[nRow].nParaMinValue);
	}
	else if (strParaValue.Find(_T("——"),0) != -1)		//数值型数据
	{
		m_arrParaData[nRow].nTranFormat = PARA_TYPE_DATA;
		CString   strValue = arrCurrentParaDate->GetAt(3);
		int nStopPos = 0;
		while (strValue.GetAt(nStopPos) == TEXT('.') || (strValue.GetAt(nStopPos) >= '0' &&  strValue.GetAt(nStopPos) <= '9' ))
		{
			nStopPos++;
			if (nStopPos == strValue.GetLength())
			{
				break;
			}
		}
		if (nStopPos < strValue.GetLength())
		{
			//获取单位名称
			WideCharToMultiByte(CP_ACP,0,strValue.Right(strValue.GetLength() - nStopPos),-1,m_arrParaData[nRow].differData.dataValue.strUnit,\
						sizeof(m_arrParaData[nRow].differData.dataValue.strUnit),0,0);
		}
		//获取到当前的值
		CString strPara = strValue.Left(nStopPos);
		m_listPara.SetEdit(nRow,3,arrCurrentParaDate->GetAt(3),RGB(0,0,0),RGB(255,255,255));
		//最大值、最小值
		char	BuffParaValue[64] = {0};
		CString strUnit,strMin,strMax;
		int nMinPos; 
		//是否存在单位求出最大值最小值
		if (strlen(m_arrParaData[nRow].differData.dataValue.strUnit) > 0)
		{
			//求出小数点数目
			nMinPos = strParaValue.Find(_T("——"),0);
			strMax = strParaValue.Right(strParaValue.GetLength() - nMinPos - 2);
			strUnit = (CString)m_arrParaData[nRow].differData.dataValue.strUnit;
			strMax.Replace(strUnit,_T(""));
			if (strMax.Find(_T("."),0) == -1)
			{
				m_arrParaData[nRow].differData.dataValue.nIntBitNum = 0;
			} 
			else
			{
				m_arrParaData[nRow].differData.dataValue.nIntBitNum = strMax.GetLength() - strMax.Find(_T("."),0) - 1;
			}
			//最大值
			m_arrParaData[nRow].nParaMaxValue = OnCalculateParaValue(strMax,m_arrParaData[nRow].differData.dataValue.nIntBitNum);
			//求出最小值
			strMin = strParaValue.Left(nMinPos);
			strMin.Replace(strUnit,_T(""));
			m_arrParaData[nRow].nParaMinValue = OnCalculateParaValue(strMin,m_arrParaData[nRow].differData.dataValue.nIntBitNum);
		}
		else
		{
			nMinPos = strParaValue.Find(_T("——"),0);
			strMax = strParaValue.Right(strParaValue.GetLength() - nMinPos - 2);
			if (strMax.Find(_T("."),0) == -1)
			{
				m_arrParaData[nRow].differData.dataValue.nIntBitNum = 0;
			} 
			else
			{
				m_arrParaData[nRow].differData.dataValue.nIntBitNum = strMax.GetLength() - strMax.Find(_T("."),0) - 1;
			}
			//最大值
			m_arrParaData[nRow].nParaMaxValue = OnCalculateParaValue(strMax,m_arrParaData[nRow].differData.dataValue.nIntBitNum);
			//求出最小值
			strMin = strParaValue.Left(nMinPos);
			m_arrParaData[nRow].nParaMinValue = OnCalculateParaValue(strMin,m_arrParaData[nRow].differData.dataValue.nIntBitNum);

		}
		m_arrParaData[nRow].nParaValue = OnCalculateParaValue(strPara,m_arrParaData[nRow].differData.dataValue.nIntBitNum);
	}
	else
	{
		AfxMessageBox(_T("数据异常"));
	}
}
//计算出对应的定值的值
int CParaView::OnCalculateParaValue( CString strParavalue,int nPointCount )
{
	char	Buff[32] = {0};
	WideCharToMultiByte(CP_ACP,0,strParavalue,-1,Buff,sizeof(Buff),0,0);
	double  dParavalue = atof(Buff);
	for (int i = 0; i < nPointCount ;i++)
	{
		dParavalue *= 10;
	}
	return (int)dParavalue;
}

void CParaView::OnCalulateString( int nRow,int nColumn )
{
	double	nMax = m_arrParaData[nRow].nParaMaxValue,
		nMin = m_arrParaData[nRow].nParaMinValue,
		nCurrentValue = m_arrParaData[nRow].nParaValue;
	for (int i = 0; i < m_arrParaData[nRow].differData.dataValue.nIntBitNum ; i++)
	{
		nMax /= 10;
		nMin /= 10;
		nCurrentValue /= 10;
	}
	CString	strOutFormat;
	strOutFormat.Format(_T("%%.%df%s"),m_arrParaData[nRow].differData.dataValue.nIntBitNum,(CString)m_arrParaData[nRow].differData.dataValue.strUnit);
	CString strOldParaValue;
	strOldParaValue.Format(strOutFormat,nCurrentValue);
	m_listPara.SetEdit(nRow,nColumn,strOldParaValue,RGB(0,0,0),RGB(255,255,255));
	CString  strErr,strErrFormat;
	strErrFormat.Format(_T("%%.%df——%%.%df"),m_arrParaData[nRow].differData.dataValue.nIntBitNum,m_arrParaData[nRow].differData.dataValue.nIntBitNum);
	strErr.Format(strErrFormat,nMin,nMax);
}