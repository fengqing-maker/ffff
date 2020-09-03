#if !defined(AFX_REALMONITORVIEW_H__31F7CF4E_47D7_4460_9961_AA3AB439E1A7__INCLUDED_)
#define AFX_REALMONITORVIEW_H__31F7CF4E_47D7_4460_9961_AA3AB439E1A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RealMonitorView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRealMonitorView view
#include "Frame.h"
enum PoleType{VolPole,FreqPole};
#define UM_SYNC_REAL                (WM_USER + 210)
#define UM_TEL_DATA                 (WM_USER + 211) 
#define UM_PARA_DATA                (WM_USER + 212)
#define UM_COM_FAILED               (WM_USER + 213)


#define IDC_UPDATE_CYCLE_INFO_TIMER       0X01
#define REAL_VIEW_BUFLEN                 1500//数据缓存大小

class CRealMonitorView : public CScrollView
{
protected:
	CRealMonitorView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRealMonitorView)
	//关闭线程
	BOOL StopThread();
// Attributes
public:
	//当同期专用数据读取成功后，以下函数用于更新界面右边的状态信息
	void UpdateStateInfo();
	int GetCurRunChannelNum(){ return m_nChannelNum;}

// Operations

	
	void DrawRealMonitor(CDC * pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRealMonitorView)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation

	virtual ~CRealMonitorView();
	LRESULT OnThreadPara(BOOL bSuccess,LPARAM lParam);
	LRESULT OnSyncReal(BYTE * pBuf,int nBufSize);
	LRESULT OnThreadTelControl(BYTE nTelConRst,UINT nTelConCmd);
	LRESULT OnComFailed(BYTE nTelConRst,UINT nTelConCmd);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CRealMonitorView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnThreadPara(BOOL bSuccess);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdatePara();
	afx_msg void OnStartSync();
	afx_msg void OnRestart();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bGFrqHigh;
	BOOL m_bGVolHigh;
	int m_sysTurnAngle;
	BOOL m_bConnect;
	void StartCommunication();
	void UpdateRightBarPara();
	void UpdateRealData();
	double m_dMaxVolDispersion;                //最大压差
	double m_dMinVolDispersion;                //最小压差
	double m_dMaxFreqDispersion;               //最大频差
	double m_dMinFreqDispersion;               //最小频差
	CString m_strSyncPoint;                    //并列点代号
	double m_dRatingOfSysVol;                  //系统侧额定电压
	double m_dRatingOfGVol;                    //待并侧额定电压
	int m_nChannelNum;                         //同期通道号

	BYTE m_nStateCount;				  //状态信息的个数
	UINT m_nTelCode;                  //遥控命令,0xffffffff表示无遥控指令
	bool m_bReqSyncThreadExit;        //请求结束同期专用线程

private:
	const int m_nHeightOfFrame;                //上下空白区的高度
	const int m_nHeightOfPole;                 //频差和压差柱的高度
	const int m_nHeightOfInfo;                 //频差、压差、角差和提示信息总共的高度
	const int m_nHeightOfTable;                //实时数据表的高度
	const int m_nWideOfFrame;                  //左右两边的空白区的宽度
	const int m_nWideOfPole;                   //频差和压差柱的宽度
	const int m_nWideOfTable;                  //实时数据表的宽度
	const int m_nWideOfRect;                   //绘图区的的宽度
	const int m_nHeightOfRect;                 //绘图区的高度
	const int m_nTimesOfWidthOfHand;      //m_TimesOfWidthOfHand:2 = m_nLengthOfRatingHand:表针最宽处的宽度,默认为16
	const double m_dRangeOfFreqDis;            //频差柱所能表示的频差范围
	const double m_dRangeOfVolDis;             //压差柱所能表示的压差范围

	CCriticalSection m_bGetDataCS; //获取数据临界区
	bool             m_nIsRun;
protected:
	COleDateTime m_dtDeviceTime;
	
	int m_nLengthOfGHand;                      //待并侧表针的长度
	int m_nLengthOfSysHand;                    //系统侧表针的长度
	int m_nLengthOfRatingHand;                 //额定电压的表针长度
	CString m_strTipInfo;
	CString m_strException;
	void DrawPole(CDC *pDC,PoleType poleType);
	CPoint m_arrPtSysHand[4];     //系统侧指针点集
	CPoint m_arrPtGHand[4];       //待并侧指针点集
	COLORREF m_clrLegPole;        //频差或压差柱合法时的颜色
	COLORREF m_clrIllegPole;      //频差或压差柱非合法时的颜色
	COLORREF m_clrSysHand;        //系统侧同步表针的颜色，hour hand时针
	COLORREF m_clrGHand;          //待并侧同步表针的颜色

	CString m_strVoltageDispersion;   //压差
	CString m_strFreqDispersion;      //频差
	CString m_strSysVoltage;          //系统侧电压
	CString m_strGVoltage;            //待并侧电压
	CString m_strSysFreq;             //待系统侧频率
	CString m_strAngleDispersion;     //相角差
	CString m_strGFreq;               //待并侧频率
	
	void DrawStaticView(CDC *pDC);
	BOOL InitRectInfo();
	CRect m_rcPoleOfFreq;             //频差柱的矩形区
	CRect m_rcPoleOfVoltage;          //压差柱的矩形区
	CRect m_rcSyncClock;              //同步表的矩形区
	CRect m_rc0SyncClockInfo;         //同步表的0°信息矩形区
	CRect m_rc90SyncClockInfo;        //同步表的90°信息矩形区
	CRect m_rc_90SyncClockInfo;       //同步表的-90°信息矩形区
	CRect m_rc180SyncClockInfo;       //同步表的180°信息矩形区
	CRect m_rcFreqDispersionInfo;     //频差信息矩形区
	CRect m_rcVoltageDispersionInfo;  //压差信息矩形区
	CRect m_rcAngleDispersionInfo;    //角差信息矩形区
	CRect m_rcTipInfo;                //提示信息矩形区
	CRect m_rcRealTable;              //实时数据表矩形区
	CRect m_rcRealDraw;               //用于绘制实时数据的矩形区
	CRect m_rcClientView;             //所有的客户绘图视图区
	CRect m_rcCutline;                //图例距形区，cutline是图例的意思

	BOOL m_bTelecontrol;     //与控制方式是否为遥控
	BOOL m_bStartSync;       //是否点击了启动同期按钮，但却没有发送同期命令
	UINT m_nCycInfoTimerID;    //循环信息的定时器ID
	UINT m_nCycValue;          //循环信息的定时记数器




	StateInfo * m_pStateInfo;         //状态信息数组
	BYTE m_nStartIndOfPrompt;         //提示信息的起始序号
	BYTE m_nEndIndOfPrompt;           //提示信息的终止序号
	BYTE m_nStartIndOfException;      //异常信息起始序号
	BYTE m_nEndIndOfException;        //异常信息终止序号
	BYTE m_nStartIndOfWorkMode;       //工作模式信息起始序号
	BYTE m_nEndIndOfWorkMode;         //工作模式信息终止序号
	vector <BYTE> m_vecExceptionIndex;       //有效异常信息序号向量
	vector <BYTE> m_vecPromptIndex;          //有效提示信息序号向量
	HANDLE m_hSyncThread;             //实测数据线程句柄
	HANDLE m_hTelThread;              //传输其它数据线程句柄
	HANDLE m_hParaThread;             //传输定值数据的线程句柄
	static DWORD WINAPI SyncDataThread(VOID *pWnd);
	static DWORD WINAPI TelDataThread(VOID *pWnd);
	static DWORD WINAPI ParaDataThread(VOID *pWnd);
	VOID  SetGrayTool();




	void StopCommunication();


	ParaData  m_arrParaData[MAX_SYS_PARA_COUNT+MAX_NOR_PARA_COUNT];
	unsigned short m_nNorParaCount;        //普通定值个数
	unsigned short m_nSysParaCount;        //系统定值个数
	
	

};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REALMONITORVIEW_H__31F7CF4E_47D7_4460_9961_AA3AB439E1A7__INCLUDED_)
