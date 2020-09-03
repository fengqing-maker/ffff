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
#define IDM_ACHROMATIC             0X0A         //去色，让曲线按黑白显示
#define IDM_DYEING                 0X0B         //上色，让曲线彩色显示 
#define IDM_CURVE_DIS_SET          0x0c         //曲线显示设置
#define IDM_CURVE_UNITE          0x0d         //曲线合并
#define IDM_SHOW_VALID          0x0e         //仅显示有效值
#define IDM_SHOW_SAMPLE          0x0f         //仅显示采样值
#define IDM_SHOW_ALL          0x10         //有效值和采样值均显示
#define IDM_READ_PARA_DATA 0X11
#define IDM_IN_PARA_FILE 0X12
#define IDM_OUT_PARA_DATA				0x13         //导出曲线定值
#define IDM_OUT_CHANNEL_DATA			0x14         //导出通道信息
#define IDM_EXPORT_WARE_TEXT            0x15         //导出波形数据

#define WIDTH_OF_RIGHT_BAR           280
#define WIDTH_PROJECT 230
#define HEIGHT_INFO 100

#define KZ_EVENT_TYPE                   0X01  //控制事件
#define LB_EVENT_TYPE                   0X02  //录波事件
#define SVERROR_EVENT_TYPE              0X03  //SV错误事件

//本软件使用VC6.0的界面设计风格，把软件的视图区分成三部，
//从纵向来看，由两行组合，其中第一行分成两列，第二行为一列
//第一行的两列中第一列为工程视图区，主要是一个树状结构
//第一行的第二列为主工作区，它因用户对工程视图区中各结点的操作不同而显示不用的视图
//第二行比较的高度比较下，在本软件中称为信息视图区，只有在波形显示和波形分析时会显示有用信息
//平时不显示信息，或显示的信息无效。
//工程视图和信息视图都使用浮动窗口，用户可以随意拖放这些视图窗口，用户还可以隐藏这些视图区的显示
class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
// Attributes
public:
// Operations
public:
	CMultiTree		m_wndTree;         //工程视图中的树结构
	CCoolBar		m_wndLeftBar;      //工程视图区的浮动窗口
	CCoolBar		m_wndRightBar;     //右边视图区的浮动窗口
	CCoolBar		m_wndBottonBar;    //信息视图区的浮动窗口
	CBottonTabCtrl	m_TabCtrl;         //信息视图区中生成多页式显示的控件
	CListCtrl       m_listParaInfo;    //定值信息列表
	CEdit           m_editWareFileInfo;          //信息视图区中的文件信息页面

	CListCtrl       m_listSyncPara;     //同期参数
	CListCtrl       m_listState;        //状态信息列表	
	CCoolTabCtrl    m_wndRightTabCtrl;  //浮动窗口中的多页显示控件
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
	public:
		void OpenNewFile(CString strFileName);
	float m_fSVolSet;           //系统侧额定电压
	float m_fGVolSet;           //待并侧额定电压 
	CString m_strDevice;
	int m_CloseTimeSet;         //断路器合闸时间
	int m_SystemAngleSet;       //系统侧应转角
	void ShowCurve();
	static CToolBarEx    m_wndMainToolBar;      //软件系统中新建、打开、提取录波文件、提取事件信息等的工具栏
	BOOL m_bTelControl;
	BOOL m_bStartSync;
	BOOL m_bInProject;
	int m_nMultiFactor;
	static VOID StopCommunication();
	VOID OpenCommunication();
	
	static BOOL GetParaFile(const CString & strFileName,CListCtrl * pList);
	void ClearSyncList();
	//曲线显示设置
	void CurveDisSet();
	//测试装置与工程文件的连接，如果连接不成功，则会把提取录波文件和事件信息的工具图标设为禁用
	//并返回FALSE，测试成功，则会把提取录波文件和事件信息的工具图标设为可用状态，并返回TRUE
	BOOL TestConnection();
	//本函数用于导出Excel文件
	//strFileName：      存储Excel文件的文件路径
	//pComboBox:         用于获取类型信息，便于Excel文件指类型信息生成不用的表单
	//pList:             用于存诸Excel文件要导出的数据列表
	//bIsSaveSelected：  是否只保存选中的列表项数据，如果为FALSE将保存pList列表中的所有表项
	static BOOL GetExcelFile(const CString & strFileName,CComboBox * pComboBox,
		CListCtrl * pList,BOOL bIsSaveSelected=FALSE,BOOL bAllInOneTable=FALSE);
	//用于新工程的创建成功后，或重新打开别的工程时，析构原来已经打开的工程
	void ReleaseProject();
	//用于展开工程视图中树状上含有strItemText文本的结点项，如果没有找了，返回FASLE
	BOOL ExpendTreeItem(CString strItemText);

	//状态栏
	CStatusBarEx  m_wndStatusBar;   
	//当前打开的录波文件信息类
	CWareFileData m_wareFileData;
	//右击录波文件时记录下录波文件的在工程文件中的索引，因为工程视图中的录波文件的顺序与工程文件中
	//的录波文件信息的顺序刚好相反，所以这里的索引值等于工程视图中的倒数第几个录波文件，当然是从0开始
	int m_nRClickWareFileIndex;
	//用于改变主工作视图区中所要显示的视图类
	BOOL ReplaceView(CRuntimeClass *pViewClass);

	//信息视图区中“通道信息”子页
	CListCtrl m_listChannelInfo;

	//2016-10-20信息视图区中“录波事件”子页
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

	CToolBarEx    m_wndToolBar;          //软件系统中用于波形曲线的放缩功能工具栏
	CFont         m_editFont; //底信息视图字体
private:
	BOOL TranErrorInfo(int errorId);
	// Generated message map functions
protected:
	int m_nVMulti;              //波形显示和分析中垂直放缩系数，不同的放缩系数对应不
								 //同的模拟量和开入开出量坐标系的垂直高度
	int m_nHMulti;              //水平放缩系数，这里的系数表示水平放缩为最小化显示的多少倍

	CImageList m_imageList;
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewProject();                          //创建工程
	afx_msg void OnProject();                             //显示隐藏工程视图区
	afx_msg void OnInfoWnd();                             //显示隐藏信息视图区
	afx_msg void OnOpenProject();                         //打开工程
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);//右击工程视图区中的树结点
	afx_msg void OnRclickParalist(NMHDR* pNMHDR, LRESULT* pResult);//右击录波文件信息中的定值列表
	afx_msg void OnRclickChanlist(NMHDR* pNMHDR, LRESULT* pResult);//右击录波文件信息中的通道信息列表
	afx_msg void OnShowEvent();                            //显示事件信息
	afx_msg void OnGetEvent();                             //从装置中获取事件信息
	afx_msg void OnSetProject();                           //工程设置，这里只能修改与装置通信的IP地址
	afx_msg void OnTestDeviceConnection();                 //点击测试装置连接时触发的事件
	afx_msg void OnShowCurve();                            //波形显示
	afx_msg void OnCurveUnite();						   //波形合并
	afx_msg void OnDeleteOutToList();                      //删除录波文件出列表
	afx_msg void OnDeleteFile();                           //彻底删除录波文件
    afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult); //点击工程视图区中的树点结时触发
	afx_msg void OnGetWareFile();                          //提取录波文件
	afx_msg void OnOpenWareFile();                         //添加录波文件入工程中
	afx_msg void OnCurveHBigger();                         //水平放大波形曲线
	afx_msg void OnCurveHSmaller();                        //水平缩小波形曲线
	afx_msg void OnCurveVBigger();                         //垂直放大波形曲线
	afx_msg void OnCurveVSmaller();                        //垂直缩小波形曲线
//	afx_msg void OnCurveAnalyse();                         //录波分析
	afx_msg void OnOpenFile();                             
	afx_msg void OnReadParaData();                           //读取装置定值
	afx_msg void OnInParaFile();                             //读取定值文件
	afx_msg void OnOutParaData();                            //导出录波曲线保护定值到Excel文件中
	afx_msg void OnOutChannelData();                        //导出录波曲线通道信息到Excel文件中
	afx_msg void OnViewToolbar();                           //显示或隐藏工具栏
	afx_msg void OnOutParaToExcel();                        //导出保护定值到Excel文件中
	afx_msg void OnOutRealDataToExcel();                    //导出实测数据到Excel文件中
	afx_msg void OnOutWareFileToComtrade();                 //按comtrade标准文件格式将录波文件导出
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
