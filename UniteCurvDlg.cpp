// UniteCurvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SID_2FY.h"
#include "UniteCurvDlg.h"
#include "MainFrm.h"
#include "dMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUniteCurvDlg dialog


extern CMySID_2FYApp theApp;
CUniteCurvDlg::CUniteCurvDlg(CString fileName,CWnd* pParent /*=NULL*/)
	: CDialog(CUniteCurvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUniteCurvDlg)
	m_showFile = _T("");
	//}}AFX_DATA_INIT
	m_filePath=fileName;
	m_nFileCount=0;
	isUnited=FALSE;
	m_defalutCheck=-1;
}


void CUniteCurvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUniteCurvDlg)
	DDX_Control(pDX, IDC_CURVE_LIST, m_uniteCurveList);
	DDX_Text(pDX, IDC_FILE_UNITED, m_showFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUniteCurvDlg, CDialog)
	//{{AFX_MSG_MAP(CUniteCurvDlg)
	ON_NOTIFY(NM_CLICK, IDC_CURVE_LIST, OnClickCurveList)
	ON_BN_CLICKED(IDC_AUTO_SELECT, OnAutoSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUniteCurvDlg message handlers

BOOL CUniteCurvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	theApp.m_pLangManger->SetDialogStrings(this, IDD);
	COleDateTime curveStartTime,span;
	CString curFileName,strFilePath,str, info;
	short curveStartMilli;
	int minDiff=300000;
	info.LoadString( IDS_WAVE_FILE );
	str.Format(_T("%s:"), info);
	m_uniteCurveList.SetExtendedStyle(LVS_EX_FULLROWSELECT   |   LVIS_FOCUSED   );   /////LVS_SHOWSELALWAYS     
	m_uniteCurveList.InsertColumn(1,str,LVCFMT_LEFT,350);
	strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,m_filePath);
	m_wareFileData.Release();
	m_wareFileData.OpenFile(strFilePath);
	if(m_wareFileData.m_isUnitedFile)
	{
	    info.LoadString( IDS_UNITEWAVE_INFO );
		CdMessageBox box(this, info);
		box.MsgBoxEx();
		OnCancel();
		return FALSE;
	}
	m_appointCurveStartTime=m_wareFileData.m_StartSampleTime;
	m_appointCurveMilliSecond=m_wareFileData.m_milliSecondOfStart;
	int diffMinute,i;
	// TODO: Add extra initialization here
	for(i=0;i<theApp.m_projectData.m_nFileCount;i++)
	{
		curFileName=(CString)theApp.m_projectData.m_arrfileTreeNode[i].strFileName;
		if(curFileName==m_filePath)
			continue;
		m_wareFileData.Release();
		strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,
			(CString)theApp.m_projectData.m_arrfileTreeNode[i].strFileName);
		m_wareFileData.OpenFile(strFilePath);
		if(m_wareFileData.m_isUnitedFile)
		{
			continue;
		}
		curveStartTime=m_wareFileData.m_StartSampleTime;
		curveStartMilli=m_wareFileData.m_milliSecondOfStart;
		int temp=m_appointCurveStartTime.GetYear()-curveStartTime.GetYear();
		if(temp)
			continue;
		temp=m_appointCurveStartTime.GetMonth()-curveStartTime.GetMonth();
		if(temp)
			continue;
		temp=m_appointCurveStartTime.GetDay()-curveStartTime.GetDay();
		if(temp)
			continue;
		temp=m_appointCurveStartTime.GetHour()-curveStartTime.GetHour();
		if(temp&&(temp!=1||temp!=-1))
			continue;
		diffMinute=m_appointCurveStartTime.GetMinute()-curveStartTime.GetMinute();
		if((diffMinute<=4&&diffMinute>=-4)||(diffMinute>55||diffMinute<-55))
		{
			temp=m_appointCurveStartTime.GetMinute()*60*1000+m_appointCurveStartTime.GetSecond()*1000+m_appointCurveMilliSecond;
			temp-=(curveStartTime.GetMinute()*60*1000+curveStartTime.GetSecond()*1000+curveStartMilli);
			if(temp<0)
				temp=0-temp;
			if(temp==0)
				continue;
			if(temp<minDiff)
			{
				minDiff=temp;
				m_defalutCheck=m_nFileCount;
				m_defalutFile=curFileName;
			}
			m_uniteCurveList.InsertItem(i,(CString)theApp.m_projectData.m_arrfileTreeNode[i].strFileName);
			m_nFileCount++;
		}
	}
	if(!m_nFileCount)
	{
		info.LoadString( IDS_UNITEWAVE_INFO2 );
		CdMessageBox box(this, info);
	 	box.MsgBoxEx();
		OnCancel();
		return FALSE;
	}
	//m_uniteCurveList.SetHotItem(m_defalutCheck);
	m_uniteCurveList.SetItemState(m_defalutCheck,   LVIS_SELECTED,   LVIS_SELECTED);   
    //UpdateData(false);   
	m_uniteCurveList.SetFocus();   
	m_showFile=m_defalutFile;
	UpdateData(FALSE);
	return false;// return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUniteCurvDlg::OnClickCurveList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	POSITION pos=m_uniteCurveList.GetFirstSelectedItemPosition();
	int nItem = m_uniteCurveList.GetNextSelectedItem(pos);
	m_curSel =m_uniteCurveList.GetItemText(nItem,0);
	if(m_curSel!=_T(""))
	{
		m_showFile=m_curSel;
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CUniteCurvDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString str, info;
	str=m_showFile;
	if(str==_T(""))
	{
		info.LoadString( IDS_SEL_UNITFILE );
		CdMessageBox box(this, info);
	 	    box.MsgBoxEx();
		return;
	}
	if(!DoWaveFileUnite(m_filePath,str))
	{
		info.LoadString( IDS_STR_UNITEERROR );
		CdMessageBox box(this, info);
	 	box.MsgBoxEx();
		return;
	}
	info.LoadString( IDS_STR_UNITS );
	CdMessageBox box(this, info);
	box.MsgBoxEx();
	for(int i = 0 ;i < theApp.m_projectData.m_nFileCount ; i++)
	{
		if(theApp.m_projectData.m_arrfileTreeNode[i].strFileName==m_newFileName)
			break;
	}
	if(i==theApp.m_projectData.m_nFileCount&&!theApp.m_projectData.AddWareFileToProject(m_newFileName))
	{
		info.LoadString( IDS_FILE_BROKEN );
		CdMessageBox box(this, info);
	 	box.MsgBoxEx();
		return;
	}
	isUnited=TRUE;
	CDialog::OnOK();
}

void CUniteCurvDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

bool CUniteCurvDlg::DoWaveFileUnite(CString waveFile1, CString waveFile2)
{
	unsigned char* newFileData=(unsigned char*)malloc(MAX_WARE_FILE_LENGTH*sizeof(unsigned char*));
	memset(newFileData,0,MAX_WARE_FILE_LENGTH);
	unsigned char* waveFileData1=(unsigned char*)malloc(MAX_WARE_FILE_LENGTH*sizeof(unsigned char*));
	memset(waveFileData1,0,MAX_WARE_FILE_LENGTH);
	unsigned char* waveFileData2=(unsigned char*)malloc(MAX_WARE_FILE_LENGTH*sizeof(unsigned char*));
	memset(waveFileData2,0,MAX_WARE_FILE_LENGTH);
	int pos=0,fileLength;
	COleDateTime file2StartTime,newFileTime;
	CString newFileName;
	unsigned short file2MilliSecond;
	unsigned int milliSecond1,milliSecond2;
	CString strFilePath;
	strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,waveFile1);
	FILE *File1 = _wfopen(strFilePath,_T("rb"));
	if(File1 == NULL)
	{
		free(waveFileData1);
		free(newFileData);
		free(waveFileData2);		
		return FALSE;
	}
	fileLength=fread(waveFileData1,sizeof(BYTE),MAX_WARE_FILE_LENGTH,File1);
	fclose(File1);

	strFilePath.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,waveFile2);
	FILE *File2 = _wfopen(strFilePath,_T("rb"));
	if(File2 == NULL)
	{
		free(waveFileData1);
		free(newFileData);
		free(waveFileData2);
		return FALSE;
	}
	fileLength=fread(waveFileData2,sizeof(BYTE),MAX_WARE_FILE_LENGTH,File2);
	fclose(File2);
		CFrame::GetDateTimeFromBuf(waveFileData2+18,file2StartTime,file2MilliSecond);

	milliSecond1=m_appointCurveStartTime.GetMinute()*60*1000+m_appointCurveStartTime.GetSecond()*1000+m_appointCurveMilliSecond;

	milliSecond2=file2StartTime.GetMinute()*60*1000+file2StartTime.GetSecond()*1000+file2MilliSecond;
	unsigned char* temp;
	m_diffMilliSecond=milliSecond1-milliSecond2;
	if(m_diffMilliSecond>0)
	{
		temp=waveFileData1;
		waveFileData1=waveFileData2;
		waveFileData2=temp;
	}
	else
	{
		m_diffMilliSecond=0-m_diffMilliSecond;
	}
	char waveType[33];
	memset(waveType,0,sizeof(waveType));
	memcpy(waveType,waveFileData1+26,32);
	CFrame::GetDateTimeFromBuf(waveFileData1+18,newFileTime,file2MilliSecond);
	CString fileName, info;
	info.LoadString( IDS_MERAGE_FILE );
	fileName.Format(_T("%s%s(%04d%02d%02d-%s-%03d).wave"),(CString)waveType, info, newFileTime.GetYear(),newFileTime.GetMonth(),newFileTime.GetDay(),
		newFileTime.Format(_T("%H%M%S")),file2MilliSecond);
	newFileName.Format(_T("%s%s"),theApp.m_projectData.m_strWareFilePath,fileName);
	memcpy(newFileData,waveFileData1,62);
	pos+=62;	
	//功能扩展预留，用一个字节标记为已合并过的录波文件
	//用两个字节记录两个录波文件之间的开始时间差(ms)
	newFileData[pos]=TRUE;

	newFileData[pos+1]=m_diffMilliSecond;
	newFileData[pos+2]=m_diffMilliSecond>>8;
	newFileData[pos+3]=m_diffMilliSecond>>16;
	newFileData[pos+4]=m_diffMilliSecond>>24;
	pos+=18;
	newFileData[pos]=waveFileData1[pos];
	int j,j2,k;
	unsigned int appoint1=0,appoint2=0;
	int* newItem=(int*)malloc(waveFileData1[pos]);
	int appointChannel[32],cur=0;
	int* subLength=(int*)malloc(waveFileData1[pos]*(sizeof(int*)));
	memset(subLength,0,waveFileData1[pos]*sizeof(int*));
	int totalCircle=waveFileData1[58]+(waveFileData1[59]<<8);
	int nItem=waveFileData1[pos],newLength=0,samplePoints;
	for(int i=0;i<nItem;i++)
	{
		subLength[i]+=waveFileData1[pos+i*4+1];
		subLength[i]+=(waveFileData1[pos+i*4+2]<<8);
		subLength[i]+=(waveFileData1[pos+i*4+3]<<16);
		subLength[i]+=(waveFileData1[pos+i*4+4]<<24);
	}
	pos+=1;
	unsigned char dataFlag=0;
	int curPos=81+4*nItem;
	pos+=4*nItem;
	for(i=0;i<nItem;i++)
	{
		memset(appointChannel,0,sizeof(appointChannel));
		newLength=0;
		appoint1=0,appoint2=0;
		dataFlag=waveFileData1[curPos];
		if(dataFlag==0x01)//为保护定值时，数据不变
		{
			newFileData[pos+i*4]=subLength[i];
			newFileData[pos+i*4+1]=subLength[i]>>8;
			newFileData[pos+i*4+2]=subLength[i]>>16;
			newFileData[pos+i*4+3]=subLength[i]>>24;
			memcpy(newFileData+pos,waveFileData1+curPos,subLength[i]);
			curPos+=subLength[i];
			pos+=subLength[i];
			continue;
		}
		samplePoints=waveFileData1[curPos+2];
		j=waveFileData1[curPos+3];
		memcpy(newFileData+pos,waveFileData1+curPos,3);
		newLength+=3;
		for(k=1;k<=j;k++)
		{
			appoint1|=(0x00000001<<waveFileData1[curPos+3+k]);
		}
		j2=waveFileData2[curPos+3];
		for(k=1;k<=j2;k++)
		{
			appoint2|=(0x00000001<<waveFileData2[curPos+3+k]);
		}
		if(appoint1!=appoint2)
		{
			appoint1|=appoint2;
			for(k=31;k>=0;k--)
			{
				unsigned int temp3=(0x00000001<<k);
				if(appoint1>=temp3)
				{
					appoint1-=(0x00000001<<k);
					appointChannel[cur++]=k;
				}
			}
			newFileData[pos+newLength]=cur;
			newLength++;
			for(k=0;k<cur;k++)
			{
				newFileData[pos+newLength]=appointChannel[k];
				newLength++;
			}
		}
		else
		{
			memcpy(newFileData+pos+newLength,waveFileData1+curPos+3,j+1);
			newLength+=(j+1);
		}
		newLength+=32;
		if(dataFlag==0xAA)
		{
			for(k=0;k<waveFileData1[curPos+1];k++)
			{
				memcpy(newFileData+pos+newLength,waveFileData1+curPos+4+j+32+
					(102+totalCircle*samplePoints*2)*k,totalCircle*samplePoints*2+102);
				newLength+=(totalCircle*samplePoints*2+102);
				memcpy(newFileData+pos+newLength,waveFileData2+curPos+4+j2+32+102+
					(102+totalCircle*samplePoints*2)*k,totalCircle*samplePoints*2);
				newLength+=(totalCircle*samplePoints*2);
			}
		}
		else if(dataFlag==0xDD||dataFlag==0xD0)
		{
			int nBytes=(waveFileData1[curPos+1]%8)?(waveFileData1[curPos+1]/8+1):(waveFileData1[curPos+1]/8);
			memcpy(newFileData+pos+newLength,waveFileData1+curPos+4+j+32,waveFileData1[curPos+1]*32);
			newLength+=waveFileData1[curPos+1]*32;
		
			memcpy(newFileData+pos+newLength,waveFileData1+curPos+4+j+32+waveFileData1[curPos+1]*32,
							totalCircle*samplePoints*nBytes);
			newLength+=totalCircle*samplePoints*nBytes;
		
			memcpy(newFileData+pos+newLength,waveFileData2+curPos+4+j2+32+waveFileData1[curPos+1]*32,
							totalCircle*samplePoints*nBytes);
			newLength+=totalCircle*samplePoints*nBytes;
			
		}
		curPos+=subLength[i];
		subLength[i]=newLength;
		pos+=newLength;
	}
	for(i=0;i<nItem;i++)
	{
		newFileData[81+i*4]=subLength[i];
		newFileData[81+i*4+1]=subLength[i]>>8;
		newFileData[81+i*4+2]=subLength[i]>>16;
		newFileData[81+i*4+3]=subLength[i]>>24;
	}
	int crc_sum=0;
	for(i=0;i<pos;i++)
	{
		crc_sum+=newFileData[i];
	}
	newFileData[pos++]=crc_sum;
	newFileData[pos++]=crc_sum>>8;
	newFileData[pos++]=crc_sum>>16;
	newFileData[pos++]=crc_sum>>24;
	CFileFind fileFinder;
	CString str, title;
	if(fileFinder.FindFile(newFileName))
	{
		title.LoadString( IDS_DLG_DEFTITLE );
        info.LoadString( IDS_RECOVE_FILEINFO );
		str.Format(info, newFileName);
		CdMessageBox box(this, str, title, MB_YESNO);
		if( IDNO == box.MsgBoxEx() )
		{
			free(waveFileData1);
			free(newFileData);
		    free(waveFileData2);
			return FALSE;
		}
	}
	FILE * newWaveFile=_wfopen(newFileName,_T("wb"));
	if(newWaveFile==NULL)
	{
		free(waveFileData1);
		free(newFileData);
		free(waveFileData2);
        str.LoadString( IDS_CREATEFILE_ERROR );
		CdMessageBox box(this, str);
	 	box.MsgBoxEx();
		return FALSE;
	}
	if(pos != fwrite(newFileData,sizeof(BYTE),pos,newWaveFile))
	{
		free(waveFileData1);
		free(newFileData);
		free(waveFileData2);
		fclose(newWaveFile);
		str.LoadString( IDS_WRITEEFILE_ERROR );
		CdMessageBox box(this, str);
	 	box.MsgBoxEx();
		return FALSE;
	}
	fclose(newWaveFile);
	m_newFileName=fileName;
	free(newFileData);
	free(waveFileData1);
	free(waveFileData2);

	return TRUE;
}

void CUniteCurvDlg::OnAutoSelect() 
{
	// TODO: Add your control notification handler code here
//		m_uniteCurveList.SetCheck(m_defalutCheck,true);
	m_uniteCurveList.SetItemState(m_defalutCheck,   LVIS_SELECTED,   LVIS_SELECTED);   
    //UpdateData(false);   
	m_uniteCurveList.SetFocus();   
	m_showFile=m_defalutFile;
	UpdateData(FALSE);
}
