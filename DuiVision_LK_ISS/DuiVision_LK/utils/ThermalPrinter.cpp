#include "stdafx.h"

#include "ThermalPrinter.h"
#include "ErrorCodeRes.h"
#include "General.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;       
#endif



LoggerPtr CThermalPrinter::m_rootLoggerPtr = Logger::getLogger("CThermalPrinter");

CThermalPrinter::CThermalPrinter()
{
	m_rootLoggerPtr->trace("function CThermalPrinter start..........");
	m_nRunMode = configPar.getIntParameter("RunMode");  
}

CThermalPrinter::~CThermalPrinter()
{
	m_rootLoggerPtr->trace("function ~CThermalPrinter start..........");

	Close();

	m_rootLoggerPtr->trace("function ~CThermalPrinter end..........");
}


//设置串口读写超时时延
void CThermalPrinter::SetComTimeOut()
{
	m_rootLoggerPtr->trace("function CThermalPrinter::SetComTimeOut() start..............");
	
	COMMTIMEOUTS comTimeOut;
	
	// 接收时，两字符间最大的时延
	comTimeOut.ReadIntervalTimeout = 1000;
	
	// 读取每字节的超时
	comTimeOut.ReadTotalTimeoutMultiplier = 1000;
	
	// 读串口数据的固定超时
	// 总超时 = ReadTotalTimeoutMultiplier * 字节数 + ReadTotalTimeoutConstant
	comTimeOut.ReadTotalTimeoutConstant = 1000;
	
	// 写每字节的超时
	comTimeOut.WriteTotalTimeoutMultiplier = 1000;
	
	// 写串口数据的固定超时
    comTimeOut.WriteTotalTimeoutConstant = 1000;
	
	m_serialPort.SetTimeouts(comTimeOut);
	
	m_rootLoggerPtr->trace("function CThermalPrinter::SetComTimeOut() end..............");
}

long CThermalPrinter::Open()
{
	m_rootLoggerPtr->trace("function CThermalPrinter::Open() start..............");
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}
	try
	{
		if ((m_printerSerialParam.nPort <= 0)
			|| m_printerSerialParam.nPort >= 255)
		{
			m_rootLoggerPtr->error("printer port error!");
			return ERROR_THERMAL_PRINT_SP_OPEN;
		}
		
		if (!m_serialPort.IsOpen())
		{
			m_serialPort.Open(m_printerSerialParam.nPort, m_printerSerialParam.ulBaudRate);
            //SetComTimeOut();
			m_serialPort.Set0WriteTimeout();
		}
	}
	catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_rootLoggerPtr->error("failed to open printer port: " + tmp);
		pEx->Delete();
		return ERROR_THERMAL_PRINT_SP_OPEN;
	}
	
	m_rootLoggerPtr->trace("function CThermalPrinter::Open() end..............");
	
	return 0;
}

long CThermalPrinter::Close()
{
	m_rootLoggerPtr->trace("function CLampMan::Close() start..............");
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}
	m_serialPort.Close();
	
	m_rootLoggerPtr->trace("function CLampMan::Close() end..............");
	return 0;
}

//查看、获取串口接收缓冲区有多少个字节等待接收？
long CThermalPrinter::GetComInBuffBytes(int &nCnt)
{
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}
	if (!m_serialPort.IsOpen())
	{
		if(Open())
		{
			m_rootLoggerPtr->error("not opened thermal printer serial port.");
			return ERROR_THERMAL_PRINT_SP_OPEN;
		}
	}
	
	COMSTAT state;
    m_serialPort.GetStatus(state);
    nCnt = state.cbInQue; 
	
    return 0;
}


/************************************************************************/
/* 发送命令至打印机，失败返回错误码，成功返回0
/* pccCmd：指令内容
nLen: 长度
/************************************************************************/
long CThermalPrinter::SendCommand(const unsigned char *pcucCmd, int nLen)
{
	m_rootLoggerPtr->trace("function CThermalPrinter::SendCommand() start .............");
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}
	try
	{
		if (!m_serialPort.IsOpen())
		{
			if(Open())
			{
				m_rootLoggerPtr->error("not opened thermal printer serial port.");
				return ERROR_THERMAL_PRINT_SP_OPEN;
			}			
		}		
		m_serialPort.Write(pcucCmd, nLen);		
	}
	catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_rootLoggerPtr->error("failed to send command: " + tmp);
		pEx->Delete();
		return ERROR_THERMAL_PRINT_SP_SEND;
	}	
		
	m_rootLoggerPtr->trace("function CThermalPrinter::SendCommand() end .............");
	
	return 0;
}

long CThermalPrinter::RecvData(unsigned char *pucCmd, int nLen)
{
	m_rootLoggerPtr->trace("function CThermalPrinter::RecvData() start .............");
	if (m_nRunMode == 1 || m_nRunMode == 9)
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
				return ERROR_THERMAL_PRINT_SP_OPEN;
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

		m_serialPort.Read(pucCmd, nLen);
	}
	catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_rootLoggerPtr->error("failed to recv command: " + tmp);
		pEx->Delete();
		return ERROR_THERMAL_PRINT_SP_RECV;
	}
	
	m_rootLoggerPtr->trace("function CThermalPrinter::RecvData() end .............");
	
	return 0;
}


long CThermalPrinter::GetState()
{
	return 0;
}

