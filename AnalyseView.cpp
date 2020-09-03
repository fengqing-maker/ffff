// AnalyseView.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "AnalyseView.h"
#include "WareFileData.h"
#include "MainFrm.h"
#include <math.h>
#include <queue>
#include "dMessageBox.h"

using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnalyseView

IMPLEMENT_DYNCREATE(CAnalyseView, CScrollView)

CAnalyseView::CAnalyseView()
{
}

CAnalyseView::~CAnalyseView()
{

}


BEGIN_MESSAGE_MAP(CAnalyseView, CCurveView)
	//{{AFX_MSG_MAP(CAnalyseView)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalyseView drawing




/////////////////////////////////////////////////////////////////////////////
// CAnalyseView diagnostics

#ifdef _DEBUG
void CAnalyseView::AssertValid() const
{
	CCurveView::AssertValid();
}

void CAnalyseView::Dump(CDumpContext& dc) const
{
	CCurveView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnalyseView message handlers


// Point CAnalyseView::MapPointToLogic(POINT p1, Point &p2,double cellOfY,POINT origitalPoint,UINT nWidthOfCell)
// {
// 	int xWidthOfCell=ANALOG_HEIGHTH_COONDIATE/CELL_COUNT;
// 	p2.x=(double)(p1.x- origitalPoint.x)/(xWidthOfCell*m_dMulti)*m_nCellOfX;
// 	p2.y=(double)(origitalPoint.y-p1.y)/nWidthOfCell/cellOfY;
// 	return p2;
// }


void CAnalyseView::ChangeCurve(Curve *pCurve,long count,bool isUnitedFile,int diffMilli,char ps)
{
	ASSERT(pCurve!=NULL);
	ASSERT(ps=='p'||ps=='s'||ps=='P'||ps=='S');
	
	m_diffMilli=diffMilli;
	m_isUnitedFile=isUnitedFile;
	CCurveView::Release(m_pCurve,m_nCurveCount);
	if(ps=='p'||ps=='P') m_strValueType=_T("一次值");
	else m_strValueType=_T("二次值");
	m_nCurveCount=pCurve[0].unit == _T("°") ? count : count+1;
	m_pCurve=new Curve[m_nCurveCount];
	int i,j,temp_n,nSmallerI;
	queue<int> qVolIndex,qCurrentIndex;
	double dMaxOfX=0,dMaxVolY=0,dMaxCurrentY=0,dMaxVal,dVolMinRange,dCurrentMinRange;
	double dValueOfCellY;
	wchar_t cUint;
	CString str;
	nSmallerI= m_nCurveCount == count ? 1 : 0;
	for(i = 1; i<m_nCurveCount; i++,nSmallerI++)
	{
		m_pCurve[i].lpPoint=new Point[pCurve[nSmallerI].count];
		if(m_pCurve[i].lpPoint == NULL)
		{
			CCurveView::Release(m_pCurve,m_nCurveCount);
			m_pCurve = NULL;
			return;
		}
		m_pCurve[i].lpStartSecPoint=new Point[pCurve[nSmallerI].startCount];
		if(m_pCurve[i].lpStartSecPoint == NULL)
		{
			CCurveView::Release(m_pCurve,m_nCurveCount);
			m_pCurve = NULL;
			return;
		}
		m_pCurve[i].lpReturnSecPoint=new Point[pCurve[nSmallerI].returnCount];
		if(m_pCurve[i].lpReturnSecPoint == NULL)
		{
			CCurveView::Release(m_pCurve,m_nCurveCount);
			m_pCurve = NULL;
			return;
		}
		m_pCurve[i].count=pCurve[nSmallerI].count;
		m_pCurve[i].nSubItemIndex = pCurve[nSmallerI].nSubItemIndex;
		m_pCurve[i].nChannelIndex = pCurve[nSmallerI].nChannelIndex;
		m_pCurve[i].firstSampleTime=pCurve[nSmallerI].firstSampleTime;
		m_pCurve[i].milliSecondOfFirst=pCurve[nSmallerI].milliSecondOfFirst;
		m_pCurve[i].startTime=pCurve[nSmallerI].startTime;
		m_pCurve[i].startCount = pCurve[nSmallerI].startCount;
		m_pCurve[i].returnCount = pCurve[nSmallerI].returnCount;
		m_pCurve[i].color=m_colorArray[i%COLOR_NUM];
		m_pCurve[i].minRange=pCurve[nSmallerI].minRange;
		m_pCurve[i].bIsDigital = pCurve[nSmallerI].bIsDigital;
		m_pCurve[i].bIsOverlap = pCurve[nSmallerI].bIsOverlap;
		m_pCurve[i].nPointInCircle = pCurve[nSmallerI].nPointInCircle;
		m_pCurve[i].name = pCurve[nSmallerI].name;
		m_pCurve[i].unit =  pCurve[nSmallerI].unit;
		m_pCurve[i].valueOfCellY = pCurve[nSmallerI].valueOfCellY;
		//这里将所有连续的电压量（或电流）使用同样的Y坐标刻度变比，是为了让用户从界面上就能看出各条曲线的
		//电压（或电流）量的大小，这里只有开入开出量才会设置Y轴上单位刻度所代表的逻辑值（valueOfCellY），
		//模拟量的valueOfCellY为0，也就是未设置，一个坐标系中，Y轴的正负轴各有十个单位刻度，每个单位刻度
		//可以表示一个浮点数的逻辑值，所以很小的数据，也可以在界面得到放大显示，minRange，用于设置坐标系
		//的显示逻辑值范围的最小值
		
				if(m_diffMilli)
				{
					if(m_diffMilli<=1500)
					{
						intersect=m_diffMilli-1500;
						str.Format(_T("△t =%dms"),m_diffMilli);
						m_strDota=str;
					}
					else
					{
						intersect=500;
						m_dota=m_diffMilli-1500;
						str.Format(_T("△t=%dms"),m_dota);
						m_strDota=str;
						m_isLabel=true;
					}
				}
		if(m_pCurve[i].bIsDigital)
		{
			
			for(j=0;j<pCurve[nSmallerI].count;j++)
			{
				if(m_isUnitedFile&&j>pCurve[nSmallerI].count/2)
				{
					m_pCurve[i].lpPoint[j]=pCurve[nSmallerI].lpPoint[j];
					m_pCurve[i].lpPoint[j].x+=intersect;
				}
				else
				{
					m_pCurve[i].lpPoint[j]=pCurve[nSmallerI].lpPoint[j];
				}
				if(m_pCurve[i].lpPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpPoint[j].x;
				}
			}
			for (j=0;j<m_pCurve[i].startCount;j++)
			{
				m_pCurve[i].lpStartSecPoint[j] = pCurve[nSmallerI].lpStartSecPoint[j];
			}
			for (j=0;j<m_pCurve[i].returnCount;j++)
			{
				m_pCurve[i].lpReturnSecPoint[j] = pCurve[nSmallerI].lpReturnSecPoint[j];
			}
		}
		else
		{
			cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
			if(cUint == TEXT('a') || cUint == TEXT('A'))
			{
				dCurrentMinRange = m_pCurve[nSmallerI].minRange;
				qCurrentIndex.push(i);
				for(j=0;j<pCurve[nSmallerI].count;j++)
				{
					if(m_isUnitedFile&&j>pCurve[nSmallerI].count/2)
					{
						m_pCurve[i].lpPoint[j]=pCurve[nSmallerI].lpPoint[j];
						m_pCurve[i].lpPoint[j].x+=intersect;
					}
					else
					{
						m_pCurve[i].lpPoint[j]=pCurve[nSmallerI].lpPoint[j];
					}
					if(DoubleCmp(m_pCurve[i].lpPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpPoint[j].y);
					}
				}
				for (j = 0; j< m_pCurve[i].startCount ; j++)
				{
					m_pCurve[i].lpStartSecPoint[j]=pCurve[nSmallerI].lpStartSecPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStartSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStartSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStartSecPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpStartSecPoint[j].y);
					}
				}
				for (j = 0; j< m_pCurve[i].returnCount ; j++)
				{
					m_pCurve[i].lpReturnSecPoint[j]=pCurve[nSmallerI].lpReturnSecPoint[j];
					if(DoubleCmp(m_pCurve[i].lpReturnSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpReturnSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpReturnSecPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpReturnSecPoint[j].y);
					}
				}
			}
			else if(cUint == TEXT('v') || cUint == TEXT('V'))
			{
				dVolMinRange = m_pCurve[i].minRange;
				qVolIndex.push(i);
				for(j=0;j<pCurve[i].count;j++)
				{
					
					if(m_isUnitedFile&&j>pCurve[nSmallerI].count/2)
					{
						m_pCurve[i].lpPoint[j]=pCurve[nSmallerI].lpPoint[j];
						m_pCurve[i].lpPoint[j].x+=intersect;
					}
					else
					{
						m_pCurve[i].lpPoint[j]=pCurve[nSmallerI].lpPoint[j];
					}
					if(DoubleCmp(m_pCurve[i].lpPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpPoint[j].y);
					}
				}
				for (j = 0; j< m_pCurve[i].startCount ; j++)
				{
					m_pCurve[i].lpStartSecPoint[j]=pCurve[nSmallerI].lpStartSecPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStartSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStartSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStartSecPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpStartSecPoint[j].y);
					}
				}
				for (j = 0; j< m_pCurve[i].returnCount ; j++)
				{
					m_pCurve[i].lpReturnSecPoint[j]=pCurve[nSmallerI].lpReturnSecPoint[j];
					if(DoubleCmp(m_pCurve[i].lpReturnSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpReturnSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpReturnSecPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpReturnSecPoint[j].y);
					}
				}
			}
			else
			{
				dMaxVal = 0;
				for(j=0;j<pCurve[nSmallerI].count;j++)
				{
					if(m_isUnitedFile&&j>pCurve[nSmallerI].count/2)
					{
						m_pCurve[i].lpPoint[j]=pCurve[nSmallerI].lpPoint[j];
						m_pCurve[i].lpPoint[j].x+=intersect;
					}
					else
					{
						m_pCurve[i].lpPoint[j]=pCurve[nSmallerI].lpPoint[j];
					}
					if(DoubleCmp(m_pCurve[i].lpPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpPoint[j].y)-dMaxVal)>0)
					{
						dMaxVal = fabs(m_pCurve[i].lpPoint[j].y);
					}
				}
				if(dMaxVal<m_pCurve[i].minRange) dMaxVal=m_pCurve[i].minRange;
				temp_n=(long)dMaxVal;
				//如果tempY大于temp_n
				if(DoubleCmp(dMaxVal-temp_n)>0)
				{
					temp_n++;
				}
				dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
				if(DoubleCmp(dMaxVal-1) ==0)
				{
					dValueOfCellY = dMaxVal/10;
				}
				m_pCurve[i].valueOfCellY = dValueOfCellY;
			}
		}
	}
	//计算电流中的dValueOfCellY
	if(dMaxCurrentY<dCurrentMinRange) dMaxCurrentY=dCurrentMinRange;
	temp_n=(long)dMaxCurrentY;
	//如果tempY大于temp_n
	if(DoubleCmp(dMaxCurrentY-temp_n)>0)
	{
		temp_n++;
	}
	dValueOfCellY=(double)temp_n/10;
	if(DoubleCmp(dMaxCurrentY-1) ==0)
	{
		dValueOfCellY = dMaxCurrentY/10;
	}
	while(!qCurrentIndex.empty())
	{
		m_pCurve[qCurrentIndex.front()].valueOfCellY = dValueOfCellY;
		qCurrentIndex.pop();
	}
	//计算电压中的dValueOfCellY
	dValueOfCellY;
	if(dMaxVolY<dVolMinRange) dMaxVolY=dVolMinRange;
	temp_n=(long)dMaxVolY;
	//如果tempY大于temp_n
	if(DoubleCmp(dMaxVolY-temp_n)>0)
	{
		temp_n++;
	}
	dValueOfCellY=(double)temp_n/10;
	if(DoubleCmp(dMaxVolY-1) ==0)
	{
		dValueOfCellY = dMaxVolY/10;
	}
	while(!qVolIndex.empty())
	{
		m_pCurve[qVolIndex.front()].valueOfCellY = dValueOfCellY;
		qVolIndex.pop();
	}
	m_nMaxOfX=(long)dMaxOfX;
	if(m_nMaxOfX<dMaxOfX) m_nMaxOfX++;
	//计算角差线
	if(!CalculateAngleCurve())
	{
		CString str;
		str.LoadString( IDS_ERROR_CALDIFF );
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		return;
	}
	m_pCurve[0].color = m_pCurve[2].color;
	m_pCurve[2].color = m_pCurve[1].color;
	m_pCurve[1].color = m_colorArray[0];
	//计算区域大小
	m_dMulti=1;
	m_dNewMulti = 1;
	m_dWindowSizeOfX = m_nMaxOfX;
	m_dInitDisPosOfx = 0-m_pCurve[0].startTime;
	SetCoorHeight(20,160);
	CRect rc;
	GetDesktopWindow()->GetWindowRect(rc);
	rc.right -= WIDTH_PROJECT+CHANNEL_INFO_WIDTH+20;
	double d1=m_clientRc.Width()-CHANNEL_INFO_WIDTH;
	m_dMulti = rc.Width()*m_nMaxOfX/(d1*m_dWindowSizeOfX);
	m_dMinMulti = m_dMulti;
	m_dNewMulti = m_dMulti;
	SetCoorHeight(20,160);
	POINT origital,tempPoint;
	Point rightPoint;
	origital.x=m_rectCurve.left;
	origital.y=0;
	rightPoint.x=m_dInitDisPosOfx+m_pCurve[0].startTime;
	origital.y=0;
	MapPoint(rightPoint,tempPoint,0,origital,0);
	tempPoint.x -= m_rectCurve.left;
	ScrollToPosition(tempPoint);
}
void CAnalyseView::ChangePrintCurve(Curve * pCurve,int nCurveCount,CRect rcPrint,double dPrintTime,char cPS)
{
	m_rcPrintCurve = rcPrint;
	ASSERT(pCurve!=NULL);
	ASSERT(cPS=='p'||cPS=='s'||cPS=='P'||cPS=='S');
	if(cPS=='p'||cPS=='P') m_strValueType=_T("一次值");
	else m_strValueType=_T("二次值");
	m_nCurveCount=nCurveCount+1;
	m_pCurve=new Curve[m_nCurveCount];
	int i,j,temp_n,nSmallerI;
	queue<int> qVolIndex,qCurrentIndex;
	double dMaxOfX=0,dMaxVolY=0,dMaxCurrentY=0,dVolMinRange,dCurrentMinRange;
	double dValueOfCellY,dMaxVal;
	wchar_t cUint;
	for(i=1,nSmallerI=0; i<m_nCurveCount; i++,nSmallerI++)
	{
		m_pCurve[i].lpPoint=new Point[pCurve[nSmallerI].count];
		if(m_pCurve[i].lpPoint == NULL)
		{
			CCurveView::Release(m_pCurve,m_nCurveCount);
			m_pCurve = NULL;
			return;
		}
		m_pCurve[i].count=pCurve[nSmallerI].count;
		m_pCurve[i].nSubItemIndex = pCurve[nSmallerI].nSubItemIndex;
		m_pCurve[i].nChannelIndex = pCurve[nSmallerI].nChannelIndex;
		m_pCurve[i].firstSampleTime=pCurve[nSmallerI].firstSampleTime;
		m_pCurve[i].milliSecondOfFirst=pCurve[nSmallerI].milliSecondOfFirst;
		m_pCurve[i].startTime=pCurve[nSmallerI].startTime;
		m_pCurve[i].color=m_colorArray[i%COLOR_NUM];
		m_pCurve[i].minRange=pCurve[nSmallerI].minRange;
		m_pCurve[i].bIsDigital = pCurve[nSmallerI].bIsDigital;
		m_pCurve[i].bIsOverlap = pCurve[nSmallerI].bIsOverlap;
		m_pCurve[i].nPointInCircle = pCurve[nSmallerI].nPointInCircle;
		m_pCurve[i].name = pCurve[nSmallerI].name;
		m_pCurve[i].unit =  pCurve[nSmallerI].unit;
		m_pCurve[i].valueOfCellY = pCurve[nSmallerI].valueOfCellY;
		//这里将所有连续的电压量（或电流）使用同样的Y坐标刻度变比，是为了让用户从界面上就能看出各条曲线的
		//电压（或电流）量的大小，这里只有开入开出量才会设置Y轴上单位刻度所代表的逻辑值（valueOfCellY），
		//模拟量的valueOfCellY为0，也就是未设置，一个坐标系中，Y轴的正负轴各有十个单位刻度，每个单位刻度
		//可以表示一个浮点数的逻辑值，所以很小的数据，也可以在界面得到放大显示，minRange，用于设置坐标系
		//的显示逻辑值范围的最小值
		if(m_pCurve[i].bIsDigital)
		{
			
			for(j=0;j<pCurve[nSmallerI].count;j++)
			{
				m_pCurve[i].lpPoint[j] = pCurve[nSmallerI].lpPoint[j];
				if(m_pCurve[i].lpPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpPoint[j].x;
				}
			}
		}
		else
		{
			cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
			if(cUint == TEXT('a') || cUint == TEXT('A'))
			{
				dCurrentMinRange = m_pCurve[i].minRange;
				qCurrentIndex.push(i);
				for(j=0;j<pCurve[nSmallerI].count;j++)
				{
					m_pCurve[i].lpPoint[j] = pCurve[nSmallerI].lpPoint[j];
					if(DoubleCmp(m_pCurve[i].lpPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpPoint[j].y);
					}
				}
			}
			else if(cUint == TEXT('v') || cUint == TEXT('V'))
			{
				dVolMinRange = m_pCurve[i].minRange;
				qVolIndex.push(i);
				for(j=0;j<pCurve[nSmallerI].count;j++)
				{
					m_pCurve[i].lpPoint[j] = pCurve[nSmallerI].lpPoint[j];
					if(DoubleCmp(m_pCurve[i].lpPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpPoint[j].y);
					}
				}
			}
			else
			{
				dMaxVal = 0;
				for(j=0;j<pCurve[nSmallerI].count;j++)
				{
					m_pCurve[i].lpPoint[j] = pCurve[nSmallerI].lpPoint[j];
					if(DoubleCmp(m_pCurve[i].lpPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpPoint[j].y)-dMaxVal)>0)
					{
						dMaxVal = fabs(m_pCurve[i].lpPoint[j].y);
					}
				}
				if(dMaxVal<m_pCurve[i].minRange) dMaxVal=m_pCurve[i].minRange;
				temp_n=(long)dMaxVal;
				//如果tempY大于temp_n
				if(DoubleCmp(dMaxVal-temp_n)>0)
				{
					temp_n++;
				}
				dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
				if(DoubleCmp(dMaxVal-1) ==0)
				{
					dValueOfCellY = dMaxVal/10;
				}
				m_pCurve[i].valueOfCellY = dValueOfCellY;
			}
		}
	}
	//计算电流中的dValueOfCellY
	if(dMaxCurrentY<dCurrentMinRange) dMaxCurrentY=dCurrentMinRange;
	temp_n=(long)dMaxCurrentY;
	//如果tempY大于temp_n
	if(DoubleCmp(dMaxCurrentY-temp_n)>0)
	{
		temp_n++;
	}
	dValueOfCellY=(double)temp_n/10;
	if(DoubleCmp(dMaxCurrentY-1) ==0)
	{
		dValueOfCellY = dMaxCurrentY/10;
	}
	while(!qCurrentIndex.empty())
	{
		m_pCurve[qCurrentIndex.front()].valueOfCellY = dValueOfCellY;
		qCurrentIndex.pop();
	}
	//计算电压中的dValueOfCellY
	dValueOfCellY;
	if(dMaxVolY<dVolMinRange) dMaxVolY=dVolMinRange;
	temp_n=(long)dMaxVolY;
	//如果tempY大于temp_n
	if(DoubleCmp(dMaxVolY-temp_n)>0)
	{
		temp_n++;
	}
	dValueOfCellY=(double)temp_n/10;
	if(DoubleCmp(dMaxVolY-1) ==0)
	{
		dValueOfCellY = dMaxVolY/10;
	}
	while(!qVolIndex.empty())
	{
		m_pCurve[qVolIndex.front()].valueOfCellY = dValueOfCellY;
		qVolIndex.pop();
	}
	m_nMaxOfX=(long)dMaxOfX;
	if(m_nMaxOfX<dMaxOfX) m_nMaxOfX++;
	//计算角差线
	if(!CalculateAngleCurve())
	{
		CString str;
		str.LoadString( IDS_ERROR_CALDIFF );
		CdMessageBox box( theApp.m_pMainWnd, str);
		box.MsgBoxEx();
		return;
	}
	m_pCurve[0].color = m_pCurve[2].color;
	m_pCurve[2].color = m_pCurve[1].color;
	m_pCurve[1].color = m_colorArray[0];
	//计算区域大小
	m_dMulti=1;
	m_dNewMulti = 1;
	SetPrintCoorHeight(0-ROW_HEIGHT*DIGITAL_DIS_LINE,0-ROW_HEIGHT*ANALYSE_DIS_LINE,rcPrint);
	double d1=(m_clientRc.Width()-CHANNEL_INFO_WIDTH_PRINT)*dPrintTime/m_nMaxOfX;
	m_dMulti = (rcPrint.Width()-CHANNEL_INFO_WIDTH_PRINT)/d1;
	m_dMinMulti = m_dMulti;
	m_dNewMulti = m_dMulti;
	SetPrintCoorHeight(0-ROW_HEIGHT*DIGITAL_DIS_LINE,0-ROW_HEIGHT*ANALYSE_DIS_LINE,rcPrint);
}
	

void CAnalyseView::DrawLine(CDC *pDC)
{
	POINT temp,origitalPoint;
	Point pLogic;
	origitalPoint.x=m_rectCurve.left;
	UINT nWidthOfCell,nTopOfCoor;  //nTopOfCoor当前坐标系的顶坐标
	long i,j,k;
	CPen *pPen;
	CPen pen(PS_SOLID,1,m_colorStartLine);
	CPen pen2(PS_DASH,1,m_colorStartLine);
	CPen pen3(PS_SOLID,1,RGB(255,0,255));
	CPen *oldPen;
	Point point;
	oldPen=pDC->SelectObject(&pen);
	nTopOfCoor = m_rectCurve.top;
	CString str;
	int diff=0;
	bool isOverlap=false;
	if(m_isUnitedFile&&intersect<0)
	{
		isOverlap=true;
	}
	if((m_nFirstDisCurve == 0) && (m_nCurveCount > 0) && (!m_pCurve[0].bIsDigital))
	{
		origitalPoint.y = nTopOfCoor + m_nHeightOfAnalogCoor/2;
		nWidthOfCell = m_nHeightOfAnalogCoor/CELL_COUNT;
		for(i=m_nFirstDisCurve;i<=1;i++)
		{
			if(m_isUnitedFile)
			{
				diff=int(intersect/20.0*(m_pCurve[i].count/(m_pCurve[i].count/(m_pCurve[i].nPointInCircle*2))/2));
				if(diff<0)
					isOverlap=true;
			}
			if(m_pCurve[i].bIsDigital) //发现第一个数字量时退出，因为模拟量都在最前面
			{
				break;
			}
			if(m_pCurve[i].count<2)
			{
				continue;
			}
			pPen=new CPen(PS_SOLID,1,m_pCurve[i].color);
			pDC->SelectObject(pPen);
			j=0;
			temp.x=m_curRect.left+CHANNEL_INFO_WIDTH;
			MapPointToLogic(temp,pLogic,0,origitalPoint,0);//
			long index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
			j=index;
			k=index;
			temp.x=m_curRect.right;
			MapPointToLogic(temp,pLogic,0,origitalPoint,0);
			index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
			if(index >= m_pCurve[i].count)
			{
				index = m_pCurve[i].count-1;
			}
			if(m_isUnitedFile&&!isOverlap&&k>=m_pCurve[i].count/2&&k<=m_pCurve[i].count/2)
			{
				pDC->MoveTo(MapPoint(m_pCurve[i].lpPoint[4800],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell));
			}
			else
				pDC->MoveTo(MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell));
			for(j=j+1;j<=index;j++)
			{
				if(m_isUnitedFile&&isOverlap&&j>=(m_pCurve[i].count/2)&&j<(m_pCurve[i].count/2)-diff)
				{
					continue;
				}
				MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);	
				
				if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
				{
					pDC->LineTo(temp);
					if(!m_isUnitedFile||(m_isUnitedFile&&(j<m_pCurve[i].count/2||j>m_pCurve[i].count/2)))
					{
						if(temp.x <=CHANNEL_INFO_WIDTH)
						{
							temp.x =CHANNEL_INFO_WIDTH;
							pDC->MoveTo(temp);
						}
						else if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
						{
							pDC->MoveTo(temp);
						}
					}
					
					else if(j==m_pCurve[i].count/2)
					{
						if(isOverlap&&((k>m_pCurve[i].count/2+diff)&&(k<m_pCurve[i].count/2)))
						{
							MapPoint(m_pCurve[i].lpPoint[j-diff-(m_pCurve[i].count/2-k)],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
							if(temp.x <=CHANNEL_INFO_WIDTH)
							{
								temp.x =CHANNEL_INFO_WIDTH;
								pDC->MoveTo(temp);
							}
							else
							{
								if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
								{
									pDC->MoveTo(temp);
								}
							}
						}
						else if(!isOverlap&&k>=m_pCurve[i].count/2&&k<m_pCurve[i].count/2+diff-1)
						{
							pDC->MoveTo(MapPoint(m_pCurve[i].lpPoint[m_pCurve[i].count/2+1],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell));
						}
						else
						{
							MapPoint(m_pCurve[i].lpPoint[j+1],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
							if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
							{
								pDC->MoveTo(temp);
							}
						}
					}
				}
			}
			delete pPen;
			pPen = NULL;
		}
	}
	nTopOfCoor = m_rectCurve.top;
	long nTemp = m_nFirstDisCurve+2;
	for(i=2;i<nTemp;i++)
	{
		if(m_pCurve[i].bIsDigital)
		{
			nTopOfCoor += m_nHeightOfDigitalCoor;
		}
		else
		{
			nTopOfCoor += m_nHeightOfAnalogCoor;
		}
	}
	for(i=m_nFirstDisCurve+2;i<=m_nLastDisCurve;i++)
	{
		pPen=new CPen(PS_SOLID,1,m_pCurve[i].color);
		pDC->SelectObject(pPen);
		if(m_pCurve[i].count<2) continue;
		
			if(m_isUnitedFile)
			{
				diff=int(intersect/20.0*(m_pCurve[i].count/(m_pCurve[i].count/(m_pCurve[i].nPointInCircle*2))/2));
				if(diff<0)
					isOverlap=true;
			}
		j=0;
		if(m_pCurve[i].bIsDigital)
		{
			origitalPoint.y = nTopOfCoor + m_nHeightOfDigitalCoor/2; 
			nWidthOfCell = m_nHeightOfDigitalCoor/CELL_COUNT;
		}
		else
		{
			origitalPoint.y = nTopOfCoor + m_nHeightOfAnalogCoor/2;
			nWidthOfCell = m_nHeightOfAnalogCoor/CELL_COUNT;
		}
		temp.x=m_curRect.left+CHANNEL_INFO_WIDTH;
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);//
		long index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
		j=index;
		k=index;
		temp.x=m_curRect.right;
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);
		int startP=BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
		index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
		if(index >= m_pCurve[i].count)
		{
			index = m_pCurve[i].count-1;
		}
		if(m_isUnitedFile&&!isOverlap&&k>=m_pCurve[i].count/2&&k<=m_pCurve[i].count/2)
		{
			temp=MapPoint(m_pCurve[i].lpPoint[m_pCurve[i].count/2+1],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
			if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
			{
				pDC->MoveTo(temp);
			}
		}
		else
			pDC->MoveTo(MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell));
		if(m_pCurve[i].bIsDigital)
		{
				for(j=j+1;j<=index;j++)
				{
					POINT befPoint,midPoint[2];
					befPoint = temp;
					MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
					//对于从0变1或从1变0的开入量的点，会在它们的中点处添加两点midPoint0,midPoint1,设变化前后两点
					//分别为p1,p2,则midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
					//p1.x)/2,并且系统会顺次连接p1-midPoint0-midPoint1-p2
					if(m_isUnitedFile&&isOverlap&&j>=(m_pCurve[i].count/2)&&j<(m_pCurve[i].count/2)-diff)
					{
						continue;
					}
					else if(m_isUnitedFile&&j==(m_pCurve[i].count/2+1))
					{
						if(!isOverlap)
						{
							MapPoint(m_pCurve[i].lpPoint[j+1],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
							if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
							{
								pDC->MoveTo(temp);
							}
						}
						if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
						{
							pDC->MoveTo(temp);
						}
					}
					else if(befPoint.y != temp.y)
					{
						midPoint[0].x =midPoint[1].x = befPoint.x + ((temp.x - befPoint.x)>>1);
						midPoint[0].y = befPoint.y;
						midPoint[1].y = temp.y;
						if(midPoint[0].x >= CHANNEL_INFO_WIDTH && midPoint[0].x < m_curRect.Width())
						{
							pDC->LineTo(midPoint[0]);
							pDC->MoveTo(midPoint[0]);
						}
						if(midPoint[1].x >= CHANNEL_INFO_WIDTH && midPoint[1].x < m_curRect.Width())
						{
							pDC->LineTo(midPoint[1]);
							pDC->MoveTo(midPoint[1]);
						}
					}
					else if(temp.x >= CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
					{
						pDC->LineTo(temp);
						pDC->MoveTo(temp);
					}
				}
		}
		else
		{
			for(j=j+1;j<=index;j++)
			{
				if(m_isUnitedFile&&isOverlap&&j>=(m_pCurve[i].count/2)&&j<(m_pCurve[i].count/2)-diff)
				{
						continue;
				}
				MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);	
				
				if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
				{
					pDC->LineTo(temp);
					if(!m_isUnitedFile||(m_isUnitedFile&&(j<m_pCurve[i].count/2||j>m_pCurve[i].count/2)))
					{
						if(temp.x <=CHANNEL_INFO_WIDTH)
						{
							temp.x =CHANNEL_INFO_WIDTH;
							pDC->MoveTo(temp);
						}
						else if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
						{
							pDC->MoveTo(temp);
						}
					}
					
					else if(j==m_pCurve[i].count/2)
					{
						if(isOverlap&&((k>m_pCurve[i].count/2+diff)&&(k<m_pCurve[i].count/2)))
						{
							MapPoint(m_pCurve[i].lpPoint[j-diff-(m_pCurve[i].count/2-k)],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
							if(temp.x <=CHANNEL_INFO_WIDTH)
							{
								temp.x =CHANNEL_INFO_WIDTH;
								pDC->MoveTo(temp);
							}
							else
							{
								if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
								{
									pDC->MoveTo(temp);
								}
							}
						}
						else if(!isOverlap&&k>=m_pCurve[i].count/2&&k<m_pCurve[i].count/2+diff-1)
						{
							pDC->MoveTo(MapPoint(m_pCurve[i].lpPoint[m_pCurve[i].count/2+1],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell));
						}
						else
						{
							MapPoint(m_pCurve[i].lpPoint[j+1],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
							if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
							{
								pDC->MoveTo(temp);
							}
						}
					}
				}
			}
		}
		pPen=new CPen(PS_SOLID,1,RGB(224,215,27));
		pDC->SelectObject(pPen);//使用黄色
		if (m_pCurve[i].bIsOverlap)//画起始段和返回段曲线
		{
			MapPoint(m_pCurve[i].lpStartSecPoint[startP],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
			if(temp.x >= CHANNEL_INFO_WIDTH-2 && temp.x <= m_curRect.Width())
			{
				pDC->MoveTo(temp);
			}
			for (int k = 0 ; k < m_pCurve[i].startCount ; k++)
			{
				MapPoint(m_pCurve[i].lpStartSecPoint[k+1],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
				{
					pDC->LineTo(temp);
				}
				if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
				{
					pDC->MoveTo(temp);
				}
			}
			MapPoint(m_pCurve[i].lpReturnSecPoint[0],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
			if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
			{
				pDC->MoveTo(temp);
			}
			for (k = 0 ; k < m_pCurve[i].returnCount ; k++)
			{
				MapPoint(m_pCurve[i].lpReturnSecPoint[k+1],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
				{
					pDC->LineTo(temp);
				}
				if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
				{
					pDC->MoveTo(temp);
				}
			}
		}
		//绘制录波启动时记时刻的红色虚线
		point.x=m_pCurve[i].startTime;
		point.y=0;
		MapPoint(point,temp,0,origitalPoint,nWidthOfCell);
		if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
		{
			pDC->SelectObject(&pen2);
			pDC->MoveTo(temp.x,nTopOfCoor);
			pDC->LineTo(temp.x,nTopOfCoor+nWidthOfCell*CELL_COUNT);
		}
		
		
		CPen clickPen(PS_SOLID,1,m_crossLineColor);
		//绘制测量时间差的基线
		point.x = m_dValueOfClick;
		MapPoint(point,temp,0,origitalPoint,nWidthOfCell);
		if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
		{
			pDC->SelectObject(&clickPen);
			pDC->MoveTo(temp.x,nTopOfCoor);
			pDC->LineTo(temp.x,nTopOfCoor+nWidthOfCell*CELL_COUNT);
			pDC->SelectObject(pPen);
		}
		nTopOfCoor += nWidthOfCell*CELL_COUNT;
		delete pPen;
		pPen = NULL;
	}
	pDC->SelectObject(oldPen);
}



void CAnalyseView::DrawGridLine(CDC *pDC)
{
	CPen pen1(PS_SOLID,1,m_coordinateColor);
	CPen pen2(PS_DASH,1,m_coordinateColor);
	CPen pen3(PS_SOLID,1,RGB(233,233,233));
	CPen penGird(PS_DASH,1,m_colorGirdLine);
	CPen *oldpen=pDC->SelectObject(&pen1);
	CBrush brush(RGB(233,233,233));
	//y轴
	pDC->MoveTo(m_rectCurve.left,m_rectCurve.top);
	pDC->LineTo(m_rectCurve.left,m_rectCurve.bottom);
	//横向线条
	UINT nWidthOfCell,nTopOfCoor;  //nTopOfCoor当前坐标系的顶坐标
	int i,nGirdHeight,nDisCoorHeight = m_nHeightOfAnalogCoor/2;
	UINT nTopOfY=m_rectCurve.top;
	
	pDC->SelectObject(&pen2);
	m_nFirstDisCurve = -1;
	m_nLastDisCurve = -1;
	UINT nIntervalOfCoorMid,nIntervalOfCoorTop=0; 
	POINT temp1,temp2,origitalPoint;
	origitalPoint.x=CHANNEL_INFO_WIDTH;
	nTopOfCoor = m_rectCurve.top;
	for(i=2;i<m_nCurveCount;i++)
	{
		if(nIntervalOfCoorTop > m_curRect.bottom && m_nLastDisCurve == -1)
		{
			if(i>0)
			{
				m_nLastDisCurve = i-1;
				break;
			}
		}
		if(m_pCurve[i].bIsDigital)
		{
			origitalPoint.y = nTopOfCoor + m_nHeightOfDigitalCoor/2; 
			nWidthOfCell = m_nHeightOfDigitalCoor/CELL_COUNT;
		}
		else
		{
			origitalPoint.y = nTopOfCoor + m_nHeightOfAnalogCoor/2;
			nWidthOfCell = m_nHeightOfAnalogCoor/CELL_COUNT;
		}
		nIntervalOfCoorMid = nIntervalOfCoorTop;
		if(m_pCurve[i].bIsDigital)
		{
			nIntervalOfCoorMid += (UINT)m_nHeightOfDigitalCoor/2;
			nIntervalOfCoorTop += (UINT)m_nHeightOfDigitalCoor;
		}
		else
		{
			nIntervalOfCoorMid += (UINT)m_nHeightOfAnalogCoor/2;
			nIntervalOfCoorTop += (UINT)m_nHeightOfAnalogCoor;
		}
		if(nIntervalOfCoorTop > m_curRect.top && m_nFirstDisCurve == -1)
		{
			m_nFirstDisCurve = i;
		}
		if(m_isLabel)
		{
			MapPoint(m_pCurve[i].lpPoint[m_pCurve[i].count/2],temp1,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);			
			if(temp1.x >= CHANNEL_INFO_WIDTH)
			{
				if(temp1.x > m_curRect.Width())
					temp1.x = m_curRect.Width();
				pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid);
				pDC->LineTo(temp1.x,nTopOfY+nIntervalOfCoorMid);
			}
			MapPoint(m_pCurve[i].lpPoint[m_pCurve[i].count/2+1],temp2,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
			if(temp2.x < m_rectCurve.left)
				temp2.x = m_rectCurve.left;
			pDC->MoveTo(temp2.x,nTopOfY+nIntervalOfCoorMid);
			pDC->LineTo(m_rectCurve.left+m_curRect.Width(),nTopOfY+nIntervalOfCoorMid);
			
		}
		else
		{
			pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid);
			pDC->LineTo(m_rectCurve.left+m_curRect.Width(),nTopOfY+nIntervalOfCoorMid);
		}
		if(!m_pCurve[i].bIsDigital)
		{
			nGirdHeight = m_nMinGirdHeight;
			pDC->SelectObject(&penGird);
			while(nGirdHeight <= nDisCoorHeight)
			{
				
				if(m_isLabel)
				{
					if(temp1.x >= CHANNEL_INFO_WIDTH)
					{	
						pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid-nGirdHeight);
						pDC->LineTo(temp1.x,nTopOfY+nIntervalOfCoorMid-nGirdHeight);
					}
					pDC->MoveTo(temp2.x,nTopOfY+nIntervalOfCoorMid-nGirdHeight);
					pDC->LineTo(m_rectCurve.left+m_curRect.Width(),nTopOfY+nIntervalOfCoorMid-nGirdHeight);
					
					if(temp1.x >= CHANNEL_INFO_WIDTH)
					{
						pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid+nGirdHeight);
						pDC->LineTo(temp1.x,nTopOfY+nIntervalOfCoorMid+nGirdHeight);
					}
					pDC->MoveTo(temp2.x,nTopOfY+nIntervalOfCoorMid+nGirdHeight);
					pDC->LineTo(m_rectCurve.left+m_curRect.Width(),nTopOfY+nIntervalOfCoorMid+nGirdHeight);
					
				}
				else
				{
					pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid-nGirdHeight);
					pDC->LineTo(m_rectCurve.left+m_curRect.Width(),nTopOfY+nIntervalOfCoorMid-nGirdHeight);
					pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid+nGirdHeight);
					pDC->LineTo(m_rectCurve.left+m_curRect.Width(),nTopOfY+nIntervalOfCoorMid+nGirdHeight);
				}
				nGirdHeight += m_nMinGirdHeight;
			}
			pDC->SelectObject(&pen2);
			
		}
	}
	if(m_isLabel)
	{
		pDC->SelectObject(pen3);
		pDC->SelectObject(&brush);
		pDC->Rectangle(temp1.x,m_rectCurve.top,temp2.x,m_rectCurve.bottom);
	}
	m_nFirstDisCurve -= 2;
	if(m_nLastDisCurve == -1)
	{
		m_nLastDisCurve = m_nCurveCount-1;
	}
	pDC->SelectObject(oldpen);
}

void CAnalyseView::DrawCurve(CDC *pDC)
{
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	int totalTime = pMainFrm->m_wareFileData.m_nCycleCount * 20;
	//m_Hscroll.ShowScrollBar(FALSE);
	CPoint pointScrollPos = GetScrollPosition();
	GetClientRect(&m_curRect);
	m_curRect.left += pointScrollPos.x;
	m_curRect.right += pointScrollPos.x;
	m_curRect.top += pointScrollPos.y;
	m_curRect.bottom += pointScrollPos.y;
	m_mousePoint.x += pointScrollPos.x;
	m_mousePoint.y += pointScrollPos.y;
	ASSERT_VALID(pDC);
	UNUSED(pDC);
	CSize logicTotalSize;
	logicTotalSize = GetTotalSize();
	logicTotalSize.cx = (logicTotalSize.cx > 2*m_curRect.Width())?2*m_curRect.Width():logicTotalSize.cx;
	CRect rcDC(0,0,logicTotalSize.cx,logicTotalSize.cy);
	CPen *pOldPen;
	CBrush *pOldBrush;
	long oldmode;
	CDC MemDC; //首先定义一个显示设备对象
	CBitmap MemBitmap;//定义一个位图对象
	
	//随后建立与屏幕显示兼容的内存显示设备
	VERIFY(MemDC.CreateCompatibleDC(NULL));
	//这时还不能绘图，因为没有地方画 ^_^
	//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
	VERIFY(MemBitmap.CreateCompatibleBitmap(pDC,rcDC.Width(),rcDC.Height()));
	//将位图选入到内存显示设备中
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);
	//先用背景色将位图清除干净，这里我用的是白色作为背景
	//你也可以用自己应该用的颜色
	MemDC.FillSolidRect(rcDC,RGB(255,255,255));
	MemDC.FillSolidRect(m_rectCurve,m_bkColor);
	//绘图
	
	
	
	oldmode=MemDC.SetBkMode(TRANSPARENT);
	SetMulti(&MemDC);
	MemDC.SetBkMode(oldmode);
		
	CFont font,*pOldFont;
	font.CreatePointFont(100,_T("宋体"));
	pOldFont=MemDC.SelectObject(&font);
	CString str=_T("名  称:");
	int i;
	CSize size;
	size=MemDC.GetTextExtent(str);
	int nTempHeightAnalogCoor= m_nHeightOfAnalogCoor/3;
	i=0;
	if(m_mousePoint.x>0)
	{
 		CPen crossPen(PS_SOLID,1,m_crossLineColor);
 		pOldPen=MemDC.SelectObject(&crossPen);
		MemDC.MoveTo(m_mousePoint.x-m_curRect.left,m_rectCurve.top);
		MemDC.LineTo(m_mousePoint.x-m_curRect.left,m_rectCurve.bottom);
		MemDC.SelectObject(pOldPen);
		Point pLogic;
		POINT origital;
		origital.x = m_rectCurve.left;
		origital.y = m_nHeightOfAnalogCoor/2;
		MapPointToLogic(m_mousePoint,pLogic,0,origital,m_nHeightOfAnalogCoor/CELL_COUNT);//获取鼠标点的横坐标
		double milliSecond;
		if(m_isLabel&&pLogic.x<2000&&pLogic.x>=1500&&m_dValueOfClick<1500)
		{
			milliSecond=1500+m_pCurve[0].milliSecondOfFirst;
		}
		else if(m_isLabel&&pLogic.x>=2000&&m_dValueOfClick<1500)
		{
			milliSecond=pLogic.x+ m_dota - 500 +m_pCurve[0].milliSecondOfFirst;
		}
		else if(m_isLabel&&pLogic.x<2000&&pLogic.x>=1500&&m_dValueOfClick>=2000)
		{
			milliSecond=m_pCurve[0].milliSecondOfFirst+m_dota+1500;
		}
		else if(m_isLabel&&m_dValueOfClick>=2000&&pLogic.x<=1500)
		{
			milliSecond=1500+m_pCurve[0].milliSecondOfFirst-(1500-pLogic.x);
		}
		else if(m_isLabel&&m_dValueOfClick>=2000)
		{
			milliSecond=m_pCurve[0].milliSecondOfFirst+pLogic.x+m_dota-500;
		}
		else
		{
			milliSecond=pLogic.x+m_pCurve[0].milliSecondOfFirst;
		}
		COleDateTimeSpan span;
		span.SetDateTimeSpan(0,0,0,milliSecond/1000);
		COleDateTime dateTime;
		dateTime=m_pCurve[0].firstSampleTime+span;
		//double d1=milliSecond-(int)milliSecond/1000*1000;
		str.Format(_T("%s:%.1fms"),dateTime.Format(_T("%Y-%m-%d %H:%M:%S")),milliSecond-(int)milliSecond/1000*1000);
		((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneText(1,str);
		if(m_isLabel&&pLogic.x>2000&&m_dValueOfClick<1500)
		{
			milliSecond = pLogic.x + m_dota - 500 - m_dValueOfClick;
		}
		else if(m_isLabel&&pLogic.x<1500&&m_dValueOfClick>2000)
		{
			milliSecond = pLogic.x - m_dota + 500 - m_dValueOfClick;
		}
		else
		{
			milliSecond = pLogic.x - m_dValueOfClick;
		}
		str.Format(_T("时间差：%.1fms"),milliSecond);
		((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneText(2,str);
		//SetStatusTextMid(str);
		long index;
		UINT nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //坐标系的顶纵坐标
		origital.x = m_rectCurve.left;
		for(i=0;i<m_nCurveCount;i++)
		{
			
			
			if(m_pCurve[i].bIsDigital)
			{
				origital.y = nTopToCoor + m_nHeightOfDigitalCoor/2;
				nWidthOfCoor = m_nHeightOfDigitalCoor;
			}
			else
			{
				if(i<3)
				{
					origital.y = nTopToCoor + nTempHeightAnalogCoor/2;
					nWidthOfCoor = nTempHeightAnalogCoor;
				}
				else
				{
					origital.y = nTopToCoor + m_nHeightOfAnalogCoor/2;
					nWidthOfCoor = m_nHeightOfAnalogCoor;
				}
				
			}
			CRect rc(m_clientRc.left+5,origital.y-5,m_clientRc.left+15,origital.y+5);
			CPen pen(PS_SOLID,1,m_pCurve[i].color),strPen(PS_SOLID,1,RGB(0,0,0));
			CBrush brush;
			if(m_bMultiColorDis)
			{
				brush.CreateSolidBrush(m_pCurve[i].color);
				pOldPen=MemDC.SelectObject(&pen);
				pOldBrush=MemDC.SelectObject(&brush);
				MemDC.Ellipse(&rc);
				MemDC.SelectObject(pOldBrush);
				MemDC.SelectObject(pOldPen);
				rc.left=m_clientRc.left+20;
				rc.top=origital.y-size.cy/2;
				rc.right=m_clientRc.left+CHANNEL_INFO_WIDTH;
				rc.bottom=origital.y+size.cy/2;
			}
			else
			{
				rc.left=m_clientRc.left+5;
				rc.top=origital.y-size.cy/2;
				rc.right=m_clientRc.left+CHANNEL_INFO_WIDTH;
				rc.bottom=origital.y+size.cy/2;
			}
			oldmode=MemDC.SetBkMode(TRANSPARENT);
			pOldPen=MemDC.SelectObject(&strPen);
			
			index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
			CString temp;
			if(m_pCurve[i].bIsDigital)
			{
				if(DoubleCmp(m_pCurve[i].lpPoint[index].y))
				{
					temp.Format(_T("%s:1"),m_pCurve[i].name);
				}
				else
				{
					temp.Format(_T("%s:0"),m_pCurve[i].name);
				}
			}
			else
			{
				double dFreq,dSwing,dAngle;
				if(!GetFreqSwingAngleStart(dFreq,dSwing,dAngle,i,index))
				{
					dAngle = 0;
					dFreq = 0;
					dSwing = 0;
				}
				if(m_pCurve[i].unit == _T("°"))
				{
					temp.Format(_T("%s:%.1f%s"),m_pCurve[i].name,m_pCurve[i].lpPoint[index].y,m_pCurve[i].unit);
				}
				else
				{
					TCHAR cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
					if(cUint == TEXT('a') || cUint == TEXT('A') || cUint == TEXT('v') || cUint == TEXT('V'))
					{
						rc.top -= size.cy/2+5;
						rc.bottom += size.cy/2+5;
						temp.Format(_T("%s:%.3f%s\r\n∠%5.1f°标么值:%.3f"),m_pCurve[i].name,
							m_pCurve[i].lpPoint[index].y,m_pCurve[i].unit,dAngle,m_pCurve[i].lpPoint[index].extendValue);
					}
					else
					{
						temp.Format(_T("%s:%.3f%s"),m_pCurve[i].name,m_pCurve[i].lpPoint[index].y,m_pCurve[i].unit);
					}
				}
			}
			m_bMultiColorDis ? str.Format(_T("%s"),temp) : str.Format(_T("%d.%s"),i,temp);
			MemDC.DrawText(str,&rc,DT_LEFT);
			MemDC.SetBkMode(oldmode);
			MemDC.SelectObject(pOldPen);
			nTopToCoor += nWidthOfCoor;
		}
	}
	else
	{
		POINT origital;
		UINT nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //坐标系的顶纵坐标
		origital.x = m_rectCurve.left;
		for(i=0;i<m_nCurveCount;i++)
		{
			if(m_pCurve[i].bIsDigital)
			{
				origital.y = nTopToCoor + m_nHeightOfDigitalCoor/2;
				nWidthOfCoor = m_nHeightOfDigitalCoor;
			}
			else
			{
				if(i<3)
				{
					origital.y = nTopToCoor + nTempHeightAnalogCoor/2;
					nWidthOfCoor = nTempHeightAnalogCoor;
				}
				else
				{
					origital.y = nTopToCoor + m_nHeightOfAnalogCoor/2;
					nWidthOfCoor = m_nHeightOfAnalogCoor;
				}
				
			}

			CPen pen(PS_SOLID,1,m_pCurve[i].color),strPen(PS_SOLID,1,RGB(0,0,0));
			CBrush brush;
			CRect rc(m_clientRc.left+5,origital.y-5,
				m_clientRc.left+15,origital.y+5);
			if(m_bMultiColorDis)
			{
				brush.CreateSolidBrush(m_pCurve[i].color);
				pOldPen=MemDC.SelectObject(&pen);
				pOldBrush=MemDC.SelectObject(&brush);
				MemDC.Ellipse(&rc);
				MemDC.SelectObject(pOldBrush);
				MemDC.SelectObject(pOldPen);
				rc.left=m_clientRc.left+20;
				rc.top=origital.y-size.cy/2;
				rc.right=m_clientRc.left+120+size.cx;
				rc.bottom=origital.y+size.cy/2;
				str.Format(_T("%s:"),m_pCurve[i].name);
			}
			else
			{
				rc.left=m_clientRc.left+5;
				rc.top=origital.y-size.cy/2;
				rc.right=m_clientRc.left+120+size.cx;
				rc.bottom=origital.y+size.cy/2;
				str.Format(_T("%d.%s:"),i,m_pCurve[i].name);
			}
			
			
			oldmode=MemDC.SetBkMode(TRANSPARENT);
			pOldPen=MemDC.SelectObject(&strPen);
			
			
			MemDC.DrawText(str,&rc,DT_LEFT);
			MemDC.SetBkMode(oldmode);
			MemDC.SelectObject(pOldPen);
			nTopToCoor += nWidthOfCoor;
		}
	}
	MemDC.SelectObject(pOldFont);
//  将内存中的图拷贝到屏幕上进行显示
// 	if(!bIsHscroll)
// 	{
// 		
// 	}
	DrawScale(&MemDC,m_isLabel,m_dota);
	pDC->BitBlt(0,0,m_curRect.Width(),logicTotalSize.cy-m_curRect.top,
		&MemDC,0,m_curRect.top,SRCCOPY);
// 	pDC->BitBlt(CHANNEL_INFO_WIDTH+1,0,m_curRect.Width(),m_curRect.Height()
// 		&MemDC,m_curRect.left+CHANNEL_INFO_WIDTH+1,m_curRect.top,SRCCOPY);
//  绘图完成后的清理
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	m_mousePoint.x -= pointScrollPos.x;
	m_mousePoint.y -= pointScrollPos.y;
}













void CAnalyseView::SetCoorHeight(int nDigitalHeight /*= DIGITAL_HEIGHTH_COONDIATE*/,
							   int nAnalogHeight/* = ANALOG_HEIGHTH_COONDIATE*/)
{
	m_nHeightOfDigitalCoor = nDigitalHeight;
	m_nHeightOfAnalogCoor = nAnalogHeight;
	POINT origital,tempPoint;
	Point rightPoint;
	int nWidth,nHeight=nHeight=m_nHeightOfRCScale*2,i;
	origital.x=CHANNEL_INFO_WIDTH;
	origital.y=0;
	rightPoint.x=m_nMaxOfX;
	origital.y=0;
	//计算最左点的横坐标
	MapPoint(rightPoint,tempPoint,0,origital,0);
	nWidth = tempPoint.x+m_curRect.left;
	for(i=2;i<m_nCurveCount;i++)
	{
		if(m_pCurve[i].bIsDigital)
		{
			nHeight += nDigitalHeight;
		}
		else
		{
			nHeight += nAnalogHeight;
		}
	}
	CRect rc;
	GetClientRect(&m_clientRc);
	rc = m_clientRc;
	ClientToScreen(&rc);
	if(rc.left < CHANNEL_INFO_WIDTH + PROJECT_WARE_FILE)
	{
		m_clientRc.right = m_clientRc.left + nWidth;
	}
	else if(m_clientRc.Width() < (nWidth+20))
	{
		m_clientRc.right = m_clientRc.left + nWidth;
	}
	if(rc.top <HEIGHT_INFO)
	{
		m_clientRc.bottom = m_clientRc.top + nHeight;
	}
	else if((nHeight+20) > m_clientRc.Height())
	{
		m_clientRc.bottom = m_clientRc.top + nHeight;
	}

	m_rectCurve=CRect(m_clientRc.left+CHANNEL_INFO_WIDTH,m_clientRc.top+m_nHeightOfRCScale,
		m_clientRc.right,m_clientRc.top+nHeight-m_nHeightOfRCScale);
	GetDocument()->UpdateAllViews(NULL);
}




BOOL CAnalyseView::CGetAppointCurve(CWareFileData * pWareFileData)
{
	ASSERT(pWareFileData != NULL);
	int i,j,k,countOfAppoint,countOfChannel;
	CString str,strTemp;
	AnalogChannel *pAnalogChannel;
	//nChannelNum是AnalogChannel中的channelNum
	//而nChannelCount所有指定通道之和
	int nChannelNum,nChannelCount=0,nAnalogChannelCount=0,indexOfChannel; 
	for(i=0;i<pWareFileData->m_nSubItemCount;i++)
	{
		switch(pWareFileData->m_arrPSubItem[i]->GetDataType())
		{
		case ANALOG_TYPE:
		case DIGITAL_IN_TYPE:
		case DIGITAL_OUT_TYPE:
			countOfAppoint = pWareFileData->m_arrPSubItem[i]->GetAppointCount();
			nChannelCount += countOfAppoint;
			if(pWareFileData->m_arrPSubItem[i]->GetDataType() == ANALOG_TYPE)
			{
				nAnalogChannelCount += countOfAppoint;
			}
			break;
		}
	}
	if(nChannelCount == 0)
	{
		return FALSE;
	}
	ASSERT(pWareFileData->m_pCurve == NULL);
	pWareFileData->m_pCurve = new Curve[nChannelCount];
	if(pWareFileData->m_pCurve == NULL)
	{
		return FALSE;
	}
	pWareFileData->m_nCurveCount = nChannelCount;
	indexOfChannel = 0;
	for(i=0;i<pWareFileData->m_nSubItemCount;i++)
	{
		if(pWareFileData->m_arrPSubItem[i]->GetDataType() != ANALOG_TYPE)
		{
			continue;
		}
		countOfChannel = pWareFileData->m_arrPSubItem[i]->GetChannelCount();
		countOfAppoint = pWareFileData->m_arrPSubItem[i]->GetAppointCount();
		for(j=0;j<countOfAppoint;j++)
		{
			nChannelNum = pWareFileData->m_arrPSubItem[i]->GetAppointChannelIndex(j);
			for(k=0;k<countOfChannel;k++)
			{
				pAnalogChannel = (AnalogChannel *)pWareFileData->m_arrPSubItem[i]->GetChannelData(k);
				ASSERT(pAnalogChannel!=NULL);
				if(nChannelNum == pAnalogChannel->channelNum)
				{
					pWareFileData->GetCurve(indexOfChannel,i,k,'s',1,CURVE_ANALYSE_40A);
					indexOfChannel++;
					break;
				}
			}
			
		}
	}
	//还原为以前的状态
	for(i=0;i<pWareFileData->m_nSubItemCount;i++)
	{
		
		switch (pWareFileData->m_arrPSubItem[i]->GetDataType())
		{
		case DIGITAL_IN_TYPE:
		case DIGITAL_OUT_TYPE:
			countOfChannel = pWareFileData->m_arrPSubItem[i]->GetChannelCount();
			countOfAppoint = pWareFileData->m_arrPSubItem[i]->GetAppointCount();
			for(j=0;j<countOfAppoint;j++)
			{
				nChannelNum = pWareFileData->m_arrPSubItem[i]->GetAppointChannelIndex(j);
				ASSERT(nChannelNum >= 0);
				pWareFileData->GetCurve(indexOfChannel,i,nChannelNum,'s',1);
				indexOfChannel++;
			}
			break;
		}
	}
	return TRUE;
}

BOOL CAnalyseView::CalculateAngleCurve()
{
	int i;
	int indexOfAngle=0;
	if((m_pCurve[1].nPointInCircle != m_pCurve[2].nPointInCircle) ||
		(m_pCurve[1].count != m_pCurve[2].count))
	{
		CCurveView::Release(m_pCurve,m_nCurveCount);
		m_nCurveCount = 0;
		m_pCurve=NULL;
		return FALSE;
	}
	
	double dAngle[2],dFreq,dSwing;
	double samplePeriod=m_pCurve[1].lpPoint[1].x;
	m_pCurve[indexOfAngle].bIsDigital =FALSE;
	m_pCurve[indexOfAngle].bIsOverlap =FALSE;
	m_pCurve[indexOfAngle].lpPoint = 
		new Point[m_pCurve[1].count];

	if(m_pCurve[indexOfAngle].lpPoint == NULL)
	{
		
		CCurveView::Release(m_pCurve,m_nCurveCount);
		m_nCurveCount = 0;
		m_pCurve=NULL;
		m_pCurve = NULL;
		m_nCurveCount = 0;
		return FALSE;
	}
	m_pCurve[indexOfAngle].count = m_pCurve[1].count;
	m_pCurve[indexOfAngle].firstSampleTime = m_pCurve[1].firstSampleTime;
	m_pCurve[indexOfAngle].fmaxOfY = m_pCurve[1].fmaxOfY;
	m_pCurve[indexOfAngle].milliSecondOfFirst = m_pCurve[1].milliSecondOfFirst;
	m_pCurve[indexOfAngle].minRange = 180;
	m_pCurve[indexOfAngle].name = _T("相角差");
	m_pCurve[indexOfAngle].nPointInCircle = m_pCurve[1].nPointInCircle;
	m_pCurve[indexOfAngle].startTime = m_pCurve[1].startTime;
	m_pCurve[indexOfAngle].unit = _T("°");
	m_pCurve[indexOfAngle].valueOfCellY = m_pCurve[indexOfAngle].minRange/10;
	for(i=0;i<m_pCurve[1].count;i++)
	{
		m_pCurve[indexOfAngle].lpPoint[i].x = samplePeriod*i;
		if(m_isUnitedFile&&i>m_pCurve[1].count/2)
		{
			m_pCurve[indexOfAngle].lpPoint[i].x += intersect;
		}
		if(GetFreqSwingAngleStart(dFreq,dSwing,dAngle[0],1,i) && 
			GetFreqSwingAngleStart(dFreq,dSwing,dAngle[1],2,i))
		{
			//较正转角在坐标平移中的误差
			double d1 = m_pCurve[2].dTurnAngle*m_pCurve[2].nPointInCircle/360;
			int indexTurn = (DoubleCmp(d1)<0)?(d1-0.5):(d1+0.5);
			d1 = m_pCurve[2].dTurnAngle - (double)indexTurn*360/
				m_pCurve[2].nPointInCircle;
			d1 =dAngle[0]-dAngle[1] - d1;
			int i1=(int)d1;
			d1 = i1%360+d1-i1;
			if(DoubleCmp(d1-180)>0)
			{
				d1 -= 360;
			}
			else if(DoubleCmp(180+d1)<0)
			{
				d1 +=360;
			}
			m_pCurve[indexOfAngle].lpPoint[i].y = d1;
		}
		else
		{
			m_pCurve[indexOfAngle].lpPoint[i].y=0;
		}
	}
	return TRUE;
}
void CAnalyseView::PrintLine(CDC *pDC)
{
	POINT temp,origitalPoint;
	origitalPoint.x=m_rectCurve.left;
	long i,j;
	CPen *pPen;
	CPen pen(PS_SOLID,1,m_colorStartLine);
	CPen pen2(PS_DASH,1,m_colorStartLine);
	CPen *pOldPen;
	Point point;
	pOldPen=pDC->SelectObject(&pen);
	int nWidthOfCell,nTopOfCoor;  //nTopOfCoor当前坐标系的顶坐标
	nTopOfCoor = m_rectCurve.top;
	Point pLogic;
	if((m_nFirstDisCurve == 0) && (m_nCurveCount > 0) && (!m_pCurve[0].bIsDigital))
	{
		origitalPoint.y = nTopOfCoor + m_nHeightOfAnalogCoor/2;
		nWidthOfCell = m_nHeightOfAnalogCoor/CELL_COUNT;
		for(i=m_nFirstDisCurve;i<=1;i++)
		{
			if(m_pCurve[i].bIsDigital) //发现第一个数字量时退出，因为模拟量都在最前面
			{
				break;
			}
			if(m_pCurve[i].count<2)
			{
				continue;
			}
			pPen=new CPen(PS_SOLID,1,m_pCurve[i].color);
			pDC->SelectObject(pPen);
			j=0;
			temp.x=m_curRect.left+CHANNEL_INFO_WIDTH_PRINT;
			MapPointToLogic(temp,pLogic,0,origitalPoint,0);//
			long index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
			j=index;
			temp.x=m_curRect.right;
			MapPointToLogic(temp,pLogic,0,origitalPoint,0);
			index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
			if(index >= m_pCurve[i].count)
			{
				index = m_pCurve[i].count-1;
			}
			pDC->MoveTo(MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell));
			for(j=j+1;j<=index;j++)
			{
				MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				pDC->LineTo(temp);
				pDC->MoveTo(temp);
			}
			delete pPen;
			pPen = NULL;
		}
	}
	nTopOfCoor = m_rectCurve.top;
	for(i=(m_nFirstDisCurve==0)?2:m_nFirstDisCurve;i<=m_nLastDisCurve;i++)
	{
		pPen=new CPen(PS_SOLID,1,m_pCurve[i].color);
		pDC->SelectObject(pPen);
		if(m_pCurve[i].count<2) continue;
		j=0;
		if(m_pCurve[i].bIsDigital)
		{
			origitalPoint.y = nTopOfCoor + m_nHeightOfDigitalCoor/2; 
			nWidthOfCell = m_nHeightOfDigitalCoor/CELL_COUNT;
		}
		else
		{
			origitalPoint.y = nTopOfCoor + m_nHeightOfAnalogCoor/2;
			nWidthOfCell = m_nHeightOfAnalogCoor/CELL_COUNT;
		}
		temp.x=m_curRect.left+CHANNEL_INFO_WIDTH_PRINT;
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);//
		long index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
		j=index;
		temp.x=m_curRect.right;
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);
		index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
		if(index >= m_pCurve[i].count)
		{
			index = m_pCurve[i].count-1;
		}
		pDC->MoveTo(MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell));
		
		if(m_pCurve[i].bIsDigital)
		{
			for(j=j+1;j<=index;j++)
			{
				POINT befPoint,midPoint[2];
				befPoint = temp;
				MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				//对于从0变1或从1变0的开入量的点，会在它们的中点处添加两点midPoint0,midPoint1,设变化前后两点
				//分别为p1,p2,则midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
				//p1.x)/2,并且系统会顺次连接p1-midPoint0-midPoint1-p2
				if(befPoint.y != temp.y)
				{
					midPoint[0].x =midPoint[1].x = befPoint.x + ((temp.x - befPoint.x)>>1);
					midPoint[0].y = befPoint.y;
					midPoint[1].y = temp.y;
					pDC->LineTo(midPoint[0]);
					pDC->MoveTo(midPoint[0]);
					pDC->LineTo(midPoint[1]);
					pDC->MoveTo(midPoint[1]);
				}
				pDC->LineTo(temp);
				pDC->MoveTo(temp);
			}
		}
		else
		{
			for(j=j+1;j<=index;j++)
			{
				MapPoint(m_pCurve[i].lpPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				pDC->LineTo(temp);
				pDC->MoveTo(temp);
			}
		}
		//绘制录波启动时记时刻的红色虚线
// 		point.x=m_pCurve[i].startTime;
// 		point.y=0;
// 		MapPoint(point,temp,0,origitalPoint,nWidthOfCell);
// 		if(temp.x > m_rectCurve.left && (temp.x+m_rcPrintCurve.left) < m_curRect.Width())
// 		{
// 			pDC->SelectObject(&pen2);
// 			pDC->MoveTo(temp.x,nTopOfCoor);
// 			pDC->LineTo(temp.x,nTopOfCoor+nWidthOfCell*CELL_COUNT);
// 		}
		nTopOfCoor += nWidthOfCell*CELL_COUNT;
		delete pPen;
		pPen = NULL;
	}
	pDC->SelectObject(pOldPen);
}
void CAnalyseView::PrintGirdLine(CDC *pDC, int nPrintFirstCurve)
{
	const int nMinGirdHeight = -200;
	CPen pen1(PS_SOLID,1,m_coordinateColor);
	CPen pen2(PS_DASH,1,m_coordinateColor);
	CPen penGird(PS_DASH,1,m_colorGirdLine);
	CPen *pOldPen=pDC->SelectObject(&pen1);
	//y轴
	pDC->MoveTo(m_rectCurve.left,m_rectCurve.top);
	pDC->LineTo(m_rectCurve.left,m_rectCurve.bottom);
	//横向线条
	int i,nGirdHeight,nDisCoorHeight = m_nHeightOfAnalogCoor/2;
	int nTopOfY=m_rectCurve.top;
	int nPosOfX = m_rectCurve.left+m_curRect.Width()-CHANNEL_INFO_WIDTH_PRINT;
	pDC->SelectObject(&pen2);
	m_nFirstDisCurve = nPrintFirstCurve;
	m_nLastDisCurve = -1;
	int nIntervalOfCoorMid,nIntervalOfCoorTop=0;
	i = nPrintFirstCurve > 2 ? nPrintFirstCurve:2;
	for(;i<m_nCurveCount;i++)
	{
		if((nTopOfY + nIntervalOfCoorTop-ROW_HEIGHT) <= m_curRect.bottom && m_nLastDisCurve == -1)
		{
			if(i>0)
			{
				m_nLastDisCurve = i-1;
				break;
			}
		}
		nIntervalOfCoorMid = nIntervalOfCoorTop;
		if(m_pCurve[i].bIsDigital)
		{
			nIntervalOfCoorMid += m_nHeightOfDigitalCoor/2;
			nIntervalOfCoorTop += m_nHeightOfDigitalCoor;
		}
		else
		{
			nIntervalOfCoorMid += m_nHeightOfAnalogCoor/2;
			nIntervalOfCoorTop += m_nHeightOfAnalogCoor;
		}
		if(nIntervalOfCoorTop > m_curRect.top && m_nFirstDisCurve == -1)
		{
			m_nFirstDisCurve = i;
		}
		pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid);
		pDC->LineTo(nPosOfX,nTopOfY+nIntervalOfCoorMid);
		if(!m_pCurve[i].bIsDigital)
		{
			nGirdHeight = nMinGirdHeight;
			pDC->SelectObject(&penGird);
			while(nGirdHeight > nDisCoorHeight)
			{
				pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid-nGirdHeight);
				pDC->LineTo(nPosOfX,nTopOfY+nIntervalOfCoorMid-nGirdHeight);
				pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid+nGirdHeight);
				pDC->LineTo(nPosOfX,nTopOfY+nIntervalOfCoorMid+nGirdHeight);
				nGirdHeight += nMinGirdHeight;
			}
			pDC->SelectObject(&pen2);
		}
	}
	if(m_nLastDisCurve == -1)
	{
		m_nLastDisCurve = m_nCurveCount-1;
	}
	pDC->SelectObject(pOldPen);
}
void CAnalyseView::DrawPrintCurve(CDC *pDC,int nFirstDrawCurve)
{
	CPoint pointScrollPos;
	Point pLogic;
	POINT origital;
	pLogic.x = m_curvePrintInfo.dStartTime;
	pLogic.y = 0;
	origital.x = m_rcPrintCurve.left + CHANNEL_INFO_WIDTH_PRINT;
	origital.y = 0;
	m_curRect = m_rcPrintCurve;
	MapPoint(pLogic,pointScrollPos,0,origital,0);
	m_curRect.left += pointScrollPos.x - origital.x;
	m_curRect.right += pointScrollPos.x - origital.x;
	pLogic.x = m_curvePrintInfo.dTestTime;
	MapPoint(pLogic,m_mousePoint,0,origital,0);
	ASSERT_VALID(pDC);
	UNUSED(pDC);
	CPen *pOldPen;
	CBrush *pOldBrush;
	long oldmode;
	CRect rcDC(m_clientRc.left,m_clientRc.top,m_clientRc.left+m_curRect.Width(),m_clientRc.top+m_curRect.Height());
	//你也可以用自己应该用的颜色
	pDC->FillSolidRect(rcDC,RGB(255,255,255));
	//绘图
	PrintScale(pDC);
	oldmode=pDC->SetBkMode(TRANSPARENT);
	if(DoubleCmp(m_dNewMulti-m_dMulti)!=0)
	{
		m_dMulti = m_dNewMulti;
		SetPrintCoorHeight(m_nHeightOfDigitalCoor,m_nHeightOfAnalogCoor,m_curRect);
		::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	}
	PrintGirdLine(pDC,nFirstDrawCurve);
	PrintLine(pDC);
	pDC->SetBkMode(oldmode);
		
	CFont font,*pOldFont;
	font.CreateFont(-350,
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	pOldFont=pDC->SelectObject(&font);
	CString str=_T("名  称:");
	CString temp;
	int i;
	CSize size;
	size=pDC->GetTextExtent(str);
	//如果鼠标线在坐标区域内，则需要画制跟随鼠标的竖线x=m_mousePoint.x,并且要计算出从
	//m_nFirstDisCurve到m_nLastDisCurve通道的瞬时值和模拟量通道的相位角,还有基线与鼠标
	//竖线的时间差和当前鼠标线的精确时间值(包含年月日时分秒)
	
	int nTempHeightAnalogCoor= m_nHeightOfAnalogCoor/3;
	CRect rc;
	if(m_mousePoint.x>m_rectCurve.left)
	{
 		CPen crossPen(PS_DOT,1,m_crossLineColor);
 		pOldPen=pDC->SelectObject(&crossPen);
		pDC->MoveTo(m_mousePoint.x,m_rectCurve.top);
		pDC->LineTo(m_mousePoint.x,m_rectCurve.bottom);
		pDC->SelectObject(pOldPen);
		origital.x = m_rectCurve.left;
		origital.y = m_nHeightOfAnalogCoor/2;
		pLogic.x = m_curvePrintInfo.dTestTime;
		
		long index;
		int nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //坐标系的顶纵坐标
		origital.x = m_rectCurve.left;
		
		for(i=m_nFirstDisCurve;i<=m_nLastDisCurve;i++)
		{
			CPen pen(PS_SOLID,1,m_pCurve[i].color),strPen(PS_SOLID,1,RGB(0,0,0));
			CBrush brush;
			
			if(m_pCurve[i].bIsDigital)
			{
				origital.y = nTopToCoor + m_nHeightOfDigitalCoor/2;
				nWidthOfCoor = m_nHeightOfDigitalCoor;
			}
			else
			{
				if(i<3)
				{
					origital.y = nTopToCoor + nTempHeightAnalogCoor/2;
					nWidthOfCoor = nTempHeightAnalogCoor;
				}
				else
				{
					origital.y = nTopToCoor + m_nHeightOfAnalogCoor/2;
					nWidthOfCoor = m_nHeightOfAnalogCoor;
				}
				
			}
			rc =CRect(m_clientRc.left+100,origital.y+100,m_clientRc.left+300,origital.y-100);
			if(m_curvePrintInfo.bMultiColor)
			{
				brush.CreateSolidBrush(m_pCurve[i].color);
				pOldPen=pDC->SelectObject(&pen);
				pOldBrush=pDC->SelectObject(&brush);
				pDC->Ellipse(&rc);
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);
				rc.left=m_clientRc.left+400;
				rc.top=origital.y+size.cy/2;
				rc.right=m_clientRc.left+4400;
				rc.bottom=origital.y-size.cy/2;
			}
			else
			{
				rc.left=m_clientRc.left+100;
				rc.top=origital.y+size.cy/2;
				rc.right=m_clientRc.left+4400;
				rc.bottom=origital.y-size.cy/2;
			}
			
			oldmode=pDC->SetBkMode(TRANSPARENT);
			pOldPen=pDC->SelectObject(&strPen);
			
			index=BinarySearch(m_pCurve[i].lpPoint,pLogic,m_pCurve[i].count);
			
			if(m_pCurve[i].bIsDigital)
			{
				if(DoubleCmp(m_pCurve[i].lpPoint[index].y))
				{
					temp.Format(_T("%s:1"),m_pCurve[i].name);
				}
				else
				{
					temp.Format(_T("%s:0"),m_pCurve[i].name);
				}
			}
			else
			{
				double dFreq,dSwing,dAngle;
				if(!GetFreqSwingAngleStart(dFreq,dSwing,dAngle,i,index))
				{
					dAngle = 0;
					dFreq = 0;
					dSwing = 0;
				}
				if(m_pCurve[i].unit == _T("°"))
				{
					temp.Format(_T("%s:%.1f%s"),m_pCurve[i].name,m_pCurve[i].lpPoint[index].y,m_pCurve[i].unit);
				}
				else
				{
					TCHAR cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
					if(cUint == TEXT('a') || cUint == TEXT('A') || cUint == TEXT('v') || cUint == TEXT('V'))
					{
						rc.top += size.cy/2+100;
						rc.bottom -= size.cy/2+100;
						temp.Format(_T("%s:%.3f%s\r\n∠%.1f°标么值:%.3f"),m_pCurve[i].name,
							m_pCurve[i].lpPoint[index].y,m_pCurve[i].unit,dAngle,m_pCurve[i].lpPoint[index].extendValue);
					}
					else
					{
						temp.Format(_T("%s:%.3f%s"),m_pCurve[i].name,m_pCurve[i].lpPoint[index].y,m_pCurve[i].unit);
					}
				}
			}
			if(!m_curvePrintInfo.bMultiColor)
			{
				str.Format(_T("%d.%s"),i,temp);
				temp = str;
			}
			pDC->DrawText(temp,&rc,DT_LEFT);
			pDC->SetBkMode(oldmode);
			pDC->SelectObject(pOldPen);
			nTopToCoor += nWidthOfCoor;
		}
	}
	else
	{
		POINT origital;
		int nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //坐标系的顶纵坐标
		origital.x = m_rectCurve.left;
		for(i=nFirstDrawCurve;i<=m_nLastDisCurve;i++)
		{
			CPen pen(PS_SOLID,1,m_pCurve[i].color),strPen(PS_SOLID,1,RGB(0,0,0));
			CBrush brush;
			if(m_pCurve[i].bIsDigital)
			{
				origital.y = nTopToCoor + m_nHeightOfDigitalCoor/2;
				nWidthOfCoor = m_nHeightOfDigitalCoor;
			}
			else
			{
				if(i<3)
				{
					origital.y = nTopToCoor + nTempHeightAnalogCoor/2;
					nWidthOfCoor = nTempHeightAnalogCoor;
				}
				else
				{
					origital.y = nTopToCoor + m_nHeightOfAnalogCoor/2;
					nWidthOfCoor = m_nHeightOfAnalogCoor;
				}
				
			}
			rc =CRect(m_clientRc.left+100,origital.y+100,m_clientRc.left+300,origital.y-100);
			
			if(m_curvePrintInfo.bMultiColor)
			{
				brush.CreateSolidBrush(m_pCurve[i].color);
				pOldPen=pDC->SelectObject(&pen);
				pOldBrush=pDC->SelectObject(&brush);
				pDC->Ellipse(&rc);
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);
				
				rc.left=m_clientRc.left+400;
				rc.top=origital.y+size.cy/2;
				rc.right=m_clientRc.left+4400;
				rc.bottom=origital.y-size.cy/2;
			}
			else
			{
				rc.left=m_clientRc.left+100;
				rc.top=origital.y+size.cy/2;
				rc.right=m_clientRc.left+4400;
				rc.bottom=origital.y-size.cy/2;
			}
			temp.Format(_T("%s:"),m_pCurve[i].name);
			if(!m_curvePrintInfo.bMultiColor)
			{
				str.Format(_T("%d.%s"),i,temp);
				temp = str;
			}
			pDC->DrawText(temp,&rc,DT_LEFT);
			pDC->SetBkMode(oldmode);
			nTopToCoor += nWidthOfCoor;
		}
	}
	pDC->SelectObject(pOldFont);
	
}

