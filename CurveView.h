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
#define CELL_COUNT 20    //1�����߿��
#define EACH_SCROLL_LEN   3     //ÿ�ι������̵ļ���3����֮һ
#define CHANNEL_INFO_WIDTH 230     //���ͨ����Ϣ����ʾ���
#define CHANNEL_INFO_WIDTH_PRINT  4500  //�ڴ�ӡ��ͼ�����ͨ����Ϣ���
#define DIGITAL_HEIGHTH_COONDIATE 20     //������������ĸ߶�
#define ANALOG_HEIGHTH_COONDIATE  60   //ģ������������߶�
#define MAX_COONDIATE_NUM 15          //������ʾ������
#define Precision (1e-8)
#define DISABLED_COLOR  RGB(225,225,225)
#define MAX_H_MULTI_NUM 10

#define MIN_NUM_INTERVAL_OF_ZERO   4    //��ʾÿ�ܲ���������������4*2

#define COLOR_NUM      12


#define PRINT_TYPE_FILE    0x01
#define PRINT_TYPE_CURVE   0x02
#define PRINT_TYPE_CHANNEL 0x03
#define PRINT_TYPE_PARA    0x04

#define CURVE_DISPLAY     0X01
#define CURVE_ANALYSE_SYNC 0X02   //��������40A,40B¼��������

#define VALID_VALUE_SHOW 0x01
#define SAMPLE_VALUE_SHOW 0x02
#define ALL_VALUE_SHOW 0x03

#define MAXNUM_CHANANL  80  //���¼��ͨ����Ŀ

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
	double extendValue; //��¼�������У�extendValue���ɲ�������������˲ʱֵ����y�Ǳ�ôֵ
	Point(){x=0;y=0;extendValue=0;}
	Point(double _x,double _y);
	Point & operator = (const Point & pt);
	bool operator <(Point p);
	Point operator -(const Point p);
	bool operator ==(Point p1);
};
struct Curve
{
	Point *lpStaPoint;           //ȫ�������β���������
	Point *lpRetPoint;           //ȫ�̷��ضβ���������
	Point *lpStartSecPoint;           //��ʼ�ε�����
	Point *lpReturnSecPoint;           //���ضε�����
	double startTime;         //����¼��ʱ�䣬��������msΪ��λ����ʾ�����һ���������ʱ����
	long StaCount;               //���ߵ�ȫ�������ε���
	long RetCount;               //���ߵ�ȫ�̷��ضε���
	long startCount;			//���߲��������ε���
	long returnCount;			//���߲������ضε���
	long nPointInCircle;  		//ģ���������ڼ���Ƶ����λ�ǵȵȣ�����������ʱû����
	COLORREF color;   			//������ɫ������ϵͳĬ��
	double valueOfCellY;  		//Y����һ���̶ȵĵ�λ,�û�����������Ϊ0��ʾ��ϵͳ����
	double minRange;      //������ʾʱ����ϵ��ʾ�߼�ֵ��ֵ�ķ�Χ��С����С��minRange
	CString name;     //ͨ������
	CString unit;            //��λ����������ȣ���������
	double fmaxOfY;
	COleDateTime firstSampleTime;     //��һ��������ʱ��
	int milliSecondOfFirst;           //��һ���������ms��
	bool bIsDigital;                  //�Ƿ�Ϊ���뿪����
	bool bIsOverlap;                  //�Ƿ�����ʼ�κͷ��ض�
	double dTurnAngle;                //ת�ǣ���λ��
	int nSubItemIndex;                //�������
	int nChannelIndex;                //nSubItemIndex�����е�ͨ�����z
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
	BOOL bPrintSet;   //���ߴ�ӡ�����Ƿ����ù�
	BOOL bAnalyse;
	BOOL bPrintFileInfo;
	BOOL bPrintChannelInfo;
	BOOL bPrintParaInfo;
	BOOL bHPrint;     //�Ƿ�����ӡ
	BOOL bMultiColor;  //�Ƿ�Ϊ��ɫ��ӡ
	BOOL bPrintPageNum; //�Ƿ��ӡҳ��
	char cPS;         //һ��ֵ���Ƕ���ֵ
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
struct WaveInfo //¼���ļ�������Ϣ
{
	float SumCycleCount;//�������ܲ���
	float SamStaCycleCount;//��ʼ�������ܲ���
	float SamRetCycleCount;//���ز������ܲ���
	float AvaStaCycleCount;//��ʼ��Ч���ܲ���
	float AvaRetCycleCount;//������Ч���ܲ���
	int   lbVer; //¼���ļ���Լ�汾��
};
class CCurveView : public CMyView
{
protected:
	CCurveView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCurveView)

// Attributes
public:
	virtual void ChangeCurve(Curve *pCurve,long count,bool isUnitedFile,int diffMilli,char ps);  //����������ʾ����
	//����Ƶ�ʣ���ֵ����λ��
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
	 //��������	
	virtual void DrawLine(CDC *pDC);           
	//���������ߣ����������ʾ������Ҫ��ʾ����ʼ����ֹ����ϵ�ı�ţ���0��ʼ��
	virtual void DrawGridLine(CDC *pDC);        
	virtual void DrawCurve(CDC *pDC);  
	//����ģ����ͨ���Ϳ��뿪����ͨ��������߶ȣ��������ʾ�������ߵ��߼������Ĵ�С
	virtual void SetCoorHeight(int nDigitalHeight = DIGITAL_HEIGHTH_COONDIATE,
		int nAnalogHeight = ANALOG_HEIGHTH_COONDIATE);
	virtual ~CCurveView();
	//Ҫ��ʾ���߼�����ת��Ϊ��ʾ�������ʾ����
	POINT MapPoint(Point p1, POINT &p2,double cellOfY,POINT origitalPoint,int nWidthOfCell);  
    //����ʾ�������ʾ����ת��Ϊ�߼����꣨��ʾ����������Ϊ��λ��
	Point MapPointToLogic(POINT p1, Point &p2,double cellOfY,POINT origitalPoint,int nWidthOfCell);  
	void ScrollByTime(const COleDateTime &DataTime,const int Milli);// ������ָ��λ��
	
	void SetMulti(CDC *pDC);            //�ı���ʾ����
	long m_nCurveCount;  //��¼���ߵ�����
	Curve * m_pCurve;  //��¼���������ָ��
	CurvePrintInfo m_curvePrintInfo;
	//void Init(Curve *pCurve,long count);
	void DrawBackBitmap( CDC* pDC );//2015-1-14 add ��ӱ���ͼ����
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
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);  //���ڹ���ʱ�Ļ����ƶ�
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strValueType;      //һ��ֵ�����ֵ
	COLORREF m_coordinateColor;  //�������ߵ���ɫ
	COLORREF m_crossLineColor; //��������ƶ������ߵ���ɫ
	COLORREF m_colorStartLine; //����¼��ʱ���ߵ���ɫ
	//UINT m_timer;   //1��ʱ�Ķ�ʱ��
	
	CPoint m_mousePoint;  //��¼WM_MOUSEMOVE�¼�ʱ��λ�ã����ڼ�¼����λ��

	long m_nMaxOfX;       //��¼����������X����
	CRect	m_rectCurve;        //��¼���߻��Ƶ����򣬰���ˮƽ�����������߻�������
	double m_dMulti;         //��¼��ǰ����ʾ����
	
	int m_arrPageHeadInfo[10];      //���ֱ�ʾ��ӡʱҳ�׵���Ϣ���ͣ����ֱ�ʾ������Ϣ�ĵڼ�����λ��
	long m_nCellOfX;       //��¼X����һ���̶�����ʾ��X���߼�ֵ
	COLORREF m_bkColor;    //��¼������ʾ����ı���ɫ
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
	int intersect;        //�ཻ
	int m_diffMilli;
	static BOOL m_isFirstShow;
	bool m_isUnitedFile;             ///��Ϊ���κϲ�����ļ�������ǣ�����ֵΪ1,����Ϊ0 
	//Ϊstr����ΪnCount,�����ÿո񲹳䣬��nCountΪ��ʱ����str���油�䣬��nCountΪ��ʱ����str��߲��� 
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


	COLORREF m_colorGirdLine;                //�����ߵ���ɫ
	//ˮƽ������С������ϵ��
	double m_dMinMulti;
	//��ʾ���ߵ���ɫ���ݣ����ͨ��������COLOR_NUM����ѭ����ʾ��ɫ
	COLORREF m_colorArray[COLOR_NUM];
	CRect m_clientRc;      //��¼��������ȫ�������Ļ�����С
	double m_dNewMulti;      //��¼������ʾ�ı���
protected:
	//Ϊ�˱��ڼ���Ƶ�ʣ���λ�ǣ������ڼ���ʱ����ΪƵ����[m_dMinFreq,m_dMaxFreq]
	//�����߲��ǺϷ��������ߣ�����Ƶ�ʣ���ǣ���ֵ��������,����ڴ˷�Χ֮�⣬����Ϊ���Ƶ�ʣ���ֵΪ0
	double m_dMaxFreq;
	double m_dMinFreq;
	//���ڼ�¼����������ʱ���������е��߼�ֵ���������߼�ֵ��
	//����������������ʱ������ڹ���ʱ���Ӿ��Ͽ��Ǵӽ�������ʧ�ˣ�������������ʱ���û����ܿ������Ĵ��ڣ�
	//���������Ҫ��Ϊ���ò���ʱ���ʱ������Ļ����ʾ������
	double m_dValueOfClick;
	int m_nHeightOfAnalogCoor;    //ģ����ͨ������ϵ�ĸ߶�
	int m_nHeightOfDigitalCoor;   //���뿪����ͨ������ϵ�ĸ߶�
	int m_nFirstDisCurve;         //��ʾ�����е�һ����Ҫ��ʾ������ϵ��ţ���0��ʼ
	int m_nLastDisCurve;          //��ʾ���������һ����Ҫ��ʾ������ϵ��ţ���0��ʼ
	CRect m_curRect;              //��ǰ����ʾ����
protected:
	
	BOOL m_bMultiColorDis;
	CRect m_rcPrintCurve;
	void DrawScale(CDC *pDC , bool isLabel , int dota);
	const int m_nHeightOfScale;           //һ��̶��ߵĸ߶�
	const int m_nHeightOfRCScale;         //�����̶ȳ�����ĸ߶�
	const int m_nMinWidthOfDisScale;      //��С�̶�����ʾ���ϵ���ʾ���
	const int m_nMinWidthOfLogScale;      //��С�̶����߼�����ϵ������ʾ������߼�ֵ
	const int m_nMinGirdWidth;            //������С�Ŀ�ȼ��
	const int m_nMinGirdHeight;           //������С�ĸ߶ȼ�࣬�ڵ�ǰ�汾�У����к�����߶�����С�߶ȼ�����
	double m_dInitDisPosOfx;              //��ʼ��ʾ����ʱ�������ʾ��X����
	double m_dWindowSizeOfX;              //���ڼ�¼һ������ʾ�Ķ೤ʱ��Ĳ������ߣ���λms
	//2016-10-29  pwq  ���¼����
	virtual void	DrawEventFlag(CDC *pDC);
	//���¼���ʾ
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
	//��ʼ����ǲ�
	double	*lpStaPhaseAngle;
	//���ض���ǲ�
	double	*lpRetPhaseAngle;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVEVIEW_H__CC9F0AEB_E8A4_423F_8EDA_92FDCC8DBAE6__INCLUDED_)
