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
#define REAL_VIEW_BUFLEN                 1500//���ݻ����С

class CRealMonitorView : public CScrollView
{
protected:
	CRealMonitorView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRealMonitorView)
	//�ر��߳�
	BOOL StopThread();
// Attributes
public:
	//��ͬ��ר�����ݶ�ȡ�ɹ������º������ڸ��½����ұߵ�״̬��Ϣ
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
	double m_dMaxVolDispersion;                //���ѹ��
	double m_dMinVolDispersion;                //��Сѹ��
	double m_dMaxFreqDispersion;               //���Ƶ��
	double m_dMinFreqDispersion;               //��СƵ��
	CString m_strSyncPoint;                    //���е����
	double m_dRatingOfSysVol;                  //ϵͳ����ѹ
	double m_dRatingOfGVol;                    //��������ѹ
	int m_nChannelNum;                         //ͬ��ͨ����

	BYTE m_nStateCount;				  //״̬��Ϣ�ĸ���
	UINT m_nTelCode;                  //ң������,0xffffffff��ʾ��ң��ָ��
	bool m_bReqSyncThreadExit;        //�������ͬ��ר���߳�

private:
	const int m_nHeightOfFrame;                //���¿հ����ĸ߶�
	const int m_nHeightOfPole;                 //Ƶ���ѹ�����ĸ߶�
	const int m_nHeightOfInfo;                 //Ƶ�ѹ��ǲ����ʾ��Ϣ�ܹ��ĸ߶�
	const int m_nHeightOfTable;                //ʵʱ���ݱ�ĸ߶�
	const int m_nWideOfFrame;                  //�������ߵĿհ����Ŀ��
	const int m_nWideOfPole;                   //Ƶ���ѹ�����Ŀ��
	const int m_nWideOfTable;                  //ʵʱ���ݱ�Ŀ��
	const int m_nWideOfRect;                   //��ͼ���ĵĿ��
	const int m_nHeightOfRect;                 //��ͼ���ĸ߶�
	const int m_nTimesOfWidthOfHand;      //m_TimesOfWidthOfHand:2 = m_nLengthOfRatingHand:��������Ŀ��,Ĭ��Ϊ16
	const double m_dRangeOfFreqDis;            //Ƶ�������ܱ�ʾ��Ƶ�Χ
	const double m_dRangeOfVolDis;             //ѹ�������ܱ�ʾ��ѹ�Χ

	CCriticalSection m_bGetDataCS; //��ȡ�����ٽ���
	bool             m_nIsRun;
protected:
	COleDateTime m_dtDeviceTime;
	
	int m_nLengthOfGHand;                      //���������ĳ���
	int m_nLengthOfSysHand;                    //ϵͳ�����ĳ���
	int m_nLengthOfRatingHand;                 //���ѹ�ı��볤��
	CString m_strTipInfo;
	CString m_strException;
	void DrawPole(CDC *pDC,PoleType poleType);
	CPoint m_arrPtSysHand[4];     //ϵͳ��ָ��㼯
	CPoint m_arrPtGHand[4];       //������ָ��㼯
	COLORREF m_clrLegPole;        //Ƶ���ѹ�����Ϸ�ʱ����ɫ
	COLORREF m_clrIllegPole;      //Ƶ���ѹ�����ǺϷ�ʱ����ɫ
	COLORREF m_clrSysHand;        //ϵͳ��ͬ���������ɫ��hour handʱ��
	COLORREF m_clrGHand;          //������ͬ���������ɫ

	CString m_strVoltageDispersion;   //ѹ��
	CString m_strFreqDispersion;      //Ƶ��
	CString m_strSysVoltage;          //ϵͳ���ѹ
	CString m_strGVoltage;            //�������ѹ
	CString m_strSysFreq;             //��ϵͳ��Ƶ��
	CString m_strAngleDispersion;     //��ǲ�
	CString m_strGFreq;               //������Ƶ��
	
	void DrawStaticView(CDC *pDC);
	BOOL InitRectInfo();
	CRect m_rcPoleOfFreq;             //Ƶ�����ľ�����
	CRect m_rcPoleOfVoltage;          //ѹ�����ľ�����
	CRect m_rcSyncClock;              //ͬ����ľ�����
	CRect m_rc0SyncClockInfo;         //ͬ�����0����Ϣ������
	CRect m_rc90SyncClockInfo;        //ͬ�����90����Ϣ������
	CRect m_rc_90SyncClockInfo;       //ͬ�����-90����Ϣ������
	CRect m_rc180SyncClockInfo;       //ͬ�����180����Ϣ������
	CRect m_rcFreqDispersionInfo;     //Ƶ����Ϣ������
	CRect m_rcVoltageDispersionInfo;  //ѹ����Ϣ������
	CRect m_rcAngleDispersionInfo;    //�ǲ���Ϣ������
	CRect m_rcTipInfo;                //��ʾ��Ϣ������
	CRect m_rcRealTable;              //ʵʱ���ݱ������
	CRect m_rcRealDraw;               //���ڻ���ʵʱ���ݵľ�����
	CRect m_rcClientView;             //���еĿͻ���ͼ��ͼ��
	CRect m_rcCutline;                //ͼ����������cutline��ͼ������˼

	BOOL m_bTelecontrol;     //����Ʒ�ʽ�Ƿ�Ϊң��
	BOOL m_bStartSync;       //�Ƿ���������ͬ�ڰ�ť����ȴû�з���ͬ������
	UINT m_nCycInfoTimerID;    //ѭ����Ϣ�Ķ�ʱ��ID
	UINT m_nCycValue;          //ѭ����Ϣ�Ķ�ʱ������




	StateInfo * m_pStateInfo;         //״̬��Ϣ����
	BYTE m_nStartIndOfPrompt;         //��ʾ��Ϣ����ʼ���
	BYTE m_nEndIndOfPrompt;           //��ʾ��Ϣ����ֹ���
	BYTE m_nStartIndOfException;      //�쳣��Ϣ��ʼ���
	BYTE m_nEndIndOfException;        //�쳣��Ϣ��ֹ���
	BYTE m_nStartIndOfWorkMode;       //����ģʽ��Ϣ��ʼ���
	BYTE m_nEndIndOfWorkMode;         //����ģʽ��Ϣ��ֹ���
	vector <BYTE> m_vecExceptionIndex;       //��Ч�쳣��Ϣ�������
	vector <BYTE> m_vecPromptIndex;          //��Ч��ʾ��Ϣ�������
	HANDLE m_hSyncThread;             //ʵ�������߳̾��
	HANDLE m_hTelThread;              //�������������߳̾��
	HANDLE m_hParaThread;             //���䶨ֵ���ݵ��߳̾��
	static DWORD WINAPI SyncDataThread(VOID *pWnd);
	static DWORD WINAPI TelDataThread(VOID *pWnd);
	static DWORD WINAPI ParaDataThread(VOID *pWnd);
	VOID  SetGrayTool();




	void StopCommunication();


	ParaData  m_arrParaData[MAX_SYS_PARA_COUNT+MAX_NOR_PARA_COUNT];
	unsigned short m_nNorParaCount;        //��ͨ��ֵ����
	unsigned short m_nSysParaCount;        //ϵͳ��ֵ����
	
	

};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REALMONITORVIEW_H__31F7CF4E_47D7_4460_9961_AA3AB439E1A7__INCLUDED_)
