// CurveView.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "CurveView.h"
#include "MainFrm.h"
#include <queue>
#include "CurvePrintSet.h"
#include <math.h>
#include "dMessageBox.h"
#include "WareEventData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  PI 3.1415926 //π
extern CMySID_2FYApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CCurveView
//判断浮点数是是否为0
long DoubleCmp(double d)
{
	//1e-8=10的负8次方
	if(fabs(d)<Precision) return 0;
	return (d>0)?1:-1;
}
Point::Point(double _x,double _y)
{
	x=_x;
	y=_y;
}
//重载<用于二分搜索
bool Point::operator <(Point p)
{
	if(DoubleCmp(x-p.x)<0) return true;
	return false;
}
//重载 == 用于二分搜索
bool Point::operator ==(Point p)
{
	if(DoubleCmp(x-p.x)==0) return true;
	return false;
}
//重载-用于二分搜索
Point Point::operator -(const Point p)
{
	Point re;
	re.x=this->x-p.x;
	return re;
}
Point & Point::operator =(const Point & pt)
{
	if(this == &pt)
	{
		return *this;
	}
	extendValue = pt.extendValue;
	x=pt.x;
	y=pt.y;
	return *this;
}



BOOL CCurveView::m_isFirstShow = FALSE;

IMPLEMENT_DYNCREATE(CCurveView, CScrollView)



/***********************************************
完成变量的初始化
***********************************************/
CCurveView::CCurveView():m_nHeightOfRCScale(20),m_nHeightOfScale(5),m_nMinWidthOfDisScale(5),
m_nMinWidthOfLogScale(1),m_nMinGirdWidth(50),m_nMinGirdHeight(10),m_pCurve(NULL),m_bPrint(FALSE),
m_bMultiColorDis(TRUE),m_dWindowSizeOfX(1000),m_dInitDisPosOfx(-300)
{
	memset(m_arrPageHeadInfo,0,sizeof(m_arrPageHeadInfo));
	m_dValueOfClick = 0;
	m_nHeightOfDigitalCoor = DIGITAL_HEIGHTH_COONDIATE;
	m_nHeightOfAnalogCoor = ANALOG_HEIGHTH_COONDIATE;
	m_nMaxOfX=0;
	m_dMulti=1;
	m_dNewMulti = 1;
	m_nCellOfX=1;
	m_bkColor=RGB(255,255,255);
	m_crossLineColor=RGB(0,0,255);
	m_coordinateColor=RGB(0,0,255);
	m_colorGirdLine =RGB(210,210,210);
	m_pCurve=NULL;
	m_nCurveCount=0;
	m_strValueType=_T("");
	m_colorArray[0]=RGB(0,0,0);          //黑色，脉振电源
	m_colorArray[1]=RGB(0,255,255);      //主控插件相角差和系统侧角转差
	m_colorArray[2]=RGB(255,0,255);      //主控插件US
	m_colorArray[3]=RGB(128,0,0);        //主控插件Ug
	m_colorArray[4]=RGB(128,128,0);      //合闸出口
	m_colorArray[5]=RGB(0,128,0);        //升压
	m_colorArray[6]=RGB(128,128,255);    //降压
	m_colorArray[7]=RGB(0,128,128);      //加速
	m_colorArray[8]=RGB(128,0,128);      //主控插件US频率
	m_colorArray[9]=RGB(0,255,128);      //主控插件Ug频率
	m_colorArray[10]=RGB(0,128,255);     //
    m_colorArray[11]=RGB(0,0,255);       //
	m_colorStartLine = RGB(255,0,0);     //红色，启动录波线的颜色
	m_clientRc = CRect(0,0,0,0);         
	m_dMinFreq = 30;
	m_dMaxFreq = 70;
	m_isUnitedFile=false;
	m_diffMilli=0;
	intersect=0;
	m_dota = 0;
	m_isLabel=false;
	m_bFirstShow=false;
	m_flagShow = ALL_VALUE_SHOW;
	m_TimeCloseOut = 0;
	memset(m_average,0,sizeof(m_average));

	m_fMZstartDota = 0;
	m_fMZreturnDota = 0;
	//
	lpStaPhaseAngle = NULL;
	lpRetPhaseAngle = NULL;
}

CCurveView::~CCurveView()
{
	CCurveView::Release(m_pCurve,m_nCurveCount);
	OnReleaseAngle();
}
/*********************************************
消息命令的监听设置
*********************************************/
BEGIN_MESSAGE_MAP(CCurveView, CMyView)
	//{{AFX_MSG_MAP(CCurveView)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDM_ACHROMATIC,OnAchromatic)
	ON_COMMAND(IDM_DYEING,OnDyeing)
	ON_COMMAND(IDM_SHOW_VALID,OnShowValid)
	ON_COMMAND(IDM_SHOW_SAMPLE,OnShowSample)
	ON_COMMAND(IDM_SHOW_ALL,OnShowAll)
	ON_COMMAND(IDM_CURVE_DIS_SET,OnCurveDisSet)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	
	ON_COMMAND(IDM_EXPORT_WARE_TEXT, OnExportWareFile) //2016-5-5 add 导出波形文件Text
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveView drawing
/***********************************
状态栏的设置
***********************************/
void CCurveView::OnInitialUpdate()
{
	//这里是设置状态栏的
	CScrollView::OnInitialUpdate();
	((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneInfo(1,IDS_SUB_TIME,SBPS_NORMAL, 230);
	((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneInfo(2,IDS_INDICATOR_TIME,SBPS_NORMAL,190);
	GetClientRect(&m_curRect);
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 300;
	SetScrollSizes(MM_TEXT, sizeTotal);
}
void CCurveView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}
#ifdef _DEBUG
void CCurveView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CCurveView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCurveView message handlers
/***********************
当键盘的按键除alt F10按下的时候
************************/
void CCurveView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	switch(nChar)
	{
	case VK_LEFT://按下键盘左方向键，跟随鼠标移动的x=m_mousePoint.x竖线向左一个像素一个像素地平移
		if(m_mousePoint.x >CHANNEL_INFO_WIDTH)
		{
			m_mousePoint.x--;
		}
		else if(m_mousePoint.x<=CHANNEL_INFO_WIDTH)
		{
			m_mousePoint.x=CHANNEL_INFO_WIDTH;
		}
		break;
	case VK_RIGHT://按下键盘右方向键，跟随鼠标移动的x=m_mousePoint.x竖线向右一个像素一个像素地平移
		GetClientRect(&rc);
		if(m_mousePoint.x<CHANNEL_INFO_WIDTH)
		{
			m_mousePoint.x=CHANNEL_INFO_WIDTH;
		}
		else if(m_mousePoint.x <=rc.right)
		{
			m_mousePoint.x++;
		}
		break;
	case VK_RETURN://按下键盘enter键，测量时间的基线移动到m_mousePoint.x处
		{
			//曲线绘制区域的大小为m_rectCurve（230,20,1116,1120）；
			//这是一个结构体
			Point pLogic;
			POINT origital;
			origital.x = m_rectCurve.left;
			origital.y = m_nHeightOfAnalogCoor/2;
			m_mousePoint.x += m_curRect.left;
			if(m_mousePoint.x<CHANNEL_INFO_WIDTH)
			{
				m_mousePoint.x=CHANNEL_INFO_WIDTH;
			}
			//获取鼠标点的横坐标
			MapPointToLogic(m_mousePoint,pLogic,0,origital,m_nHeightOfAnalogCoor/CELL_COUNT);
			m_dValueOfClick = pLogic.x;
			m_mousePoint.x -= m_curRect.left;
			break;
		}
	default:
		break;
	}
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
/***********************************************************************************
函数名：MapPointToLogic
描述：将显示区域的显示坐标转化为逻辑坐标（显示坐标以像素为单位）
参数：p1[in]:  p2[out]: cellOfY[in]:  nWidthOfcell[in]:   
返回值
***********************************************************************************/
Point CCurveView::MapPointToLogic(POINT p1, Point &p2,double cellOfY,POINT origitalPoint,int nWidthOfCell)
{
	int xWidthOfCell;
	if(m_bPrint)
	{
		//3 60 20CELL_COUNT 1倍格刻度线的宽度
		xWidthOfCell = abs(m_nHeightOfAnalogCoor/CELL_COUNT);
	}
	else
	{
		//60
		xWidthOfCell=ANALOG_HEIGHTH_COONDIATE/CELL_COUNT;
	}
	//m_dMulti倍数；m_nCellOfX=X轴上一个刻度的逻辑值
	//p2.x当前的逻辑值，求出刻度的个数然后乘以刻度的逻辑值
	p2.x=(double)(p1.x-origitalPoint.x)/(xWidthOfCell*m_dMulti)*m_nCellOfX;
	//标么值
	p2.y=(double)(origitalPoint.y-p1.y)/nWidthOfCell/cellOfY;
	return p2;
}
/********************************
第一次显示录波文件或者录波文件切换 主要是进行数据的拷贝
********************************/
void CCurveView::ChangeCurve(Curve *pCurve,long count,bool isUnitedFile,int diffMilli,char ps)
{
	//判断是否是1次值或者2次值
	ASSERT(ps=='p'||ps=='s'||ps=='P'||ps=='S');
	if(ps=='p'||ps=='P') m_strValueType=_T("一次值");
	else m_strValueType=_T("二次值");
	//是否为合并波形
	m_isUnitedFile=isUnitedFile;
	//不懂什么用
	m_diffMilli=diffMilli;
	//定义的频率，振幅和角度
	double freq,swing,angle;
	int angleIndex = -1;
	//600个结构体数组里面存储的是瞬时值，标幺值、逻辑值
	Point AngleAnalyse[2][300]; //3S的有效值空间
	//为什么是32
	int   halfPointCnt = 32;
	//清空函数
	CCurveView::Release(m_pCurve,m_nCurveCount);
	OnReleaseAngle();
	//获取框架的索引
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	
	int i,j,temp_n;
	
	CString str;
	//定义两个容器
	queue<int> qVolIndex,qCurrentIndex;
	//定义一堆的double型数据
	double dMaxOfX=0,dMaxVolY=0,dMaxCurrentY=0,dMaxVal,dVolMinRange,dCurrentMinRange;
	//字符型数据
	wchar_t cUint;
	double dValueOfCellY;
	BOOL isRecord = FALSE,isShowCurveMZ=false;
	i = 0;	
	//如果结构体的第一个成员的单位不为空
	//是否显示脉振电压/角差
	if (pCurve[0].unit != _T(""))
	{
		//第一个成员的单位的最后一位是不是"V"
		cUint = pCurve[0].unit[pCurve[0].unit.GetLength()-1];
		if(cUint == TEXT('v') || cUint == TEXT('V'))
		{
			//结构体的第一个成员和第二个成员的单位数据是否一致
			if (pCurve[0].unit == pCurve[1].unit)
			{
				i = 1;
				count += 1;
				//
				isShowCurveMZ = TRUE;
			}
		}
		//第2个成员的单位时候等于第三个成员的单位
		else if (pCurve[1].unit == pCurve[2].unit)
		{
			//如果第二个成员的单位不为空
			if (pCurve[1].unit != _T(""))
			{
				//获取单位的数据中的最后一位
				cUint = pCurve[1].unit[pCurve[1].unit.GetLength()-1];
				//如果最后一位为"V"
				if(cUint == TEXT('v') || cUint == TEXT('V'))
				{
					i = 1;
					count += 1;
					isShowCurveMZ = TRUE; //显示脉振电压
				}	

			}
		}
	}
	//通过计算记录了43条曲线
	m_nCurveCount = count;
	//相当于开辟一个结构体指针数据，里面存储的都是结构体
	m_pCurve = new Curve[m_nCurveCount];
	//用于补偿最后点数不够的问题，即50个点只能连49条线，此处需要51个点才能连出50线，后面同理
	for(int curveIndex = 0;i < m_nCurveCount;i++ , curveIndex++)
	{
		//你会观察到是比cuiveIndex大1
		//开始采样点数组51
		m_pCurve[i].lpStaPoint=new Point[pCurve[curveIndex].StaCount+1];
		//返回采样点数组5951
		m_pCurve[i].lpRetPoint=new Point[pCurve[curveIndex].RetCount+1];	
		//起始断点数组0,1600,800
		m_pCurve[i].lpStartSecPoint=new Point[pCurve[curveIndex].startCount];
		//返回段点数组0,4800,240
		m_pCurve[i].lpReturnSecPoint=new Point[pCurve[curveIndex].returnCount];
		//如果开辟空间失败，就清除该空间
		if(m_pCurve[i].lpStaPoint == NULL)
		{
			CCurveView::Release(m_pCurve,m_nCurveCount);
			m_pCurve = NULL;
			return;
		}
		//全程启动段点数50
		m_pCurve[i].StaCount=pCurve[curveIndex].StaCount;
		//全程返回段点数5950
		m_pCurve[i].RetCount=pCurve[curveIndex].RetCount;
		//子项序号5,0,1
		m_pCurve[i].nSubItemIndex = pCurve[curveIndex].nSubItemIndex;		
		//通道序号5,0,1
		m_pCurve[i].nChannelIndex = pCurve[curveIndex].nChannelIndex;
		//第一个采样点时间
		m_pCurve[i].firstSampleTime=pCurve[curveIndex].firstSampleTime;
		//第一个采样点的ms数
		m_pCurve[i].milliSecondOfFirst=pCurve[curveIndex].milliSecondOfFirst;
		//启动录波时间
		m_pCurve[i].startTime=pCurve[curveIndex].startTime;
		//采样启动段点数
		m_pCurve[i].startCount = pCurve[curveIndex].startCount;
		//曲线返回段点数
		m_pCurve[i].returnCount = pCurve[curveIndex].returnCount;
		//曲线颜色
		m_pCurve[i].color=m_colorArray[i%COLOR_NUM];
		//最小的逻辑坐标值
		m_pCurve[i].minRange=pCurve[curveIndex].minRange;
		//是否有开入开出量
		m_pCurve[i].bIsDigital = pCurve[curveIndex].bIsDigital;
		//是否有起始段，返回段
		m_pCurve[i].bIsOverlap = pCurve[curveIndex].bIsOverlap;
		//计算频率和相位角0,64,64,32
		m_pCurve[i].nPointInCircle = pCurve[curveIndex].nPointInCircle;	
		//通道名称
		m_pCurve[i].name = pCurve[curveIndex].name;
		//单位
		m_pCurve[i].unit =  pCurve[curveIndex].unit;
		//不设与设为0都是默认系统设置
		m_pCurve[i].valueOfCellY = pCurve[curveIndex].valueOfCellY;
		
		//上面感觉就是开辟空间和赋值
		
		dMaxVal = 0;
		int sampleRetBegin=0;
		//下面是计算返回段开始显示的时间
		if(m_diffMilli>0)
		{
			//相交
			intersect=1000;
			m_dota=m_diffMilli;
			str.Format(_T("△t =%dms"),m_dota);
			m_strDota=str;
			m_isLabel=true;
			//58500   pCurve[1].nPointInCircle=64
			sampleRetBegin = 60000 - pCurve[1].returnCount*1.0/pCurve[1].nPointInCircle*20;
		}
		else if (pCurve[2].nPointInCircle != 0 )
		{//返回段开始显示的时间
			sampleRetBegin = pMainFrm->m_wareFileData.m_nCycleCount*20- pCurve[2].returnCount*1.0/pCurve[2].nPointInCircle*20;
		}
		else if ( pCurve[1].nPointInCircle != 0 )
		{//2015-1-30 add 如果不显示角差，待并侧电压ug就在pCruve[1]
            sampleRetBegin = pMainFrm->m_wareFileData.m_nCycleCount*20- pCurve[1].returnCount*1.0/pCurve[1].nPointInCircle*20;
		}
		else
		{
			sampleRetBegin = 0;
		}
		if (!m_isFirstShow)
		{
			sampleRetBegin = 0;
			intersect = 0;
		}

		//这里将所有连续的电压量（或电流）使用同样的Y坐标刻度变比，是为了让用户从界面上就能看出各条曲线的
		//电压（或电流）量的大小，这里只有开入开出量才会设置Y轴上单位刻度所代表的逻辑值（valueOfCellY），
		//模拟量的valueOfCellY为0，也就是未设置，一个坐标系中，Y轴的正负轴各有十个单位刻度，每个单位刻度
		//可以表示一个浮点数的逻辑值，所以很小的数据，也可以在界面得到放大显示，minRange，用于设置坐标系
		//的显示逻辑值范围的最小值
		//是否有开入开出量
		//是开入开出量
		if(m_pCurve[i].bIsDigital)
		{
			//曲线全程启动段点数
			for(j=0;j<pCurve[curveIndex].StaCount;j++)
			{
				//把原本的结构体0的全程启动段采样点数组，赋值到新结构体1里面
				m_pCurve[i].lpStaPoint[j]=pCurve[curveIndex].lpStaPoint[j];
				//不被进入
				if(m_pCurve[i].lpStaPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
				}
			}
			//lpStaPoint里面的最后一个值和前面的值一样，且最后会在它的X值上+10
			m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
			m_pCurve[i].lpStaPoint[j].x+=10;
			//曲线全程返回段点数
			for(j=0;j<pCurve[i].RetCount;j++)
			{
				//新的结构体的retpoint等于原来的数据，且在它的X的基础上加上相交的坐标点的个数
				m_pCurve[i].lpRetPoint[j]=pCurve[curveIndex].lpRetPoint[j];
				m_pCurve[i].lpRetPoint[j].x+=intersect;
				//不被进入
				if(m_pCurve[i].lpRetPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
				}
			}
			//RetCount里面的最后一个值和前面的值一样，且最后会在它的X值上+10
			m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
			m_pCurve[i].lpRetPoint[j].x+=10;
			//曲线采样启动段点数
			for (j=0;j<pCurve[curveIndex].startCount;j++)
			{
				m_pCurve[i].lpStartSecPoint[j]=pCurve[curveIndex].lpStartSecPoint[j];
				//不被进入
				if(m_pCurve[i].lpStartSecPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpStartSecPoint[j].x;
				}
			}			
			//曲线采样返回段点数
			for (j=0;j<pCurve[curveIndex].returnCount;j++)
			{
				//lpReturnSecPoint在原来的基础上加上相交的点数和时间
				m_pCurve[i].lpReturnSecPoint[j]=pCurve[curveIndex].lpReturnSecPoint[j];
				//59500=58500+1000（intersect）
				m_pCurve[i].lpReturnSecPoint[j].x+=(intersect+sampleRetBegin);	
				//不被进入
				if(m_pCurve[i].lpReturnSecPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpReturnSecPoint[j].x;
				}	
				//如果有合闸出口这个通道的话
				if (pCurve[curveIndex].lpReturnSecPoint[j].y && (m_pCurve[i].name.Find(_T("合闸出口")) >= 0 || m_pCurve[i].name.Find(_T("Close Outlet")) >= 0 ) && !isRecord)
				{
					isRecord = TRUE;
					//0
					m_TimeCloseOut = pCurve[curveIndex].lpReturnSecPoint[j].x+sampleRetBegin;
				}
			}
		}
		//如果不是开入开出量
		else
		{
			//获取单位的数据中的最后一位
			cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
			//如果单位为安培的话
			if(cUint == TEXT('a') || cUint == TEXT('A'))
			{
				//逻辑值的值的范围最小不能小于minRange
				dCurrentMinRange = m_pCurve[i].minRange;
				qCurrentIndex.push(i);
				//50曲线全程启动段点数
				for(j=0;j<pCurve[curveIndex].StaCount;j++)
				{
					m_pCurve[i].lpStaPoint[j]=pCurve[curveIndex].lpStaPoint[j];
					//比较大小
					if(DoubleCmp(m_pCurve[i].lpStaPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStaPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpStaPoint[j].y);
					}
				}
				m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
				m_pCurve[i].lpStaPoint[j].x+=10;
				//5950
				//曲线全程返回段点数
				for(j=0;j<pCurve[curveIndex].RetCount;j++)
				{
					//它的x需要加上相交数
					m_pCurve[i].lpRetPoint[j]=pCurve[curveIndex].lpRetPoint[j];
					m_pCurve[i].lpRetPoint[j].x+=intersect;
					if(DoubleCmp(m_pCurve[i].lpRetPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpRetPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpRetPoint[j].y);
					}
				}
				m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
				m_pCurve[i].lpRetPoint[j].x+=10;
				//曲线采样启动段点数
				for (j = 0; j< m_pCurve[i].startCount ; j++)
				{
					m_pCurve[i].lpStartSecPoint[j]=pCurve[curveIndex].lpStartSecPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStartSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStartSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStartSecPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpStartSecPoint[j].y);
					}
				}
				//曲线采样返回段点数
				for (j = 0; j< m_pCurve[i].returnCount ; j++)
				{
					m_pCurve[i].lpReturnSecPoint[j]=pCurve[curveIndex].lpReturnSecPoint[j];
					m_pCurve[i].lpReturnSecPoint[j].x+=(intersect+sampleRetBegin);
					if(DoubleCmp(m_pCurve[i].lpReturnSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpReturnSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpReturnSecPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpReturnSecPoint[j].y);
					}
				}
			}
			//只有通道2,通道3会传入进去 当这条通道信息的单位为伏特的时候
			else if(cUint == TEXT('v') || cUint == TEXT('V'))
			{
				//获取通道的索引
				angleIndex = m_pCurve[i].nChannelIndex;//刚好采样模块量有两个通道，第一us，第二个ug，
				//最小逻辑值
				dVolMinRange = m_pCurve[i].minRange;
				//是第几条通道
				qVolIndex.push(i);
				//简单的赋值运算    传入的时候好像并没有角差这类东西
				for(j=0;j<pCurve[curveIndex].StaCount;j++)
				{
					m_pCurve[i].lpStaPoint[j]=pCurve[curveIndex].lpStaPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStaPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStaPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpStaPoint[j].y);
					}
				}
				m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
				m_pCurve[i].lpStaPoint[j].x+=10;
				for(j=0;j<pCurve[i].RetCount;j++)
				{
					m_pCurve[i].lpRetPoint[j]=pCurve[curveIndex].lpRetPoint[j];
					m_pCurve[i].lpRetPoint[j].x+=intersect;
					if(DoubleCmp(m_pCurve[i].lpRetPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpRetPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpRetPoint[j].y);
					}
				}
				m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
				m_pCurve[i].lpRetPoint[j].x+=10;
				int index=0;
				halfPointCnt =m_pCurve[i].nPointInCircle/2;	
				//1600曲线采样启动段点数 表示启动采样段点数组的长度为1600   保存电压的采样值
				for (j = 0; j< m_pCurve[i].startCount ; j++)
				{
					m_pCurve[i].lpStartSecPoint[j]=pCurve[curveIndex].lpStartSecPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStartSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStartSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStartSecPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpStartSecPoint[j].y);
					}
					//感觉它在这里保存到数组中的数据并没有什么用
					if (j%halfPointCnt == 0)
					{//半个周波取一次角差
						AngleAnalyse[angleIndex][index].x = m_pCurve[i].lpStartSecPoint[j].x;
						AngleAnalyse[angleIndex][index].y = 0;
						//3个未赋值的变量，他是第几个数据，32的倍数,起始段点数组，段点数
						if (GetFreqSwingAngle(freq,swing,angle,i,j,m_pCurve[i].lpStartSecPoint,m_pCurve[i].startCount))
						{
							AngleAnalyse[angleIndex][index].y = angle;
						}
						index++;
					}
				}
				//曲线采样返回段点数4800     电压采样值
				for (j = 0; j< m_pCurve[i].returnCount ; j++)
				{
					m_pCurve[i].lpReturnSecPoint[j]=pCurve[curveIndex].lpReturnSecPoint[j];
					m_pCurve[i].lpReturnSecPoint[j].x+=(intersect+sampleRetBegin);
					if(DoubleCmp(m_pCurve[i].lpReturnSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpReturnSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpReturnSecPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpReturnSecPoint[j].y);
					}
					if (j%halfPointCnt == 0)
					{
						AngleAnalyse[angleIndex][index].x = m_pCurve[i].lpReturnSecPoint[j].x;
						AngleAnalyse[angleIndex][index].y = 0;
						if (GetFreqSwingAngle(freq,swing,angle,i,j, m_pCurve[i].lpReturnSecPoint, m_pCurve[i].returnCount))
						{
							AngleAnalyse[angleIndex][index].y = angle;
						}
						index++;
					}
				}
			}
			else
			{//角度，频率等处理
				//曲线的全程启动段点数50   只需要拷贝全程启动段点数，和全程返回段点数				
				
				for(j=0;j<pCurve[curveIndex].StaCount;j++)
				{
					m_pCurve[i].lpStaPoint[j]=pCurve[curveIndex].lpStaPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStaPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStaPoint[j].y)-dMaxVal)>0)
					{
						//这个是候dMaxVal被更新为30
						dMaxVal = fabs(m_pCurve[i].lpStaPoint[j].y);
					}
				}
				m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
				m_pCurve[i].lpStaPoint[j].x+=10;
				for(j=0;j<pCurve[curveIndex].RetCount;j++)
				{
					m_pCurve[i].lpRetPoint[j]=pCurve[curveIndex].lpRetPoint[j];
					m_pCurve[i].lpRetPoint[j].x+=intersect;
					if(DoubleCmp(m_pCurve[i].lpRetPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpRetPoint[j].y)-dMaxVal)>0)
					{
						dMaxVal = fabs(m_pCurve[i].lpRetPoint[j].y);
					}
				}
				m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
				m_pCurve[i].lpRetPoint[j].x+=10;
				//如果通道名称为主控插件相角差   事实证明主控插件相角差的通道序号是第一个
				if ( m_pCurve[i].name.Find(_T("主控插件相角差")) >= 0 || m_pCurve[i].name.Find(_T("Mainboard Angle Diff.")) >= 0 )
				{
					//全程启动段点数 里面的y就是角差  49
					for(j=0;j<pCurve[curveIndex].StaCount-1;j++)
					{
						//前后两点的角度相差180°，则波形
						if (fabs(m_pCurve[i].lpStaPoint[j].y-m_pCurve[i].lpStaPoint[j+1].y)>180)
						{
							m_pCurve[i].lpStaPoint[j].x=m_pCurve[i].lpStaPoint[j+1].x;						
						}
					}
					//全程返回段点数  1620
					for(j=0;j<pCurve[curveIndex].RetCount-1;j++)
					{
						if (fabs(m_pCurve[i].lpRetPoint[j].y-m_pCurve[i].lpRetPoint[j+1].y)>180)
						{
							m_pCurve[i].lpRetPoint[j].x=m_pCurve[i].lpRetPoint[j+1].x;
						}
					}
					//以下代码没有被调用			就问你气不气
					for (j = 0; j< m_pCurve[i].startCount ; j++)
					{//有存在采样前段				
						m_pCurve[i].lpStartSecPoint[j].x = AngleAnalyse[1][j].x;
						m_pCurve[i].lpStartSecPoint[j].y = AngleAnalyse[0][j].y-AngleAnalyse[1][j].y; //us - ug，开始前段
						if(DoubleCmp(m_pCurve[i].lpStartSecPoint[j].y+180)<0)
						{
							m_pCurve[i].lpStartSecPoint[j].y += 360;
						}
						else if(DoubleCmp(m_pCurve[i].lpStartSecPoint[j].y-180)>0)
						{
							m_pCurve[i].lpStartSecPoint[j].y -= 360;
						}
						if(DoubleCmp(fabs(m_pCurve[i].lpStartSecPoint[j].y)-dMaxVal)>0)
						{
							dMaxVal = fabs(m_pCurve[i].lpStartSecPoint[j].y);
						}
					}
					int retPos;
					//不被调用
					for (j = 0; j< m_pCurve[i].returnCount ; j++)
					{
						retPos = j+m_pCurve[i].startCount;
						m_pCurve[i].lpReturnSecPoint[j].x = AngleAnalyse[0][retPos].x;
						m_pCurve[i].lpReturnSecPoint[j].y = AngleAnalyse[0][retPos].y-AngleAnalyse[1][retPos].y;
						if(DoubleCmp(m_pCurve[i].lpReturnSecPoint[j].y+180)<0)
						{
							m_pCurve[i].lpReturnSecPoint[j].y += 360;
						}
						else if(DoubleCmp(m_pCurve[i].lpReturnSecPoint[j].y-180)>0)
						{
							m_pCurve[i].lpReturnSecPoint[j].y -= 360;
						}
						if(DoubleCmp(fabs(m_pCurve[i].lpReturnSecPoint[j].y)-dMaxVal)>0)
						{
							dMaxVal = fabs(m_pCurve[i].lpReturnSecPoint[j].y);
						}
					}
					//不被调用
					for(j=0;j<m_pCurve[i].startCount-1;j++)
					{
						if (fabs(m_pCurve[i].lpStartSecPoint[j].y-m_pCurve[i].lpStartSecPoint[j+1].y)>180)
						{
							m_pCurve[i].lpStartSecPoint[j].x=m_pCurve[i].lpStartSecPoint[j+1].x;
						}
					}
					//不被调用
					for(j=0;j<m_pCurve[i].returnCount-1;j++)
					{
						if (fabs(m_pCurve[i].lpReturnSecPoint[j].y-m_pCurve[i].lpReturnSecPoint[j+1].y)>180)
						{
							m_pCurve[i].lpReturnSecPoint[j].x=m_pCurve[i].lpReturnSecPoint[j+1].x;
						}
					}
				}
				//dMaxVal=30
				if(dMaxVal<m_pCurve[i].minRange) 
				{
					dMaxVal=m_pCurve[i].minRange;
				}
				//30    30
				temp_n=(long)dMaxVal;
				//调用判断函数
				if(DoubleCmp(dMaxVal-temp_n)>0)
				{
					//31
					temp_n++;
				}
				//4   y轴上一个刻度的逻辑值
				dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
				if(DoubleCmp(dMaxVal-1) ==0)
				{
					dValueOfCellY = dMaxVal/10;
				}
				//这里是的等于4，表示为让系统自行设置	y轴上一个刻度的单位
				m_pCurve[i].valueOfCellY = dValueOfCellY;
			}
		}
	}
	//初始化指针缓存空间
	int	nStaAngleCount = 0,nRetAngleCount = 0;			//保存角度值的缓存的大小
	int	nIndexChannel = 0;
	int	nCacluIndex = 1;
	for(nIndexChannel = 0;nIndexChannel < m_nCurveCount;nIndexChannel++)
	{
		if (m_pCurve[nIndexChannel].name.Find(_T("主控插件相角差")) >= 0 || m_pCurve[nIndexChannel].name.Find(_T("Mainboard Angle Diff")) >= 0 )
		{
			nStaAngleCount = m_pCurve[nIndexChannel].StaCount;
			nRetAngleCount = m_pCurve[nIndexChannel].RetCount;
			break;
		}
	}
	if (nStaAngleCount != 0  )
	{
		lpStaPhaseAngle = new double[nStaAngleCount*2];
		//特殊处理一
 		lpStaPhaseAngle[0] = m_pCurve[nIndexChannel].lpStaPoint[0].y;
		//对数据进行处理
		for (nCacluIndex = 1; nCacluIndex < nStaAngleCount ; nCacluIndex++)
		{
			double nIntervalSample = m_pCurve[nIndexChannel].lpStaPoint[nCacluIndex - 1].y + (double)(m_pCurve[nIndexChannel].lpStaPoint[nCacluIndex].y - m_pCurve[nIndexChannel].lpStaPoint[nCacluIndex - 1].y)/2;
			lpStaPhaseAngle[nCacluIndex*2 - 1] = nIntervalSample;
			//
			lpStaPhaseAngle[nCacluIndex*2] = m_pCurve[nIndexChannel].lpStaPoint[nCacluIndex].y;
		}
		lpStaPhaseAngle[nStaAngleCount*2 - 1] = m_pCurve[nIndexChannel].lpStaPoint[nStaAngleCount - 1].y;
	}
	if (nRetAngleCount != 0  )
	{
		lpRetPhaseAngle = new double[nRetAngleCount*2];
		//特殊处理一
		lpRetPhaseAngle[0] = m_pCurve[nIndexChannel].lpRetPoint[0].y;
		//对数据进行处理
		for (nCacluIndex = 1; nCacluIndex < nRetAngleCount ; nCacluIndex++)
		{
			double nIntervalSample = m_pCurve[nIndexChannel].lpRetPoint[nCacluIndex - 1].y + (double)(m_pCurve[nIndexChannel].lpRetPoint[nCacluIndex].y - m_pCurve[nIndexChannel].lpRetPoint[nCacluIndex - 1].y)/2;
			lpRetPhaseAngle[nCacluIndex*2 - 1] = nIntervalSample;
			//
			lpRetPhaseAngle[nCacluIndex*2] = m_pCurve[nIndexChannel].lpRetPoint[nCacluIndex].y;
		}
		lpRetPhaseAngle[nRetAngleCount*2 - 1] = m_pCurve[nIndexChannel].lpRetPoint[nRetAngleCount - 1].y;
	}
//#define		OUTPUT_FILE			0x01
#ifdef OUTPUT_FILE
 	CString	strTemp,strAngle;
// 	for (int nDot = 0 ;nDot < nStaAngleCount*2;nDot++)
// 	{
// 		strTemp.Format(_T("第%d个点：%0.2f\r\n"),nDot,lpStaPhaseAngle[nDot]);
// 		strAngle += strTemp;
// 	}
	for (int nDot = 0 ;nDot < nRetAngleCount*2 ;nDot++)
	{
		strTemp.Format(_T("第%d个点：%0.2f\r\n"),nDot,lpRetPhaseAngle[nDot]);
		strAngle += strTemp;
	}
	char	Buff[100000] = {0};
	//把获取到的数据全部以文本形式提取
	FILE	*mFile = fopen("E:\\us_ug.txt","a+");
	if (mFile == NULL)
	{
		AfxMessageBox(_T("文件打开失败"));
	}
	WideCharToMultiByte(CP_ACP,0,strAngle,-1,Buff,100000,NULL,NULL);
	fwrite(Buff,strlen(Buff),1,mFile);
	fclose(mFile);
#endif

	//自己显示的三个波形都需要显示脉振电压
	if (isShowCurveMZ)
	{
		if (!InitAddChannel())
		{
			return;
		}
	}
	//计算电流中的dValueOfCellY
	//不被执行
	if(dMaxCurrentY<dCurrentMinRange)
	{
		dMaxCurrentY=dCurrentMinRange;
	}
	//100  100
	temp_n=(long)dMaxCurrentY;
	//如果tempY大于temp_n
	if(DoubleCmp(dMaxCurrentY-temp_n)>0)
	{
		//101
		temp_n++;
	}
	//11
	dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
	//不被执行
	if(DoubleCmp(dMaxCurrentY-1) ==0)
	{
		dValueOfCellY = dMaxCurrentY/10;
	}
	//容器里面有内容，所以不执行下面的代码程序段
	while(!qCurrentIndex.empty())
	{
		m_pCurve[qCurrentIndex.front()].valueOfCellY = dValueOfCellY;
		qCurrentIndex.pop();
	}
	//计算电压中的dValueOfCellY
	//不被执行    dMaxVal=146
	if(dMaxVolY<dVolMinRange) 
	{
		dMaxVolY=dVolMinRange;
	}
	//146
	temp_n=(long)dMaxVolY;
	//如果tempY大于temp_n
	if(DoubleCmp(dMaxVolY-temp_n)>0)
	{
		//147了
		temp_n++;
	}
	//15
	dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
	//不被执行
	if(DoubleCmp(dMaxVolY-1) ==0)
	{
		dValueOfCellY = dMaxVolY/10;
	}
	//在算频率的时候好像被执行两次
	while(!qVolIndex.empty())
	{
		m_pCurve[qVolIndex.front()].valueOfCellY = dValueOfCellY;
		qVolIndex.pop();
	}
	//最大的X轴坐标 16709
	m_nMaxOfX=(long)dMaxOfX;
	if(m_nMaxOfX<dMaxOfX) 
	{
		//16710了
		m_nMaxOfX++;
	}
	//计算区域大小
	//一个可读的逻辑值
	m_dMulti=1;
	//新的逻辑值
	m_dNewMulti = 1;
	//16710
	m_dWindowSizeOfX = m_nMaxOfX;
	//初始显示曲线时最左边显示的X坐标0
	m_dInitDisPosOfx = 0-m_pCurve[2].startTime;
	SetCoorHeight(20,60);
	//获取窗口信息
	CRect rc;
	//获取电脑的屏幕分辨率
	GetDesktopWindow()->GetWindowRect(rc);
	//230 230这个位置是画布长
	rc.right -= WIDTH_PROJECT+CHANNEL_INFO_WIDTH+20;
	//50130
	double d1=m_clientRc.Width()-CHANNEL_INFO_WIDTH;
	//1366*16710/(50130*16710)
	//放大的倍数
	m_dMulti = rc.Width()*m_nMaxOfX/(d1*m_dWindowSizeOfX);
	m_dMinMulti = m_dMulti;
	m_dNewMulti = m_dMulti;
	//16710
	m_dWindowSizeOfX = m_nMaxOfX;
	//0
	m_dInitDisPosOfx = 0-m_pCurve[2].startTime;
	SetCoorHeight(20,60);	// 	POINT origital,tempPoint;
	POINT origital,tempPoint;
	Point rightPoint;
	//230
	origital.x=m_rectCurve.left;
	origital.y=0;
	//0
	rightPoint.x=m_dInitDisPosOfx+m_pCurve[2].startTime;
	origital.y=0;
	MapPoint(rightPoint,tempPoint,0,origital,0);
	//0
	tempPoint.x -= m_rectCurve.left;
	//把视图移动到给定的点
	ScrollToPosition(tempPoint);
}
/**********************************************
1041L 开始调用
**********************************************/
POINT CCurveView::MapPoint(Point p1, POINT &p2,double cellOfY,POINT origitalPoint,int nWidthOfCell)
{
	//为了统一x坐标，这里的xWidthOfCell全按模拟量计算
	int xWidthOfCell;
	//好像不被执行
	if(m_bPrint)
	{
		//x轴上每格刻度的逻辑值为3                m_nCellOfX 记录X轴上一个刻度所表示的X的逻辑值 m_curRect当前显示的区域
		xWidthOfCell = abs(m_nHeightOfAnalogCoor/CELL_COUNT);
		//现在传入的Point结构体的X*3*当前的倍数/逻辑值+传入进来的点坐标的x值-当前窗体X的坐标值+x坐标值
		p2.x=p1.x*xWidthOfCell*m_dMulti/m_nCellOfX+origitalPoint.x-m_curRect.left+m_rcPrintCurve.left;
	}
	else
	{
		//60/20
		xWidthOfCell=ANALOG_HEIGHTH_COONDIATE/CELL_COUNT;
		//现在传入的Point结构体的X*3*当前的倍数/逻辑值+传入进来的点坐标的x值-当前窗体X的坐标值		
		p2.x=p1.x*xWidthOfCell*m_dMulti/m_nCellOfX+origitalPoint.x-m_curRect.left;
	}
	//当前坐标系的宽坐标/传入值
	p2.y=(-1)*p1.y*nWidthOfCell/cellOfY;
	//传入的点的Y值
	p2.y+=origitalPoint.y;
	//获取到一个坐标点的坐标
	return p2;
}
/******************************************************************
1450L调用过一次
******************************************************************/
void CCurveView::DrawLine(CDC *pDC)
{
	POINT temp,origitalPoint;
	//曲线绘制区域的左上角
	origitalPoint.x=m_rectCurve.left;
	UINT nWidthOfCell,nTopOfCoor;  //nTopOfCoor当前坐标系的顶坐标
	long i,j;
	CPen *pPen;
	//初始化画笔
	CPen pen(PS_SOLID,1,m_colorStartLine);
	CPen pen2(PS_DASH,1,m_colorStartLine);
	CPen pen3(PS_SOLID,1,RGB(255,0,255));
	CPen samplePen(PS_SOLID,1,RGB(224,215,27));
	CPen *pOldPen;
	Point point;
	pOldPen=pDC->SelectObject(&pen);
	//绘制曲线的区域
	nTopOfCoor = m_rectCurve.top;
	CString str;
	//显示区域中第一个需要显示的坐标系序号，从0开始
	for(i=0;i<m_nFirstDisCurve;i++)
	{
		if(m_pCurve[i].bIsDigital)
		{
			//20
			nTopOfCoor += m_nHeightOfDigitalCoor;
		}
		else
		{
			//60
			nTopOfCoor += m_nHeightOfAnalogCoor;
		}
	}
	Point pLogic;
	//17
	//当前曲线绘制区域显示了多少条曲线
	for(i=m_nFirstDisCurve;i<=m_nLastDisCurve;i++)
	{
		int diff=0;
		bool isOverlap=false;
		if(m_isLabel)
		{
			diff=intersect/10;
		}
		pPen=new CPen(PS_SOLID,1,m_pCurve[i].color);
		pDC->SelectObject(pPen);
//		if(m_pCurve[i].StaCount<2) continue;
		j=0;
		//是否为开入开出量                   利用origitalPoint.y记录每条线的起始位置
		//根据通道类型设置直线占多少位置
		if(m_pCurve[i].bIsDigital)
		{
			//270 290 310 .....
			origitalPoint.y = nTopOfCoor + m_nHeightOfDigitalCoor/2; 
			//1
			nWidthOfCell = m_nHeightOfDigitalCoor/CELL_COUNT;
		}
		//如果不是开入开出量
		else
		{
			//50 110 170 230
			origitalPoint.y = nTopOfCoor + m_nHeightOfAnalogCoor/2;
			//3
			nWidthOfCell = m_nHeightOfAnalogCoor/CELL_COUNT;
		}

		int indexSta=0,indexRet=0,startP=0,returnP=0,endSta=0,endRet=0,endStart=0,endReturn=0;
		//当前显示区域加上230

		temp.x=m_curRect.left+CHANNEL_INFO_WIDTH;
		//图上的逻辑坐标值
		//244L
		//传入的参数就是每一条执行开始绘制的左坐标
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);
		//二分查找

		indexSta=BinarySearch(m_pCurve[i].lpStaPoint,pLogic,m_pCurve[i].StaCount+1);
		indexRet=BinarySearch(m_pCurve[i].lpRetPoint,pLogic,m_pCurve[i].RetCount+1);
		//是否有起始段和返回段
		if (m_pCurve[i].bIsOverlap)
		{
			//二分查找
			startP=BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
			returnP=BinarySearch(m_pCurve[i].lpReturnSecPoint,pLogic,m_pCurve[i].returnCount);
		}
		//当前显示区域的右下角的x
		temp.x=m_curRect.right;
		//244L
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);
		endSta=BinarySearch(m_pCurve[i].lpStaPoint,pLogic,m_pCurve[i].StaCount+1);
		endRet=BinarySearch(m_pCurve[i].lpRetPoint,pLogic,m_pCurve[i].RetCount+1);
		if (m_pCurve[i].bIsOverlap)
		{
			endStart=BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
			endReturn=BinarySearch(m_pCurve[i].lpReturnSecPoint,pLogic,m_pCurve[i].returnCount);
		}
		if (m_flagShow & VALID_VALUE_SHOW)  //VALID_VALUE_SHOW  0x01
		{
			if(!m_pCurve[i].bIsDigital)
			{
				MapPoint(m_pCurve[i].lpStaPoint[indexSta],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				if(temp.x >= CHANNEL_INFO_WIDTH-2 && temp.x <= m_curRect.Width())
				{
					pDC->MoveTo(temp);
				}
				else if (temp.x < CHANNEL_INFO_WIDTH)
				{
					temp.x = CHANNEL_INFO_WIDTH;
					pDC->MoveTo(temp);
				}
				for (int k = indexSta ; k < endSta; k++)
				{
					MapPoint(m_pCurve[i].lpStaPoint[k+1],temp,m_pCurve[i].valueOfCellY,
						origitalPoint,nWidthOfCell);
					if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
					{
						pDC->LineTo(temp);
						pDC->MoveTo(temp);
					}
				}
				MapPoint(m_pCurve[i].lpRetPoint[indexRet],temp,m_pCurve[i].valueOfCellY,
					origitalPoint,nWidthOfCell);
				if(temp.x >= CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
				{
					pDC->MoveTo(temp);
				}
				else if (temp.x < CHANNEL_INFO_WIDTH)
				{
					temp.x = CHANNEL_INFO_WIDTH;
					pDC->MoveTo(temp);
				}
				for (k = indexRet ; k < endRet ; k++)
				{
					MapPoint(m_pCurve[i].lpRetPoint[k+1],temp,m_pCurve[i].valueOfCellY,
						origitalPoint,nWidthOfCell);
					if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width()+4)
					{
						if (temp.x > m_curRect.Width())
						{
							temp.x = m_curRect.Width();
						}
						pDC->LineTo(temp);
						pDC->MoveTo(temp);
					}
				}
				
			}
			else
			{
				MapPoint(m_pCurve[i].lpStartSecPoint[startP],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				if (temp.x < CHANNEL_INFO_WIDTH)
				{
					temp.x = CHANNEL_INFO_WIDTH;
				}
				pDC->MoveTo(temp);
				for(j=startP+1;j<=endStart;j++)
				{
					POINT befPoint,midPoint[2];
					befPoint = temp;
					MapPoint(m_pCurve[i].lpStartSecPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
					//对于从0变1或从1变0的开入量的点，会在它们的中点处添加两点midPoint0,midPoint1,设变化前后两点
					//分别为p1,p2,则midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
					//p1.x)/2,并且系统会顺次连接p1-midPoint0-midPoint1-p2
					if (temp.x < CHANNEL_INFO_WIDTH)
					{
						temp.x = CHANNEL_INFO_WIDTH;
					}
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
				if (startP<endStart)
				{
					indexSta = m_pCurve[i].startCount/m_pCurve[i].nPointInCircle*2;
					if ((m_pCurve[i].lpStartSecPoint[endStart].y!=m_pCurve[i].lpStaPoint[indexSta].y))
					{//开关量有效段返回段中，若第一个值与起始段的最后一个值不一样，画上一条折线；
						Point tempPoint = m_pCurve[i].lpStartSecPoint[endStart];
						if (m_pCurve[i].lpStartSecPoint[endStart].y > 0)
						{
							tempPoint.y=0;
						}
						else
						{
							tempPoint.y=m_pCurve[i].lpStaPoint[indexSta].y;
						}
						POINT temp2;
						MapPoint(tempPoint,temp2,m_pCurve[i].valueOfCellY,
							origitalPoint,nWidthOfCell);
						if (temp2.x < CHANNEL_INFO_WIDTH)
						{
							temp2.x = CHANNEL_INFO_WIDTH;
						}
						pDC->LineTo(temp2);
					}
				}

				MapPoint(m_pCurve[i].lpStaPoint[indexSta],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				if (temp.x < CHANNEL_INFO_WIDTH)
				{
					temp.x = CHANNEL_INFO_WIDTH;
				}
				pDC->MoveTo(temp);
				for(j=indexSta+1;j<=endSta;j++)
				{
					POINT befPoint,midPoint[2];
					befPoint = temp;
					MapPoint(m_pCurve[i].lpStaPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
					//对于从0变1或从1变0的开入量的点，会在它们的中点处添加两点midPoint0,midPoint1,设变化前后两点
					//分别为p1,p2,则midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
					//p1.x)/2,并且系统会顺次连接p1-midPoint0-midPoint1-p2
					if (temp.x < CHANNEL_INFO_WIDTH)
					{
						temp.x = CHANNEL_INFO_WIDTH;
					}
// 					if (temp.y!=30)
// 					{
// 						Sleep(1);
// 					}
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

				MapPoint(m_pCurve[i].lpReturnSecPoint[returnP],temp,m_pCurve[i].valueOfCellY,
					origitalPoint,nWidthOfCell);
				pDC->MoveTo(temp);
				for(j=returnP;j<endReturn;j++)
				{
					POINT befPoint,midPoint[2];
					befPoint = temp;
					MapPoint(m_pCurve[i].lpReturnSecPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
					if (temp.x < CHANNEL_INFO_WIDTH)
					{
						temp.x = CHANNEL_INFO_WIDTH;
					}
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
		
				MapPoint(m_pCurve[i].lpRetPoint[indexRet],temp,m_pCurve[i].valueOfCellY,
					origitalPoint,nWidthOfCell);
				if (temp.x < CHANNEL_INFO_WIDTH)
				{
					temp.x = CHANNEL_INFO_WIDTH;
				}
				pDC->MoveTo(temp);
				if (indexRet == 0 && (m_pCurve[i].lpRetPoint[indexRet].y!=m_pCurve[i].lpStaPoint[m_pCurve[i].StaCount].y))
				{//开关量有效段返回段中，若第一个值与起始段的最后一个值不一样，画上一条折线；
					Point tempPoint = m_pCurve[i].lpRetPoint[indexRet];
					if (m_pCurve[i].lpRetPoint[indexRet].y > 0)
					{
						tempPoint.y=0;
					}
					else
					{
						tempPoint.y=m_pCurve[i].lpStaPoint[m_pCurve[i].StaCount].y;
					}
					POINT temp2;
					MapPoint(tempPoint,temp2,m_pCurve[i].valueOfCellY,
						origitalPoint,nWidthOfCell);
					if (temp2.x < CHANNEL_INFO_WIDTH)
					{
						temp2.x = CHANNEL_INFO_WIDTH;
					}
					pDC->LineTo(temp2);
				}
				/*在采样段与有效段重合处只显示采样段*/
				if (endRet>(m_pCurve[i].RetCount-m_pCurve[i].returnCount/m_pCurve[i].nPointInCircle*2))
				{
					endRet = m_pCurve[i].RetCount-m_pCurve[i].returnCount/m_pCurve[i].nPointInCircle*2;
				}
				////////////////////////////////////////
				for(j=indexRet;j<=endRet;j++)
				{
					POINT befPoint,midPoint[2];
					befPoint = temp;
					MapPoint(m_pCurve[i].lpRetPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
					if (temp.x < CHANNEL_INFO_WIDTH)
					{
						temp.x = CHANNEL_INFO_WIDTH;
					}
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
 		}
		if (m_flagShow & SAMPLE_VALUE_SHOW)
		{
			if (m_bMultiColorDis && m_flagShow != SAMPLE_VALUE_SHOW)
			{
				pDC->SelectObject(&samplePen);//使用黄色
			}
			if (m_pCurve[i].bIsOverlap)//画起始段和返回段曲线
			{
			 	if (!m_pCurve[i].bIsDigital)
				{
					MapPoint(m_pCurve[i].lpStartSecPoint[startP],temp,m_pCurve[i].valueOfCellY,
						origitalPoint,nWidthOfCell);
					if(temp.x >= CHANNEL_INFO_WIDTH-2 && temp.x <= m_curRect.Width())
					{
						pDC->MoveTo(temp);
					}
					else if (temp.x < CHANNEL_INFO_WIDTH)
					{
						temp.x = CHANNEL_INFO_WIDTH;
						pDC->MoveTo(temp);
					}
					for (int k = startP ; k < endStart ; k++)
					{
						MapPoint(m_pCurve[i].lpStartSecPoint[k+1],temp,m_pCurve[i].valueOfCellY,
							origitalPoint,nWidthOfCell);
						if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
						{
							pDC->LineTo(temp);
							pDC->MoveTo(temp);
						}
					}
					MapPoint(m_pCurve[i].lpReturnSecPoint[returnP],temp,m_pCurve[i].valueOfCellY,
						origitalPoint,nWidthOfCell);
					if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
					{
						pDC->MoveTo(temp);
					}
					else if (temp.x < CHANNEL_INFO_WIDTH)
					{
						temp.x = CHANNEL_INFO_WIDTH;
						pDC->MoveTo(temp);
					}
					for (k = returnP ; k < endReturn ; k++)
					{
						MapPoint(m_pCurve[i].lpReturnSecPoint[k+1],temp,m_pCurve[i].valueOfCellY,
							origitalPoint,nWidthOfCell);
						if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
						{
							pDC->LineTo(temp);
							pDC->MoveTo(temp);
						}
					}
 				}
				else if (m_flagShow == SAMPLE_VALUE_SHOW)
				{
					MapPoint(m_pCurve[i].lpStartSecPoint[startP],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
					if (temp.x < CHANNEL_INFO_WIDTH)
					{
						temp.x = CHANNEL_INFO_WIDTH;
					}
					pDC->MoveTo(temp);
					for(j=startP+1;j<endStart;j++)
					{
						POINT befPoint,midPoint[2];
						befPoint = temp;
						MapPoint(m_pCurve[i].lpStartSecPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
						//对于从0变1或从1变0的开入量的点，会在它们的中点处添加两点midPoint0,midPoint1,设变化前后两点
						//分别为p1,p2,则midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
						//p1.x)/2,并且系统会顺次连接p1-midPoint0-midPoint1-p2
						if (temp.x < CHANNEL_INFO_WIDTH)
						{
							temp.x = CHANNEL_INFO_WIDTH;
						}
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
					
					MapPoint(m_pCurve[i].lpReturnSecPoint[returnP],temp,m_pCurve[i].valueOfCellY,
						origitalPoint,nWidthOfCell);
					pDC->MoveTo(temp);
					for(j=returnP;j<endReturn;j++)
					{
						POINT befPoint,midPoint[2];
						befPoint = temp;
						MapPoint(m_pCurve[i].lpReturnSecPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
						if (temp.x < CHANNEL_INFO_WIDTH)
						{
							temp.x = CHANNEL_INFO_WIDTH;
						}
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
		if (m_TimeCloseOut != 0)
		//绘合闸出口入红线
		{
			if (m_dota>0)
			{
				point.x = m_TimeCloseOut+((CMainFrame *)GetParentFrame())->m_CloseTimeSet+1000;
			}
			else
			{
				point.x = m_TimeCloseOut+((CMainFrame *)GetParentFrame())->m_CloseTimeSet;
			}
			
			point.y=0;
			MapPoint(point,temp,0,origitalPoint,nWidthOfCell);
			if(temp.x > CHANNEL_INFO_WIDTH && temp.x < m_curRect.Width())
			{
				pDC->SelectObject(&pen2);
				pDC->MoveTo(temp.x,nTopOfCoor);
				pDC->LineTo(temp.x,nTopOfCoor+nWidthOfCell*CELL_COUNT);				
			}
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
	pDC->SelectObject(pOldPen);
}

/*****************************************************
函数名：SetMulti
描述：改变显示的倍率
参数：pDC[in]：画图句柄
返回：void
*****************************************************/
void CCurveView::SetMulti(CDC *pDC)
{
	if(DoubleCmp(m_dNewMulti-m_dMulti)!=0)
	{
		m_dMulti = m_dNewMulti;
		SetCoorHeight(m_nHeightOfDigitalCoor,m_nHeightOfAnalogCoor);
		::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	}
	DrawGridLine(pDC);
	DrawLine(pDC);
	
}

void CCurveView::DrawBackBitmap(CDC* pDC)
{
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP_LOGO);/* IDB_BITMAP1   */
	BITMAP bm;
	bmp.GetBitmap (&bm);
	CPoint pointScrollPos = GetScrollPosition();
	//DrawTransparentBitmap( bmp, m_defChananlInfoWide, m_nHeightOfRCScale, bm.bmWidth, bm.bmHeight, m_defChananlInfoWide, m_nHeightOfRCScale, pDC, m_bkColor );
	int hight = m_rectCurve.Height();
	if (  hight >  bm.bmHeight )
	{
		hight = bm.bmHeight;
	}else
	{
		hight -= pointScrollPos.y;
	}
	DrawTransparentBitmap( bmp, pDC, CHANNEL_INFO_WIDTH , m_nHeightOfRCScale + pointScrollPos.y, bm.bmWidth, hight, m_bkColor);
}

void CCurveView::DrawTransparentBitmap(CBitmap &bmp, CDC *pDC, int x, int y, int nWidth, int nHeight, COLORREF crColour)
{
	COLORREF crOldBack=pDC->SetBkColor(RGB(255,255,255));
	COLORREF crOldText=pDC->SetTextColor(RGB(0,0,0));
	CDC dcImage, dcMask;
	//IDB_BITMAP1 是待显示位图的资源ID
	BITMAP bm;
	bmp.GetBitmap(&bm);
	//int nWidth=bm.bmWidth,nHeight=bm.bmHeight;
	//为图像及mask 各创建一个DC
	dcImage.CreateCompatibleDC(pDC);
	dcMask.CreateCompatibleDC(pDC);
	//把图像装入image DC
	CBitmap* pOldBitmapImage=dcImage.SelectObject(&bmp);
	//为“掩码”位图创建一个单色bitmap
	CBitmap bitmapMask;
	bitmapMask.CreateBitmap(nWidth, nHeight, 1, 1, NULL); //把mask 位图装入mask DC
	CBitmap* pOldBitmapMask = dcMask.SelectObject(&bitmapMask); //用透明色创建“掩码”位图
	dcImage.SetBkColor(crColour);//crColor 是位图中的透明色
	dcMask.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);//分3 步进行实际的绘制
	pDC->BitBlt(x, y, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcMask, 0, 0, SRCAND);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT); //恢复原先设置
	dcImage.SelectObject(pOldBitmapImage);
	dcMask.SelectObject(pOldBitmapMask);
	pDC->SetBkColor(crOldBack);
	pDC->SetTextColor(crOldText);
}

void CCurveView::DrawGridLine(CDC *pDC)
{
	CPen pen1(PS_SOLID,1,m_coordinateColor);
	CPen pen2(PS_DASH,1,m_coordinateColor);
	CPen pen3(PS_SOLID,1,RGB(233,233,233));
	CPen penGird(PS_DASH,1,m_colorGirdLine);
	CPen *pOldPen=pDC->SelectObject(&pen1);
	CBrush brush(RGB(233,233,233));
	POINT temp1,temp2,origitalPoint;
	UINT nWidthOfCell,nTopOfCoor;  //nTopOfCoor当前坐标系的顶坐标
	//y轴
	pDC->MoveTo(m_rectCurve.left,m_rectCurve.top);
	pDC->LineTo(m_rectCurve.left,m_rectCurve.bottom);
	//横向线条
	int i,nGirdHeight,nDisCoorHeight = m_nHeightOfAnalogCoor/2;
	UINT nTopOfY=m_rectCurve.top;

	pDC->SelectObject(&pen2);
	m_nFirstDisCurve = -1;
	m_nLastDisCurve = -1;
	UINT nIntervalOfCoorMid,nIntervalOfCoorTop=0; 
	origitalPoint.x=CHANNEL_INFO_WIDTH;
	origitalPoint.y=0;
	nTopOfCoor = m_rectCurve.top;
	for(i=0;i<m_nCurveCount;i++)
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
			MapPoint(m_pCurve[i].lpStaPoint[m_pCurve[i].StaCount],temp1,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);			
			if(temp1.x >= CHANNEL_INFO_WIDTH)
			{
				if(temp1.x > m_curRect.Width())
					temp1.x = m_curRect.Width();
				pDC->MoveTo(m_rectCurve.left,nTopOfY+nIntervalOfCoorMid);
				pDC->LineTo(temp1.x,nTopOfY+nIntervalOfCoorMid);
			}
			MapPoint(m_pCurve[i].lpRetPoint[0],temp2,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
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
			while(nGirdHeight <= nDisCoorHeight)//从中间蓝线向两边各画三条刻度线
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
		if(temp1.x<m_rectCurve.left)
			temp1.x=m_rectCurve.left+1;
		if(temp2.x>m_rectCurve.left)
		{
			pDC->Rectangle(temp1.x,m_rectCurve.top,temp2.x,m_rectCurve.bottom);
		}
	}
	if(m_nLastDisCurve == -1)
	{
		m_nLastDisCurve = m_nCurveCount-1;
	}
	pDC->SelectObject(pOldPen);
}
//画曲线
void CCurveView::DrawCurve(CDC *pDC)
//鼠标在坐标轴范围内移动，或是滚动条左右移动时，都将引起曲线及刻度线的重绘
//everytime the RedrawWindow member function is called ,do the DrawCurve movement!
{
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	int totalTime = pMainFrm->m_wareFileData.m_nCycleCount * 20;				//总的采样时间：周波数 * 周期
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
	CPen *pOldPen;
	CBrush *pOldBrush;
	long oldmode;
	CDC MemDC; //首先定义一个显示设备对象
	CBitmap MemBitmap;//定义一个位图对象
	CSize logicTotalSize;
	logicTotalSize = GetTotalSize();
	logicTotalSize.cx = (logicTotalSize.cx > 2*m_curRect.Width())?2*m_curRect.Width():logicTotalSize.cx;
	CRect rcDC(0,0,logicTotalSize.cx,logicTotalSize.cy);
	//随后建立与屏幕显示兼容的内存显示设备
	VERIFY(MemDC.CreateCompatibleDC(NULL));
	//这时还不能绘图，因为没有地方画 ^_^
	//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
	VERIFY(MemBitmap.CreateCompatibleBitmap(pDC,rcDC.Width(),rcDC.Height()));
	//将位图选入到内存显示设备中
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);
	//MemDC.SelectObject(pOldBit);
	//先用背景色将位图清除干净，这里我用的是白色作为背景
	//你也可以用自己应该用的颜色
	MemDC.FillSolidRect(rcDC,RGB(255,255,255));
// 	MemDC.FillSolidRect(rcDC,RGB(0,0,0));//DEBUG-TEST-ZYS

	MemDC.FillSolidRect(m_rectCurve,m_bkColor);
	DrawBackBitmap(&MemDC);
	//绘图
	oldmode=MemDC.SetBkMode(TRANSPARENT);
	//此处包含画曲线，和画表格线的调用
	SetMulti(&MemDC);
	MemDC.SetBkMode(oldmode);
	int startSec,returnSec,staSec,retSec; //retSec = 启动红线时间，staSec = 启动前段时间
	CFont font,*pOldFont;
	font.CreatePointFont(100,_T("宋体"));
	pOldFont=MemDC.SelectObject(&font);
	CString str=_T("名  称:");
	int i;
	CSize size;
	size=MemDC.GetTextExtent(str);
	//如果鼠标线在坐标区域内，则需要画制跟随鼠标的竖线x=m_mousePoint.x,并且要计算出从
	//m_nFirstDisCurve到m_nLastDisCurve通道的瞬时值和模拟量通道的相位角,还有基线与鼠标
	//竖线的时间差和当前鼠标线的精确时间值(包含年月日时分秒)
	CString info;
	if(m_mousePoint.x>0)
	{
 		CPen crossPen(PS_SOLID,1,m_crossLineColor);
 		pOldPen=MemDC.SelectObject(&crossPen);
		MemDC.MoveTo(m_mousePoint.x-m_curRect.left,m_rectCurve.top);
		MemDC.LineTo(m_mousePoint.x-m_curRect.left,m_rectCurve.bottom);
		MemDC.SelectObject(pOldPen);
		Point pLogic;  //鼠标指向波形的时间差(从波形起始处算起)
		POINT origital;
		origital.x = m_rectCurve.left;
		origital.y = m_nHeightOfAnalogCoor/2;
		//与起始时间点的时间差
		MapPointToLogic(m_mousePoint,pLogic,0,origital,m_nHeightOfAnalogCoor/CELL_COUNT);//获取鼠标点的横坐标
		double milliSecond;
		
		int startTime;
		if (m_pCurve[2].nPointInCircle)
		{
			startTime=m_pCurve[2].startCount/m_pCurve[2].nPointInCircle*20;
		}
		else
		{
			startTime = 0;
		}
		int returnTime=startTime+intersect;
		if(m_isLabel&&pLogic.x<returnTime&&pLogic.x>=startTime&&m_dValueOfClick<startTime)
		{
			milliSecond=returnTime+m_pCurve[2].milliSecondOfFirst;
		}
		else if(m_isLabel&&pLogic.x>=returnTime&&m_dValueOfClick<startTime)
		{
			milliSecond=pLogic.x+ m_dota - intersect +m_pCurve[2].milliSecondOfFirst;
		}
		else if(m_isLabel&&pLogic.x<returnTime&&pLogic.x>=startTime&&m_dValueOfClick>=returnTime)
		{
			milliSecond=m_pCurve[2].milliSecondOfFirst+m_dota+returnTime;
		}
		else if(m_isLabel&&m_dValueOfClick>=returnTime&&pLogic.x<=startTime)
		{
			milliSecond=returnTime+m_pCurve[2].milliSecondOfFirst-(returnTime-pLogic.x);
		}
		else if(m_isLabel&&m_dValueOfClick>=returnTime)
		{
			milliSecond=m_pCurve[2].milliSecondOfFirst+pLogic.x+m_dota-intersect;
		}
		else
		{
			milliSecond=pLogic.x+m_pCurve[2].milliSecondOfFirst;
		}
		COleDateTimeSpan span;
		span.SetDateTimeSpan(0,0,0,milliSecond/1000);	
		str.Format(_T("%s:%.1fms"),span.Format(_T("%Y-%m-%d %H:%M:%S")),milliSecond-(int)milliSecond/1000*1000);
		
		COleDateTime dateTime;	
		dateTime=m_pCurve[2].firstSampleTime;
		str.Format(_T("%s:%.1fms"),dateTime.Format(_T("%Y-%m-%d %H:%M:%S")),milliSecond-(int)milliSecond/1000*1000);
		
		dateTime=m_pCurve[2].firstSampleTime+span;
		str.Format(_T("%s:%.1fms"),dateTime.Format(_T("%Y-%m-%d %H:%M:%S")),milliSecond-(int)milliSecond/1000*1000);
		
		//double d1=milliSecond-(int)milliSecond/1000*1000;
		((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneText(1,str);
		if(m_isLabel&&pLogic.x>returnTime&&m_dValueOfClick<startTime)
		{
			milliSecond = pLogic.x + m_dota - intersect - m_dValueOfClick;
		}
		else if(m_isLabel&&pLogic.x<startTime&&m_dValueOfClick>returnTime)
		{
			milliSecond = pLogic.x - m_dota + intersect - m_dValueOfClick;
		}
		else
		{
			milliSecond = pLogic.x - m_dValueOfClick;
		}
	//	pLogic.x = milliSecond + m_dValueOfClick;
		info.LoadString( IDS_INDICATOR_TIME );
		str.Format(_T("%s：%.1fms"), info, milliSecond);
		((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneText(2,str);
		//SetStatusTextMid(str);
		long indexAvail,indexStart,indexReturn;
		UINT nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //坐标系的顶纵坐标
		origital.x = m_rectCurve.left;
		for(i=0;i<m_nFirstDisCurve;i++)
		{
			if(m_pCurve[i].bIsDigital)
			{
				nWidthOfCoor = m_nHeightOfDigitalCoor;
			}
			else
			{
				nWidthOfCoor = m_nHeightOfAnalogCoor;
			}
			nTopToCoor += nWidthOfCoor;
		}
		CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
		Point  *pTempPoint = new Point[6*m_pCurve[0].nPointInCircle]; //4周波临时点2015-1-30 add
		for(i=m_nFirstDisCurve;i<=m_nLastDisCurve;i++)
		{
			staSec = pMainFrm->m_wareFileData.m_nAvailStartCycleCount*20;
			//lhz 全程启动后段的起始时间应该为
			//retSec = m_pCurve[i].startTime + intersect;
			retSec = staSec + intersect;
			//end 
			if (m_pCurve[i].bIsOverlap)				//当他是模拟量数据的时候就会计算出开始采样段与起始时间点的时间间隔、返回采样段的开始时间
			{
				startSec  = m_pCurve[2].startCount/m_pCurve[2].nPointInCircle*20;
				returnSec = totalTime - m_pCurve[2].returnCount/m_pCurve[2].nPointInCircle*20 - m_dota + intersect; //x值：采样返回前段的时间ms
			}
			if(m_pCurve[i].bIsDigital)
			{
				origital.y = nTopToCoor + m_nHeightOfDigitalCoor/2;
				nWidthOfCoor = m_nHeightOfDigitalCoor;
			}
			else
			{
				origital.y = nTopToCoor + m_nHeightOfAnalogCoor/2;
				nWidthOfCoor = m_nHeightOfAnalogCoor;
			}
			CRect rc(m_clientRc.left+5,origital.y-5,m_clientRc.left+15,origital.y+5);
			CPen pen(PS_SOLID,1,m_pCurve[i].color),strPen(PS_SOLID,1,RGB(0,0,0));
			CBrush brush;
			double tempAngle;
			if(m_bMultiColorDis)//判断是否彩色显示
			{
				brush.CreateSolidBrush(m_pCurve[i].color);
				pOldPen=MemDC.SelectObject(&pen);
				pOldBrush=MemDC.SelectObject(&brush);
				MemDC.Ellipse(&rc);//RC先画一个小圆，标注
				MemDC.SelectObject(pOldBrush);
				MemDC.SelectObject(pOldPen);
				rc.left=m_clientRc.left+20;
				rc.top=origital.y-size.cy/2;
				rc.right=m_clientRc.left+CHANNEL_INFO_WIDTH;
				rc.bottom=origital.y+size.cy;//RC，标注当前通道
			}
			else//黑白显示
			{
				rc.left=m_clientRc.left+5;
				rc.top=origital.y-size.cy/2;
				rc.right=m_clientRc.left+CHANNEL_INFO_WIDTH;
				rc.bottom=origital.y+size.cy;
			}
			oldmode=MemDC.SetBkMode(TRANSPARENT);
			pOldPen=MemDC.SelectObject(&strPen);
			if (staSec>pLogic.x)
			{
				indexAvail=BinarySearch(m_pCurve[i].lpStaPoint,pLogic,m_pCurve[i].StaCount);
			}
			else if (pLogic.x >= retSec )
			{
				indexAvail=BinarySearch(m_pCurve[i].lpRetPoint,pLogic,m_pCurve[i].RetCount);
			}
			else
			{
				indexAvail = 0;
			}
			CString temp;
			if(m_pCurve[i].bIsDigital)
			{
				if (pLogic.x < startSec)
				{
					int temIndex = BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
					
					if(DoubleCmp(m_pCurve[i].lpStartSecPoint[temIndex].y))
					{
						temp.Format(_T("%-10s:1"),m_pCurve[i].name);
					}
					else
					{
						temp.Format(_T("%-10s:0"),m_pCurve[i].name);
					}
				}
				else if (pLogic.x<staSec)
				{
					if(DoubleCmp(m_pCurve[i].lpStaPoint[indexAvail].y))
					{
						temp.Format(_T("%-10s:1"),m_pCurve[i].name);
					}
					else
					{
						temp.Format(_T("%-10s:0"),m_pCurve[i].name);
					}
				}
				else if (pLogic.x > returnSec)
				{
					int temIndex = BinarySearch(m_pCurve[i].lpReturnSecPoint,pLogic,m_pCurve[i].returnCount);
					
					if(DoubleCmp(m_pCurve[i].lpReturnSecPoint[temIndex].y))
					{
						temp.Format(_T("%-10s:1"),m_pCurve[i].name);
					}
					else
					{
						temp.Format(_T("%-10s:0"),m_pCurve[i].name);
					}
				}
				else if (pLogic.x > retSec)
				{
					if(DoubleCmp(m_pCurve[i].lpRetPoint[indexAvail].y))
					{
						temp.Format(_T("%-10s:1"),m_pCurve[i].name);
					}
					else
					{
						temp.Format(_T("%-10s:0"),m_pCurve[i].name);
					}
				}
				else
				{
					if(DoubleCmp(m_pCurve[i].lpStaPoint[indexAvail].y))
					{
						temp.Format(_T("%-10s:1"),m_pCurve[i].name);
					}
					else
					{
						temp.Format(_T("%-10s:0"),m_pCurve[i].name);
					}

				}
			}
			else 
			{
				double dFreq=0,dSwing=0,dAngle=0;
				int systemAngleSet = pMainFrm->m_SystemAngleSet;
				bool flag=true;//标识是否处于采样值区域
				wchar_t cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
				BOOL isAngle = FALSE;
	  
				if ( m_pCurve[i].name.Find(_T("主控插件相角差")) >= 0 || m_pCurve[i].name.Find( _T("Mainboard Angle Diff.") ) >= 0 )
				{
					isAngle = TRUE;
				}
				CString volStr;
				volStr.LoadString( IDS_PULSAT_VOL );		//脉振电压
				if (m_pCurve[i].name == volStr )
				{
					info.LoadString( IDS_PERUNIT_VALUE );		//标么值
					if(pLogic.x < startSec/* - m_fMZstartDota*/)
					{
						indexStart = BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
						str.Format(_T("\n%s:%.3f"), info,
							m_pCurve[i].lpStartSecPoint[indexStart].y);
					}
					else if (pLogic.x > returnSec/* - m_fMZreturnDota*/)
					{
						indexReturn = BinarySearch(m_pCurve[i].lpReturnSecPoint,pLogic,m_pCurve[i].returnCount);
						str.Format(_T("\n%s:%.3f"), info,
							m_pCurve[i].lpReturnSecPoint[indexReturn].y);
					}
					else
					{
						str.Format(_T("\n"));
					}
				}
				else			//m_pCurve[i].bIsOverlap:有起始段、返回段吗？
				{
					if(pLogic.x < startSec && m_pCurve[i].bIsOverlap)			//开始采样段
					{
						indexStart = BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
						//if(!GetFreqSwingAngle(dFreq,dSwing,dAngle,i,indexStart,TRUE))
						if(!GetFreqSwingAngle(dFreq,dSwing,dAngle,i,indexStart,m_pCurve[i].lpStartSecPoint,m_pCurve[i].startCount))
						{
							dAngle = 0;
							dFreq = 0;
							dSwing = 0;
						}
					}
					if (pLogic.x > returnSec && m_pCurve[i].bIsOverlap)			//返回采样段
					{
						indexReturn = BinarySearch(m_pCurve[i].lpReturnSecPoint,pLogic,m_pCurve[i].returnCount);
						//if(!GetFreqSwingAngle(dFreq,dSwing,dAngle,i,indexReturn,FALSE))
						if(!GetFreqSwingAngle(dFreq,dSwing,dAngle,i,indexReturn,m_pCurve[i].lpReturnSecPoint, m_pCurve[i].returnCount))
						{
							dAngle = 0;
							dFreq = 0;
							dSwing = 0;
						}
						if ( 0 == dAngle && indexReturn < m_pCurve[i].nPointInCircle*2 )
						{
							float diffTime = m_pCurve[i].lpReturnSecPoint[0].x - m_pCurve[i].lpStartSecPoint[m_pCurve[i].startCount - 1].x;
							int tempCount  = m_pCurve[i].nPointInCircle*2;
							if ( diffTime < (40.0/m_pCurve[i].nPointInCircle) && indexReturn < tempCount
								&& m_pCurve[i].startCount > tempCount && m_pCurve[i].returnCount > 2*tempCount   )
							{//两段数据小于2个点 
								memcpy( pTempPoint, &m_pCurve[i].lpStartSecPoint[ m_pCurve[i].startCount - 1 - tempCount ], tempCount*sizeof(pTempPoint[0]) );
								memcpy( pTempPoint + tempCount, m_pCurve[i].lpReturnSecPoint, tempCount*2*sizeof(pTempPoint[0]) );
							} 
							GetFreqSwingAngle(dFreq,dSwing,dAngle,i, indexReturn+tempCount, pTempPoint, tempCount*3 );
						}
                            
					}
					info.LoadString( IDS_SAMPLE_VALUE );			//采样值
					CString effValueStr, tempStr;
					effValueStr.LoadString( IDS_EFFECTIVE_VALUE );			//有效值
					if (pLogic.x < startSec && pLogic.x < staSec && m_flagShow == ALL_VALUE_SHOW && m_pCurve[i].bIsOverlap && !isAngle)
					{
					
						str.Format(_T("\n%s:%.3f%s\n%s:%.3f%s"), effValueStr,
							m_pCurve[i].lpStaPoint[indexAvail].y+m_average[i],m_pCurve[i].unit, info,
							m_pCurve[i].lpStartSecPoint[indexStart].y+m_average[i],m_pCurve[i].unit);
					}
					else if (pLogic.x < startSec && pLogic.x < staSec && m_flagShow == ALL_VALUE_SHOW && m_pCurve[i].bIsOverlap && isAngle)
					{
						tempAngle = m_pCurve[i].lpStaPoint[indexAvail].y+m_average[i]-systemAngleSet;
						tempAngle = tempAngle>180?tempAngle-360:tempAngle;
					
						tempStr.LoadString( IDS_SYS_ANGLE_DIFF );
						str.Format(_T(" %.1f%s\n%s:%.1f%s"),
							m_pCurve[i].lpStaPoint[indexAvail].y+m_average[i],m_pCurve[i].unit,
							tempStr,tempAngle,m_pCurve[i].unit);

					}
					else if (pLogic.x < startSec && m_flagShow == SAMPLE_VALUE_SHOW && m_pCurve[i].bIsOverlap && !isAngle)
					{
						str.Format(_T("\n%s:%.3f%s"), info, m_pCurve[i].lpStartSecPoint[indexStart].y+m_average[i],
							m_pCurve[i].unit);
					}
	// 				else if (pLogic.x < startSec && m_flagShow == SAMPLE_VALUE_SHOW && m_pCurve[i].bIsOverlap && isAngle)
	// 				{
	// 					str.Format(_T("\n分析值:%.1f%s"),m_pCurve[i].lpStartSecPoint[indexStart].y+m_average[i],
	// 						m_pCurve[i].unit);
	// 				}
					else if (pLogic.x > returnSec && pLogic.x > retSec && m_flagShow == ALL_VALUE_SHOW && m_pCurve[i].bIsOverlap && !isAngle)
					{
						str.Format(_T("\n%s:%.3f%s\n%s:%.3f%s"), effValueStr,
							m_pCurve[i].lpRetPoint[indexAvail].y+m_average[i],m_pCurve[i].unit,
							info, m_pCurve[i].lpReturnSecPoint[indexReturn].y+m_average[i],m_pCurve[i].unit);
					}
					else if (pLogic.x > returnSec && pLogic.x > retSec && m_flagShow == ALL_VALUE_SHOW && m_pCurve[i].bIsOverlap && isAngle)
					{
						tempAngle = m_pCurve[i].lpRetPoint[indexAvail].y+m_average[i]-systemAngleSet;
						tempAngle = tempAngle>180?360-tempAngle:tempAngle;
						tempStr.LoadString( IDS_SYS_ANGLE_DIFF );
						str.Format(_T(" %.1f%s\n%s:%.1f%s"),
							m_pCurve[i].lpRetPoint[indexAvail].y+m_average[i],m_pCurve[i].unit,
							tempStr, tempAngle,m_pCurve[i].unit);
					}
					else if (pLogic.x > returnSec && m_flagShow == SAMPLE_VALUE_SHOW && m_pCurve[i].bIsOverlap && !isAngle)
					{
						str.Format(_T("\n%s:%.3f%s"), info, m_pCurve[i].lpReturnSecPoint[indexReturn].y+m_average[i],
							m_pCurve[i].unit);
					}
	// 				else if (pLogic.x > returnSec && m_flagShow == SAMPLE_VALUE_SHOW && m_pCurve[i].bIsOverlap && isAngle)
	// 				{
	// 					str.Format(_T("\n分析值:%.1f%s"),m_pCurve[i].lpReturnSecPoint[indexReturn].y+m_average[i],
	// 						m_pCurve[i].unit);
	// 				}
					else if (pLogic.x >= retSec)			//感觉除起始采样段以外的全在这里执行
					{//retSec >= 500ms
						flag = FALSE;
						if (m_flagShow == SAMPLE_VALUE_SHOW)
						{
							str.Format(_T(""));
						}
						else if (!isAngle)
						{
							str.Format(_T("\n%s:%.3f%s\n"), effValueStr,m_pCurve[i].lpRetPoint[indexAvail].y+m_average[i],m_pCurve[i].unit);
						}
						else 
						{
							Point	pCurTimeDiffPos = pLogic;
							pCurTimeDiffPos.x -= 3;	
							int		nCurTimeDot = BinarySearch(m_pCurve[i].lpRetPoint,pCurTimeDiffPos,m_pCurve[i].RetCount);
							if ((int)pCurTimeDiffPos.x%10 < 5)
							{
								nCurTimeDot = 2 * nCurTimeDot + 1;
							}
							else
							{
								nCurTimeDot = 2 * nCurTimeDot;
							}
							tempAngle = lpRetPhaseAngle[nCurTimeDot] + m_average[i]-systemAngleSet;
							//tempAngle = m_pCurve[i].lpRetPoint[indexAvail].y+m_average[i]-systemAngleSet;
							tempAngle = tempAngle>180?tempAngle-360:tempAngle;
							tempStr.LoadString( IDS_SYS_ANGLE_DIFF );
							//str.Format(_T(" %.1f%s\n%s:%.1f%s\n"),m_pCurve[i].lpRetPoint[indexAvail].y+m_average[i],m_pCurve[i].unit,tempStr, tempAngle,m_pCurve[i].unit);
							str.Format(_T(" %.1f%s\n%s:%.1f%s\n"), lpRetPhaseAngle[nCurTimeDot]+m_average[i],m_pCurve[i].unit,tempStr, tempAngle,m_pCurve[i].unit);
						}
					}
					else if (pLogic.x < staSec)				//起始段会到这里执行
					{//pLogic.x<500
						flag = FALSE;
						if (m_flagShow == SAMPLE_VALUE_SHOW)
						{
							str.Format(_T(""));
						}
						else if (!isAngle)
						{
							str.Format(_T("\n%s:%.3f%s\n"), effValueStr, m_pCurve[i].lpStaPoint[indexAvail].y+m_average[i],m_pCurve[i].unit);
						}
						else
						{
							Point	pCurTimeDiffPos = pLogic;
							int		nCurTimeDot = BinarySearch(m_pCurve[i].lpRetPoint,pCurTimeDiffPos,m_pCurve[i].RetCount);
							if ((int)pCurTimeDiffPos.x%10 < 5)
							{
								nCurTimeDot = 2 * pCurTimeDiffPos.x/10 + 1;
							}
							else
							{
								nCurTimeDot = 2 * pCurTimeDiffPos.x/10;
							}
							tempStr.LoadString( IDS_SYS_ANGLE_DIFF );
							//tempAngle = m_pCurve[i].lpStaPoint[indexAvail].y+m_average[i]-systemAngleSet;
							tempAngle = lpStaPhaseAngle[nCurTimeDot] + m_average[i]-systemAngleSet;
							tempAngle = tempAngle>180?tempAngle-360:tempAngle;
							//str.Format(_T(" %.1f%s\n%s:%.1f%s\n"),m_pCurve[i].lpStaPoint[indexAvail].y+m_average[i],m_pCurve[i].unit,tempStr,tempAngle,m_pCurve[i].unit);
							str.Format(_T(" %.1f%s\n%s:%.1f%s\n"), lpStaPhaseAngle[nCurTimeDot]+m_average[i],m_pCurve[i].unit,tempStr, tempAngle,m_pCurve[i].unit);
						}
					}
					else
					{
						flag = FALSE;
						if (m_flagShow == SAMPLE_VALUE_SHOW)
						{
							str.Format(_T(""));
						}
						else
						{
							str.Format(_T("\n%s:%.3f%s\n"), effValueStr, m_pCurve[i].lpStaPoint[indexAvail].y+m_average[i],m_pCurve[i].unit);
						}

					}
				}
				
				cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
				if(cUint == TEXT('a') || cUint == TEXT('A') || cUint == TEXT('v') || cUint == TEXT('V'))
				{
					rc.top -= size.cy/2+5;
					rc.bottom += size.cy+5;
					temp.Format(_T("%s %-8s"),m_pCurve[i].name,str);
					if (flag)
					{
						temp.Format(_T("%s %-8s  %.1f°"),m_pCurve[i].name,str,dAngle);
					}
				}
				else
				{
					rc.top -= size.cy/2+5;
					rc.bottom += size.cy+5;
					temp.Format(_T("%s  %-10s"),m_pCurve[i].name,str);
				}
			}
			m_bMultiColorDis ? str.Format(_T("%s"),temp) : str.Format(_T("%d.%s"),i,temp);
			MemDC.DrawText(str,&rc,DT_VCENTER);
			MemDC.SetBkMode(oldmode);
			MemDC.SelectObject(pOldPen);
			nTopToCoor += nWidthOfCoor;
//			CString cDebug;	cDebug.Format(_T("DrawCurve1111**%d**:%s"), i, str);MessageBox(cDebug);//DEBUG-TEST-ZYS
		}
		delete [] pTempPoint;
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
				
				origital.y = nTopToCoor + m_nHeightOfAnalogCoor/2;
				nWidthOfCoor = m_nHeightOfAnalogCoor;
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
				str.Format(_T("%s"),m_pCurve[i].name);
			}
			else
			{
				rc.left=m_clientRc.left+5;
				rc.top=origital.y-size.cy/2;
				rc.right=m_clientRc.left+120+size.cx;
				rc.bottom=origital.y+size.cy/2;
				str.Format(_T("%d.%s"),i,m_pCurve[i].name);
			}
			oldmode=MemDC.SetBkMode(TRANSPARENT);
			pOldPen=MemDC.SelectObject(&strPen);
			
			MemDC.DrawText(str,&rc,DT_LEFT);
			MemDC.SetBkMode(oldmode);
			MemDC.SelectObject(pOldPen);
			nTopToCoor += nWidthOfCoor;
//			CString cDebug;cDebug.Format(_T("DrawCurve2222**%d**:%s"), i, str);MessageBox(cDebug); //DEBUG-TEST-ZYS
		}
	}
	MemDC.SelectObject(pOldFont);
	DrawScale(&MemDC,m_isLabel,m_dota);//把刻度线也画入位图中
	//2016-10-27  pwq
	DrawEventFlag(&MemDC);
	if(m_mousePoint.x>0)
	{
		//画事件发生提示内容
		DrawEventToolTips(&MemDC);
	}
	pDC->BitBlt(0,0,m_curRect.Width(),logicTotalSize.cy-m_curRect.top,
		&MemDC,0,m_curRect.top,SRCCOPY);
// 	pDC->BitBlt(CHANNEL_INFO_WIDTH+1,0,m_curRect.Width(),m_curRect.Height(),
// 		&MemDC,m_curRect.left+CHANNEL_INFO_WIDTH+1,m_curRect.top,SRCCOPY);
	//  绘图完成后的清理
	MemBitmap.DeleteObject();
	MemDC.SelectObject(pOldBit);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	m_mousePoint.x -= pointScrollPos.x;
	m_mousePoint.y -= pointScrollPos.y;
}

//2016-9-21 addd 简当的两点之间线性插值      不好意思你没有被使用
double CCurveView::Interpolation(double MouseLogicPointX, int MeasurePointIndex, int arrayCnt,Point *pMeaPoints ) const 
{

	Point point0, point1;
 	double yYalue = 0.0f;
	long ret = DoubleCmp( MouseLogicPointX - pMeaPoints[MeasurePointIndex].x );
	if ( ret > 0 && (MeasurePointIndex+1) < arrayCnt )
	{//在参考点右边
		memcpy( &point0, pMeaPoints+MeasurePointIndex, sizeof(Point) );
		memcpy( &point1, pMeaPoints+MeasurePointIndex+1, sizeof(Point) );
		//yYalue = Interpolation( MouseLogicPoint.x, point0, point1 ); 
		yYalue = ( ( point1.y - point0.y )/(point1.x - point0.x) )*( MouseLogicPointX - point0.x ) + point0.y;
	}else if( ret < 0  && MeasurePointIndex > 0)
	{//在参考点左边
		memcpy( &point0, pMeaPoints+(MeasurePointIndex-1), sizeof(Point) );
		memcpy( &point1, pMeaPoints+MeasurePointIndex, sizeof(Point) );
		yYalue =  ( ( point1.y - point0.y )/(point1.x - point0.x) )*( MouseLogicPointX - point0.x ) + point0.y;
	}else
	{
		yYalue = pMeaPoints[MeasurePointIndex].y;
 	}

    return yYalue;
}

void CCurveView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	CSize sizeTotal;
	CRect rc,rcView;
	GetWindowRect(&rcView);
	GetDesktopWindow()->GetWindowRect(&rc);
	rc.bottom -= rcView.top+HEIGHT_INFO+30;
	int height[2];
	height[0] = m_clientRc.Height();
	height[1] = rc.Height()-20;
	sizeTotal.cy = max(height[0],height[1]);
	//当不需要垂直滚动时需要把垂直滚动条的宽度加回来否则窗口会变窄
	if(sizeTotal.cy>m_clientRc.Height())
	{
		sizeTotal.cx=m_clientRc.Width()+20;
	}
	else
	{
		sizeTotal.cx = m_clientRc.Width();
	}

// 	//计算最右点的横坐标
	
	SetScrollSizes(MM_TEXT,sizeTotal);
	
	ResizeParentToFit();   
}
void CCurveView::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	PAINTSTRUCT ps;
	CDC* pDC=BeginPaint(&ps) ;
	if (pDC==NULL)
	{
		return;
	}	 
	//曲线的条数
	if(!m_bPrint && m_nCurveCount > 0 )
	{
		DrawCurve(pDC);
	}
	EndPaint(&ps);
	// Do not call CScrollView::OnPaint() for painting messages
}
//滚动窗口
BOOL CCurveView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{      
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_TRACKPOS;

	if (LOBYTE(nScrollCode) == SB_THUMBTRACK)
	{
		GetScrollInfo(SB_HORZ, &info);
		nPos = info.nTrackPos;
	}	
	if (HIBYTE(nScrollCode) == SB_THUMBTRACK)
	{
		GetScrollInfo(SB_VERT, &info);
		nPos = info.nTrackPos;
	}

	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}
//窗口滚动时画布移动
BOOL CCurveView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{
	// TODO: Add your specialized code here and/or call the base class
	int xOrig, x;
	int yOrig, y;
	
	// don't scroll if there is no valid scroll range (ie. no scroll bar)
	CScrollBar* pBar;
	DWORD dwStyle = GetStyle();
	pBar = GetScrollBarCtrl(SB_VERT);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_VSCROLL)))
	{
		// vertical scroll bar not enabled
		sizeScroll.cy = 0;
	}
	pBar = GetScrollBarCtrl(SB_HORZ);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_HSCROLL)))
	{
		// horizontal scroll bar not enabled
		sizeScroll.cx = 0;
	}
	
	// adjust current x position
	xOrig = x = GetScrollPos(SB_HORZ);
	int xMax = GetScrollLimit(SB_HORZ);
	x += sizeScroll.cx;
	if (x < 0)
		x = 0;
	else if (x > xMax)
		x = xMax;
	
	// adjust current y position
	yOrig = y = GetScrollPos(SB_VERT);
	int yMax = GetScrollLimit(SB_VERT);
	y += sizeScroll.cy;
	if (y < 0)
		y = 0;
	else if (y > yMax)
		y = yMax;
	
	// did anything change?
	if (x == xOrig && y == yOrig)
		return FALSE;
	
	if (bDoScroll)
	{
		// do scroll and update scroll positions
		//(与CScrollView::OnScrollBy所不同的就在这一个调用，
		//因为如何的滚动由的我们的CCurveView::DrawCurve已经做了，
		//如果这里再从重调于ScrollWindow，会让界面闪烁)
		//ScrollWindow(-(x-xOrig), -(y-yOrig));
		if (x != xOrig)
		{
			SetScrollPos(SB_HORZ, x);
		}
		if (y != yOrig)
		{
			
			SetScrollPos(SB_VERT, y);
		}
		//CScrollView::OnScrollBy中没有此调用，这里调用是让我们的CCurveView::DrawCurve实现滚动
		::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	}
	return TRUE;
	//return CScrollView::OnScrollBy(sizeScroll, bDoScroll);
}
//鼠标移动
void CCurveView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect(&rc);
	rc.left += CHANNEL_INFO_WIDTH;
	m_mousePoint=point;
 	if(!rc.PtInRect(point))
 	{
 		m_mousePoint.x=-1;
 	}
 	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
//	CScrollView::OnMouseMove(nFlags, point);
}
void CCurveView::SetCoorHeight(int nDigitalHeight /*= DIGITAL_HEIGHTH_COONDIATE*/,int nAnalogHeight/* = ANALOG_HEIGHTH_COONDIATE*/)
{
	m_nHeightOfDigitalCoor = nDigitalHeight;
	m_nHeightOfAnalogCoor = nAnalogHeight;
	CRect rc;
	POINT origital,tempPoint;
	Point rightPoint;
	int nWidth,nHeight=m_nHeightOfRCScale*2,i;
	origital.x=CHANNEL_INFO_WIDTH;
	origital.y=0;
	rightPoint.x=m_nMaxOfX;
	origital.y=0;
	//计算最右点的横坐标
	MapPoint(rightPoint,tempPoint,0,origital,0);
	nWidth = tempPoint.x+m_curRect.left;
	for(i=0;i<m_nCurveCount;i++)
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
//当鼠标左键按下的时候
void CCurveView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//将测量时间差的基线设为x=m_mousePoint.x,并将基线转化为坐标系的逻辑坐标
	Point pLogic;
	POINT origital;
	origital.x = m_rectCurve.left;
	origital.y = m_nHeightOfAnalogCoor/2;
	point.x += m_curRect.left;
	if(point.x>=CHANNEL_INFO_WIDTH)
	{
	MapPointToLogic(point,pLogic,0,origital,m_nHeightOfAnalogCoor/CELL_COUNT);//获取鼠标点的横坐标
	m_dValueOfClick = pLogic.x;
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	CScrollView::OnLButtonDown(nFlags, point);
	}
}
/**************************************************************************************************
函数名：GetFreqSwingAngle
描述：计算频率、幅值、相位角
参数：dFreq[out]：频率；dSwing[out]：幅值； dAngle[out]：相位角
       indexOfChannel[in]：通道的序列号  indexOfPoint[in]：  bFront[in]：
返回值：计算成功TRUE,否则FALSE
****************************************************************************************************/
//返回段点数组
BOOL CCurveView::GetFreqSwingAngle(double &dFreq, double &dSwing, double &dAngle,const int indexOfChannel, const int indexOfPoint, Point* pPoint, int count )
{
	/*CString c_infor;
	c_infor.Format(_T("%d==%d==%d"),indexOfChannel,indexOfPoint,count);
	AfxMessageBox(c_infor);*/
	int nPointInMinFreq,  //最低频率下每周波的采样点数 106
	nLowPointIndex,     //搜索坐标系中正弦周波的中零点的起始采样点序号
	nIndexOfZeroPoint;  //已搜索到的零点坐标的索引序号  为2或者1或者0
	int indexOfZero[3] = { -1, -1, -1};     //用于保存与三个零点坐标最近的采样点的序号
	BOOL bCanSec=TRUE;      //是否还可能找到合理的相位、频率、幅值
	double dPointOfX[]={-1,-1,-1};			//缓存过零点X的坐标值
	//最低频率下每周波的采样点数   106（采用最低频率的一个半周波的目的是为了满足频率是符合的值）
	nPointInMinFreq =(int)(double) 50 * m_pCurve[indexOfChannel].nPointInCircle/m_dMinFreq;
	//搜索坐标系中正弦周波的中零点的起始采样点序号
	nLowPointIndex = indexOfPoint - 3*nPointInMinFreq/2;		//当前点的索引号号减去一个半周波的采样点数	
	//已搜索到的零点坐标的索引序号  因为数组的空间大小为3，所以说最后一个的下标为2，数据从后往前加
	nIndexOfZeroPoint = 2;
	//lhz 2017-1-6 moidy 如果波形的数据前一周波和后一周波都有数据，则往前取两个过零点，往后取一个过零点,2fy不考虑
	bool isResetTargerZeroNum = FALSE;				//重置水平过零点
	//剩下的点超过一个半周波
    if ( indexOfPoint < (count - nPointInMinFreq/2) )					//如果剩下的点不足一个半周波了，那就可能需要在左边找三个过零点	
    {
		nIndexOfZeroPoint -= 1;
        isResetTargerZeroNum = TRUE;
    }
	if (indexOfChannel == 2 && 205 == indexOfPoint )
	{
		int tmp = 0;
		tmp++;
	}
	//dPointOfX:近0点X的坐标  indexOfZero：是缓存仅零点的Y的值
	//去当前采样点左边进行过零点查找，如果剩余的周波不足1个半，就让3个点都尝试在这里面找，反之最多有两个点在左边找
	FindZeroPosLeft( nIndexOfZeroPoint, indexOfChannel,  indexOfPoint, pPoint, count, indexOfZero, dPointOfX);		
	switch (nIndexOfZeroPoint)
	{
		case 2:	
			if ( !isResetTargerZeroNum )		////当剩下的波形周期不足1.5周期,且你他喵的一个左边的过零点都没有找到，还算个球，可以放弃了。
			{
				if(nLowPointIndex > 0 )			//索引值大于一个半周波
				{
					bCanSec =FALSE;				
				}
				else							//我只能说你能进来这个地方进行判断，波长有点短。就那么一丢丢那种（最低频率三个周期那种）
				{
					nIndexOfZeroPoint = 0;		//你去后面剩下不足一个半周波的地方找三个点
				}
			}
			break;
		case 1:									
			if (isResetTargerZeroNum )			//剩下的点超过一个半周期
			{
				nIndexOfZeroPoint = 0;			//你还需要找三个点，也就是你还需要从你这儿点往后的一个半周波内找到三个过零点
			}
			else								//剩余的点不足一个半周波								
			{
				//没有找到0点
				if(nLowPointIndex > 0)         //而且你前面超过一个半周波都还差两个点，你自己凉凉算了，你叫不足一个半周波的点去找两个点不干
				{
					bCanSec =FALSE;
				}
				else							//就是那种最低频率下不足三个周期的波形，然后你还需要找到两个点
				{
					dPointOfX[0] = dPointOfX[2];				
					indexOfZero[0] = indexOfZero[2];
					dPointOfX[2] = -1;
					indexOfZero[2] = -1;
					nIndexOfZeroPoint = 1;		//
				}
			}
			break;
		case 0:	
			if ( isResetTargerZeroNum )			//剩下的点超过一个半周波
			{
				nIndexOfZeroPoint = 1;						//你还需要找2个点
				indexOfZero[0] = indexOfZero[1];
				indexOfZero[1] = -1;
				dPointOfX[0] = dPointOfX[1];
			}
			else						//未传入的点不足一个半周波
			{
				if(nLowPointIndex > 0)	//前面一个半周波都没找找到合适的点，现在剩下的点不足一个半周波你还想什么
				{
					bCanSec =FALSE;
				}
				else					//又是那种短短的吗？
				{
					dPointOfX[0] = dPointOfX[1];				//把找到的两个点放前面
					indexOfZero[0] = indexOfZero[1];
					dPointOfX[1] = dPointOfX[2];
					indexOfZero[1] = indexOfZero[2];
					dPointOfX[2] = -1;
					indexOfZero[2] = -1;
					nIndexOfZeroPoint = 2;						//剩下的点不足一个半周期你还的找一个点
				}
			}
			break;
		default:	
			ASSERT(nIndexOfZeroPoint == -1);	//已经在左边找到了应该找到的点数：剩下的点不足一个半周期就找3个点，超过一个半周期就找2个点
			if ( isResetTargerZeroNum )			//左边找三个点
			{
				nIndexOfZeroPoint = 2;
			}
			else
			{
				nIndexOfZeroPoint = 3;			//左边找两个点
			}
			break;
	}
	//左边找完了，但是还没有找到三个点时候需要从参照点右边找
	if(nIndexOfZeroPoint < 3 && bCanSec)					
	{
		//去右边找零点
		bCanSec = FindZeroPosRight( nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count, indexOfZero, dPointOfX);
		if ( FALSE == bCanSec )
		{//如果往后的点找不到，则全部过零点，往周波前找  重新寻找3个过零点
			nIndexOfZeroPoint = 2;				//既然从左边找两个点凑不够3哥过零点，那么就尝试从左边找三个
			bCanSec = FindZeroPosLeft( nIndexOfZeroPoint, indexOfChannel,  indexOfPoint, pPoint, count,indexOfZero, dPointOfX);
		}
	}
	if( bCanSec )
	{//计算
		bCanSec = CaluateAngle( dFreq, dSwing, dAngle, indexOfChannel,indexOfPoint, pPoint, count, indexOfZero, dPointOfX);
	}
	//频率在合理范围内
	if(bCanSec && dFreq < m_dMaxFreq && dFreq > m_dMinFreq)
	{
		/*lhz moidy 由于频率切换过程中，会出现正半轴是一个频率，负半轴是一个频率，即波形不对称，则求出来的角度不准确*/
		BOOL isErrorAng = (DoubleCmp(pPoint[indexOfPoint].y) < 0 && DoubleCmp( dAngle - 179.5 ) < 0 )|| (DoubleCmp(pPoint[indexOfPoint].y) > 0 && DoubleCmp( dAngle - 180.5 ) > 0); 
		double diffTime = (dPointOfX[1] - dPointOfX[0]) - ( dPointOfX[2] - dPointOfX[1] );
        diffTime = fabs( diffTime );
		BOOL isNoCosWave = DoubleCmp( diffTime - 0.1 ) > 0 ; /*>0.1 ms 不是一个对称的正炫波*/
		//以下我记得不被调用
		if ( indexOfChannel == 1 && indexOfPoint == 1708)
		{
			CString tempStr;
			tempStr.Format( _T(" x = %0.3f diffTime = %0.3f \r\n"), pPoint[indexOfPoint].x, diffTime ); 
		//	TRACE( tempStr );
		}
		if ( isErrorAng || isNoCosWave)
		{/*采样值在负半轴，求的角度在<180,说明求出来的角度不准确，需切换为同频率周期计算，现做简易程序修改直接换成180*/
			if ( isNoCosWave )
			{
				dAngle = dAngle;
			}
			dAngle = isErrorAng ? 180.0 : dAngle;
			//同频率周期计算
			CaluateAngleByFreqA( dFreq, dAngle, indexOfChannel, indexOfPoint, pPoint, count,indexOfZero, dPointOfX);
		} 
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//这段代码都没有被使用
BOOL CCurveView::CaluateAngleByHalfWave( double &dFreq,  double &dAngle, const int indexOfChannel,
							  const int indexOfPoint, Point* pPoint, int count, int *indexOfZero, double *dPointOfX)
{
    double dZeroAngle = 0.0, dTime, cycle ;
    if ( indexOfPoint <= indexOfZero[2] && indexOfPoint >= indexOfZero[1] )
    {
		cycle = (dPointOfX[2] - dPointOfX[1])*2;
		dZeroAngle = dPointOfX[1];
		dTime = pPoint[indexOfPoint].x - dZeroAngle;
    }else if ( indexOfPoint <= indexOfZero[1] && indexOfPoint >= indexOfZero[0] )
    {
		cycle = (dPointOfX[1] - dPointOfX[0])*2;
		dZeroAngle = dPointOfX[0];
		dTime = pPoint[indexOfPoint].x - dZeroAngle;
    }else
	{//重新查找0值位置
       int nPointInMinFreq =(int)(double) 50 * m_pCurve[indexOfChannel].nPointInCircle/m_dMinFreq;
	   int j = 0;
       int num = indexOfPoint - nPointInMinFreq/2;
       if ( num < 0 )
       {
		   return FALSE;
       }

       //向左边找一个点
	   int nIndexOfZeroPoint = 0;
	   BOOL isFind = FindZeroPosLeft(nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count, indexOfZero, dPointOfX );
       if ( FALSE == isFind )
       {
           return FALSE;
       }
	   nIndexOfZeroPoint = 1;
	   //向右边找一个过零
	    num = indexOfPoint + nPointInMinFreq/2;
	   for( j = indexOfPoint+2; j < num && j < count; j++ )
	   {
		   if(DoubleCmp(pPoint[j].y) != DoubleCmp(pPoint[j-1].y))
		   {
			   //线性插值过程
			   switch(DoubleCmp(pPoint[j-1].y))
			   {
			   case -1:
				   dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + 
					   (pPoint[j].x - pPoint[j-1].x)*
					   fabs(pPoint[j-1].y)/(pPoint[j].y-
					   pPoint[j-1].y);
				   break;
			   case 1:
				   dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + 
					   (pPoint[j].x - pPoint[j-1].x)*
					   pPoint[j-1].y/(fabs(pPoint[j].y)+
					   pPoint[j-1].y);
				   break;
			   default:
				   dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x;
				   break;
			   }
			   indexOfZero[nIndexOfZeroPoint] = j-1;
			   j -=MIN_NUM_INTERVAL_OF_ZERO;  //为了避免在距零点很近的地方找到第二个零点，这样的零头找到也是无效的
			   nIndexOfZeroPoint--;
			   break;
		   }

	   }
	   if ( 1 == nIndexOfZeroPoint )
	   {//未找到
		   return FALSE;
	   }
	   //
	   cycle = (dPointOfX[1] - dPointOfX[0])*2;
	   dZeroAngle = dPointOfX[0];
	   dTime = pPoint[indexOfPoint].x - dZeroAngle;
	}
    dFreq = 1000/cycle;
	if ( DoubleCmp(pPoint[indexOfPoint].y) > 0 )
	{//则0°在 indexOfZero[1]
		dAngle = dTime*360/cycle;
	}else if ( DoubleCmp(pPoint[indexOfPoint].y) < 0 )
	{
		dAngle = 180.0 + dTime*360/cycle;
	}else
	{
		dAngle =  DoubleCmp(pPoint[indexOfPoint+1].y) < 0 ? 180.0: 0;
	}
	return TRUE;
}
/*根据频率加速度计算角度*/
BOOL CCurveView::CaluateAngleByFreqA( double &dFreq,  double &dAngle, const int indexOfChannel,
										const int indexOfPoint, Point* pPoint, int count, int *indexOfZero, double *dPointOfX)
{
	 long fh = DoubleCmp(pPoint[indexOfPoint].y );
	if (  DoubleCmp( fh ) == 0  )
	{
		 dAngle =  DoubleCmp(pPoint[indexOfPoint+1].y) < 0 ? 180.0: 0;
		 return TRUE;
	}
	if ( indexOfPoint <= indexOfZero[2] && indexOfPoint >= indexOfZero[1])
	{
		int zeroPos = indexOfZero[0];
		double difYValue = pPoint[zeroPos+MIN_NUM_INTERVAL_OF_ZERO].y - 
			pPoint[zeroPos].y;
		if ( DoubleCmp( difYValue ) < 0 )
		{//属于递减，则第1个过零点为180°。3个点包含的周波不是正周期，第0个的过零点必须为0°角。
            int tmp = indexOfZero[0];
            if ( DoubleCmp( pPoint[indexOfPoint].y ) < 0 )
            {//则需从左边找一个角度为0°的过零点
				indexOfZero[2] = indexOfZero[1];                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               			indexOfZero[2] = indexOfZero[1];
				indexOfZero[1] = indexOfZero[0];

				dPointOfX[2] = dPointOfX[1];
				dPointOfX[1] = dPointOfX[0];

				int nIndexOfZeroPoint = 0; 
				BOOL isFind = FindZeroPosLeft( nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count,
					indexOfZero, dPointOfX);
				if ( FALSE == isFind )
				{
					return FALSE;
				}
            }else if( DoubleCmp( pPoint[indexOfPoint].y ) > 0  )
			{//第2个过零点为0°，再往右边再找一个点
				indexOfZero[0] = indexOfZero[1];
				indexOfZero[1] = indexOfZero[2];                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               			indexOfZero[2] = indexOfZero[1];
				
				dPointOfX[0] = dPointOfX[1];
				dPointOfX[1] = dPointOfX[2];
				int nIndexOfZeroPoint = 2; 
				int findBegin = indexOfZero[1] + 1;
				BOOL isFind = FindZeroPosRight( nIndexOfZeroPoint, indexOfChannel, findBegin, pPoint, count, 
					indexOfZero, dPointOfX);
				if ( FALSE == isFind )
				{
					return FALSE;
				}                
			}
		}
	}else
	{ //当前点的正周期一周波
	  
       if (   fh > 0 )
       {//在正半轴,在左边找一个，在右边找两个
		   int nIndexOfZeroPoint = 0; 
		   BOOL isFind = FindZeroPosLeft( nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count, 
			   indexOfZero, dPointOfX);
		   if ( FALSE == isFind )
		   {
			   return FALSE;
		   }
		   nIndexOfZeroPoint = 1;
		   isFind = FindZeroPosRight( nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count, 
			   indexOfZero, dPointOfX);
		   if ( FALSE == isFind )
		   {
			   return FALSE;
		   }
       }else if ( fh < 0)
       {//在负半轴
		   int nIndexOfZeroPoint = 1; 
		   BOOL isFind = FindZeroPosLeft( nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count, 
			   indexOfZero, dPointOfX);
		   if ( FALSE == isFind )
		   {
			   return FALSE;
		   }
		   nIndexOfZeroPoint = 2;
		   isFind = FindZeroPosRight( nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count, 
			   indexOfZero, dPointOfX);
		   if ( FALSE == isFind )
		   {
			   return FALSE;
		   }
       }
	}
	//
	double T1 = 0.0,T2 = 0.0, T;
	T1 = dPointOfX[1] - dPointOfX[0];
	T2 = dPointOfX[2] - dPointOfX[1];
	double f0 = 1.0;
	double a = 1.0;

    f0 = ((T2*T2 - T1*T1 + 2*T1*T2)*1000)/(2*T1*T2*(T1+T2)); //*1000转换为S的单位，才为hz
   
	a = (T1-T2)*1000/(T1*T2*(T1+T2)); //*1000转换为S的单位
	T = pPoint[indexOfPoint].x - dPointOfX[0];
	double orgAng ;
	orgAng = dAngle;
	dAngle = (360.0*f0*T)/1000 + (180*a*T*T)/1000; //除1000将ms转换为S
    /*
    if ( DoubleCmp( dAngle - 360 ) > 0 )
    {
		int mulit = dAngle/360;
		if ( fh > 0 )
		{
		    dAngle -= mulit*360;
		}else
		{
			dAngle = 360.0;
		}
    }
	*/ 
	dFreq = f0 + a*T/2;
	return TRUE;
}
//还是三个参数，通道的编号，传入的是第几个点，结构体，总的点数有多少，近零点数组，另外一个数组（他们两个都是3个空间的）
BOOL CCurveView::CaluateAngle( double &dFreq, double &dSwing, double &dAngle, const int indexOfChannel,
							  const int indexOfPoint, Point* pPoint, int count, int *indexOfZero, double *dPointOfX)
{
	BOOL bCanSec = TRUE;
	double dmax[2],         //dmax表示一个周波中的两个最值的y值
		cycle,              //周期长度，单位ms
		dZeroAngle,         //零相位角在逻辑坐标系中的x值
		dTime;              //当前要计算的采样点坐标与零相位角之间的时间差
	dFreq = 1000/(dPointOfX[2]-dPointOfX[0]);			//通过过零点计算的频率			//过零点的X的距离差值
	//不可能执行的
	if(DoubleCmp(dFreq-m_dMinFreq)<0 && DoubleCmp(m_dMaxFreq - dFreq)<0 )
	{
		bCanSec =FALSE;
	}
	else
	{
		//两个极值的算法
		dmax[0] = pPoint[(indexOfZero[0]+indexOfZero[1])>>1].y;				//过零点的索引值的
		dmax[1] = pPoint[(indexOfZero[1]+indexOfZero[2])>>1].y;
		//振幅
		dSwing = max(fabs(dmax[0]),fabs(dmax[1]));
		//如果两个极值都不是近零的话
		if((DoubleCmp(dmax[0])+DoubleCmp(dmax[1])) != 0)	//如果两个极值不是一正一负就凉凉
		{
			return FALSE;
		}
		//如果第一个极值是正数不近0的话
		if(DoubleCmp(dmax[0]) > 0)			//如果前面两个过零点在上半轴
		{
			//过零点的的X值
			dZeroAngle = dPointOfX[0];
		}
		else								
		{
			//过零点的的X值
			dZeroAngle = dPointOfX[1]; 
		}
		//周期长度，单位ms
		cycle = dPointOfX[2] - dPointOfX[0];					//一个周期的X轴距离
		//当前要计算的采样点坐标与零相位角之间的时间差
		dTime = pPoint[indexOfPoint].x - dZeroAngle;			//
		//当前位置距离零相位点1个周期以内的时候
		if(DoubleCmp(cycle - fabs(dTime)) >= 0)					//采样点在三个过零点里
		{
			if(DoubleCmp(dTime)>=0)
			{
				dAngle = dTime*360/cycle;
			}
			else
			{
				dAngle = (cycle+dTime)*360/cycle;
			}
		}
		else	
		{
			if((DoubleCmp(dTime)>=0) && (DoubleCmp(dTime - cycle*2) < 0))
			{
				dAngle = (dTime-cycle)*360/cycle;
			}
		}
	}
    return bCanSec;
}
//找左边的过零点
BOOL CCurveView::FindZeroPosLeft( int  &nIndexOfZeroPoint, const int indexOfChannel, const int indexOfPoint, Point* pPoint, int count,
								  int *indexOfZero,  double *dPointOfX)
{
	BOOL bCanSec = TRUE;
	//如果在当前计算点之点找出的零点不足三点时，需要在当前点之后找，以补足三个点   找0点坐标值
	if(nIndexOfZeroPoint < 3 && nIndexOfZeroPoint>= 0)
	{
		int j = 0, nLowPointIndex;
		//106
		int nPointInMinFreq =(int)(double) 50 * m_pCurve[indexOfChannel].nPointInCircle/m_dMinFreq;
		//159
		nPointInMinFreq = nPointInMinFreq*3/2;
		//如果传入的点是大于等于159   这个步骤就是最多留下一个周波的点出来 
		if(indexOfPoint >= nPointInMinFreq)
		{
			nLowPointIndex = indexOfPoint -nPointInMinFreq;
		}
		//如果这个点是第160个点以内的点的话
		else
		{
			nLowPointIndex = 0;
		}
		//j等于传进来点的值如果
		//这些步骤的结果就是使得这里执行的次数0到159
		for(j=indexOfPoint;j>nLowPointIndex;j--)
		{	
			//判断前后两个的位置的趋势是不是一样
			if(DoubleCmp(pPoint[j].y) != DoubleCmp(pPoint[j-1].y))
			{
				//线性插值过程    dPointOfX是具有3个空间的数组
				switch(DoubleCmp(pPoint[j-1].y))
				{
					//当角度小于0的时候
					case -1:		//从上往下
						dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + (pPoint[j].x - pPoint[j-1].x)*fabs(pPoint[j-1].y)/(pPoint[j].y-pPoint[j-1].y);
						break;
					//当角度大于0时
					case 1:
						dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + (pPoint[j].x - pPoint[j-1].x)*pPoint[j-1].y/(fabs(pPoint[j].y)+pPoint[j-1].y);
						break;
						//接近零时
					default:
						dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x;
						break;
				}
				indexOfZero[nIndexOfZeroPoint] = j-1;
				//4
				j -=MIN_NUM_INTERVAL_OF_ZERO;  //为了避免在距零点很近的地方找到第二个零点，这样的零头找到也是无效的
				nIndexOfZeroPoint--;
				if(nIndexOfZeroPoint<0)			//每找到一个过零点就减一，在合理的范围内找到几个是几个
				{
					break;
				}
			}
		}
	}
	bCanSec = nIndexOfZeroPoint < 0;
	return bCanSec;
}
//找右边
BOOL CCurveView::FindZeroPosRight( int  &nIndexOfZeroPoint, const int indexOfChannel, const int indexOfPoint, Point* pPoint, int count,
								  int *indexOfZero, double *dPointOfX)
{
	BOOL bCanSec = TRUE;
	//如果在当前计算点之点找出的零点不足三点时，需要在当前点之后找，以补足三个点
	if(nIndexOfZeroPoint < 3 )
	{
		int nPointInMinFreq =(int)(double) 50 * m_pCurve[indexOfChannel].nPointInCircle/m_dMinFreq;
		int nHighPointIndex = indexOfPoint + nPointInMinFreq;
		nHighPointIndex += nPointInMinFreq/2;/*lhz 2017-1-11 moidy，由于在过0点附近的点往后找，需找1.5个周波才能找到，因此需再加半个周波的点数据*/
		if(indexOfPoint + nPointInMinFreq/2 > count)
		{
			return FALSE;
		}
		int j=indexOfPoint+1;
		if(nIndexOfZeroPoint>0 && abs(indexOfZero[nIndexOfZeroPoint-1] - j) < 4)
		{
			j += MIN_NUM_INTERVAL_OF_ZERO; //为了避免在距零点很近的地方找到第二个零点，这样的零头找到也是无效
		}
		for(;j<nHighPointIndex;j++)
		{
			if(DoubleCmp(pPoint[j].y) != DoubleCmp(pPoint[j-1].y))
			{
				switch(DoubleCmp(pPoint[j].y))
				{
					case 1:
						dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + (pPoint[j].x -pPoint[j-1].x)*fabs(pPoint[j-1].y)/(pPoint[j].y-pPoint[j-1].y);
						break;
					case -1:
						dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + (pPoint[j].x - pPoint[j-1].x)*pPoint[j-1].y/(fabs(pPoint[j].y)+pPoint[j-1].y);
						break;
					default:
						dPointOfX[nIndexOfZeroPoint] = pPoint[j].x;
						break;
				}
				indexOfZero[nIndexOfZeroPoint] = j;
				j+=MIN_NUM_INTERVAL_OF_ZERO;      //为了避免在距零点很近的地方找到第二个零点，这样的零头找到也是无效的
				nIndexOfZeroPoint++;
				if(nIndexOfZeroPoint>2) 
				{
					break;
				}
			}
		}
        if(nIndexOfZeroPoint <= 2) //前后都找了一遍，不足两个零点，所以此曲线不是正弦波
		{
			bCanSec =FALSE;
		}
	}
	return bCanSec;
}
void CCurveView::DrawScale(CDC *pDC , bool isLabel , int dota)
{
	POINT origitalPoint,pTemp[6],pStartTime;
	Point pMinLogPoint,pMaxLogPoint;
	origitalPoint.x=m_rectCurve.left;
	pMinLogPoint.x = m_pCurve[2].startTime;
	MapPoint(pMinLogPoint,pStartTime,0,origitalPoint,0);
	pTemp[0].x=m_curRect.left+CHANNEL_INFO_WIDTH;
	MapPointToLogic(pTemp[0],pMinLogPoint,0,origitalPoint,0);
	pTemp[0].x=m_curRect.right;
	MapPointToLogic(pTemp[0],pMaxLogPoint,0,origitalPoint,0);
	int nMinLogX=pMinLogPoint.x-m_pCurve[2].startTime,nMaxLogX=pMaxLogPoint.x-m_pCurve[2].startTime,nLogScale;
	double dLogScale;
	//保证每个逻辑刻度之间的显示横坐标差大于5个像素
	dLogScale = (pMaxLogPoint.x - pMinLogPoint.x)/((double)(m_curRect.Width()-CHANNEL_INFO_WIDTH)/m_nMinWidthOfDisScale);
	nLogScale = dLogScale;
	nLogScale = (DoubleCmp(dLogScale - nLogScale) == 0) ?nLogScale:(nLogScale+1);
	if(nLogScale < m_nMinWidthOfLogScale)
	{
		nLogScale = m_nMinWidthOfLogScale;
	}
	CRect rcTopScale(0,m_curRect.top,m_curRect.Width(),m_curRect.top+m_nHeightOfRCScale)
		,rcBottonScale;
	if(m_curRect.bottom <= (m_rectCurve.bottom+m_nHeightOfRCScale))
	{
		rcBottonScale = CRect(0,m_curRect.bottom-m_nHeightOfRCScale,m_curRect.Width(),m_curRect.bottom);
	}
	else
	{
		rcBottonScale = CRect(0,m_rectCurve.bottom,m_curRect.Width(),m_rectCurve.bottom+m_nHeightOfRCScale);
	}
	pDC->FillSolidRect(rcTopScale,RGB(255,255,255));
	pDC->FillSolidRect(rcBottonScale,RGB(255,255,255));
	rcTopScale.left+=CHANNEL_INFO_WIDTH;
	rcBottonScale.left += CHANNEL_INFO_WIDTH;
	int i,nScaleBegin,nScaleEnd,nDisScalePos;
	nScaleBegin = nMinLogX%nLogScale ? nMinLogX/nLogScale+1 : nMinLogX/nLogScale;
	nScaleEnd = nMaxLogX%nLogScale ? nMaxLogX/nLogScale+1 : nMaxLogX/nLogScale;
	int temp1,temp2,temp3;
	if(isLabel)
	{
		temp1 = 300%nLogScale ? 300/nLogScale+1 : 300/nLogScale;
		temp2 = 1300%nLogScale ? 1300/nLogScale+1 : 1300/nLogScale;
		temp3 =	(dota-1000)%nLogScale ? (dota-1000)/nLogScale+1 : (dota-1000)/nLogScale;
	}
	double dLogRange = pMaxLogPoint.x-pMinLogPoint.x;
	double dDisRange = m_curRect.Width()-CHANNEL_INFO_WIDTH;
	pTemp[0].y = rcTopScale.bottom;
	pTemp[1].y = rcTopScale.bottom-3;
	pTemp[2].y = rcTopScale.bottom-6;
	pTemp[3].y = rcBottonScale.top;
	pTemp[4].y = rcBottonScale.top+3;
	pTemp[5].y = rcBottonScale.top+6;
	CPen *pOldPen,pen(PS_SOLID,1,RGB(0,0,0)),penGird(PS_DOT,1,m_colorGirdLine);
	CFont font,*pOldFont;
	font.CreatePointFont(80,_T("MS Sans Serif"));
	pOldFont=pDC->SelectObject(&font);
	pOldPen = pDC->SelectObject(&pen);
	int nOldBKMode = pDC->SetBkMode(TRANSPARENT);
	BOOL bNotTimesOf5;
	bool bDrawGirdLine = ((5*nLogScale-pMinLogPoint.x)/dLogRange*dDisRange > 50);
	pDC->MoveTo(rcTopScale.left,rcTopScale.bottom);
	pDC->LineTo(rcTopScale.right,rcTopScale.bottom);
	pDC->MoveTo(rcBottonScale.left,rcBottonScale.top);
	pDC->LineTo(rcBottonScale.right,rcBottonScale.top);
	CRect textRc;
	CString str, info;
	textRc = CRect(20,rcTopScale.top,CHANNEL_INFO_WIDTH,rcTopScale.bottom);
	info.LoadString( IDS_STR_TIME );
	str.Format(_T("%s[ms]  %s"), info,m_strDota);
	pDC->DrawText(str,&textRc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	textRc.top = rcBottonScale.top;
	textRc.bottom = rcBottonScale.bottom;
	pDC->DrawText(str,&textRc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	for(i=nScaleBegin;i<=nScaleEnd;i++)
	{
		if(m_isLabel&&i>=temp1&&i<temp2)
			continue;
		bNotTimesOf5 = (i%5);
		nDisScalePos = (i*nLogScale+m_pCurve[2].startTime-pMinLogPoint.x)/dLogRange*dDisRange+CHANNEL_INFO_WIDTH;
		if(nDisScalePos < CHANNEL_INFO_WIDTH) continue;
		pTemp[0].x = pTemp[1].x = pTemp[2].x = pTemp[3].x = pTemp[4].x = pTemp[5].x = nDisScalePos; 
		pDC->MoveTo(pTemp[0]);
		bNotTimesOf5 ? pDC->LineTo(pTemp[1]) : pDC->LineTo(pTemp[2]);
		pDC->MoveTo(pTemp[3]);
		bNotTimesOf5 ? pDC->LineTo(pTemp[4]) : pDC->LineTo(pTemp[5]);
		if(i % 10 == 0)
		{
			textRc = CRect(nDisScalePos-30,rcTopScale.top,nDisScalePos+30,pTemp[2].y);
			if(m_isLabel&&i>=temp2)
			{
				str.Format(_T("%d"),(int)((temp3+i)*nLogScale));
			}
			else
			{
				str.Format(_T("%d"),(int)(i*nLogScale));
			}
			pDC->DrawText(str,&textRc,DT_CENTER);
			textRc = CRect(nDisScalePos-30,pTemp[5].y,nDisScalePos+30,rcBottonScale.bottom);
			pDC->DrawText(str,&textRc,DT_CENTER);
			
			if(nDisScalePos == CHANNEL_INFO_WIDTH || (!i)) continue;
			pDC->SelectObject(&penGird);
			pDC->MoveTo(pTemp[0]);
			pDC->LineTo(pTemp[3]);
			pDC->SelectObject(&pen); 
		}
		else if((!bNotTimesOf5) && bDrawGirdLine && (nDisScalePos != CHANNEL_INFO_WIDTH))
		{
			pDC->SelectObject(&penGird);
			pDC->MoveTo(pTemp[0]);
			pDC->LineTo(pTemp[3]);
			pDC->SelectObject(&pen);
		}
	}
	pDC->SetBkMode(nOldBKMode);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
}

void CCurveView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CPoint pTemp = point;
	CMenu PopupMenu;
	PopupMenu.CreatePopupMenu();
	CString info;
	info.LoadString( IDS_CURVE_SET );
	PopupMenu.AppendMenu(MF_ENABLED, IDM_CURVE_DIS_SET, info);
	info.LoadString( IDS_DIS_COLOUR );
	PopupMenu.AppendMenu( MF_ENABLED, IDM_DYEING, info );
	info.LoadString( IDS_DIS_BACKWRITHE );
	PopupMenu.AppendMenu(MF_ENABLED, IDM_ACHROMATIC, info );
    info.LoadString( IDS_ONLY_SHOW_EFF );
	PopupMenu.AppendMenu(MF_ENABLED, IDM_SHOW_VALID, info );
	info.LoadString( IDS_ONLY_SHOW_SAMPLE );
	PopupMenu.AppendMenu(MF_ENABLED, IDM_SHOW_SAMPLE, info );
	info.LoadString( IDS_SHOW_ALL );
	PopupMenu.AppendMenu(MF_ENABLED, IDM_SHOW_ALL, info );
	info.LoadString( IDS_EXPORT_WAVEDATA );
	PopupMenu.AppendMenu(MF_ENABLED, IDM_EXPORT_WARE_TEXT, info ); //2016-5-5 add export


	ClientToScreen(&pTemp);
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON , pTemp.x, pTemp.y, this);
	CScrollView::OnRButtonDown(nFlags, point);
}

void CCurveView::OnDyeing()
{
	m_bMultiColorDis = TRUE;
	m_bkColor=RGB(255,255,255);
	m_crossLineColor=RGB(0,0,255);
	m_coordinateColor=RGB(0,0,255);
	m_colorArray[0]=RGB(0,0,0);
	m_colorArray[1]=RGB(0,255,255);
	m_colorArray[2]=RGB(255,0,255);
	m_colorArray[3]=RGB(128,0,0);
	m_colorArray[4]=RGB(128,128,0);
	m_colorArray[5]=RGB(0,128,0);
	m_colorArray[6]=RGB(128,128,255);
	m_colorArray[7]=RGB(0,128,128);
	m_colorArray[8]=RGB(128,0,128);
	m_colorArray[9]=RGB(0,255,128);
	m_colorArray[10]=RGB(0,128,255);
    m_colorArray[11]=RGB(0,0,255);
	int i;
	for(i=0;i<m_nCurveCount;i++)
	{
		m_pCurve[i].color = m_colorArray[i%COLOR_NUM];
	}
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);

}

void CCurveView::OnShowValid()
{
	m_flagShow = VALID_VALUE_SHOW;
}
void CCurveView::OnShowSample()
{
	m_flagShow = SAMPLE_VALUE_SHOW;
}
void CCurveView::OnShowAll()
{
	m_flagShow = ALL_VALUE_SHOW;
}
void CCurveView::OnAchromatic()
{
	m_bMultiColorDis = FALSE;
	m_bkColor=RGB(255,255,255);
	m_crossLineColor=RGB(100,100,100);
	m_coordinateColor=RGB(100,100,100);
	m_colorStartLine = RGB(100,100,100);
	int i;
	for(i=0;i<m_nCurveCount;i++)
	{
		m_pCurve[i].color = RGB(100,100,100);
	}
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);	
}

void CCurveView::OnCurveDisSet()
{
	((CMainFrame *)GetParentFrame())->CurveDisSet();
	m_curvePrintInfo.bPrintSet = FALSE;
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
}

void CCurveView::OnFilePrint() 
{
	// TODO: Add your command handler code here
	if(!m_curvePrintInfo.bPrintSet)
	{
		CString info;
		info.LoadString(IDS_PRINT_INFO);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	CMyView::OnFilePrint();
}

void CCurveView::OnFilePrintPreview() 
{
	// TODO: Add your command handler code here
	if(!m_curvePrintInfo.bPrintSet)
	{
		CString info;
		info.LoadString(IDS_PRINT_INFO);
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		return;
	}
	CMyView::OnFilePrintPreview();
}

BOOL CCurveView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	memset(m_arrPageHeadInfo,0,sizeof(m_arrPageHeadInfo));
	PRINTDLG pd; //该结构包含打印对话框中的所有信息 
	LPDEVMODE lpDevMode; 
	
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd)) //获得默认的打印机的信息 
	{ 
		lpDevMode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpDevMode) 
		{ 
			lpDevMode->dmPaperSize=DMPAPER_A4; //将打印纸设置为A4 
			if(m_curvePrintInfo.bHPrint)
			{
				lpDevMode->dmOrientation=DMORIENT_LANDSCAPE; //将打印机设置为横向打印 
				
			}
			else
			{
				lpDevMode->dmOrientation= DMORIENT_PORTRAIT; //横向打印
			}
		} 
		GlobalUnlock(pd.hDevMode); 
	}
	
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	int nRowNum = 2,i,nCurveRow = m_curvePrintInfo.bAnalyse ? ANALYSE_DIS_LINE : CURVE_DIS_LINE;
	int nRowInPage = m_curvePrintInfo.bHPrint ? A4_HLINE : A4_VLINE;
	int nPageCount=0,nTemp;
	m_arrPageHeadInfo[nPageCount] = PRINT_TYPE_FILE <<16;
	if(m_curvePrintInfo.bPrintFileInfo)
	{
		nRowNum += 4;
	}
	nRowNum+=3;//一个标题两个刻度线
	for(i = 0;i < pMainFrm->m_wareFileData.m_nCurveCount;i++)
	{
		if(i == 0 && m_curvePrintInfo.bAnalyse) continue; //波形分析时，第一条曲线要与第二条曲线绘在同一坐标系中
		if(pMainFrm->m_wareFileData.m_pCurve[i].bIsDigital)
		{
			nRowNum += DIGITAL_DIS_LINE;
			if(nRowNum > nRowInPage)
			{
				nPageCount++;
				m_arrPageHeadInfo[nPageCount] = (PRINT_TYPE_CURVE<<16);
				m_arrPageHeadInfo[nPageCount] += m_curvePrintInfo.bAnalyse ? i+1 : i;
				nRowNum = DIGITAL_DIS_LINE+2;
			}
			else if(nRowNum == nRowInPage)
			{
				nPageCount++;
				m_arrPageHeadInfo[nPageCount] = (PRINT_TYPE_CURVE<<16);
				m_arrPageHeadInfo[nPageCount] += m_curvePrintInfo.bAnalyse ? i+2 : i+1;
				nRowNum = 2;
			}
		}
		else
		{
			nRowNum += nCurveRow;
			if(nRowNum > nRowInPage)
			{
				nPageCount++;
				m_arrPageHeadInfo[nPageCount] = (PRINT_TYPE_CURVE<<16);
				m_arrPageHeadInfo[nPageCount] += m_curvePrintInfo.bAnalyse ? i+1 : i;
				nRowNum = nCurveRow+2;
			}
			else if(nRowNum == nRowInPage)
			{
				nPageCount++;
				m_arrPageHeadInfo[nPageCount] = (PRINT_TYPE_CURVE<<16);
				m_arrPageHeadInfo[nPageCount] += m_curvePrintInfo.bAnalyse ? i+2 : i+1;
				nRowNum = 2;
			}
		}
	}

	if(nRowNum == 2) //表示上一页刚好把曲线绘完
	{
		nRowNum = 0;
	}
	if(m_curvePrintInfo.bPrintChannelInfo)
	{
		
		if(nRowInPage - nRowNum < 3)
		{
			nPageCount++;
			m_arrPageHeadInfo[nPageCount] = PRINT_TYPE_CHANNEL<<16;
			nRowNum = 0;
		}
		nRowNum+=2;
		nTemp = 0;
		while(nRowNum+pMainFrm->m_wareFileData.m_nCurveCount-nTemp > nRowInPage)
		{
			nPageCount++;
			nTemp += (nRowInPage-nRowNum);
			m_arrPageHeadInfo[nPageCount] = (PRINT_TYPE_CHANNEL<<16) +nTemp;
			nRowNum = 1;
		}
		nRowNum += pMainFrm->m_wareFileData.m_nCurveCount-nTemp;
		
	}

	
	//打印定值的页面规化
	
	if(m_curvePrintInfo.bPrintParaInfo)
	{
		if(nRowInPage - nRowNum < 3)
		{
			nPageCount++;
			m_arrPageHeadInfo[nPageCount] = PRINT_TYPE_PARA<<16;
			nRowNum = 0;
		}
		nRowNum+=2;
		nTemp = 0;
		while(nRowNum+pMainFrm->m_listParaInfo.GetItemCount()-nTemp > nRowInPage)
		{
			nPageCount++;
			nTemp += (nRowInPage-nRowNum);
			m_arrPageHeadInfo[nPageCount] = (PRINT_TYPE_PARA<<16) +nTemp;
			nRowNum = 1;
		}
		nRowNum+=pMainFrm->m_listParaInfo.GetItemCount()-nTemp;
	}
	if(nRowNum) nPageCount++;
	pInfo->SetMaxPage(nPageCount);
	return DoPreparePrinting(pInfo);
}

void CCurveView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bPrint = FALSE;
	
	//	CWareFileData *pWareFileData = &((CMainFrame *)GetParentFrame())->m_wareFileData;
	m_isFirstShow = FALSE;
	ChangeCurve(m_pOldCurve,m_nOldCurveCount,m_isUnitedFile,m_diffMilli,m_cOldPS);
	CCurveView::Release(m_pOldCurve,m_nOldCurveCount);
// 	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
// 	pMainFrm->ShowCurve();
	if(!m_curvePrintInfo.bMultiColor)
	{
		OnDyeing();
	}
	m_rcPrintCurve.top = 100;//设置m_rcPrintCurve为无效矩形
}

void CCurveView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bPrint = TRUE;
	m_rcPrintCurve.top = 100;//设置m_rcPrintCurve为无效矩形
	m_pOldCurve = m_pCurve;
	m_pCurve = NULL;
	m_nOldCurveCount = m_nCurveCount;
	m_cOldPS = m_strValueType == _T("二次值") ? 's' : 'p';
	pInfo->m_nNumPreviewPages = 1;
}

void CCurveView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	// TODO: Add your specialized code here and/or call the base class
	int        i, nRowI=0,nRowInPage = m_curvePrintInfo.bHPrint ? A4_HLINE:A4_VLINE;
	int        nPrintType,nPrintNum,nRowTemp,nParaItemCount;
	CString    str;
	CFont      font;
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	nParaItemCount = pMainFrm->m_listParaInfo.GetItemCount();
	CWareFileData *pWareFileData = &pMainFrm->m_wareFileData;
	CRect rcText(A4_H_FRAME,0-A4_HEAD_INTERVAL,(A4_W - A4_H_FRAME),0 - (A4_HEAD_INTERVAL+ROW_HEIGHT)),rcTemp;
	if(m_curvePrintInfo.bHPrint)
	{
		rcText = CRect(A4_H_FRAME,0-A4_HEAD_INTERVAL,(A4_H - A4_H_FRAME),0 - (A4_HEAD_INTERVAL+ROW_HEIGHT));
	}
	pDC->SetMapMode(MM_HIMETRIC);
	

	// 14-point fixed-pitch font
	font.CreateFont(-350,
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	// Courier New is a TrueType font
	CFont* pOldFont = (CFont*) (pDC->SelectObject(&font));
	nPrintType = m_arrPageHeadInfo[pInfo->m_nCurPage-1]>>16;
	nPrintNum = 0x0000FFFF & m_arrPageHeadInfo[pInfo->m_nCurPage-1];
	CString info;
	switch (nPrintType)
	{
	case PRINT_TYPE_FILE:
		nRowI = 2;
		info.LoadString( IDS_STR_FILENAME );
		str.Format(_T("%s:%s"), info, pWareFileData->m_strFileName);
		pDC->DrawText(str,&rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rcText.top -= ROW_HEIGHT;
		rcText.bottom -= ROW_HEIGHT;
		info.LoadString( IDS_STR_FILESRC );
		str.Format(_T("%s:%s"), info, (CString)pWareFileData->m_strFileHead);
		pDC->DrawText(str,&rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rcText.top -= ROW_HEIGHT;
		rcText.bottom -= ROW_HEIGHT;
		if(m_curvePrintInfo.bPrintFileInfo)
		{
			nRowI += 4;
			info.LoadString( IDS_RECORDTYPE );
			str.Format(_T("%s:%s"), info, (CString)pWareFileData->m_strStartType);
			pDC->DrawText(str,&rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top -= ROW_HEIGHT;
			rcText.bottom -= ROW_HEIGHT;
			info.LoadString( IDS_SIMPLE_CYCLE );
			str.Format(_T("%s:%.1f"), info, pWareFileData->m_nCycleCount);
			pDC->DrawText(str,&rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top -= ROW_HEIGHT;
			rcText.bottom -= ROW_HEIGHT;
			info.LoadString( IDS_FIRST_SAMTIME );
			str.Format(_T("%s：%s.%d"), info, pWareFileData->m_FirstSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),
				pWareFileData->m_milliSecondOfFirst);
			pDC->DrawText(str,&rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top -= ROW_HEIGHT;
			rcText.bottom -= ROW_HEIGHT;
			info.LoadString( IDS_RECORD_TIME );
			str.Format(_T("%s%s.%d"), info, pWareFileData->m_StartSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),
				pWareFileData->m_milliSecondOfStart);
			pDC->DrawText(str,&rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top -= ROW_HEIGHT;
			rcText.bottom -= ROW_HEIGHT;
			info.LoadString( IDS_RETURN_RECORD_TIME );
			str.Format(_T("返回段采样启动时间：%s.%d"), info, pWareFileData->m_ReturnSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),
				pWareFileData->m_milliSecondOfReturn);
			pDC->DrawText(str,&rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top -= ROW_HEIGHT;
			rcText.bottom -= ROW_HEIGHT;
		}
		nRowI += 3;    //用于绘制上下刻度尺
		info.LoadString( IDS_STR_WAVECURVE );
		info += _T(": ");
		pDC->DrawText( info, &rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rcText.top= rcText.bottom;
		rcText.bottom -= 2*ROW_HEIGHT;
		for(i=0;i<pWareFileData->m_nCurveCount;i++) 
		{
			if(i==0 && m_curvePrintInfo.bAnalyse) continue;   //波形分析时，前两条曲线要绘在同一坐标系中
			if(pWareFileData->m_pCurve[i].bIsDigital)
			{
				nRowTemp = DIGITAL_DIS_LINE;
			}
			else
			{
				nRowTemp = m_curvePrintInfo.bAnalyse ? ANALYSE_DIS_LINE : CURVE_DIS_LINE;
			}
			if(nRowI + nRowTemp >nRowInPage)
			{
				break;
			}
			else
			{
				nRowI += nRowTemp;
				rcText.bottom -= nRowTemp*ROW_HEIGHT;
			}
		}
		//绘制曲线
		PrintCurve(pDC,rcText,0);
		
		if(i == pWareFileData->m_nCurveCount)
		{
			//绘制表格
			if(m_curvePrintInfo.bPrintChannelInfo && nRowInPage-nRowI > 3)
			{
				nRowI++;
				rcText.top = rcText.bottom;
				rcText.bottom -= ROW_HEIGHT;
				info.LoadString( IDS_CHANEL_INFO ); 
				pDC->DrawText(info + _T(":"), rcText, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
				nRowI++;
				rcText.top = rcText.bottom;
				rcText.bottom -= ROW_HEIGHT;
				nRowTemp = nRowInPage >= (pWareFileData->m_nCurveCount+nRowI) ?
					pWareFileData->m_nCurveCount:(nRowInPage-nRowI);
				nRowI += nRowTemp;
				rcText.bottom -= nRowTemp*ROW_HEIGHT;
				PrintChannelInfo(pDC,rcText,0);
			}
			
			if(m_curvePrintInfo.bPrintParaInfo && (nRowInPage - nRowI) >3)
			{
				nRowI+=2;
				rcText.top = rcText.bottom;
				rcText.bottom -= ROW_HEIGHT;
				info.LoadString( IDS_PARA_INFO );
				pDC->DrawText( info, rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
				rcText.top = rcText.bottom;
				rcText.bottom -= ROW_HEIGHT;
				nRowTemp = nRowInPage > (nParaItemCount+nRowI) ?
					nParaItemCount:(nRowInPage-nRowI);
				nRowI += nRowTemp;
				rcText.bottom -= nRowTemp*ROW_HEIGHT;
				PrintParaInfo(pDC,rcText,0);
			}
		}
		break;
	case PRINT_TYPE_CURVE:
		nRowI = 2;
		rcText.bottom -= ROW_HEIGHT;
		for(i=nPrintNum;i<pWareFileData->m_nCurveCount;i++) 
		{
			if(pWareFileData->m_pCurve[i].bIsDigital)
			{
				nRowTemp = DIGITAL_DIS_LINE;
			}
			else
			{
				nRowTemp = m_curvePrintInfo.bAnalyse ? ANALYSE_DIS_LINE : CURVE_DIS_LINE;
			}
			if(nRowI + nRowTemp >nRowInPage)
			{
				break;
			}
			else
			{
				nRowI += nRowTemp;
				rcText.bottom -= nRowTemp*ROW_HEIGHT;
			}
		}
		//绘制曲线
		PrintCurve(pDC,rcText,nPrintNum);

		if(i == pWareFileData->m_nCurveCount && (nRowInPage - nRowI) >2)
		{
			//绘制表格
			
			if(m_curvePrintInfo.bPrintChannelInfo)
			{
				nRowI++;
				rcText.top = rcText.bottom;
				rcText.bottom -= ROW_HEIGHT;
				info.LoadString( IDS_CHANEL_INFO ); 
				pDC->DrawText(info+ _T(":"), rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
				nRowI++;
				rcText.top = rcText.bottom;
				rcText.bottom -= ROW_HEIGHT;
				nRowTemp = nRowInPage >= (pWareFileData->m_nCurveCount+nRowI) ?
					pWareFileData->m_nCurveCount:(nRowInPage-nRowI);
				nRowI += nRowTemp;
				rcText.bottom -= nRowTemp*ROW_HEIGHT;
				PrintChannelInfo(pDC,rcText,0);
			}
			if(m_curvePrintInfo.bPrintParaInfo && (nRowInPage - nRowI) >2)
			{
				nRowI+=2;
				rcText.top = rcText.bottom;
				rcText.bottom -= ROW_HEIGHT;
				info.LoadString( IDS_PARA_INFO );
				pDC->DrawText(info, rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
				rcText.top = rcText.bottom;
				rcText.bottom -= ROW_HEIGHT;
				nRowTemp = nRowInPage > (nParaItemCount+nRowI) ?
					nParaItemCount:(nRowInPage-nRowI);
				nRowI += nRowTemp;
				rcText.bottom -= nRowTemp*ROW_HEIGHT;
				PrintParaInfo(pDC,rcText,0);
			}
		}
		break;
	case PRINT_TYPE_CHANNEL:
		if(nPrintNum == 0)
		{
			nRowI=2;
			info.LoadString( IDS_CHANEL_INFO ); 
			pDC->DrawText( info + _T(":"), rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top = rcText.bottom;
			rcText.bottom += ROW_HEIGHT;
		}
		else
		{
			nRowI=1;
		}
		
		nRowTemp = nRowInPage >= (pWareFileData->m_nCurveCount-nPrintNum+nRowI) ?
			(pWareFileData->m_nCurveCount-nPrintNum):(nRowInPage-nRowI);
		nRowI += nRowTemp;
		rcText.bottom -= nRowTemp*ROW_HEIGHT;
		PrintChannelInfo(pDC,rcText,nPrintNum);

		if(m_curvePrintInfo.bPrintParaInfo && (nRowInPage - nRowI) >2)
		{
			nRowI+=2;
			rcText.top = rcText.bottom;
			rcText.bottom -= ROW_HEIGHT;
			info.LoadString( IDS_PARA_INFO );
			pDC->DrawText( info, rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top = rcText.bottom;
			rcText.bottom -= ROW_HEIGHT;
			nRowTemp = nRowInPage > (nParaItemCount+nRowI) ?
				nParaItemCount:(nRowInPage-nRowI);
			nRowI += nRowTemp;
			rcText.bottom -= nRowTemp*ROW_HEIGHT;
			PrintParaInfo(pDC,rcText,0);
			}
		break;
	case PRINT_TYPE_PARA:
		if(nPrintNum == 0)
		{
			nRowI=2;
			info.LoadString( IDS_PARA_INFO );
			pDC->DrawText( info, rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top = rcText.bottom;
			rcText.bottom += ROW_HEIGHT;
		}
		else
		{
			nRowI=1;
		}
		nRowTemp = nRowInPage > (nParaItemCount-nPrintNum+nRowI) ?
			(nParaItemCount-nPrintNum):(nRowInPage-nRowI);
		nRowI += nRowTemp;
		rcText.bottom -= nRowTemp*ROW_HEIGHT;
		PrintParaInfo(pDC,rcText,nPrintNum);
		break;
	}


			
	
	PrintPageNum(pDC,pInfo);
	pDC->SelectObject(pOldFont);
}

void CCurveView::OnFilePrintSetup() 
{
	// TODO: Add your command handler code here
	CMainFrame * pMainFrm = (CMainFrame * )GetParentFrame();
	CCurvePrintSet dlg(&pMainFrm->m_wareFileData,m_isLabel,m_dota);
	CString str;
	if(IDOK == dlg.DoModal())
	{
		m_curvePrintInfo.bPrintSet = TRUE;
		m_curvePrintInfo.bAnalyse = IsKindOf(RUNTIME_CLASS(CCurveView)) ? FALSE : TRUE;
		m_curvePrintInfo.bHPrint = dlg.m_bHPrint;
		m_curvePrintInfo.bPrintChannelInfo = dlg.m_bDisChannelInfo;
		m_curvePrintInfo.bPrintFileInfo = dlg.m_bDisFileInfo;
		m_curvePrintInfo.bPrintParaInfo = dlg.m_bDisParaInfo;
		m_curvePrintInfo.dEndTime = dlg.m_dEndTime - dlg.m_dMinTime;
		m_curvePrintInfo.dStartTime = dlg.m_dStartTime - dlg.m_dMinTime;
		m_curvePrintInfo.dTestTime = dlg.m_dTestTime - dlg.m_dMinTime;
		m_curvePrintInfo.cPS = dlg.m_cPS;
		m_curvePrintInfo.bMultiColor = dlg.m_bMultiColor;
		m_curvePrintInfo.bPrintPageNum = dlg.m_bPrintPageNum;
	}
}

void CCurveView::PrintCurve(CDC *pDC, const CRect & rcCurve,int nCurveIndex)
{
	CWareFileData *pWareFileData = &((CMainFrame *)GetParentFrame())->m_wareFileData;
	CRect rc(rcCurve.left-50,rcCurve.top +50,rcCurve.right+50,rcCurve.bottom - 50);
	pDC->Rectangle(&rc);
	if(m_rcPrintCurve != rcCurve)
	{
		ChangePrintCurve(pWareFileData->m_pCurve,pWareFileData->m_nCurveCount,rcCurve,
			m_curvePrintInfo.dEndTime - m_curvePrintInfo.dStartTime,m_curvePrintInfo.cPS);
		if(!m_curvePrintInfo.bMultiColor)
		{
			OnAchromatic();
		}
	}
	DrawPrintCurve(pDC,nCurveIndex);
}


void CCurveView::OnReleaseAngle()
{
	if (lpStaPhaseAngle != NULL)
	{
		delete lpStaPhaseAngle;
	}
	if (lpRetPhaseAngle != NULL)
	{
		delete lpRetPhaseAngle;
	}
}

void CCurveView::PrintChannelInfo(CDC *pDC, const CRect &rcPrint, int nInfoIndex)
{
	int nWidth[]={5000,4000,4000,4000};
	if(m_curvePrintInfo.bHPrint)
	{
		nWidth[0] = A4_W * 5/17;
		nWidth[1] = A4_W * 4/17;
		nWidth[2] = A4_W * 4/17;
	}
	int i,nRow,xPos,yPos,nCurveIndex,nSubItemIndex,nChannelIndex;
	CRect rcTemp;
	CString str, info;
	CWareFileData * pWareFileData = &((CMainFrame*)GetParentFrame())->m_wareFileData;
	pDC->MoveTo(rcPrint.TopLeft());
	pDC->LineTo(rcPrint.right,rcPrint.top);
	pDC->MoveTo(rcPrint.TopLeft());
	pDC->LineTo(rcPrint.left,rcPrint.bottom);
	xPos = rcPrint.left+nWidth[0];
	pDC->MoveTo(xPos,rcPrint.top);
	pDC->LineTo(xPos,rcPrint.bottom);
	xPos += nWidth[1];
	pDC->MoveTo(xPos,rcPrint.top);
	pDC->LineTo(xPos,rcPrint.bottom);
	xPos += nWidth[2];
	pDC->MoveTo(xPos,rcPrint.top);
	pDC->LineTo(xPos,rcPrint.bottom);
	xPos = rcPrint.right;
	pDC->MoveTo(xPos,rcPrint.top);
	pDC->LineTo(xPos,rcPrint.bottom);
	rcTemp = CRect(rcPrint.left+300,rcPrint.top,rcPrint.left+nWidth[0],rcPrint.top-ROW_HEIGHT);
	info.LoadString( IDS_CHANNEL_NAME );
	pDC->DrawText( info, &rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	rcTemp.left=rcTemp.right+300;
	rcTemp.right += nWidth[1];
	info.LoadString( IDS_CHANNEL_TYPE );
	pDC->DrawText( info,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	rcTemp.left=rcTemp.right+300;
	rcTemp.right += nWidth[2];
	info.LoadString( IDS_STR_CYCLE );
	pDC->DrawText(info, &rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	rcTemp.left=rcTemp.right+300;
	rcTemp.right += nWidth[3];
	info.LoadString( IDS_POINT_CYCLE );
	pDC->DrawText(info, &rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	pDC->MoveTo(rcPrint.left,rcTemp.bottom);
	pDC->LineTo(rcPrint.right,rcTemp.bottom);
	nRow = 0 - rcPrint.Height()/ROW_HEIGHT - 1;
	yPos = rcPrint.top-ROW_HEIGHT;
	for(i=0; i<nRow; i++,yPos-=ROW_HEIGHT)
	{
		nCurveIndex =i+nInfoIndex;
		rcTemp = CRect(rcPrint.left+300,yPos,rcPrint.left+nWidth[0],yPos-ROW_HEIGHT);
		nSubItemIndex = pWareFileData->m_pCurve[nCurveIndex].nSubItemIndex;
		nChannelIndex = pWareFileData->m_pCurve[nCurveIndex].nChannelIndex;
		pWareFileData->m_arrPSubItem[nSubItemIndex]->GetChannelName(nChannelIndex,str);
		pDC->DrawText(str,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);

		rcTemp.left= rcTemp.right+300;
		rcTemp.right += nWidth[1];
		switch(pWareFileData->m_arrPSubItem[nSubItemIndex]->GetDataType())
		{
		case ANALOG_TYPE:
			str.LoadString( IDS_STR_ANALOG );
			break;
		case DIGITAL_IN_TYPE:
			str.LoadString( IDS_DIGITAL_INPUT );
			break;
		case DIGITAL_OUT_TYPE:
			str.LoadString( IDS_DIGITAL_OUTPUT );
			break;
		case FREQUENCY_TYPE:
			str.LoadString( IDS_FREQUENCY );
			break;
		case ANGLE_DIFF_TYPE:
			str.LoadString( IDS_ANG_DIFF2 );
			break;
		default:
			break;
		}
		pDC->DrawText(str,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		
		rcTemp.left = rcTemp.right+300;
		rcTemp.right += nWidth[2];
		str.Format(_T("%.1f"),pWareFileData->m_arrPSubItem[nSubItemIndex]->GetCircleCount());
		pDC->DrawText(str,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		
		rcTemp.left = rcTemp.right+300;
		rcTemp.right += nWidth[3];
		str.Format(_T("%d"),pWareFileData->m_arrPSubItem[nSubItemIndex]->GetPointCountInCircle(nChannelIndex));
		pDC->DrawText(str,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		pDC->MoveTo(rcPrint.left,rcTemp.bottom);
		pDC->LineTo(rcPrint.right,rcTemp.bottom);
	}
}

void CCurveView::PrintParaInfo(CDC *pDC, const CRect &rcPrint, int nParaInfoIndex)
{
	int nWidth[]={5000,5000,7000};
	if(m_curvePrintInfo.bHPrint)
	{
		nWidth[0] = A4_W * 5/17;
		nWidth[1] = A4_W * 5/17;
		nWidth[2] = A4_W * 7/17;
	}
	int nRow,xPos,yPos,nItemIndex;
	CRect rcTemp;
	CString str;
	CListCtrl * pListCtrl = &((CMainFrame*)GetParentFrame())->m_listParaInfo;
	pDC->MoveTo(rcPrint.TopLeft());
	pDC->LineTo(rcPrint.right,rcPrint.top);
	pDC->MoveTo(rcPrint.TopLeft());
	pDC->LineTo(rcPrint.left,rcPrint.bottom);
	xPos = rcPrint.left+nWidth[0];
	pDC->MoveTo(xPos,rcPrint.top);
	pDC->LineTo(xPos,rcPrint.bottom);
	xPos += nWidth[1];
	pDC->MoveTo(xPos,rcPrint.top);
	pDC->LineTo(xPos,rcPrint.bottom);
	pDC->MoveTo(rcPrint.right,rcPrint.top);
	pDC->LineTo(rcPrint.BottomRight());
	rcTemp = CRect(rcPrint.left+300,rcPrint.top,rcPrint.left+nWidth[0],rcPrint.top-ROW_HEIGHT);
	CString info;
	info.LoadString( IDS_PARA_TPYEANDINFO );
	pDC->DrawText(info,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	rcTemp.left=rcTemp.right+300;
	rcTemp.right += nWidth[1];
	info.LoadString( IDS_PARA_NAME );
	pDC->DrawText(info, &rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	rcTemp.left=rcTemp.right+300;
	rcTemp.right += nWidth[2];
	info.LoadString( IDS_PARA_VALUE );
	pDC->DrawText( info, &rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	pDC->MoveTo(rcPrint.left,rcTemp.bottom);
	pDC->LineTo(rcPrint.right,rcTemp.bottom);
	nRow = 0 - rcPrint.Height()/ROW_HEIGHT-1+nParaInfoIndex;
	yPos = rcPrint.top-ROW_HEIGHT;
	for(nItemIndex=nParaInfoIndex; nItemIndex<nRow; nItemIndex++,yPos-=ROW_HEIGHT)
	{
		rcTemp = CRect(rcPrint.left+300,yPos,rcPrint.left+nWidth[0],yPos-ROW_HEIGHT);
		str = pListCtrl->GetItemText(nItemIndex,0);
		pDC->DrawText(str,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rcTemp.left= rcTemp.right+300;
		rcTemp.right += nWidth[1];
		str = pListCtrl->GetItemText(nItemIndex,1);
		pDC->DrawText(str,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rcTemp.left = rcTemp.right+300;
		rcTemp.right += nWidth[2];
		str = pListCtrl->GetItemText(nItemIndex,2);
		pDC->DrawText(str,&rcTemp,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		pDC->MoveTo(rcPrint.left,rcTemp.bottom);
		pDC->LineTo(rcPrint.right,rcTemp.bottom);
	}
}

void CCurveView::ChangePrintCurve(Curve *pCurve, int nCurveCount, CRect rcPrint, double dPrintTime,char cPS)
{
	m_rcPrintCurve = rcPrint;
	ASSERT(cPS=='p'||cPS=='s'||cPS=='P'||cPS=='S');
	if(cPS=='p'||cPS=='P') m_strValueType=_T("一次值");
	else m_strValueType=_T("二次值");
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	m_pCurve=new Curve[nCurveCount];
	if(m_pCurve == NULL)
	{
		m_nCurveCount = 0;
		return;
	}
	CString str;
	m_nCurveCount=nCurveCount;
	int i,j,temp_n;
	queue<int> qVolIndex,qCurrentIndex;
	double dMaxOfX=0,dMaxVolY=0,dMaxVal,dMaxCurrentY=0,dVolMinRange,dCurrentMinRange;
	double dValueOfCellY;
	wchar_t cUint;
	for(i=0;i<nCurveCount;i++)
	{
		m_pCurve[i].lpStaPoint=new Point[pCurve[i].StaCount+1];
		m_pCurve[i].lpRetPoint=new Point[pCurve[i].RetCount+1];
		m_pCurve[i].lpStartSecPoint=new Point[pCurve[i].startCount];
		m_pCurve[i].lpReturnSecPoint=new Point[pCurve[i].returnCount];
		if(m_pCurve[i].lpStaPoint == NULL)
		{
			CCurveView::Release(m_pCurve,m_nCurveCount);
			m_pCurve = NULL;
			return;
		}
		m_pCurve[i].StaCount=pCurve[i].StaCount;
		m_pCurve[i].RetCount=pCurve[i].RetCount;
		m_pCurve[i].nSubItemIndex = pCurve[i].nSubItemIndex;
		m_pCurve[i].nChannelIndex = pCurve[i].nChannelIndex;
		m_pCurve[i].firstSampleTime=pCurve[i].firstSampleTime;
		m_pCurve[i].milliSecondOfFirst=pCurve[i].milliSecondOfFirst;
		m_pCurve[i].startTime=pCurve[i].startTime;
		m_pCurve[i].startCount = pCurve[i].startCount;
		m_pCurve[i].returnCount = pCurve[i].returnCount;
		m_pCurve[i].color=m_colorArray[i%COLOR_NUM];
		m_pCurve[i].minRange=pCurve[i].minRange;
		m_pCurve[i].bIsDigital = pCurve[i].bIsDigital;
		m_pCurve[i].bIsOverlap = pCurve[i].bIsOverlap;
		m_pCurve[i].nPointInCircle = pCurve[i].nPointInCircle;
		m_pCurve[i].name = pCurve[i].name;
		m_pCurve[i].unit =  pCurve[i].unit;
		m_pCurve[i].valueOfCellY = pCurve[i].valueOfCellY;
		//这里将所有连续的电压量（或电流）使用同样的Y坐标刻度变比，是为了让用户从界面上就能看出各条曲线的
		//电压（或电流）量的大小，这里只有开入开出量才会设置Y轴上单位刻度所代表的逻辑值（valueOfCellY），
		//模拟量的valueOfCellY为0，也就是未设置，一个坐标系中，Y轴的正负轴各有十个单位刻度，每个单位刻度
		//可以表示一个浮点数的逻辑值，所以很小的数据，也可以在界面得到放大显示，minRange，用于设置坐标系
		//的显示逻辑值范围的最小值
		dMaxVal = 0;
		int sampleRetBegin=0;
		if(m_diffMilli>0)
		{
			intersect=1000;
			m_dota=m_diffMilli;
			str.Format(_T("△t =%dms"),m_dota);
			m_strDota=str;
			m_isLabel=true;
			sampleRetBegin = 60000 - pCurve[2].returnCount/pCurve[2].nPointInCircle*20;
		}
		else
		{
			sampleRetBegin = pMainFrm->m_wareFileData.m_nCycleCount*20- pCurve[2].returnCount/pCurve[2].nPointInCircle*20;
		}
		if(m_pCurve[i].bIsDigital)
		{
			
			for(j=0;j<pCurve[i].StaCount;j++)
			{
				m_pCurve[i].lpStaPoint[j]=pCurve[i].lpStaPoint[j];
				if(m_pCurve[i].lpStaPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
				}
			}
			m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
			m_pCurve[i].lpStaPoint[j].x+=10;
			for(j=0;j<pCurve[i].RetCount;j++)
			{
				m_pCurve[i].lpRetPoint[j]=pCurve[i].lpRetPoint[j];
				m_pCurve[i].lpRetPoint[j].x+=intersect;
				if(m_pCurve[i].lpRetPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
				}
			}
			//用于补偿最后点数不够的问题，即50个点只能连49条线，此处需要51个点才能连出50线，后面同理
			m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
			m_pCurve[i].lpRetPoint[j].x+=10;

			for (j=0;j<pCurve[i].startCount;j++)
			{
				m_pCurve[i].lpStartSecPoint[j]=pCurve[i].lpStartSecPoint[j];
				if(m_pCurve[i].lpStartSecPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpStartSecPoint[j].x;
				}
			}
			for (j=0;j<pCurve[i].returnCount;j++)
			{
				m_pCurve[i].lpReturnSecPoint[j]=pCurve[i].lpReturnSecPoint[j];
				m_pCurve[i].lpReturnSecPoint[j].x+=(intersect+sampleRetBegin);
				if(m_pCurve[i].lpReturnSecPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpReturnSecPoint[j].x;
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
				for(j=0;j<pCurve[i].StaCount;j++)
				{
					m_pCurve[i].lpStaPoint[j]=pCurve[i].lpStaPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStaPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStaPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpStaPoint[j].y);
					}
				}
				m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
				m_pCurve[i].lpStaPoint[j].x+=10;
				for(j=0;j<pCurve[i].RetCount;j++)
				{
					m_pCurve[i].lpRetPoint[j]=pCurve[i].lpRetPoint[j];
					m_pCurve[i].lpRetPoint[j].x+=intersect;
					if(DoubleCmp(m_pCurve[i].lpRetPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpRetPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpRetPoint[j].y);
					}
				}
				m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
				m_pCurve[i].lpRetPoint[j].x+=10;
				for (j = 0; j< m_pCurve[i].startCount ; j++)
				{
					m_pCurve[i].lpStartSecPoint[j]=pCurve[i].lpStartSecPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStartSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStartSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStartSecPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpStartSecPoint[j].y);
					}
				}
				for (j = 0; j< m_pCurve[i].returnCount ; j++)
				{
					m_pCurve[i].lpReturnSecPoint[j]=pCurve[i].lpReturnSecPoint[j];
					m_pCurve[i].lpReturnSecPoint[j].x+=(intersect+sampleRetBegin);
					if(DoubleCmp(m_pCurve[i].lpReturnSecPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpReturnSecPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpReturnSecPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpReturnSecPoint[j].y);
					}
				}
			}
			else if(cUint == TEXT('v') || cUint == TEXT('V'))
			{
				dVolMinRange = m_pCurve[i].minRange;
				qVolIndex.push(i);
				for(j=0;j<pCurve[i].StaCount;j++)
				{
					m_pCurve[i].lpStaPoint[j]=pCurve[i].lpStaPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStaPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStaPoint[j].y)-dMaxVolY)>0)
					{
						dMaxVolY = fabs(m_pCurve[i].lpStaPoint[j].y);
					}
				}
				m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
				m_pCurve[i].lpStaPoint[j].x+=10;
				for(j=0;j<pCurve[i].RetCount;j++)
				{
					m_pCurve[i].lpRetPoint[j]=pCurve[i].lpRetPoint[j];
					m_pCurve[i].lpRetPoint[j].x+=intersect;
					if(DoubleCmp(m_pCurve[i].lpRetPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpRetPoint[j].y)-dMaxCurrentY)>0)
					{
						dMaxCurrentY = fabs(m_pCurve[i].lpRetPoint[j].y);
					}
				}
				m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
				m_pCurve[i].lpRetPoint[j].x+=10;
				for (j = 0; j< m_pCurve[i].startCount ; j++)
				{
					m_pCurve[i].lpStartSecPoint[j]=pCurve[i].lpStartSecPoint[j];
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
					m_pCurve[i].lpReturnSecPoint[j]=pCurve[i].lpReturnSecPoint[j];
					m_pCurve[i].lpReturnSecPoint[j].x+=(intersect+sampleRetBegin);
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
				for(j=0;j<pCurve[i].StaCount;j++)
				{
					m_pCurve[i].lpStaPoint[j]=pCurve[i].lpStaPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStaPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStaPoint[j].y)-dMaxVal)>0)
					{
						dMaxVal = fabs(m_pCurve[i].lpStaPoint[j].y);
					}
				}
				m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
				m_pCurve[i].lpStaPoint[j].x+=10;
				for(j=0;j<pCurve[i].RetCount;j++)
				{
					m_pCurve[i].lpRetPoint[j]=pCurve[i].lpRetPoint[j];
					m_pCurve[i].lpRetPoint[j].x+=intersect;
					if(DoubleCmp(m_pCurve[i].lpRetPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpRetPoint[j].y)-dMaxVal)>0)
					{
						dMaxVal = fabs(m_pCurve[i].lpRetPoint[j].y);
					}
				}
				m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
				m_pCurve[i].lpRetPoint[j].x+=10;
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
	//计算电压中的dValueOfCellY
	if(dMaxVolY<dVolMinRange) dMaxVolY=dVolMinRange;
	temp_n=(long)dMaxVolY;
	//如果tempY大于temp_n
	if(DoubleCmp(dMaxVolY-temp_n)>0)
	{
		temp_n++;
	}
	dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
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
	//计算区域大小
	m_dMulti=1;
	m_dNewMulti = 1;
	SetPrintCoorHeight(0-ROW_HEIGHT*DIGITAL_DIS_LINE,0-ROW_HEIGHT*CURVE_DIS_LINE,rcPrint);
	double d1=(m_clientRc.Width()-CHANNEL_INFO_WIDTH_PRINT)*dPrintTime/m_nMaxOfX;
	m_dMulti = (rcPrint.Width()-CHANNEL_INFO_WIDTH_PRINT)/d1;
	m_dMinMulti = m_dMulti;
	m_dNewMulti = m_dMulti;
	SetPrintCoorHeight(0-ROW_HEIGHT*DIGITAL_DIS_LINE,0-ROW_HEIGHT*CURVE_DIS_LINE,rcPrint);
	m_curRect = rcPrint;
}

void CCurveView::SetPrintCoorHeight(int nHeightDigital,int nHeightAnalog, const CRect & rcPrint)
{
	const int nHeightOfRCScale = 0-ROW_HEIGHT;
	m_nHeightOfDigitalCoor = nHeightDigital;
	m_nHeightOfAnalogCoor = nHeightAnalog;
	POINT origital,tempPoint;
	Point rightPoint;
	int nWidth;
	origital.x=rcPrint.left + CHANNEL_INFO_WIDTH_PRINT;
	origital.y=0;
	rightPoint.x=m_nMaxOfX;
	origital.y=0;
	//计算最左点的横坐标
	MapPoint(rightPoint,tempPoint,0,origital,0);
	nWidth = tempPoint.x + m_curRect.left;
	
	m_clientRc = rcPrint;
	if(m_clientRc.Width() < (nWidth+200))
	{
		m_clientRc.right = m_clientRc.left + nWidth;
	}
	m_rectCurve=CRect(m_clientRc.left+CHANNEL_INFO_WIDTH_PRINT,m_clientRc.top+nHeightOfRCScale,
		m_clientRc.right,m_clientRc.top+rcPrint.Height()-nHeightOfRCScale);
}

void CCurveView::Release(Curve *pCurve,int nCurveCount)
{
	if(pCurve != NULL)
	{
		int i;
		for(i=0;i<nCurveCount;i++)
		{
			if(pCurve[i].lpStaPoint != NULL)
			{
				delete [pCurve[i].StaCount+1] pCurve[i].lpStaPoint;
			}
			if(pCurve[i].lpRetPoint != NULL)
			{
				delete [pCurve[i].RetCount+1] pCurve[i].lpRetPoint;
			}
			if(pCurve[i].lpStartSecPoint != NULL)
			{
				//delete [pCurve[i].startCount] pCurve[i].lpStartSecPoint;
				delete [] pCurve[i].lpStartSecPoint;
			}
			if(pCurve[i].lpReturnSecPoint != NULL)
			{
				//delete [pCurve[i].returnCount] pCurve[i].lpReturnSecPoint;
				delete [] pCurve[i].lpReturnSecPoint;
			}
			
		}
		delete [nCurveCount] pCurve;
	}
}

void CCurveView::DrawPrintCurve(CDC *pDC,int nFirstDrawCurve)
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
				origital.y = nTopToCoor + m_nHeightOfAnalogCoor/2;
				nWidthOfCoor = m_nHeightOfAnalogCoor;
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
			
			index=BinarySearch(m_pCurve[i].lpStaPoint,pLogic,m_pCurve[i].StaCount);
			
			if(m_pCurve[i].bIsDigital)
			{
				if(DoubleCmp(m_pCurve[i].lpStaPoint[index].y))
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
			//	if(!GetFreqSwingAngle(dFreq,dSwing,dAngle,i,index,TRUE))
				if(!GetFreqSwingAngle(dFreq,dSwing,dAngle,i,index,m_pCurve[i].lpStaPoint,m_pCurve[i].StaCount))
				{
					dAngle = 0;
					dFreq = 0;
					dSwing = 0;
				}
				TCHAR cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
				if(cUint == TEXT('a') || cUint == TEXT('A') || cUint == TEXT('v') || cUint == TEXT('V'))
				{
					rc.top += size.cy/2+100;
					rc.bottom -= size.cy/2+100;
					temp.Format(_T("%s:%.3f%s\r\n           %.1f°"),m_pCurve[i].name,
						m_pCurve[i].lpStaPoint[index].y,m_pCurve[i].unit,dAngle);
				}
				else
				{
					temp.Format(_T("%s:%.3f%s"),m_pCurve[i].name,m_pCurve[i].lpStaPoint[index].y,m_pCurve[i].unit);
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
				
				origital.y = nTopToCoor + m_nHeightOfAnalogCoor/2;
				nWidthOfCoor = m_nHeightOfAnalogCoor;
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

void CCurveView::PrintGirdLine(CDC *pDC, int nPrintFirstCurve)
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
	for(i=nPrintFirstCurve;i<m_nCurveCount;i++)
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

void CCurveView::PrintLine(CDC *pDC)
{	
	POINT temp,origitalPoint;
	CPen samplePen(PS_SOLID,1,RGB(224,215,27));
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

	for(i=m_nFirstDisCurve;i<=m_nLastDisCurve;i++)
	{
		pPen=new CPen(PS_SOLID,1,m_pCurve[i].color);
		pDC->SelectObject(pPen);
		if(m_pCurve[i].StaCount<2) continue;
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
		int indexSta=0,indexRet=0,startP=0,returnP=0,endSta=0,endRet=0,endStart=0,endReturn=0;
		temp.x=m_curRect.left+CHANNEL_INFO_WIDTH_PRINT;
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);
		indexSta=BinarySearch(m_pCurve[i].lpStaPoint,pLogic,m_pCurve[i].StaCount+1);
		indexRet=BinarySearch(m_pCurve[i].lpRetPoint,pLogic,m_pCurve[i].RetCount+1);

 		startP=BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
		returnP=BinarySearch(m_pCurve[i].lpReturnSecPoint,pLogic,m_pCurve[i].returnCount);
		temp.x=m_curRect.right;
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);
		endSta=BinarySearch(m_pCurve[i].lpStaPoint,pLogic,m_pCurve[i].StaCount+1);
		endRet=BinarySearch(m_pCurve[i].lpRetPoint,pLogic,m_pCurve[i].RetCount+1);
		
		endStart=BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
		endReturn=BinarySearch(m_pCurve[i].lpReturnSecPoint,pLogic,m_pCurve[i].returnCount);
		
		
		if(!m_pCurve[i].bIsDigital)
		{
			MapPoint(m_pCurve[i].lpStaPoint[indexSta],temp,m_pCurve[i].valueOfCellY,
			origitalPoint,nWidthOfCell);
			if(temp.x >= CHANNEL_INFO_WIDTH_PRINT-2 && temp.x <= m_curRect.Width())
			{
				pDC->MoveTo(temp);
			}
			else if (temp.x < CHANNEL_INFO_WIDTH_PRINT)
			{
				temp.x = CHANNEL_INFO_WIDTH_PRINT;
				pDC->MoveTo(temp);
			}
			for (int k = indexSta ; k < endSta; k++)
			{
				MapPoint(m_pCurve[i].lpStaPoint[k+1],temp,m_pCurve[i].valueOfCellY,
					origitalPoint,nWidthOfCell);
				pDC->LineTo(temp);
				pDC->MoveTo(temp);
			}
			MapPoint(m_pCurve[i].lpRetPoint[indexRet],temp,m_pCurve[i].valueOfCellY,
				origitalPoint,nWidthOfCell);
			if(temp.x > CHANNEL_INFO_WIDTH_PRINT && temp.x < m_curRect.Width())
			{
				pDC->MoveTo(temp);
			}
			else if (temp.x < CHANNEL_INFO_WIDTH_PRINT)
			{
				temp.x = CHANNEL_INFO_WIDTH_PRINT;
				pDC->MoveTo(temp);
			}
			for (k = indexRet+1 ; k < endRet ; k++)
			{
				MapPoint(m_pCurve[i].lpRetPoint[k+1],temp,m_pCurve[i].valueOfCellY,
					origitalPoint,nWidthOfCell);
				pDC->LineTo(temp);
				pDC->MoveTo(temp);
			}
		}
		else
		{
			MapPoint(m_pCurve[i].lpStaPoint[indexSta],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
			if (temp.x < CHANNEL_INFO_WIDTH_PRINT)
			{
				temp.x = CHANNEL_INFO_WIDTH_PRINT;
			}
			pDC->MoveTo(temp);
			for(j=indexSta+1;j<=endSta;j++)
			{
				POINT befPoint,midPoint[2];
				befPoint = temp;
				MapPoint(m_pCurve[i].lpStaPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				//对于从0变1或从1变0的开入量的点，会在它们的中点处添加两点midPoint0,midPoint1,设变化前后两点
				//分别为p1,p2,则midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
				//p1.x)/2,并且系统会顺次连接p1-midPoint0-midPoint1-p2
				if (temp.x < CHANNEL_INFO_WIDTH_PRINT)
				{
					temp.x = CHANNEL_INFO_WIDTH_PRINT;
				}
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
				
			MapPoint(m_pCurve[i].lpRetPoint[indexRet],temp,m_pCurve[i].valueOfCellY,
				origitalPoint,nWidthOfCell);
			pDC->MoveTo(temp);
			for(j=indexRet;j<=endRet;j++)
			{
				POINT befPoint,midPoint[2];
				befPoint = temp;
				MapPoint(m_pCurve[i].lpRetPoint[j],temp,m_pCurve[i].valueOfCellY,origitalPoint,nWidthOfCell);
				if (temp.x < CHANNEL_INFO_WIDTH_PRINT)
				{
					temp.x = CHANNEL_INFO_WIDTH_PRINT;
				}
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
			if (m_bMultiColorDis)
			{
				pDC->SelectObject(&samplePen);//使用黄色
			}
			if (m_pCurve[i].bIsOverlap)//画起始段和返回段曲线
			{
			 	if (!m_pCurve[i].bIsDigital)
				{
					MapPoint(m_pCurve[i].lpStartSecPoint[startP],temp,m_pCurve[i].valueOfCellY,
						origitalPoint,nWidthOfCell);
					pDC->MoveTo(temp);
					for (int k = startP ; k < endStart ; k++)
					{
						MapPoint(m_pCurve[i].lpStartSecPoint[k+1],temp,m_pCurve[i].valueOfCellY,
							origitalPoint,nWidthOfCell);
						pDC->LineTo(temp);
						pDC->MoveTo(temp);
					}
					MapPoint(m_pCurve[i].lpReturnSecPoint[returnP],temp,m_pCurve[i].valueOfCellY,
						origitalPoint,nWidthOfCell);
					pDC->MoveTo(temp);
					for (k = returnP ; k < endReturn ; k++)
					{
						MapPoint(m_pCurve[i].lpReturnSecPoint[k+1],temp,m_pCurve[i].valueOfCellY,
							origitalPoint,nWidthOfCell);
						pDC->LineTo(temp);
						pDC->MoveTo(temp);
					}
 				}
			}
		nTopOfCoor += nWidthOfCell*CELL_COUNT;
		delete pPen;
		pPen = NULL;
	}
	pDC->SelectObject(pOldPen);
}
void CCurveView::PrintScale(CDC *pDC)
{
	const int nHeightOfRCScale = 0-ROW_HEIGHT;
	const int nMinWidthOfDisScale = 80;
	POINT origitalPoint,pTemp[6],pStartTime;
	Point pMinLogPoint,pMaxLogPoint;
	origitalPoint.x=m_rectCurve.left;
	pMinLogPoint.x = m_pCurve[2].startTime;
	MapPoint(pMinLogPoint,pStartTime,0,origitalPoint,0);
	pTemp[0].x=m_curRect.left+CHANNEL_INFO_WIDTH_PRINT;
	MapPointToLogic(pTemp[0],pMinLogPoint,0,origitalPoint,0);
	pTemp[0].x=m_curRect.right;
	MapPointToLogic(pTemp[0],pMaxLogPoint,0,origitalPoint,0);
	int nMinLogX=pMinLogPoint.x-m_pCurve[2].startTime,nMaxLogX=pMaxLogPoint.x-m_pCurve[2].startTime,nLogScale;
	double dLogScale;
	//保证每个逻辑刻度之间的显示横坐标差大于5个像素
	dLogScale = (pMaxLogPoint.x - pMinLogPoint.x)/((double)(m_curRect.Width()-CHANNEL_INFO_WIDTH_PRINT)/nMinWidthOfDisScale);
	nLogScale = dLogScale;
	nLogScale = (DoubleCmp(dLogScale - nLogScale) == 0) ?nLogScale:(nLogScale+1);
	if(nLogScale < m_nMinWidthOfLogScale)
	{
		nLogScale = m_nMinWidthOfLogScale;
	}
	CRect rcTopScale(m_rcPrintCurve.left,m_curRect.top,m_rcPrintCurve.left+m_curRect.Width(),m_curRect.top+nHeightOfRCScale)
		,rcBottonScale;
	if(m_curRect.bottom >= (m_rectCurve.bottom+nHeightOfRCScale))
	{
		rcBottonScale = CRect(m_rcPrintCurve.left,m_curRect.bottom-nHeightOfRCScale,m_rcPrintCurve.left+m_curRect.Width(),
			m_curRect.bottom);
	}
	else
	{
		rcBottonScale = CRect(m_rcPrintCurve.left,m_rectCurve.bottom,m_rcPrintCurve.left+m_curRect.Width(),
			m_rectCurve.bottom+m_nHeightOfRCScale);
	}
	pDC->FillSolidRect(rcTopScale,RGB(255,255,255));
	pDC->FillSolidRect(rcBottonScale,RGB(255,255,255));
	rcTopScale.left+=CHANNEL_INFO_WIDTH_PRINT;
	rcBottonScale.left += CHANNEL_INFO_WIDTH_PRINT;
	int i,nScaleBegin,nScaleEnd,nDisScalePos;
	nScaleBegin = nMinLogX%nLogScale ? nMinLogX/nLogScale+1 : nMinLogX/nLogScale;
	nScaleEnd = nMaxLogX%nLogScale ? nMaxLogX/nLogScale+1 : nMaxLogX/nLogScale;int temp1,temp2,temp3;
	if(m_isLabel)
	{
		temp1 = 300%nLogScale ? 300/nLogScale+1 : 300/nLogScale;
		temp2 = 1300%nLogScale ? 1300/nLogScale+1 : 1300/nLogScale;
		temp3 =	(m_dota-1000)%nLogScale ? (m_dota-1000)/nLogScale+1 : (m_dota-1000)/nLogScale;
	}
	double dLogRange = pMaxLogPoint.x-pMinLogPoint.x;
	double dDisRange = m_curRect.Width()-CHANNEL_INFO_WIDTH_PRINT;
	pTemp[0].y = rcTopScale.bottom;
	pTemp[1].y = rcTopScale.bottom+90;
	pTemp[2].y = rcTopScale.bottom+CHANNEL_INFO_WIDTH;
	pTemp[3].y = rcBottonScale.top;
	pTemp[4].y = rcBottonScale.top-90;
	pTemp[5].y = rcBottonScale.top-CHANNEL_INFO_WIDTH;
	CPen *pOldPen,pen(PS_SOLID,1,RGB(0,0,0)),penGird(PS_DOT,1,m_colorGirdLine);
	CFont font,*pOldFont;
	font.CreateFont(-280,
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	pOldFont=pDC->SelectObject(&font);
	pOldPen = pDC->SelectObject(&pen);
	int nOldBKMode = pDC->SetBkMode(TRANSPARENT);
	BOOL bNotTimesOf5;
	bool bDrawGirdLine = ((5*nLogScale+m_pCurve[2].startTime-pMinLogPoint.x)/dLogRange*dDisRange > 500);
	pDC->MoveTo(rcTopScale.left,rcTopScale.bottom);
	pDC->LineTo(rcTopScale.right,rcTopScale.bottom);
	pDC->MoveTo(rcBottonScale.left,rcBottonScale.top);
	pDC->LineTo(rcBottonScale.right,rcBottonScale.top);
	CRect textRc;
	CString str, info;
	textRc = CRect(m_rcPrintCurve.left + 200,rcTopScale.top,m_rcPrintCurve.left + CHANNEL_INFO_WIDTH_PRINT,
		rcTopScale.bottom);
	info.LoadString( IDS_STR_TIME );
	str.Format(_T("%s[ms]  %s"), info, m_strDota);
	pDC->DrawText(str,&textRc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	textRc.top = rcBottonScale.top;
	textRc.bottom = rcBottonScale.bottom;
	pDC->DrawText(str,&textRc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	for(i=nScaleBegin;i<=nScaleEnd;i++)
	{
		if(m_isLabel&&i>=temp1&&i<temp2)
			continue;
		bNotTimesOf5 = (i%5);
		nDisScalePos = (i*nLogScale+m_pCurve[2].startTime-pMinLogPoint.x)/dLogRange*dDisRange+m_rectCurve.left;
		if(nDisScalePos < m_rectCurve.left) continue;
		if(nDisScalePos > m_rcPrintCurve.right) break;
		pTemp[0].x = pTemp[1].x = pTemp[2].x = pTemp[3].x = pTemp[4].x = pTemp[5].x = nDisScalePos; 
		pDC->MoveTo(pTemp[0]);
		bNotTimesOf5 ? pDC->LineTo(pTemp[1]) : pDC->LineTo(pTemp[2]);
		pDC->MoveTo(pTemp[3]);
		bNotTimesOf5 ? pDC->LineTo(pTemp[4]) : pDC->LineTo(pTemp[5]);
		if(i % 10 == 0)
		{
			textRc = CRect(nDisScalePos-400,rcTopScale.top,nDisScalePos+400,pTemp[2].y);
			
			if(textRc.right <= m_rcPrintCurve.right)
			{
				if(m_isLabel&&i>=temp2)
				{
					str.Format(_T("%d"),(int)((temp3+i)*nLogScale));
				}
				else
				{
					str.Format(_T("%d"),(int)(i*nLogScale));
				}
				pDC->DrawText(str,&textRc,DT_CENTER);
				textRc = CRect(nDisScalePos-400,pTemp[5].y,nDisScalePos+400,rcBottonScale.bottom);
				pDC->DrawText(str,&textRc,DT_CENTER);
			}
			if(nDisScalePos == CHANNEL_INFO_WIDTH_PRINT) continue;
			pDC->SelectObject(&penGird);
			pDC->MoveTo(pTemp[0]);
			pDC->LineTo(pTemp[3]);
			pDC->SelectObject(&pen);
		}
		else if((!bNotTimesOf5) && bDrawGirdLine && (nDisScalePos != CHANNEL_INFO_WIDTH_PRINT))
		{
			pDC->SelectObject(&penGird);
			pDC->MoveTo(pTemp[0]);
			pDC->LineTo(pTemp[3]);
			pDC->SelectObject(&pen);
		}
	}
	pDC->SetBkMode(nOldBKMode);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
}

void CCurveView::PrintPageNum(CDC* pDC, CPrintInfo* pInfo)
{
	if(!m_curvePrintInfo.bPrintPageNum)
	{
		return;
	}
	CRect rcText;
	if(m_curvePrintInfo.bHPrint)
	{
		rcText = CRect(A4_H_FRAME,0-A4_HEAD_INTERVAL-ROW_HEIGHT*(A4_VLINE+1),
			A4_H - A4_H_FRAME,0-A4_HEAD_INTERVAL-ROW_HEIGHT*(A4_HLINE+2));
	}
	else
	{
		rcText = CRect(A4_H_FRAME,0-A4_HEAD_INTERVAL-ROW_HEIGHT*(A4_VLINE+1),
			A4_W - A4_H_FRAME,0-A4_HEAD_INTERVAL-ROW_HEIGHT*(A4_VLINE+2));
	}
	CString str, info;
	info.LoadString( IDS_PAGE_INFO );
	str.Format( info, pInfo->m_nCurPage,pInfo->GetMaxPage());
	pDC->DrawText(str,rcText,DT_RIGHT|DT_SINGLELINE|DT_VCENTER);
}


#define  PAI  3.1415926
BOOL CCurveView::InitAddChannel()
{
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	int index;
	int index2 = 0;

	if (m_pCurve[1].name == _T("主控插件Us") || m_pCurve[1].name == _T("Mainboard Us"))
	{
		index = 1;
		index2 = 2;
	}
	else if (m_pCurve[2].name == _T("主控插件Us") || m_pCurve[2].name == _T("Mainboard Us"))
	{
		index = 2;
		if (m_pCurve[3].name == _T("主控插件Ug") || m_pCurve[3].name == _T("Mainboard Ug"))
		{
			index2 =3;
		}
		else if (m_pCurve[1].name == _T("主控插件Ug") || m_pCurve[1].name == _T("Mainboard Ug"))
		{
			index2 =1;
		}
	}
	else if (m_pCurve[3].name == _T("主控插件Us") || m_pCurve[3].name == _T("Mainboard Us"))
	{
		index = 3;
		if (m_pCurve[2].name == _T("主控插件Ug") || m_pCurve[2].name == _T("Mainboard Ug"))
		{
			index2 =2;
		}
	}
	if (index2 == 0)
	{
		CString str;
		str.LoadString( IDS_CHANNAL_SHOW_ERROR );
		CdMessageBox box(this, str);
		box.MsgBoxEx();
		return false;
	}
	
	m_pCurve[0].lpStartSecPoint=new Point[m_pCurve[index].startCount];
	m_pCurve[0].lpReturnSecPoint=new Point[m_pCurve[index].returnCount];
	m_pCurve[0].startCount = m_pCurve[index].startCount;
	m_pCurve[0].returnCount = m_pCurve[index].returnCount;
	m_pCurve[0].lpStaPoint=new Point[1];
	m_pCurve[0].lpRetPoint=new Point[1];
	m_pCurve[0].StaCount=0;
	m_pCurve[0].RetCount=0;
	m_pCurve[0].nSubItemIndex = m_pCurve[index].nSubItemIndex;
	m_pCurve[0].nChannelIndex =m_pCurve[index].nChannelIndex;
	m_pCurve[0].firstSampleTime=m_pCurve[index].firstSampleTime;
	m_pCurve[0].milliSecondOfFirst=m_pCurve[index].milliSecondOfFirst;
	m_pCurve[0].startTime=m_pCurve[index].startTime;

	m_pCurve[0].color=m_colorArray[0%COLOR_NUM];
	m_pCurve[0].minRange=m_pCurve[index].minRange;
	m_pCurve[0].bIsDigital = m_pCurve[index].bIsDigital;
	m_pCurve[0].bIsOverlap = TRUE;
	m_pCurve[0].nPointInCircle = m_pCurve[index].nPointInCircle;
	m_pCurve[0].name.LoadString( IDS_PULSAT_VOL );
	m_pCurve[0].unit =  _T(" ");
	m_pCurve[0].valueOfCellY = 0.28;
	float Period = GetCurvePeriod(TRUE,index);
	float sSetVol = pMainFrm->m_fSVolSet;
	float gSetVol = pMainFrm->m_fGVolSet;
	int   indexFreq = 0;
    double sysVol = 0.0, ugVol = 0.0, maxValue = 0.0;
	double nSamplePeriod = static_cast<double>(1000/FREQUENCY)/(m_pCurve[index].nPointInCircle);
	double waveRidge = 0.0; //电压波形波峰

	int i = 0;
	for (i =0 ; i < m_pCurve[0].startCount ; i++)
	{
		
		if ( 0 == (i % m_pCurve[index].nPointInCircle) )
		{
			int maxPoint = i + m_pCurve[index].nPointInCircle;
            waveRidge = 0.0;
           	for (int j = i; j < maxPoint; j++ )
			{
				if ( fabs( m_pCurve[index].lpStartSecPoint[j].y ) - waveRidge > 0  )
				{
					waveRidge =  fabs( m_pCurve[index].lpStartSecPoint[j].y );
				}
			}
		}
		m_pCurve[0].lpStartSecPoint[i].x = m_pCurve[index].lpStartSecPoint[i].x /*- m_fMZstartDota*/; //2014-8-18 moidy 脉振电压x轴值不变，有应转角，只是系统侧电压波形整体偏移应转角
		if (sSetVol!=0 && gSetVol!=0)
		{
			sysVol = m_pCurve[index].lpStartSecPoint[i].y; //2014-8-18 moidy 
			ugVol = m_pCurve[index2].lpStartSecPoint[i].y;
			if ( theApp.m_freq[0] > 0 )
			{//有系统侧电压
				 double vol = pow( waveRidge, 2) - pow(sysVol, 2); //偏移电压
				 bool isIncrease = false;
				 double angle = 0.0;
				 if ( (i < m_pCurve[0].startCount && sysVol < m_pCurve[index].lpStartSecPoint[i+ 1].y ) 
					 || ( (m_pCurve[0].returnCount-1) == i && sysVol > m_pCurve[index].lpStartSecPoint[i - 1].y))
				 {	 
					 vol = -sqrt( vol );
				 }else
				 {
					 vol = sqrt( vol );
				 }
                 if ( pMainFrm->m_SystemAngleSet > 0)
                 {
					 angle = (PI * pMainFrm->m_SystemAngleSet )/180.0;
					 sysVol = cos( angle ) * sysVol + sin( angle ) * vol; //U = cos30Ut + （Us平方 - Ut平方）开根号 /2 
                 }else if ( pMainFrm->m_SystemAngleSet < 0)
                 {
					 angle = -(PI * pMainFrm->m_SystemAngleSet )/180.0;
					 sysVol = cos( angle )* sysVol - sin( angle ) * vol; //U = cos30Ut - （Us平方 - Ut平方）开根号 /2 
                 }
			}
			m_pCurve[0].lpStartSecPoint[i].y = sysVol/sSetVol - ugVol/gSetVol;

		}
		else
		{
			m_pCurve[0].lpStartSecPoint[i].y = 0;
		}
		//2014-10-22 add 
		if ( fabs(m_pCurve[0].lpStartSecPoint[i].y) > maxValue )
		{
			maxValue = fabs(m_pCurve[0].lpStartSecPoint[i].y);
		}
	}
	//2014-10-23 add
	if (maxValue > 2 )
	{
       m_pCurve[0].valueOfCellY = (maxValue+1)/10;
	}
	Period = GetCurvePeriod(FALSE,index);
	waveRidge = 0.0;
	for (i =0 ; i < m_pCurve[0].returnCount ; i++)
	{
		
		m_pCurve[0].lpReturnSecPoint[i].x = m_pCurve[index].lpReturnSecPoint[i].x /*- m_fMZstartDota*/;
		if (sSetVol!=0&&gSetVol!=0)
		{	
			if (  0 == (i % m_pCurve[index].nPointInCircle) )
			{
				int maxPoint = i + m_pCurve[index].nPointInCircle;
				waveRidge = 0.0;
				for (int j = i; j < maxPoint; j++ )
				{
					if ( fabs( m_pCurve[index].lpReturnSecPoint[j].y ) - waveRidge > 0  )
					{
						waveRidge =  fabs( m_pCurve[index].lpReturnSecPoint[j].y );
					}
				}
			}
			sysVol = m_pCurve[index].lpReturnSecPoint[i].y; //2014-8-18 moidy 
			ugVol = m_pCurve[index2].lpReturnSecPoint[i].y;
			if ( theApp.m_freq[0] > 0 )
			{//有系统侧电压
				double vol = pow( waveRidge, 2) - pow(sysVol, 2); //偏移电压
				bool isIncrease = false;
				if ( (i < m_pCurve[0].returnCount && sysVol < m_pCurve[index].lpReturnSecPoint[i+1].y ) 
					|| ( (m_pCurve[0].returnCount-1) == i && sysVol > m_pCurve[index].lpReturnSecPoint[i - 1].y))
				{//递增
					
					vol = -sqrt( vol );
				}else
				{
					vol = sqrt( vol );
				}
				double angle = 0.0;
				if ( pMainFrm->m_SystemAngleSet > 0)
				{
					angle = (PI * pMainFrm->m_SystemAngleSet )/180.0;
                    sysVol = cos( angle ) * sysVol + sin( angle ) * vol;
				}else if ( pMainFrm->m_SystemAngleSet < 0)
				{
					angle = -(PI * pMainFrm->m_SystemAngleSet )/180.0;
					sysVol = cos( angle ) * sysVol - sin( angle ) * vol;
				}
				
			}
		    m_pCurve[0].lpReturnSecPoint[i].y = sysVol/sSetVol - ugVol/gSetVol;
		}
		else
		{
			m_pCurve[0].lpReturnSecPoint[i].y = 0;
		}
	
	}
	return TRUE;
}

float CCurveView::GetCurvePeriod(BOOL isHead,int index)
{
	double dPointOfX[]={-1,-1,-1}; 
	int nPointInMinFreq =(int)(double) 50 * m_pCurve[0].nPointInCircle/m_dMinFreq;
	int indexOfZero[3];     //用于保存与三个零点坐标最近的采样点的序号
	int nIndexOfZeroPoint = 2;
	nPointInMinFreq = nPointInMinFreq*3/2;
	
	int pointCount;
	Point* pPoint;
	if (isHead)
	{
		pPoint = m_pCurve[index].lpStartSecPoint;
		pointCount = m_pCurve[index].startCount;
	}
	else
	{
		pPoint = m_pCurve[index].lpReturnSecPoint;
		pointCount = m_pCurve[index].returnCount;
	}
	for (int j = pointCount-1 ;j >=0 ;j--)
	{
		if(DoubleCmp(pPoint[j].y) != DoubleCmp(pPoint[j-1].y))
		{
			//线性插值过程
			switch(DoubleCmp(pPoint[j-1].y))
			{
			case -1:
				dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + 
					(pPoint[j].x - pPoint[j-1].x)*
					fabs(pPoint[j-1].y)/(pPoint[j].y-pPoint[j-1].y);
				break;
			case 1:
				dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + 
					(pPoint[j].x - pPoint[j-1].x)*pPoint[j-1].y/(fabs(pPoint[j].y)+pPoint[j-1].y);
				break;
			default:
				dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x;
				break;
			}
			
			indexOfZero[nIndexOfZeroPoint] = j-1;
			j -=MIN_NUM_INTERVAL_OF_ZERO;  //为了避免在距零点很近的地方找到第二个零点，这样的零头找到也是无效的
			nIndexOfZeroPoint--;
			if(nIndexOfZeroPoint<0) //如果已经找到了三个零点
			{
				return pPoint[indexOfZero[2]].x - pPoint[indexOfZero[0]].x;
			}
		}
	}
	return 0;
}

Point CCurveView::ModifyAngle(Point x)
{
	CMainFrame* pMainFrm = (CMainFrame*)GetParentFrame();
	int sysAngleSet = pMainFrm->m_SystemAngleSet;
	x.y = x.y - sysAngleSet;
	x.y = x.y>180?x.y-360:x.y;
	return x;
}
//2016-5-5 add 添加导出录波数据
BOOL  MultCharToChar(const CString &src,char * retBuf, int bufLen) 
{
	ASSERT(retBuf != NULL);
	int len = src.GetLength();
//	LPCWSTR  pwch = src.GetString();
	/*ASSERT(bufLen > len);*/
	int ret = ::WideCharToMultiByte(CP_ACP, 0, src, -1, (LPSTR)retBuf, bufLen, NULL, NULL);
	/*retBuf[len] = '\0';*/
	return ret;	
 }
void CCurveView::OnExportWareFile()
{
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
	CString strPath;
	if(::SHGetPathFromIDList(pidl,FullPath))           //在ITEMIDLIST中得到目录名的整个路径   
	{   
		//成功   
		strPath=FullPath;
		if(strPath[strPath.GetLength()-1]!=TEXT('\\'))
		{
			strPath += _T("\\");
		}
		CFile file;
		//创建录波文件
		int i = 0;
		int nCurveCount = m_nCurveCount;
		int exportCount = 0;
		int type = 0;
		for(i = 0; i < nCurveCount; i++)
		{
			if ( m_pCurve[i].bIsDigital )
			{
				continue;
			}
			CString filePath;
			filePath.Format(_T("%s%s.txt"), strPath, m_pCurve[i].name );
			CFile file;
			if (file.Open(filePath, CFile::modeCreate | CFile::modeWrite))
			{
				char   buf[30] = {0};
				CString temp;
				int pointCnt = m_pCurve[i].startCount;
				int j=0;
				for (j = 0; j < pointCnt; j++)
				{
					temp.Format( _T(" %0.3f\r\n"), m_pCurve[i].lpStartSecPoint[j].y );
					memset( buf, 0, sizeof(buf) );
					MultCharToChar( temp, buf, temp.GetLength());
					file.Write( buf, temp.GetLength() );
				}
				 pointCnt = m_pCurve[i].returnCount;
				for (j = 0; j < pointCnt; j++)
				{
					temp.Format( _T(" %0.3f\r\n"), m_pCurve[i].lpReturnSecPoint[j].y );
					memset( buf, 0, sizeof(buf) );
					MultCharToChar( temp, buf, temp.GetLength());
					file.Write( buf, temp.GetLength() );
				}
				file.Close();
			}
	       	exportCount++;
		}
		if( exportCount > 0 )
		{
			AfxMessageBox(_T("导出文件成功"));
		}else
		{
			AfxMessageBox(_T("无模拟量通道"));
		}
	}
}


void CCurveView::DrawEventFlag(CDC *pDC)
{
	//画事件标志
	CWareFileData *pWareData = ((CMainFrame *)GetParentFrame())->GetWaveFileDataPtr();
	int nSubItem = pWareData->GetSubItemArrayCount();
	CWareDataSubItem *pSub = NULL;
	
	for (int i=0;i<nSubItem;i++)
	{
		pSub = pWareData->m_arrPSubItem[i];
		if (WARE_EVENT_TYPE == pSub->GetDataType())
		{
			break;
		}
	}
	if (pSub == NULL)
	{
		CString tempStr;
		tempStr.LoadString( IDS_EVENTSUB_ERROR );
		AfxMessageBox(tempStr);
		return;
	}
	//事件个数
	int count = pSub->GetChannelCount();
	if (WARE_EVENT_TYPE == pSub->GetDataType() && count > 0)
	{	
		CRect rect;
		double EventLogicX = 0.0;
		long   EventPixelX = 0;
		int nBottomOfScale = 0;/*整个坐标系总高*/
		if(m_curRect.bottom <= (m_rectCurve.bottom+m_nHeightOfRCScale))
		{
			nBottomOfScale = m_curRect.bottom - m_nHeightOfRCScale;
		}
		else
		{
			nBottomOfScale = m_rectCurve.bottom;
		}
		CEventDescribe event;
		for (int i = 0; i < count; i++)
		{
			CEventDescribe *pEvent = NULL;
			pEvent = (CEventDescribe *)( (CWareEventData *)pSub)->GetChannelData(i);
			event= * pEvent;
			if( NULL != pEvent && event.isVail)
			{	
				COleDateTime tempTime=event.eventTime-m_pCurve[0].firstSampleTime;
				EventLogicX=(tempTime.GetMinute()*60+tempTime.GetSecond())*1000+event.nMillisecond-m_pCurve[2].milliSecondOfFirst;
				//	
				int startTime;
				if (m_pCurve[2].nPointInCircle)
				{
					startTime=m_pCurve[2].startCount/m_pCurve[2].nPointInCircle*20;
				}
				else
				{
					startTime = 0;
				}
                if ( EventLogicX > startTime+m_dota )
                {
					EventLogicX += intersect;
                }

				Point invirtualpoint;
				POINT onvirtualpoint,origitalPoint;
				invirtualpoint.x=EventLogicX;
				origitalPoint.x=m_rectCurve.left;
				MapPoint(invirtualpoint,onvirtualpoint,0,origitalPoint,0);
				EventPixelX=onvirtualpoint.x;
				if ( pEvent )
				{
					pEvent->nEventPosX = EventPixelX;
				}
				CRect rect;
				rect.top = nBottomOfScale - 3;
				rect.bottom = nBottomOfScale + 3;
				rect.left = EventPixelX - 3;
				rect.right = EventPixelX + 3;	

				COLORREF penClor;
				if (LB_EVENT_TYPE  == event.type)
				{
					penClor = (0, 0, 255);
				//	penClor = RGB(0,255,0);
				}else
				{
				//	penClor = (29, 163, 61);
					penClor = (0, 0, 255);
				//	penClor = RGB(0,255,0);
				}				
				CPen evenPen(PS_SOLID, 2, penClor);
				CPen *pOldPen = pDC->SelectObject(&evenPen);
				if (rect.left > CHANNEL_INFO_WIDTH)
				{
					pDC->MoveTo(EventPixelX,m_rectCurve.bottom);
					pDC->LineTo(EventPixelX,m_rectCurve.bottom);
					pDC->MoveTo(rect.left, rect.top);
					pDC->LineTo(rect.right, rect.bottom);
					pDC->MoveTo(rect.right, rect.top);
					pDC->LineTo(rect.left, rect.bottom);
				}
				pDC->SelectObject(pOldPen);
			}
		}
	}
}



void CCurveView::DrawEventToolTips(CDC* pDC)
{

	//画事件发生提示内容
	int totalCount = 0;
	CPoint pointScrollPos = GetScrollPosition();
	CString tempStr = _T("无意义");
	CSize TextSize = pDC->GetTextExtent(tempStr);
	vector<CEventDescribe> eventList; 	
	CWareFileData *pWareData = ((CMainFrame *)GetParentFrame())->GetWaveFileDataPtr();
	int nSubItem = pWareData->GetSubItemArrayCount();
	CWareDataSubItem *pSub = NULL;
	
	for (int i=0;i<nSubItem;i++)
	{
		pSub = pWareData->m_arrPSubItem[i];
		if (WARE_EVENT_TYPE == pSub->GetDataType())
		{
			break;
		}
	}
	if (pSub == NULL)
	{
		tempStr.LoadString( IDS_EVENTSUB_ERROR );
		AfxMessageBox(tempStr);
		return;
	}
	//事件个数
	int count = pSub->GetChannelCount();
    int retCount = 0 ;
	if (WARE_EVENT_TYPE == pSub->GetDataType() && count > 0)
	{
		CEventDescribe event;
		totalCount = count;
		for (int i = 0; i < count; i++)
		{
			CEventDescribe *pEvent = NULL;
			pEvent = (CEventDescribe *)( (CWareEventData *)pSub)->GetChannelData(i);
			event= * pEvent;
			if ( NULL != pEvent && event.isVail && ((m_mousePoint.x - pointScrollPos.x) > event.nEventPosX - 2  && (m_mousePoint.x - pointScrollPos.x) < event.nEventPosX + 2))
			{
				eventList.push_back( event );
				retCount++;
				/*eventAddr[retCount++] = (DWORD)pEvent;*/
			}
		}
		
	}
	if (retCount > 0 )
	{
		ASSERT( eventList.size() > 0 );
		CEventDescribe event;
		CRect rect;
		int width = 250, hight = 10;
		CString name, temp, str,comment;
		for (int i = 0; i < retCount; i++)
		{

			event = eventList[i];
			name = event.name;
			if (event.type == LB_EVENT_TYPE)
			{
				comment.LoadString( IDS_WAVE_EVENT );
			}
			else if (event.type == KZ_EVENT_TYPE)
			{
				comment.LoadString( IDS_CONTROL_EVENT );
			}
			else if (event.type == SVERROR_EVENT_TYPE)
			{
				comment.LoadString( IDS_SVERROR_EVENT );
			}
			temp.Format(_T("%s %s %u(%s)\n"), name, event.eventTime.Format(_T("%Y-%m-%d %H:%M:%S")),event.nMillisecond,comment);
			str += temp;
			hight += TextSize.cy/2 + 10;		
			CSize textSize = pDC->GetTextExtent(temp);
			if(textSize.cx + 20 > width)
				width = textSize.cx + 20;
		}
		rect.left = m_mousePoint.x - pointScrollPos.x + 5;
		CRect clientRect;
		GetClientRect(clientRect);
		if (!((rect.left + width > clientRect.left + CHANNEL_INFO_WIDTH) && (rect.left + width < clientRect.right)))
		{
			rect.left = rect.left - width;
		}
		rect.top = m_mousePoint.y /*- pointScrollPos.y */+ 5;
		if (!((rect.top + hight > clientRect.top + m_nHeightOfRCScale) &&(rect.top + hight < clientRect.bottom - m_nHeightOfRCScale)))
		{
			rect.top = rect.top - hight;
		}
		rect.right = rect.left + width;
		rect.bottom = rect.top + hight;
		int oldmode = pDC->SetBkMode(TRANSPARENT);
		pDC->FillSolidRect(rect, RGB(207, 252, 242));
		pDC->DrawText(str, &rect, DT_LEFT);
		pDC->SetBkMode(oldmode);
//		CString cDebug;cDebug.Format(_T("DrawEventToolTips:%s"), str);MessageBox(str);//DEBUG-TEST-ZYS
	}
	eventList.clear();
}
void CCurveView::ScrollByTime(const COleDateTime &DataTime,const int Milli)
{
	/*计算可视区域最左方与最右方点时间距离*/
	CMainFrame *pMainFrm = static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CWareFileData *pFileData = pMainFrm->GetWaveFileDataPtr();
    double EventLogicX;

	
	COleDateTime tempTime=DataTime-m_pCurve[0].firstSampleTime;
	EventLogicX=(tempTime.GetMinute()*60+tempTime.GetSecond())*1000+Milli-m_pCurve[2].milliSecondOfFirst;
	Point invirtualpoint;
	POINT onvirtualpoint,origitalPoint;
	invirtualpoint.x=EventLogicX;
	origitalPoint.x=m_rectCurve.left;
	MapPoint(invirtualpoint,onvirtualpoint,0,origitalPoint,0);

	long PixelOfX = onvirtualpoint.x;
	int wideHalf = m_curRect.Width()/2;
	long movePixel = 0;
	movePixel = PixelOfX - wideHalf;
	CPoint pointScrollPos = GetScrollPosition();
	pointScrollPos.x += movePixel;
	if ( pointScrollPos.x <= 0 ) 
	{
		return;
	}
	pointScrollPos.y = 0xffffffff;	
	ScrollToPosition(pointScrollPos);/*滚动到某个绝对位置，滚动时只是坐标轴区域的像素*/
	RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW);
}


