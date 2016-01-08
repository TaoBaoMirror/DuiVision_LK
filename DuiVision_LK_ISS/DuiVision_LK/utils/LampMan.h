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




/*!> 串口参数 */
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
		unsigned char szHead[2];			//帧起始符：2字节,上位机  --'  单元控制板   0XEA 0XE5 表示
		unsigned char ucAddr;				//地址域：1字节,用一个字节表示数码管控制板的ID。第1个数码管控制板用0X00表示。共有16个数码管控制板，地址从0X00到0X0F
		unsigned char ucDataLen;			//数据长度: 1字节,表示命令码、数据域的字节数。目前每个存取窗只有5个两位数码管。只能用0X06表示。
		
		/*
		union UNCmdCode
		{
			struct 
			{
				unsigned char ucLed_1:1;	//D0位：对应数码管1；1表示点亮，0表示灭。
				unsigned char ucLed_2:1;	//D1位：对应数码管2；1表示点亮，0表示灭。
				unsigned char ucLed_3:1;	//D2位：对应数码管3；1表示点亮，0表示灭。
				unsigned char ucLed_4:1;	//D3位：对应数码管4；1表示点亮，0表示灭。
				unsigned char ucLed_5:1;	//D4位：对应数码管5；1表示点亮，0表示灭。
				unsigned char ucOth:3;	//其余D5位，D6位，D7位保留
			}v;
			
			unsigned char ucCmdCode;
		}l;			//命令码:  1字节
		*/

		/*
			0X00表示：数据域的字节对应数码管不为零关灯，为零保持。
			0X01表示：数据域的字节对应数码管不为零显示，为零不理会。
			0X02表示：数据域收到的数据全部显示。
			0X03表示：广播命令用检测数码管，全部显示88。
		*/
		unsigned char ucCmdCode;//命令码:  1字节

		
		//数据域：字节数由数据长度。但目前是5个字节
		//字节1～5是分别对应显示数码管1～5要显示的数，每个字节只能从0X00～0X63（0～99）。
		unsigned char ucDig_1;
		unsigned char ucDig_2;
		unsigned char ucDig_3;
		unsigned char ucDig_4;
		unsigned char ucDig_5;
		
		//校验码：1字节,是帧起始符、地址域、数据长度、命令码、数据域的字节和，使用累加和校验，一个字节，溢出忽略。
		unsigned char ucCheckSum;
		
	}StLampSendCmdFrame;

	typedef struct tagLampRecvAckFrame
	{
		unsigned char szHead[2];			//帧起始符：2字节,0XCA 0XC5 用表示

		unsigned char ucAddr;				//地址域：1字节,用一个字节表示数码管控制板的ID。第1个数码管控制板用0X00表示。共有16个数码管控制板，地址从0X00到0X0F
		unsigned char ucDataLen;			//数据长度: 1字节目前用0X01表示

		unsigned char ucAckCmd;			//命令有两种情况，正确的回复收到原命令码，错误的用0XA1表示

		//校验码：1字节,是帧起始符、地址域、数据长度、命令码、数据域的字节和，使用累加和校验，一个字节，溢出忽略。
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

	//查看、获取串口接收缓冲区有多少个字节等待接收？
	long GetComInBuffBytes(int &nCnt);

	//设置串口读写超时时延
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

	CRITICAL_SECTION m_cs;	//关键段

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