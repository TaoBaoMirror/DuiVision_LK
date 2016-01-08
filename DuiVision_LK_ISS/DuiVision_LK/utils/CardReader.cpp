// CardReader.cpp: implementation of the CCardReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <string>
#include <iostream>

#include "CardReader.h"
#include <process.h>
#include "General.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma pack(1)

typedef struct tagFrameHead
{
	unsigned char ucSTX;	/*!> 开始符 */
// 	unsigned char ucType;	/*!> 设备类型 */
// 	unsigned char ucAddr;	/*!> 地址 */
// 	unsigned char ucCmd;	/*!> 命令 */
	unsigned char ucLen;	/*!> 长度 */
	unsigned char ucStatus;	/*!> 状态 */
}StFrameHead;

typedef struct tagDataBlack
{
	unsigned char ucWindowID;		/*!> 窗口ID */
	unsigned char ucBasketIDHight;		/*!> 篮子高位 */
	unsigned char ucBasketIDLow;		/*!> 篮子低位 */
	unsigned char szReserve[13];	/*!> 暂时保留 */
}StDataBlack;

typedef struct tagDataFrameRecv
{
	StFrameHead stHead;		/*!> 帧头 */
	StDataBlack stData;		/*!> 数据段 */
	unsigned char ucXOR;	/*!> 异或校验值 */
	unsigned char ucETX;	/*!> 结束符 */
}StDataFrameRecv;

#pragma pack()

LoggerPtr CCardReader::m_pLogger = Logger::getLogger("CCardReader");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCardReader::CCardReader()
{
	m_bOpen  = false;
	
	m_hThread = NULL;
	
	m_stSP.nPort = configPar.getIntParameter("CardReader_Port");
	m_stSP.ulBaudRate = configPar.getIntParameter("CardReader_Baudrate");
	m_nRunMode = configPar.getIntParameter("RunMode"); 
	
	m_bEnable = false;

	m_bGet = false;

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

CCardReader::~CCardReader()
{
	m_bExit=true;
	Sleep(200);
	
	
	if (m_bOpen)
	{
		close();
	}
	
	DWORD   dwExitCode;
	if(GetExitCodeThread( m_hThread, &dwExitCode ) )
	{
		::TerminateThread(m_hThread,dwExitCode);
		CloseHandle(m_hThread);
	}

	DeleteCriticalSection(&m_cs);
}


long CCardReader::open()
{
	m_pLogger->trace("CCardReader::open start ...");
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}
	
	try
	{
		if ((m_stSP.nPort <= 0)
			|| m_stSP.nPort >= 255)
		{
			m_pLogger->error("bar card reader port error!");
			return ERROR_CARD_READER_SP_OPEN;
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
		m_pLogger->error("failed to open card reader port: " + tmp);
		pEx->Delete();
		return ERROR_CARD_READER_SP_OPEN;
	}
	
	m_pLogger->trace("CCardReader::open end ...");
	
	return 0;
}

long CCardReader::close()
{
	m_pLogger->trace("CCardReader::close start ...");
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}
	
	m_spMan.Close();
	
	m_pLogger->trace("CCardReader::close end ...");
	
	return 0;	
}


//查看、获取串口接收缓冲区有多少个字节等待接收？
long CCardReader::getComInBuffBytes(int &nCnt)
{
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}
	
	if (!m_spMan.IsOpen())
	{
		if(open())
		{
			m_pLogger->error("not opened card reader serial port.");
			return ERROR_CARD_READER_SP_OPEN;
		}
	}
	
	COMSTAT state;
    m_spMan.GetStatus(state);
    nCnt = state.cbInQue; 
	
    return 0;
}

long CCardReader::sendData(const unsigned char *pcucData, int nLen)
{
	m_pLogger->trace("CCardReader::sendData start ...");
	
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
				m_pLogger->error("not opened card reader serial port.");
				return ERROR_CARD_READER_SP_OPEN;
			}
		}
		
		if(nLen != m_spMan.Write(pcucData, nLen))
		{
			m_pLogger->error("card reader send data falsed .");
			return ERROR_CARD_READER_SP_SEND;
		}
		
	}
	catch(CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_pLogger->error("failed to send command: " + tmp);
		pEx->Delete();
		return ERROR_CARD_READER_SP_OPEN;
	}
	
	m_pLogger->trace("CCardReader::sendData end ...");
	
	return 0;
}


long CCardReader::recvData(unsigned char *pucData, int nLen)//想要读取的字节长度
{
	
	m_pLogger->trace("CCardReader::recvData start ...");
	
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
				m_pLogger->error("not opened card reader  serial port.");
				return ERROR_CARD_READER_SP_OPEN;
			}
		}
		
		if(!m_spMan.Read(pucData, nLen))
		{
			m_pLogger->error("card reader  RECV data falsed .");
			m_spMan.ClearReadBuffer();
			return ERROR_CARD_READER_SP_RECV;
		}
		
	}
	catch(CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		m_spMan.ClearReadBuffer();
		m_pLogger->error("failed to RECV command: " + tmp);
		pEx->Delete();
		return ERROR_CARD_READER_SP_RECV;
	}
	
	m_pLogger->trace("CCardReader::recvData end ...");
	
	return 0;
}

void CCardReader::run()
{
	m_pLogger->trace("CCardReader::run start ...");
	
	if (NULL == m_hThread)
	{
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, workThread, this, 0, 0);
		if (NULL == m_hThread)
		{
			m_pLogger->error("CCardReader::run create thread failed ...");
			//::AfxMessageBox("读卡器线程失败！");
			General::MessageBox(GetCurShowDlgWndHnd(), configPar.getValueParameter("Card_thread").c_str(), "", CMB_OK);
			return;
		}
	}
	
	m_pLogger->trace("CCardReader::run end ...");
}


//设置串口读写超时时延
void CCardReader::SetComTimeOut()
{
	m_pLogger->trace("function CCardReader::SetComTimeOut() start..............");
	
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
	
	m_pLogger->trace("function CCardReader::SetComTimeOut() end..............");
}


unsigned int  CCardReader::workThread(void* lp)
{
	long lRes;
	int nInBufBytes;
	
	StCardReaderID stCRID;
	
	m_pLogger->trace("CCardReader::workThread start ...");
	
	CCardReader *pCR = (CCardReader*)lp;
	
	while(true)
	{
		if (pCR->m_bExit==true)
		{
			return 0;
		}
		//已经使能读卡功能，可以读卡了
		if (pCR->IsEnable())
		{
			pCR->EnterCS();
			//先监测串口接收缓冲区，是否有字节接收到？
			lRes = pCR->getComInBuffBytes(nInBufBytes);
			pCR->LeaveCS();
			if (!lRes)
			{
				//串口接收缓冲区，有字节
				if (0 < nInBufBytes)
				{
				//	Sleep(1);
					pCR->EnterCS();
					lRes = pCR->UnpackFrameData(stCRID);
					pCR->LeaveCS();
					if (lRes)
					{
						//解析数据失败了
						pCR->m_spMan.ClearReadBuffer();
						continue;
					}
					else
					{
						PostMessage(pCR->GetCurShowDlgWndHnd(), WM_LK_CARD_READER, 
							WPARAM(&(stCRID)), 0);
							
						//pCR->SetBasketID(stCRID.strBasketID);

					}					
				}
				else//串口接收缓冲区，暂时尚无数据
				{
					Sleep(500);
				}
			}
			
		}
		else
		{
			Sleep(500);
		}
	}
	
	m_pLogger->trace("CCardReader::workThread end ...");
	
	return 0;
}


long CCardReader::UnpackFrameData(StCardReaderID &stCRID)
{
	long lRes;

	m_pLogger->trace("CCardReader::UnpackFrameData start ...");
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	int nHeadLen = sizeof(StFrameHead);
	//int nDataLen;

	unsigned char szFrame[32];
	memset(szFrame, 0, 32);
	
	//先读帧头
	lRes = recvData(szFrame, sizeof(StFrameHead));
	if(lRes)
	{
		return lRes;
	}

	StFrameHead *pstFH = (StFrameHead*)szFrame;

	/*!> 具体参数，可参看协议书 */
	if (!(0xAA == pstFH->ucSTX && 
	//	0x51 == pstFH->ucType &&
	//	0x00 == pstFH->ucAddr &&
	//	0x85 == pstFH->ucCmd &&
		0x11 == pstFH->ucLen &&
		0x00 == pstFH->ucStatus
		))
	{
		/*!> 读取帧头错误，则丢弃余下未读取的帧数据（脏数据） */
		m_spMan.ClearReadBuffer();
		return ERROR_CARD_READER_SP_UNPACK;
	}

	//再读取 数据块（16个字节） + XOR（1个字节） + 结束符（1个字节）
	lRes = recvData(szFrame + nHeadLen, sizeof(StDataBlack) + 2);
	if(lRes)
	{
		return lRes;
	}
	
	//校验异或值
	bool bRes = false;
	bRes = CheckXOR(szFrame, sizeof(StDataFrameRecv) - 1);//结束符不算在里面
	if (bRes)
	{
		return ERROR_CARD_READER_SP_UNPACK;
	} 

	unsigned short usBasketID;

	/*!> 过滤数据---- 窗口ID、篮子的ID */
	StDataFrameRecv *pstDFR;
	pstDFR = (StDataFrameRecv*)szFrame;
	stCRID.ucWindowID = pstDFR->stData.ucWindowID;
	usBasketID = 0x00;
	usBasketID = pstDFR->stData.ucBasketIDHight;
	usBasketID = (usBasketID<<8);
	usBasketID |= pstDFR->stData.ucBasketIDLow;

	stCRID.strBasketID = General::integertostring(usBasketID);

	m_pLogger->trace("CCardReader::UnpackFrameData end ...");
	
	return 0;
}


bool CCardReader::CheckXOR(unsigned char *pucDataFrame, int nLen)
{
	unsigned char ucXOR = 0;
	bool bRet = false;

	m_pLogger->trace("CCardReader::CheckXOR start ...");

	for (int i = 0; i < nLen-1; ++i)
	{
		ucXOR ^= pucDataFrame[i];
	}

	if (ucXOR == pucDataFrame[nLen-1])
	{
		bRet = true;
	} 
	else
	{
		bRet = false;
	}

	m_pLogger->trace("CCardReader::CheckXOR end ...");

	return bRet;
}