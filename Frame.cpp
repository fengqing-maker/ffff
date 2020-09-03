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

//nFlag ����QUERY_SUMARY��QUERY_WARE_DATA,size��ʾ֡���ȣ���FRAME_HEAD ��FRAME_TRAIL�����˵���ֽ�����,
//����ʱΪ���֡���ȣ�����ʱ��ʾʵ��֡����,���frame�����󣬽���������
//frame��ʾָ������֡��ָ��
//��nFlagΪQUERY_WARE_DATAʱ��happentime��type��pieceNum����������,����type��ΪС��32�ֽڣ����������ݵĲ���ȫ��0
//����ͷ���֡
unsigned char * CFrame::MadeSendFrame(CComService & comService,ServerFrame *pServerFrame)
{
	comService.m_frameCount++;
	memset(comService.m_sendFrame,0,sizeof(comService.m_sendFrame));
	BYTE * pSendFrame = comService.m_sendFrame; //������λ������
	int & nSendFrameSize = comService.m_nSendFrameSize;
		
	pSendFrame[0]=FRAME_HEAD;
	pSendFrame[1]=0;//��ʼ��LengthL��LengthHΪ0
	pSendFrame[2]=0;
	pSendFrame[3]=FRAME_HEAD;
	pSendFrame[4]=comService.m_frameCount;
	pSendFrame[5]=comService.m_frameCount>>8;
	pSendFrame[6]=pServerFrame->nFlag;  //nFlag��Ϣ��
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
			case QUERY_PARA_COUNT:     //��ѯ��ֵ��Ŀ
			case QUERY_PARA_AREA_NUM:  //��ѯ��ǰ��ֵ����
			case QUERY_SYS_PARA_COUNT: //��ѯϵͳ��ֵ����Ŀ
			case QUERY_SYS_PARA_DEVICE_RESET:   
			case QUERY_SYS_PARA_SAVE_PARA:  //�̻�ϵͳ��ֵ
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
	pSendFrame[nSendFrameSize-2]=(unsigned char)GetCks(pSendFrame,nSendFrameSize); //��������Ľ����ΪQUERY
	pSendFrame[nSendFrameSize-1]=FRAME_TRAIL; //֡β��
	return pSendFrame;
}

//�����¼������Ŀcount�����frame����QUERY_SUMMARY������-1
int CFrame::GetWareStartTypeCount()
{
	BYTE *pRecvFrame =m_WaveCom.m_recvFrame;
	if(pRecvFrame[6]!=QUERY_WARE_SUMMARY) return -1;
	return pRecvFrame[7];
}
//���ж��Ƿ���������������0��û�г�������֡����
unsigned char CFrame::GetFrameType(const CComService & comService,const unsigned char *frame, int size)
{
	unsigned short temp= GetCks(frame,size);
	//�������
	if(temp>255)
	{
		return 0;
	}
	if(frame[size-2]!=(unsigned char)temp) 
	{
		return 0;
	}
	//���ǵ�ǰ���ȴ�������֡
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
// 	year+=time[0];   //��ȡ���
// 	CTime tempTime(year,time[1],time[2],time[3],time[4],time[5]);
// 	event.milliSecond=time[7];
// 	event.milliSecond<<=8;
// 	event.milliSecond+=time[6];
// 	event.happenTime=tempTime;
// 	return event;
// }

//�ж��Ƿ�Ϊ��ǰ��Ҫ���յ�����֡���������һ���ڽ�������֡������ж�
//���õ�ǰ��������֡��������
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
		//���ݿ�Ų���
		return (0-ERROR_LAWPC_ERROR);
	}
	//curFilePos��Ҫд���ļ�λ��
	int curFilePos=BLOCK_SIZE*pServerFrame->queryFrame.wareData.nPieceNum;
	if(pServerFrame->queryFrame.wareData.nPieceNum==0)
	{
		fileLength=CFrame::GetFileLength();
	}
	else if(fileLength!=GetFileLength())
	{
		//�����¼�����ݣ�����λ�����Ѷ�ʧ
		return (0-ERROR_WARE_DATA_LOST);
	}
	if(fileLength<=curFilePos)
	{
		//��λ���ֿ����
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
��������IsValidFile
������У���ļ��Ƿ�Ϸ�
������file[in]:У����ļ����ݣ��ļ�����
����ֵ��True�Ϸ���False���Ϸ�
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
�������ƣ�OpenConnection
��Ҫ������ �����ӣ�ע�⣬�����ӳɹ����������ͨ�ųɹ�
������
comType    ��������
strIPAddr  ֻ�е���������Ϊ����UDPʱ����Ч����ʾIP��ַ

  ����ֵ��
  TRUE           ��ʾ�����ӳɹ�
  FALSE		   ��ʾ������ʧ��
  �޸���־����
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
�������ƣ�SetIPAddr
��Ҫ������ ����IP��ַ��ע�⣬�������ʹ��USB��������ͨ�Ž�����
false

  ������
  str       IP��ַ�ַ������õ��ʮ���Ʊ�ʾ����192.168.1.122
  ����ֵ��
  true      IP��ַ���óɹ�
		false     IP��ַ����ʧ��
		�޸���־����
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
		//���ͺͽ�������
		if( isReSend )
		{
			isSendOk = comService.SendData() ;
			isReSend = true; //�ط�
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
	} while (timeOutTimes < RETRY_TIMES); //ͨ��ʱ���ݵĽ��ճ�ʱ����������������5����
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
�������ƣ�SendAndRecvFrame
��Ҫ������ ʵ�����ݵĽ��պͷ��ͣ��ȷ�������֡���ٵȴ����ݽ���
�˺��������������̣߳�ֱ�����յ���ʱ���򳬹�RECV_TIMEOUT
msʱ��
������
ServerFrame     ������������֡�Ķ���ṹ��
����ֵ��
0         ����ͨ�ųɹ�
ERROR_SEND_FAILED          0X01      //��������֡ʧ��
ERROR_RECV_TIMEOUT         0X02      //��������֡��ʱ
ERROR_RECV_ERROR           0X03      //�������ݳ���
ERROR_NO_TYPE_SERVER       0X04      //װ�ò�֧�����ַ���
ERROR_NO_TYPE_FRAME        0X05      //װ�ò�֧����������֡ 
ERROR_DEVICE_BUSY          0X06      //װ��æ    
ERROR_RECV_FAILED          0X07      //�ӳ�����ʧ��     
ERROR_RECV_DATA_ERROR      0X08      //���յ������ݰ����� 
ERROR_BIND_SOCK_FAILED     0X09      //�󶨶˿�ʧ��
ERROR_WARE_DATA_LOST       0X0A      //������ȡ��¼���ļ���װ���϶�ʧ
ERROR_LAWPC_ERROR          0X0B      //��λ�����͵����ݳ���     
�޸���־����
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
	if(re=ValidateFrame(*pComService,pServerFrame->nFlag))   //��֤֡
	{
		return ERROR_VALIDATE;
	}
	return SUCCESS_COMMUNICATE;
}
/********************************************************************
��������RecvEvent
����������λ����ѯ�¼�����
������nEventTypeNum���¼�������ţ�nEventStartIndex�ӵڼ�����ʼȡ
���أ���֡���յ����¼��ĸ������������ʧ�ܣ�����0
�޸ļ�¼��
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
	//��ȡ�¼�����ʧ��
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
��������GetEventDataFromFrame
����������λ��������֡�д�nFramePosλ�ÿ�ʼ��ȡһ���¼�
������nFramePos:[in/out]��ʼȡ�¼���λ��,out��һ���¼���ʼλ�ã�
      pEventData:[out]ȡ�����¼�
���أ��Ƿ��ȡ�¼��ɹ�
�޸�ʱ�䣺
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
	//�¼�����
	BYTE len=pRecvFrame[curPos];
	curPos++;
	memset(buf,0,sizeof(buf));
	memcpy(buf,pRecvFrame+curPos,len);
	pEventData->strEvnetType=buf;
	//�¼�����
	curPos+=len;
	memset(buf,0,sizeof(buf));
	len=pRecvFrame[curPos];
	curPos++;
	memcpy(buf,pRecvFrame+curPos,len);
	curPos+=len;
	pEventData->strEventName=buf;
	//��չ����
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
�������ƣ�TransferTime
��Ҫ������ ʵ��ʱ�䣨COleDataTime�����ֽ�ʱ�䣨¼���ļ��ж���
��ʱ���ʽ��֮����໥ת��

  ������
  direction       ʱ��ת���ķ���
  TRUE  data -> happenTime��nMillisecond
  FALSE happenTime��nMillisecond -> data
  
	date            �ֽ�ʱ�䣬����Ϊ8���ֽ�
	happenTime      COleDataTimeʱ��
	nMillisecond    happenTime�ĺ��벿��
	����ֵ��
	TRUE      ת���ɹ�
	FALSE     ת��ʧ��
	�޸���־����
************************************************************/
BOOL CFrame::TransferTime(BOOL direction, BYTE *date, 
				COleDateTime &happenTime, unsigned short &nMillisecond)
{
	int dateTime[6];
	if(direction)
	{
		//��date-��happenTime��nMillisecond
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
��������GetRealData
����������ʵ������
������pRealData[in/out]:����λ����ȡ����ʵ�����ݱ�
���أ��ɹ���TRUE������FALSE
�޸ļ�¼��
****************************************************************************/
BOOL CFrame::GetRealData(MapRealData * pRealData)
{
	//nCurCount ��ʾ��ǰװ�õ�ʵ������
	//nCruNum ��ʾ��ǰ���µ���ʵ�����������
	
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
�������ƣ�SaveParaArea
��Ҫ������ �̻���ֵ

  ������
  nParaAreaNum    [in]Ҫ�̻��Ķ�ֵ����
  ��ֵ����Ϊ0xff��ʾ�̻�ϵͳ��ֵ
  ����ֵ��
  TRUE      ��ѯϵͳ��ֵ�ɹ�
		FALSE     ��ѯϵͳ��ֵʧ��
		�޸���־����
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
�������ƣ�DviceReset
��Ҫ������ �ָ�װ�ó�������

  ������
		��	
		����ֵ��
		TRUE      ���óɹ�
		FALSE     ����ʧ��
		�޸���־����
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
�������ƣ�GetSingleSysParaData
��Ҫ������ ��ѯָ����ֵ��ŵĶ�ֵ

  ������
  nParaAreaNum    [in]��ֵ���ţ�����ֵ����Ϊ0xffʱ��ʾ����ϵ
  ͳ��ֵ
  pParaData       [out]�ù��洢��ֵ��ָ��
		nCount          [in/out]����pParaData�ܹ��洢�Ķ�ֵ�ĸ���
		����ʵ��ͨ�������Ķ�ֵ����
		����ֵ��
		TRUE      ��ѯ��ֵ�ɹ�
		FALSE     ��ѯ��ֵʧ��
		�޸���־����
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
	//�ж��Ƿ��ѯ�ɹ�
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
	//��ʱ
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
�������ƣ�SetParaData
��Ҫ������ ���ö�ֵ

  ������
		nParaArea       [in]��ֵ���ţ�����ֵ����Ϊ0xffʱ��ʾ����ϵ
		ͳ��ֵ
		nCount          [in]Ҫ���õ�ϵͳ��ֵ�ĸ���
		pBuf            [in/out]Ҫ���ö�ֵ��ֵ��������
		����ֵ��
		TRUE      ����ϵͳ��ֵ�ɹ�
		FALSE     ����ϵͳ��ֵʧ��
		�޸���־����	
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
��ȡ�¼�����
pTypeNums        [out]�¼������������
pEventCounts     [out]�����¼����͵�����¼�����
nMaxOfTypes      [in/out] pTypeNums��pEventCounts�������ܴ洢��������ݸ���
*****************************************************************************************/
bool CFrame::GetEventSummary(BYTE *pTypeNums,unsigned short *pEventCounts,int & nMaxOfTypes)
{
	ServerFrame serverFrame;
	serverFrame.nFlag=QUERY_EVENT_SUMMARY; //��ѯ�¼�����
	serverFrame.nServerType=SERVER_EVENT;
	int i,re;
	re=CFrame::SendAndRecvFrame(&serverFrame);
	//��ȡ�¼�����ʧ��
	if(re)
	{
//		m_linkStatus=false; 
 		return FALSE;
	}
	BYTE *bufSummary;
	if(nMaxOfTypes < m_EventCom.m_recvFrame[7])   //m_recvFrame���͵ĵ�7λΪ�յ��¼��ĸ�������                           
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
�������ƣ�SetCurParaArea
��Ҫ������ ���õ�ǰ��ֵ��������

  ������
  nObjParaArea        Ŀ�궨ֵ����
  ����ֵ��
  TRUE      ���óɹ�
		FALSE     ����ʧ��
		�޸���־����
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
�������ƣ�TelControl
��Ҫ������ ң��װ��

  ������
  nConCode        ң�ؿ����룬����μ�ͨѶЭ��
  ����ֵ��
  0         ң�سɹ�
  ʧ��ԭ����	  ң��ʧ��ԭ����
  �޸���־����
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
�������ƣ�GetSyncRealData
��Ҫ������ ��ѯͬ��ר��ʵ�����ݣ���������������ȡ��������Ӧ��
����������

  ������

  pBuf           [out]���ڴ洢�ظ���֡�����дӡ�����ϵͳ��ѹ��
  ����״̬�ź���n/(����)8*8--n-1�ŵ�״ֵ̬��
  (�μ�ͨ��Э��)
		nBufSize       [in]pBufSize�Ĵ�С
		nCountOfState  [in]״̬��Ϣ���ĸ��� 
		����ֵ��
		TRUE      ��ѯ�ɹ�
		FALSE     ��ѯʧ��
		�޸���־����
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
�������ƣ�GetStateCount
��Ҫ������ ��ȡ״̬��Ϣ�ĸ�������ʾ��Ϣ���쳣��Ϣ������ģʽ
��״̬����

  ������
  nObjParaArea        Ŀ�궨ֵ����
  ����ֵ��
  TRUE      ���óɹ�
  FALSE     ����ʧ��
  �޸���־����
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


