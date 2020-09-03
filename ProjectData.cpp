// ProjectData.cpp: implementation of the CProjectData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SID_2FY.h"
#include "ProjectData.h"
#include "Frame.h"
#include "Mainfrm.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern CMySID_2FYApp theApp;
BYTE CProjectData::m_LinkType = 1;
CProjectData::CProjectData()
{
	Inlitialize();
	memset(m_arrIPAddr,0,sizeof(m_arrIPAddr));
	m_nFileCount=0;
	memset(m_strDBFileName,0,sizeof(m_strDBFileName));
	memset(m_strDeviceType,0,sizeof(m_strDeviceType));
	memset(m_strProjectName,0,sizeof(m_strProjectName));

	m_dwAnalogDef = 0;
	m_dwAngleDiffDef = 0;
	m_dwDigtalInDef = 0;
	m_dwDigtalOutDef = 0;
	m_dwFrequencyDef = 0;
}

CProjectData::~CProjectData()
{

}
/********************************************************************************
函数名：CreateProject
描述：创建工程
参数：strDeviceType[in]:设备类型；strProjectName[in]:工程名称；
      strPath[in]:工程目录路径；ipAddr[in]：设备Ip地址；
	  bIsTestConnect[in]：选上了创建时连接装置和连接设备方式
返回：创建成功返回TRUE,否则返回FALSE
修改记录：
*********************************************************************************/
BOOL CProjectData::CreateProject(CString strDeviceType,CString strProjectName,
		CString strPath,BYTE ipAddr[4],int bIsTestConnect)
{
	m_LinkType = (BYTE)bIsTestConnect;
	BOOL bProjectCreateSuc=TRUE;
	WideCharToMultiByte(CP_ACP, 0, strProjectName, -1, m_strProjectName, sizeof(m_strProjectName),NULL,NULL);
	
	WideCharToMultiByte(CP_ACP, 0, strDeviceType, -1, m_strDeviceType, sizeof(m_strDeviceType),NULL,NULL);
	memcpy(m_arrIPAddr,ipAddr,sizeof(m_arrIPAddr));
	
	
	CFileFind fileFind;
	CString tempPath=strPath+_T("\\");
	CString info;
	if(fileFind.FindFile(strPath))
	{
		tempPath.LoadString( IDS_PRJ_DIR );
        info.LoadString( IDS_PATH_INFO );
		tempPath += strPath + info;
		CdMessageBox box(theApp.m_pMainWnd, tempPath, _T(""), MB_OK);
        box.MsgBoxEx(); 
		return FALSE;
	}
	ConvertPath(tempPath);
	//创建工程目录
	if(CreateDirectory(tempPath,NULL))
	{
		HRESULT   hr   =   S_OK;   
		_CatalogPtr   pCatalog   =   NULL;
		CString   strcnn;
		sprintf(m_strDBFileName,"%sDatabase.mdb",strProjectName);
		strcnn.Format(_T("Provider=Microsoft.JET.OLEDB.4.0;Data source=%s%s"),tempPath,
			(CString)m_strDBFileName); 
		try 
		{
			//创建事件数据库
			hr   =   pCatalog.CreateInstance(__uuidof(Catalog)); 
			if(FAILED(hr)) 
			{
				_com_issue_error(hr);
				bProjectCreateSuc=FALSE;
			}
			else
			{
				pCatalog->Create(_bstr_t(strcnn));   //Create   MDB
				((_ConnectionPtr )pCatalog->GetActiveConnection())->Close();
			}
			CString sqlCreateTable;
			sqlCreateTable = _T("create table [event]([EventName] text(50) with COMPRESSION not null,[EventType] \
				text(50) with compression not null,[HappenTime] datetime not null,[Millisecond] integer not null,\
				[Param] text(50) not null,CONSTRAINT CK_CH_Milli Check([Millisecond]>-1 and [Millisecond]<100000),\
				CONSTRAINT PK_ID Primary Key ([EventName],[EventType],[HappenTime],[Millisecond]))");
			if(theApp.m_adoDatabase.IsOpen())
			{
				theApp.m_adoDatabase.Close();
			}
			if(!theApp.m_adoDatabase.Open(strcnn,adModeUnknown))
			{
                info.LoadString( IDS_DATABASE_CONNECT );
				CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
                box.MsgBoxEx(); 
				bProjectCreateSuc=FALSE;
			}
			//创建数据库表
			theApp.m_adoDatabase.Execute(sqlCreateTable);
		}
		catch(...) 
		{
			//   Notify   the   user   of   errors   if   any. 
			info.LoadString( IDS_DATABASE_CREATE );
			CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
            box.MsgBoxEx();
			bProjectCreateSuc=FALSE;
		}
		//创建保存录波波文的wareFile文件夹
		CString warePath=tempPath+_T("waveFile\\\\");
		if(CreateDirectory(warePath,NULL))
		{
			m_strWareFilePath=warePath;
			m_strProjectPath=tempPath;
			if(!CreateProjectFile())
			{
				bProjectCreateSuc=FALSE;
			}
		}
		else
		{
			bProjectCreateSuc=FALSE;
		}
		//如果用户选择了创建时连接置，这里将测试装置的连接，如果连接失败，系统会以工程创建失败论处
		//bIsTestConnect?(bProjectCreateSuc = TestConnection()):NULL;
		if (bIsTestConnect&CONNECT_DEVICE)
		{
			bProjectCreateSuc = TestConnection();
		}
	}
	else
	{
		return FALSE;
	}
	//如果工程文件夹创建成功后，而文件件下的内容创建有错，而删除此文件夹和其下的所有文件
	if(!bProjectCreateSuc)
	{
		info.LoadString( IDS_PRJCREATE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		if(theApp.m_adoDatabase.IsOpen())
		{
			theApp.m_adoDatabase.Close();
		}
		DeleteDirectory(tempPath);
		return FALSE;
	}
	return TRUE;
}

/********************************************************************************
函数名：CreateProjectFile
描述：创建工程文件
参数：无
返回：创建成功返回TRUE,否则返回FALSE
修改记录：
*********************************************************************************/	
BOOL CProjectData::CreateProjectFile()
{
	BYTE fileBuf[200];
	memset(fileBuf,0,sizeof(fileBuf));
	int fileLength=0,curPos,temLen;
	FILE *dataFile;
	CString fileName, info;
	fileName.Format(_T("%s%s.prj"),m_strProjectPath,(CString)m_strProjectName);
	dataFile=_wfopen(fileName,_T("wb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	m_strProjectFileTitle=m_strProjectName;
	//工程名
	curPos=0;
	temLen=sizeof(m_strProjectName);
	memcpy(fileBuf,m_strProjectName,temLen);
	curPos+=temLen;
	//装置类型
	temLen=sizeof(m_strDeviceType);
	memcpy(fileBuf+curPos,m_strDeviceType,temLen);
	curPos+=temLen;
	//连接设备方式
	temLen = sizeof(m_LinkType);
	if (m_LinkType&CONNECT_BY_NET)
	{
		memset(fileBuf+curPos,CONNECT_BY_NET,temLen);
	}
	else if (m_LinkType&CONNECT_BY_USB)
	{
		memset(fileBuf+curPos,CONNECT_BY_USB,temLen);
	}
	curPos+=temLen;
	//IP地址
	temLen=sizeof(m_arrIPAddr);
	memcpy(fileBuf+curPos,m_arrIPAddr,temLen);
	curPos+=temLen;

	//事件ACCESS文件名
	temLen=sizeof(m_strDBFileName);
	memcpy(fileBuf+curPos,m_strDBFileName,temLen);
	curPos+=temLen;
 
	/////用于保存各子项默认显示的通道号
	DWORD showChannel = 0;
	temLen = sizeof(showChannel);
	memcpy(fileBuf+curPos,&showChannel,temLen);
	curPos+=temLen;
	memcpy(fileBuf+curPos,&showChannel,temLen);
	curPos+=temLen;
	memcpy(fileBuf+curPos,&showChannel,temLen);
	curPos+=temLen;
	memcpy(fileBuf+curPos,&showChannel,temLen);
	curPos+=temLen;
	memcpy(fileBuf+curPos,&showChannel,temLen);
	curPos+=temLen;
	
	temLen=sizeof(m_nFileCount);  //录波文件个数
	memcpy(fileBuf+curPos,&m_nFileCount,temLen);
	curPos+=temLen;
	fileLength=curPos;
	fwrite(fileBuf,sizeof(unsigned char),fileLength,dataFile);
	fclose(dataFile);
    dataFile=NULL;
	return TRUE;
}
/****************************************************************
功能：打开工程文件，读取文件内容
返回：bool 是否打开工程成功
****************************************************************/
BOOL CProjectData::OpenProjectFile(CString strProjectFileTitle)
{
	CString path,str,info;
	path.Format(_T("%s%s.prj"),m_strProjectPath,strProjectFileTitle);
	FILE *dataFile;
	dataFile=_wfopen(path,_T("rb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	m_strProjectFileTitle=strProjectFileTitle;
	fseek(dataFile,0,SEEK_END);
	int filelength=ftell(dataFile);   //ftell返回long值，文件所在位置的字节数
	BYTE *fileBuf=new BYTE[filelength+10];
	memset(fileBuf,0,filelength+10);
	fseek(dataFile,0,SEEK_SET);
	int temp=fread(fileBuf,sizeof(BYTE),filelength+10,dataFile);
	fclose(dataFile);
	if(filelength!=temp)  //工程文件内容未读取完返回失败
	{
		delete [] fileBuf;
		return FALSE;
	}
	//读工程名
	int curPos=0;
	temp=sizeof(m_strProjectName);
	memcpy(m_strProjectName,fileBuf,temp);

    //读装置类型
	curPos+=temp;
	temp=sizeof(m_strDeviceType);
	memcpy(m_strDeviceType,fileBuf+curPos,temp);
	
	curPos+=temp;
	str = m_strDeviceType;
	theApp.m_tempItem = theApp.m_mapDiviceType.find(str);
	if(theApp.m_tempItem == theApp.m_mapDiviceType.end())  //查看软件是否支持打开此装置类型
	{
		CString strTemp;
		info.LoadString( IDS_UNKOWN_TYPE );
		strTemp.Format(info, str);
		CdMessageBox box(theApp.m_pMainWnd, strTemp, _T(""), MB_OK);
        box.MsgBoxEx();
		delete [] fileBuf;
		return FALSE;
	}
    
	//读取设备连接方式
	temp = sizeof(m_LinkType);
	m_LinkType = fileBuf[curPos];
//	memcpy(&m_LinkType,fileBuf+curPos,temp);
    
	//读取IP地址
	curPos+=temp;
	temp=sizeof(m_arrIPAddr);
	m_arrIPAddr[0] = fileBuf[curPos] ;
	m_arrIPAddr[1] = fileBuf[curPos+1];
	m_arrIPAddr[2] = fileBuf[curPos+2];
	m_arrIPAddr[3] = fileBuf[curPos+3];
//	memcpy(m_arrIPAddr,fileBuf+curPos,temp);

	//事件ACCESS文件名
	curPos+=temp;
	temp=sizeof(m_strDBFileName);
	memcpy(m_strDBFileName,fileBuf+curPos,temp);
    
	//模拟量显示通道，规约文件中此占文件4字节，m_dwAnalogDef时int类型不同的机器长度不一至,
	//建议使用常量，而不是用sizeof，存入文件同理
	curPos+=temp;
	temp=sizeof(m_dwAnalogDef);  //默认显示的各子项通道
	memcpy(&theApp.m_projectData.m_dwAnalogDef,fileBuf+curPos,temp);

	//开入量要显示的通道，4字节
	curPos+=temp;
	memcpy(&theApp.m_projectData.m_dwDigtalInDef,fileBuf+curPos,temp);

	//开出量要显示的通道，4字节
    curPos+=temp;
	memcpy(&theApp.m_projectData.m_dwDigtalOutDef,fileBuf+curPos,temp);
	
    //频率量要显示的通道，4字节
	curPos+=temp;
	memcpy(&theApp.m_projectData.m_dwFrequencyDef,fileBuf+curPos,temp);

	//角差量要显示的通道，频率量要显示的通道，4字节
	curPos+=temp;
	memcpy(&theApp.m_projectData.m_dwAngleDiffDef,fileBuf+curPos,temp);
    
	//2字节(由低到高)
	curPos+=temp;
    temp=sizeof(m_nFileCount);
	memcpy(&m_nFileCount,fileBuf+curPos,temp);

	curPos+=temp;
	if(m_nFileCount>MAX_FILE_COUNT)
	{
		info.LoadString( IDS_MAX_FILENUM );
		str.Format(info, MAX_FILE_COUNT,m_nFileCount);
		CdMessageBox box(theApp.m_pMainWnd, str, _T(""), MB_OK);
        box.MsgBoxEx();
		delete [] fileBuf;
		return FALSE;
	}
	//计算逻辑上主文件长度，187：以上读取变量所占字节总和，105录波文件所占字节数的总和，包括录波文件名，录波文件的启动类型，
	//录波启动时间
	temp=187+m_nFileCount*105; 
	ASSERT(filelength == temp);
	if (filelength != temp)
	{
		str.LoadString( IDS_PRJFILE_BROKEN );
		CdMessageBox box(theApp.m_pMainWnd, str, _T(""), MB_OK);
        box.MsgBoxEx();
		delete [] fileBuf;
		return FALSE;
	}
	int i;
	for(i=0;i<m_nFileCount;i++)
	{
		//录波文件名
		temp=sizeof(m_arrfileTreeNode[i].strFileName);
		memcpy(m_arrfileTreeNode[i].strFileName,fileBuf+curPos,temp);
		
		//录波文件的启动类型
		curPos+=temp;
		temp=sizeof(m_arrfileTreeNode[i].strWareType);
		memset(m_arrfileTreeNode[i].strWareType,0,temp);
		memcpy(m_arrfileTreeNode[i].strWareType,fileBuf+curPos,temp);

		curPos+=temp;
		//读取时间
		unsigned short year;
		BYTE month,day,hour,minute,second;
		temp = sizeof(year);
		memcpy(&year,fileBuf+curPos,temp);
		curPos+=temp;
		temp=sizeof(month);
		memcpy(&month,fileBuf+curPos,temp);
		curPos += temp;
		temp=sizeof(day);
		memcpy(&day,fileBuf+curPos,temp);
		curPos += temp;
		temp=sizeof(hour);
		memcpy(&hour,fileBuf+curPos,temp);
		curPos += temp;
		temp=sizeof(minute);
		memcpy(&minute,fileBuf+curPos,temp);
		curPos += temp;
		temp=sizeof(second);
		memcpy(&second,fileBuf+curPos,temp);
		curPos += temp;
		temp=sizeof(m_arrfileTreeNode[i].nMillisecond);
		memcpy(&m_arrfileTreeNode[i].nMillisecond,fileBuf+curPos,temp);
		curPos += temp;
		m_arrfileTreeNode[i].dtStartTime.SetDateTime(year,month,day,hour,minute,second);
	}
	delete [] fileBuf;
	fileBuf=NULL;
	return TRUE;
}

BOOL CProjectData::SaveTreeNodeToProject(int index)
{
	ASSERT(index == m_nFileCount);
	CString path, info;
	path.Format(_T("%s%s.prj"),m_strProjectPath,m_strProjectFileTitle);
	FILE *dataFile;
	dataFile=_wfopen(path,_T("rb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	fseek(dataFile,0,SEEK_END);
	int filelength=ftell(dataFile);
	BYTE *fileBuf=new BYTE[filelength+sizeof(m_arrfileTreeNode[index])+100];
	memset(fileBuf,0,filelength+sizeof(m_arrfileTreeNode[index])+100);
	fseek(dataFile,0,SEEK_SET);
	int temp=fread(fileBuf,sizeof(BYTE),filelength+sizeof(m_arrfileTreeNode[index])+10,dataFile);
	fclose(dataFile);
	if(filelength!=temp)
	{
		delete [] fileBuf;
		info.LoadString( IDS_PRJFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	//计算逻辑上主文件长度
	temp=187+m_nFileCount*105;
	ASSERT(filelength == temp);
	m_nFileCount++;
	memcpy(fileBuf+185,&m_nFileCount,sizeof(m_nFileCount));
	//将录波文件信息填入工程文件中
	int curPos = temp;
	temp=sizeof(m_arrfileTreeNode[index].strFileName);
	memcpy(fileBuf+curPos,m_arrfileTreeNode[index].strFileName,temp);
	curPos+=temp;
	temp=sizeof(m_arrfileTreeNode[index].strWareType);
	memcpy(fileBuf+curPos,m_arrfileTreeNode[index].strWareType,temp);
	curPos+=temp;
	//写入时间
	unsigned short year;
	BYTE month,day,hour,minute,second;
	year=(unsigned short)m_arrfileTreeNode[index].dtStartTime.GetYear();
	temp = sizeof(year);
	memcpy(fileBuf+curPos,&year,temp);
	curPos+=temp;
	month = (BYTE)m_arrfileTreeNode[index].dtStartTime.GetMonth();
	temp=sizeof(month);
	memcpy(fileBuf+curPos,&month,temp);
	curPos += temp;
	day = (BYTE)m_arrfileTreeNode[index].dtStartTime.GetDay();
	temp=sizeof(day);
	memcpy(fileBuf+curPos,&day,temp);
	curPos += temp;
	hour = (BYTE)m_arrfileTreeNode[index].dtStartTime.GetHour();
	temp=sizeof(hour);
	memcpy(fileBuf+curPos,&hour,temp);
	curPos += temp;
	minute = (BYTE)m_arrfileTreeNode[index].dtStartTime.GetMinute();
	temp=sizeof(minute);
	memcpy(fileBuf+curPos,&minute,temp);
	curPos += temp;
	second = (BYTE)m_arrfileTreeNode[index].dtStartTime.GetSecond();
	temp=sizeof(second);
	memcpy(fileBuf+curPos,&second,temp);
	curPos += temp;
	temp=sizeof(m_arrfileTreeNode[index].nMillisecond);
	memcpy(fileBuf+curPos,&m_arrfileTreeNode[index].nMillisecond,temp);
	curPos += temp;
	dataFile=_wfopen(path,_T("wb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		delete [] fileBuf;
		return FALSE;
	}
	if(curPos == fwrite(fileBuf,sizeof(BYTE),curPos,dataFile))
	{
		fclose(dataFile);
		delete [] fileBuf;
		
		return TRUE;
	}
	fclose(dataFile);
	delete [] fileBuf;
	info.LoadString( IDS_ADDFILE_PRJ );
	CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
    box.MsgBoxEx();
	return FALSE;
}

BOOL CProjectData::ModifyWareFileName(CString fileName,int index)
{
	ASSERT(index >=0 && index < m_nFileCount);
	char tempFileName[255];
	WideCharToMultiByte(CP_ACP,0,fileName,-1,tempFileName,255,NULL,NULL);
	ASSERT(strlen(tempFileName) < sizeof(m_arrfileTreeNode[index].strFileName));
	
    
	//重命名录波文件
	CString path, info;
	path.Format(_T("%s%s"),m_strWareFilePath,(CString)m_arrfileTreeNode[index].strFileName);
	FILE *dataFile;
	dataFile=_wfopen(path,_T("rb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	fseek(dataFile,0,SEEK_END);
	int filelength=ftell(dataFile);
	BYTE *fileBuf=new BYTE[filelength+10];
	memset(fileBuf,0,filelength+10);
	fseek(dataFile,0,SEEK_SET);
	int temp=fread(fileBuf,sizeof(BYTE),filelength+10,dataFile);
	fclose(dataFile);
	if(filelength==temp) 
	{
		delete [] fileBuf;
		return FALSE;
	}
	
	dataFile=NULL;
	path.Format(_T("%s%s"),m_strWareFilePath,(CString)tempFileName);
	CFileFind fileFind;
	if(fileFind.FindFile(path))
	{
		delete [] fileBuf;
		return FALSE;
	}
	//创建新的录波文件
	dataFile=_wfopen(path,_T("wb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		delete [] fileBuf;
		return FALSE;
	}
	if(filelength != fwrite(fileBuf,sizeof(BYTE),filelength,dataFile))
	{
		info.LoadString( IDS_WRITEFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();;
		delete [] fileBuf;
		fclose(dataFile);
		return FALSE;
	}
	fclose(dataFile);
	path.Format(_T("%s%s"),m_strWareFilePath,(CString)m_arrfileTreeNode[index].strFileName);
	if(!DeleteFile(path))
	{
		info.LoadString( IDS_DELFILE_ERROR2 );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		path.Format(_T("%s%s"),m_strWareFilePath,(CString)tempFileName);
		DeleteFile(path);
		delete [] fileBuf;
		return FALSE;
	}
	memcpy(m_arrfileTreeNode[index].strFileName,tempFileName,strlen(tempFileName)+1);
	delete [] fileBuf;
	return TRUE;
}

BOOL CProjectData::DeleteWareFile(int index)								
{
	ASSERT(index >=0 && index < m_nFileCount);
	CString path, info;
	path.Format(_T("%s%s"),m_strWareFilePath,(CString)m_arrfileTreeNode[index].strFileName);
	if(!DeleteFile(path))
	{
		info.LoadString( IDS_DELFILE_ERROR2 );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	if(!DeleteWareFileOutToProject(index))
	{
		return FALSE;
	}
	return TRUE;
}

void CProjectData::Inlitialize()
{
	memset(m_arrIPAddr,0,sizeof(m_arrIPAddr));
	m_nFileCount=0;
	memset(m_strDBFileName,0,sizeof(m_strDBFileName));
	memset(m_strDeviceType,0,sizeof(m_strDeviceType));
	memset(m_strProjectName,0,sizeof(m_strProjectName));
	m_strProjectFileTitle=_T("");
	m_strProjectPath=_T("");
	m_strWareFilePath=_T("");
}

CProjectData & CProjectData::operator =(const CProjectData &project)
{
	int i;
	m_nFileCount=project.m_nFileCount;
	for(i=0;i<m_nFileCount;i++)
	{
		m_arrfileTreeNode[i]=project.m_arrfileTreeNode[i];
	}
	memcpy(m_arrIPAddr,project.m_arrIPAddr,sizeof(m_arrIPAddr));
	strcpy(m_strDBFileName,project.m_strDBFileName);
	strcpy(m_strDeviceType,project.m_strDeviceType);
	m_strProjectFileTitle=project.m_strProjectFileTitle;
	strcpy(m_strProjectName,project.m_strProjectName);
	m_strProjectPath=project.m_strProjectPath;
	m_strWareFilePath=project.m_strWareFilePath;
	return * this;
}



BOOL CProjectData::DeleteDirectory(CString strDirName)
{
	CFileFind   tempFind;   
	CString   tempFileFind;   
	tempFileFind.Format(_T("%s\\*.*"),strDirName);   
	BOOL   IsFinded=(BOOL)tempFind.FindFile(tempFileFind);   
	while(IsFinded)   
	{   
		IsFinded=(BOOL)tempFind.FindNextFile();   
		if(!tempFind.IsDots())   
		{   
			CString foundFileName;   
			foundFileName=tempFind.GetFileName();   
			if(tempFind.IsDirectory())   
			{   
				CString tempDir;   
				tempDir.Format(_T("%s\\%s"),strDirName,foundFileName);   
				DeleteDirectory(tempDir);   
			}   
			else   
			{   
				CString tempFileName;   
				tempFileName.Format(_T("%s\\%s"),strDirName,foundFileName);   
				DeleteFile(tempFileName);   
			}   
		}   
	}   
	tempFind.Close();   
	if(!RemoveDirectory(strDirName))   
	{   
		CString str;
		str.LoadString( IDS_DELDIRECTORY_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, str, _T(""), MB_OK);
        box.MsgBoxEx();
		return   FALSE;   
	}   
	return   TRUE;  
}

BOOL CProjectData::OpenProject(CString filePath)
{
	m_strProjectPath=filePath.Left(filePath.ReverseFind(TEXT('\\'))+1);   //工程路径文件夹位置
	m_strWareFilePath=m_strProjectPath+_T("waveFile\\");                  //工程波形文件夹的位置 
	m_strProjectFileTitle=filePath.Right(filePath.GetLength()-filePath.ReverseFind('\\')-1);  //工程文件标题
	m_strProjectFileTitle=m_strProjectFileTitle.Left(m_strProjectFileTitle.GetLength()-4);
	if(!OpenProjectFile(m_strProjectFileTitle))
	{
		return FALSE;
	}
	CString   strcnn;          //数据连接驱动字符串
	strcnn.Format(_T("Provider=Microsoft.JET.OLEDB.4.0;Data source=%s%s"),m_strProjectPath,
		(CString)m_strDBFileName); 
	if(theApp.m_adoDatabase.IsOpen())  
	{
		theApp.m_adoDatabase.Close();
	}
	if(!theApp.m_adoDatabase.Open(strcnn))
	{
		return FALSE;	
	}
	return TRUE;
}

BOOL CProjectData::SaveProjectSetting()
{
	CString path, info;
	path.Format(_T("%s%s.prj"),m_strProjectPath,m_strProjectFileTitle);
	FILE *dataFile;
	dataFile=_wfopen(path,_T("rb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	fseek(dataFile,0,SEEK_END);
	int filelength=ftell(dataFile);
	BYTE *fileBuf=new BYTE[filelength+10];
	memset(fileBuf,0,filelength+10);
	fseek(dataFile,0,SEEK_SET);
	int temp=fread(fileBuf,sizeof(BYTE),filelength+10,dataFile);
	fclose(dataFile);
	if(filelength!=temp)
	{
		info.LoadString( IDS_PRJFILE_READ_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		delete [] fileBuf;
		return FALSE;
	}
	
	dataFile=_wfopen(path,_T("wb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		delete [] fileBuf;
		return FALSE;
	}
	int curPos=0;
	temp=sizeof(m_strProjectName);
	memcpy(fileBuf,m_strProjectName,temp);
	curPos+=temp;
	temp=sizeof(m_strDeviceType);
	memcpy(fileBuf+curPos,m_strDeviceType,temp);
	curPos+=temp;
	fileBuf[curPos] = m_LinkType;
	curPos++;
	temp=sizeof(m_arrIPAddr);
	memcpy(fileBuf+curPos,m_arrIPAddr,temp);
	curPos+=temp;
	
	temp=sizeof(m_strDBFileName);
	memcpy(fileBuf+curPos,m_strDBFileName,temp);
	curPos+=temp;
	
	temp = sizeof(m_dwAnalogDef);
	memcpy(fileBuf+curPos,&theApp.m_projectData.m_dwAnalogDef,temp);
	curPos+=temp;
	memcpy(fileBuf+curPos,&theApp.m_projectData.m_dwDigtalInDef,temp);
	curPos+=temp;
	memcpy(fileBuf+curPos,&theApp.m_projectData.m_dwDigtalOutDef,temp);
	curPos+=temp;
	memcpy(fileBuf+curPos,&theApp.m_projectData.m_dwFrequencyDef,temp);
	curPos+=temp;
	memcpy(fileBuf+curPos,&theApp.m_projectData.m_dwAngleDiffDef,temp);
	curPos+=temp;


	temp=sizeof(m_nFileCount);
	memcpy(fileBuf+curPos,&m_nFileCount,temp);
	curPos+=temp;
	if(m_nFileCount>=MAX_FILE_COUNT)
	{
		CString info, str;
		info.LoadString( IDS_PRJFILE_OVER_NUM );
        str.Format( info, MAX_FILE_COUNT );
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
		delete [] fileBuf;
		return FALSE;
	}
	//计算逻辑上主文件长度
	temp=187+m_nFileCount*105;
	ASSERT(filelength == temp);
	if(temp==fwrite(fileBuf,sizeof(BYTE),temp,dataFile))
	{
		fclose(dataFile);
		delete [] fileBuf;
		return TRUE;
	}
	fclose(dataFile);
	delete [] fileBuf;
	CString str;
	str.LoadString( IDS_PRJFILE_WRITE_FAILE );
	CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
    box.MsgBoxEx();
	return FALSE;
}
/********************************************************************************
函数名：ConvertPath
描述：转换文件路径
参数：path[in/out]：要转换的路径
返回：
修改记录：
*********************************************************************************/
void CProjectData::ConvertPath(CString & path)
{
	int nPos=0;
	nPos=path.Find(TEXT('\\'),nPos);
	do
	{
		path.Insert(nPos,TEXT('\\'));
		nPos+=2;
		nPos=path.Find(TEXT('\\'),nPos);
	}while(nPos!=-1);
}

BOOL CProjectData::DeleteWareFileOutToProject(int index)
{
	CString path, info;
	path.Format(_T("%s%s.prj"),m_strProjectPath,m_strProjectFileTitle);
	FILE *dataFile;
	dataFile=_wfopen(path,_T("rb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	fseek(dataFile,0,SEEK_END);
	int filelength=ftell(dataFile);
	BYTE *fileBuf=new BYTE[filelength+10];
	memset(fileBuf,0,filelength+10);
	fseek(dataFile,0,SEEK_SET);
	int temp=fread(fileBuf,sizeof(BYTE),filelength+10,dataFile);
	fclose(dataFile);
	if(filelength!=temp)  return FALSE;
	//计算逻辑上主文件长度
	temp=187+m_nFileCount*105;
	ASSERT(filelength == temp);
	int i,curPos;
	m_nFileCount--;
	memcpy(fileBuf+185,&m_nFileCount,sizeof(m_nFileCount));
	for(i=index;i<m_nFileCount;i++)
	{
		m_arrfileTreeNode[i]=m_arrfileTreeNode[i+1];
		curPos=187+i*105;
		memcpy(fileBuf+curPos,fileBuf+curPos+105,105);
	}
	curPos=187+m_nFileCount*105;
	dataFile=_wfopen(path,_T("wb"));
	if(dataFile==NULL)
	{
		info.LoadString( IDS_OPENFILE_ERROR );
		CdMessageBox box(theApp.m_pMainWnd, info);
        box.MsgBoxEx();
		return FALSE;
	}
	if(curPos == fwrite(fileBuf,sizeof(BYTE),curPos,dataFile))
	{
		fclose(dataFile);
		return TRUE;
	}
	fclose(dataFile);
	return FALSE;
}

BOOL CProjectData::AddWareFileToProject(CString strWareFileName)
{
	CString str, info;
	if(m_nFileCount >= MAX_FILE_COUNT)
	{
		info.LoadString( IDS_MAX_FILENUM );
		str.Format( info, MAX_FILE_COUNT, m_nFileCount);
		CdMessageBox box(theApp.m_pMainWnd, str, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	char tempFileName[255];
	WideCharToMultiByte(CP_ACP,0,strWareFileName,-1,tempFileName,255,NULL,NULL);
	if(strlen(tempFileName) >= MAX_FILE_NAME_LENGTH)
	{
		info.LoadString( IDS_FILENAME_LENG );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
	    int k=strlen(tempFileName);
		return FALSE;
	}
	CString path;
	path.Format(_T("%s%s"),m_strWareFilePath,strWareFileName);
	CFileFind fileFind;
	if(!fileFind.FindFile(path))
	{
		info.LoadString( IDS_WAREFILE_INFO );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		return FALSE;
	}
	FILE *wareFile;
	wareFile=_wfopen(path,_T("rb"));
	if(wareFile == NULL)
	{
		info.LoadString( IDS_STR_OPENE );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();;
		return FALSE;
	}
	BYTE fileBuf[100];
	if(sizeof(fileBuf) != fread(fileBuf,sizeof(BYTE),sizeof(fileBuf),wareFile))
	{
		info.LoadString( IDS_WAVEFILE_BROKEN );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		fclose(wareFile);
		return FALSE;
	}
	fclose(wareFile);
	if(!CFrame::GetDateTimeFromBuf(fileBuf+18,m_arrfileTreeNode[m_nFileCount].dtStartTime,
		m_arrfileTreeNode[m_nFileCount].nMillisecond))
	{
		info.LoadString( IDS_WAVEFILE_BROKEN );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();;
		return FALSE;
	}
	memcpy(m_arrfileTreeNode[m_nFileCount].strWareType,fileBuf+26,sizeof(m_arrfileTreeNode[m_nFileCount].strWareType));
	strcpy(m_arrfileTreeNode[m_nFileCount].strFileName,tempFileName);
	return SaveTreeNodeToProject(m_nFileCount);
}


BOOL CProjectData::TestConnection()
{
	CString strIP,strTemp;
	BOOL bIsConnection=FALSE;
	if (m_LinkType & CONNECT_BY_NET)
	{
		strIP.Format(_T("%d.%d.%d.%d"),m_arrIPAddr[0],m_arrIPAddr[1],
			m_arrIPAddr[2],m_arrIPAddr[3]);
	}
	else
	{
		strIP.Format(_T("USB"));
	}
	CString str=CFrame::GetDeviceType(strIP);
	CString connectErrorStr, info;
	connectErrorStr.LoadString( IDS_CONNECT_ERROR );
	if( str == connectErrorStr )
	{
		CdMessageBox box( theApp.m_pMainWnd, str);
	    box.MsgBoxEx();
	//	((CMainFrame*)GetParentFrame())->StopCommunication();
		bIsConnection = FALSE;
	}
	else if(str == _T(""))
	{
		info.LoadString( IDS_TYPEDEVICE_UNKNOW );
		CdMessageBox box(theApp.m_pMainWnd, info, _T(""), MB_OK);
        box.MsgBoxEx();
		bIsConnection = FALSE;
	}
// 	else if(str!=m_strDeviceType)
// 	{
// 		theApp.m_tempItem = theApp.m_mapDiviceType.find(str);
// 		if(theApp.m_tempItem == theApp.m_mapDiviceType.end())
// 		{
// 			info.LoadString( IDS_TYPEDEVICE_INFO2 );
// 			strTemp.Format(info, str);
// 
// 		}
// 		else
// 		{
// 			info.LoadString( IDS_TYPEDEVICE_ERROR );
// 			strTemp.Format(info, (CString)m_strDeviceType, str);
// 		}
// 		CdMessageBox box(theApp.m_pMainWnd, strTemp, _T(""), MB_OK);
//         box.MsgBoxEx();
// 		return FALSE;
// 	}
	else
	{
		WideCharToMultiByte(CP_ACP,0,str,-1,theApp.m_projectData.m_strDeviceType,sizeof(theApp.m_projectData.m_strDeviceType),0,0);
		bIsConnection = TRUE;
	}
	return bIsConnection;
}
