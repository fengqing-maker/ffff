// EventView.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "EventView.h"
#include "EventProgressDlg.h"
#include "mainfrm.h"
#include "Frame.h"
#include "CustomFileDialog.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventView
extern CMySID_2FYApp theApp;
IMPLEMENT_DYNCREATE(CEventView, CFormView)
CEventView::CEventView()
	: CFormView(CEventView::IDD)
{
	//{{AFX_DATA_INIT(CEventView)
	m_timeEnd = COleDateTime::GetCurrentTime();
	m_timeStart = COleDateTime::GetCurrentTime();
	m_strEventName = _T("");
	m_strOrderBy = _T("HappenTime DESC,Millisecond DESC");
	m_lSortedColumn = -1;
	//}}AFX_DATA_INIT
}

CEventView::~CEventView()
{
}

void CEventView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventView)
	DDX_Control(pDX, IDC_EVENT_COMBO, m_ctlEventType);
	DDX_Control(pDX, IDC_EVENT_LIST, m_listEvent);
	DDX_DateTimeCtrl(pDX, IDC_END_TIME, m_timeEnd);
	DDX_DateTimeCtrl(pDX, IDC_START_TIME, m_timeStart);
	DDX_Text(pDX, IDC_EVENT_NAME_EDIT, m_strEventName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventView, CFormView)
	//{{AFX_MSG_MAP(CEventView)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SORT_BTN, OnSortBtn)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_EVENT_LIST, OnRclickEventList)
	ON_COMMAND(ID_EVENT_DELETE,OnEventDelete)
	ON_COMMAND(ID_EVENT_DELETE_ALL,OnEventDeleteAll)
	ON_COMMAND(ID_EVENT_OUT_TO_EXCEL,OnEventOutToExcel)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_EVENT_LIST, OnColumnclickEventList)
	ON_BN_CLICKED(IDC_INPUT_EVENT, OnInputEvent)
	ON_BN_CLICKED(IDC_GET_EVENT, OnGetEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventView diagnostics

#ifdef _DEBUG
void CEventView::AssertValid() const
{
	CFormView::AssertValid();
}

void CEventView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEventView message handlers

void CEventView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	GetDlgItem(IDC_SORT_BTN)->EnableWindow(TRUE);
	// TODO: Add your specialized code here and/or call the base class
//	theApp.m_pLangManger->SetWndStrings(this, IDD);
	CString sql,str, info;
	COleDateTimeSpan timeSpan(31,0,0,0);
	m_timeStart = COleDateTime::GetCurrentTime() - timeSpan;
	UpdateEventType();
    info.LoadString( IDS_ALL_TYPE );
	m_ctlEventType.SelectString(0, info);
	m_imageList.Create(16,16,ILC_COLOR16|ILC_MASK,4,4);
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ARROW_UP));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ARROW_DOWN));
	CRect rc;
	m_listEvent.GetClientRect(&rc);
	m_listEvent.SetImageList(&m_imageList,LVSIL_SMALL);
	long width=rc.Width();
	m_listEvent.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	info.LoadString( IDS_SERIAL );
	m_listEvent.InsertColumn(0, info, LVCFMT_LEFT, 80);
	info.LoadString( IDS_EVENT_NAME );
	m_listEvent.InsertColumn(1, info,LVCFMT_LEFT,200);
	info.LoadString( IDS_EVENT_TYPE );
	m_listEvent.InsertColumn(2, info, LVCFMT_LEFT,150);
	info.LoadString( IDS_TIME_HAPPEN );
	m_listEvent.InsertColumn(3, info,LVCFMT_LEFT,200);
	info.LoadString( IDS_EVENT_PARA );
	m_listEvent.InsertColumn(4, info,LVCFMT_LEFT,width-650);
	m_ctlEventType.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
//	m_ctlEventType.MoveWindow(rc.left,rc.top,rc.Width(),300);
	QueryData();
	UpdateData(FALSE);
}


void CEventView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

/****************************************************************
功能：提取装置事件按钮响应函数
输入：无
输出：bool是否提取成功
作者：
*****************************************************************/
BOOL CEventView::GetDeviceEvent()
{
	CString strProgress;
	TCHAR sql[300];
	int i;
	int nEventTypeCount = 100;
	BYTE arrTypeIndexs[100];
	unsigned short arrEventCounts[100];
	//获取事件组成概览，arrTypeIndexs事件类型序号数组，arrEventCounts各个事件类型的最大事件个数
	if(!CFrame::GetEventSummary(arrTypeIndexs,arrEventCounts,nEventTypeCount))  
	{
		((CMainFrame*)GetParentFrame())->StopCommunication();
		CString str;
		str.LoadString( IDS_CONNECT_ERROR );
		CdMessageBox box( this, str);
	    box.MsgBoxEx();
	//test	return false;
	}
	GetDlgItem(IDC_SORT_BTN)->EnableWindow(TRUE);
	CEventProgressDlg dlg;
	if(IsWindow(this->GetSafeHwnd()))
	{
		dlg.Create(IDD_PROGRESS,this);
		dlg.ShowWindow(SW_SHOW);
	}
	else 
		return FALSE;
	
	for(i=0;i<nEventTypeCount;i++)
	{
		unsigned short nEventStartIndex=0;
		BYTE nEventCountOfFrame;
		CAdoRecordset record;
		//for执行一次取一种类型的所有事件
		do 
		{   //do一次取同一种类型nEventCountOfFrame个事件
			nEventCountOfFrame = CFrame::RecvEvent(arrTypeIndexs[i],nEventStartIndex,arrEventCounts[i]);
			if(nEventCountOfFrame == 0)
			{
				if (i<nEventTypeCount)
				{
					break;
				}
				UpdateEventType();
				return QueryData();
			}
			int j;
			UINT nFramePos=9;
			EventData eventData;
			for(j=0;j<nEventCountOfFrame;j++)
			{
				
				if(!CFrame::GetEventDataFromFrame(nFramePos,&eventData))
				{
					UpdateEventType();
					return QueryData();
				}
				swprintf(sql,_T("select * from event where EventName='%s' and EventType='%s' and DATEDIFF('s','%s',HappenTime)=0 and Millisecond=%d"),
					eventData.strEventName,eventData.strEvnetType,eventData.happentTime.Format(_T("%Y-%m-%d %H:%M:%S")),
					eventData.nMilliSecond);
				
				if(record.IsOpen())
				{
					record.Close();
				}
				if(record.Open(sql,theApp.m_adoDatabase,adLockOptimistic))
				{
					if(record.IsEOF())  //若此事件未在数据库中，就添加事件
					{
						record.AddNew();
						record[_T("Param")]=eventData.strExtendContent;
						record[_T("HappenTime")]=eventData.happentTime;
						record[_T("Millisecond")]=eventData.nMilliSecond;
						record[_T("EventType")]=eventData.strEvnetType;
						record[_T("EventName")]=eventData.strEventName;
						try
						{
							record.Update();
						}
						catch (...)
						{
							CString info;
							info.LoadString( IDS_ADDEVENT_ERROR );
							CdMessageBox box(this, info);
		                    box.MsgBoxEx();
						}
					}
				}
				
				CString info;
				info.LoadString( IDS_DOWN_NOW );
                strProgress.Format(info, eventData.strEvnetType);
// 				switch (j%4)
// 				{
// 				case 0:
// 					strProgress.Format(_T("正在提取：%s."),eventData.strEvnetType);
// 					break;
// 				case 1:
// 					strProgress.Format(_T("正在提取：%s.."),eventData.strEvnetType);
// 					break;
// 				case 2:
// 					strProgress.Format(_T("正在提取：%s..."),eventData.strEvnetType);
// 					break;
// 				case 3:
// 					strProgress.Format(_T("正在提取：%s...."),eventData.strEvnetType,j);
// 					break;
// 				}

				dlg.SetEvevntType(strProgress);
			}
			if(record.IsOpen())
			{
				record.Close();
			}
			nEventStartIndex+=nEventCountOfFrame;
		} while ((nEventCountOfFrame!=0) && (nEventStartIndex<arrEventCounts[i]));
	}
	UpdateEventType();
	return QueryData();
}

void CEventView::OnSortBtn() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	QueryData();
	UpdateData(FALSE);

}

/****************************************************************
功能：查询事件数据
输入：无
输出：bool是否提取成功
作者：
*****************************************************************/
BOOL CEventView::QueryData()
{
	m_listEvent.DeleteAllItems();
	ASSERT(m_listEvent.GetItemCount() == 0);
	CString str,strEventType,info;
	long i=0;
	long j=0;
	CString sql;
	m_ctlEventType.GetLBText(m_ctlEventType.GetCurSel(),strEventType);
	info.LoadString( IDS_ALL_TYPE );
	if(strEventType != info)
	{
		sql.Format(_T("select * from event where EventType='%s' and DATEDIFF('n','%s',HappenTime)>0 and DATEDIFF('n',HappenTime,'%s')>0 and EventName like '%%%s%%' order by %s"),
			strEventType,m_timeStart.Format(_T("%Y-%m-%d 0:0:0")),m_timeEnd.Format(_T("%Y-%m-%d 23:59:59")),
			m_strEventName,m_strOrderBy);
	}
	else
	{
		sql.Format(_T("select * from event where DATEDIFF('n','%s',HappenTime)>0 and DATEDIFF('n',HappenTime,'%s')>0 and EventName like '%%%s%%' order by %s"),
			m_timeStart.Format(_T("%Y-%m-%d 0:0:0")),m_timeEnd.Format(_T("%Y-%m-%d 23:59:59")),m_strEventName,m_strOrderBy);
	}
	if(m_recordset.IsOpen())
	{
		m_recordset.Close();
	}
	if(!m_recordset.Open(sql,theApp.m_adoDatabase))
	{
		CString info;
		info.LoadString( IDS_OPEN_DATAF );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return FALSE;
	}
	while(!m_recordset.IsEOF())
	{
		str.Format(_T("%d"),i+1);
		m_listEvent.InsertItem(LVIF_TEXT|LVIF_IMAGE,i,str,0,0,-1,NULL);
		m_listEvent.SetItemText(i,1,m_recordset[_T("EventName")].ToString());
		m_listEvent.SetItemText(i,2,m_recordset[_T("EventType")].ToString());
		str.Format(_T("%s.%s"),m_recordset[_T("HappenTime")].ToString(),m_recordset[_T("Millisecond")].ToString());
		m_listEvent.SetItemText(i,3,str);
		m_listEvent.SetItemText(i,4,m_recordset[_T("Param")].ToString());
		m_recordset.MoveNext();
		i++;
	}
	m_nEventCounts = i;
	return TRUE;
}

void CEventView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(m_listEvent.GetSafeHwnd())
	{
		m_listEvent.SetWindowPos(   
			this,   10,   120,   
			cx-20,   cy-130,     
  SWP_NOOWNERZORDER   |   SWP_NOZORDER   |   SWP_SHOWWINDOW   );   
	}
}

/*********************************************************************
函数名：OnRclickEventList
描述：右击事件列表的响应函数
参数：pNMHDR：选中的行，pResult
返回:
修改记录：
*********************************************************************/
void CEventView::OnRclickEventList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POSITION pos=m_listEvent.GetFirstSelectedItemPosition();
	if(pos==NULL) return;
	CMenu PopupMenu;
	PopupMenu.CreatePopupMenu();
	CPoint point;
	GetCursorPos(&point);
	CString info;
	info.LoadString( IDS_EXPORT_EVENT );
	PopupMenu.AppendMenu(MF_ENABLED, ID_EVENT_OUT_TO_EXCEL, info);	
	info.LoadString( IDS_DEL_EVENT );
	PopupMenu.AppendMenu(MF_ENABLED, ID_EVENT_DELETE, info);
	info.LoadString( IDS_DEL_ALL );
	PopupMenu.AppendMenu(MF_ENABLED, ID_EVENT_DELETE_ALL, info);	
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON , point.x, point.y, this);
	*pResult = 0;
}

/*********************************************************************
函数名：OnEventDelete
描述：删除选中事件
参数：
返回:
修改记录：
*********************************************************************/
void CEventView::OnEventDelete()
{
	CString info, title;
	info.LoadString( IDS_DELEEVENT_INFO );
	title.LoadString( IDS_STR_TITLE );
	CdMessageBox box(this, info, title, MB_YESNO);        ;
	if(IDNO == box.MsgBoxEx())
	{
		return;
	}
	CEventProgressDlg dlg;
	
	POSITION pos=m_listEvent.GetFirstSelectedItemPosition();
	ASSERT(pos!=NULL);
	int nSelectItemCount=0,nPos=0,i;
	while(pos)    //获取选中事件总数
	{
		nSelectItemCount++;
		m_listEvent.GetNextSelectedItem(pos);
	}
	if(nSelectItemCount >= 50)  //要删除事件大于50加进度条
	{ 
		if(IsWindow(this->GetSafeHwnd()))
		{
			dlg.Create(IDD_PROGRESS,this);
			dlg.ShowWindow(SW_SHOW);
			CString info;
			info.LoadString( IDS_DELEEVENT_INFO3 );
			dlg.SetEvevntType(info, TRUE, nSelectItemCount);
		}
		else return;
	}
	CString eventName,happenTime,milliSecond,eventType,sql;
	pos=m_listEvent.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_listEvent.GetNextSelectedItem(pos);
		m_recordset.MoveFirst();
		ASSERT(!m_recordset.IsEOF());
		for(i=1;i<=nItem;i++)
		{
			ASSERT(!m_recordset.IsEOF());
			m_recordset.MoveNext();
		}
		eventName=m_recordset[_T("EventName")].ToString();
		eventType=m_recordset[_T("EventType")].ToString();
		milliSecond=m_recordset[_T("Millisecond")].ToString();
		happenTime=m_recordset[_T("HappenTime")].ToString();
		CAdoRecordset event;
		sql.Format(_T("select * from event where EventName='%s' and EventType='%s' and DATEDIFF('s','%s',HappenTime)=0 and Millisecond=%s"),
			eventName,eventType,happenTime,milliSecond);
		if(!event.Open(sql,theApp.m_adoDatabase,adLockOptimistic))
		{
			CString info;
			info.LoadString( IDS_OPEN_DATAF );
			CdMessageBox box(this, info);
		    box.MsgBoxEx();
			event.Close();
			return;
		}
		if(event.IsEOF())
		{
			CString info;
			info.LoadString( IDS_WAVEDATA_INFO );
			CdMessageBox box(this, info);
		    box.MsgBoxEx();
			event.Close();
			return;
		}
		event.Delete();
		event.Close();
		if(nSelectItemCount >=50)
		{
			nPos++;
			dlg.SetProgressPos(nPos);
		}
		
	}
	UpdateEventType();
	QueryData();
}

/*********************************************************************
函数名：OnEventDelete
描述：删除所有事件
参数：
返回:
修改记录：
*********************************************************************/
void CEventView::OnEventDeleteAll()
{
	CString info, title;
	info.LoadString( IDS_DELEEVENT_INFO2 );
	title.LoadString( IDS_STR_TITLE );
	CdMessageBox box(this, info, title, MB_YESNO);
	if(IDNO == box.MsgBoxEx() )
	{
		return;
	}
	CEventProgressDlg dlg;
	
	if(m_nEventCounts >= 50)
	{
		if(IsWindow(this->GetSafeHwnd()))
		{
			dlg.Create(IDD_PROGRESS,this);
			
			dlg.ShowWindow(SW_SHOW);
			CString info;
			info.LoadString( IDS_DELEEVENT_INFO3 );
			dlg.SetEvevntType( info, TRUE,m_nEventCounts);
		}
		else return;
	}
	int nItem=0,nPos=0;
	CString eventName,happenTime,milliSecond,eventType,sql;
	while (nItem < m_nEventCounts)
	{
		m_recordset.MoveFirst();   //回到首行
		ASSERT(!m_recordset.IsEOF());
		for(int i=1;i<=nItem;i++)
		{
			ASSERT(!m_recordset.IsEOF());
			m_recordset.MoveNext();
		}
		eventName=m_recordset[_T("EventName")].ToString();
		eventType=m_recordset[_T("EventType")].ToString();
		milliSecond=m_recordset[_T("Millisecond")].ToString();
		happenTime=m_recordset[_T("HappenTime")].ToString();
		CAdoRecordset event;
		sql.Format(_T("select * from event where EventName='%s' and EventType='%s' and DATEDIFF('s','%s',HappenTime)=0 and Millisecond=%s"),
			eventName,eventType,happenTime,milliSecond);
		if(!event.Open(sql,theApp.m_adoDatabase,adLockOptimistic))
		{
			CString info;
			info.LoadString( IDS_OPEN_DATAF );
			CdMessageBox box(this, info);
	 	    box.MsgBoxEx();
			event.Close();
			return;
		}
		if(event.IsEOF())
		{
			CString info;
			info.LoadString( IDS_WAVEDATA_INFO );
			CdMessageBox box(this, info);
	 	    box.MsgBoxEx();
			event.Close();
			return;
		}
		event.Delete();
		event.Close();
		if(m_nEventCounts >=50)
		{
			nPos++;
			dlg.SetProgressPos(nPos);
		}
		nItem++;
	}
	UpdateEventType();
	QueryData();
}
/************************************************
函数名：UpdateEventType
描述：更新事件类型列表
修改记录：
************************************************/
void CEventView::UpdateEventType()
{
	int i;
	CString sql, info;
	for(i=m_ctlEventType.GetCount()-1;i>=0;i--)
	{
		m_ctlEventType.DeleteString(i);
	}
    info.LoadString( IDS_ALL_TYPE );
	m_ctlEventType.InsertString(0, info);
	sql.Format(_T("select DISTINCT(EventType) from event"));
	if(m_recordset.IsOpen())
	{
		m_recordset.Close();
	}
	if(m_recordset.Open(sql,theApp.m_adoDatabase))
	{
		while(!m_recordset.IsEOF())
		{
			m_ctlEventType.AddString(m_recordset[_T("EventType")].ToString());
			m_recordset.MoveNext();
		}
	}
	m_recordset.Close();
	info.LoadString( IDS_ALL_TYPE );
	m_ctlEventType.SelectString(0, info);
}

void CEventView::OnColumnclickEventList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	
	int lCol=pNMListView->iSubItem;
	if(lCol==0) return;
	LVCOLUMN   lv;   
	lv.mask   =  LVCF_IMAGE   |   LVCF_FMT;   
	if   (m_lSortedColumn   >=   0) //清除其他箭头   
	{   
		m_listEvent.GetColumn(m_lSortedColumn,&lv);   
		lv.fmt   =   (lv.fmt   &   LVCFMT_CENTER)   |   (lv.fmt   &   LVCFMT_LEFT)   
			|   (lv.fmt   &   LVCFMT_RIGHT);   
		m_listEvent.SetColumn(m_lSortedColumn,&lv);   
	}   
	m_listEvent.GetColumn(lCol,&lv);   
	lv.iImage   =   (lv.iImage+1)%2;
	CString strSortType;
	if(lv.iImage==0)
	{
		strSortType=_T("ASC");
	}
	else
	{
		strSortType=_T("DESC");
	}
	switch(lCol)
	{
	case 1:
		m_strOrderBy=_T("EventName ")+strSortType;
		break;
	case 2:
		m_strOrderBy=_T("EventType ")+strSortType;
		break;
	case 3:
		m_strOrderBy=_T("HappenTime ")+strSortType+_T(",Millisecond ") + strSortType;
		break;
	case 4:
		m_strOrderBy=_T("Param ")+strSortType;
		break;
	}
	QueryData();
	UpdateData(FALSE);
	if   (lv.fmt   &   LVCFMT_RIGHT) //右对齐则图标在左   
		lv.fmt   =   LVCFMT_RIGHT   |   LVCFMT_IMAGE;   
	else //否则图标在右   
		lv.fmt   =   (lv.fmt   &   LVCFMT_CENTER)   |   LVCFMT_IMAGE     
		|   LVCFMT_BITMAP_ON_RIGHT; //不设置右、中间对齐则为左对齐   
	m_listEvent.SetColumn(lCol,&lv);   
	m_lSortedColumn   =   lCol; 
}

void CEventView::OnEventOutToExcel()
{
	CCustomFileDialog dlg(FALSE,NULL,_T("*.xls"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Worksheet Files (*.xls)|*.xls||"));
	if(IDOK==dlg.DoModal())
	{
		CString strFilePath,strSheetName;
		CString info, title;
		CString eventInfoStr;
		//lhz modiy 20131-7-8 not sure
		eventInfoStr.LoadString( IDS_EVENT_INFO );
		strSheetName = eventInfoStr;
		strFilePath = dlg.GetPathName();
		title.LoadString( IDS_EVENT_TITLE );
		CString str;
		if(!CMainFrame::GetExcelFile(strFilePath,&m_ctlEventType,&m_listEvent,FALSE))
		{
            str.LoadString( IDS_EPORTEVENT_ERROR );
		}
		else
		{
			info.LoadString( IDS_EXPORTEVENT_FINISH );
			str.Format(info, strFilePath);
		}
		CdMessageBox box(this, str, title);
		box.MsgBoxEx();
	}

}



//DEL void CEventView::OnEditchangeEventCombo() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

//DEL void CEventView::OnKillfocusEventCombo() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

/******************************************************
函数名：OnInputEvent
描述：导入事件按钮响应事件
参数：
返回：
修改记录；
******************************************************/
void CEventView::OnInputEvent() 
{
	// TODO: Add your control notification handler code here
	CString path;
	CFileDialog dlg(TRUE,NULL,_T("*.event"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("event files (*.event)|*event||"));
	if(IDOK==dlg.DoModal())
	{
		CProjectData project;
		path=dlg.GetPathName();
	}
	else
	{
		return;
	}
	FILE *dataFile;
	dataFile=_wfopen(path,_T("rb"));
	if(dataFile==NULL)
	{
		CString info;
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box( this, info);
	    box.MsgBoxEx();
		return;
	}
	//求文件长度
	fseek(dataFile,0,SEEK_END);
	int filelength=ftell(dataFile);
    //读所有文件内容
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

	m_listEvent.DeleteAllItems();
	ASSERT(m_listEvent.GetItemCount() == 0);
	//GetDlgItem(IDC_SORT_BTN)->EnableWindow(FALSE);
	int curPos=0,evtLen,i=0,strLength;
	char ch[200];
	CString str,sql;
	EventData eventData;
	CAdoRecordset record;
	while((evtLen = fileBuf[curPos])!=0)
	{
		str.Format(_T("%d"),i);
		//m_listEvent.InsertItem(LVIF_TEXT|LVIF_IMAGE,i,str,0,0,-1,NULL);
		curPos++;
		if(!CFrame::TransferTime(TRUE,fileBuf+curPos,eventData.happentTime,eventData.nMilliSecond))
		{
			return;
		}
// 		str.Format(_T("%d-%d-%d %d:%d:%d.%d"),2000+fileBuf[curPos],fileBuf[curPos+1],fileBuf[curPos+2],
// 			fileBuf[curPos+3],fileBuf[curPos+4],fileBuf[curPos+5],fileBuf[curPos+6]+(int)(fileBuf[curPos+7]<<8));
// 		m_listEvent.SetItemText(i,3,str);//时间
		curPos += 8;

		strLength = fileBuf[curPos++];
		memset(ch,0,200);
		memcpy(ch,fileBuf+curPos,strLength);
		curPos += strLength;
		eventData.strEvnetType = ch;//.Format(_T("%s"),ch);
		//m_listEvent.SetItemText(i,2,str);//事件类型

		strLength = fileBuf[curPos++];
		memset(ch,0,200);
		memcpy(ch,fileBuf+curPos,strLength);
		curPos += strLength;
		eventData.strEventName = ch;
		//m_listEvent.SetItemText(i,1,str);//事件名称

		strLength = fileBuf[curPos++];
		memset(ch,0,200);
		memcpy(ch,fileBuf+curPos,strLength);
		curPos += strLength;
		eventData.strExtendContent = ch;
		//m_listEvent.SetItemText(i,4,str);//参数

		i++;
		sql.Format(_T("select * from event where EventName='%s' and EventType='%s' and DATEDIFF('s','%s',HappenTime)=0 and Millisecond=%d"),
			eventData.strEventName,eventData.strEvnetType,eventData.happentTime.Format(_T("%Y-%m-%d %H:%M:%S")),
			eventData.nMilliSecond);
		
		if(record.IsOpen())
		{
			record.Close();
		}
		if(record.Open(sql,theApp.m_adoDatabase,adLockOptimistic))
		{
			if(record.IsEOF())
			{
				record.AddNew();
				record[_T("Param")]=eventData.strExtendContent;
				record[_T("HappenTime")]=eventData.happentTime;
				record[_T("Millisecond")]=eventData.nMilliSecond;
				record[_T("EventType")]=eventData.strEvnetType;
				record[_T("EventName")]=eventData.strEventName;
				try
				{
					record.Update();
				}
				catch (...)
				{
					CString info;
					info.LoadString( IDS_ADDEVENT_ERROR );
					CdMessageBox box(this, info);
		            box.MsgBoxEx();
				}
			}
		}
	}
	if(record.IsOpen())
	{
		record.Close();
	}
	UpdateEventType();
	QueryData();
	UpdateData(FALSE);
}

void CEventView::OnGetEvent() 
{
	// TODO: Add your control notification handler code here
	GetDeviceEvent();
}

VOID CEventView::ReadEventFile()
{
	OnInputEvent();
}

VOID CEventView::OutEventToExcel()
{
	OnEventOutToExcel();
}
