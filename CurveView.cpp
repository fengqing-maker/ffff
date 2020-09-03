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

#define  PI 3.1415926 //��
extern CMySID_2FYApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CCurveView
//�жϸ��������Ƿ�Ϊ0
long DoubleCmp(double d)
{
	//1e-8=10�ĸ�8�η�
	if(fabs(d)<Precision) return 0;
	return (d>0)?1:-1;
}
Point::Point(double _x,double _y)
{
	x=_x;
	y=_y;
}
//����<���ڶ�������
bool Point::operator <(Point p)
{
	if(DoubleCmp(x-p.x)<0) return true;
	return false;
}
//���� == ���ڶ�������
bool Point::operator ==(Point p)
{
	if(DoubleCmp(x-p.x)==0) return true;
	return false;
}
//����-���ڶ�������
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
��ɱ����ĳ�ʼ��
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
	m_colorArray[0]=RGB(0,0,0);          //��ɫ�������Դ
	m_colorArray[1]=RGB(0,255,255);      //���ز����ǲ��ϵͳ���ת��
	m_colorArray[2]=RGB(255,0,255);      //���ز��US
	m_colorArray[3]=RGB(128,0,0);        //���ز��Ug
	m_colorArray[4]=RGB(128,128,0);      //��բ����
	m_colorArray[5]=RGB(0,128,0);        //��ѹ
	m_colorArray[6]=RGB(128,128,255);    //��ѹ
	m_colorArray[7]=RGB(0,128,128);      //����
	m_colorArray[8]=RGB(128,0,128);      //���ز��USƵ��
	m_colorArray[9]=RGB(0,255,128);      //���ز��UgƵ��
	m_colorArray[10]=RGB(0,128,255);     //
    m_colorArray[11]=RGB(0,0,255);       //
	m_colorStartLine = RGB(255,0,0);     //��ɫ������¼���ߵ���ɫ
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
��Ϣ����ļ�������
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
	
	ON_COMMAND(IDM_EXPORT_WARE_TEXT, OnExportWareFile) //2016-5-5 add ���������ļ�Text
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveView drawing
/***********************************
״̬��������
***********************************/
void CCurveView::OnInitialUpdate()
{
	//����������״̬����
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
�����̵İ�����alt F10���µ�ʱ��
************************/
void CCurveView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	switch(nChar)
	{
	case VK_LEFT://���¼������������������ƶ���x=m_mousePoint.x��������һ������һ�����ص�ƽ��
		if(m_mousePoint.x >CHANNEL_INFO_WIDTH)
		{
			m_mousePoint.x--;
		}
		else if(m_mousePoint.x<=CHANNEL_INFO_WIDTH)
		{
			m_mousePoint.x=CHANNEL_INFO_WIDTH;
		}
		break;
	case VK_RIGHT://���¼����ҷ��������������ƶ���x=m_mousePoint.x��������һ������һ�����ص�ƽ��
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
	case VK_RETURN://���¼���enter��������ʱ��Ļ����ƶ���m_mousePoint.x��
		{
			//���߻�������Ĵ�СΪm_rectCurve��230,20,1116,1120����
			//����һ���ṹ��
			Point pLogic;
			POINT origital;
			origital.x = m_rectCurve.left;
			origital.y = m_nHeightOfAnalogCoor/2;
			m_mousePoint.x += m_curRect.left;
			if(m_mousePoint.x<CHANNEL_INFO_WIDTH)
			{
				m_mousePoint.x=CHANNEL_INFO_WIDTH;
			}
			//��ȡ����ĺ�����
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
��������MapPointToLogic
����������ʾ�������ʾ����ת��Ϊ�߼����꣨��ʾ����������Ϊ��λ��
������p1[in]:  p2[out]: cellOfY[in]:  nWidthOfcell[in]:   
����ֵ
***********************************************************************************/
Point CCurveView::MapPointToLogic(POINT p1, Point &p2,double cellOfY,POINT origitalPoint,int nWidthOfCell)
{
	int xWidthOfCell;
	if(m_bPrint)
	{
		//3 60 20CELL_COUNT 1����̶��ߵĿ��
		xWidthOfCell = abs(m_nHeightOfAnalogCoor/CELL_COUNT);
	}
	else
	{
		//60
		xWidthOfCell=ANALOG_HEIGHTH_COONDIATE/CELL_COUNT;
	}
	//m_dMulti������m_nCellOfX=X����һ���̶ȵ��߼�ֵ
	//p2.x��ǰ���߼�ֵ������̶ȵĸ���Ȼ����Կ̶ȵ��߼�ֵ
	p2.x=(double)(p1.x-origitalPoint.x)/(xWidthOfCell*m_dMulti)*m_nCellOfX;
	//��ôֵ
	p2.y=(double)(origitalPoint.y-p1.y)/nWidthOfCell/cellOfY;
	return p2;
}
/********************************
��һ����ʾ¼���ļ�����¼���ļ��л� ��Ҫ�ǽ������ݵĿ���
********************************/
void CCurveView::ChangeCurve(Curve *pCurve,long count,bool isUnitedFile,int diffMilli,char ps)
{
	//�ж��Ƿ���1��ֵ����2��ֵ
	ASSERT(ps=='p'||ps=='s'||ps=='P'||ps=='S');
	if(ps=='p'||ps=='P') m_strValueType=_T("һ��ֵ");
	else m_strValueType=_T("����ֵ");
	//�Ƿ�Ϊ�ϲ�����
	m_isUnitedFile=isUnitedFile;
	//����ʲô��
	m_diffMilli=diffMilli;
	//�����Ƶ�ʣ�����ͽǶ�
	double freq,swing,angle;
	int angleIndex = -1;
	//600���ṹ����������洢����˲ʱֵ������ֵ���߼�ֵ
	Point AngleAnalyse[2][300]; //3S����Чֵ�ռ�
	//Ϊʲô��32
	int   halfPointCnt = 32;
	//��պ���
	CCurveView::Release(m_pCurve,m_nCurveCount);
	OnReleaseAngle();
	//��ȡ��ܵ�����
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	
	int i,j,temp_n;
	
	CString str;
	//������������
	queue<int> qVolIndex,qCurrentIndex;
	//����һ�ѵ�double������
	double dMaxOfX=0,dMaxVolY=0,dMaxCurrentY=0,dMaxVal,dVolMinRange,dCurrentMinRange;
	//�ַ�������
	wchar_t cUint;
	double dValueOfCellY;
	BOOL isRecord = FALSE,isShowCurveMZ=false;
	i = 0;	
	//����ṹ��ĵ�һ����Ա�ĵ�λ��Ϊ��
	//�Ƿ���ʾ�����ѹ/�ǲ�
	if (pCurve[0].unit != _T(""))
	{
		//��һ����Ա�ĵ�λ�����һλ�ǲ���"V"
		cUint = pCurve[0].unit[pCurve[0].unit.GetLength()-1];
		if(cUint == TEXT('v') || cUint == TEXT('V'))
		{
			//�ṹ��ĵ�һ����Ա�͵ڶ�����Ա�ĵ�λ�����Ƿ�һ��
			if (pCurve[0].unit == pCurve[1].unit)
			{
				i = 1;
				count += 1;
				//
				isShowCurveMZ = TRUE;
			}
		}
		//��2����Ա�ĵ�λʱ����ڵ�������Ա�ĵ�λ
		else if (pCurve[1].unit == pCurve[2].unit)
		{
			//����ڶ�����Ա�ĵ�λ��Ϊ��
			if (pCurve[1].unit != _T(""))
			{
				//��ȡ��λ�������е����һλ
				cUint = pCurve[1].unit[pCurve[1].unit.GetLength()-1];
				//������һλΪ"V"
				if(cUint == TEXT('v') || cUint == TEXT('V'))
				{
					i = 1;
					count += 1;
					isShowCurveMZ = TRUE; //��ʾ�����ѹ
				}	

			}
		}
	}
	//ͨ�������¼��43������
	m_nCurveCount = count;
	//�൱�ڿ���һ���ṹ��ָ�����ݣ�����洢�Ķ��ǽṹ��
	m_pCurve = new Curve[m_nCurveCount];
	//���ڲ������������������⣬��50����ֻ����49���ߣ��˴���Ҫ51�����������50�ߣ�����ͬ��
	for(int curveIndex = 0;i < m_nCurveCount;i++ , curveIndex++)
	{
		//���۲쵽�Ǳ�cuiveIndex��1
		//��ʼ����������51
		m_pCurve[i].lpStaPoint=new Point[pCurve[curveIndex].StaCount+1];
		//���ز���������5951
		m_pCurve[i].lpRetPoint=new Point[pCurve[curveIndex].RetCount+1];	
		//��ʼ�ϵ�����0,1600,800
		m_pCurve[i].lpStartSecPoint=new Point[pCurve[curveIndex].startCount];
		//���ضε�����0,4800,240
		m_pCurve[i].lpReturnSecPoint=new Point[pCurve[curveIndex].returnCount];
		//������ٿռ�ʧ�ܣ�������ÿռ�
		if(m_pCurve[i].lpStaPoint == NULL)
		{
			CCurveView::Release(m_pCurve,m_nCurveCount);
			m_pCurve = NULL;
			return;
		}
		//ȫ�������ε���50
		m_pCurve[i].StaCount=pCurve[curveIndex].StaCount;
		//ȫ�̷��ضε���5950
		m_pCurve[i].RetCount=pCurve[curveIndex].RetCount;
		//�������5,0,1
		m_pCurve[i].nSubItemIndex = pCurve[curveIndex].nSubItemIndex;		
		//ͨ�����5,0,1
		m_pCurve[i].nChannelIndex = pCurve[curveIndex].nChannelIndex;
		//��һ��������ʱ��
		m_pCurve[i].firstSampleTime=pCurve[curveIndex].firstSampleTime;
		//��һ���������ms��
		m_pCurve[i].milliSecondOfFirst=pCurve[curveIndex].milliSecondOfFirst;
		//����¼��ʱ��
		m_pCurve[i].startTime=pCurve[curveIndex].startTime;
		//���������ε���
		m_pCurve[i].startCount = pCurve[curveIndex].startCount;
		//���߷��ضε���
		m_pCurve[i].returnCount = pCurve[curveIndex].returnCount;
		//������ɫ
		m_pCurve[i].color=m_colorArray[i%COLOR_NUM];
		//��С���߼�����ֵ
		m_pCurve[i].minRange=pCurve[curveIndex].minRange;
		//�Ƿ��п��뿪����
		m_pCurve[i].bIsDigital = pCurve[curveIndex].bIsDigital;
		//�Ƿ�����ʼ�Σ����ض�
		m_pCurve[i].bIsOverlap = pCurve[curveIndex].bIsOverlap;
		//����Ƶ�ʺ���λ��0,64,64,32
		m_pCurve[i].nPointInCircle = pCurve[curveIndex].nPointInCircle;	
		//ͨ������
		m_pCurve[i].name = pCurve[curveIndex].name;
		//��λ
		m_pCurve[i].unit =  pCurve[curveIndex].unit;
		//��������Ϊ0����Ĭ��ϵͳ����
		m_pCurve[i].valueOfCellY = pCurve[curveIndex].valueOfCellY;
		
		//����о����ǿ��ٿռ�͸�ֵ
		
		dMaxVal = 0;
		int sampleRetBegin=0;
		//�����Ǽ��㷵�ضο�ʼ��ʾ��ʱ��
		if(m_diffMilli>0)
		{
			//�ཻ
			intersect=1000;
			m_dota=m_diffMilli;
			str.Format(_T("��t =%dms"),m_dota);
			m_strDota=str;
			m_isLabel=true;
			//58500   pCurve[1].nPointInCircle=64
			sampleRetBegin = 60000 - pCurve[1].returnCount*1.0/pCurve[1].nPointInCircle*20;
		}
		else if (pCurve[2].nPointInCircle != 0 )
		{//���ضο�ʼ��ʾ��ʱ��
			sampleRetBegin = pMainFrm->m_wareFileData.m_nCycleCount*20- pCurve[2].returnCount*1.0/pCurve[2].nPointInCircle*20;
		}
		else if ( pCurve[1].nPointInCircle != 0 )
		{//2015-1-30 add �������ʾ�ǲ�������ѹug����pCruve[1]
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

		//���ｫ���������ĵ�ѹ�����������ʹ��ͬ����Y����̶ȱ�ȣ���Ϊ�����û��ӽ����Ͼ��ܿ����������ߵ�
		//��ѹ������������Ĵ�С������ֻ�п��뿪�����Ż�����Y���ϵ�λ�̶���������߼�ֵ��valueOfCellY����
		//ģ������valueOfCellYΪ0��Ҳ����δ���ã�һ������ϵ�У�Y������������ʮ����λ�̶ȣ�ÿ����λ�̶�
		//���Ա�ʾһ�����������߼�ֵ�����Ժ�С�����ݣ�Ҳ�����ڽ���õ��Ŵ���ʾ��minRange��������������ϵ
		//����ʾ�߼�ֵ��Χ����Сֵ
		//�Ƿ��п��뿪����
		//�ǿ��뿪����
		if(m_pCurve[i].bIsDigital)
		{
			//����ȫ�������ε���
			for(j=0;j<pCurve[curveIndex].StaCount;j++)
			{
				//��ԭ���Ľṹ��0��ȫ�������β��������飬��ֵ���½ṹ��1����
				m_pCurve[i].lpStaPoint[j]=pCurve[curveIndex].lpStaPoint[j];
				//��������
				if(m_pCurve[i].lpStaPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
				}
			}
			//lpStaPoint��������һ��ֵ��ǰ���ֵһ����������������Xֵ��+10
			m_pCurve[i].lpStaPoint[j]=m_pCurve[i].lpStaPoint[j-1];
			m_pCurve[i].lpStaPoint[j].x+=10;
			//����ȫ�̷��ضε���
			for(j=0;j<pCurve[i].RetCount;j++)
			{
				//�µĽṹ���retpoint����ԭ�������ݣ���������X�Ļ����ϼ����ཻ�������ĸ���
				m_pCurve[i].lpRetPoint[j]=pCurve[curveIndex].lpRetPoint[j];
				m_pCurve[i].lpRetPoint[j].x+=intersect;
				//��������
				if(m_pCurve[i].lpRetPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpRetPoint[j].x;
				}
			}
			//RetCount��������һ��ֵ��ǰ���ֵһ����������������Xֵ��+10
			m_pCurve[i].lpRetPoint[j]=m_pCurve[i].lpRetPoint[j-1];
			m_pCurve[i].lpRetPoint[j].x+=10;
			//���߲��������ε���
			for (j=0;j<pCurve[curveIndex].startCount;j++)
			{
				m_pCurve[i].lpStartSecPoint[j]=pCurve[curveIndex].lpStartSecPoint[j];
				//��������
				if(m_pCurve[i].lpStartSecPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpStartSecPoint[j].x;
				}
			}			
			//���߲������ضε���
			for (j=0;j<pCurve[curveIndex].returnCount;j++)
			{
				//lpReturnSecPoint��ԭ���Ļ����ϼ����ཻ�ĵ�����ʱ��
				m_pCurve[i].lpReturnSecPoint[j]=pCurve[curveIndex].lpReturnSecPoint[j];
				//59500=58500+1000��intersect��
				m_pCurve[i].lpReturnSecPoint[j].x+=(intersect+sampleRetBegin);	
				//��������
				if(m_pCurve[i].lpReturnSecPoint[j].x>dMaxOfX)
				{
					dMaxOfX=m_pCurve[i].lpReturnSecPoint[j].x;
				}	
				//����к�բ�������ͨ���Ļ�
				if (pCurve[curveIndex].lpReturnSecPoint[j].y && (m_pCurve[i].name.Find(_T("��բ����")) >= 0 || m_pCurve[i].name.Find(_T("Close Outlet")) >= 0 ) && !isRecord)
				{
					isRecord = TRUE;
					//0
					m_TimeCloseOut = pCurve[curveIndex].lpReturnSecPoint[j].x+sampleRetBegin;
				}
			}
		}
		//������ǿ��뿪����
		else
		{
			//��ȡ��λ�������е����һλ
			cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
			//�����λΪ����Ļ�
			if(cUint == TEXT('a') || cUint == TEXT('A'))
			{
				//�߼�ֵ��ֵ�ķ�Χ��С����С��minRange
				dCurrentMinRange = m_pCurve[i].minRange;
				qCurrentIndex.push(i);
				//50����ȫ�������ε���
				for(j=0;j<pCurve[curveIndex].StaCount;j++)
				{
					m_pCurve[i].lpStaPoint[j]=pCurve[curveIndex].lpStaPoint[j];
					//�Ƚϴ�С
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
				//����ȫ�̷��ضε���
				for(j=0;j<pCurve[curveIndex].RetCount;j++)
				{
					//����x��Ҫ�����ཻ��
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
				//���߲��������ε���
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
				//���߲������ضε���
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
			//ֻ��ͨ��2,ͨ��3�ᴫ���ȥ ������ͨ����Ϣ�ĵ�λΪ���ص�ʱ��
			else if(cUint == TEXT('v') || cUint == TEXT('V'))
			{
				//��ȡͨ��������
				angleIndex = m_pCurve[i].nChannelIndex;//�պò���ģ����������ͨ������һus���ڶ���ug��
				//��С�߼�ֵ
				dVolMinRange = m_pCurve[i].minRange;
				//�ǵڼ���ͨ��
				qVolIndex.push(i);
				//�򵥵ĸ�ֵ����    �����ʱ�����û�нǲ����ණ��
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
				//1600���߲��������ε��� ��ʾ���������ε�����ĳ���Ϊ1600   �����ѹ�Ĳ���ֵ
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
					//�о��������ﱣ�浽�����е����ݲ�û��ʲô��
					if (j%halfPointCnt == 0)
					{//����ܲ�ȡһ�νǲ�
						AngleAnalyse[angleIndex][index].x = m_pCurve[i].lpStartSecPoint[j].x;
						AngleAnalyse[angleIndex][index].y = 0;
						//3��δ��ֵ�ı��������ǵڼ������ݣ�32�ı���,��ʼ�ε����飬�ε���
						if (GetFreqSwingAngle(freq,swing,angle,i,j,m_pCurve[i].lpStartSecPoint,m_pCurve[i].startCount))
						{
							AngleAnalyse[angleIndex][index].y = angle;
						}
						index++;
					}
				}
				//���߲������ضε���4800     ��ѹ����ֵ
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
			{//�Ƕȣ�Ƶ�ʵȴ���
				//���ߵ�ȫ�������ε���50   ֻ��Ҫ����ȫ�������ε�������ȫ�̷��ضε���				
				
				for(j=0;j<pCurve[curveIndex].StaCount;j++)
				{
					m_pCurve[i].lpStaPoint[j]=pCurve[curveIndex].lpStaPoint[j];
					if(DoubleCmp(m_pCurve[i].lpStaPoint[j].x-dMaxOfX)>0)
					{
						dMaxOfX=m_pCurve[i].lpStaPoint[j].x;
					}
					if(DoubleCmp(fabs(m_pCurve[i].lpStaPoint[j].y)-dMaxVal)>0)
					{
						//����Ǻ�dMaxVal������Ϊ30
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
				//���ͨ������Ϊ���ز����ǲ�   ��ʵ֤�����ز����ǲ��ͨ������ǵ�һ��
				if ( m_pCurve[i].name.Find(_T("���ز����ǲ�")) >= 0 || m_pCurve[i].name.Find(_T("Mainboard Angle Diff.")) >= 0 )
				{
					//ȫ�������ε��� �����y���ǽǲ�  49
					for(j=0;j<pCurve[curveIndex].StaCount-1;j++)
					{
						//ǰ������ĽǶ����180�㣬����
						if (fabs(m_pCurve[i].lpStaPoint[j].y-m_pCurve[i].lpStaPoint[j+1].y)>180)
						{
							m_pCurve[i].lpStaPoint[j].x=m_pCurve[i].lpStaPoint[j+1].x;						
						}
					}
					//ȫ�̷��ضε���  1620
					for(j=0;j<pCurve[curveIndex].RetCount-1;j++)
					{
						if (fabs(m_pCurve[i].lpRetPoint[j].y-m_pCurve[i].lpRetPoint[j+1].y)>180)
						{
							m_pCurve[i].lpRetPoint[j].x=m_pCurve[i].lpRetPoint[j+1].x;
						}
					}
					//���´���û�б�����			������������
					for (j = 0; j< m_pCurve[i].startCount ; j++)
					{//�д��ڲ���ǰ��				
						m_pCurve[i].lpStartSecPoint[j].x = AngleAnalyse[1][j].x;
						m_pCurve[i].lpStartSecPoint[j].y = AngleAnalyse[0][j].y-AngleAnalyse[1][j].y; //us - ug����ʼǰ��
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
					//��������
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
					//��������
					for(j=0;j<m_pCurve[i].startCount-1;j++)
					{
						if (fabs(m_pCurve[i].lpStartSecPoint[j].y-m_pCurve[i].lpStartSecPoint[j+1].y)>180)
						{
							m_pCurve[i].lpStartSecPoint[j].x=m_pCurve[i].lpStartSecPoint[j+1].x;
						}
					}
					//��������
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
				//�����жϺ���
				if(DoubleCmp(dMaxVal-temp_n)>0)
				{
					//31
					temp_n++;
				}
				//4   y����һ���̶ȵ��߼�ֵ
				dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
				if(DoubleCmp(dMaxVal-1) ==0)
				{
					dValueOfCellY = dMaxVal/10;
				}
				//�����ǵĵ���4����ʾΪ��ϵͳ��������	y����һ���̶ȵĵ�λ
				m_pCurve[i].valueOfCellY = dValueOfCellY;
			}
		}
	}
	//��ʼ��ָ�뻺��ռ�
	int	nStaAngleCount = 0,nRetAngleCount = 0;			//����Ƕ�ֵ�Ļ���Ĵ�С
	int	nIndexChannel = 0;
	int	nCacluIndex = 1;
	for(nIndexChannel = 0;nIndexChannel < m_nCurveCount;nIndexChannel++)
	{
		if (m_pCurve[nIndexChannel].name.Find(_T("���ز����ǲ�")) >= 0 || m_pCurve[nIndexChannel].name.Find(_T("Mainboard Angle Diff")) >= 0 )
		{
			nStaAngleCount = m_pCurve[nIndexChannel].StaCount;
			nRetAngleCount = m_pCurve[nIndexChannel].RetCount;
			break;
		}
	}
	if (nStaAngleCount != 0  )
	{
		lpStaPhaseAngle = new double[nStaAngleCount*2];
		//���⴦��һ
 		lpStaPhaseAngle[0] = m_pCurve[nIndexChannel].lpStaPoint[0].y;
		//�����ݽ��д���
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
		//���⴦��һ
		lpRetPhaseAngle[0] = m_pCurve[nIndexChannel].lpRetPoint[0].y;
		//�����ݽ��д���
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
// 		strTemp.Format(_T("��%d���㣺%0.2f\r\n"),nDot,lpStaPhaseAngle[nDot]);
// 		strAngle += strTemp;
// 	}
	for (int nDot = 0 ;nDot < nRetAngleCount*2 ;nDot++)
	{
		strTemp.Format(_T("��%d���㣺%0.2f\r\n"),nDot,lpRetPhaseAngle[nDot]);
		strAngle += strTemp;
	}
	char	Buff[100000] = {0};
	//�ѻ�ȡ��������ȫ�����ı���ʽ��ȡ
	FILE	*mFile = fopen("E:\\us_ug.txt","a+");
	if (mFile == NULL)
	{
		AfxMessageBox(_T("�ļ���ʧ��"));
	}
	WideCharToMultiByte(CP_ACP,0,strAngle,-1,Buff,100000,NULL,NULL);
	fwrite(Buff,strlen(Buff),1,mFile);
	fclose(mFile);
#endif

	//�Լ���ʾ���������ζ���Ҫ��ʾ�����ѹ
	if (isShowCurveMZ)
	{
		if (!InitAddChannel())
		{
			return;
		}
	}
	//��������е�dValueOfCellY
	//����ִ��
	if(dMaxCurrentY<dCurrentMinRange)
	{
		dMaxCurrentY=dCurrentMinRange;
	}
	//100  100
	temp_n=(long)dMaxCurrentY;
	//���tempY����temp_n
	if(DoubleCmp(dMaxCurrentY-temp_n)>0)
	{
		//101
		temp_n++;
	}
	//11
	dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
	//����ִ��
	if(DoubleCmp(dMaxCurrentY-1) ==0)
	{
		dValueOfCellY = dMaxCurrentY/10;
	}
	//�������������ݣ����Բ�ִ������Ĵ�������
	while(!qCurrentIndex.empty())
	{
		m_pCurve[qCurrentIndex.front()].valueOfCellY = dValueOfCellY;
		qCurrentIndex.pop();
	}
	//�����ѹ�е�dValueOfCellY
	//����ִ��    dMaxVal=146
	if(dMaxVolY<dVolMinRange) 
	{
		dMaxVolY=dVolMinRange;
	}
	//146
	temp_n=(long)dMaxVolY;
	//���tempY����temp_n
	if(DoubleCmp(dMaxVolY-temp_n)>0)
	{
		//147��
		temp_n++;
	}
	//15
	dValueOfCellY=(long)((temp_n%10)==0)?(temp_n/10):(temp_n/10+1);
	//����ִ��
	if(DoubleCmp(dMaxVolY-1) ==0)
	{
		dValueOfCellY = dMaxVolY/10;
	}
	//����Ƶ�ʵ�ʱ�����ִ������
	while(!qVolIndex.empty())
	{
		m_pCurve[qVolIndex.front()].valueOfCellY = dValueOfCellY;
		qVolIndex.pop();
	}
	//����X������ 16709
	m_nMaxOfX=(long)dMaxOfX;
	if(m_nMaxOfX<dMaxOfX) 
	{
		//16710��
		m_nMaxOfX++;
	}
	//���������С
	//һ���ɶ����߼�ֵ
	m_dMulti=1;
	//�µ��߼�ֵ
	m_dNewMulti = 1;
	//16710
	m_dWindowSizeOfX = m_nMaxOfX;
	//��ʼ��ʾ����ʱ�������ʾ��X����0
	m_dInitDisPosOfx = 0-m_pCurve[2].startTime;
	SetCoorHeight(20,60);
	//��ȡ������Ϣ
	CRect rc;
	//��ȡ���Ե���Ļ�ֱ���
	GetDesktopWindow()->GetWindowRect(rc);
	//230 230���λ���ǻ�����
	rc.right -= WIDTH_PROJECT+CHANNEL_INFO_WIDTH+20;
	//50130
	double d1=m_clientRc.Width()-CHANNEL_INFO_WIDTH;
	//1366*16710/(50130*16710)
	//�Ŵ�ı���
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
	//����ͼ�ƶ��������ĵ�
	ScrollToPosition(tempPoint);
}
/**********************************************
1041L ��ʼ����
**********************************************/
POINT CCurveView::MapPoint(Point p1, POINT &p2,double cellOfY,POINT origitalPoint,int nWidthOfCell)
{
	//Ϊ��ͳһx���꣬�����xWidthOfCellȫ��ģ��������
	int xWidthOfCell;
	//���񲻱�ִ��
	if(m_bPrint)
	{
		//x����ÿ��̶ȵ��߼�ֵΪ3                m_nCellOfX ��¼X����һ���̶�����ʾ��X���߼�ֵ m_curRect��ǰ��ʾ������
		xWidthOfCell = abs(m_nHeightOfAnalogCoor/CELL_COUNT);
		//���ڴ����Point�ṹ���X*3*��ǰ�ı���/�߼�ֵ+��������ĵ������xֵ-��ǰ����X������ֵ+x����ֵ
		p2.x=p1.x*xWidthOfCell*m_dMulti/m_nCellOfX+origitalPoint.x-m_curRect.left+m_rcPrintCurve.left;
	}
	else
	{
		//60/20
		xWidthOfCell=ANALOG_HEIGHTH_COONDIATE/CELL_COUNT;
		//���ڴ����Point�ṹ���X*3*��ǰ�ı���/�߼�ֵ+��������ĵ������xֵ-��ǰ����X������ֵ		
		p2.x=p1.x*xWidthOfCell*m_dMulti/m_nCellOfX+origitalPoint.x-m_curRect.left;
	}
	//��ǰ����ϵ�Ŀ�����/����ֵ
	p2.y=(-1)*p1.y*nWidthOfCell/cellOfY;
	//����ĵ��Yֵ
	p2.y+=origitalPoint.y;
	//��ȡ��һ������������
	return p2;
}
/******************************************************************
1450L���ù�һ��
******************************************************************/
void CCurveView::DrawLine(CDC *pDC)
{
	POINT temp,origitalPoint;
	//���߻�����������Ͻ�
	origitalPoint.x=m_rectCurve.left;
	UINT nWidthOfCell,nTopOfCoor;  //nTopOfCoor��ǰ����ϵ�Ķ�����
	long i,j;
	CPen *pPen;
	//��ʼ������
	CPen pen(PS_SOLID,1,m_colorStartLine);
	CPen pen2(PS_DASH,1,m_colorStartLine);
	CPen pen3(PS_SOLID,1,RGB(255,0,255));
	CPen samplePen(PS_SOLID,1,RGB(224,215,27));
	CPen *pOldPen;
	Point point;
	pOldPen=pDC->SelectObject(&pen);
	//�������ߵ�����
	nTopOfCoor = m_rectCurve.top;
	CString str;
	//��ʾ�����е�һ����Ҫ��ʾ������ϵ��ţ���0��ʼ
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
	//��ǰ���߻���������ʾ�˶���������
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
		//�Ƿ�Ϊ���뿪����                   ����origitalPoint.y��¼ÿ���ߵ���ʼλ��
		//����ͨ����������ֱ��ռ����λ��
		if(m_pCurve[i].bIsDigital)
		{
			//270 290 310 .....
			origitalPoint.y = nTopOfCoor + m_nHeightOfDigitalCoor/2; 
			//1
			nWidthOfCell = m_nHeightOfDigitalCoor/CELL_COUNT;
		}
		//������ǿ��뿪����
		else
		{
			//50 110 170 230
			origitalPoint.y = nTopOfCoor + m_nHeightOfAnalogCoor/2;
			//3
			nWidthOfCell = m_nHeightOfAnalogCoor/CELL_COUNT;
		}

		int indexSta=0,indexRet=0,startP=0,returnP=0,endSta=0,endRet=0,endStart=0,endReturn=0;
		//��ǰ��ʾ�������230

		temp.x=m_curRect.left+CHANNEL_INFO_WIDTH;
		//ͼ�ϵ��߼�����ֵ
		//244L
		//����Ĳ�������ÿһ��ִ�п�ʼ���Ƶ�������
		MapPointToLogic(temp,pLogic,0,origitalPoint,0);
		//���ֲ���

		indexSta=BinarySearch(m_pCurve[i].lpStaPoint,pLogic,m_pCurve[i].StaCount+1);
		indexRet=BinarySearch(m_pCurve[i].lpRetPoint,pLogic,m_pCurve[i].RetCount+1);
		//�Ƿ�����ʼ�κͷ��ض�
		if (m_pCurve[i].bIsOverlap)
		{
			//���ֲ���
			startP=BinarySearch(m_pCurve[i].lpStartSecPoint,pLogic,m_pCurve[i].startCount);
			returnP=BinarySearch(m_pCurve[i].lpReturnSecPoint,pLogic,m_pCurve[i].returnCount);
		}
		//��ǰ��ʾ��������½ǵ�x
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
					//���ڴ�0��1���1��0�Ŀ������ĵ㣬�������ǵ��е㴦�������midPoint0,midPoint1,��仯ǰ������
					//�ֱ�Ϊp1,p2,��midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
					//p1.x)/2,����ϵͳ��˳������p1-midPoint0-midPoint1-p2
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
					{//��������Ч�η��ض��У�����һ��ֵ����ʼ�ε����һ��ֵ��һ��������һ�����ߣ�
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
					//���ڴ�0��1���1��0�Ŀ������ĵ㣬�������ǵ��е㴦�������midPoint0,midPoint1,��仯ǰ������
					//�ֱ�Ϊp1,p2,��midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
					//p1.x)/2,����ϵͳ��˳������p1-midPoint0-midPoint1-p2
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
				{//��������Ч�η��ض��У�����һ��ֵ����ʼ�ε����һ��ֵ��һ��������һ�����ߣ�
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
				/*�ڲ���������Ч���غϴ�ֻ��ʾ������*/
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
				pDC->SelectObject(&samplePen);//ʹ�û�ɫ
			}
			if (m_pCurve[i].bIsOverlap)//����ʼ�κͷ��ض�����
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
						//���ڴ�0��1���1��0�Ŀ������ĵ㣬�������ǵ��е㴦�������midPoint0,midPoint1,��仯ǰ������
						//�ֱ�Ϊp1,p2,��midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
						//p1.x)/2,����ϵͳ��˳������p1-midPoint0-midPoint1-p2
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
		//����¼������ʱ��ʱ�̵ĺ�ɫ����
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
		//���բ���������
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
		//���Ʋ���ʱ���Ļ���
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
��������SetMulti
�������ı���ʾ�ı���
������pDC[in]����ͼ���
���أ�void
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
	//IDB_BITMAP1 �Ǵ���ʾλͼ����ԴID
	BITMAP bm;
	bmp.GetBitmap(&bm);
	//int nWidth=bm.bmWidth,nHeight=bm.bmHeight;
	//Ϊͼ��mask ������һ��DC
	dcImage.CreateCompatibleDC(pDC);
	dcMask.CreateCompatibleDC(pDC);
	//��ͼ��װ��image DC
	CBitmap* pOldBitmapImage=dcImage.SelectObject(&bmp);
	//Ϊ�����롱λͼ����һ����ɫbitmap
	CBitmap bitmapMask;
	bitmapMask.CreateBitmap(nWidth, nHeight, 1, 1, NULL); //��mask λͼװ��mask DC
	CBitmap* pOldBitmapMask = dcMask.SelectObject(&bitmapMask); //��͸��ɫ���������롱λͼ
	dcImage.SetBkColor(crColour);//crColor ��λͼ�е�͸��ɫ
	dcMask.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);//��3 ������ʵ�ʵĻ���
	pDC->BitBlt(x, y, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcMask, 0, 0, SRCAND);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT); //�ָ�ԭ������
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
	UINT nWidthOfCell,nTopOfCoor;  //nTopOfCoor��ǰ����ϵ�Ķ�����
	//y��
	pDC->MoveTo(m_rectCurve.left,m_rectCurve.top);
	pDC->LineTo(m_rectCurve.left,m_rectCurve.bottom);
	//��������
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
			while(nGirdHeight <= nDisCoorHeight)//���м����������߸��������̶���
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
//������
void CCurveView::DrawCurve(CDC *pDC)
//����������᷶Χ���ƶ������ǹ����������ƶ�ʱ�������������߼��̶��ߵ��ػ�
//everytime the RedrawWindow member function is called ,do the DrawCurve movement!
{
	CMainFrame * pMainFrm = (CMainFrame *)GetParentFrame();
	int totalTime = pMainFrm->m_wareFileData.m_nCycleCount * 20;				//�ܵĲ���ʱ�䣺�ܲ��� * ����
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
	CDC MemDC; //���ȶ���һ����ʾ�豸����
	CBitmap MemBitmap;//����һ��λͼ����
	CSize logicTotalSize;
	logicTotalSize = GetTotalSize();
	logicTotalSize.cx = (logicTotalSize.cx > 2*m_curRect.Width())?2*m_curRect.Width():logicTotalSize.cx;
	CRect rcDC(0,0,logicTotalSize.cx,logicTotalSize.cy);
	//���������Ļ��ʾ���ݵ��ڴ���ʾ�豸
	VERIFY(MemDC.CreateCompatibleDC(NULL));
	//��ʱ�����ܻ�ͼ����Ϊû�еط��� ^_^
	//���潨��һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С
	VERIFY(MemBitmap.CreateCompatibleBitmap(pDC,rcDC.Width(),rcDC.Height()));
	//��λͼѡ�뵽�ڴ���ʾ�豸��
	//ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ��
	CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);
	//MemDC.SelectObject(pOldBit);
	//���ñ���ɫ��λͼ����ɾ����������õ��ǰ�ɫ��Ϊ����
	//��Ҳ�������Լ�Ӧ���õ���ɫ
	MemDC.FillSolidRect(rcDC,RGB(255,255,255));
// 	MemDC.FillSolidRect(rcDC,RGB(0,0,0));//DEBUG-TEST-ZYS

	MemDC.FillSolidRect(m_rectCurve,m_bkColor);
	DrawBackBitmap(&MemDC);
	//��ͼ
	oldmode=MemDC.SetBkMode(TRANSPARENT);
	//�˴����������ߣ��ͻ�����ߵĵ���
	SetMulti(&MemDC);
	MemDC.SetBkMode(oldmode);
	int startSec,returnSec,staSec,retSec; //retSec = ��������ʱ�䣬staSec = ����ǰ��ʱ��
	CFont font,*pOldFont;
	font.CreatePointFont(100,_T("����"));
	pOldFont=MemDC.SelectObject(&font);
	CString str=_T("��  ��:");
	int i;
	CSize size;
	size=MemDC.GetTextExtent(str);
	//�������������������ڣ�����Ҫ���Ƹ�����������x=m_mousePoint.x,����Ҫ�������
	//m_nFirstDisCurve��m_nLastDisCurveͨ����˲ʱֵ��ģ����ͨ������λ��,���л��������
	//���ߵ�ʱ���͵�ǰ����ߵľ�ȷʱ��ֵ(����������ʱ����)
	CString info;
	if(m_mousePoint.x>0)
	{
 		CPen crossPen(PS_SOLID,1,m_crossLineColor);
 		pOldPen=MemDC.SelectObject(&crossPen);
		MemDC.MoveTo(m_mousePoint.x-m_curRect.left,m_rectCurve.top);
		MemDC.LineTo(m_mousePoint.x-m_curRect.left,m_rectCurve.bottom);
		MemDC.SelectObject(pOldPen);
		Point pLogic;  //���ָ���ε�ʱ���(�Ӳ�����ʼ������)
		POINT origital;
		origital.x = m_rectCurve.left;
		origital.y = m_nHeightOfAnalogCoor/2;
		//����ʼʱ����ʱ���
		MapPointToLogic(m_mousePoint,pLogic,0,origital,m_nHeightOfAnalogCoor/CELL_COUNT);//��ȡ����ĺ�����
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
		str.Format(_T("%s��%.1fms"), info, milliSecond);
		((CMainFrame *)GetParentFrame())->m_wndStatusBar.SetPaneText(2,str);
		//SetStatusTextMid(str);
		long indexAvail,indexStart,indexReturn;
		UINT nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //����ϵ�Ķ�������
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
		Point  *pTempPoint = new Point[6*m_pCurve[0].nPointInCircle]; //4�ܲ���ʱ��2015-1-30 add
		for(i=m_nFirstDisCurve;i<=m_nLastDisCurve;i++)
		{
			staSec = pMainFrm->m_wareFileData.m_nAvailStartCycleCount*20;
			//lhz ȫ��������ε���ʼʱ��Ӧ��Ϊ
			//retSec = m_pCurve[i].startTime + intersect;
			retSec = staSec + intersect;
			//end 
			if (m_pCurve[i].bIsOverlap)				//������ģ�������ݵ�ʱ��ͻ�������ʼ����������ʼʱ����ʱ���������ز����εĿ�ʼʱ��
			{
				startSec  = m_pCurve[2].startCount/m_pCurve[2].nPointInCircle*20;
				returnSec = totalTime - m_pCurve[2].returnCount/m_pCurve[2].nPointInCircle*20 - m_dota + intersect; //xֵ����������ǰ�ε�ʱ��ms
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
			if(m_bMultiColorDis)//�ж��Ƿ��ɫ��ʾ
			{
				brush.CreateSolidBrush(m_pCurve[i].color);
				pOldPen=MemDC.SelectObject(&pen);
				pOldBrush=MemDC.SelectObject(&brush);
				MemDC.Ellipse(&rc);//RC�Ȼ�һ��СԲ����ע
				MemDC.SelectObject(pOldBrush);
				MemDC.SelectObject(pOldPen);
				rc.left=m_clientRc.left+20;
				rc.top=origital.y-size.cy/2;
				rc.right=m_clientRc.left+CHANNEL_INFO_WIDTH;
				rc.bottom=origital.y+size.cy;//RC����ע��ǰͨ��
			}
			else//�ڰ���ʾ
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
				bool flag=true;//��ʶ�Ƿ��ڲ���ֵ����
				wchar_t cUint = m_pCurve[i].unit[m_pCurve[i].unit.GetLength()-1];
				BOOL isAngle = FALSE;
	  
				if ( m_pCurve[i].name.Find(_T("���ز����ǲ�")) >= 0 || m_pCurve[i].name.Find( _T("Mainboard Angle Diff.") ) >= 0 )
				{
					isAngle = TRUE;
				}
				CString volStr;
				volStr.LoadString( IDS_PULSAT_VOL );		//�����ѹ
				if (m_pCurve[i].name == volStr )
				{
					info.LoadString( IDS_PERUNIT_VALUE );		//��ôֵ
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
				else			//m_pCurve[i].bIsOverlap:����ʼ�Ρ����ض���
				{
					if(pLogic.x < startSec && m_pCurve[i].bIsOverlap)			//��ʼ������
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
					if (pLogic.x > returnSec && m_pCurve[i].bIsOverlap)			//���ز�����
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
							{//��������С��2���� 
								memcpy( pTempPoint, &m_pCurve[i].lpStartSecPoint[ m_pCurve[i].startCount - 1 - tempCount ], tempCount*sizeof(pTempPoint[0]) );
								memcpy( pTempPoint + tempCount, m_pCurve[i].lpReturnSecPoint, tempCount*2*sizeof(pTempPoint[0]) );
							} 
							GetFreqSwingAngle(dFreq,dSwing,dAngle,i, indexReturn+tempCount, pTempPoint, tempCount*3 );
						}
                            
					}
					info.LoadString( IDS_SAMPLE_VALUE );			//����ֵ
					CString effValueStr, tempStr;
					effValueStr.LoadString( IDS_EFFECTIVE_VALUE );			//��Чֵ
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
	// 					str.Format(_T("\n����ֵ:%.1f%s"),m_pCurve[i].lpStartSecPoint[indexStart].y+m_average[i],
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
	// 					str.Format(_T("\n����ֵ:%.1f%s"),m_pCurve[i].lpReturnSecPoint[indexReturn].y+m_average[i],
	// 						m_pCurve[i].unit);
	// 				}
					else if (pLogic.x >= retSec)			//�о�����ʼ�����������ȫ������ִ��
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
					else if (pLogic.x < staSec)				//��ʼ�λᵽ����ִ��
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
						temp.Format(_T("%s %-8s  %.1f��"),m_pCurve[i].name,str,dAngle);
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
		UINT nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //����ϵ�Ķ�������
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
	DrawScale(&MemDC,m_isLabel,m_dota);//�ѿ̶���Ҳ����λͼ��
	//2016-10-27  pwq
	DrawEventFlag(&MemDC);
	if(m_mousePoint.x>0)
	{
		//���¼�������ʾ����
		DrawEventToolTips(&MemDC);
	}
	pDC->BitBlt(0,0,m_curRect.Width(),logicTotalSize.cy-m_curRect.top,
		&MemDC,0,m_curRect.top,SRCCOPY);
// 	pDC->BitBlt(CHANNEL_INFO_WIDTH+1,0,m_curRect.Width(),m_curRect.Height(),
// 		&MemDC,m_curRect.left+CHANNEL_INFO_WIDTH+1,m_curRect.top,SRCCOPY);
	//  ��ͼ��ɺ������
	MemBitmap.DeleteObject();
	MemDC.SelectObject(pOldBit);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	m_mousePoint.x -= pointScrollPos.x;
	m_mousePoint.y -= pointScrollPos.y;
}

//2016-9-21 addd �򵱵�����֮�����Բ�ֵ      ������˼��û�б�ʹ��
double CCurveView::Interpolation(double MouseLogicPointX, int MeasurePointIndex, int arrayCnt,Point *pMeaPoints ) const 
{

	Point point0, point1;
 	double yYalue = 0.0f;
	long ret = DoubleCmp( MouseLogicPointX - pMeaPoints[MeasurePointIndex].x );
	if ( ret > 0 && (MeasurePointIndex+1) < arrayCnt )
	{//�ڲο����ұ�
		memcpy( &point0, pMeaPoints+MeasurePointIndex, sizeof(Point) );
		memcpy( &point1, pMeaPoints+MeasurePointIndex+1, sizeof(Point) );
		//yYalue = Interpolation( MouseLogicPoint.x, point0, point1 ); 
		yYalue = ( ( point1.y - point0.y )/(point1.x - point0.x) )*( MouseLogicPointX - point0.x ) + point0.y;
	}else if( ret < 0  && MeasurePointIndex > 0)
	{//�ڲο������
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
	//������Ҫ��ֱ����ʱ��Ҫ�Ѵ�ֱ�������Ŀ�ȼӻ������򴰿ڻ��խ
	if(sizeTotal.cy>m_clientRc.Height())
	{
		sizeTotal.cx=m_clientRc.Width()+20;
	}
	else
	{
		sizeTotal.cx = m_clientRc.Width();
	}

// 	//�������ҵ�ĺ�����
	
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
	//���ߵ�����
	if(!m_bPrint && m_nCurveCount > 0 )
	{
		DrawCurve(pDC);
	}
	EndPaint(&ps);
	// Do not call CScrollView::OnPaint() for painting messages
}
//��������
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
//���ڹ���ʱ�����ƶ�
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
		//(��CScrollView::OnScrollBy����ͬ�ľ�����һ�����ã�
		//��Ϊ��εĹ����ɵ����ǵ�CCurveView::DrawCurve�Ѿ����ˣ�
		//��������ٴ��ص���ScrollWindow�����ý�����˸)
		//ScrollWindow(-(x-xOrig), -(y-yOrig));
		if (x != xOrig)
		{
			SetScrollPos(SB_HORZ, x);
		}
		if (y != yOrig)
		{
			
			SetScrollPos(SB_VERT, y);
		}
		//CScrollView::OnScrollBy��û�д˵��ã���������������ǵ�CCurveView::DrawCurveʵ�ֹ���
		::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	}
	return TRUE;
	//return CScrollView::OnScrollBy(sizeScroll, bDoScroll);
}
//����ƶ�
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
	//�������ҵ�ĺ�����
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
//�����������µ�ʱ��
void CCurveView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//������ʱ���Ļ�����Ϊx=m_mousePoint.x,��������ת��Ϊ����ϵ���߼�����
	Point pLogic;
	POINT origital;
	origital.x = m_rectCurve.left;
	origital.y = m_nHeightOfAnalogCoor/2;
	point.x += m_curRect.left;
	if(point.x>=CHANNEL_INFO_WIDTH)
	{
	MapPointToLogic(point,pLogic,0,origital,m_nHeightOfAnalogCoor/CELL_COUNT);//��ȡ����ĺ�����
	m_dValueOfClick = pLogic.x;
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	CScrollView::OnLButtonDown(nFlags, point);
	}
}
/**************************************************************************************************
��������GetFreqSwingAngle
����������Ƶ�ʡ���ֵ����λ��
������dFreq[out]��Ƶ�ʣ�dSwing[out]����ֵ�� dAngle[out]����λ��
       indexOfChannel[in]��ͨ�������к�  indexOfPoint[in]��  bFront[in]��
����ֵ������ɹ�TRUE,����FALSE
****************************************************************************************************/
//���ضε�����
BOOL CCurveView::GetFreqSwingAngle(double &dFreq, double &dSwing, double &dAngle,const int indexOfChannel, const int indexOfPoint, Point* pPoint, int count )
{
	/*CString c_infor;
	c_infor.Format(_T("%d==%d==%d"),indexOfChannel,indexOfPoint,count);
	AfxMessageBox(c_infor);*/
	int nPointInMinFreq,  //���Ƶ����ÿ�ܲ��Ĳ������� 106
	nLowPointIndex,     //��������ϵ�������ܲ�����������ʼ���������
	nIndexOfZeroPoint;  //�������������������������  Ϊ2����1����0
	int indexOfZero[3] = { -1, -1, -1};     //���ڱ��������������������Ĳ���������
	BOOL bCanSec=TRUE;      //�Ƿ񻹿����ҵ��������λ��Ƶ�ʡ���ֵ
	double dPointOfX[]={-1,-1,-1};			//��������X������ֵ
	//���Ƶ����ÿ�ܲ��Ĳ�������   106���������Ƶ�ʵ�һ�����ܲ���Ŀ����Ϊ������Ƶ���Ƿ��ϵ�ֵ��
	nPointInMinFreq =(int)(double) 50 * m_pCurve[indexOfChannel].nPointInCircle/m_dMinFreq;
	//��������ϵ�������ܲ�����������ʼ���������
	nLowPointIndex = indexOfPoint - 3*nPointInMinFreq/2;		//��ǰ��������źż�ȥһ�����ܲ��Ĳ�������	
	//�������������������������  ��Ϊ����Ŀռ��СΪ3������˵���һ�����±�Ϊ2�����ݴӺ���ǰ��
	nIndexOfZeroPoint = 2;
	//lhz 2017-1-6 moidy ������ε�����ǰһ�ܲ��ͺ�һ�ܲ��������ݣ�����ǰȡ��������㣬����ȡһ�������,2fy������
	bool isResetTargerZeroNum = FALSE;				//����ˮƽ�����
	//ʣ�µĵ㳬��һ�����ܲ�
    if ( indexOfPoint < (count - nPointInMinFreq/2) )					//���ʣ�µĵ㲻��һ�����ܲ��ˣ��ǾͿ�����Ҫ����������������	
    {
		nIndexOfZeroPoint -= 1;
        isResetTargerZeroNum = TRUE;
    }
	if (indexOfChannel == 2 && 205 == indexOfPoint )
	{
		int tmp = 0;
		tmp++;
	}
	//dPointOfX:��0��X������  indexOfZero���ǻ��������Y��ֵ
	//ȥ��ǰ��������߽��й������ң����ʣ����ܲ�����1���룬����3���㶼�������������ң���֮������������������
	FindZeroPosLeft( nIndexOfZeroPoint, indexOfChannel,  indexOfPoint, pPoint, count, indexOfZero, dPointOfX);		
	switch (nIndexOfZeroPoint)
	{
		case 2:	
			if ( !isResetTargerZeroNum )		////��ʣ�µĲ������ڲ���1.5����,����������һ����ߵĹ���㶼û���ҵ���������򣬿��Է����ˡ�
			{
				if(nLowPointIndex > 0 )			//����ֵ����һ�����ܲ�
				{
					bCanSec =FALSE;				
				}
				else							//��ֻ��˵���ܽ�������ط������жϣ������е�̡�����ôһ�������֣����Ƶ�������������֣�
				{
					nIndexOfZeroPoint = 0;		//��ȥ����ʣ�²���һ�����ܲ��ĵط���������
				}
			}
			break;
		case 1:									
			if (isResetTargerZeroNum )			//ʣ�µĵ㳬��һ��������
			{
				nIndexOfZeroPoint = 0;			//�㻹��Ҫ�������㣬Ҳ�����㻹��Ҫ��������������һ�����ܲ����ҵ����������
			}
			else								//ʣ��ĵ㲻��һ�����ܲ�								
			{
				//û���ҵ�0��
				if(nLowPointIndex > 0)         //������ǰ�泬��һ�����ܲ������������㣬���Լ��������ˣ���в���һ�����ܲ��ĵ�ȥ�������㲻��
				{
					bCanSec =FALSE;
				}
				else							//�����������Ƶ���²����������ڵĲ��Σ�Ȼ���㻹��Ҫ�ҵ�������
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
			if ( isResetTargerZeroNum )			//ʣ�µĵ㳬��һ�����ܲ�
			{
				nIndexOfZeroPoint = 1;						//�㻹��Ҫ��2����
				indexOfZero[0] = indexOfZero[1];
				indexOfZero[1] = -1;
				dPointOfX[0] = dPointOfX[1];
			}
			else						//δ����ĵ㲻��һ�����ܲ�
			{
				if(nLowPointIndex > 0)	//ǰ��һ�����ܲ���û���ҵ����ʵĵ㣬����ʣ�µĵ㲻��һ�����ܲ��㻹��ʲô
				{
					bCanSec =FALSE;
				}
				else					//�������̶̵ֶ���
				{
					dPointOfX[0] = dPointOfX[1];				//���ҵ����������ǰ��
					indexOfZero[0] = indexOfZero[1];
					dPointOfX[1] = dPointOfX[2];
					indexOfZero[1] = indexOfZero[2];
					dPointOfX[2] = -1;
					indexOfZero[2] = -1;
					nIndexOfZeroPoint = 2;						//ʣ�µĵ㲻��һ���������㻹����һ����
				}
			}
			break;
		default:	
			ASSERT(nIndexOfZeroPoint == -1);	//�Ѿ�������ҵ���Ӧ���ҵ��ĵ�����ʣ�µĵ㲻��һ�������ھ���3���㣬����һ�������ھ���2����
			if ( isResetTargerZeroNum )			//�����������
			{
				nIndexOfZeroPoint = 2;
			}
			else
			{
				nIndexOfZeroPoint = 3;			//�����������
			}
			break;
	}
	//��������ˣ����ǻ�û���ҵ�������ʱ����Ҫ�Ӳ��յ��ұ���
	if(nIndexOfZeroPoint < 3 && bCanSec)					
	{
		//ȥ�ұ������
		bCanSec = FindZeroPosRight( nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count, indexOfZero, dPointOfX);
		if ( FALSE == bCanSec )
		{//�������ĵ��Ҳ�������ȫ������㣬���ܲ�ǰ��  ����Ѱ��3�������
			nIndexOfZeroPoint = 2;				//��Ȼ�������������ղ���3�����㣬��ô�ͳ��Դ����������
			bCanSec = FindZeroPosLeft( nIndexOfZeroPoint, indexOfChannel,  indexOfPoint, pPoint, count,indexOfZero, dPointOfX);
		}
	}
	if( bCanSec )
	{//����
		bCanSec = CaluateAngle( dFreq, dSwing, dAngle, indexOfChannel,indexOfPoint, pPoint, count, indexOfZero, dPointOfX);
	}
	//Ƶ���ں���Χ��
	if(bCanSec && dFreq < m_dMaxFreq && dFreq > m_dMinFreq)
	{
		/*lhz moidy ����Ƶ���л������У��������������һ��Ƶ�ʣ���������һ��Ƶ�ʣ������β��Գƣ���������ĽǶȲ�׼ȷ*/
		BOOL isErrorAng = (DoubleCmp(pPoint[indexOfPoint].y) < 0 && DoubleCmp( dAngle - 179.5 ) < 0 )|| (DoubleCmp(pPoint[indexOfPoint].y) > 0 && DoubleCmp( dAngle - 180.5 ) > 0); 
		double diffTime = (dPointOfX[1] - dPointOfX[0]) - ( dPointOfX[2] - dPointOfX[1] );
        diffTime = fabs( diffTime );
		BOOL isNoCosWave = DoubleCmp( diffTime - 0.1 ) > 0 ; /*>0.1 ms ����һ���ԳƵ����Ų�*/
		//�����Ҽǵò�������
		if ( indexOfChannel == 1 && indexOfPoint == 1708)
		{
			CString tempStr;
			tempStr.Format( _T(" x = %0.3f diffTime = %0.3f \r\n"), pPoint[indexOfPoint].x, diffTime ); 
		//	TRACE( tempStr );
		}
		if ( isErrorAng || isNoCosWave)
		{/*����ֵ�ڸ����ᣬ��ĽǶ���<180,˵��������ĽǶȲ�׼ȷ�����л�ΪͬƵ�����ڼ��㣬�������׳����޸�ֱ�ӻ���180*/
			if ( isNoCosWave )
			{
				dAngle = dAngle;
			}
			dAngle = isErrorAng ? 180.0 : dAngle;
			//ͬƵ�����ڼ���
			CaluateAngleByFreqA( dFreq, dAngle, indexOfChannel, indexOfPoint, pPoint, count,indexOfZero, dPointOfX);
		} 
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//��δ��붼û�б�ʹ��
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
	{//���²���0ֵλ��
       int nPointInMinFreq =(int)(double) 50 * m_pCurve[indexOfChannel].nPointInCircle/m_dMinFreq;
	   int j = 0;
       int num = indexOfPoint - nPointInMinFreq/2;
       if ( num < 0 )
       {
		   return FALSE;
       }

       //�������һ����
	   int nIndexOfZeroPoint = 0;
	   BOOL isFind = FindZeroPosLeft(nIndexOfZeroPoint, indexOfChannel, indexOfPoint, pPoint, count, indexOfZero, dPointOfX );
       if ( FALSE == isFind )
       {
           return FALSE;
       }
	   nIndexOfZeroPoint = 1;
	   //���ұ���һ������
	    num = indexOfPoint + nPointInMinFreq/2;
	   for( j = indexOfPoint+2; j < num && j < count; j++ )
	   {
		   if(DoubleCmp(pPoint[j].y) != DoubleCmp(pPoint[j-1].y))
		   {
			   //���Բ�ֵ����
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
			   j -=MIN_NUM_INTERVAL_OF_ZERO;  //Ϊ�˱����ھ����ܽ��ĵط��ҵ��ڶ�����㣬��������ͷ�ҵ�Ҳ����Ч��
			   nIndexOfZeroPoint--;
			   break;
		   }

	   }
	   if ( 1 == nIndexOfZeroPoint )
	   {//δ�ҵ�
		   return FALSE;
	   }
	   //
	   cycle = (dPointOfX[1] - dPointOfX[0])*2;
	   dZeroAngle = dPointOfX[0];
	   dTime = pPoint[indexOfPoint].x - dZeroAngle;
	}
    dFreq = 1000/cycle;
	if ( DoubleCmp(pPoint[indexOfPoint].y) > 0 )
	{//��0���� indexOfZero[1]
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
/*����Ƶ�ʼ��ٶȼ���Ƕ�*/
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
		{//���ڵݼ������1�������Ϊ180�㡣3����������ܲ����������ڣ���0���Ĺ�������Ϊ0��ǡ�
            int tmp = indexOfZero[0];
            if ( DoubleCmp( pPoint[indexOfPoint].y ) < 0 )
            {//����������һ���Ƕ�Ϊ0��Ĺ����
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
			{//��2�������Ϊ0�㣬�����ұ�����һ����
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
	{ //��ǰ���������һ�ܲ�
	  
       if (   fh > 0 )
       {//��������,�������һ�������ұ�������
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
       {//�ڸ�����
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

    f0 = ((T2*T2 - T1*T1 + 2*T1*T2)*1000)/(2*T1*T2*(T1+T2)); //*1000ת��ΪS�ĵ�λ����Ϊhz
   
	a = (T1-T2)*1000/(T1*T2*(T1+T2)); //*1000ת��ΪS�ĵ�λ
	T = pPoint[indexOfPoint].x - dPointOfX[0];
	double orgAng ;
	orgAng = dAngle;
	dAngle = (360.0*f0*T)/1000 + (180*a*T*T)/1000; //��1000��msת��ΪS
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
//��������������ͨ���ı�ţ�������ǵڼ����㣬�ṹ�壬�ܵĵ����ж��٣���������飬����һ�����飨������������3���ռ�ģ�
BOOL CCurveView::CaluateAngle( double &dFreq, double &dSwing, double &dAngle, const int indexOfChannel,
							  const int indexOfPoint, Point* pPoint, int count, int *indexOfZero, double *dPointOfX)
{
	BOOL bCanSec = TRUE;
	double dmax[2],         //dmax��ʾһ���ܲ��е�������ֵ��yֵ
		cycle,              //���ڳ��ȣ���λms
		dZeroAngle,         //����λ�����߼�����ϵ�е�xֵ
		dTime;              //��ǰҪ����Ĳ���������������λ��֮���ʱ���
	dFreq = 1000/(dPointOfX[2]-dPointOfX[0]);			//ͨ�����������Ƶ��			//������X�ľ����ֵ
	//������ִ�е�
	if(DoubleCmp(dFreq-m_dMinFreq)<0 && DoubleCmp(m_dMaxFreq - dFreq)<0 )
	{
		bCanSec =FALSE;
	}
	else
	{
		//������ֵ���㷨
		dmax[0] = pPoint[(indexOfZero[0]+indexOfZero[1])>>1].y;				//����������ֵ��
		dmax[1] = pPoint[(indexOfZero[1]+indexOfZero[2])>>1].y;
		//���
		dSwing = max(fabs(dmax[0]),fabs(dmax[1]));
		//���������ֵ�����ǽ���Ļ�
		if((DoubleCmp(dmax[0])+DoubleCmp(dmax[1])) != 0)	//���������ֵ����һ��һ��������
		{
			return FALSE;
		}
		//�����һ����ֵ����������0�Ļ�
		if(DoubleCmp(dmax[0]) > 0)			//���ǰ��������������ϰ���
		{
			//�����ĵ�Xֵ
			dZeroAngle = dPointOfX[0];
		}
		else								
		{
			//�����ĵ�Xֵ
			dZeroAngle = dPointOfX[1]; 
		}
		//���ڳ��ȣ���λms
		cycle = dPointOfX[2] - dPointOfX[0];					//һ�����ڵ�X�����
		//��ǰҪ����Ĳ���������������λ��֮���ʱ���
		dTime = pPoint[indexOfPoint].x - dZeroAngle;			//
		//��ǰλ�þ�������λ��1���������ڵ�ʱ��
		if(DoubleCmp(cycle - fabs(dTime)) >= 0)					//�������������������
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
//����ߵĹ����
BOOL CCurveView::FindZeroPosLeft( int  &nIndexOfZeroPoint, const int indexOfChannel, const int indexOfPoint, Point* pPoint, int count,
								  int *indexOfZero,  double *dPointOfX)
{
	BOOL bCanSec = TRUE;
	//����ڵ�ǰ�����֮���ҳ�����㲻������ʱ����Ҫ�ڵ�ǰ��֮���ң��Բ���������   ��0������ֵ
	if(nIndexOfZeroPoint < 3 && nIndexOfZeroPoint>= 0)
	{
		int j = 0, nLowPointIndex;
		//106
		int nPointInMinFreq =(int)(double) 50 * m_pCurve[indexOfChannel].nPointInCircle/m_dMinFreq;
		//159
		nPointInMinFreq = nPointInMinFreq*3/2;
		//�������ĵ��Ǵ��ڵ���159   �����������������һ���ܲ��ĵ���� 
		if(indexOfPoint >= nPointInMinFreq)
		{
			nLowPointIndex = indexOfPoint -nPointInMinFreq;
		}
		//���������ǵ�160�������ڵĵ�Ļ�
		else
		{
			nLowPointIndex = 0;
		}
		//j���ڴ��������ֵ���
		//��Щ����Ľ������ʹ������ִ�еĴ���0��159
		for(j=indexOfPoint;j>nLowPointIndex;j--)
		{	
			//�ж�ǰ��������λ�õ������ǲ���һ��
			if(DoubleCmp(pPoint[j].y) != DoubleCmp(pPoint[j-1].y))
			{
				//���Բ�ֵ����    dPointOfX�Ǿ���3���ռ������
				switch(DoubleCmp(pPoint[j-1].y))
				{
					//���Ƕ�С��0��ʱ��
					case -1:		//��������
						dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + (pPoint[j].x - pPoint[j-1].x)*fabs(pPoint[j-1].y)/(pPoint[j].y-pPoint[j-1].y);
						break;
					//���Ƕȴ���0ʱ
					case 1:
						dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x + (pPoint[j].x - pPoint[j-1].x)*pPoint[j-1].y/(fabs(pPoint[j].y)+pPoint[j-1].y);
						break;
						//�ӽ���ʱ
					default:
						dPointOfX[nIndexOfZeroPoint] = pPoint[j-1].x;
						break;
				}
				indexOfZero[nIndexOfZeroPoint] = j-1;
				//4
				j -=MIN_NUM_INTERVAL_OF_ZERO;  //Ϊ�˱����ھ����ܽ��ĵط��ҵ��ڶ�����㣬��������ͷ�ҵ�Ҳ����Ч��
				nIndexOfZeroPoint--;
				if(nIndexOfZeroPoint<0)			//ÿ�ҵ�һ�������ͼ�һ���ں���ķ�Χ���ҵ������Ǽ���
				{
					break;
				}
			}
		}
	}
	bCanSec = nIndexOfZeroPoint < 0;
	return bCanSec;
}
//���ұ�
BOOL CCurveView::FindZeroPosRight( int  &nIndexOfZeroPoint, const int indexOfChannel, const int indexOfPoint, Point* pPoint, int count,
								  int *indexOfZero, double *dPointOfX)
{
	BOOL bCanSec = TRUE;
	//����ڵ�ǰ�����֮���ҳ�����㲻������ʱ����Ҫ�ڵ�ǰ��֮���ң��Բ���������
	if(nIndexOfZeroPoint < 3 )
	{
		int nPointInMinFreq =(int)(double) 50 * m_pCurve[indexOfChannel].nPointInCircle/m_dMinFreq;
		int nHighPointIndex = indexOfPoint + nPointInMinFreq;
		nHighPointIndex += nPointInMinFreq/2;/*lhz 2017-1-11 moidy�������ڹ�0�㸽���ĵ������ң�����1.5���ܲ������ҵ���������ټӰ���ܲ��ĵ�����*/
		if(indexOfPoint + nPointInMinFreq/2 > count)
		{
			return FALSE;
		}
		int j=indexOfPoint+1;
		if(nIndexOfZeroPoint>0 && abs(indexOfZero[nIndexOfZeroPoint-1] - j) < 4)
		{
			j += MIN_NUM_INTERVAL_OF_ZERO; //Ϊ�˱����ھ����ܽ��ĵط��ҵ��ڶ�����㣬��������ͷ�ҵ�Ҳ����Ч
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
				j+=MIN_NUM_INTERVAL_OF_ZERO;      //Ϊ�˱����ھ����ܽ��ĵط��ҵ��ڶ�����㣬��������ͷ�ҵ�Ҳ����Ч��
				nIndexOfZeroPoint++;
				if(nIndexOfZeroPoint>2) 
				{
					break;
				}
			}
		}
        if(nIndexOfZeroPoint <= 2) //ǰ������һ�飬����������㣬���Դ����߲������Ҳ�
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
	//��֤ÿ���߼��̶�֮�����ʾ����������5������
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
	PRINTDLG pd; //�ýṹ������ӡ�Ի����е�������Ϣ 
	LPDEVMODE lpDevMode; 
	
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd)) //���Ĭ�ϵĴ�ӡ������Ϣ 
	{ 
		lpDevMode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpDevMode) 
		{ 
			lpDevMode->dmPaperSize=DMPAPER_A4; //����ӡֽ����ΪA4 
			if(m_curvePrintInfo.bHPrint)
			{
				lpDevMode->dmOrientation=DMORIENT_LANDSCAPE; //����ӡ������Ϊ�����ӡ 
				
			}
			else
			{
				lpDevMode->dmOrientation= DMORIENT_PORTRAIT; //�����ӡ
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
	nRowNum+=3;//һ�����������̶���
	for(i = 0;i < pMainFrm->m_wareFileData.m_nCurveCount;i++)
	{
		if(i == 0 && m_curvePrintInfo.bAnalyse) continue; //���η���ʱ����һ������Ҫ��ڶ������߻���ͬһ����ϵ��
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

	if(nRowNum == 2) //��ʾ��һҳ�պð����߻���
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

	
	//��ӡ��ֵ��ҳ��滯
	
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
	m_rcPrintCurve.top = 100;//����m_rcPrintCurveΪ��Ч����
}

void CCurveView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bPrint = TRUE;
	m_rcPrintCurve.top = 100;//����m_rcPrintCurveΪ��Ч����
	m_pOldCurve = m_pCurve;
	m_pCurve = NULL;
	m_nOldCurveCount = m_nCurveCount;
	m_cOldPS = m_strValueType == _T("����ֵ") ? 's' : 'p';
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
		VARIABLE_PITCH|FF_SWISS,_T("����"));
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
			str.Format(_T("%s��%s.%d"), info, pWareFileData->m_FirstSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),
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
			str.Format(_T("���ضβ�������ʱ�䣺%s.%d"), info, pWareFileData->m_ReturnSampleTime.Format(_T("%Y-%m-%d %H:%M:%S")),
				pWareFileData->m_milliSecondOfReturn);
			pDC->DrawText(str,&rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			rcText.top -= ROW_HEIGHT;
			rcText.bottom -= ROW_HEIGHT;
		}
		nRowI += 3;    //���ڻ������¿̶ȳ�
		info.LoadString( IDS_STR_WAVECURVE );
		info += _T(": ");
		pDC->DrawText( info, &rcText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rcText.top= rcText.bottom;
		rcText.bottom -= 2*ROW_HEIGHT;
		for(i=0;i<pWareFileData->m_nCurveCount;i++) 
		{
			if(i==0 && m_curvePrintInfo.bAnalyse) continue;   //���η���ʱ��ǰ��������Ҫ����ͬһ����ϵ��
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
		//��������
		PrintCurve(pDC,rcText,0);
		
		if(i == pWareFileData->m_nCurveCount)
		{
			//���Ʊ��
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
		//��������
		PrintCurve(pDC,rcText,nPrintNum);

		if(i == pWareFileData->m_nCurveCount && (nRowInPage - nRowI) >2)
		{
			//���Ʊ��
			
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
	if(cPS=='p'||cPS=='P') m_strValueType=_T("һ��ֵ");
	else m_strValueType=_T("����ֵ");
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
		//���ｫ���������ĵ�ѹ�����������ʹ��ͬ����Y����̶ȱ�ȣ���Ϊ�����û��ӽ����Ͼ��ܿ����������ߵ�
		//��ѹ������������Ĵ�С������ֻ�п��뿪�����Ż�����Y���ϵ�λ�̶���������߼�ֵ��valueOfCellY����
		//ģ������valueOfCellYΪ0��Ҳ����δ���ã�һ������ϵ�У�Y������������ʮ����λ�̶ȣ�ÿ����λ�̶�
		//���Ա�ʾһ�����������߼�ֵ�����Ժ�С�����ݣ�Ҳ�����ڽ���õ��Ŵ���ʾ��minRange��������������ϵ
		//����ʾ�߼�ֵ��Χ����Сֵ
		dMaxVal = 0;
		int sampleRetBegin=0;
		if(m_diffMilli>0)
		{
			intersect=1000;
			m_dota=m_diffMilli;
			str.Format(_T("��t =%dms"),m_dota);
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
			//���ڲ������������������⣬��50����ֻ����49���ߣ��˴���Ҫ51�����������50�ߣ�����ͬ��
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
				//���tempY����temp_n
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
	//�����ѹ�е�dValueOfCellY
	if(dMaxVolY<dVolMinRange) dMaxVolY=dVolMinRange;
	temp_n=(long)dMaxVolY;
	//���tempY����temp_n
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
	//���������С
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
	//���������ĺ�����
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
	//��Ҳ�������Լ�Ӧ���õ���ɫ
	pDC->FillSolidRect(rcDC,RGB(255,255,255));
	//��ͼ
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
		VARIABLE_PITCH|FF_SWISS,_T("����"));
	pOldFont=pDC->SelectObject(&font);
	CString str=_T("��  ��:");
	CString temp;
	int i;
	CSize size;
	size=pDC->GetTextExtent(str);
	//�������������������ڣ�����Ҫ���Ƹ�����������x=m_mousePoint.x,����Ҫ�������
	//m_nFirstDisCurve��m_nLastDisCurveͨ����˲ʱֵ��ģ����ͨ������λ��,���л��������
	//���ߵ�ʱ���͵�ǰ����ߵľ�ȷʱ��ֵ(����������ʱ����)
	
	
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
		int nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //����ϵ�Ķ�������
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
					temp.Format(_T("%s:%.3f%s\r\n           %.1f��"),m_pCurve[i].name,
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
		int nTopToCoor=m_rectCurve.top,nWidthOfCoor;    //����ϵ�Ķ�������
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
	//y��
	pDC->MoveTo(m_rectCurve.left,m_rectCurve.top);
	pDC->LineTo(m_rectCurve.left,m_rectCurve.bottom);
	//��������
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
	int nWidthOfCell,nTopOfCoor;  //nTopOfCoor��ǰ����ϵ�Ķ�����
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
				//���ڴ�0��1���1��0�Ŀ������ĵ㣬�������ǵ��е㴦�������midPoint0,midPoint1,��仯ǰ������
				//�ֱ�Ϊp1,p2,��midPoint0.y = p1.y,midPoint1.y = p2.y,midPoint0.x=midPoint1.x=p1.x+(p2.x-
				//p1.x)/2,����ϵͳ��˳������p1-midPoint0-midPoint1-p2
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
				pDC->SelectObject(&samplePen);//ʹ�û�ɫ
			}
			if (m_pCurve[i].bIsOverlap)//����ʼ�κͷ��ض�����
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
	//��֤ÿ���߼��̶�֮�����ʾ����������5������
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
		VARIABLE_PITCH|FF_SWISS,_T("����"));
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

	if (m_pCurve[1].name == _T("���ز��Us") || m_pCurve[1].name == _T("Mainboard Us"))
	{
		index = 1;
		index2 = 2;
	}
	else if (m_pCurve[2].name == _T("���ز��Us") || m_pCurve[2].name == _T("Mainboard Us"))
	{
		index = 2;
		if (m_pCurve[3].name == _T("���ز��Ug") || m_pCurve[3].name == _T("Mainboard Ug"))
		{
			index2 =3;
		}
		else if (m_pCurve[1].name == _T("���ز��Ug") || m_pCurve[1].name == _T("Mainboard Ug"))
		{
			index2 =1;
		}
	}
	else if (m_pCurve[3].name == _T("���ز��Us") || m_pCurve[3].name == _T("Mainboard Us"))
	{
		index = 3;
		if (m_pCurve[2].name == _T("���ز��Ug") || m_pCurve[2].name == _T("Mainboard Ug"))
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
	double waveRidge = 0.0; //��ѹ���β���

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
		m_pCurve[0].lpStartSecPoint[i].x = m_pCurve[index].lpStartSecPoint[i].x /*- m_fMZstartDota*/; //2014-8-18 moidy �����ѹx��ֵ���䣬��Ӧת�ǣ�ֻ��ϵͳ���ѹ��������ƫ��Ӧת��
		if (sSetVol!=0 && gSetVol!=0)
		{
			sysVol = m_pCurve[index].lpStartSecPoint[i].y; //2014-8-18 moidy 
			ugVol = m_pCurve[index2].lpStartSecPoint[i].y;
			if ( theApp.m_freq[0] > 0 )
			{//��ϵͳ���ѹ
				 double vol = pow( waveRidge, 2) - pow(sysVol, 2); //ƫ�Ƶ�ѹ
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
					 sysVol = cos( angle ) * sysVol + sin( angle ) * vol; //U = cos30Ut + ��Usƽ�� - Utƽ���������� /2 
                 }else if ( pMainFrm->m_SystemAngleSet < 0)
                 {
					 angle = -(PI * pMainFrm->m_SystemAngleSet )/180.0;
					 sysVol = cos( angle )* sysVol - sin( angle ) * vol; //U = cos30Ut - ��Usƽ�� - Utƽ���������� /2 
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
			{//��ϵͳ���ѹ
				double vol = pow( waveRidge, 2) - pow(sysVol, 2); //ƫ�Ƶ�ѹ
				bool isIncrease = false;
				if ( (i < m_pCurve[0].returnCount && sysVol < m_pCurve[index].lpReturnSecPoint[i+1].y ) 
					|| ( (m_pCurve[0].returnCount-1) == i && sysVol > m_pCurve[index].lpReturnSecPoint[i - 1].y))
				{//����
					
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
	int indexOfZero[3];     //���ڱ��������������������Ĳ���������
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
			//���Բ�ֵ����
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
			j -=MIN_NUM_INTERVAL_OF_ZERO;  //Ϊ�˱����ھ����ܽ��ĵط��ҵ��ڶ�����㣬��������ͷ�ҵ�Ҳ����Ч��
			nIndexOfZeroPoint--;
			if(nIndexOfZeroPoint<0) //����Ѿ��ҵ����������
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
//2016-5-5 add ��ӵ���¼������
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
	BROWSEINFO   bi;                         //BROWSEINFO�ṹ��   
	TCHAR   Buffer[MAX_PATH]=_T("");   
	TCHAR   FullPath[MAX_PATH]=_T("");   
	bi.hwndOwner   =   m_hWnd;       //m_hWnd��ĳ���������   
	bi.pidlRoot   =   NULL;   
	bi.pszDisplayName   =   Buffer;   //����ѡ���Ŀ¼���Ļ�����   
	bi.lpszTitle   =   _T("Selection");   //�����Ĵ��ڵ�������ʾ   
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS   ;   //ֻ����Ŀ¼��������־��MSDN   
	bi.lpfn   =   NULL;                                                   //�ص���������ʱ������   
	bi.lParam   =   0;   
	bi.iImage   =   0;   
	ITEMIDLIST*   pidl   =   ::SHBrowseForFolder   (&bi);   //��ʾ�������ڣ�ITEMIDLIST����Ҫ   
	CString strPath;
	if(::SHGetPathFromIDList(pidl,FullPath))           //��ITEMIDLIST�еõ�Ŀ¼��������·��   
	{   
		//�ɹ�   
		strPath=FullPath;
		if(strPath[strPath.GetLength()-1]!=TEXT('\\'))
		{
			strPath += _T("\\");
		}
		CFile file;
		//����¼���ļ�
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
			AfxMessageBox(_T("�����ļ��ɹ�"));
		}else
		{
			AfxMessageBox(_T("��ģ����ͨ��"));
		}
	}
}


void CCurveView::DrawEventFlag(CDC *pDC)
{
	//���¼���־
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
	//�¼�����
	int count = pSub->GetChannelCount();
	if (WARE_EVENT_TYPE == pSub->GetDataType() && count > 0)
	{	
		CRect rect;
		double EventLogicX = 0.0;
		long   EventPixelX = 0;
		int nBottomOfScale = 0;/*��������ϵ�ܸ�*/
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

	//���¼�������ʾ����
	int totalCount = 0;
	CPoint pointScrollPos = GetScrollPosition();
	CString tempStr = _T("������");
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
	//�¼�����
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
	/*��������������������ҷ���ʱ�����*/
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
	ScrollToPosition(pointScrollPos);/*������ĳ������λ�ã�����ʱֻ�����������������*/
	RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW);
}


