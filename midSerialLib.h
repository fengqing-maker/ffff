//midSerialLib.h

//using to the applications as interface.

#ifndef USBMW_MID_SERIALLIB_INCLUDE
#define USBMW_MID_SERIALLIB_INCLUDE

#define TIME_SET 25                     //���ݷ��Ͷ�ʱʱ�䣬25ms
#define SERVER_SET 20                    //ÿ֡���ɷ��ͷ������Ϊ20��
#define FRAME_HEAD_LOW 0x55                   //֡ͷ
#define FRAME_TAIL_LOW 0x16                   //֡β
#define FRAME_SIZE 4096                   //֡����
#define CONTROL_SIZE 40960               //������������ֵ

struct deviceInfo{
	CString pid ;
	CString vid ;
};
const int WELLSIZE = 51200 ;					//�������С50k

/*************************************************************************************
�������ƣ�PFCALLBACK
��Ҫ�������м��ص�����
������
    serviceNum             �������ͺ�
	retData                ���յ��ķ������ݻ�����������������֡ͷ
	frame_size             retData����������
����ֵ��
	�û��Լ��ж���
���û������м�����
�޸���־����
*************************************************************************************/
typedef int (WINAPI *PFCALLBACK)(int serviceNum,unsigned char* retData,int frame_size);




/*************************************************************************************
�������ƣ�Interface
��Ҫ�������������ݷ��͹���
������
serviceData            ���������������������ֽڵĳ��ȣ�����ţ����úͷ����������
���ݣ������USB�м��ͨ��Э��
priority               ���ݵ����ȼ�
1����ʾ������ȼ�����451��������ʵʱ���ݲ�ѯ���������м�
�����ݷ����У���Ҫ��ʱ40MS���ٷ��ͷ�������
2����ʾ��ͨ���ȼ���������¼����ȡ���������м�����ݷ��͹�
���У���Ҫ��ʱ30MS
3����ʾ������ȼ����������¼�����ֵ���ܣ��������͹����в���
Ҫ������ʱ��������װ�÷������ݡ�
����ֵ��
ERROR_COMMUNICATION    ͨ�Ŵ���
ERROR_RESEND           ���ʹ���
SUCCESS_COMMUNICATION  ���ͳɹ�
���û������ⲿ����
�޸���־����
*************************************************************************************/
extern "C" int  _declspec (dllexport) Interface(unsigned char* serviceData, int priority);


/*************************************************************************************
�������ƣ�InitIntermediate
��Ҫ�������м���ʼ��������ÿ��ͨѶ�����У�����λ���������ͨѶʧ�ܶ���Ҫ���³�ʼ����
���
������
CallBackFunction       �м����յ�����ʱ�Ļص��������û������ڴ˺�����ʵ�����ݽ���
dInfo                  ����PID UID��USB�豸��Ϣ
Buffer_size            �м���з��ͻ������Ĵ�С��Ĭ��Ϊ50KB
����ֵ��
0                      ��ʼ��ʧ��
��0                      ��ʼ���ɹ�
���û������ⲿ����
�޸���־����
*************************************************************************************/
extern "C" BOOL  _declspec (dllexport) InitIntermediate(PFCALLBACK CallBackFunction,deviceInfo dInfo,int Buffer_size=WELLSIZE);	


/*************************************************************************************
�������ƣ�CloseMidSerial
��Ҫ�������ر��м��
��������
����ֵ����
���û������ⲿ����
�޸���־����
*************************************************************************************/
extern "C" VOID  _declspec (dllexport) CloseMidSerial();



#endif