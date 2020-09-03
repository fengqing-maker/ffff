// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__5D30DB11_2EC6_4356_8D7F_17B196C78492__INCLUDED_)
#define AFX_MAINFRM_H__5D30DB11_2EC6_4356_8D7F_17B196C78492__INCLUDED_

#include "WareFileData.h"	// Added by ClassView
#include "CoolTabCtrl.h"
#include "StatusBarEx.h"
#include "ToolBarEx.h"
#include "ButtonTabCtrl.h"
#include "MltiTree.h"
#include "WareEventList.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IDC_TREECTRL 0x01
#define IDC_BOTTON_BAR 0X02
#define IDC_LEFT_BAR 0X03
#define IDC_EDIT_FILE_INFO 0X04
#define IDC_LIST_PARA_INFO 0X05
#define IDC_LIST_CHANNEL_INFO 0X06
#define IDC_BUTTON_TAB 0X07
#define IDC_RIGHT_LIST_TAB                 0X08
#define IDC_SYNC_PARA_LIST                 0x09
#define IDC_STATE_LIST                     0x0A
#define IDC_RIGHT_BAR                      0X0B



#define IDM_SHOW_CURVE 0X01
#define IDM_CURVE_ANALYSE 0X02
#define IDM_DELETE_OUT_TO_LIST 0X03
#define IDM_DELETE_FILE 0X04
#define IDM_PROJECT_SET 0X05
#define IDM_DEVICE_CONNECT_TEST 0X06
#define IDM_OUT_REAL_DATA_TO_EXCEL 0x07
#define IDM_OUT_PARA_DATA_TO_EXCEL 0X08
#define IDM_OUT_TO_COMTRADE        0X09
#define IDM_ACHROMATIC             0X0A         //ȥɫ�������߰��ڰ���ʾ
#define IDM_DYEING                 0X0B         //��ɫ�������߲�ɫ��ʾ 
#define IDM_CURVE_DIS_SET          0x0c         //������ʾ����
#define IDM_CURVE_UNITE          0x0d         //���ߺϲ�
#define IDM_SHOW_VALID          0x0e         //����ʾ��Чֵ
#define IDM_SHOW_SAMPLE          0x0f         //����ʾ����ֵ
#define IDM_SHOW_ALL          0x10         //��Чֵ�Ͳ���ֵ����ʾ
#define IDM_READ_PARA_DATA 0X11
#define IDM_IN_PARA_FILE 0X12
#define IDM_OUT_PARA_DATA				0x13         //�������߶�ֵ
#define IDM_OUT_CHANNEL_DATA			0x14         //����ͨ����Ϣ
#define IDM_EXPORT_WARE_TEXT            0x15         //������������

#define WIDTH_OF_RIGHT_BAR           280
#define WIDTH_PROJECT 230
#define HEIGHT_INFO 100

#define KZ_EVENT_TYPE                   0X01  //�����¼�
#define LB_EVENT_TYPE                   0X02  //¼���¼�
#define SVERROR_EVENT_TYPE              0X03  //SV�����¼�

//�����ʹ��VC6.0�Ľ�����Ʒ�񣬰��������ͼ���ֳ�������
//��������������������ϣ����е�һ�зֳ����У��ڶ���Ϊһ��
//��һ�е������е�һ��Ϊ������ͼ������Ҫ��һ����״�ṹ
//��һ�еĵڶ���Ϊ���������������û��Թ�����ͼ���и����Ĳ�����ͬ����ʾ���õ���ͼ
//�ڶ��бȽϵĸ߶ȱȽ��£��ڱ�����г�Ϊ��Ϣ��ͼ����ֻ���ڲ�����ʾ�Ͳ��η���ʱ����ʾ������Ϣ
//ƽʱ����ʾ��Ϣ������ʾ����Ϣ��Ч��
//������ͼ����Ϣ��ͼ��ʹ�ø������ڣ��û����������Ϸ���Щ��ͼ���ڣ��û�������������Щ��ͼ������ʾ
class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
// Attributes
public:
// Operations
public:
	CMultiTree		m_wndTree;         //������ͼ�е����ṹ
	CCoolBar		m_wndLeftBar;      //������ͼ���ĸ�������
	CCoolBar		m_wndRightBar;     //�ұ���ͼ���ĸ�������
	CCoolBar		m_wndBottonBar;    //��Ϣ��ͼ���ĸ�������
	CBottonTabCtrl	m_TabCtrl;         //��Ϣ��ͼ�������ɶ�ҳʽ��ʾ�Ŀؼ�
	CListCtrl       m_listParaInfo;    //��ֵ��Ϣ�б�
	CEdit           m_editWareFileInfo;          //��Ϣ��ͼ���е��ļ���Ϣҳ��

	CListCtrl       m_listSyncPara;     //ͬ�ڲ���
	CListCtrl       m_listState;        //״̬��Ϣ�б�	
	CCoolTabCtrl    m_wndRightTabCtrl;  //���������еĶ�ҳ��ʾ�ؼ�
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
	public:
		void OpenNewFile(CString strFileName);
	float m_fSVolSet;           //ϵͳ����ѹ
	float m_fGVolSet;           //��������ѹ 
	CString m_strDevice;
	int m_CloseTimeSet;         //��·����բʱ��
	int m_SystemAngleSet;       //ϵͳ��Ӧת��
	void ShowCurve();
	static CToolBarEx    m_wndMainToolBar;      //���ϵͳ���½����򿪡���ȡ¼���ļ�����ȡ�¼���Ϣ�ȵĹ�����
	BOOL m_bTelControl;
	BOOL m_bStartSync;
	BOOL m_bInProject;
	int m_nMultiFactor;
	static VOID StopCommunication();
	VOID OpenCommunication();
	
	static BOOL GetParaFile(const CString & strFileName,CListCtrl * pList);
	void ClearSyncList();
	//������ʾ����
	void CurveDisSet();
	//����װ���빤���ļ������ӣ�������Ӳ��ɹ���������ȡ¼���ļ����¼���Ϣ�Ĺ���ͼ����Ϊ����
	//������FALSE�����Գɹ���������ȡ¼���ļ����¼���Ϣ�Ĺ���ͼ����Ϊ����״̬��������TRUE
	BOOL TestConnection();
	//���������ڵ���Excel�ļ�
	//strFileName��      �洢Excel�ļ����ļ�·��
	//pComboBox:         ���ڻ�ȡ������Ϣ������Excel�ļ�ָ������Ϣ���ɲ��õı�
	//pList:             ���ڴ���Excel�ļ�Ҫ�����������б�
	//bIsSaveSelected��  �Ƿ�ֻ����ѡ�е��б������ݣ����ΪFALSE������pList�б��е����б���
	static BOOL GetExcelFile(const CString & strFileName,CComboBox * pComboBox,
		CListCtrl * pList,BOOL bIsSaveSelected=FALSE,BOOL bAllInOneTable=FALSE);
	//�����¹��̵Ĵ����ɹ��󣬻����´򿪱�Ĺ���ʱ������ԭ���Ѿ��򿪵Ĺ���
	void ReleaseProject();
	//����չ��������ͼ����״�Ϻ���strItemText�ı��Ľ������û�����ˣ�����FASLE
	BOOL ExpendTreeItem(CString strItemText);

	//״̬��
	CStatusBarEx  m_wndStatusBar;   
	//��ǰ�򿪵�¼���ļ���Ϣ��
	CWareFileData m_wareFileData;
	//�һ�¼���ļ�ʱ��¼��¼���ļ����ڹ����ļ��е���������Ϊ������ͼ�е�¼���ļ���˳���빤���ļ���
	//��¼���ļ���Ϣ��˳��պ��෴���������������ֵ���ڹ�����ͼ�еĵ����ڼ���¼���ļ�����Ȼ�Ǵ�0��ʼ
	int m_nRClickWareFileIndex;
	//���ڸı���������ͼ������Ҫ��ʾ����ͼ��
	BOOL ReplaceView(CRuntimeClass *pViewClass);

	//��Ϣ��ͼ���С�ͨ����Ϣ����ҳ
	CListCtrl m_listChannelInfo;

	//2016-10-20��Ϣ��ͼ���С�¼���¼�����ҳ
	CWareEventList m_listWareEvent;
	//2016-10-27 pwq
	CWareFileData* GetWaveFileDataPtr(){return &m_wareFileData;}
	void LoadProjectData();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

	CToolBarEx    m_wndToolBar;          //���ϵͳ�����ڲ������ߵķ������ܹ�����
	CFont         m_editFont; //����Ϣ��ͼ����
private:
	BOOL TranErrorInfo(int errorId);
	// Generated message map functions
protected:
	int m_nVMulti;              //������ʾ�ͷ����д�ֱ����ϵ������ͬ�ķ���ϵ����Ӧ��
								 //ͬ��ģ�����Ϳ��뿪��������ϵ�Ĵ�ֱ�߶�
	int m_nHMulti;              //ˮƽ����ϵ���������ϵ����ʾˮƽ����Ϊ��С����ʾ�Ķ��ٱ�

	CImageList m_imageList;
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewProject();                          //��������
	afx_msg void OnProject();                             //��ʾ���ع�����ͼ��
	afx_msg void OnInfoWnd();                             //��ʾ������Ϣ��ͼ��
	afx_msg void OnOpenProject();                         //�򿪹���
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);//�һ�������ͼ���е������
	afx_msg void OnRclickParalist(NMHDR* pNMHDR, LRESULT* pResult);//�һ�¼���ļ���Ϣ�еĶ�ֵ�б�
	afx_msg void OnRclickChanlist(NMHDR* pNMHDR, LRESULT* pResult);//�һ�¼���ļ���Ϣ�е�ͨ����Ϣ�б�
	afx_msg void OnShowEvent();                            //��ʾ�¼���Ϣ
	afx_msg void OnGetEvent();                             //��װ���л�ȡ�¼���Ϣ
	afx_msg void OnSetProject();                           //�������ã�����ֻ���޸���װ��ͨ�ŵ�IP��ַ
	afx_msg void OnTestDeviceConnection();                 //�������װ������ʱ�������¼�
	afx_msg void OnShowCurve();                            //������ʾ
	afx_msg void OnCurveUnite();						   //���κϲ�
	afx_msg void OnDeleteOutToList();                      //ɾ��¼���ļ����б�
	afx_msg void OnDeleteFile();                           //����ɾ��¼���ļ�
    afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult); //���������ͼ���е������ʱ����
	afx_msg void OnGetWareFile();                          //��ȡ¼���ļ�
	afx_msg void OnOpenWareFile();                         //���¼���ļ��빤����
	afx_msg void OnCurveHBigger();                         //ˮƽ�Ŵ�������
	afx_msg void OnCurveHSmaller();                        //ˮƽ��С��������
	afx_msg void OnCurveVBigger();                         //��ֱ�Ŵ�������
	afx_msg void OnCurveVSmaller();                        //��ֱ��С��������
//	afx_msg void OnCurveAnalyse();                         //¼������
	afx_msg void OnOpenFile();                             
	afx_msg void OnReadParaData();                           //��ȡװ�ö�ֵ
	afx_msg void OnInParaFile();                             //��ȡ��ֵ�ļ�
	afx_msg void OnOutParaData();                            //����¼�����߱�����ֵ��Excel�ļ���
	afx_msg void OnOutChannelData();                        //����¼������ͨ����Ϣ��Excel�ļ���
	afx_msg void OnViewToolbar();                           //��ʾ�����ع�����
	afx_msg void OnOutParaToExcel();                        //����������ֵ��Excel�ļ���
	afx_msg void OnOutRealDataToExcel();                    //����ʵ�����ݵ�Excel�ļ���
	afx_msg void OnOutWareFileToComtrade();                 //��comtrade��׼�ļ���ʽ��¼���ļ�����
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnModifyPsw();
	afx_msg void OnMulti2();
	afx_msg void OnMulti4();
	afx_msg void OnMulti1();
	afx_msg void OnInEventFile();
	afx_msg void OnOutEvent();
	afx_msg void OnRestart();
	afx_msg void OnRightBar();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnDefineSelf();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__5D30DB11_2EC6_4356_8D7F_17B196C78492__INCLUDED_)
