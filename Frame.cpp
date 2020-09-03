// Frame.cpp: implementation of the CFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SID_2FY.h"
#include "midSerialLib.h"
//#include "Frame.h"
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

//BOOL CFrame::m_linkStatus = FALSE;

HANDLE m_hCom=NULL;
CString strPort=_T("");

CComService CFrame::m_WaveCom;
CComService CFrame::m_ParaCom;
CComService CFrame::m_EventCom;
CComService CFrame::m_RealCom;
CComService CFrame::m_TelCom;
CComService CFrame::m_SyncCom;
CString	CFrame::m_strIPAdress;


unsigned short CFrame::GetCks(const unsigned char *frame, int size)
{
	unsigned short re=0;
	if(size<9)
	{
		return 257;
	}
	for(int i=4;i<size-2;i++)
	{
		re+=frame[i];
	}
	re=re<<8;
	re=re>>8;
	return re;
}

//nFlag 可以QUERY_SUMARY或QUERY_WARE_DATA,size表示帧长度（从FRAME_HEAD 到FRAME_TRAIL包含端点的字节数）,
//传入时为最大帧长度，传出时表示实际帧长度,如何frame不够大，将发生错误
//frame表示指向整个帧的指针
//当nFlag为QUERY_WARE_DATA时，happentime、type、pieceNum都必需设置,对于type必为小于32字节，并且无内容的部分全是0
//制造和发送帧
unsigned char * CFrame::MadeSendFrame(CComService & comService,ServerFrame *pServerFrame)
{
	comService.m_frameCount++;
	memset(comService.m_sendFrame,0,sizeof(comService.m_sendFrame));
	BYTE * pSendFrame = comService.m_sendFrame; //发送下位机内容
	int & nSendFrameSize = comService.m_nSendFrameSize;
		
	pSendFrame[0]=FRAME_HEAD;
	pSendFrame[1]=0;//初始化LengthL和LengthH为0
	pSendFrame[2]=0;
	pSendFrame[3]=FRAME_HEAD;
	pSendFrame[4]=comService.m_frameCount;
	pSendFrame[5]=comService.m_frameCount>>8;
	pSendFrame[6]=pServerFrame->nFlag;  //nFlag消息号
	CString info;
	info.LoadString( IDS_FRAME_INFO );
	switch(pServerFrame->nServerType)
	{
	case SERVER_EVENT:
		{
			switch(pServerFrame->nFlag)
			{
			case QUERY_DEVICE_TYPE:
			case QUERY_EVENT_SUMMARY:
				{
					nSendFrameSize=9;
					pSendFrame[1]=3;
					break;
				}
			case QUERY_EVENT_DATA:
				{
					nSendFrameSize=12;
					pSendFrame[1]=6;
					pSendFrame[7]=pServerFrame->queryFrame.eventData.eventType;
					pSendFrame[8]=(BYTE)pServerFrame->queryFrame.eventData.eventStartIndex;
					pSendFrame[9]=pServerFrame->queryFrame.eventData.eventStartIndex>>8;
					break;
				}
			default:
				{
					CdMessageBox box( theApp.m_pMainWnd, info );
					box.MsgBoxEx();
					return NULL;
				}
			}
			break;
		}
	case SERVER_PARA:
		{
			switch(pServerFrame->nFlag)
			{
			case QUERY_DEVICE_TYPE:
			case QUERY_PARA_COUNT:     //查询定值数目
			case QUERY_PARA_AREA_NUM:  //查询当前定值区号
			case QUERY_SYS_PARA_COUNT: //查询系统定值的数目
			case QUERY_SYS_PARA_DEVICE_RESET:   
			case QUERY_SYS_PARA_SAVE_PARA:  //固化系统定值
				{
					nSendFrameSize=9;
					pSendFrame[1]=3;
					break;
				}
			case QUERY_PARA_SAVE_AREA_PARA:
				{
					nSendFrameSize=10;
					pSendFrame[1]=4;
					pSendFrame[7]=pServerFrame->queryFrame.nQueryOrSaveParaArea;
					break;
				}
			case QUERY_PARA_AREA_TRANSFER:
				{
					nSendFrameSize=11;
					pSendFrame[1]=5;
					pSendFrame[7]=pServerFrame->queryFrame.nQueryOrSaveParaArea;
					pSendFrame[8]=~pSendFrame[7];
					break;
				}
			case QUERY_PARA_DATA:
				{
					nSendFrameSize=12;
					pSendFrame[1]=6;
					pSendFrame[7]=pServerFrame->queryFrame.queryPara.nAreaNum;
					pSendFrame[8]=(BYTE)pServerFrame->queryFrame.queryPara.nParaNum;
					pSendFrame[9]=(BYTE)(pServerFrame->queryFrame.queryPara.nParaNum>>8);
					break;
				}
			case QUERY_SYS_PARA_DATA:
				{
					nSendFrameSize=11;
					pSendFrame[1]=5;
					ASSERT(0xff == pServerFrame->queryFrame.queryPara.nAreaNum);
					pSendFrame[7]=(BYTE)pServerFrame->queryFrame.queryPara.nParaNum;
					pSendFrame[8]=(BYTE)(pServerFrame->queryFrame.queryPara.nParaNum>>8);
					break;
				}
			case QUERY_PARA_SET_AREA_PARA:
				{
					ASSERT(!(pServerFrame->queryFrame.setPara.nBufSize%2));
					nSendFrameSize=10+pServerFrame->queryFrame.setPara.nBufSize;
					ASSERT(nSendFrameSize<MAX_FRAME_SIZE);
					int length=4+pServerFrame->queryFrame.setPara.nBufSize;
					pSendFrame[1]=(BYTE)length;
					pSendFrame[2]=length>>8;
					pSendFrame[7]=pServerFrame->queryFrame.setPara.nAreaNum;
					memcpy(pSendFrame+8,pServerFrame->queryFrame.setPara.pParaBuf,pServerFrame->queryFrame.setPara.nBufSize);
					break;
				}
			case QUERY_SYS_PARA_SET_PARA:
				{
					ASSERT(!(pServerFrame->queryFrame.setPara.nBufSize%2));
					nSendFrameSize=9+pServerFrame->queryFrame.setPara.nBufSize;
					ASSERT(nSendFrameSize<MAX_FRAME_SIZE);
					int length=3+pServerFrame->queryFrame.setPara.nBufSize;
					pSendFrame[1]=(BYTE)length;
					pSendFrame[2]=length>>8;
					memcpy(pSendFrame+7,pServerFrame->queryFrame.setPara.pParaBuf,pServerFrame->queryFrame.setPara.nBufSize);
					break;
				}
			default:
				{
					CdMessageBox box( theApp.m_pMainWnd, info );
					box.MsgBoxEx();
					return NULL;
				}
			}
			break;
		}
	case SERVER_REAL:
		{
			switch(pServerFrame->nFlag)
			{
			case QUERY_DEVICE_TYPE:
			case QUERY_REAL_DATA:
				{
					nSendFrameSize=9;
					pSendFrame[1]=3;
					break;
				}
			default:
				{
					CdMessageBox box( theApp.m_pMainWnd, info );
					box.MsgBoxEx();
					return NULL;
				}
			}
			break;
		}
	case SERVER_WARE:
		if((pServerFrame->nFlag - QUERY_WARE_SUMMARY)%0x10 == 0)
		{
			nSendFrameSize=9;
			pSendFrame[1]=3;
			break;
		}
		switch(pServerFrame->nFlag)
		{
			case QUERY_DEVICE_TYPE:
				{
					nSendFrameSize=9;
					pSendFrame[1]=3;
					break;
				}
				
			case QUERY_WARE_DATA:
				{
					pSendFrame[1]=45;
					memcpy(pSendFrame+7,pServerFrame->queryFrame.wareData.happenTime,8);
					memcpy(pSendFrame+15,pServerFrame->queryFrame.wareData.strStartType,
						strlen(pServerFrame->queryFrame.wareData.strStartType)+1);
					pSendFrame[47]=(BYTE)pServerFrame->queryFrame.wareData.nPieceNum;
					pSendFrame[48]=(BYTE)(pServerFrame->queryFrame.wareData.nPieceNum>>8);
					nSendFrameSize=51;
					break;
				}
			default:
				{
					CdMessageBox box( theApp.m_pMainWnd, info );
					box.MsgBoxEx();
					return NULL;
				}
				break;
		}
	case SERVER_TEL:
		{
			switch(pServerFrame->nFlag)
			{
			case QUERY_TEL_CONTROL:
				pSendFrame[1]=4;
				nSendFrameSize = 10;
				pSendFrame[7] = pServerFrame->queryFrame.nTelConCode;
				break;
			}
			break;
		}
	case SERVER_SYNC:
		{
			switch(pServerFrame->nFlag)
			{
			case QUERY_SYNC_REAL_DATA:
				pSendFrame[1]=3;
				nSendFrameSize = 9;
				break;
			case QUERY_SYNC_STATE_HEAD:
				pSendFrame[1]=4;
				nSendFrameSize = 10;
				pSendFrame[7] = pServerFrame->queryFrame.nStartIndOfStateHead;
				break;
			case QUERY_SYNC_STATE_COUNT:
				pSendFrame[1]=3;
				nSendFrameSize = 9;
				break;
			}
			break;
		}
	}
	pSendFrame[nSendFrameSize-2]=(unsigned char)GetCks(pSendFrame,nSendFrameSize); //计算出来的较验和为QUERY
	pSendFrame[nSendFrameSize-1]=FRAME_TRAIL; //帧尾码
	return pSendFrame;
}

//求故障录波总数目count，如果frame不是QUERY_SUMMARY，返回-1
int CFrame::GetWareStartTypeCount()
{
	BYTE *pRecvFrame =m_WaveCom.m_recvFrame;
	if(pRecvFrame[6]!=QUERY_WARE_SUMMARY) return -1;
	return pRecvFrame[7];
}
//先判断是否出错，如果出错，返回0，没有出错，返回帧类型
unsigned char CFrame::GetFrameType(const CComService & comService,const unsigned char *frame, int size)
{
	unsigned short temp= GetCks(frame,size);
	//较验出错
	if(temp>255)
	{
		return 0;
	}
	if(frame[size-2]!=(unsigned char)temp) 
	{
		return 0;
	}
	//不是当前所等待的数据帧
	if(!IsCurrentFrame(comService,frame,size))
	{
		return 0;  
	}
	return frame[6];
}

unsigned short CFrame::GetCurBlockCount()
{
	unsigned short re; 
	BYTE *pRecvFrame = m_WaveCom.m_recvFrame;
	re=pRecvFrame[8];
	re=re<<8;
	re+=pRecvFrame[7];
	return re;
}


// Event & CFrame::GetEvent(Event & event,int index,unsigned char * frame,int size)
// {
// 	event.eventName=_T("");
// 	size=size-2;
// 	int curPos=8+index*40;
// 	if((curPos+40)>size)
// 	{
// 		return event;
// 	}
// 	char name[32];
// 	unsigned char time[8];
// 	memcpy(name,frame+curPos,sizeof(name));
// 	memcpy(time,frame+curPos+sizeof(name),sizeof(time));
// 	event.eventName=(CString)name;
// 	int year=((CTime)CTime::GetCurrentTime()).GetYear();
// 	year=year/100*100;
// 	year+=time[0];   //获取年份
// 	CTime tempTime(year,time[1],time[2],time[3],time[4],time[5]);
// 	event.milliSecond=time[7];
// 	event.milliSecond<<=8;
// 	event.milliSecond+=time[6];
// 	event.happenTime=tempTime;
// 	return event;
// }

//判断是否为当前的要接收的数据帧，这个函数一般在较验数据帧无误后判断
//调用的前提是数据帧较验无误
BOOL CFrame::IsCurrentFrame(const CComService & comService,const unsigned char *frame, int size)
{
	unsigned short frameCount=0;
	frameCount=frame[5];
	frameCount <<= 8;
	frameCount += frame[4];
	if(frameCount==comService.m_frameCount)
	{
		return TRUE;
	}
	return FALSE;
}

UINT CFrame::GetFileLength()
{
	UINT re=0;
	BYTE *pRecvFrame = m_WaveCom.m_recvFrame;
	ASSERT(pRecvFrame[6]==QUERY_WARE_DATA);
	re= *((UINT *)(pRecvFrame + 9));
	return re;
}

int CFrame::GetBlock(unsigned char * file,UINT & fileLength,ServerFrame *pServerFrame)
{
	ASSERT(pServerFrame->nServerType==SERVER_WARE);
	ASSERT(pServerFrame->nFlag==QUERY_WARE_DATA);
	int re;
	re=SendAndRecvFrame(pServerFrame);
	if(re)
	{
//		m_linkStatus=false;
		return (0-re);
	}
	if(pServerFrame->queryFrame.wareData.nPieceNum!=CFrame::GetCurBlockCount())
	{
		//数据块号不对
		return (0-ERROR_LAWPC_ERROR);
	}
	//curFilePos将要写的文件位置
	int curFilePos=BLOCK_SIZE*pServerFrame->queryFrame.wareData.nPieceNum;
	if(pServerFrame->queryFrame.wareData.nPieceNum==0)
	{
		fileLength=CFrame::GetFileLength();
	}
	else if(fileLength!=GetFileLength())
	{
		//请求的录波数据，在下位机的已丢失
		return (0-ERROR_WARE_DATA_LOST);
	}
	if(fileLength<=curFilePos)
	{
		//下位机分块出错
		return (0-ERROR_LAWPC_ERROR);
	}
	BYTE * pRecvFrame = m_WaveCom.m_recvFrame;
	short &nRecvFrameSize = m_WaveCom.m_nRecvFrameSize;
	int blockLength=nRecvFrameSize-15;
	memcpy(file+curFilePos,pRecvFrame+13,blockLength);
	curFilePos+=blockLength;
	pServerFrame->queryFrame.wareData.nPieceNum++;
	re = curFilePos*100/fileLength;
// 	if(DoubleCmp(d1-re-1) == 0)
// 	{
// 		re++;
// 	}
	if (re>100)
	{
		re = 100;
	}
	return re;
}
/*************************************************************
函数名：IsValidFile
描述：校验文件是否合法
参数：file[in]:校验的文件内容，文件长度
返回值：True合法，False不合法
*************************************************************/
BOOL CFrame::IsValidFile(unsigned char * file,UINT size)
{
	UINT cks1=0,cks2=0;
	cks1 = *((UINT *)(file+size-4));
	int i=0,top=size-4;
	for(i=0;i<top;i++)
	{
		cks2 += file[i];
	}
	if(cks1==cks2)
	{
		return TRUE;
	}
	return FALSE;
}





void CFrame::CloseConnection()
{
	CNetSocket::m_udpNetWork.CableConfig(false);
	CUSBSocket::m_usbMidWare->CableConfig(false);
	/*if (m_strIPAddr != _T("USB"))
	{
		closesocket(m_sockForLawPC);
		WSACloseEvent(m_wEvent);
		m_sockForLawPC=INVALID_SOCKET;
	}
	else if (m_strIPAddr = _T("USB"))
	{
		CloseMidSerial();
	}*/
}
/************************************************************
函数名称：OpenConnection
简要描述： 打开连接，注意，打开连接成功不代表可以通信成功
参数：
comType    连接类型
strIPAddr  只有当连接类型为网络UDP时才有效，表示IP地址

  返回值：
  TRUE           表示打开连接成功
  FALSE		   表示打开连接失败
  修改日志：无
************************************************************/
BOOL CFrame::OpenConnection(ComType comType,const CString & strIPAddr)
{
	CNetSocket::m_udpNetWork.CableConfig(true);
	CUSBSocket::m_usbMidWare->CableConfig(true);
	BOOL re = FALSE;
	switch(comType)
	{
	case EN_USBMW:
		if(m_EventCom.OpenService(comType,USBMW_EVENT_PORT) &&
			m_WaveCom.OpenService(comType,USBMW_WARE_PORT) &&
			m_ParaCom.OpenService(comType,USBMW_PARAM_PORT) &&
			m_RealCom.OpenService(comType,USBMW_REAL_PORT) &&
			m_TelCom.OpenService(comType,USBMW_TEL_PORT) &&
			m_SyncCom.OpenService(comType,USBMW_SYNC_PORT))
		{
			re = TRUE;
		}
		
		break;
	case EN_NETUDP:

		if(m_EventCom.OpenService(comType,NET_UDP_EVENT_PORT) &&
			m_WaveCom.OpenService(comType,NET_UDP_WARE_PORT) &&
			m_ParaCom.OpenService(comType,NET_UDP_PARAM_PORT) &&
			m_RealCom.OpenService(comType,NET_UDP_REAL_PORT) &&
			m_TelCom.OpenService(comType,NET_UDP_TEL_PORT) && 
			m_SyncCom.OpenService(comType,NET_UDP_SYNC_PORT))
		{
			re = SetIPAddr(strIPAddr);
		}
		break;
	}
	
	return re;
}

/************************************************************
函数名称：SetIPAddr
简要描述： 设置IP地址，注意，如果正在使用USB进行数据通信将返回
false

  参数：
  str       IP地址字符串，用点分十进制表示，如192.168.1.122
  返回值：
  true      IP地址设置成功
		false     IP地址设置失败
		修改日志：无
************************************************************/
bool CFrame::SetIPAddr(const CString & str)
{
	switch(m_WaveCom.GetComType())
	{
	case EN_NETUDP:
		return CNetSocket::m_udpNetWork.SetIPAddr(str);
	}
	return false;
}



int CFrame::PollingCommunicate(CComService & comService)
{
	int timeOutTimes;
// 	if ( theApp.m_bRestart )
// 	{
// 		timeOutTimes=0;
// 	}
// 	else
	{
		timeOutTimes = 2;
	}
	bool isReSend = true, isSendOk = false;
	do 
	{
		//发送和接收数据
		if( isReSend )
		{
			isSendOk = comService.SendData() ;
			isReSend = true; //重发
		}
		if( isSendOk && true == comService.RecvData(RECV_TIMEOUT) )
		{
			if ( TRUE == IsCurrentFrame(  comService, comService.m_recvFrame, comService.m_nRecvFrameSize ) )
			{
				break;
			}
		
			isReSend = false;
		}
	
		if (m_strIPAdress == _T("USB"))
		{
			if (!CUSBSocket::m_usbMidWare->ResetUSBMidWare())
			{
				timeOutTimes = RETRY_TIMES;
				break;
			}
		}
		timeOutTimes++;
	} while (timeOutTimes < RETRY_TIMES); //通信时数据的接收超时次数不允许超过几（5）次
	if(timeOutTimes>=RETRY_TIMES)
	{
		return ERROR_RECV_TIMEOUT;
	}
// 	if(!CFrame::IsCurrentFrame(comService,comService.m_recvFrame,comService.m_nRecvFrameSize))
// 	{
// 		timeOutTimes++;
// 		goto ReTray;
// 	}
	return 0;
}

int CFrame::ValidateFrame(const CComService &comservice,UINT nFlag)
{
	const BYTE *pRecvFrame = comservice.m_recvFrame;
	const short & nRecvFrameSize = comservice.m_nRecvFrameSize;
	UINT nTempFlag=(UINT)GetFrameType(comservice,pRecvFrame,nRecvFrameSize); 
	if(nFlag != nTempFlag)
	{
		switch(nTempFlag)
		{
		case QUERY_NO_TYPE_SERVER:
			return ERROR_NO_TYPE_SERVER;
		case QUERY_NO_TYPE_FRAME:
			return ERROR_NO_TYPE_FRAME;
		case QUERY_DEVICE_BUSY:
			return ERROR_DEVICE_BUSY;
		default:
			{
				return ERROR_RECV_DATA_ERROR;
			}
		}
	}
	if(pRecvFrame[nRecvFrameSize-2] != GetCks(pRecvFrame,nRecvFrameSize))
	{
		return ERROR_LAWPC_ERROR;
	}
	return 0;
}

/************************************************************
函数名称：SendAndRecvFrame
简要描述： 实现数据的接收和发送，先发送请求帧，再等待数据接收
此函数将阻塞调用线程，直到接收到数时，或超过RECV_TIMEOUT
ms时长
参数：
ServerFrame     请求服务的数据帧的定义结构体
返回值：
0         数据通信成功
ERROR_SEND_FAILED          0X01      //发送数据帧失败
ERROR_RECV_TIMEOUT         0X02      //接收数据帧超时
ERROR_RECV_ERROR           0X03      //接收数据出错
ERROR_NO_TYPE_SERVER       0X04      //装置不支持这种服务
ERROR_NO_TYPE_FRAME        0X05      //装置不支持这种命令帧 
ERROR_DEVICE_BUSY          0X06      //装置忙    
ERROR_RECV_FAILED          0X07      //接成数据失败     
ERROR_RECV_DATA_ERROR      0X08      //接收到的数据包出错 
ERROR_BIND_SOCK_FAILED     0X09      //绑定端口失败
ERROR_WARE_DATA_LOST       0X0A      //正在提取的录波文件在装置上丢失
ERROR_LAWPC_ERROR          0X0B      //下位机传送的数据出错     
修改日志：无
************************************************************/
int CFrame::SendAndRecvFrame(ServerFrame * pServerFrame)
{
	CComService *pComService;
	switch(pServerFrame->nServerType)
	{
	case SERVER_WARE:
		pComService = &m_WaveCom;
		break;
	case SERVER_EVENT:
		pComService = &m_EventCom;
		break;
	case SERVER_PARA:
		pComService = &m_ParaCom;
		break;
	case SERVER_REAL:
		pComService = &m_RealCom;
		break;
	case SERVER_TEL:
		pComService = &m_TelCom;
		break;
	case SERVER_SYNC:
		pComService = &m_SyncCom;
		break;
	}
	MadeSendFrame(*pComService,pServerFrame);
	int re;
	if(re=PollingCommunicate(*pComService))
	{
		CloseConnection();
		return ERROR_COMMUNICATE;
	}
	if(re=ValidateFrame(*pComService,pServerFrame->nFlag))   //验证帧
	{
		return ERROR_VALIDATE;
	}
	return SUCCESS_COMMUNICATE;
}
/********************************************************************
函数名：RecvEvent
描述：向下位机查询事件数据
参数：nEventTypeNum：事件类型序号，nEventStartIndex从第几个开始取
返回：本帧接收到了事件的个数，如果接收失败，返回0
修改记录：
********************************************************************/
int CFrame::RecvEvent(BYTE nEventTypeNum, unsigned short nEventStartIndex, unsigned short nMaxEventNum)
{
	ASSERT(nEventStartIndex<nMaxEventNum);
	ServerFrame serverFrame;
	memset(&serverFrame,0,sizeof(serverFrame));
	serverFrame.nFlag=QUERY_EVENT_DATA;
	serverFrame.nServerType=SERVER_EVENT;
	serverFrame.queryFrame.eventData.eventType=nEventTypeNum;
	serverFrame.queryFrame.eventData.eventStartIndex=nEventStartIndex;
	int re=SendAndRecvFrame(&serverFrame);
	if(re)
	{
//		m_linkStatus = FALSE;
		return 0;
	}
	return m_EventCom.m_recvFrame[8];
}



BOOL CFrame::SaveEventToAccess()
{
	CAdoRecordset record;
	ServerFrame serverFrame;
	serverFrame.nFlag=QUERY_EVENT_SUMMARY;
	serverFrame.nServerType=SERVER_EVENT;
	int i,re=SendAndRecvFrame(&serverFrame);
	//提取事件概览失败
	if(re)
	{
//		m_linkStatus=false;
		return FALSE;
	}
	BYTE bufSummary[1000];
	memset(bufSummary,0,sizeof(bufSummary));
	int nEventTypeCount;
	BYTE *pRecvFrame = m_EventCom.m_recvFrame;
	short &nRecvFrameSize = m_EventCom.m_nRecvFrameSize;

	nEventTypeCount=pRecvFrame[7];
	ASSERT(nRecvFrameSize==(nEventTypeCount*3+10));
	memcpy(bufSummary,pRecvFrame+8,3*nEventTypeCount);
	for(i=0;i<nEventTypeCount;i++)
	{
		int bufPos=i*3;
		BYTE nEventTypeNum=bufSummary[bufPos];
		unsigned short nEventStartIndex=0;
		unsigned short nEventMaxCount = bufSummary[bufPos+2];
		nEventMaxCount<<=8;
		nEventMaxCount+=bufSummary[bufPos+1];
		BYTE nEventCountOfFrame;
		do 
		{
			nEventCountOfFrame = RecvEvent(nEventTypeNum,nEventStartIndex,nEventMaxCount);
			if(0 == nEventCountOfFrame)
			{
				return FALSE;	
			}
			
			BYTE j;
			UINT nFramePos=9;
			EventData eventData;
			for(j=0;j<nEventCountOfFrame;j++)
			{
				if(!GetEventDataFromFrame(nFramePos,&eventData))
				{
					return FALSE;
				}
				CString sql;
				sql.Format(_T("select * from event where EventName='%s' and EventType='%s' and DATEDIFF('s','%s',HappenTime)=0 and Millisecond=%d"),
					eventData.strEventName,eventData.strEvnetType,eventData.happentTime.Format(_T("%Y-%m-%d %H:%M:%S")),
					eventData.nMilliSecond);
				if(record.IsOpen())
				{
					record.Close();
				}
				if(record.Open(sql,theApp.m_adoDatabase,adLockOptimistic))
				{
					if(record.IsEOF())
					{
						record.AddNew();
						record[_T("EventName")]=eventData.strEventName;
						record[_T("EventType")]=eventData.strEvnetType;
						record[_T("Param")]=eventData.strExtendContent;
						record[_T("HappenTime")]=eventData.happentTime;
						record[_T("Millisecond")]=eventData.nMilliSecond;
						try
						{
							record.Update();
						}
						catch (...)
						{
							CString info;
							info.LoadString( IDS_ADDEVENT_ERROR );
							CdMessageBox box( theApp.m_pMainWnd, info);
		                    box.MsgBoxEx();
						}
					}
					
				}
			}
			if(record.IsOpen())
			{
				record.Close();
			}
			ASSERT((nFramePos+2)==nRecvFrameSize);
			nEventStartIndex+=nEventCountOfFrame;
		} while ((nEventCountOfFrame!=0) && (nEventStartIndex<nEventMaxCount));
	}
	return TRUE;
}

/**************************************************************************
函数名：GetEventDataFromFrame
描述：从下位机发来的帧中从nFramePos位置开始，取一个事件
参数：nFramePos:[in/out]开始取事件的位置,out下一个事件开始位置；
      pEventData:[out]取出的事件
返回：是否获取事件成功
修改时间：
**************************************************************************/
BOOL CFrame::GetEventDataFromFrame(UINT & nFramePos,EventData *pEventData)
{
	BYTE lenOfEvent;
	char buf[255];
	UINT curPos=nFramePos;
	BYTE *pRecvFrame = m_EventCom.m_recvFrame;
	short &nRecvFrameSize = m_EventCom.m_nRecvFrameSize;
	lenOfEvent=pRecvFrame[curPos];  
	curPos++;
	ASSERT((nFramePos+lenOfEvent)<nRecvFrameSize);
	if(!TransferTime(TRUE,pRecvFrame+curPos,pEventData->happentTime,pEventData->nMilliSecond))
	{
		return FALSE;
	}
	curPos+=8;
	//事件类型
	BYTE len=pRecvFrame[curPos];
	curPos++;
	memset(buf,0,sizeof(buf));
	memcpy(buf,pRecvFrame+curPos,len);
	pEventData->strEvnetType=buf;
	//事件名称
	curPos+=len;
	memset(buf,0,sizeof(buf));
	len=pRecvFrame[curPos];
	curPos++;
	memcpy(buf,pRecvFrame+curPos,len);
	curPos+=len;
	pEventData->strEventName=buf;
	//扩展内容
	len=pRecvFrame[curPos];
	memset(buf,0,sizeof(buf));
	curPos++;
	memcpy(buf,pRecvFrame+curPos,len);
	pEventData->strExtendContent=buf;
	curPos+=len;
	ASSERT((curPos - nFramePos) == lenOfEvent);
	nFramePos=curPos;
	return TRUE;
	
}
/************************************************************
函数名称：TransferTime
简要描述： 实现时间（COleDataTime）与字节时间（录波文件中定义
的时间格式）之间的相互转化

  参数：
  direction       时间转化的方向：
  TRUE  data -> happenTime和nMillisecond
  FALSE happenTime和nMillisecond -> data
  
	date            字节时间，长度为8个字节
	happenTime      COleDataTime时间
	nMillisecond    happenTime的毫秒部分
	返回值：
	TRUE      转化成功
	FALSE     转化失败
	修改日志：无
************************************************************/
BOOL CFrame::TransferTime(BOOL direction, BYTE *date, 
				COleDateTime &happenTime, unsigned short &nMillisecond)
{
	int dateTime[6];
	if(direction)
	{
		//从date-》happenTime和nMillisecond
		dateTime[0]=(int)BASE_YEAR+date[0];
		dateTime[1]= date[1];
		dateTime[2]= date[2];
		dateTime[3]= date[3];
		dateTime[4]= date[4];
		dateTime[5]= date[5];
		if(happenTime.SetDateTime(dateTime[0],dateTime[1],dateTime[2],dateTime[3],dateTime[4],dateTime[5]))
		{
			return FALSE;
		}
		nMillisecond= *((unsigned short *)(date + 6));
	}
	else
	{
		date[0]=(BYTE)(happenTime.GetYear()-BASE_YEAR);
		date[1]=(BYTE)happenTime.GetMonth();
		date[2]=(BYTE)happenTime.GetDay();
		date[3]=(BYTE)happenTime.GetHour();
		date[4]=(BYTE)happenTime.GetMinute();
		date[5]=(BYTE)happenTime.GetSecond();
		*((unsigned short *)(date + 6))=nMillisecond;
	}
	return TRUE;
}

/***************************************************************************
函数名：GetRealData
描述：加载实测数据
参数：pRealData[in/out]:从下位机获取到的实测数据表
返回：成功：TRUE，否则FALSE
修改记录：
****************************************************************************/
BOOL CFrame::GetRealData(MapRealData * pRealData)
{
	//nCurCount 表示当前装置的实测数据
	//nCruNum 表示当前更新到的实测数据项序号
	
	unsigned short nCurCount=0,nCurNum=0;
	BYTE nCurFrameCount;
	ServerFrame serverFrame;
	serverFrame.nServerType=SERVER_REAL;
	serverFrame.nFlag=QUERY_REAL_DATA;
	BYTE *pRecvFrame = m_RealCom.m_recvFrame;
	int re=SendAndRecvFrame(&serverFrame);
	BYTE i;
	if(re)
	{
//		m_linkStatus=false;
		return FALSE;
	}
	
	nCurCount= *((unsigned short *)(pRecvFrame + 7));
	if(pRealData->size()<nCurCount)
	{
		pRealData->erase(pRealData->begin(),pRealData->end());
	}
	do
	{
		UINT nCurFramePos;
		nCurFrameCount=pRecvFrame[9];
		nCurFramePos=10;
		ASSERT((12+84*nCurFrameCount) == m_RealCom.m_nRecvFrameSize);
		for(i=0;i<nCurFrameCount;i++)
		{
			GetSingleRealData(nCurFramePos,pRealData);
		}
		nCurNum+=nCurFrameCount;
		if(nCurNum<nCurCount)
		{
			re=SendAndRecvFrame(&serverFrame);
			if(re)
			{
//				m_linkStatus=false;
				return FALSE;
			}
		}
		else break;
	}while(1);
	return TRUE;
}

void CFrame::GetSingleRealData(UINT & nFramePos,MapRealData *pRealData)
{
	UINT nCurPos=nFramePos;
	int key;
	MapRealData & re=*pRealData;
	ASSERT((nCurPos+84)<m_RealCom.m_nRecvFrameSize);
	BYTE *pRecvFrame = m_RealCom.m_recvFrame;
	key= (int)*((unsigned short *)(pRecvFrame + nCurPos));
	nCurPos+=2;
	if(pRealData->find(key)==pRealData->end())
	{
		RealData real;
		memset(&real,0,sizeof(real));
		pRealData->insert(MapRealData::value_type(key,real));
	}
	memcpy(re[key].strType,pRecvFrame+nCurPos,sizeof(re[key].strType)-1);
	nCurPos+=sizeof(re[key].strType)-1;
	memcpy(re[key].strName,pRecvFrame+nCurPos,sizeof(re[key].strName)-1);
	nCurPos+=sizeof(re[key].strName)-1;
	memcpy(re[key].strValue,pRecvFrame+nCurPos,sizeof(re[key].strValue)-1);
	nCurPos+=sizeof(re[key].strValue)-1;
	memcpy(re[key].strExtend,pRecvFrame+nCurPos,sizeof(re[key].strExtend)-1);
	nCurPos+=sizeof(re[key].strExtend)-1;
	nFramePos=nCurPos;
}

/************************************************************
函数名称：SaveParaArea
简要描述： 固化定值

  参数：
  nParaAreaNum    [in]要固化的定值区号
  定值区号为0xff表示固化系统定值
  返回值：
  TRUE      查询系统定值成功
		FALSE     查询系统定值失败
		修改日志：无
************************************************************/
BYTE CFrame::SaveParaArea(BYTE nParaAreaNum)
{
	ServerFrame serverFrame;
	serverFrame.nServerType=SERVER_PARA;

	serverFrame.nFlag=(nParaAreaNum == 0xff) ? QUERY_SYS_PARA_SAVE_PARA:QUERY_PARA_SAVE_AREA_PARA;
	serverFrame.queryFrame.nQueryOrSaveParaArea=nParaAreaNum;
	BYTE *pRecvFrame = m_ParaCom.m_recvFrame;

	int re=SendAndRecvFrame(&serverFrame);
	if(re)
	{
//		m_linkStatus=false;
		return FALSE;
	}
	return pRecvFrame[7];
}

/************************************************************
函数名称：DviceReset
简要描述： 恢复装置出厂设置

  参数：
		无	
		返回值：
		TRUE      设置成功
		FALSE     设置失败
		修改日志：无
************************************************************/
BYTE CFrame::DeviceReset()
{
	ServerFrame serverFrame;
	serverFrame.nServerType=SERVER_PARA;

	serverFrame.nFlag= QUERY_SYS_PARA_DEVICE_RESET;

	BYTE *pRecvFrame = m_ParaCom.m_recvFrame;

	int re=SendAndRecvFrame(&serverFrame);
	if(re)
	{
//		m_linkStatus=false;
		return FALSE;
	}
	return pRecvFrame[7];
}

BOOL CFrame::GetParaArea(BYTE &nCurParaArea, BYTE &nParaAreaCount)
{
	ServerFrame serverFrame;
	serverFrame.nServerType=SERVER_PARA;
	serverFrame.nFlag=QUERY_PARA_AREA_NUM;
	BYTE * pRecvFrame = m_ParaCom.m_recvFrame;
	if(SendAndRecvFrame(&serverFrame))
	{
//		m_linkStatus=false;
		return FALSE;
	}
	nCurParaArea=pRecvFrame[7];
	nParaAreaCount=pRecvFrame[8];
	return TRUE;
}

/************************************************************
函数名称：GetSingleSysParaData
简要描述： 查询指定定值编号的定值

  参数：
  nParaAreaNum    [in]定值区号，当定值区号为0xff时表示设置系
  统定值
  pParaData       [out]用过存储定值的指针
		nCount          [in/out]传入pParaData能够存储的定值的个数
		传出实际通信上来的定值个数
		返回值：
		TRUE      查询定值成功
		FALSE     查询定值失败
		修改日志：无
************************************************************/
BOOL CFrame::GetSingleParaData(BYTE nParaAreaNum, unsigned short nParaNum,ParaData *pParaData)
{
	ServerFrame serverFrame;
	serverFrame.nServerType=SERVER_PARA;
	serverFrame.nFlag=(nParaAreaNum == 0xff) ? QUERY_SYS_PARA_DATA : QUERY_PARA_DATA;
	serverFrame.queryFrame.queryPara.nAreaNum=nParaAreaNum;
	serverFrame.queryFrame.queryPara.nParaNum=nParaNum;
	BYTE *pRecvFrame = m_ParaCom.m_recvFrame;
	if(SendAndRecvFrame(&serverFrame))
	{
//		m_linkStatus=false;
		return FALSE;
	}
	//判断是否查询成功
	if(OPERATION_RESULT_SUCCESS!=pRecvFrame[7])
	{
		return FALSE;
	}
	if(GetParaFromFrame(pParaData))	return TRUE;
	return FALSE;
}

BOOL CFrame::GetParaFromFrame(ParaData *pParaData)
{
	UINT nCurPos=8;
	memset(pParaData,0,sizeof(*pParaData));
	BYTE *pRecvFrame = m_ParaCom.m_recvFrame;
	pParaData->nParaNum= *((unsigned short *)(pRecvFrame + nCurPos));
	nCurPos+=2;
	pParaData->nTranFormat=pRecvFrame[nCurPos];
	ASSERT(pParaData->nTranFormat == PARA_TYPE_DATA || pParaData->nTranFormat == PARA_TYPE_OPTION);
	nCurPos++;
	int tempLen=sizeof(pParaData->strTypeName)-1;
	memcpy(pParaData->strTypeName,pRecvFrame+nCurPos,tempLen);
	nCurPos+=tempLen;
	tempLen=sizeof(pParaData->strPareName)-1;
	memcpy(pParaData->strPareName,pRecvFrame+nCurPos,tempLen);
	nCurPos+=tempLen;
	pParaData->nParaValue= *((unsigned short *)(pRecvFrame + nCurPos));
	nCurPos += 2;
	pParaData->nParaMaxValue = *((unsigned short *)(pRecvFrame + nCurPos));
	nCurPos += 2;
	pParaData->nParaMinValue = *((unsigned short *)(pRecvFrame + nCurPos));
	nCurPos += 2;
	if(pParaData->nTranFormat == PARA_TYPE_DATA)
	{
		ASSERT(m_ParaCom.m_nRecvFrameSize==81);
		if (m_ParaCom.m_nRecvFrameSize!=81)
		{
			return FALSE;
		}
		pParaData->differData.dataValue.nIntBitNum=pRecvFrame[nCurPos];
		nCurPos++;
		pParaData->differData.dataValue.nDecBitNum=pRecvFrame[nCurPos];
		nCurPos++;
		tempLen=sizeof(pParaData->differData.dataValue.strUnit)-1;
		memcpy(pParaData->differData.dataValue.strUnit,pRecvFrame+nCurPos,tempLen);
		nCurPos+=tempLen;
	}
	else
	{
		ASSERT(m_ParaCom.m_nRecvFrameSize == 583);
		if (m_ParaCom.m_nRecvFrameSize != 583)
		{
			return FALSE;
		}
		int i;
		tempLen=sizeof(pParaData->differData.arrStrOption[0])-1;
		for(i=0;i<PARA_OPTION_COUNT;i++)
		{
			memcpy(pParaData->differData.arrStrOption[i],pRecvFrame+nCurPos,tempLen);
			nCurPos += tempLen;
		}
	}
	
	return TRUE;
}

CString CFrame::GetDeviceType(CString strIP)
{
	CString connectErrorStr;
	connectErrorStr.LoadString( IDS_CONNECT_ERROR );
	if(strIP == _T("USB"))
	{
		m_strIPAdress = strIP;
		if(!OpenConnection(EN_USBMW,_T("")))
		{
			return connectErrorStr;
		}
	}
	else
	{
		if(!OpenConnection(EN_NETUDP,strIP))
		{
			return connectErrorStr;
		}
	}
	ServerFrame serverFrame;
	memset(&serverFrame,0,sizeof(serverFrame));
	serverFrame.nServerType=SERVER_PARA;
	serverFrame.nFlag=QUERY_DEVICE_TYPE;
	CString re;
	
	if(SendAndRecvFrame(&serverFrame))
	{
//		m_linkStatus=false;
		return connectErrorStr;
	}
	//暂时
	re=(CString)(m_ParaCom.m_recvFrame+7);
	return re;
}

BOOL CFrame::GetAllPara(BYTE nParaArea, ParaData *pParaData, unsigned short &nCount)
{
	
	ServerFrame serverFrame;
	serverFrame.nServerType=SERVER_PARA;
	serverFrame.nFlag=(nParaArea == 0xff) ? QUERY_SYS_PARA_COUNT : QUERY_PARA_COUNT;
	if(SendAndRecvFrame(&serverFrame))
	{
//		m_linkStatus=false;
		return FALSE;
	}
	unsigned short nCurCount,i;
	ASSERT(m_ParaCom.m_nRecvFrameSize == 11);
	if (m_ParaCom.m_nRecvFrameSize != 11)
	{
		return FALSE;
	}
	BYTE *pRecvFrame = m_ParaCom.m_recvFrame;
	nCurCount = *((unsigned short *)(pRecvFrame + 7));
	ASSERT(nCount >= nCurCount);
	if (nCount < nCurCount)
	{
		return FALSE;
	}
	nCount=nCurCount;
	for(i=0;i<nCount;i++)
	{
		if(!GetSingleParaData(nParaArea,i,&pParaData[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

/************************************************************
函数名称：SetParaData
简要描述： 设置定值

  参数：
		nParaArea       [in]定值区号，当定值区号为0xff时表示设置系
		统定值
		nCount          [in]要设置的系统定值的个数
		pBuf            [in/out]要设置定值的值数据数组
		返回值：
		TRUE      设置系统定值成功
		FALSE     设置系统定值失败
		修改日志：无	
************************************************************/
BYTE CFrame::SetParaData(BYTE nParaArea,unsigned short nCount, unsigned short *pBuf)
{
	ServerFrame serverFrame;
	memset(&serverFrame,0,sizeof(serverFrame));
	serverFrame.nServerType=SERVER_PARA;
	if (nParaArea == 0xFF)
	{
		serverFrame.nFlag=QUERY_SYS_PARA_SET_PARA;
	}
	else
	{
		serverFrame.nFlag=QUERY_PARA_SET_AREA_PARA;
	}
	serverFrame.queryFrame.setPara.nAreaNum=nParaArea;
	serverFrame.queryFrame.setPara.nBufSize=nCount*2;
	serverFrame.queryFrame.setPara.pParaBuf=(void *)pBuf;
	if(SendAndRecvFrame(&serverFrame))
	{
//		m_linkStatus=false;
		return FALSE;
	}
	return m_ParaCom.m_recvFrame[7];
}

#define		WAVE_SUMMARY_MAX_COUNT 28  //2019-8-29 CZZ
BOOL CFrame::GetAllWareHead(WareHead *pWareHead, BYTE &nCount)
{
	BYTE nCurCount;
	int		nIndex = 0;
	ServerFrame serverFrame;
	memset(&serverFrame,0,sizeof(serverFrame));
	serverFrame.nServerType=SERVER_WARE;
	serverFrame.nFlag=QUERY_WARE_SUMMARY ;
	if(CFrame::SendAndRecvFrame(&serverFrame))
	{
		return FALSE;
	}
	nCurCount=GetWareStartTypeCount();
	nCount=nCurCount;
	int nReCnt = (nCount%WAVE_SUMMARY_MAX_COUNT == 0) ? nCount/WAVE_SUMMARY_MAX_COUNT:nCount/WAVE_SUMMARY_MAX_COUNT + 1;
	for (int j = 0 ;j < nReCnt ;j++)
	{
		if(CFrame::SendAndRecvFrame(&serverFrame))
		{
			return FALSE;
		}
		nCurCount = nCount - j*WAVE_SUMMARY_MAX_COUNT;
		if (nCurCount > WAVE_SUMMARY_MAX_COUNT)
		{
			nCurCount = WAVE_SUMMARY_MAX_COUNT;
		}
		for(int i=0;i<nCurCount;i++,nIndex++)
		{
			if(!GetSingleWareHead(pWareHead[nIndex],i))
			{
				return FALSE;
			}
		}
		serverFrame.nFlag += 0x10;
	}
	return TRUE;
}

BOOL CFrame::GetSingleWareHead(WareHead &wareHead,BYTE nNum)
{
	ASSERT((m_WaveCom.m_recvFrame[6] - QUERY_WARE_SUMMARY)%0x10 == 0);
	ASSERT(nNum < m_WaveCom.m_recvFrame[7]);
	UINT curPos,tempLen;
	curPos = 8 + nNum * 40;
	tempLen=sizeof(wareHead.strWareStartType)-1;
	BYTE * pRecvFrame = m_WaveCom.m_recvFrame;
	memcpy(wareHead.strWareStartType,pRecvFrame+curPos,tempLen);
	curPos += tempLen;
	if(!GetDateTimeFromBuf(pRecvFrame+curPos,wareHead.happenTime,wareHead.milliSecond))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CFrame::GetDateTimeFromBuf(BYTE *buf, COleDateTime &oleDt, unsigned short &nMillSecond)
{
	int year;
	year=(int)buf[0]+BASE_YEAR;
	if(oleDt.SetDateTime(year,buf[1],buf[2],buf[3],buf[4],buf[5]))
	{
		return FALSE;
	}
	nMillSecond= *((unsigned short *)(buf + 6));
	if(nMillSecond >= 1000) return FALSE;
	return TRUE;
}

void CFrame::GetDateToBuf(BYTE *buf, COleDateTime olDt, unsigned short nMillSecond)
{
	buf[0] = (BYTE)(olDt.GetYear()-BASE_YEAR);
	buf[1] = (BYTE)olDt.GetMonth();
	buf[2] = (BYTE)olDt.GetDay();
	buf[3] = (BYTE)olDt.GetHour();
	buf[4] = (BYTE)olDt.GetMinute();
	buf[5] = (BYTE)olDt.GetSecond();
	*((unsigned short *)(buf + 6)) = nMillSecond;
}



/****************************************************************************************
获取事件概览
pTypeNums        [out]事件类型序号数组
pEventCounts     [out]各个事件类型的最大事件个数
nMaxOfTypes      [in/out] pTypeNums和pEventCounts数组最能存储的最大数据个数
*****************************************************************************************/
bool CFrame::GetEventSummary(BYTE *pTypeNums,unsigned short *pEventCounts,int & nMaxOfTypes)
{
	ServerFrame serverFrame;
	serverFrame.nFlag=QUERY_EVENT_SUMMARY; //查询事件概览
	serverFrame.nServerType=SERVER_EVENT;
	int i,re;
	re=CFrame::SendAndRecvFrame(&serverFrame);
	//提取事件概览失败
	if(re)
	{
//		m_linkStatus=false; 
 		return FALSE;
	}
	BYTE *bufSummary;
	if(nMaxOfTypes < m_EventCom.m_recvFrame[7])   //m_recvFrame发送的第7位为收到事件的个数？？                           
	{
		return false;
	}
	nMaxOfTypes = m_EventCom.m_recvFrame[7];
	ASSERT(m_EventCom.m_nRecvFrameSize==(nMaxOfTypes*3+10));
	if (m_EventCom.m_nRecvFrameSize!=(nMaxOfTypes*3+10))
	{
		return FALSE;
	}
	bufSummary = &(m_EventCom.m_recvFrame[8]);
	for(i=0; i<nMaxOfTypes; i++)
	{
		pTypeNums[i] = *bufSummary;
		pEventCounts[i] = *((unsigned short *)(++bufSummary));
		bufSummary+=2;
	}
	return true;
}
	
/************************************************************
函数名称：SetCurParaArea
简要描述： 设置当前定值区的区号

  参数：
  nObjParaArea        目标定值区号
  返回值：
  TRUE      设置成功
		FALSE     设置失败
		修改日志：无
************************************************************/
BYTE CFrame::SetCurParaArea(BYTE nObjParaArea)
{
	ServerFrame serverFrame;
	serverFrame.nServerType = SERVER_PARA;
	serverFrame.nFlag = QUERY_PARA_AREA_TRANSFER;
	serverFrame.queryFrame.nQueryOrSaveParaArea = nObjParaArea;
	if((!CFrame::SendAndRecvFrame(&serverFrame)))
	{
		return m_ParaCom.m_recvFrame[7];
	}
	return FALSE;
}

/************************************************************
函数名称：TelControl
简要描述： 遥控装置

  参数：
  nConCode        遥控控制码，定义参见通讯协议
  返回值：
  0         遥控成功
  失败原因码	  遥控失败原因码
  修改日志：无
************************************************************/
BYTE CFrame::TelControl(BYTE nConCode)
{
	ServerFrame serverFrame;
	serverFrame.nServerType = SERVER_TEL;
	serverFrame.nFlag = QUERY_TEL_CONTROL;
	serverFrame.queryFrame.nTelConCode = nConCode;
	if((!CFrame::SendAndRecvFrame(&serverFrame)))
	{
		return m_TelCom.m_recvFrame[8];
	}
	return 0xff;
}

BOOL CFrame::GetAllStateHeadInfo(StateInfo * pState,const int nCount)
{
	ASSERT(nCount > 0 && pState);
	ServerFrame serverFrame;
	serverFrame.nServerType = SERVER_SYNC;
	serverFrame.nFlag = QUERY_SYNC_STATE_HEAD;
	serverFrame.queryFrame.nStartIndOfStateHead = 0;
	BYTE *pRecvFrame;
	int i,nRecvHeadIndex = 0,nCurFrmHeadCount;
	while(nCurFrmHeadCount = nCount - nRecvHeadIndex)
	{
		serverFrame.queryFrame.nStartIndOfStateHead = nRecvHeadIndex;
		if(CFrame::SendAndRecvFrame(&serverFrame))
		{
			return FALSE;
		}
		if(nCurFrmHeadCount > 15)
		{
			nCurFrmHeadCount = 15;
		}
		pRecvFrame = m_SyncCom.m_recvFrame;
		ASSERT(pRecvFrame[7] == nRecvHeadIndex);
		if (pRecvFrame[7] != nRecvHeadIndex)
		{
			return FALSE;
		}
		ASSERT(nCurFrmHeadCount*49+10 == m_SyncCom.m_nRecvFrameSize);
		if (nCurFrmHeadCount*49+10 != m_SyncCom.m_nRecvFrameSize)
		{
			return FALSE;
		}
		pRecvFrame += 8;
		for(i=0;i<nCurFrmHeadCount;i++)
		{
			pState[nRecvHeadIndex].nStateHeadIndex = *pRecvFrame++;
			strcpy(pState[nRecvHeadIndex].strStateType,(char *)pRecvFrame);
			pRecvFrame += 16;
			strcpy(pState[nRecvHeadIndex].strStateName,(char *)pRecvFrame);
			pRecvFrame += 32;
			nRecvHeadIndex++;
		}
		ASSERT(nRecvHeadIndex <= nCount);
		if (nRecvHeadIndex > nCount)
		{
			return FALSE;
		}
		if(nRecvHeadIndex == nCount)
		{
			break;
		}
	}
	return TRUE;
}

/************************************************************
函数名称：GetSyncRealData
简要描述： 查询同期专用实测数据，并将数据整块提取出来，由应用
层来做解析

  参数：

  pBuf           [out]用于存储回复的帧数据中从“主控系统电压”
  到“状态信号量n/(整除)8*8--n-1号的状态值”
  (参见通信协议)
		nBufSize       [in]pBufSize的大小
		nCountOfState  [in]状态信息量的个数 
		返回值：
		TRUE      查询成功
		FALSE     查询失败
		修改日志：无
************************************************************/
BOOL CFrame::GetSyncRealData(BYTE *pBuf,const size_t nBufSize,const int nCountOfState)
{
	ServerFrame serverFrame;
	serverFrame.nServerType = SERVER_SYNC;
	serverFrame.nFlag = QUERY_SYNC_REAL_DATA;
	if(CFrame::SendAndRecvFrame(&serverFrame))
	{
		return FALSE;
	}
	BYTE * pRecvFrame = m_SyncCom.m_recvFrame;
	if (nCountOfState != *((unsigned short*)(pRecvFrame+17)))
	{
		return FALSE;
	}
	int nByteCount = 18 + (nCountOfState%8 ? 1 : 0) + nCountOfState/8;
	if (nBufSize != nByteCount)
	{
		return FALSE;
	}
	if (m_SyncCom.m_nRecvFrameSize != nByteCount + 9)
	{
		return FALSE;
	}

	memcpy(pBuf,pRecvFrame+7,nByteCount);
	/*dSVol = *((unsigned short*)pRecvFrame) * 0.1;
	pRecvFrame += 2;
	dGVol = *((unsigned short*)pRecvFrame) * 0.1;
	pRecvFrame += 2;
	dSFreq = *((unsigned short*)pRecvFrame) * 0.001;
	pRecvFrame += 2;
	dGFreq = *((unsigned short*)pRecvFrame) * 0.001;
	pRecvFrame += 2;
	dAngle = *((unsigned short*)pRecvFrame) * 0.1;
	pRecvFrame += 4;

	int i,j,byteCount = nCount/8,indexOfState=0;
	BYTE mask;
	for(i=0;i<byteCount;i++)
	{
		mask = 0x01;
		j=8;
		while(j--)
		{
			pState[indexOfState++].bValue = (*pRecvFrame & mask) ? true : false;
			mask <<= 1;
		}
	}
	mask = 0x01;
	j = nCount%8;
	while(j--)
	{
		pState[indexOfState++].bValue = (*pRecvFrame & mask) ? true : false;
		mask <<= 1;
	}*/
	return TRUE;	
}

/************************************************************
函数名称：GetStateCount
简要描述： 获取状态信息的个数和提示信息、异常信息、工作模式
的状态区间

  参数：
  nObjParaArea        目标定值区号
  返回值：
  TRUE      设置成功
  FALSE     设置失败
  修改日志：无
************************************************************/
BOOL CFrame::GetStateCount(BYTE & nStateCount,BYTE & nStartIndOfPrompt,BYTE & nEndIndOfPrompt,
								  BYTE & nStartIndOfException,BYTE & nEndIndOfException,
							      BYTE & nStartIndOfWorkMode,BYTE & nEndIndOfWorkMode)
{
	ServerFrame serverFrame;
	serverFrame.nServerType = SERVER_SYNC;
	serverFrame.nFlag = QUERY_SYNC_STATE_COUNT;
	if(CFrame::SendAndRecvFrame(&serverFrame))
	{
		return FALSE;
	}
	ASSERT(m_SyncCom.m_nRecvFrameSize == 16);
	BYTE *pRecvFrame = m_SyncCom.m_recvFrame+7;
	nStateCount = *pRecvFrame++;
	nStartIndOfPrompt = *pRecvFrame++;
	nEndIndOfPrompt = *pRecvFrame++;
	nStartIndOfException = *pRecvFrame++;
	nEndIndOfException = *pRecvFrame++;
	nStartIndOfWorkMode = *pRecvFrame++;
	nEndIndOfWorkMode = *pRecvFrame++;
	return TRUE;
}


