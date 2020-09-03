// RealMonitorView.cpp : implementation file
//

#include "stdafx.h"
#include "RealMonitorView.h"
#include "MainFrm.h"
#include <math.h>
#include "SID_2FY.h"
#include "resource.h"
#include "PWDDlg.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEvent m_enableEvent;
extern CMySID_2FYApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CRealMonitorView
IMPLEMENT_DYNCREATE(CRealMonitorView, CScrollView)

CRealMonitorView::CRealMonitorView()
:m_nHeightOfInfo(70),m_nHeightOfPole(400),m_nHeightOfTable(60),
m_nHeightOfFrame(20),m_nHeightOfRect(m_nHeightOfInfo+m_nHeightOfPole+m_nHeightOfTable+2*m_nHeightOfFrame),
m_nWideOfFrame(25),m_nWideOfPole(70),m_nWideOfTable(500),m_nWideOfRect(700),
m_dRangeOfFreqDis(1.0),m_dRangeOfVolDis(25.0),m_nTimesOfWidthOfHand(20),
m_rcClientView(0,0,0,0)
{
	
	m_clrGHand =RGB(0,255,0);
	m_clrSysHand = RGB(255,0,0);
	m_clrLegPole =RGB(0,255,0);
	m_clrIllegPole =RGB(255,0,0);
	m_dMaxFreqDispersion = 0.15;    //出厂设置
	m_dMinFreqDispersion = -0.15;
	m_dMaxVolDispersion = 5.0;
	m_dMinVolDispersion = -5.0;
	m_nChannelNum = -1;
	m_strVoltageDispersion = _T("");
	m_strFreqDispersion = _T("");
	m_strSysVoltage = _T("");
	m_strGVoltage = _T("");
	m_strSysFreq = _T("");
	m_strAngleDispersion = _T("");
	m_strGFreq = _T("");
	m_strSyncPoint = _T("");
	m_dRatingOfGVol = 100;//2016-12-13 由-1调整为100
	m_dRatingOfSysVol = 100;

	m_nStateCount = 0;
	m_pStateInfo = NULL;
	m_nStartIndOfPrompt = -1;
	m_nStartIndOfException = -1;
	m_nStartIndOfWorkMode = -1;
	m_nEndIndOfException = -1;
	m_nEndIndOfPrompt = -1;
	m_nEndIndOfWorkMode = -1;

	m_bReqSyncThreadExit = false;
	m_nTelCode = 0xffffffff;

	m_bStartSync = FALSE;
	m_bTelecontrol =FALSE;

	m_nSysParaCount = 0;
	m_nNorParaCount = 0;
	
	m_hParaThread = NULL;
	m_hSyncThread = NULL;
	m_hTelThread = NULL;
	m_nIsRun = true; //2014-11-26 add
}

CRealMonitorView::~CRealMonitorView()
{
// 	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
// 	pMainFrm->m_wndMainToolBar.GetToolBarCtrl().EnableButton(IDM_RIGHT_BAR,FALSE);
	
}


BEGIN_MESSAGE_MAP(CRealMonitorView, CScrollView)
	//{{AFX_MSG_MAP(CRealMonitorView)
	ON_WM_SIZE()
	ON_MESSAGE(UM_SYNC_REAL,OnSyncReal)
	ON_MESSAGE(UM_PARA_DATA,OnThreadPara)
 	ON_MESSAGE(UM_COM_FAILED,OnComFailed)
	ON_MESSAGE(UM_TEL_DATA,OnThreadTelControl)
	ON_WM_TIMER()
	ON_COMMAND(IDM_UPDATE_PARA, OnUpdatePara)
	ON_COMMAND(IDC_START_SYNC, OnStartSync)
	ON_COMMAND(IDC_RESTART, OnRestart)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRealMonitorView drawing

void CRealMonitorView::OnInitialUpdate()
{
	CToolBarCtrl & reToolBarCtrl = ((CMainFrame *)GetParentFrame())->m_wndMainToolBar.GetToolBarCtrl();
	reToolBarCtrl.EnableButton(IDM_UPDATE_PARA,FALSE);
	reToolBarCtrl.EnableButton(IDC_START_SYNC,FALSE);
	CStatusBarEx & statusBar = ((CMainFrame*)GetParentFrame())->m_wndStatusBar;
	statusBar.SetPaneText(1,_T(""));
	statusBar.SetPaneText(2,_T(""));
	StartCommunication();
	
	CScrollView::OnInitialUpdate();
}

void CRealMonitorView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	DrawRealMonitor(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CRealMonitorView diagnostics

#ifdef _DEBUG
void CRealMonitorView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CRealMonitorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRealMonitorView message handlers

void CRealMonitorView::DrawRealMonitor(CDC *pDC)
{
	ASSERT_VALID(pDC);
	UNUSED(pDC);
	CPen *pOldPen;
	CBrush *pOldBrush;
	CRect rcTemp1,rcTemp2;
	CDC MemDC; //首先定义一个显示设备对象
	CBitmap MemBitmap;//定义一个位图对象
	CString str,str1, info;
	int nHeightOfRow,nWidthOfRow;
	nHeightOfRow = m_rcRealTable.Height()/3;
	nWidthOfRow = m_rcRealTable.Width()/4;
	//随后建立与屏幕显示兼容的内存显示设备
	VERIFY(MemDC.CreateCompatibleDC(NULL));
	//这时还不能绘图，因为没有地方画 ^_^
	//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
	VERIFY(MemBitmap.CreateCompatibleBitmap(pDC,m_rcClientView.Width(),m_rcClientView.Height()));
	//将位图选入到内存显示设备中
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);

	//先用背景色将位图清除干净，这里我用的是白色作为背景
	//你也可以用自己应该用的颜色
	MemDC.FillSolidRect(m_rcClientView,RGB(255,255,255));
	CFont font,*pOldFont;
	font.CreatePointFont(120,_T("宋体"));
	pOldFont=MemDC.SelectObject(&font);
	DrawStaticView(&MemDC);
	if(m_strFreqDispersion != _T(""))
	{
		DrawPole(&MemDC,FreqPole);
	}
	if(m_strVoltageDispersion != _T(""))
	{
		DrawPole(&MemDC,VolPole);
	}
	wchar_t *pStr;
	double dTemp;
	if(m_strGVoltage == _T(""))
	{
		dTemp = 0;
	}
	else
	{
		pStr = m_strGVoltage.GetBuffer(0);
		swscanf(pStr,_T("%lf"),&dTemp);
		m_strGVoltage.ReleaseBuffer();
	}
	dTemp = dTemp/m_dRatingOfGVol;
	if(DoubleCmp(dTemp-1.4)>0)
	{
		dTemp = 1.4;
	}
	if(DoubleCmp(dTemp-0.2) < 0)
	{
		dTemp = 0.2;
	}
	m_nLengthOfGHand = dTemp * m_nLengthOfRatingHand;
	if(m_strAngleDispersion == _T(""))
	{
		dTemp = 180;
	}
	else
	{
		swscanf(m_strAngleDispersion,_T("%lf"),&dTemp);
	//	dTemp = dTemp + m_sysTurnAngle;
	}
	CPen pen(PS_NULL,1,RGB(0,0,0));
	CPoint ptCenter = m_rcSyncClock.CenterPoint();
	pOldPen = MemDC.SelectObject(&pen);
	if(AD_COUNTERCLOCKWISE  == MemDC.GetArcDirection())
	{
		dTemp = dTemp;
	}
	MemDC.AngleArc(ptCenter.x,ptCenter.y,m_nLengthOfGHand,90,dTemp);
	m_arrPtGHand[0] = MemDC.GetCurrentPosition();
	MemDC.AngleArc(ptCenter.x,ptCenter.y,m_nLengthOfRatingHand/m_nTimesOfWidthOfHand,180,dTemp);
	m_arrPtGHand[1] = MemDC.GetCurrentPosition();
	MemDC.AngleArc(ptCenter.x,ptCenter.y,2*m_nLengthOfRatingHand/m_nTimesOfWidthOfHand,270,dTemp);
	m_arrPtGHand[2] = MemDC.GetCurrentPosition();
	MemDC.AngleArc(ptCenter.x,ptCenter.y,m_nLengthOfRatingHand/m_nTimesOfWidthOfHand,0,dTemp);
	m_arrPtGHand[3] = MemDC.GetCurrentPosition();
	MemDC.SelectObject(pOldPen);
	CBrush brushGHand(m_clrGHand);
	pOldBrush = MemDC.SelectObject(&brushGHand);
	MemDC.Polygon(m_arrPtGHand,4);
	MemDC.SelectObject(pOldBrush);
	
	//文字项的绘制
	//绘制表项
	rcTemp1 = CRect(nWidthOfRow+m_rcRealTable.left+2,m_rcRealTable.top+2,
			nWidthOfRow*2-2+m_rcRealTable.left,m_rcRealTable.top+nHeightOfRow-1);
	rcTemp2 = CRect(3*nWidthOfRow+m_rcRealTable.left+2,m_rcRealTable.top+2,
			m_rcRealTable.right-2,m_rcRealTable.top+nHeightOfRow-1);
	if(0xff == m_nChannelNum)
	{
		str.LoadString( IDS_UNKOWN );
		MemDC.DrawText( str, &rcTemp2,DT_CENTER);
		MemDC.DrawText( str, &rcTemp1,DT_CENTER);
	}
	else if (0 == m_nChannelNum)
	{
		str.LoadString( IDS_NONE );
		MemDC.DrawText( str, &rcTemp2,DT_CENTER);
		MemDC.DrawText( str, &rcTemp1,DT_CENTER);
	}
	else
	{
		str.Format(_T("%d"),m_nChannelNum);
		MemDC.DrawText(m_strSyncPoint,&rcTemp2,DT_CENTER);
		MemDC.DrawText(str,&rcTemp1,DT_CENTER);
	}
	rcTemp1 = CRect(rcTemp1.left,rcTemp1.top+nHeightOfRow,rcTemp1.right,rcTemp1.bottom+nHeightOfRow);
	MemDC.DrawText(m_strSysVoltage,&rcTemp1,DT_CENTER);
	rcTemp1 = CRect(rcTemp1.left,rcTemp1.top+nHeightOfRow,rcTemp1.right,rcTemp1.bottom+nHeightOfRow);
	MemDC.DrawText(m_strGVoltage,&rcTemp1,DT_CENTER);
	rcTemp2 = CRect(rcTemp2.left,rcTemp2.top+nHeightOfRow,rcTemp2.right,rcTemp2.bottom+nHeightOfRow);
	MemDC.DrawText(m_strSysFreq,&rcTemp2,DT_CENTER);
	rcTemp2 = CRect(rcTemp2.left,rcTemp2.top+nHeightOfRow,rcTemp2.right,rcTemp2.bottom+nHeightOfRow);
	MemDC.DrawText(m_strGFreq,&rcTemp2,DT_CENTER);
	str = _T("");
	info.LoadString( IDS_FRE_DIFF );
	str.Format(_T("%s%s: %s"),info,str1,m_strFreqDispersion);
	info.LoadString( IDS_VOLTAGE_DIFF );
	MemDC.DrawText(str,m_rcFreqDispersionInfo,DT_LEFT);
	str.Format(_T("%s%s: %s"), info,str1,m_strVoltageDispersion);
	MemDC.DrawText(str,m_rcVoltageDispersionInfo,DT_RIGHT);
	info.LoadString( IDS_ANG_DIFF );
	str.Format(_T("%s%s: %s"), info,str1,m_strAngleDispersion);
	MemDC.DrawText(str,m_rcAngleDispersionInfo,DT_CENTER);
	info.LoadString( IDS_DLG_DEFTITLE );
	str.Format(_T("%s: %s"), info, m_strTipInfo);
	MemDC.DrawText(str,m_rcTipInfo,DT_CENTER);
	MemDC.SelectObject(pOldFont);
	pDC->BitBlt(0,0,m_rcClientView.Width(),m_rcClientView.Height(),&MemDC,0,0,SRCCOPY);
	// 	pDC->BitBlt(CHANNEL_INFO_WIDTH+1,0,m_curRect.Width(),m_curRect.Height(),
	// 		&MemDC,m_curRect.left+CHANNEL_INFO_WIDTH+1,m_curRect.top,SRCCOPY);
	//  绘图完成后的清理
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}

BOOL CRealMonitorView::InitRectInfo()
{
	if(m_rcRealDraw.Width() < m_nWideOfRect || m_rcRealDraw.Height() < m_nHeightOfRect)
	{
		return FALSE;
	}
	m_rcRealDraw.bottom = m_rcRealDraw.top + m_nHeightOfRect;
	m_rcRealDraw.right = m_rcRealDraw.left + m_nWideOfRect;
	m_rcPoleOfVoltage.top = m_rcRealDraw.top + m_nHeightOfFrame;
	m_rcPoleOfVoltage.left = m_rcRealDraw.left + m_nWideOfFrame;
	m_rcPoleOfVoltage.right = m_rcPoleOfVoltage.left + m_nWideOfPole;
	m_rcPoleOfVoltage.bottom = m_rcPoleOfVoltage.top + m_nHeightOfPole;
	m_rcCutline = CRect(m_rcPoleOfVoltage.right+10,m_rcPoleOfVoltage.top,m_rcPoleOfVoltage.right+110,m_rcPoleOfVoltage.top+70);
	m_rcPoleOfFreq = m_rcPoleOfVoltage;
	m_rcPoleOfFreq.OffsetRect(m_nWideOfRect-2*m_nWideOfFrame-m_nWideOfPole,0);
	//m_rcSyncColck的圆心x坐标在m_rcRealDraw中心处，直径为m_nHeightOfPole-50,圆心在m_rcPoleOfFreq的中心处的y坐标
	m_rcSyncClock = CRect(m_nWideOfRect/2-m_nHeightOfPole/2+25+m_rcRealDraw.left,m_rcPoleOfFreq.top + 25,
		m_nWideOfRect/2+m_nHeightOfPole/2-25+m_rcRealDraw.left,m_rcPoleOfFreq.bottom - 25);
	m_nLengthOfRatingHand = ((double)m_rcSyncClock.Width()/2-10)/1.4;
	//初始化表针的多边形
	CPoint ptCenter = m_rcSyncClock.CenterPoint();
	

	//宽度为30，高度为20
	m_rc0SyncClockInfo = CRect(m_rcSyncClock.CenterPoint().x-15,m_rcSyncClock.top-25,
		m_rcSyncClock.CenterPoint().x+15,m_rcSyncClock.top-5);
	//宽度为50，高度为20
	m_rc_90SyncClockInfo = CRect(m_rcSyncClock.right+5,m_rcSyncClock.CenterPoint().y-10,
		m_rcSyncClock.right+55,m_rcSyncClock.CenterPoint().y+10);
	//宽度为50，高度为20
	m_rc90SyncClockInfo = CRect(m_rcSyncClock.left-55,m_rcSyncClock.CenterPoint().y-10,
		m_rcSyncClock.left,m_rcSyncClock.CenterPoint().y+10);
	//宽度为50，高度为20
	m_rc180SyncClockInfo = CRect(m_rc0SyncClockInfo.left-10,m_rcSyncClock.bottom + 5,
		m_rc0SyncClockInfo.right+10,m_rcSyncClock.bottom + 25);
	//宽度为110，高度为20
	m_rcFreqDispersionInfo = CRect(m_rcPoleOfFreq.left-10,m_rcPoleOfFreq.bottom + 10,
		m_rcPoleOfFreq.left+190,m_rcPoleOfFreq.bottom +30);
	//宽度为110，高度为20
	m_rcVoltageDispersionInfo = CRect(m_rcPoleOfVoltage.left-120,m_rcFreqDispersionInfo.top,
		m_rcPoleOfVoltage.right+10,m_rcFreqDispersionInfo.bottom);
	//宽度为130，高度为20
	m_rcAngleDispersionInfo =CRect(m_rc180SyncClockInfo.left-80,m_rcFreqDispersionInfo.top,
		m_rc180SyncClockInfo.right+80,m_rcFreqDispersionInfo.bottom);
	//宽度为700，高度为20
	m_rcTipInfo = CRect(m_rc0SyncClockInfo.left-350,m_rcAngleDispersionInfo.bottom+10,
		m_rc0SyncClockInfo.right+350,m_rcAngleDispersionInfo.bottom+30);
	m_rcRealTable = CRect(m_nWideOfRect/2-m_nWideOfTable/2+m_rcRealDraw.left,m_rcPoleOfFreq.bottom+m_nHeightOfInfo,
		m_nWideOfRect/2+m_nWideOfTable/2+m_rcRealDraw.left,m_rcPoleOfFreq.bottom+m_nHeightOfInfo+m_nHeightOfTable);
	m_rcRealDraw = CRect(m_nWideOfRect/2-m_nWideOfTable/2+m_rcRealDraw.left,m_rcPoleOfFreq.bottom+m_nHeightOfInfo,
		m_nWideOfRect/2+m_nWideOfTable/2+m_rcRealDraw.left,m_rcRealDraw.bottom - m_nHeightOfFrame);
	return TRUE;
}

void CRealMonitorView::DrawStaticView(CDC *pDC)
{
	CRect rcTemp;
	CPoint ptTemp;
	CString str,str1, info;
	str1 = _T("");
	double dTemp;
	int nRadii;        //半径
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	pDC->Rectangle(&m_rcPoleOfFreq);
	pDC->Rectangle(&m_rcPoleOfVoltage);
	pDC->MoveTo(m_rcPoleOfFreq.left-10,m_rcPoleOfFreq.CenterPoint().y);
	pDC->LineTo(m_rcPoleOfFreq.right+10,m_rcPoleOfFreq.CenterPoint().y);
	pDC->MoveTo(m_rcPoleOfVoltage.left-10,m_rcPoleOfVoltage.CenterPoint().y);
	pDC->LineTo(m_rcPoleOfVoltage.right+10,m_rcPoleOfVoltage.CenterPoint().y);
	pDC->Rectangle(m_rcRealTable);
	pDC->Rectangle(m_rcCutline);
	rcTemp =CRect(m_rcCutline.left+5,m_rcCutline.top+1,m_rcCutline.right-5,m_rcCutline.top+20);
    info.LoadString( IDS_CLOCK_COLOR );
	pDC->DrawText( info, &rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	rcTemp =CRect(m_rcCutline.left+40,m_rcCutline.top+21,m_rcCutline.right-5,m_rcCutline.top+40);
	info.LoadString( IDS_SYS_SIDE );
	pDC->DrawText(info,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	rcTemp =CRect(m_rcCutline.left+40,m_rcCutline.top+41,m_rcCutline.right-5,m_rcCutline.top+60);
	info.LoadString( IDS_INCOMER );
	pDC->DrawText( info, &rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	pDC->Ellipse(&m_rcSyncClock);
	ptTemp = m_rcSyncClock.CenterPoint();
	nRadii = (double)m_nLengthOfRatingHand*1.4;
	rcTemp =CRect(ptTemp.x-nRadii,ptTemp.y-nRadii,ptTemp.x+nRadii,ptTemp.y+nRadii);
	pDC->Ellipse(&rcTemp);
	nRadii = m_nLengthOfRatingHand;
	rcTemp =CRect(ptTemp.x-nRadii,ptTemp.y-nRadii,ptTemp.x+nRadii,ptTemp.y+nRadii);
	pDC->Ellipse(&rcTemp);
	pDC->DrawText(_T("0°"),&m_rc0SyncClockInfo,DT_CENTER);
	pDC->DrawText(_T("90°"),&m_rc90SyncClockInfo,DT_RIGHT);
	pDC->DrawText(_T("-90°"),&m_rc_90SyncClockInfo,DT_LEFT);
	pDC->DrawText(_T("180°"),&m_rc180SyncClockInfo,DT_CENTER);
	int i,nHeightOfRow,nWidthOfRow;
	nHeightOfRow = m_rcRealTable.Height()/3;
	nWidthOfRow = m_rcRealTable.Width()/4;
	CRect rcText;
	//绘制表格
	for(i = 1;i<3;i++)
	{
		pDC->MoveTo(m_rcRealTable.left,m_rcRealTable.top + nHeightOfRow*i);
		pDC->LineTo(m_rcRealTable.right,m_rcRealTable.top + nHeightOfRow*i);
	}
	for(i=1;i<4;i++)
	{
		pDC->MoveTo(m_rcRealTable.left+i*nWidthOfRow,m_rcRealTable.top);
		pDC->LineTo(m_rcRealTable.left+i*nWidthOfRow,m_rcRealTable.bottom);
	}
	rcText =CRect(m_rcRealTable.left+2,m_rcRealTable.top+2,
		m_rcRealTable.left+nWidthOfRow-2,m_rcRealTable.top+nHeightOfRow-1);
	info.LoadString( IDS_CHANNAL_NUM );
	pDC->DrawText( info, &rcText,DT_CENTER);
	rcText =CRect(m_rcRealTable.left+2,m_rcRealTable.top+nHeightOfRow+2,
		m_rcRealTable.left+nWidthOfRow-2,m_rcRealTable.top+2*nHeightOfRow-1);
	info.LoadString( IDS_SYS_VOL );
	str.Format(_T("%s%s"), info, str1);
	pDC->DrawText(str,&rcText,DT_CENTER);
	rcText =CRect(m_rcRealTable.left+2,m_rcRealTable.top+2*nHeightOfRow+2,
		m_rcRealTable.left+nWidthOfRow-2,m_rcRealTable.bottom-1);
	info.LoadString( IDS_INCOMER_VOL );
	str.Format(_T("%s%s"), info, str1);
	pDC->DrawText(str,&rcText,DT_CENTER);

	rcText =CRect(m_rcRealTable.left+2*nWidthOfRow+2,m_rcRealTable.top+2,
		m_rcRealTable.left+3*nWidthOfRow-2,m_rcRealTable.top+nHeightOfRow-1);
	info.LoadString( IDS_PARALLERL_POINT );
	pDC->DrawText( info, &rcText,DT_CENTER);
	rcText =CRect(m_rcRealTable.left+2*nWidthOfRow+2,m_rcRealTable.top+nHeightOfRow+2,
		m_rcRealTable.left+3*nWidthOfRow-2,m_rcRealTable.top+2*nHeightOfRow-1);
	
	info.LoadString( IDS_SYS_FRE );
	str.Format(_T("%s%s"), info, str1);
	pDC->DrawText(str,&rcText,DT_CENTER);
	rcText =CRect(m_rcRealTable.left+2*nWidthOfRow+2,m_rcRealTable.top+2*nHeightOfRow+2,
		m_rcRealTable.left+3*nWidthOfRow-2,m_rcRealTable.bottom-1);
	info.LoadString( IDS_INCOMER_FRE );
	str.Format(_T("%s%s"), info, str1);
	pDC->DrawText(str,&rcText,DT_CENTER);
	//CPen *pOldPen,pen(PS_NULL,1,RGB(0,0,0));
	//pOldPen = pDC->SelectObject(&pen);
	
	//pDC->SelectObject(pOldPen);
	wchar_t * pStr;
	
	if(m_dRatingOfSysVol < 0)
	{
		m_nLengthOfSysHand = 0;
	}
	else
	{
		if(m_strSysVoltage == _T(""))
		{
			dTemp = 0;
		}
		else
		{
			pStr = m_strSysVoltage.GetBuffer(0);
			swscanf(pStr,_T("%lf"),&dTemp);
			m_strSysVoltage.ReleaseBuffer();
			dTemp /= m_dRatingOfSysVol;
		}
		if(DoubleCmp(dTemp-1.4)>0)
		{
			dTemp = 1.4;
		}
		m_nLengthOfSysHand =dTemp *m_nLengthOfRatingHand; 
	}
	pDC->SetBkMode(oldBkMode);
	ptTemp = m_rcSyncClock.CenterPoint();
	m_arrPtSysHand[0] = CPoint(ptTemp.x,ptTemp.y-m_nLengthOfSysHand);
	m_arrPtSysHand[1] = CPoint(ptTemp.x-m_nLengthOfRatingHand/m_nTimesOfWidthOfHand,ptTemp.y);
	m_arrPtSysHand[2] = CPoint(ptTemp.x,ptTemp.y+2*m_nLengthOfRatingHand/m_nTimesOfWidthOfHand);
	m_arrPtSysHand[3] = CPoint(ptTemp.x+m_nLengthOfRatingHand/m_nTimesOfWidthOfHand,ptTemp.y);
	CBrush *pOldBrush,brush(m_clrSysHand),brush1(m_clrGHand),brush2(RGB(0,0,0));
	pOldBrush = pDC->SelectObject(&brush);
	pDC->Polygon(m_arrPtSysHand,4);
	rcTemp = CRect(m_rcCutline.left+5,m_rcCutline.top+25,m_rcCutline.left+35,m_rcCutline.top+36);
	pDC->Rectangle(rcTemp);
	pDC->SelectObject(brush1);
	rcTemp = CRect(m_rcCutline.left+5,m_rcCutline.top+45,m_rcCutline.left+35,m_rcCutline.top+56);
	pDC->Rectangle(rcTemp);
	//绘制左边箭头线
	ptTemp =m_rcSyncClock.CenterPoint();
	CPen *pOldPen,pen(PS_NULL,1,RGB(0,0,0));
	pOldPen = pDC->SelectObject(&pen);
	pDC->AngleArc(ptTemp.x,ptTemp.y,m_rcSyncClock.Width()/2+15,90,15);
	pDC->SelectObject(pOldPen);
	pDC->AngleArc(ptTemp.x,ptTemp.y,m_rcSyncClock.Width()/2+15,105,25);
	CPoint arrPoint[3];
	pOldPen = pDC->SelectObject(&pen);
	ptTemp = arrPoint[0] = pDC->GetCurrentPosition();
	pDC->AngleArc(ptTemp.x,ptTemp.y,5,0,0);
	arrPoint[1] = pDC->GetCurrentPosition();
	pDC->AngleArc(ptTemp.x,ptTemp.y,5,35,20);
	arrPoint[2] = pDC->GetCurrentPosition();
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(brush2);
	pDC->Polygon(arrPoint,3);
	rcTemp = CRect(ptTemp.x-20,ptTemp.y-25,ptTemp.x+20,ptTemp.y-5);
	oldBkMode = pDC->SetBkMode(TRANSPARENT);
	info.LoadString( IDS_SLOW );
	pDC->DrawText( info, rcTemp,DT_CENTER);
	pDC->SetBkMode(oldBkMode);

	//绘制右边箭头线
	ptTemp =m_rcSyncClock.CenterPoint();
	pOldPen = pDC->SelectObject(&pen);
	pDC->AngleArc(ptTemp.x,ptTemp.y,m_rcSyncClock.Width()/2+15,90,-10);
	pDC->SelectObject(pOldPen);
	pDC->AngleArc(ptTemp.x,ptTemp.y,m_rcSyncClock.Width()/2+15,80,-25);
	pOldPen = pDC->SelectObject(&pen);
	ptTemp = arrPoint[0] = pDC->GetCurrentPosition();
	pDC->AngleArc(ptTemp.x,ptTemp.y,5,180,0);
	arrPoint[1] = pDC->GetCurrentPosition();
	pDC->AngleArc(ptTemp.x,ptTemp.y,5,145,-20);
	arrPoint[2] = pDC->GetCurrentPosition();
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(brush2);
	pDC->Polygon(arrPoint,3);
	rcTemp = CRect(ptTemp.x-20,ptTemp.y-25,ptTemp.x+20,ptTemp.y-5);
	oldBkMode = pDC->SetBkMode(TRANSPARENT);
	info.LoadString( IDS_FAST );
	pDC->DrawText(info, rcTemp,DT_CENTER);
	pDC->SetBkMode(oldBkMode);
	pDC->SelectObject(pOldBrush);
}



void CRealMonitorView::DrawPole(CDC *pDC,PoleType poleType)
{
	wchar_t *pStr;
	double dTemp;
	CBrush *pOldBrush,brushLegPole(m_clrLegPole),brushIllegPole(m_clrIllegPole);
	CPen *pOldPen,pen(PS_SOLID,1,RGB(255,0,0));
	CPoint ptCenter;
	CRect rcTemp;
	CString strMaxVolDis,strMinVolDis,strMaxFreqDis,strMinFreqDis;
	strMaxVolDis.Format(_T("%.0f%%"),m_dMaxVolDispersion);
	strMinVolDis.Format(_T("%.0f%%"),m_dMinVolDispersion);
	strMaxFreqDis.Format(_T("%.2f"),m_dMaxFreqDispersion);
	strMinFreqDis.Format(_T("%.2f"),m_dMinFreqDispersion);
	switch (poleType)
	{
	case FreqPole:
		ptCenter = m_rcPoleOfFreq.CenterPoint();
		pOldPen = pDC->SelectObject(&pen);
		dTemp = m_dMaxFreqDispersion/(m_dRangeOfFreqDis*2);
		pDC->MoveTo(m_rcPoleOfFreq.left-5,ptCenter.y-dTemp*m_nHeightOfPole);
		pDC->LineTo(m_rcPoleOfFreq.right+5,ptCenter.y-dTemp*m_nHeightOfPole);
		pDC->SelectObject(pOldPen);
// 		rcTemp = CRect(m_rcPoleOfFreq.right+6,ptCenter.y-dTemp*m_nHeightOfPole-10,
// 			m_rcPoleOfFreq.right+46,ptCenter.y-dTemp*m_nHeightOfPole+10);
		if(dTemp * m_nHeightOfPole < 15)
		{
			rcTemp = CRect(m_rcPoleOfFreq.left-46,ptCenter.y-15-10,
				m_rcPoleOfFreq.left-6,ptCenter.y-15+10);
		}
		else
		{
			rcTemp = CRect(m_rcPoleOfFreq.left-46,ptCenter.y-dTemp*m_nHeightOfPole-10,
				m_rcPoleOfFreq.left-6,ptCenter.y-dTemp*m_nHeightOfPole+10);
		}
		pDC->DrawText(strMaxFreqDis,rcTemp,DT_VCENTER|DT_SINGLELINE|DT_RIGHT);
		pOldPen = pDC->SelectObject(&pen);
		dTemp = m_dMinFreqDispersion/(m_dRangeOfFreqDis*2);
		pDC->MoveTo(m_rcPoleOfFreq.left-5,ptCenter.y-dTemp*m_nHeightOfPole);
		pDC->LineTo(m_rcPoleOfFreq.right+5,ptCenter.y-dTemp*m_nHeightOfPole);
		pDC->SelectObject(pOldPen);
		rcTemp = CRect(m_rcPoleOfFreq.left-46,ptCenter.y-dTemp*m_nHeightOfPole-10,
				m_rcPoleOfFreq.left-6,ptCenter.y-dTemp*m_nHeightOfPole+10);
		pDC->DrawText(strMinFreqDis,rcTemp,DT_VCENTER|DT_SINGLELINE|DT_RIGHT);
		pStr = m_strFreqDispersion.GetBuffer(0);
		VERIFY(1 == swscanf(pStr,_T("%lf"),&dTemp));
		m_strFreqDispersion.ReleaseBuffer(-1);
		if(DoubleCmp(dTemp-m_dRangeOfFreqDis)>=0) dTemp = m_dRangeOfFreqDis;
		if(!m_bGFrqHigh && DoubleCmp(dTemp-m_dMinFreqDispersion)>=0 && DoubleCmp(m_dMaxFreqDispersion-dTemp)>=0)
		{
			pOldBrush=pDC->SelectObject(&brushLegPole);
		}
		else if (m_bGFrqHigh && DoubleCmp(dTemp)>0 && DoubleCmp(m_dMaxFreqDispersion-dTemp)>=0)
		{
			pOldBrush=pDC->SelectObject(&brushLegPole);
		}
		else
		{
			pOldBrush=pDC->SelectObject(&brushIllegPole);
		}
		if(DoubleCmp(fabs(dTemp)-m_dRangeOfFreqDis)> 0)
		{
			dTemp = DoubleCmp(dTemp)>0?m_dRangeOfFreqDis:0-m_dRangeOfFreqDis;
		}
		dTemp = dTemp /(m_dRangeOfFreqDis*2);
		
		if(DoubleCmp(dTemp)>=0)
		{
			rcTemp =CRect(m_rcPoleOfFreq.left,ptCenter.y-dTemp*m_nHeightOfPole,m_rcPoleOfFreq.right,ptCenter.y);
		}
		else
		{
			rcTemp =CRect(m_rcPoleOfFreq.left,ptCenter.y,m_rcPoleOfFreq.right,ptCenter.y-dTemp*m_nHeightOfPole);
		}
		pDC->Rectangle(rcTemp);
		pDC->SelectObject(pOldBrush);
		break;
	case VolPole:
		ptCenter = m_rcPoleOfVoltage.CenterPoint();
		pOldPen = pDC->SelectObject(&pen);
		dTemp = m_dMaxVolDispersion/(m_dRangeOfVolDis*2);
		pDC->MoveTo(m_rcPoleOfVoltage.left-5,ptCenter.y-dTemp*m_nHeightOfPole);
		pDC->LineTo(m_rcPoleOfVoltage.right+5,ptCenter.y-dTemp*m_nHeightOfPole);
		pDC->SelectObject(pOldPen);
		if(dTemp * m_nHeightOfPole < 15)
		{
			rcTemp = CRect(m_rcPoleOfVoltage.right+6,ptCenter.y-15-10,
				m_rcPoleOfVoltage.right+46,ptCenter.y-15+10);
		}
		else
		{
			rcTemp = CRect(m_rcPoleOfVoltage.right+6,ptCenter.y-dTemp*m_nHeightOfPole-10,
				m_rcPoleOfVoltage.right+46,ptCenter.y-dTemp*m_nHeightOfPole+10);
		}
		
		pDC->DrawText(strMaxVolDis,rcTemp,DT_VCENTER|DT_SINGLELINE|DT_LEFT);
		pOldPen = pDC->SelectObject(&pen);
		dTemp = m_dMinVolDispersion/(m_dRangeOfVolDis*2);
		pDC->MoveTo(m_rcPoleOfVoltage.left-5,ptCenter.y-dTemp*m_nHeightOfPole);
		pDC->LineTo(m_rcPoleOfVoltage.right+5,ptCenter.y-dTemp*m_nHeightOfPole);
		pDC->SelectObject(pOldPen);
		rcTemp = CRect(m_rcPoleOfVoltage.right+6,ptCenter.y-dTemp*m_nHeightOfPole-10,
			m_rcPoleOfVoltage.right+56,ptCenter.y-dTemp*m_nHeightOfPole+10);
		pDC->DrawText(strMinVolDis,rcTemp,DT_VCENTER|DT_SINGLELINE|DT_LEFT);
		pStr = m_strVoltageDispersion.GetBuffer(0);
		VERIFY(1 == swscanf(pStr,_T("%lf"),&dTemp));
		m_strVoltageDispersion.ReleaseBuffer(-1);
		if(!m_bGVolHigh && DoubleCmp(dTemp-m_dMinVolDispersion)>=0 && DoubleCmp(m_dMaxVolDispersion-dTemp)>=0)
		{
			pOldBrush=pDC->SelectObject(&brushLegPole);
		}
		else if (m_bGVolHigh && DoubleCmp(dTemp)>0 && DoubleCmp(m_dMaxVolDispersion-dTemp)>=0)
		{
			pOldBrush=pDC->SelectObject(&brushLegPole);
		}
		else
		{
			pOldBrush=pDC->SelectObject(&brushIllegPole);
		}
		if(DoubleCmp(fabs(dTemp)-m_dRangeOfVolDis)> 0)
		{
			dTemp = DoubleCmp(dTemp)>0?m_dRangeOfVolDis:0-m_dRangeOfVolDis;
		}
		dTemp /= m_dRangeOfVolDis*2;
		
		if(DoubleCmp(dTemp)>=0)
		{
			rcTemp =CRect(m_rcPoleOfVoltage.left,ptCenter.y-dTemp*m_nHeightOfPole,m_rcPoleOfVoltage.right,ptCenter.y);
		}
		else
		{
			rcTemp =CRect(m_rcPoleOfVoltage.left,ptCenter.y,m_rcPoleOfVoltage.right,ptCenter.y-dTemp*m_nHeightOfPole);
		}
		pDC->Rectangle(rcTemp);
		pDC->SelectObject(pOldBrush);
		break;
	default:
		break;
	}
}


void CRealMonitorView::UpdateRightBarPara()
{
	
	if(m_nChannelNum == -1) return;
	CListCtrl *pList = &((CMainFrame *)GetParentFrame())->m_listSyncPara;
	CToolBarCtrl & reToolBarCtrl = ((CMainFrame *)GetParentFrame())->m_wndMainToolBar.GetToolBarCtrl();
	
	BYTE nCurParaArea,nParaCount;
	if (!CFrame::GetParaArea(nCurParaArea,nParaCount))
	{
		StopCommunication();
		return;
	}
	if(!(CFrame::GetAllPara(nCurParaArea,m_arrParaData,m_nNorParaCount)))
	{
		//通信失败
		StopCommunication();
		return;
	}
	int i,nTemp = m_nNorParaCount;
	CString str,strFormat, info;
	double d1;
	for(i=0;i<nTemp;i++)
	{
		switch(m_arrParaData[i].nTranFormat)
		{
		case PARA_TYPE_DATA:
			d1 = pow(0.1,m_arrParaData[i].differData.dataValue.nDecBitNum) * m_arrParaData[i].nParaValue;
			strFormat.Format(_T("%%.%df%%s"),m_arrParaData[i].differData.dataValue.nDecBitNum);
			str.Format(strFormat,d1,(CString)m_arrParaData[i].differData.dataValue.strUnit);
			pList->SetItemText(i,1,str);
			if(0 == strcmp(m_arrParaData[i].strPareName,"待并侧额定电压") || 0 == strcmp(m_arrParaData[i].strPareName,"Incomer Rated Volt.") )
			{//
				m_dRatingOfGVol = d1;
			}
			else if(0 == strcmp(m_arrParaData[i].strPareName,"系统侧额定电压")  || 0 == strcmp(m_arrParaData[i].strPareName,"Busbar Rated Volt.") )
			{
				m_dRatingOfSysVol = d1;
			}
			break;
		case PARA_TYPE_OPTION:
			str = (CString)m_arrParaData[i].differData.arrStrOption[m_arrParaData[i].nParaValue];
			pList->SetItemText(i,1,str);
			info = m_arrParaData[i].strPareName;
			
			if( info.Find(_T("装置控制方式")) >= 0  || info.Find(_T("Device Control Mode")) >= 0 )
			{
				
				m_bTelecontrol = ( (str.Find( _T("遥控")) >=0 || str.Find( _T("Remote")) >=0) ? TRUE : FALSE);
			}
			else if ( info.Find( _T("系统侧应转角")) >= 0 || info.Find( _T("Busbar Turn Angle")) >= 0 )
			{
				if (str == _T("+30°"))
				{
					m_sysTurnAngle = -30;
				}
				else if (str == _T("-30°"))
				{
					m_sysTurnAngle = 30;
				}
				else if (str == _T("0°"))
				{
					m_sysTurnAngle = 0;
				}
			}
			else if ( info.Find(_T( "差频并网电压区间")) >=0 || info.Find(_T( "Sync. Volt. Interval")) >= 0 )  
			{
				if ( str.Find(_T("待并侧高于系统侧")) >= 0 || str.Find(_T("Incomer Higher")) >= 0 )
				{
				 	m_bGVolHigh  = TRUE;
			 	}
				else if (str.Find(_T("待并侧在系统侧两侧")) >= 0 || str.Find(_T("Either Side Higher")) >= 0 )
				{
					m_bGVolHigh  = FALSE;
				}
			}
			else if (info.Find(_T("差频并网频率区间")) >=0 || info.Find(_T( "Sync. Freq. Interval")) >=0 )
			{
				if ( str.Find(_T("待并侧高于系统侧")) >= 0 || str.Find(_T("Incomer Higher")) >=0 )
				{
					m_bGFrqHigh  = TRUE;
				}
				else if (str.Find(_T("待并侧在系统侧两侧")) >= 0 || str.Find(_T("Either Side Higher")) >= 0 )
				{
					m_bGFrqHigh  = FALSE;
				}
			}
//			reToolBarCtrl.EnableButton(IDC_START_SYNC,m_bTelecontrol);
			break;
		default:
			ASSERT(FALSE);
		}
	}
	if (m_nChannelNum == 0)
	{
		str.LoadString( IDS_NONE );
	}
	else if (m_nChannelNum == 0xFF)
	{
		str.LoadString( IDS_MORE_CHANALE );
	}
	else
	{
		info.LoadString( IDS_CHANNEL );
		str.Format(_T("%d %s"), m_nChannelNum, info );
	}
	pList->SetItemText(nTemp,1,str);
}


BOOL CRealMonitorView::StopThread()
{
	m_nIsRun = false;
	BOOL re = FALSE;
//	HANDLE hanle[3]={m_hSyncThread,m_hTelThread,m_hParaThread};
	if(m_hSyncThread != NULL &&WAIT_OBJECT_0 != ::WaitForSingleObject(m_hSyncThread, INFINITE))
	{
		::TerminateThread(m_hSyncThread,0);
		CloseHandle(m_hSyncThread);
		m_hSyncThread = NULL;
	}
	if(m_hTelThread != NULL && WAIT_OBJECT_0 != ::WaitForSingleObject(m_hTelThread, INFINITE))
	{
		::TerminateThread(m_hTelThread,0);
		CloseHandle(m_hTelThread);
		m_hTelThread = NULL;
	}
	if(m_hParaThread != NULL && WAIT_OBJECT_0 != ::WaitForSingleObject(m_hParaThread, INFINITE))
	{
		::TerminateThread(m_hParaThread,0);
		CloseHandle(m_hParaThread);
		m_hParaThread = NULL;
	}
	return TRUE;
}



BOOL CRealMonitorView::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	StopThread();
	CStatusBarEx & statusBar = ((CMainFrame*)GetParentFrame())->m_wndStatusBar;
	statusBar.SetPaneText(1,_T(""));
	statusBar.SetPaneText(2,_T(""));
//	statusBar.SetPaneText(4,_T(""));
//	statusBar.SetPaneText(5,_T(""));
	return CScrollView::DestroyWindow();
}



void CRealMonitorView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
// 	CRect rc;
// 	
	CSize sizeTotal;
// 	GetDesktopWindow()->GetWindowRect(&rc);
// 	GetWindowRect(&m_rcClientView);
// 	rc.right -= WIDTH_PROJECT + WIDTH_OF_RIGHT_BAR+5;
// 	rc.bottom -= 163;
	m_rcClientView = CRect(0,0,cx,cy);
	m_rcRealDraw = CRect(0,0,m_nWideOfRect,m_nHeightOfRect);
	if(m_rcClientView.Width() < m_nWideOfRect || m_rcClientView.Height() < m_nHeightOfRect)
	{
		m_rcClientView = m_rcRealDraw;
	}
	else
	{
		CSize size = m_rcClientView.CenterPoint() - m_rcRealDraw.CenterPoint();
		m_rcRealDraw.SetRect(m_rcRealDraw.left+size.cx,m_rcRealDraw.top + size.cy,
			m_rcRealDraw.right+size.cx,m_rcRealDraw.bottom + size.cy);
	}
	sizeTotal = m_rcClientView.Size();
	InitRectInfo();
	SetScrollSizes(MM_TEXT,sizeTotal);
	//ResizeParentToFit();
}

LRESULT CRealMonitorView::OnSyncReal(BYTE *wParam,int nBufSize)
{
	ASSERT(wParam);
	ASSERT(nBufSize == (m_nStateCount%8 ? 1 : 0) + 18 + m_nStateCount/8);
	//2014-11-20 add 加锁
	m_bGetDataCS.Lock();
	BYTE *pBuf=(BYTE*)wParam;
	BYTE tempBuf[REAL_VIEW_BUFLEN];
	memcpy( tempBuf, pBuf, sizeof(tempBuf) );
    pBuf = tempBuf;
    m_bGetDataCS.Unlock();
	//
	CToolBarCtrl & reToolBarCtrl = ((CMainFrame *)GetParentFrame())->m_wndMainToolBar.GetToolBarCtrl();
	BYTE * pOldBuf = pBuf;
	double dValue1,dValue2;
	CString str;
	int i,j,nIndexOfState;
	BYTE mode;
	dValue1 = *((unsigned short *)pBuf) * 0.1;
	pBuf += 2;
	str.Format(_T("%.1fV"),dValue1);
	m_strSysVoltage.Format(_T("%6s"),str); 

	dValue2 = *((unsigned short *)pBuf) * 0.1;
	pBuf += 2;
	str.Format(_T("%.1fV"),dValue2);
	m_strGVoltage.Format(_T("%6s"),str);

	//计算压差
	dValue1 = dValue2*100/m_dRatingOfGVol-dValue1*100/m_dRatingOfSysVol;
	str.Format(_T("%.1f%%Un"),dValue1);
	m_strVoltageDispersion.Format(_T("%6s"),str); 

	dValue1 = *((unsigned short *)pBuf) * 0.001;
	pBuf += 2;
	str.Format(_T("%.3fHz"),dValue1);
	m_strSysFreq.Format(_T("%6s"),str);

	dValue2 = *((unsigned short *)pBuf) * 0.001;
	pBuf += 2;
	str.Format(_T("%.3fHz"),dValue2);
	m_strGFreq.Format(_T("%6s"),str);

	//计算频差
	dValue2 -= dValue1;
// 	if(dValue2 >9.999)
// 	{
// 		dValue2 = 9.999;
// 	}
// 	else if(dValue2 < -9.999)
// 	{
// 		dValue2 = -9.999;
// 	}
	str.Format(_T("%.3fHz"),dValue2);
	m_strFreqDispersion.Format(_T("%6s"),str); 

	dValue1 = *((short *)pBuf) * 0.1;
	pBuf += 2;
	if (dValue1>360)
	{
		dValue1 -= dValue1/360*360;
	}
// 	if (dValue1 + m_sysTurnAngle > 180)
// 	{
// 		dValue1 = 360 - (dValue1 + m_sysTurnAngle);
// 	}
// 	else if (dValue1 + m_sysTurnAngle < -180)
// 	{
// 		dValue1 = 360 + (dValue1 + m_sysTurnAngle);
// 	}
// 	else
// 	{
// 		dValue1 = dValue1 + m_sysTurnAngle;
// 	}
	//	m_strAngleDispersion.Format(_T("%.1lf°"),dTemp);
	str.Format(_T("%.1f°"),dValue1);
	m_strAngleDispersion.Format(_T("%6s"),str);

	ASSERT(m_nStateCount == (*(unsigned short *)pBuf));
	pBuf += 2;        //状态个数
	nIndexOfState = 0;
	i = m_nStateCount/8;
	
	while(i--)
	{
		j=8;
		mode = 0x01;
		while(j--)
		{
			m_pStateInfo[nIndexOfState++].bValue = (*pBuf & mode) ? true :false;
			mode <<= 1;
		}
		pBuf++;
	}
	mode = 0x01;
	j = m_nStateCount % 8;
	while(j--)
	{
		m_pStateInfo[nIndexOfState++].bValue = (*pBuf & mode) ? true : false;
		mode <<= 1;
	}
//	m_pStateInfo[35].bValue=1;
	pBuf++;
	ASSERT(nIndexOfState == m_nStateCount);
	if(m_nChannelNum != *pBuf)
	{
		//更新通道定值
		CMainFrame * pMainFrame = (CMainFrame *)GetParentFrame();
		m_nChannelNum = *pBuf++;
		//CToolBarCtrl & reToolBarCtrl = pMainFrame->m_wndToolBar.GetToolBarCtrl();
		if(m_nChannelNum == 0xff)
		{
			reToolBarCtrl.EnableButton(IDM_UPDATE_PARA,FALSE);
			((CMainFrame *)GetParentFrame())->ClearSyncList();
		}
		else
		{
//			reToolBarCtrl.EnableButton(IDM_UPDATE_PARA,TRUE);
			UpdateRightBarPara();
		}
		
	}
	else
	{
		//reToolBarCtrl.EnableButton(IDM_UPDATE_PARA,(m_nChannelNum == 0 || m_nChannelNum == 0xff) ? FALSE : TRUE);
		pBuf++;

	}
	if(m_nChannelNum == 0XFF)
	{
		m_strSyncPoint.LoadString( IDS_NONE );
	}
	else if (m_nChannelNum == 0)
	{
		m_strSyncPoint.LoadString( IDS_NONE_PARALLEL );
	}
	else
	{
		m_strSyncPoint = (CString)pBuf;
	}
	//delete [nBufSize] pOldBuf; 2014-11-20 防止内存越界添加
	//更新界面
	UpdateStateInfo();
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	j++;
	return 0;
}


extern LANGID SetThreadUILanguage(LANGID lcid);
DWORD WINAPI CRealMonitorView::SyncDataThread(VOID *pWnd)
{
	CRealMonitorView * lpWnd = (CRealMonitorView *)pWnd;
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
	int nByteCount = 18 + (lpWnd->m_nStateCount%8 ? 1 : 0) + lpWnd->m_nStateCount/8;
	BYTE * pBuf;
	UINT count = 0;
    pBuf = new BYTE[nByteCount]; //2014-11-20 modiy
	while( lpWnd->m_nIsRun )
	{
		if(lpWnd->m_bReqSyncThreadExit)
		{
			TRACE(_T("user exit!\n"));
			delete [] pBuf; 
			return 0;
		}
		Sleep(150);
		 lpWnd->m_bGetDataCS.Lock();
	    
		if(!CFrame::GetSyncRealData(pBuf,nByteCount,lpWnd->m_nStateCount))
		{
			::PostMessage(lpWnd->m_hWnd,UM_COM_FAILED,WPARAM(0),LPARAM(0));
//			SetGrayTool();
		//	TRACE(_T("commination failed!\n"));
			delete [] pBuf;
			lpWnd->m_bGetDataCS.Unlock();
			return 0;
		}
		lpWnd->m_bGetDataCS.Unlock();
		::PostMessage(lpWnd->m_hWnd,UM_SYNC_REAL,(unsigned int)(DWORD)pBuf,nByteCount);
		//TRACE(_T("Sync success %d\n"),count++);
	}
	delete [] pBuf;
	return 0;
}


void CRealMonitorView::UpdateStateInfo()
{
	CMainFrame *pMainFrm = (CMainFrame *)GetParentFrame();
	CListCtrl * pList = &pMainFrm->m_listState;
	int count = 0;
	CString str,strWorkState;
	strWorkState.LoadString( IDS_ACTION );
	int i;
	for(i=0;i<m_nStateCount;i++)
	{
		str = m_pStateInfo[i].bValue ? _T("1"):_T("0");
		pList->SetItemText(i,2,str);
	}
	for(i = m_nStartIndOfWorkMode;i< m_nEndIndOfWorkMode;i++)
	{
		if(m_pStateInfo[i].bValue)
		{
			count++;
			str=m_pStateInfo[i].strStateName;
			str+="  ";
			strWorkState += str;
		}
	}
	//	ASSERT(strWorkState == _T(""));
	((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneInfo(1,IDS_SUB_TIME,SBPS_NORMAL,count*140/*70*/+70);
	((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneText(1,strWorkState);
	m_vecExceptionIndex.erase(m_vecExceptionIndex.begin(),m_vecExceptionIndex.end());
	m_vecPromptIndex.erase(m_vecPromptIndex.begin(),m_vecPromptIndex.end());
	
	for(i = m_nStartIndOfException;i< m_nEndIndOfException;i++)
	{
		if(m_pStateInfo[i].bValue)
		{
			m_vecExceptionIndex.push_back(i);
		}
	}
	CString strStartName;
	for(i = m_nStartIndOfPrompt;i< m_nEndIndOfPrompt;i++)
	{
		if(m_pStateInfo[i].bValue)
		{
			m_vecPromptIndex.push_back(i);
            strStartName = (CString)m_pStateInfo[i].strStateName;
			if(m_bTelecontrol && (strStartName.Find(_T("就绪状态")) >= 0 || strStartName.Find(_T("Ready")) >= 0) )
			{//
				pMainFrm->m_wndMainToolBar.GetToolBarCtrl().EnableButton(IDC_START_SYNC,TRUE);
			}
		}
	}
}

void CRealMonitorView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bConnect)
	{
		::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	}
	m_nCycValue++;
	CString str;
	static UINT index,count=0;
	m_strException.LoadString( IDS_EXCEPTION );
	if(m_vecExceptionIndex.size() > 0)
	{
 		count = m_vecExceptionIndex.size();
// 		index = m_vecExceptionIndex.front();
// 		strException += m_pStateInfo[index].strStateName;
		while (m_vecExceptionIndex.size())
		{
			index = m_vecExceptionIndex.front();
			m_vecExceptionIndex.erase(m_vecExceptionIndex.begin());
			str=m_pStateInfo[index].strStateName;
			str+=_T("   ");
			m_strException += str;
		}
	}
	if(m_vecPromptIndex.size() > 0)
	{
		m_strTipInfo = _T("");
// 		index = m_nCycValue % m_vecPromptIndex.size();
// 		m_strTipInfo = m_pStateInfo[index].strStateName;
		while (m_vecPromptIndex.size())
		{
			index = m_vecPromptIndex.front();
			m_vecPromptIndex.erase(m_vecPromptIndex.begin());
			str=m_pStateInfo[index].strStateName;
			str+=_T("   ");
			m_strTipInfo += str;
		}
	}
	((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneInfo(2,IDS_INDICATOR_TIME,SBPS_NORMAL,count*200/*100*/+70);
	((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneText(2,m_strException);
//	((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetTextColor(2,RGB(255,0,0));
	CScrollView::OnTimer(nIDEvent);
}


void CRealMonitorView::StopCommunication()
{
	m_bConnect = FALSE;
// 	if (m_nCycInfoTimerID == IDC_UPDATE_CYCLE_INFO_TIMER)
// 	{
// 		KillTimer(IDC_UPDATE_CYCLE_INFO_TIMER);
// 		m_nCycInfoTimerID = 0;
// 	}
	m_vecPromptIndex.erase(m_vecPromptIndex.begin(),m_vecPromptIndex.end());
	m_vecExceptionIndex.erase(m_vecExceptionIndex.begin(),m_vecExceptionIndex.end());
	StopThread();
	((CMainFrame *)GetParentFrame())->StopCommunication();
}

void CRealMonitorView::StartCommunication()
{
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	CListCtrl *pList = &((CMainFrame *)GetParentFrame())->m_listSyncPara;
	pList->DeleteAllItems();
	m_nNorParaCount = MAX_NOR_PARA_COUNT;
	m_nSysParaCount = MAX_SYS_PARA_COUNT;
	BYTE nCurParaArea,nParaCount;
	if (!CFrame::GetParaArea(nCurParaArea,nParaCount))
	{
		StopCommunication();
		return;
	}
	if(!(CFrame::GetAllPara(nCurParaArea,m_arrParaData,m_nNorParaCount)))
	{
		//通信失败
		StopCommunication();
		return;
	}
	OnThreadPara((DWORD)TRUE,NULL);
	CString str,strFormat, info;
	int i,nTemp = m_nNorParaCount;
//	double d1;
	for(i=0;i<m_nNorParaCount;i++)
	{
		pList->InsertItem(i,(CString)m_arrParaData[i].strPareName);
		pList->SetItemText(i,1,_T(""));
	}
	info.LoadString( IDS_SYN_CHANANL );
	pList->InsertItem(m_nNorParaCount, info);
	UpdateRightBarPara();
	if(!CFrame::GetStateCount(m_nStateCount,m_nStartIndOfPrompt,m_nEndIndOfPrompt,m_nStartIndOfException,
		m_nEndIndOfException,m_nStartIndOfWorkMode,m_nEndIndOfWorkMode))
	{
		//通信失败
		StopCommunication();
		return;
	}
	m_pStateInfo = new StateInfo[m_nStateCount];
	if(!CFrame::GetAllStateHeadInfo(m_pStateInfo,m_nStateCount))
	{
		//通信失败
		StopCommunication();
		return;
	}
	pMainFrm->m_wndMainToolBar.GetToolBarCtrl().EnableButton(IDM_UPDATE_PARA,TRUE);
	pList = &((CMainFrame *)GetParentFrame())->m_listState;
	pList->DeleteAllItems();
	CString strTypeName = _T("");
	for(i=0;i<m_nStateCount;i++)
	{
		if(strTypeName != (CString)m_pStateInfo[i].strStateType)
		{
			strTypeName = (CString)m_pStateInfo[i].strStateType;
			pList->InsertItem(i,strTypeName);
		}
		else
		{
			pList->InsertItem(i,_T(""));
			
		}
		pList->SetItemText(i,1,(CString)m_pStateInfo[i].strStateName);
		pList->SetItemText(i,2,_T(""));
	}
	m_hSyncThread = ::CreateThread(NULL  , 0 , CRealMonitorView::SyncDataThread, this , 0,  NULL);
	ASSERT(m_hSyncThread);
	
	if(!::SetThreadPriority(m_hSyncThread,THREAD_PRIORITY_HIGHEST))
	{
		CString str;
		str.LoadString( IDS_SET_PRI_ERROR );
		CdMessageBox box( this, str);
	    box.MsgBoxEx();
	}
	m_bConnect = TRUE;
	m_nCycInfoTimerID = SetTimer(IDC_UPDATE_CYCLE_INFO_TIMER,220,NULL);
	ASSERT(m_nCycInfoTimerID == IDC_UPDATE_CYCLE_INFO_TIMER);
	pMainFrm->OpenCommunication();
}

void CRealMonitorView::OnUpdatePara() 
{
	// TODO: Add your command handler code here
	((CMainFrame *)GetParentFrame())->m_wndMainToolBar.GetToolBarCtrl().EnableButton(IDM_UPDATE_PARA,FALSE);
	m_hParaThread = ::CreateThread(NULL,0,CRealMonitorView::ParaDataThread,this,NULL,NULL);

}

void CRealMonitorView::OnStartSync() 
{
	// TODO: Add your command handler code here
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	CToolBarCtrl & reToolBarCtrl = pMainFrm->m_wndMainToolBar.GetToolBarCtrl();
	ASSERT(m_nChannelNum != 0xff);
	CPWDDlg dlg;
	dlg.DoModal();
	if (dlg.m_bCorrect)
	{
		if(m_nTelCode == -1)
		{
			m_nTelCode = EnumStartSync;
			m_hTelThread = ::CreateThread(NULL,0,CRealMonitorView::TelDataThread,this,NULL,NULL);
			reToolBarCtrl.EnableButton(IDC_START_SYNC,FALSE);
		}
		else
		{
			CString str;
			str.LoadString( IDS_DEV_BUSSINESS );
			CdMessageBox box( this, str);
	        box.MsgBoxEx();
		}
	}
}
extern LANGID SetThreadUILanguage(LANGID lcid);
LRESULT CRealMonitorView::OnThreadPara(BOOL bSuccess,LPARAM lParam)
{
	CToolBarCtrl & reToolBarCtrl = ((CMainFrame *)GetParentFrame())->m_wndMainToolBar.GetToolBarCtrl();
	CListCtrl *pList = &((CMainFrame *)GetParentFrame())->m_listSyncPara;
	if(bSuccess)
	{
		int i,nTemp = m_nNorParaCount;
		CString str,strFormat;
		double d1;
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
		for(i=0;i<nTemp;i++)
		{
			switch(m_arrParaData[i].nTranFormat)
			{
			case PARA_TYPE_DATA:
				d1 = pow(0.1,m_arrParaData[i].differData.dataValue.nDecBitNum) * m_arrParaData[i].nParaValue;
				strFormat.Format(_T("%%.%df%%s"),m_arrParaData[i].differData.dataValue.nDecBitNum);
				str.Format(strFormat,d1,(CString)m_arrParaData[i].differData.dataValue.strUnit);
				pList->SetItemText(i,1,str);

				if(0 == strcmp(m_arrParaData[i].strPareName,"待并侧额定电压") || 0 == strcmp(m_arrParaData[i].strPareName,"Incomer Rated Volt.") )
				{//
					m_dRatingOfGVol = d1;
				}
				else if(0 == strcmp(m_arrParaData[i].strPareName,"系统侧额定电压") || 0 == strcmp(m_arrParaData[i].strPareName,"Busbar Rated Volt.") )
				{
					m_dRatingOfSysVol = d1;
				}
				else if (0 == strcmp(m_arrParaData[i].strPareName,"允许压差") || 0 == strcmp(m_arrParaData[i].strPareName,"Allow Volt. Diff") )
				{
					m_dMaxVolDispersion = d1;
					m_dMinVolDispersion = 0 - d1;
				}
				else if (0 == strcmp(m_arrParaData[i].strPareName,"允许频差") || 0 == strcmp(m_arrParaData[i].strPareName,"Allow Freq. Diff"))
				{
					m_dMaxFreqDispersion = d1;
					m_dMinFreqDispersion = 0 - d1;
				}
				break;
			case PARA_TYPE_OPTION:
				pList->SetItemText(i,1,(CString)m_arrParaData[i].differData.arrStrOption[m_arrParaData[i].nParaValue]);
				break;
			default:
				ASSERT(FALSE);
			}
		}
		reToolBarCtrl.EnableButton(IDM_UPDATE_PARA,TRUE);
		m_enableEvent.SetEvent();
	//	::PostMessage(pView->m_hWnd,UM_ENABLE_BUTTON,0,0);
		//TRACE(_T("para message success\n"));
		
	}
	else
	{
		StopCommunication();
	}
	return 0;
}


DWORD WINAPI CRealMonitorView::TelDataThread(VOID *pWnd)
{
	CRealMonitorView * pView = (CRealMonitorView *)pWnd;
	DWORD re = CFrame::TelControl(pView->m_nTelCode);
	if(re == 0xff)
	{
		::PostMessage(pView->m_hWnd,UM_COM_FAILED,re,pView->m_nTelCode);
//		SetGrayTool();
	}
	else
	{
		::PostMessage(pView->m_hWnd,UM_TEL_DATA,re,pView->m_nTelCode);
	}
	return 0;
}
DWORD WINAPI CRealMonitorView::ParaDataThread(VOID *pWnd)
{
	CRealMonitorView * pView = (CRealMonitorView *)pWnd;
	ASSERT(pView->m_nChannelNum != 0xff);
	
	BYTE nCurParaArea,nParaCount;
	if (!CFrame::GetParaArea(nCurParaArea,nParaCount))
	{
		//通信失败
		::PostMessage(pView->m_hWnd,UM_PARA_DATA,(DWORD)FALSE,NULL);
		TRACE(_T("Para update failed\n"));
		return 0;
	}
	if(CFrame::GetAllPara(nCurParaArea,pView->m_arrParaData,pView->m_nNorParaCount))
	{
		m_enableEvent.ResetEvent();
		
		//通信成功
		::PostMessage(pView->m_hWnd,UM_PARA_DATA,(DWORD)TRUE,NULL);
		WaitForSingleObject(m_enableEvent.m_hObject,INFINITE);
		TRACE(_T("Para update success\n"));
	}
	else
	{
		//通信失败
		::PostMessage(pView->m_hWnd,UM_PARA_DATA,(DWORD)FALSE,NULL);
		TRACE(_T("Para update failed\n"));
	}
	return 0;
}

void CRealMonitorView::OnRestart() 
{
	// TODO: Add your command handler code here
	BYTE ret = 1;
	CPWDDlg dlg;
	dlg.DoModal();
	if (dlg.m_bCorrect)
	{
		ret = CFrame::TelControl(EnumReset);
		theApp.m_eRestart.SetEvent();
	}
	else
	{
		return;
	}
	CString str;
	if(ret == 0)
	{
		str.LoadString( IDS_DEVS_RESTART_OK );
	}
	else
	{
	    str.LoadString( IDS_DEVS_RESTART_ERROR );
	}
	CdMessageBox box( this, str);
	box.MsgBoxEx();
}
LRESULT CRealMonitorView::OnThreadTelControl(BYTE nTelConRst,UINT nTelConCmd)
{
	CToolBarCtrl & reToolBarCtrl = ((CMainFrame *)GetParentFrame())->m_wndMainToolBar.GetToolBarCtrl();
	m_nTelCode = 0xffffffff;
	if(nTelConRst == 0)
	{
		//遥控成功
		switch(nTelConCmd)
		{
		case EnumStartSync:
			TRACE(_T("tel-control start sync \n"));
			reToolBarCtrl.EnableButton(IDC_START_SYNC,TRUE);
			break;
		case EnumReset:
			TRACE(_T("tel-control reset\n"));
			reToolBarCtrl.EnableButton(IDC_RESTART,TRUE);
			break;
		}
	}
	else if(nTelConRst == 0xff)
	{
		StopCommunication();
	}
	else
	{
		TRACE(_T("tel-control failed \n"));
	}
	return 0;
}

void CRealMonitorView::OnDestroy() 
{
	StopThread();
	CScrollView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}
LRESULT CRealMonitorView::OnComFailed(BYTE nTelConRst,UINT nTelConCmd)
{
	SetGrayTool();
	return 0;
}

VOID CRealMonitorView::SetGrayTool()
{
	CMainFrame::StopCommunication();
}
