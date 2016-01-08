#include "StdAfx.h"
#include "LampMan.h"
#include "ErrorCodeRes.h"
#include "General.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;       
#endif

LoggerPtr CLampMan::m_rootLoggerPtr = Logger::getLogger("CLampMan");

CLampMan::CLampMan()
{
	m_rootLoggerPtr->trace("function CLampMan::CLampMan() start.............");

	m_stPortParam.nPort = configPar.getIntParameter("LampPort");
	m_stPortParam.ulBaudRate = configPar.getIntParameter("LampBaudRate");

	m_nTierNum = configPar.getIntParameter("TierNum");
	m_nTierCapacity = configPar.getIntParameter("TierCapacity");
	m_nShowTierNum = configPar.getIntParameter("ShowTierNum");
	m_bDoubleFace = ((0 == configPar.getIntParameter("DoubleFace")) ? false : true );
	m_nRunMode = configPar.getIntParameter("RunMode");
	memset(&m_stLSCF, 0, sizeof(StLampSendCmdFrame));
	
	//帧起始符：2字节,上位机  --'  单元控制板   0XEA 0XE5 表示
	m_stLSCF.szHead[0] = 0xEA;
	m_stLSCF.szHead[1] = 0xE5;
	
	//数据长度: 1字节,表示命令码、数据域的字节数。目前每个存取窗只有5个两位数码管。只能用0X06表示。
	m_stLSCF.ucDataLen = 0x06;

	Open();

	InitializeCriticalSection(&m_cs);
	
	m_rootLoggerPtr->trace("function CLampMan::CLampMan() end.............");

}

CLampMan::~CLampMan()
{
	m_rootLoggerPtr->trace("function ~CLampMan() start.............");
	
	DeleteCriticalSection(&m_cs);

	Close();
	
	m_rootLoggerPtr->trace("function ~CLampMan() end.............");
}


long CLampMan::SendCommand(const unsigned char *pcucCmd, int nLen)
{
	m_rootLoggerPtr->trace("function CLampMan::SendCommand() start .............");
	
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	try
	{
		if (!m_serialPort.IsOpen())
		{
			if(Open())
			{
				m_rootLoggerPtr->error("not opened digital lamp serial port.");
				return ERROR_LAMP_PORT_OPEN;
			}
			
		}
			
		if (nLen != m_serialPort.Write(pcucCmd, nLen))
		{
			return ERROR_LAMP_SP_SEND_DATA_INCONSISTENCY;
		}
		
	}
	catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_rootLoggerPtr->error("failed to send command: " + tmp);
		pEx->Delete();
		return ERROR_LAMP_SP_SEND;
	}	
	
	
	m_rootLoggerPtr->trace("function CLampMan::SendCommand() end .............");

	return 0;
}

//查看、获取串口接收缓冲区有多少个字节等待接收？
long CLampMan::GetComInBuffBytes(int &nCnt)
{
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	if (!m_serialPort.IsOpen())
	{
		if(Open())
		{
			m_rootLoggerPtr->error("not opened thermal printer serial port.");
			return ERROR_LAMP_PORT_OPEN;
		}
	}

	COMSTAT state;
    m_serialPort.GetStatus(state);
    nCnt = state.cbInQue; 

    return 0;
}

long CLampMan::RecvData(unsigned char *pucCmd, int nLen)
{
	m_rootLoggerPtr->trace("function CLampMan::RecvData() start .............");
	
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	int nInBuffBytes = 0;
	try
	{
		if (!m_serialPort.IsOpen())
		{
			if(Open())
			{
				m_rootLoggerPtr->error("not opened digital lamp serial port.");
				return ERROR_LAMP_PORT_OPEN;
			}
		}
		
		/*
		GetComInBuffBytes(nInBuffBytes);
		//串口接收缓存区的字节数和需要接收到的字节数不一致
		if (nInBuffBytes != nLen)
		{
			//直接退出得了，免得读取脏数据（或避免阻塞读取，虽已设置读写IO超时了）
			return ERROR_LAMP_SP_RECV_DATA_INCONSISTENCY;
		}
		*/

		if (!m_serialPort.Read(pucCmd, nLen))
		{
			return ERROR_LAMP_SP_RECV_NULL;
		}
	}
	catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_rootLoggerPtr->error("failed to recv command: " + tmp);
		pEx->Delete();
		return ERROR_LAMP_SP_RECV;
	}
	
	m_rootLoggerPtr->trace("function CLampMan::RecvData() end .............");
	
	return 0;
}



long CLampMan::Open()
{
	m_rootLoggerPtr->trace("function CLampMan::Open() start..............");
	
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	try
	{
		if ((m_stPortParam.nPort <= 0)
			|| m_stPortParam.nPort >= 255)
		{
			m_rootLoggerPtr->error("printer port error!");
			return ERROR_LAMP_PORT_OPEN;
		}
		
		if (!m_serialPort.IsOpen())
		{
			m_serialPort.Open(m_stPortParam.nPort, m_stPortParam.ulBaudRate);
            SetComTimeOut();
			m_serialPort.ClearReadBuffer();
			m_serialPort.ClearWriteBuffer();
			//m_serialPort.Set0WriteTimeout();
		}
	}
	catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_rootLoggerPtr->error("failed to open printer port: " + tmp);
		pEx->Delete();
		return ERROR_LAMP_PORT_OPEN;
	}
	
	m_rootLoggerPtr->trace("function CLampMan::Open() end..............");
	
	return 0;
}

long CLampMan::Close()
{
	m_rootLoggerPtr->trace("function CLampMan::Close() start..............");
		
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	m_serialPort.Close();
	
	m_rootLoggerPtr->trace("function CLampMan::Close() end..............");

	return 0;
}

//设置串口读写超时时延
void CLampMan::SetComTimeOut()
{
	m_rootLoggerPtr->trace("function CLampMan::SetComTimeOut() start..............");

	COMMTIMEOUTS comTimeOut;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return ;
	}
	
	// 接收时，两字符间最大的时延
	comTimeOut.ReadIntervalTimeout = 10;
	
	// 读取每字节的超时
	comTimeOut.ReadTotalTimeoutMultiplier = 10;
	
	// 读串口数据的固定超时
	// 总超时 = ReadTotalTimeoutMultiplier * 字节数 + ReadTotalTimeoutConstant
	comTimeOut.ReadTotalTimeoutConstant = 50;
	
	// 写每字节的超时
	comTimeOut.WriteTotalTimeoutMultiplier = 10;
	
	// 写串口数据的固定超时
    comTimeOut.WriteTotalTimeoutConstant = 10;

	m_serialPort.SetTimeouts(comTimeOut);

	m_rootLoggerPtr->trace("function CLampMan::SetComTimeOut() end..............");
}

void CLampMan::Convert(const int nOperType, const int nLocId, unsigned char &ucBoard, unsigned char &ucSeqNum)
{

	int nSingleSum;
	bool  bLeft;
	
	//单个伺服的药槽个数;(24层，每层5个槽)
	nSingleSum = m_nTierNum * m_nTierCapacity;
		
	//是左？右？伺服机(LOC_ID,数据库存储的是从1开始的)
	bLeft = ((0 == (nLocId - 1) / nSingleSum)? true : false);

	/*
		每个可视窗口，有4个控制板，
		每个控制板，可控制5个槽
		即：每个可视窗口，可看到4x5 = 20 个槽
	*/
	ucBoard =(unsigned char) ((nLocId - 1) % (m_nTierCapacity * m_nShowTierNum))/m_nTierCapacity;
	ucSeqNum =(unsigned char) (nLocId - 1) % m_nTierCapacity;
	if (bLeft)
	{
		ucBoard +=(unsigned char) (0 * m_nShowTierNum);//0
	}
	else
	{
		ucBoard +=(unsigned char) (1 * m_nShowTierNum);//4
	}
// 	switch (nOperType)
// 	{
// 	case PRE_FETCH_OPER://只在前门操作
// 		{
// 			if (bLeft)
// 			{
// 				ucBoard +=(unsigned char) (0 * m_nShowTierNum);//0
// 			}
// 			else
// 			{
// 				ucBoard +=(unsigned char) (1 * m_nShowTierNum);//4
// 			}
// 		}
// 		break;
// 	case BATCH_ADD_OPER:
// 	case MED_MAN_OPER:
// 		{
// 				if (bLeft)
// 				{
// 					ucBoard += (unsigned char)(0 * m_nShowTierNum);//0
// 				}
// 				else
// 				{
// 					ucBoard += (unsigned char)(1 * m_nShowTierNum);//4
// 				}
// 		}
// 		break;
// 	case ADJUST_OPER:
// 		break;
// 	default:
// 		break;
// 	}
}

long CLampMan::TurnOnLamp(int nOperType, int nLocId, int nDigData)
{
	m_rootLoggerPtr->trace("function CLampMan::TurnOnLamp() start..............");
	
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	long lRes;
	/*
	unsigned char ucBoard = 0x00;
	unsigned char ucSeqNum = 3;
	unsigned char ucDigData = 36;
	*/
	unsigned char ucBoard=0;
	unsigned char ucSeqNum=0;

	
	Convert(nOperType, nLocId, ucBoard, ucSeqNum);
	EnterCS();
	lRes = LampOperate(ON_LAMP, ucBoard, ucSeqNum, (unsigned char)nDigData);
	LeaveCS();
	if (!lRes)
	{
		return lRes;
	}
	m_rootLoggerPtr->trace("function CLampMan::TurnOnLamp() end..............");
	return 0;
}

long CLampMan::TurnOffLamp(int nOperType, int nLocId, int nDigData)
{
	m_rootLoggerPtr->trace("function CLampMan::TurnOffLamp() start..............");
	
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	long lRes;
	
	/*
	unsigned char ucBoard = 0x00;
	unsigned char ucSeqNum = 3;
	unsigned char ucDigData = 36;
	*/
	unsigned char ucBoard;
	unsigned char ucSeqNum;

	Convert(nOperType, nLocId, ucBoard, ucSeqNum);
	
	EnterCS();
	lRes = LampOperate(OFF_LAMP, ucBoard, ucSeqNum, nDigData);
	LeaveCS();
	if (!lRes)
	{
		return lRes;
	}	
	m_rootLoggerPtr->trace("function CLampMan::TurnOffLamp() end..............");
	return 0;
}

long CLampMan::LampOperate(unsigned char ucCmd, unsigned char ucBoard,unsigned char ucSeqNum, unsigned char ucDigData)
{
	long lRes;

	m_rootLoggerPtr->trace("function CLampMan::LampOperate() start..............");

	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}


	//共有16个数码管控制板，地址从0X00到0X0F
	if (0x00 > ucBoard || 0x0F < ucBoard)
	{
		m_rootLoggerPtr->debug("显示层数不对"+General::integertostring(ucBoard));
		return ERROR_LAMP_PARAM;
	}

	//一个控制板，只能控制5个两位数码管
	if (DIG_1 > ucSeqNum || DIG_5 < ucSeqNum)
	{
		m_rootLoggerPtr->debug("显示槽数不对"+General::integertostring(ucSeqNum));
		return ERROR_LAMP_PARAM;
	}

	//只能从0X00～0X63（0～99）
	if (0x00 > ucDigData )
	{
		m_rootLoggerPtr->debug("显示值大于99");
		return ERROR_LAMP_PARAM;
	}
	if (0x63 < ucDigData)
	{
		ucDigData=0x63;
	}
	FillInCmd(ucCmd,ucBoard,ucSeqNum,ucDigData);
	int index=0;
	while(index<5)
	{
		lRes = SendCommand((const unsigned char *)&m_stLSCF, sizeof(StLampSendCmdFrame));
		m_rootLoggerPtr->debug("数码管发送指令:"+General::integertostring((int)ucCmd)+"-"+General::integertostring((int)ucBoard)+"-"+General::integertostring((int)ucSeqNum)+"-"+General::integertostring((int)ucDigData)+"");
		if (lRes)
		{
			m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));			
			m_serialPort.ClearReadBuffer();
			m_serialPort.ClearWriteBuffer();			
			return lRes;
		}			
		Sleep(50);
		int nRes=0;
		GetComInBuffBytes(nRes);
		if (nRes>0)
			break;
		else
			index++;
	}
	memset((unsigned char*)&m_stLRAF, 0, sizeof(StLampRecvAckFrame));
	
	lRes = RecvData((unsigned char*)&m_stLRAF, sizeof(StLampRecvAckFrame));
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to RecvData(), code:" + General::integertostring(lRes));

		m_serialPort.ClearReadBuffer();
		m_serialPort.ClearWriteBuffer();

		return lRes;
	}
	
	if (!VerifyAck(ucBoard))
	{
		return ERROR_LAMP_VERIFY_SUM;
	}
	m_serialPort.ClearReadBuffer();
	m_serialPort.ClearWriteBuffer();
	m_rootLoggerPtr->trace("function CLampMan::LampOperate() end..............");

	return 0;
}

void CLampMan::FillInCmd(unsigned char ucCmd, unsigned char ucBoard,unsigned char ucSeqNum, unsigned char ucDigData)
{
	m_rootLoggerPtr->trace("function CLampMan::FillInCmd() start..............");

	m_stLSCF.ucAddr =  ucBoard;
	
	m_stLSCF.ucCmdCode = ucCmd;

	switch(ucSeqNum)
	{
	case DIG_1:
		m_stLSCF.ucDig_1 = ucDigData;
		m_stLSCF.ucDig_2 = 0x00;
		m_stLSCF.ucDig_3 = 0x00;
		m_stLSCF.ucDig_4 = 0x00;
		m_stLSCF.ucDig_5 = 0x00;
		break;
	case DIG_2:
		m_stLSCF.ucDig_1 = 0x00;
		m_stLSCF.ucDig_2 = ucDigData;
		m_stLSCF.ucDig_3 = 0x00;
		m_stLSCF.ucDig_4 = 0x00;
		m_stLSCF.ucDig_5 = 0x00;
		break;
	case DIG_3:
		m_stLSCF.ucDig_1 = 0x00;
		m_stLSCF.ucDig_2 = 0x00;
		m_stLSCF.ucDig_3 = ucDigData;
		m_stLSCF.ucDig_4 = 0x00;
		m_stLSCF.ucDig_5 = 0x00;
		break;
	case DIG_4:
		m_stLSCF.ucDig_1 = 0x00;
		m_stLSCF.ucDig_2 = 0x00;
		m_stLSCF.ucDig_3 = 0x00;
		m_stLSCF.ucDig_4 = ucDigData;
		m_stLSCF.ucDig_5 = 0x00;
		break;
	case DIG_5:
		m_stLSCF.ucDig_1 = 0x00;
		m_stLSCF.ucDig_2 = 0x00;
		m_stLSCF.ucDig_3 = 0x00;
		m_stLSCF.ucDig_4 = 0x00;
		m_stLSCF.ucDig_5 = ucDigData;
		break;
	default:
		break;
	}

	m_stLSCF.ucCheckSum = CreateSum((unsigned char *)&m_stLSCF, sizeof(StLampSendCmdFrame) - 1);

	m_rootLoggerPtr->trace("function CLampMan::FillInCmd() end..............");
}

unsigned char CLampMan::CreateSum(unsigned char *pucBuf, int nLen)
{
	unsigned char ucSum = 0;

	for (int i = 0; i < nLen; ++i)
	{
		ucSum += pucBuf[i];
	}

	return ucSum;
}

bool CLampMan::CheckSum(unsigned char *pucBuf, int nLen)
{
	unsigned char ucSum = 0;

	for (int i = 0; i < nLen; ++i)
	{
		ucSum += pucBuf[i];
	}

	if (ucSum == pucBuf[nLen])
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CLampMan::VerifyAck(unsigned char ucBoard)
{
#if 0
	if (ucBoard != m_stLRAF.ucAddr)
	{
		return false;
	}
#endif

	if(!CheckSum((unsigned char *)&m_stLRAF, sizeof(StLampRecvAckFrame) - 1))
	{
		return false;
	}

	if (0xA1 == m_stLRAF.ucAckCmd)
	{
		return false;
	}

	return true;
	 
}