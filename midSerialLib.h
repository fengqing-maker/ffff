//midSerialLib.h

//using to the applications as interface.

#ifndef USBMW_MID_SERIALLIB_INCLUDE
#define USBMW_MID_SERIALLIB_INCLUDE

#define TIME_SET 25                     //数据发送定时时间，25ms
#define SERVER_SET 20                    //每帧最多可发送服务个数为20个
#define FRAME_HEAD_LOW 0x55                   //帧头
#define FRAME_TAIL_LOW 0x16                   //帧尾
#define FRAME_SIZE 4096                   //帧容量
#define CONTROL_SIZE 40960               //缓冲区容量定值

struct deviceInfo{
	CString pid ;
	CString vid ;
};
const int WELLSIZE = 51200 ;					//输出井大小50k

/*************************************************************************************
函数名称：PFCALLBACK
简要描述：中间层回调函数
参数：
    serviceNum             服务类型号
	retData                接收到的服务数据缓冲区，不包含服务帧头
	frame_size             retData数据区长度
返回值：
	用户自己行定义
调用环境：中间层调用
修改日志：无
*************************************************************************************/
typedef int (WINAPI *PFCALLBACK)(int serviceNum,unsigned char* retData,int frame_size);




/*************************************************************************************
函数名称：Interface
简要描述：用于数据发送功能
参数：
serviceData            服务数据区，包含两个字节的长度，服务号，备用和服务的数据区
内容，请参数USB中间层通信协议
priority               数据的优先级
1：表示最低优先级（在451中暂用于实时数据查询），即在中间
层数据发送中，需要延时40MS后再发送服务数据
2：表示普通优先级（暂用于录波提取），即在中间层数据发送过
程中，需要延时30MS
3：表示最高优先级（暂用于事件、定值功能），即发送过程中不需
要经过延时，立即向装置发送数据。
返回值：
ERROR_COMMUNICATION    通信错误
ERROR_RESEND           发送错误
SUCCESS_COMMUNICATION  发送成功
调用环境：外部调用
修改日志：无
*************************************************************************************/
extern "C" int  _declspec (dllexport) Interface(unsigned char* serviceData, int priority);


/*************************************************************************************
函数名称：InitIntermediate
简要描述：中间层初始化函数，每次通讯过程中，若上位机软件发现通讯失败都需要重新初始化中
间层
参数：
CallBackFunction       中间层接收到数据时的回调函数，用户可以在此函数中实现数据接收
dInfo                  包含PID UID等USB设备信息
Buffer_size            中间层中发送缓冲区的大小，默认为50KB
返回值：
0                      初始化失败
非0                      初始化成功
调用环境：外部调用
修改日志：无
*************************************************************************************/
extern "C" BOOL  _declspec (dllexport) InitIntermediate(PFCALLBACK CallBackFunction,deviceInfo dInfo,int Buffer_size=WELLSIZE);	


/*************************************************************************************
函数名称：CloseMidSerial
简要描述：关闭中间层
参数：无
返回值：无
调用环境：外部调用
修改日志：无
*************************************************************************************/
extern "C" VOID  _declspec (dllexport) CloseMidSerial();



#endif