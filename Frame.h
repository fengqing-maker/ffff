// Frame.h: interface for the CFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAME_H__04824487_17CA_452C_B51D_64DB47331F63__INCLUDED_)
#define AFX_FRAME_H__04824487_17CA_452C_B51D_64DB47331F63__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//QUERY_SUMMARY和QUERY_DATA不能改为0
#include <vector>
#include "ComService.h"
using namespace std;
#define BASE_YEAR                  2000      //起始年份，由于下位机用一个字节保存年份
                                             //所以本软件最多能支持2255年
//数据通讯
#define ERROR_COMMUNICATE          0X01      //通讯错误
#define ERROR_VALIDATE		       0X02      //数据验证错误
#define SUCCESS_COMMUNICATE		   0x00		 //通讯成功



//ServerFrame结构中nServerType的取值
#define SERVER_WARE                0X01      //录波服务
#define SERVER_EVENT               0X02      //事件服务
#define SERVER_PARA                0X03      //定值服务
#define SERVER_REAL                0X04      //实测数据服务
#define SERVER_DEBUG               0X05      //调试服务
#define SERVER_TEL                 0X06      //遥控服务
#define SERVER_SYNC                0X07      //同期专用协议


#define ERROR_SEND_FAILED          0X01      //发送数据帧失败
#define ERROR_RECV_TIMEOUT         0X02      //接收数据帧超时
#define ERROR_RECV_ERROR           0X03      //接收数据出错
#define ERROR_NO_TYPE_SERVER       0X04      //装置不支持这种服务
#define ERROR_NO_TYPE_FRAME        0X05      //装置不支持这种命令帧 
#define ERROR_DEVICE_BUSY          0X06      //装置忙    
#define ERROR_RECV_FAILED          0X07      //接成数据失败     
#define ERROR_RECV_DATA_ERROR      0X08      //接收到的数据包出错 
#define ERROR_BIND_SOCK_FAILED     0X09      //绑定端口失败
#define ERROR_WARE_DATA_LOST       0X0A      //正在提取的录波文件在装置上丢失
#define ERROR_LAWPC_ERROR          0X0B      //下位机传送的数据出错


#define MAX_DEVICE_SAVE_WARE_FILE_COUNT 64   //设备能存储的录波文件数

#define QUERY_WARE_SUMMARY              0x71        //查询波形数据概要
#define QUERY_WARE_DATA                 0x72        //查询波形数据文件
#define QUERY_DEVICE_TYPE          0x00		   //查询装置类型号
#define QUERY_EVENT_SUMMARY        0X40        //查询事件概览
#define QUERY_EVENT_DATA           0X41        //查询事件信息
#define QUERY_REAL_DATA            0X50        //查询实测数据
#define QUERY_PARA_COUNT           0x10        //查询定值数目
#define QUERY_SYS_PARA_COUNT       0X11        //查询系统定值的数目
#define QUERY_PARA_AREA_NUM        0X20        //查询当前定值区号
#define QUERY_PARA_AREA_TRANSFER   0x30        //切换定值区
#define QUERY_PARA_DATA            0x40        //查询指定区的定值
#define QUERY_SYS_PARA_DATA        0X48        //查询系统定值
#define QUERY_PARA_SET_AREA_PARA   0x50        //设定指值区中所有定值
#define QUERY_SYS_PARA_SET_PARA    0X51        //设定系统定值
#define QUERY_PARA_SAVE_AREA_PARA  0X60        //固化指定定值区的所有值
#define QUERY_SYS_PARA_SAVE_PARA   0X61        //固化系统定值
#define QUERY_SYS_PARA_DEVICE_RESET   0X70     //固化系统定值
#define QUERY_TEL_CONTROL          0X30        //遥控
#define QUERY_SYNC_STATE_HEAD      0X40        //同期专用协议中获取状态信息头
#define QUERY_SYNC_REAL_DATA       0X50        //查询同期同步表数据
#define QUERY_SYNC_STATE_COUNT     0X30        //查询同期状态信息

#define QUERY_NO_TYPE_SERVER       0X01        //装置不支持此类服务
#define QUERY_NO_TYPE_FRAME        0X02        //不支持此家数据帧
#define QUERY_DEVICE_BUSY          0X03        //装置忙


#define OPERATION_RESULT_SUCCESS        0X16         //操作成功
#define OPERATION_RESULT_OVERFLOW       0X26         //操作数越限
#define OPERATION_RESULT_TRANS_ERROR    0X36         //传输出错
#define OPERATION_RESULT_DEVICE_LOCK    0X46         //装置闭锁
#define OPERATION_RESULT_FORMAT_ERROR   0X56         //格式不匹配
#define OPERATION_RESULT_TIMEOUT        0X66         //超时错误
#define OPERATION_RESULT_MODULE_ERROR   0X76         //工作模式错误

#define PARA_TYPE_DATA                  0X01         //数据型定值
#define PARA_TYPE_OPTION                0X02         //选项型定值
#define PARA_OPTION_COUNT               0X10         //选项数目

#define NET_UDP_EVENT_PORT 9000              //NET事件服务端口
#define NET_UDP_WARE_PORT 9001               //NET录波文件提取服务端口
#define NET_UDP_REAL_PORT 9002               //NET实测数据服务端口
#define NET_UDP_PARAM_PORT 9003              //NET保护定值数据服务端口
#define NET_UDP_DEBUG_PORT 9004              //NET装置调试服务端口，目前软件中未使用
#define NET_UDP_TEL_PORT   9005              //NET遥控接口
#define NET_UDP_SYNC_PORT  9006              //N3T同期专用数据通信接口

#define USBMW_EVENT_PORT 20              //USB事件服务端口
#define USBMW_WARE_PORT 21               //USB录波文件提取服务端口
#define USBMW_REAL_PORT 22               //USB实测数据服务端口
#define USBMW_PARAM_PORT 23              //USB保护定值数据服务端口
#define USBMW_DEBUG_PORT 24              //USB装置调试服务端口，目前软件中未使用
#define USBMW_TEL_PORT   25              //USB遥控接口
#define USBMW_SYNC_PORT  26              //USB同期专用数据通讯接口


#define FRAME_HEAD  0x68           //帧头码
#define FRAME_TRAIL  0x16          //帧尾码
//向下位机请求的服务中数据帧大小定义
#define BLOCK_SIZE 512  //数据块的大小
#define RETRY_TIMES 5   //通信时数据的接收超时次数不允许超过几（5）次
#define RECV_TIMEOUT 2000   //从发送数据到收到下位机的回复时的超时时间
#define FILE_INTERVAL_TIME 2500  //录波文件每隔2.5秒才能提取一次
#define FRAME_INTERVAL_TIME 10   //数据帧间的时间间隔

#define MAX_FRAME_SIZE    1200              //数据帧最长为（1200）
#define MAX_WARE_FILE_LENGTH   0x200000     //录波文件的最大长度为1.0M  2015-7-6 修改成 2.0Mb

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
	BYTE nStateHeadIndex;            //状态序号
	char strStateType[17];           //状态类型
	char strStateName[33];           //状态名称
	bool bValue;                     //状态的当前值

};


//保存录波文件的头信息
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

//保存从下位机提取上来的保护定值信息
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
//保存从下位机报取上来的事件信息
struct EventData
{
	COleDateTime happentTime;        //发生时间
	unsigned short nMilliSecond;     //毫秒
	CString strEvnetType;            //事件类型
	CString strEventName;            //事件名称
	CString strExtendContent;
	EventData()
	{
		strEventName=_T("");
		strEvnetType=_T("");
		strExtendContent=_T("");
		nMilliSecond=0;
	}
};

//保存从下位机提取上来实测数据信息
struct RealData
{
	char strType[19];     //实测数据名称
	char strName[33];     //实测数据类型
	char strValue[17];    //测量值
	char strExtend[17];   //扩展值
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
	
//用于保存上位机向下位机查询或提取数据的制作数据帧的数据结构
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
			unsigned short eventStartIndex; //从第几个事件开始提取
		} eventData;
		BYTE nQueryOrSaveParaArea;              //查询或固化定值时的定值区号
		BYTE nTelConCode;                       //遥控控制码
		BYTE nStartIndOfStateHead;              //查询同基状态信息头时的起始序号
		struct 
		{
			BYTE nAreaNum;               //定值区号，在查询系统定值时无效
			unsigned short nParaNum;     //定值编号
		} queryPara;
		struct 
		{
			BYTE nAreaNum;               //定值区号，在设置系统定值时无效
			void * pParaBuf;             //定值数值缓冲区
			UINT nBufSize;               //定值数值缓冲区的大小
		} setPara;
	} queryFrame;
};

enum EnTelCon{EnumReset=0x01,EnumStartSync=0x02,EnumStopSync=0x03,EnumResumeDefaultValue=0x04};  //遥控命令

class CFrame  
{
public:
	/*******************************************************************************************
	公共函数:
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
	static BOOL OpenConnection(ComType comType,const CString & strIPAddr);

	/************************************************************
	函数名称：CloseConnection
	简要描述： 关闭连接
	参数：无
	返回值：无
	修改日志：无
	************************************************************/
	static void CloseConnection();

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
	static bool SetIPAddr(const CString & str);


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
	static BOOL TransferTime(BOOL direction,BYTE * date,COleDateTime & happenTime,unsigned short & nMillisecond);

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
	static int SendAndRecvFrame(ServerFrame * pServerFrame);
	//成功回0，失败返回ERROR_SEND_FAILED 或 ERROR_RECV_TIMEOUT
	static int PollingCommunicate(CComService & comServer);
	static BOOL IsCurrentFrame(const CComService & comService, const unsigned char * frame,int size);
	static unsigned char *MadeSendFrame(CComService & comServer,ServerFrame *pQueryFrame);
	//判断数据帧类型之前，在本函数中已经判断是否较验通过，是否为当前正等待的数据帧
	static unsigned char GetFrameType(const CComService & comService,const unsigned char *frame,int size);
	static unsigned short GetCks(const unsigned char * frame,int size);



	//定值操作
	static BOOL GetParaFromFrame(ParaData *pParaData);
	/************************************************************
	函数名称：SetParaData
	简要描述： 设置定值
	           
	参数：
		nParaArea       [in]定值区号，当定值区号为0xff时表示设置系统定值
	    nCount          [in]要设置的系统定值的个数
		pBuf            [in/out]要设置定值的值数据数组
	返回值：
	    TRUE      设置系统定值成功
		FALSE     设置系统定值失败
	修改日志：无
	************************************************************/
	static BYTE SetParaData(BYTE nParaArea,unsigned short nCount,unsigned short * pBuf);

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
	static BOOL GetSingleParaData(BYTE nParaAreaNum,unsigned short nParaNum,ParaData *pParaData);

	//nCurParaArea和nParaAreaCount都是传出参数
	static BOOL GetParaArea(BYTE & nCurParaArea,BYTE & nParaAreaCount);

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
	static BYTE SaveParaArea(BYTE nParaAreaNum);


	/************************************************************
	函数名称：GetAllSysPara
	简要描述： 查询所有系统定值或某一定值区内的所有定值
	           
	参数：
		nParaArea       [in]表示定值区号，如果为0xff表示系统定值
	    pParaData       [out]用过存储系统定值的指针
		nCount          [in/out]传入pParaData能够存储的定值的个数
		                传出实际通信上来的系统定值个数
	返回值：
	    TRUE      查询系统定值成功
		FALSE     查询系统定值失败
	修改日志：无
	************************************************************/
	static BOOL GetAllPara(BYTE nParaArea,ParaData * pParaData,unsigned short &nCount);

	
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
	static BYTE SetCurParaArea(BYTE nObjParaArea);

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
	static BYTE DeviceReset();

	//实时数据操作
	static void GetSingleRealData(UINT & nFramePos,MapRealData *pRealData);
	//nCount [in/out]传入pRealData的最大元素个数，返回当前装置的实测数据项数
	static BOOL GetRealData(MapRealData * pRealDataMap);


	//录波操作
	//必须保证frame的内容是从下位机发过来的录波数据信息，如果不是，本函数所返的结果无效
	static UINT GetFileLength();
	//获取当前块的序号
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
	//本函数用来获取pServerFrame波形数据信息，并返回已完成文件传送的百分比，如果为100，表示文件传送完成
	//如果传送的文件在下位机上忽然丢失，返回ERROR_WARE_DATA_LOST，否则返回底层调用的错误信息
	//fileLength是一个传出参数
	static int GetBlock(unsigned char * file,UINT & fileLength,ServerFrame *pServerFrame);
	static int GetWareStartTypeCount();


	
	//事件操作
	static BOOL GetEventDataFromFrame(UINT & nFramePos,EventData *pEventData);
	static BOOL SaveEventToAccess();
	//返回本帧接收到了事件的个数，如果接收失败，返回0
	static int RecvEvent(BYTE nEventTypeNum, unsigned short nEventStartIndex, unsigned short nMaxEventNum);
	/****************************************************************************************
	获取事件概览
	pTypeNums        [out]事件类型序号数组
	pEventCounts     [out]各个事件类型的最大事件个数
	nMaxOfTypes      [in/out] pTypeNums和pEventCounts数组最能存储的最大数据个数
	*****************************************************************************************/
	static bool GetEventSummary(BYTE *pTypeNums,unsigned short *pEventCounts,int & nMaxOfTypes);


	
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
	static BYTE TelControl(BYTE nConCode);
	

	
	/*******************************************************************************************
	同期专用通讯规约操作函数:
		GetAllStateInfo 
		GetSyncData
    ********************************************************************************************/


	
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
	static BOOL GetStateCount(BYTE & nStateCount,BYTE & nStartIndOfPrompt,BYTE & nEndIndOfPrompt,
							  BYTE & nStartIndOfException,BYTE & nEndIndOfException,
							  BYTE & nStartIndOfWorkMode,BYTE & nEndIndOfWorkMode);
	
	/************************************************************
	函数名称：GetAllStateHeadInfo
	简要描述： 设置当前定值区的区号
	
	  参数：
	  nObjParaArea        目标定值区号
	  返回值：
	  TRUE      设置成功
	  FALSE     设置失败
	  修改日志：无
	************************************************************/
	static BOOL GetAllStateHeadInfo(StateInfo * pState,const int nCount);
	
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
	static BOOL GetSyncRealData(BYTE *pBuf,const size_t nBufSize,const int nCountOfState);
	
	
	


	//获取第index的录波事件的事件内容，请注意，如果第index个录波事件在数据帧中没有，
	//将返回事件名为空字符串的事件,index是基于0的
	// 	static Event & GetEvent(Event & event,int index,unsigned char * frame,int size);
	 
	//static vector<ParaData *> m_sysPara;        //系统定值
	//static vector<ParaData *> m_norPara;        //普通定值
	//static vector<RealData *> m_realData;
//	static BOOL m_linkStatus;
	static CComService  m_EventCom;
	static CComService  m_WaveCom;
	static CComService  m_ParaCom;
	static CComService  m_TelCom;  //遥控服务
	static CComService  m_RealCom; 
	static CComService  m_SyncCom;    //同期专用服务
	static CString      m_strIPAdress;
};


#endif // !defined(AFX_FRAME_H__04824487_17CA_452C_B51D_64DB47331F63__INCLUDED_)
