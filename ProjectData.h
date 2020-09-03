// ProjectData.h: interface for the CProjectData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTDATA_H__F883F9A6_95DA_44CA_944F_9AF0D5F94B1D__INCLUDED_)
#define AFX_PROJECTDATA_H__F883F9A6_95DA_44CA_944F_9AF0D5F94B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MAX_FILE_COUNT 200           //最大录波文件个数
#define MAX_FILE_NAME_LENGTH 64      //工程文件中工程名占及事件ACCESS文件名的字节数
#define MAX_STR_LENGTH 32            //工程文件中装置类型及录波启动类型所占的字节数
#define PROJECT_WARE_FILE 0x01
#define TEMP_WARE_FILE 0x02
#define OUT_WARE_FILE 0x03

#define CONNECT_DEVICE 0x01
#define CONNECT_BY_USB 0x02
#define CONNECT_BY_NET 0x04
//用于记录工程视图中录波文件子树中的结点项，它也是工程文件中的录波文件信息
struct FileTreeNode 
{
	char strFileName[MAX_FILE_NAME_LENGTH]; //录波文件的文件名（不含后缀）
	char strWareType[MAX_STR_LENGTH];       //录波启动类型
	COleDateTime dtStartTime;               //录波启动时间
	unsigned short nMillisecond;
	FileTreeNode & operator =(const FileTreeNode & ftn)
	{
		::memcpy(strFileName,ftn.strFileName,sizeof(ftn.strFileName));
		::memcpy(strWareType,ftn.strWareType,sizeof(ftn.strWareType));
		dtStartTime=ftn.dtStartTime;
		nMillisecond=ftn.nMillisecond;
		return *this;
	}
};
class CProjectData  
{
public:
	CProjectData();
	virtual ~CProjectData();

    static void  ConvertPath(CString & path);
    static BOOL DeleteDirectory(CString strDirName);

	BOOL TestConnection();
	//添加一个已经存在于工程录波文件目录下的录波文件于工程之中，strWareFileName只是文件名，不是路径
	BOOL AddWareFileToProject(CString strWareFileName);
	BOOL DeleteWareFileOutToProject(int index);
	BOOL SaveProjectSetting();
	BOOL OpenProject(CString filePath);
	CProjectData & operator =(const CProjectData &project);   //涉及数组，进行深拷贝
	void Inlitialize();
	BOOL DeleteWareFile(int index);
	BOOL ModifyWareFileName(CString fileName,int index);

	//添加录波文件入工程之中
	BOOL SaveTreeNodeToProject(int index);
	BOOL OpenProjectFile(CString strProjectFileTitle);
	//用于创建工程时制作并生成工程文件
	BOOL CreateProject(CString strDeviceType,CString strProjectName,
		CString strPath,BYTE ipAddr[4],int bIsTestConnect);
	BOOL CreateProjectFile();
public: 	//member
	static	BYTE m_LinkType;                         //设备连接方式

    CString m_strProjectFileTitle;                   //录波工程文件的文件名，不含后缀
	unsigned short m_nFileCount;                     //录波文件数N,2字节(由低到高),跟模拟量出现同样的问题
	BYTE m_arrIPAddr[4];                             //IP地址
	char m_strDeviceType[MAX_STR_LENGTH];            //装置类型
	char m_strProjectName[MAX_FILE_NAME_LENGTH];     //工程名称
	char m_strDBFileName[MAX_FILE_NAME_LENGTH];      //事件ACCESS文件名
	FileTreeNode m_arrfileTreeNode[MAX_FILE_COUNT];  //录波文件集合
	CString m_strWareFilePath;
	CString m_strProjectPath;
	unsigned int m_dwAnalogDef;                     //模拟量显示通道,规约4字节，若软件安在64位机，int表示就是8个字节是会导致出错，未做处理
	unsigned int m_dwDigtalInDef;                   //开入量要显示的通道规约4字节
	unsigned int m_dwDigtalOutDef;                  //开出量要显示的通道  
	unsigned int m_dwFrequencyDef;
	unsigned int m_dwAngleDiffDef;
};

#endif // !defined(AFX_PROJECTDATA_H__F883F9A6_95DA_44CA_944F_9AF0D5F94B1D__INCLUDED_)
