// BarcodeScannerMan.cpp: implementation of the CBarcodeScannerMan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <string>
#include <iostream>


#include "BarcodeScannerMan.h"

#include <process.h>

#include "General.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//一维扫描器的启停命令码
const unsigned char g_szOnCmd[] = "$$$$#99900114;%%%%";
const unsigned char g_szOffCmd[] = "$$$$#99900111;%%%%";
//二维扫描器的启停命令码
const unsigned char g_szOnCmdQrCode[3] = {0x1b, 0x33, 0x00};
const unsigned char g_szOffCmdQrCode[3] = {0x1b, 0x30, 0x00};


LoggerPtr CBarcodeScannerMan::m_pLogger = Logger::getLogger("CBarcodeScannerMan");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBarcodeScannerMan::CBarcodeScannerMan()
{
	m_bOpen  = false;
	
	m_hThread = NULL;
	
	m_stSP.nPort = configPar.getIntParameter("BarcodeScanner_Port");
	m_stSP.ulBaudRate = configPar.getIntParameter("BarcodeScanner_Baudrate");
	m_nRunMode = configPar.getIntParameter("RunMode"); 

	m_bQRCode = false;
	m_bQRCode = (1 == configPar.getIntParameter("BarcodeScanner_QR_Code"))?true:false;

	m_bEnable = false;

	InitializeCriticalSection(&m_cs);
	
	if(!open())
	{
		m_bOpen = true;
	}
	else
	{
		m_bOpen = false;
	}
	m_bExit=false;
}

CBarcodeScannerMan::~CBarcodeScannerMan()
{
// 	if (m_bOpen)
// 	{
		TurnOff();
// 	}
	DeleteCriticalSection(&m_cs);
	m_bExit=true;
	Sleep(100);
	DWORD   dwExitCode;
	if(GetExitCodeThread( m_hThread, &dwExitCode ) )
	{
		::TerminateThread(m_hThread,dwExitCode);
		CloseHandle(m_hThread);
	}


}


long CBarcodeScannerMan::open()
{
	m_pLogger->trace("CBarcodeScannerMan::open start ...");

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	try
	{
		if ((m_stSP.nPort <= 0)
			|| m_stSP.nPort >= 255)
		{
			m_pLogger->error("bar code scanner port error!");
			return ERROR_BARCODE_SCANNER_SP_OPEN;
		}
		
		if (!m_spMan.IsOpen())
		{
			m_spMan.Open(m_stSP.nPort, m_stSP.ulBaudRate);

			SetComTimeOut();
			m_spMan.ClearReadBuffer();
			m_spMan.ClearWriteBuffer();
		}
	}
	catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_pLogger->error("failed to open barcode scanner port: " + tmp);
		pEx->Delete();
		return ERROR_BARCODE_SCANNER_SP_OPEN;
	}

	m_pLogger->trace("CBarcodeScannerMan::open end ...");

	return 0;
}

long CBarcodeScannerMan::close()
{
	m_pLogger->trace("CBarcodeScannerMan::close start ...");
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_spMan.Close();

	m_pLogger->trace("CBarcodeScannerMan::close end ...");
	
	return 0;	
}


//查看、获取串口接收缓冲区有多少个字节等待接收？
long CBarcodeScannerMan::getComInBuffBytes(int &nCnt)
{
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	if (!m_spMan.IsOpen())
	{
		if(open())
		{
			m_pLogger->error("not opened bar code scanner serial port.");
			return ERROR_BARCODE_SCANNER_SP_OPEN;
		}
	}
	
	COMSTAT state;
    m_spMan.GetStatus(state);
    nCnt = state.cbInQue; 
	
    return 0;
}

long CBarcodeScannerMan::sendData(const unsigned char *pcucData, int nLen)
{
	m_pLogger->trace("CBarcodeScannerMan::sendData start ...");

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	try
	{
		if (!m_spMan.IsOpen())
		{
			if (open())
			{
				m_pLogger->error("not opened bar code scanner serial port.");
				return ERROR_BARCODE_SCANNER_SP_OPEN;
			}
		}

		if(nLen != m_spMan.Write(pcucData, nLen))
		{
			m_pLogger->error("bar code scanner send data falsed .");
			return ERROR_BARCODE_SCANNER_SP_SEND;
		}

	}
	catch(CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_pLogger->error("failed to send command: " + tmp);
		pEx->Delete();
		return ERROR_BARCODE_SCANNER_SP_OPEN;
	}

	m_pLogger->trace("CBarcodeScannerMan::sendData end ...");

	return 0;
}


long CBarcodeScannerMan::recvData(unsigned char *pucData, int nLen)
{

	m_pLogger->trace("CBarcodeScannerMan::recvData start ...");
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	try
	{
		if (!m_spMan.IsOpen())
		{
			if (open())
			{
				m_pLogger->error("not opened bar code scanner  serial port.");
				return ERROR_BARCODE_SCANNER_SP_OPEN;
			}
		}
		
		if(!m_spMan.Read(pucData, nLen))
		{
			m_pLogger->error("bar code scanner  RECV data falsed .");
			m_spMan.ClearReadBuffer();
			return ERROR_BARCODE_SCANNER_SP_RECV;
		}
		
	}
	catch(CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_spMan.ClearReadBuffer();
		m_pLogger->error("failed to RECV command: " + tmp);
		pEx->Delete();
		return ERROR_BARCODE_SCANNER_SP_RECV;
	}
	
	m_pLogger->trace("CBarcodeScannerMan::recvData end ...");
	
	return 0;
}


void CBarcodeScannerMan::run()
{
	m_pLogger->trace("CBarcodeScannerMan::run start ...");

	if (NULL == m_hThread)
	{
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, workThread, this, 0, 0);
		if (NULL == m_hThread)
		{
			m_pLogger->error("CBarcodeScannerMan::run create thread failed ...");
			//::AfxMessageBox("条码扫描线程失败！");
			General::MessageBox(GetCurShowDlgWndHnd(), configPar.getValueParameter("Scanner_Failed").c_str(), "", CMB_OK);
			return;
		}
	}

	m_pLogger->trace("CBarcodeScannerMan::run end ...");
}


//读取条码
long CBarcodeScannerMan::readBarcode(string  &strBarcode)
{
	long lRes;

	m_pLogger->trace("CBarcodeScannerMan::readBarcode start ...");

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	unsigned char szData[256];

	memset(szData, 0, 256);

	lRes = recvData(szData, sizeof(szData));
	if(lRes)
	{
		return lRes;
	}

	strBarcode = (char*)szData;

	m_pLogger->trace("CBarcodeScannerMan::readBarcode end ...");

	return 0;

}

unsigned int  CBarcodeScannerMan::workThread(void* lp)
{
	long lRes;
	int nInBufBytes;

	string  strBarcode;

	m_pLogger->trace("CBarcodeScannerMan::workThread start ...");

	CBarcodeScannerMan *pBSM = (CBarcodeScannerMan*)lp;

	while(true)
	{
		if (pBSM->m_bExit==true)
		{
			return 0;
		}
		//已经使能扫描功能，可以扫描了
		if (pBSM->IsEnable())
		{
			pBSM->EnterCS();
			//先监测串口接收缓冲区，是否有字节接收到？
			lRes = pBSM->getComInBuffBytes(nInBufBytes);
			pBSM->LeaveCS();
			if (!lRes)
			{
				//串口接收缓冲区，有字节
				if (0 < nInBufBytes)
				{
					Sleep(1);
					pBSM->EnterCS();
					lRes = pBSM->readBarcode(strBarcode);
					pBSM->LeaveCS();
					if (lRes)
					{
						//读取数据失败了
						//Sleep(1);
						pBSM->m_spMan.ClearReadBuffer();
						continue;
					}
					else
					{
						strBarcode.erase(strBarcode.size()-2, 2);
						PostMessage(pBSM->GetCurShowDlgWndHnd(), WM_LK_BARCODE, 
							WPARAM(strBarcode.c_str()), 0);
					//	m_pLogger->debug("读取条码:"+strBarcode);
					}					
				}
// 				else//串口接收缓冲区，暂时尚无数据
// 				{
// 					Sleep(500);
// 				}
			}
			
		}
		else
		{
			Sleep(500);
		}
	}
	
	m_pLogger->trace("CBarcodeScannerMan::workThread end ...");

	return 0;
}


//设置串口读写超时时延
void CBarcodeScannerMan::SetComTimeOut()
{
	m_pLogger->trace("function CBarcodeScannerMan::SetComTimeOut() start..............");
	
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
	
	m_spMan.SetTimeouts(comTimeOut);
	
	m_pLogger->trace("function CBarcodeScannerMan::SetComTimeOut() end..............");
}


long CBarcodeScannerMan::TurnOn()
{
	long lRes;

	m_pLogger->trace("function CBarcodeScannerMan::TurnOn() start..............");

	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	SetEnable(false);
	Sleep(20);
	m_spMan.ClearWriteBuffer();
	EnterCS();
	if (m_bQRCode)//二维
	{
		lRes = sendData(g_szOnCmdQrCode, sizeof(g_szOnCmdQrCode));
	} 
	else//一维
	{
		lRes = sendData(g_szOnCmd, sizeof(g_szOnCmd));
	}
	LeaveCS();
	if (lRes)
	{
		m_spMan.ClearReadBuffer();
		SetEnable(false);
		m_pLogger->debug("打开扫描功能失败");
		return ERROR_BARCODE_SCANNER_SET;
		
	}

	Sleep(100);
	m_spMan.ClearReadBuffer();
	m_spMan.ClearWriteBuffer();
//	m_pLogger->debug("打开扫描功能");
	SetEnable();

	m_pLogger->trace("function CBarcodeScannerMan::TurnOn() end..............");

	return 0;
}

long CBarcodeScannerMan::TurnOff()
{
	long lRes;
		
	m_pLogger->trace("function CBarcodeScannerMan::TurnOff() start..............");
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	m_spMan.ClearWriteBuffer();
	SetEnable(false);
	Sleep(20);

	EnterCS();	
	if (m_bQRCode)//二维
	{
		lRes = sendData(g_szOffCmdQrCode, sizeof(g_szOffCmdQrCode));
	} 
	else//一维
	{
		lRes = sendData(g_szOffCmd, sizeof(g_szOffCmd));
	}
	LeaveCS();
	if (lRes)
	{
		m_spMan.ClearReadBuffer();
		SetEnable(true);
		m_pLogger->debug("关闭扫描功能失败");
		return ERROR_BARCODE_SCANNER_SET;	
	}

	Sleep(100);
	m_spMan.ClearReadBuffer();
	m_spMan.ClearWriteBuffer();
	SetEnable(false);
//	m_pLogger->debug("关闭扫描功能");
	m_pLogger->trace("function CBarcodeScannerMan::TurnOff() end..............");

	return 0;
}
