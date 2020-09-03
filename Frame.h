// Frame.h: interface for the CFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAME_H__04824487_17CA_452C_B51D_64DB47331F63__INCLUDED_)
#define AFX_FRAME_H__04824487_17CA_452C_B51D_64DB47331F63__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//QUERY_SUMMARY��QUERY_DATA���ܸ�Ϊ0
#include <vector>
#include "ComService.h"
using namespace std;
#define BASE_YEAR                  2000      //��ʼ��ݣ�������λ����һ���ֽڱ������
                                             //���Ա���������֧��2255��
//����ͨѶ
#define ERROR_COMMUNICATE          0X01      //ͨѶ����
#define ERROR_VALIDATE		       0X02      //������֤����
#define SUCCESS_COMMUNICATE		   0x00		 //ͨѶ�ɹ�



//ServerFrame�ṹ��nServerType��ȡֵ
#define SERVER_WARE                0X01      //¼������
#define SERVER_EVENT               0X02      //�¼�����
#define SERVER_PARA                0X03      //��ֵ����
#define SERVER_REAL                0X04      //ʵ�����ݷ���
#define SERVER_DEBUG               0X05      //���Է���
#define SERVER_TEL                 0X06      //ң�ط���
#define SERVER_SYNC                0X07      //ͬ��ר��Э��


#define ERROR_SEND_FAILED          0X01      //��������֡ʧ��
#define ERROR_RECV_TIMEOUT         0X02      //��������֡��ʱ
#define ERROR_RECV_ERROR           0X03      //�������ݳ���
#define ERROR_NO_TYPE_SERVER       0X04      //װ�ò�֧�����ַ���
#define ERROR_NO_TYPE_FRAME        0X05      //װ�ò�֧����������֡ 
#define ERROR_DEVICE_BUSY          0X06      //װ��æ    
#define ERROR_RECV_FAILED          0X07      //�ӳ�����ʧ��     
#define ERROR_RECV_DATA_ERROR      0X08      //���յ������ݰ����� 
#define ERROR_BIND_SOCK_FAILED     0X09      //�󶨶˿�ʧ��
#define ERROR_WARE_DATA_LOST       0X0A      //������ȡ��¼���ļ���װ���϶�ʧ
#define ERROR_LAWPC_ERROR          0X0B      //��λ�����͵����ݳ���


#define MAX_DEVICE_SAVE_WARE_FILE_COUNT 64   //�豸�ܴ洢��¼���ļ���

#define QUERY_WARE_SUMMARY              0x71        //��ѯ�������ݸ�Ҫ
#define QUERY_WARE_DATA                 0x72        //��ѯ���������ļ�
#define QUERY_DEVICE_TYPE          0x00		   //��ѯװ�����ͺ�
#define QUERY_EVENT_SUMMARY        0X40        //��ѯ�¼�����
#define QUERY_EVENT_DATA           0X41        //��ѯ�¼���Ϣ
#define QUERY_REAL_DATA            0X50        //��ѯʵ������
#define QUERY_PARA_COUNT           0x10        //��ѯ��ֵ��Ŀ
#define QUERY_SYS_PARA_COUNT       0X11        //��ѯϵͳ��ֵ����Ŀ
#define QUERY_PARA_AREA_NUM        0X20        //��ѯ��ǰ��ֵ����
#define QUERY_PARA_AREA_TRANSFER   0x30        //�л���ֵ��
#define QUERY_PARA_DATA            0x40        //��ѯָ�����Ķ�ֵ
#define QUERY_SYS_PARA_DATA        0X48        //��ѯϵͳ��ֵ
#define QUERY_PARA_SET_AREA_PARA   0x50        //�趨ֵָ�������ж�ֵ
#define QUERY_SYS_PARA_SET_PARA    0X51        //�趨ϵͳ��ֵ
#define QUERY_PARA_SAVE_AREA_PARA  0X60        //�̻�ָ����ֵ��������ֵ
#define QUERY_SYS_PARA_SAVE_PARA   0X61        //�̻�ϵͳ��ֵ
#define QUERY_SYS_PARA_DEVICE_RESET   0X70     //�̻�ϵͳ��ֵ
#define QUERY_TEL_CONTROL          0X30        //ң��
#define QUERY_SYNC_STATE_HEAD      0X40        //ͬ��ר��Э���л�ȡ״̬��Ϣͷ
#define QUERY_SYNC_REAL_DATA       0X50        //��ѯͬ��ͬ��������
#define QUERY_SYNC_STATE_COUNT     0X30        //��ѯͬ��״̬��Ϣ

#define QUERY_NO_TYPE_SERVER       0X01        //װ�ò�֧�ִ������
#define QUERY_NO_TYPE_FRAME        0X02        //��֧�ִ˼�����֡
#define QUERY_DEVICE_BUSY          0X03        //װ��æ


#define OPERATION_RESULT_SUCCESS        0X16         //�����ɹ�
#define OPERATION_RESULT_OVERFLOW       0X26         //������Խ��
#define OPERATION_RESULT_TRANS_ERROR    0X36         //�������
#define OPERATION_RESULT_DEVICE_LOCK    0X46         //װ�ñ���
#define OPERATION_RESULT_FORMAT_ERROR   0X56         //��ʽ��ƥ��
#define OPERATION_RESULT_TIMEOUT        0X66         //��ʱ����
#define OPERATION_RESULT_MODULE_ERROR   0X76         //����ģʽ����

#define PARA_TYPE_DATA                  0X01         //�����Ͷ�ֵ
#define PARA_TYPE_OPTION                0X02         //ѡ���Ͷ�ֵ
#define PARA_OPTION_COUNT               0X10         //ѡ����Ŀ

#define NET_UDP_EVENT_PORT 9000              //NET�¼�����˿�
#define NET_UDP_WARE_PORT 9001               //NET¼���ļ���ȡ����˿�
#define NET_UDP_REAL_PORT 9002               //NETʵ�����ݷ���˿�
#define NET_UDP_PARAM_PORT 9003              //NET������ֵ���ݷ���˿�
#define NET_UDP_DEBUG_PORT 9004              //NETװ�õ��Է���˿ڣ�Ŀǰ�����δʹ��
#define NET_UDP_TEL_PORT   9005              //NETң�ؽӿ�
#define NET_UDP_SYNC_PORT  9006              //N3Tͬ��ר������ͨ�Žӿ�

#define USBMW_EVENT_PORT 20              //USB�¼�����˿�
#define USBMW_WARE_PORT 21               //USB¼���ļ���ȡ����˿�
#define USBMW_REAL_PORT 22               //USBʵ�����ݷ���˿�
#define USBMW_PARAM_PORT 23              //USB������ֵ���ݷ���˿�
#define USBMW_DEBUG_PORT 24              //USBװ�õ��Է���˿ڣ�Ŀǰ�����δʹ��
#define USBMW_TEL_PORT   25              //USBң�ؽӿ�
#define USBMW_SYNC_PORT  26              //USBͬ��ר������ͨѶ�ӿ�


#define FRAME_HEAD  0x68           //֡ͷ��
#define FRAME_TRAIL  0x16          //֡β��
//����λ������ķ���������֡��С����
#define BLOCK_SIZE 512  //���ݿ�Ĵ�С
#define RETRY_TIMES 5   //ͨ��ʱ���ݵĽ��ճ�ʱ����������������5����
#define RECV_TIMEOUT 2000   //�ӷ������ݵ��յ���λ���Ļظ�ʱ�ĳ�ʱʱ��
#define FILE_INTERVAL_TIME 2500  //¼���ļ�ÿ��2.5�������ȡһ��
#define FRAME_INTERVAL_TIME 10   //����֡���ʱ����

#define MAX_FRAME_SIZE    1200              //����֡�Ϊ��1200��
#define MAX_WARE_FILE_LENGTH   0x200000     //¼���ļ�����󳤶�Ϊ1.0M  2015-7-6 �޸ĳ� 2.0Mb

#define MAX_SYS_PARA_COUNT                100
#define MAX_NOR_PARA_COUNT                 300


#define BASE_YEAR 2000
// struct wareFileHead
// {
// 	unsigned char happenTime[8];
// 	unsigned char 
//};

struct StateInfo
{
	BYTE nStateHeadIndex;            //״̬���
	char strStateType[17];           //״̬����
	char strStateName[33];           //״̬����
	bool bValue;                     //״̬�ĵ�ǰֵ

};


//����¼���ļ���ͷ��Ϣ
struct WareHead
{
	char strWareStartType[33];
	COleDateTime happenTime;
	unsigned short milliSecond;
	WareHead()
	{
		::memset(strWareStartType,0,sizeof(strWareStartType));
	}
};

//�������λ����ȡ�����ı�����ֵ��Ϣ
struct ParaData
{
	unsigned short nParaNum;
	BYTE nTranFormat;
	char strTypeName[21];
	char strPareName[33];
	unsigned short nParaValue;
	unsigned short nParaMaxValue;
	unsigned short nParaMinValue;
	union 
	{
		struct 
		{
			BYTE nIntBitNum;
			BYTE nDecBitNum;
			char strUnit[9];
		} dataValue;
		char arrStrOption[PARA_OPTION_COUNT][33];
	} differData;
};
//�������λ����ȡ�������¼���Ϣ
struct EventData
{
	COleDateTime happentTime;        //����ʱ��
	unsigned short nMilliSecond;     //����
	CString strEvnetType;            //�¼�����
	CString strEventName;            //�¼�����
	CString strExtendContent;
	EventData()
	{
		strEventName=_T("");
		strEvnetType=_T("");
		strExtendContent=_T("");
		nMilliSecond=0;
	}
};

//�������λ����ȡ����ʵ��������Ϣ
struct RealData
{
	char strType[19];     //ʵ����������
	char strName[33];     //ʵ����������
	char strValue[17];    //����ֵ
	char strExtend[17];   //��չֵ
	RealData & operator =(const RealData & r)
	{
		::memcpy(strType,r.strType,sizeof(strType));
		::memcpy(strName,r.strName,sizeof(strName));
		::memcpy(strValue,r.strValue,sizeof(strValue));
		::memcpy(strExtend,r.strExtend,sizeof(strExtend));
		return *this;
	}
};
typedef map<int, RealData, less<int> > MapRealData;
	
//���ڱ�����λ������λ����ѯ����ȡ���ݵ���������֡�����ݽṹ
struct ServerFrame
{
	UINT nServerType;
	BYTE nFlag;
	union   
	{
		struct 
		{
			BYTE happenTime[8];
			unsigned short nPieceNum;
			char strStartType[33];
		}  wareData;
		struct  
		{
			BYTE eventType;
			unsigned short eventStartIndex; //�ӵڼ����¼���ʼ��ȡ
		} eventData;
		BYTE nQueryOrSaveParaArea;              //��ѯ��̻���ֵʱ�Ķ�ֵ����
		BYTE nTelConCode;                       //ң�ؿ�����
		BYTE nStartIndOfStateHead;              //��ѯͬ��״̬��Ϣͷʱ����ʼ���
		struct 
		{
			BYTE nAreaNum;               //��ֵ���ţ��ڲ�ѯϵͳ��ֵʱ��Ч
			unsigned short nParaNum;     //��ֵ���
		} queryPara;
		struct 
		{
			BYTE nAreaNum;               //��ֵ���ţ�������ϵͳ��ֵʱ��Ч
			void * pParaBuf;             //��ֵ��ֵ������
			UINT nBufSize;               //��ֵ��ֵ�������Ĵ�С
		} setPara;
	} queryFrame;
};

enum EnTelCon{EnumReset=0x01,EnumStartSync=0x02,EnumStopSync=0x03,EnumResumeDefaultValue=0x04};  //ң������

class CFrame  
{
public:
	/*******************************************************************************************
	��������:
		OpenConnection 
		CloseConnection
		SetIPAddr
		SendAndRecvFrame
		PollingCommunicate
		IsCurrentFrame	
		MadeSendFrame
		GetFrameType
    ********************************************************************************************/

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
	static BOOL OpenConnection(ComType comType,const CString & strIPAddr);

	/************************************************************
	�������ƣ�CloseConnection
	��Ҫ������ �ر�����
	��������
	����ֵ����
	�޸���־����
	************************************************************/
	static void CloseConnection();

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
	static bool SetIPAddr(const CString & str);


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
	static BOOL TransferTime(BOOL direction,BYTE * date,COleDateTime & happenTime,unsigned short & nMillisecond);

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
	static int SendAndRecvFrame(ServerFrame * pServerFrame);
	//�ɹ���0��ʧ�ܷ���ERROR_SEND_FAILED �� ERROR_RECV_TIMEOUT
	static int PollingCommunicate(CComService & comServer);
	static BOOL IsCurrentFrame(const CComService & comService, const unsigned char * frame,int size);
	static unsigned char *MadeSendFrame(CComService & comServer,ServerFrame *pQueryFrame);
	//�ж�����֡����֮ǰ���ڱ��������Ѿ��ж��Ƿ����ͨ�����Ƿ�Ϊ��ǰ���ȴ�������֡
	static unsigned char GetFrameType(const CComService & comService,const unsigned char *frame,int size);
	static unsigned short GetCks(const unsigned char * frame,int size);



	//��ֵ����
	static BOOL GetParaFromFrame(ParaData *pParaData);
	/************************************************************
	�������ƣ�SetParaData
	��Ҫ������ ���ö�ֵ
	           
	������
		nParaArea       [in]��ֵ���ţ�����ֵ����Ϊ0xffʱ��ʾ����ϵͳ��ֵ
	    nCount          [in]Ҫ���õ�ϵͳ��ֵ�ĸ���
		pBuf            [in/out]Ҫ���ö�ֵ��ֵ��������
	����ֵ��
	    TRUE      ����ϵͳ��ֵ�ɹ�
		FALSE     ����ϵͳ��ֵʧ��
	�޸���־����
	************************************************************/
	static BYTE SetParaData(BYTE nParaArea,unsigned short nCount,unsigned short * pBuf);

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
	static BOOL GetSingleParaData(BYTE nParaAreaNum,unsigned short nParaNum,ParaData *pParaData);

	//nCurParaArea��nParaAreaCount���Ǵ�������
	static BOOL GetParaArea(BYTE & nCurParaArea,BYTE & nParaAreaCount);

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
	static BYTE SaveParaArea(BYTE nParaAreaNum);


	/************************************************************
	�������ƣ�GetAllSysPara
	��Ҫ������ ��ѯ����ϵͳ��ֵ��ĳһ��ֵ���ڵ����ж�ֵ
	           
	������
		nParaArea       [in]��ʾ��ֵ���ţ����Ϊ0xff��ʾϵͳ��ֵ
	    pParaData       [out]�ù��洢ϵͳ��ֵ��ָ��
		nCount          [in/out]����pParaData�ܹ��洢�Ķ�ֵ�ĸ���
		                ����ʵ��ͨ��������ϵͳ��ֵ����
	����ֵ��
	    TRUE      ��ѯϵͳ��ֵ�ɹ�
		FALSE     ��ѯϵͳ��ֵʧ��
	�޸���־����
	************************************************************/
	static BOOL GetAllPara(BYTE nParaArea,ParaData * pParaData,unsigned short &nCount);

	
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
	static BYTE SetCurParaArea(BYTE nObjParaArea);

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
	static BYTE DeviceReset();

	//ʵʱ���ݲ���
	static void GetSingleRealData(UINT & nFramePos,MapRealData *pRealData);
	//nCount [in/out]����pRealData�����Ԫ�ظ��������ص�ǰװ�õ�ʵ����������
	static BOOL GetRealData(MapRealData * pRealDataMap);


	//¼������
	//���뱣֤frame�������Ǵ���λ����������¼��������Ϣ��������ǣ������������Ľ����Ч
	static UINT GetFileLength();
	//��ȡ��ǰ������
	static unsigned short GetCurBlockCount();
	static BOOL GetSingleWareHead(WareHead & wareHead,BYTE nNum);
	static BOOL GetAllWareHead(WareHead *pWareHead,BYTE & nCount);
	
public:
// 	static DWORD USBPoll(unsigned char* send_buf, short sendBytes,unsigned char* receive_buf, short* readBytes);
// 	static BOOL enumUsbSerialKey();
// 	static BOOL InitComm();
	static int ValidateFrame(const CComService &comservice,UINT nFlag);
	static BOOL GetDateTimeFromBuf(BYTE *buf,COleDateTime & oleDt,unsigned short & nMillSecond);
	static void GetDateToBuf(BYTE *buf,COleDateTime olDt,unsigned short  nMillSecond);
	static CString GetDeviceType(CString strIP);
	static BOOL IsValidFile(unsigned char * file,UINT size);
	//������������ȡpServerFrame����������Ϣ��������������ļ����͵İٷֱȣ����Ϊ100����ʾ�ļ��������
	//������͵��ļ�����λ���Ϻ�Ȼ��ʧ������ERROR_WARE_DATA_LOST�����򷵻صײ���õĴ�����Ϣ
	//fileLength��һ����������
	static int GetBlock(unsigned char * file,UINT & fileLength,ServerFrame *pServerFrame);
	static int GetWareStartTypeCount();


	
	//�¼�����
	static BOOL GetEventDataFromFrame(UINT & nFramePos,EventData *pEventData);
	static BOOL SaveEventToAccess();
	//���ر�֡���յ����¼��ĸ������������ʧ�ܣ�����0
	static int RecvEvent(BYTE nEventTypeNum, unsigned short nEventStartIndex, unsigned short nMaxEventNum);
	/****************************************************************************************
	��ȡ�¼�����
	pTypeNums        [out]�¼������������
	pEventCounts     [out]�����¼����͵�����¼�����
	nMaxOfTypes      [in/out] pTypeNums��pEventCounts�������ܴ洢��������ݸ���
	*****************************************************************************************/
	static bool GetEventSummary(BYTE *pTypeNums,unsigned short *pEventCounts,int & nMaxOfTypes);


	
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
	static BYTE TelControl(BYTE nConCode);
	

	
	/*******************************************************************************************
	ͬ��ר��ͨѶ��Լ��������:
		GetAllStateInfo 
		GetSyncData
    ********************************************************************************************/


	
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
	static BOOL GetStateCount(BYTE & nStateCount,BYTE & nStartIndOfPrompt,BYTE & nEndIndOfPrompt,
							  BYTE & nStartIndOfException,BYTE & nEndIndOfException,
							  BYTE & nStartIndOfWorkMode,BYTE & nEndIndOfWorkMode);
	
	/************************************************************
	�������ƣ�GetAllStateHeadInfo
	��Ҫ������ ���õ�ǰ��ֵ��������
	
	  ������
	  nObjParaArea        Ŀ�궨ֵ����
	  ����ֵ��
	  TRUE      ���óɹ�
	  FALSE     ����ʧ��
	  �޸���־����
	************************************************************/
	static BOOL GetAllStateHeadInfo(StateInfo * pState,const int nCount);
	
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
	static BOOL GetSyncRealData(BYTE *pBuf,const size_t nBufSize,const int nCountOfState);
	
	
	


	//��ȡ��index��¼���¼����¼����ݣ���ע�⣬�����index��¼���¼�������֡��û�У�
	//�������¼���Ϊ���ַ������¼�,index�ǻ���0��
	// 	static Event & GetEvent(Event & event,int index,unsigned char * frame,int size);
	 
	//static vector<ParaData *> m_sysPara;        //ϵͳ��ֵ
	//static vector<ParaData *> m_norPara;        //��ͨ��ֵ
	//static vector<RealData *> m_realData;
//	static BOOL m_linkStatus;
	static CComService  m_EventCom;
	static CComService  m_WaveCom;
	static CComService  m_ParaCom;
	static CComService  m_TelCom;  //ң�ط���
	static CComService  m_RealCom; 
	static CComService  m_SyncCom;    //ͬ��ר�÷���
	static CString      m_strIPAdress;
};


#endif // !defined(AFX_FRAME_H__04824487_17CA_452C_B51D_64DB47331F63__INCLUDED_)
