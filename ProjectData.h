// ProjectData.h: interface for the CProjectData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTDATA_H__F883F9A6_95DA_44CA_944F_9AF0D5F94B1D__INCLUDED_)
#define AFX_PROJECTDATA_H__F883F9A6_95DA_44CA_944F_9AF0D5F94B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MAX_FILE_COUNT 200           //���¼���ļ�����
#define MAX_FILE_NAME_LENGTH 64      //�����ļ��й�����ռ���¼�ACCESS�ļ������ֽ���
#define MAX_STR_LENGTH 32            //�����ļ���װ�����ͼ�¼������������ռ���ֽ���
#define PROJECT_WARE_FILE 0x01
#define TEMP_WARE_FILE 0x02
#define OUT_WARE_FILE 0x03

#define CONNECT_DEVICE 0x01
#define CONNECT_BY_USB 0x02
#define CONNECT_BY_NET 0x04
//���ڼ�¼������ͼ��¼���ļ������еĽ�����Ҳ�ǹ����ļ��е�¼���ļ���Ϣ
struct FileTreeNode 
{
	char strFileName[MAX_FILE_NAME_LENGTH]; //¼���ļ����ļ�����������׺��
	char strWareType[MAX_STR_LENGTH];       //¼����������
	COleDateTime dtStartTime;               //¼������ʱ��
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
	//���һ���Ѿ������ڹ���¼���ļ�Ŀ¼�µ�¼���ļ��ڹ���֮�У�strWareFileNameֻ���ļ���������·��
	BOOL AddWareFileToProject(CString strWareFileName);
	BOOL DeleteWareFileOutToProject(int index);
	BOOL SaveProjectSetting();
	BOOL OpenProject(CString filePath);
	CProjectData & operator =(const CProjectData &project);   //�漰���飬�������
	void Inlitialize();
	BOOL DeleteWareFile(int index);
	BOOL ModifyWareFileName(CString fileName,int index);

	//���¼���ļ��빤��֮��
	BOOL SaveTreeNodeToProject(int index);
	BOOL OpenProjectFile(CString strProjectFileTitle);
	//���ڴ�������ʱ���������ɹ����ļ�
	BOOL CreateProject(CString strDeviceType,CString strProjectName,
		CString strPath,BYTE ipAddr[4],int bIsTestConnect);
	BOOL CreateProjectFile();
public: 	//member
	static	BYTE m_LinkType;                         //�豸���ӷ�ʽ

    CString m_strProjectFileTitle;                   //¼�������ļ����ļ�����������׺
	unsigned short m_nFileCount;                     //¼���ļ���N,2�ֽ�(�ɵ͵���),��ģ��������ͬ��������
	BYTE m_arrIPAddr[4];                             //IP��ַ
	char m_strDeviceType[MAX_STR_LENGTH];            //װ������
	char m_strProjectName[MAX_FILE_NAME_LENGTH];     //��������
	char m_strDBFileName[MAX_FILE_NAME_LENGTH];      //�¼�ACCESS�ļ���
	FileTreeNode m_arrfileTreeNode[MAX_FILE_COUNT];  //¼���ļ�����
	CString m_strWareFilePath;
	CString m_strProjectPath;
	unsigned int m_dwAnalogDef;                     //ģ������ʾͨ��,��Լ4�ֽڣ����������64λ����int��ʾ����8���ֽ��ǻᵼ�³���δ������
	unsigned int m_dwDigtalInDef;                   //������Ҫ��ʾ��ͨ����Լ4�ֽ�
	unsigned int m_dwDigtalOutDef;                  //������Ҫ��ʾ��ͨ��  
	unsigned int m_dwFrequencyDef;
	unsigned int m_dwAngleDiffDef;
};

#endif // !defined(AFX_PROJECTDATA_H__F883F9A6_95DA_44CA_944F_9AF0D5F94B1D__INCLUDED_)
