#if !defined(AFX_ANALYSEVIEW_H__5CE0E796_F878_4E74_9441_C6C33D4A6468__INCLUDED_)
#define AFX_ANALYSEVIEW_H__5CE0E796_F878_4E74_9441_C6C33D4A6468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CurveView.h"
// AnalyseView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnalyseView view

class CAnalyseView : public CCurveView
{
protected:
	CAnalyseView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAnalyseView)

// Attributes
public:
	virtual void PrintLine(CDC * pDC);
	virtual void PrintGirdLine(CDC * pDC,int nPrintFirstCurve);
	virtual void DrawPrintCurve(CDC * pDC,int nFirstDrawCurve);
	virtual void ChangePrintCurve(Curve * pCurve,int nCurveCount,CRect rcPrint,double dPrintTime,char cPS);
	virtual void ChangeCurve(Curve *pCurve,long count,bool isUnitedFile,int diffMilli,char ps);  //����������ʾ����
// 	POINT MapPoint(Point p1, POINT &p2,double cellOfY,POINT origitalPoint,UINT nWidthOfCell);  //Ҫ��ʾ���߼�����ת��Ϊ��ʾ�������ʾ����
// 	Point MapPointToLogic(POINT p1, Point &p2,double cellOfY,POINT origitalPoint,UINT nWidthOfCell);   //����ʾ�������ʾ����ת��Ϊ�߼�����
	//����ʾ����������Ϊ��λ��
	virtual void DrawLine(CDC *pDC);            //��������
	
	virtual void DrawGridLine(CDC *pDC);        //���ƿ̶ȸ���
	//void Init(Curve *pCurve,long count);
	virtual void DrawCurve(CDC *pDC);  
// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveView)
	protected:
	//}}AFX_VIRTUAL
	
	// Implementation
	protected:
		//UINT m_timer;   //1��ʱ�Ķ�ʱ��
		
		
	public:
		int m_dota;
		int intersect;
		int m_diffMilli;
		bool m_isUnitedFile;
		BOOL CalculateAngleCurve();
		BOOL CGetAppointCurve(CWareFileData * pWareFileData);//��ȡָ��ͨ���Ĳ������ݣ�����pWareFileData��
		virtual void SetCoorHeight(int nDigitalHeight = DIGITAL_HEIGHTH_COONDIATE,int nAnalogHeight = ANALOG_HEIGHTH_COONDIATE);

	protected:
		virtual ~CAnalyseView();
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif
		
		// Generated message map functions
		//{{AFX_MSG(CCurveView)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnInfoWnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALYSEVIEW_H__5CE0E796_F878_4E74_9441_C6C33D4A6468__INCLUDED_)
