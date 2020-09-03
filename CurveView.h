#if !defined(AFX_CURVEVIEW_H__CC9F0AEB_E8A4_423F_8EDA_92FDCC8DBAE6__INCLUDED_)
#define AFX_CURVEVIEW_H__CC9F0AEB_E8A4_423F_8EDA_92FDCC8DBAE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurveView.h : header file
//
#include "PreView.h"
/////////////////////////////////////////////////////////////////////////////
// CCurveView view
#define CELL_COUNT 20    //1倍格线宽度
#define EACH_SCROLL_LEN   3     //每次滚动量程的几（3）分之一
#define CHANNEL_INFO_WIDTH 230     //左边通道信息的显示宽度
#define CHANNEL_INFO_WIDTH_PRINT  4500  //在打印视图中左边通道信息宽度
#define DIGITAL_HEIGHTH_COONDIATE 20     //数字量纵坐标的高度
#define ANALOG_HEIGHTH_COONDIATE  60   //模拟量的纵坐标高度
#define MAX_COONDIATE_NUM 15          //最大的显示坐标数
#define Precision (1e-8)
#define DISABLED_COLOR  RGB(225,225,225)
#define MAX_H_MULTI_NUM 10

#define MIN_NUM_INTERVAL_OF_ZERO   4    //表示每周波采样点数不低于4*2

#define COLOR_NUM      12


#define PRINT_TYPE_FILE    0x01
#define PRINT_TYPE_CURVE   0x02
#define PRINT_TYPE_CHANNEL 0x03
#define PRINT_TYPE_PARA    0x04

#define CURVE_DISPLAY     0X01
#define CURVE_ANALYSE_SYNC 0X02   //曲线用于40A,40B录波分析的

#define VALID_VALUE_SHOW 0x01
#define SAMPLE_VALUE_SHOW 0x02
#define ALL_VALUE_SHOW 0x03

#define MAXNUM_CHANANL  80  //最大录波通道数目

long DoubleCmp(double d);
template <class T>
long BinarySearch(T arr[],T key,long count)
{
	long left=0,right=count-1,mid=count/2;
	while(right-left>1)
	{
		if(key==arr[mid])
		{
			return mid;
		}
		if(key<arr[mid])
		{
			right=mid;
			mid=(left+right)/2;
		}
		else
		{
			left=mid;
			mid=(left+right)/2;
		}
	}
	T t1,t2;
	t1=key-arr[left];
	t2=arr[right]-key;
	if(t1<t2)
	{
		return left;
	}
	else
	{
		return right;
	}
} 
struct Point
{
	double x;
	double y;
	double extendValue; //在录波分析中，extendValue是由采样点计算出来的瞬时值，而y是标么值
	Point(){x=0;y=0;extendValue=0;}
	Point(double _x,double _y);
	Point & operator = (const Point & pt);
	bool operator <(Point p);
	Point operator -(const Point p);
	bool operator ==(Point p1);
};
struct Curve
{
	Point *lpStaPoint;           //全程启动段采样点数组
	Point *lpRetPoint;           //全程返回段采样点数组
	Point *lpStartSecPoint;           //起始段点数组
	Point *lpReturnSecPoint;           //返回段点数组
	double startTime;         //启动录波时间，这里是以ms为单位，表示距离第一个采样点的时间间隔
	long StaCount;               //曲线的全程启动段点数
	long RetCount;               //曲线的全程返回段点数
	long startCount;			//曲线采样启动段点数
	long returnCount;			//曲线采样返回段点数
	long nPointInCircle;  		//模拟量中用于计算频率相位角等等，开入量中暂时没有用
	COLORREF color;   			//曲线颜色，可以系统默认
	double valueOfCellY;  		//Y轴上一个刻度的单位,用户如果不设或设为0表示让系统设置
	double minRange;      //曲线显示时坐标系表示逻辑值的值的范围最小不能小于minRange
	CString name;     //通道名称
	CString unit;            //单位，如伏，安等，必须设置
	double fmaxOfY;
	COleDateTime firstSampleTime;     //第一个采样点时间
	int milliSecondOfFirst;           //第一个采样点的ms数
	bool bIsDigital;                  //是否为开入开出量
	bool bIsOverlap;                  //是否有起始段和返回段
	double dTurnAngle;                //转角，单位度
	int nSubItemIndex;                //子项序号
	int nChannelIndex;                //nSubItemIndex子项中的通道序号z
	Curve()
	{
		bIsOverlap = false;
		bIsDigital = false;
		fmaxOfY=0;
		nSubItemIndex = -1;
		nChannelIndex = -1;
		unit=_T("");
		name =_T("");
		minRange=0;
		color=RGB(0,0,255);
		StaCount=0;
		RetCount=0;
		startCount = 0;
		returnCount = 0;
		valueOfCellY=0;
		nPointInCircle = 0;
		dTurnAngle = 0;
		lpStaPoint=NULL;
		lpRetPoint=NULL;
		lpStartSecPoint = NULL;
		lpReturnSecPoint = NULL;
	}
};
struct CurvePrintInfo
{
	BOOL bPrintSet;   //曲线打印参数是否设置过
	BOOL bAnalyse;
	BOOL bPrintFileInfo;
	BOOL bPrintChannelInfo;
	BOOL bPrintParaInfo;
	BOOL bHPrint;     //是否横向打印
	BOOL bMultiColor;  //是否为彩色打印
	BOOL bPrintPageNum; //是否打印页码
	char cPS;         //一次值还是二次值
	double dStartTime;
	double dEndTime;
	double dTestTime;
	CurvePrintInfo()
	{
		bPrintSet = FALSE;
		bAnalyse = FALSE;
		bHPrint = FALSE;
		bPrintFileInfo = FALSE;
		bPrintChannelInfo = FALSE;
		bPrintParaInfo = FALSE;
		bPrintPageNum = FALSE;
		bMultiColor = FALSE;
		dStartTime = 0;
		dEndTime = 0;
		dTestTime = 0;
	}
};
struct WaveInfo //录波文件基本信息
{
	float SumCycleCount;//采样总周波数
	float SamStaCycleCount;//起始采样段周波数
	float SamRetCycleCount;//返回采样段周波数
	float AvaStaCycleCount;//起始有效段周波数
	float AvaRetCycleCount;//返回有效段周波数
	int   lbVer; //录波文件规约版本号
};
class CCurveView : public CMyView
{
protected:
	CCurveView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCurveView)

// Attributes
public:
	virtual void ChangeCurve(Curve *pCurve,long count,bool isUnitedFile,int diffMilli,char ps);  //重新设置显示曲线
	//计算频率，幅值，相位角
	BOOL CCurveView::GetFreqSwingAngle(double &dFreq, double &dSwing, double &dAngle,
                             const int indexOfChannel, const int indexOfPoint, Point* pPoint, int count );
	BOOL CaluateAngle( double &dFreq, double &dSwing, double &dAngle, const int indexOfChannel,
		const int indexOfPoint, Point* pPoint, int count, int *indexOfZero, double *dPointOfX);
	BOOL CaluateAngleByHalfWave( double &dFreq,  double &dAngle, const int indexOfChannel,
		const int indexOfPoint, Point* pPoint, int count, int *indexOfZero, double *dPointOfX);
	BOOL CaluateAngleByFreqA( double &dFreq,  double &dAngle, const int indexOfChannel,
		const int indexOfPoint, Point* pPoint, int count, int *indexOfZero, double *dPointOfX);
	BOOL FindZeroPosRight( int  &nIndexOfZeroPoint, const int indexOfChannel, const int indexOfPoint, Point* pPoint, 
		int count, int *indexOfZero, double *dPointOfX);
	BOOL FindZeroPosLeft( int  &nIndexOfZeroPoint, const int indexOfChannel, const int indexOfPoint, Point* pPoint,
		int count, int *indexOfZero, double *dPointOfX);
	 //绘制曲线	
	virtual void DrawLine(CDC *pDC);           
	//绘制坐标线，并计算出显示区域中要显示的起始和终止坐标系的编号（从0开始）
	virtual void DrawGridLine(CDC *pDC);        
	virtual void DrawCurve(CDC *pDC);  
	//设置模拟量通道和开入开出量通道的坐标高度，计算出显示所有曲线的逻辑画布的大小
	virtual void SetCoorHeight(int nDigitalHeight = DIGITAL_HEIGHTH_COONDIATE,
		int nAnalogHeight = ANALOG_HEIGHTH_COONDIATE);
	virtual ~CCurveView();
	//要显示的逻辑坐标转化为显示区域的显示坐标
	POINT MapPoint(Point p1, POINT &p2,double cellOfY,POINT origitalPoint,int nWidthOfCell);  
    //将显示区域的显示坐标转化为逻辑坐标（显示坐标以像素为单位）
	Point MapPointToLogic(POINT p1, Point &p2,double cellOfY,POINT origitalPoint,int nWidthOfCell);  
	void ScrollByTime(const COleDateTime &DataTime,const int Milli);// 滚动到指定位置
	
	void SetMulti(CDC *pDC);            //改变显示倍率
	long m_nCurveCount;  //记录曲线的条数
	Curve * m_pCurve;  //记录曲线数组的指钍
	CurvePrintInfo m_curvePrintInfo;
	//void Init(Curve *pCurve,long count);
	void DrawBackBitmap( CDC* pDC );//2015-1-14 add 添加背景图绘制
	void DrawTransparentBitmap(CBitmap &bmp, CDC *pDC, int x, int y, int nWidth, int nHeight, COLORREF crColour);
// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveView)
	public:
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);  //窗口滚动时的画布移动
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strValueType;      //一次值或二次值
	COLORREF m_coordinateColor;  //坐标轴线的颜色
	COLORREF m_crossLineColor; //跟随鼠标移动的竖线的颜色
	COLORREF m_colorStartLine; //启动录波时间线的颜色
	//UINT m_timer;   //1倍时的定时器
	
	CPoint m_mousePoint;  //记录WM_MOUSEMOVE事件时的位置，用于记录鼠标的位置

	long m_nMaxOfX;       //记录曲线中最大的X坐标
	CRect	m_rectCurve;        //记录曲线绘制的区域，包括水平滚动条和曲线绘制区域
	double m_dMulti;         //记录当前的显示倍数
	
	int m_arrPageHeadInfo[10];      //高字表示打印时页首的信息类型，低字表示该类信息的第几个单位。
	long m_nCellOfX;       //记录X轴上一个刻度所表示的X的逻辑值
	COLORREF m_bkColor;    //记录整个显示区域的背景色
public:
	Point ModifyAngle(Point x);
	float m_fMZreturnDota;
	float m_fMZstartDota;
	float GetCurvePeriod(BOOL isHead,int index);
	BOOL InitAddChannel();
	double m_TimeCloseOut;
	int m_flagShow;
	bool m_bFirstShow;
	int m_dota;
	double m_average[MAXNUM_CHANANL];
	CString m_strDota;
	bool m_isLabel;
	int intersect;        //相交
	int m_diffMilli;
	static BOOL m_isFirstShow;
	bool m_isUnitedFile;             ///否为波形合并后的文件，如果是，则标记值为1,否则为0 
	//为str长度为nCount,不足用空格补充，当nCount为正时，在str后面补充，当nCount为负时，在str左边补充 
	static CString & Fromat(CString & str,int nCount);
	void PrintPageNum(CDC* pDC, CPrintInfo* pInfo);
	void PrintScale(CDC *pDC);
	virtual void PrintLine(CDC * pDC);
	virtual void PrintGirdLine(CDC * pDC,int nPrintFirstCurve);
	virtual void DrawPrintCurve(CDC * pDC,int nFirstDrawCurve);
	virtual void SetPrintCoorHeight(int nHeightDigital,int nHeightAnalog,const CRect & rcPrint);
	static void Release(Curve * pCurve,int nCurveCount);
	
	virtual void ChangePrintCurve(Curve * pCurve,int nCurveCount,CRect rcPrint,double dPrintTime,char cPS);
	void PrintParaInfo(CDC *pDC,const CRect & rcPrint,int nParaInfoIndex);
	void PrintChannelInfo(CDC *pDC,const CRect & rcPrint,int nInfoIndex);
	void PrintCurve(CDC *pDC,const CRect & rcCurve,int nCurveIndex);
	void	OnReleaseAngle();


	COLORREF m_colorGirdLine;                //格子线的颜色
	//水平方向最小化放缩系数
	double m_dMinMulti;
	//显示曲线的颜色数据，如果通道数多于COLOR_NUM，将循环显示颜色
	COLORREF m_colorArray[COLOR_NUM];
	CRect m_clientRc;      //记录所有曲线全部绘完后的画布大小
	double m_dNewMulti;      //记录即将显示的倍率
protected:
	//为了便于计算频率，相位角，我们在计算时，认为频率在[m_dMinFreq,m_dMaxFreq]
	//的曲线才是合法正弦曲线，它的频率，相角，幅值才有意义,如果在此范围之外，将认为相角频率，幅值为0
	double m_dMaxFreq;
	double m_dMinFreq;
	//用于记录鼠标左击点在时间坐标轴中的逻辑值，由于是逻辑值，
	//所以鼠标左击产生的时间基线在滚动时从视觉上看是从界面上消失了，但当滚动会来时，用户又能看到它的存在，
	//这样设计主要是为了让测量时间差时不受屏幕的显示的限制
	double m_dValueOfClick;
	int m_nHeightOfAnalogCoor;    //模拟量通道坐标系的高度
	int m_nHeightOfDigitalCoor;   //开入开出量通道坐标系的高度
	int m_nFirstDisCurve;         //显示区域中第一个需要显示的坐标系序号，从0开始
	int m_nLastDisCurve;          //显示区域中最后一个需要显示的坐标系序号，从0开始
	CRect m_curRect;              //当前的显示区域
protected:
	
	BOOL m_bMultiColorDis;
	CRect m_rcPrintCurve;
	void DrawScale(CDC *pDC , bool isLabel , int dota);
	const int m_nHeightOfScale;           //一般刻度线的高度
	const int m_nHeightOfRCScale;         //整个刻度尺区域的高度
	const int m_nMinWidthOfDisScale;      //最小刻度在显示器上的显示间距
	const int m_nMinWidthOfLogScale;      //最小刻度在逻辑坐标系中所表示的最大逻辑值
	const int m_nMinGirdWidth;            //格线最小的宽度间距
	const int m_nMinGirdHeight;           //格线最小的高度间距，在当前版本中，所有横向格线都以最小高度间距绘制
	double m_dInitDisPosOfx;              //初始显示曲线时最左边显示的X坐标
	double m_dWindowSizeOfX;              //用于记录一屏能显示的多长时间的波形曲线，单位ms
	//2016-10-29  pwq  画事件标记
	virtual void	DrawEventFlag(CDC *pDC);
	//画事件提示
	virtual void	DrawEventToolTips(CDC *pDC);


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCurveView)
	afx_msg void OnFilePrintSetup();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAchromatic();
	afx_msg void OnDyeing();
	afx_msg void OnShowValid();
	afx_msg void OnShowSample();
	afx_msg void OnShowAll();
	afx_msg void OnCurveDisSet();
	afx_msg void OnExportWareFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    double Interpolation(double MouseLogicPointX, int MeasurePointIndex, int arrayCnt,Point *pMeaPoints ) const;
private:
	Curve * m_pOldCurve;
	char m_cOldPS;
	int m_nOldCurveCount;
	BOOL m_bPrint;
	//起始段相角差
	double	*lpStaPhaseAngle;
	//返回段相角差
	double	*lpRetPhaseAngle;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVEVIEW_H__CC9F0AEB_E8A4_423F_8EDA_92FDCC8DBAE6__INCLUDED_)
