#ifndef __LAMP_MAN_LK_H__
#define __LAMP_MAN_LK_H__

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include <string>
#include <list>

#include "Serialport.h"

using namespace std;
using namespace log4cxx;




/*!> ���ڲ��� */
typedef struct tagLampSerialParam
{
	int nPort;
	
	unsigned long ulBaudRate;
	
}StLampSerialParam;

class CLampMan
{
private:

	enum
	{
		OFF_LAMP,
		ON_LAMP,
		ALL_ON_LAMP,
		BROAD_CAST_LAMP
	};

	enum 
	{
		DIG_1 = 0,
		DIG_2,
		DIG_3,
		DIG_4,
		DIG_5
	}EnDigNum;

#pragma pack(1)
	
	typedef struct tagLampSendCmdFrame
	{
		unsigned char szHead[2];			//֡��ʼ����2�ֽ�,��λ��  --'  ��Ԫ���ư�   0XEA 0XE5 ��ʾ
		unsigned char ucAddr;				//��ַ��1�ֽ�,��һ���ֽڱ�ʾ����ܿ��ư��ID����1������ܿ��ư���0X00��ʾ������16������ܿ��ư壬��ַ��0X00��0X0F
		unsigned char ucDataLen;			//���ݳ���: 1�ֽ�,��ʾ�����롢��������ֽ�����Ŀǰÿ����ȡ��ֻ��5����λ����ܡ�ֻ����0X06��ʾ��
		
		/*
		union UNCmdCode
		{
			struct 
			{
				unsigned char ucLed_1:1;	//D0λ����Ӧ�����1��1��ʾ������0��ʾ��
				unsigned char ucLed_2:1;	//D1λ����Ӧ�����2��1��ʾ������0��ʾ��
				unsigned char ucLed_3:1;	//D2λ����Ӧ�����3��1��ʾ������0��ʾ��
				unsigned char ucLed_4:1;	//D3λ����Ӧ�����4��1��ʾ������0��ʾ��
				unsigned char ucLed_5:1;	//D4λ����Ӧ�����5��1��ʾ������0��ʾ��
				unsigned char ucOth:3;	//����D5λ��D6λ��D7λ����
			}v;
			
			unsigned char ucCmdCode;
		}l;			//������:  1�ֽ�
		*/

		/*
			0X00��ʾ����������ֽڶ�Ӧ����ܲ�Ϊ��صƣ�Ϊ�㱣�֡�
			0X01��ʾ����������ֽڶ�Ӧ����ܲ�Ϊ����ʾ��Ϊ�㲻��ᡣ
			0X02��ʾ���������յ�������ȫ����ʾ��
			0X03��ʾ���㲥�����ü������ܣ�ȫ����ʾ88��
		*/
		unsigned char ucCmdCode;//������:  1�ֽ�

		
		//�������ֽ��������ݳ��ȡ���Ŀǰ��5���ֽ�
		//�ֽ�1��5�Ƿֱ��Ӧ��ʾ�����1��5Ҫ��ʾ������ÿ���ֽ�ֻ�ܴ�0X00��0X63��0��99����
		unsigned char ucDig_1;
		unsigned char ucDig_2;
		unsigned char ucDig_3;
		unsigned char ucDig_4;
		unsigned char ucDig_5;
		
		//У���룺1�ֽ�,��֡��ʼ������ַ�����ݳ��ȡ������롢��������ֽںͣ�ʹ���ۼӺ�У�飬һ���ֽڣ�������ԡ�
		unsigned char ucCheckSum;
		
	}StLampSendCmdFrame;

	typedef struct tagLampRecvAckFrame
	{
		unsigned char szHead[2];			//֡��ʼ����2�ֽ�,0XCA 0XC5 �ñ�ʾ

		unsigned char ucAddr;				//��ַ��1�ֽ�,��һ���ֽڱ�ʾ����ܿ��ư��ID����1������ܿ��ư���0X00��ʾ������16������ܿ��ư壬��ַ��0X00��0X0F
		unsigned char ucDataLen;			//���ݳ���: 1�ֽ�Ŀǰ��0X01��ʾ

		unsigned char ucAckCmd;			//�����������������ȷ�Ļظ��յ�ԭ�����룬�������0XA1��ʾ

		//У���룺1�ֽ�,��֡��ʼ������ַ�����ݳ��ȡ������롢��������ֽںͣ�ʹ���ۼӺ�У�飬һ���ֽڣ�������ԡ�
		unsigned char ucCheckSum;

	}StLampRecvAckFrame;
	
#pragma pack()

public:

	CLampMan();
	~CLampMan();

public:

	long Open();

	long TurnOnLamp(int nOperType, int nLocId, int nData);

	long TurnOffLamp(int nOperType, int nLocId, int nData);
protected:
	long SendCommand(const unsigned char *pcucCmd, int nLen);
	long RecvData(unsigned char *pucCmd, int nLen);
	
	long Close();

	long LampOperate(unsigned char ucCmd, unsigned char ucBoard,unsigned char ucSeqNum, unsigned char ucDigData);

	void FillInCmd(unsigned char ucCmd, unsigned char ucBoard,unsigned char ucSeqNum, unsigned char ucDigData);

	unsigned char CreateSum(unsigned char *pucBuf, int nLen);
	bool CheckSum(unsigned char *pucBuf, int nLen);

	bool VerifyAck(unsigned char ucBoard);

	void Convert(const int nOperType, const int nLocId, unsigned char &ucBoard, unsigned char &ucSeqNum);

	//�鿴����ȡ���ڽ��ջ������ж��ٸ��ֽڵȴ����գ�
	long GetComInBuffBytes(int &nCnt);

	//���ô��ڶ�д��ʱʱ��
	void SetComTimeOut();

	inline void EnterCS()
	{
		EnterCriticalSection(&m_cs);
	}
	
	inline void LeaveCS()
	{
		LeaveCriticalSection(&m_cs);
	}
private:

	CRITICAL_SECTION m_cs;	//�ؼ���

	StLampSerialParam m_stPortParam;

	static LoggerPtr  m_rootLoggerPtr;

	StLampSendCmdFrame m_stLSCF;
	StLampRecvAckFrame m_stLRAF;

	CSerialPort m_serialPort;

	int m_nTierNum;
	int m_nTierCapacity;
	int m_nShowTierNum;

	bool m_bDoubleFace;

	int m_nRunMode;
};


#endif  /*!> __LAMP_MAN_LK_H__ */