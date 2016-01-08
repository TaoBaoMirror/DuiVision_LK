// CardReader.h: interface for the CCardReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARDREADER_H__EFAFD5F1_12BB_49A6_8B06_C60F403932A1__INCLUDED_)
#define AFX_CARDREADER_H__EFAFD5F1_12BB_49A6_8B06_C60F403932A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Serialport.h"


#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;


#include "ErrorCodeRes.h"
#include "General.h"
#include <string>
using namespace std;

/*!> 消息上传的数据---窗口ID，卡ID */
typedef struct tagCardReaderID
{
	unsigned char ucWindowID;
	string strBasketID;
}StCardReaderID;

class CCardReader  
{
public:
	/*!> 串口参数 */
	
	typedef struct tagSerialParam
	{
		int nPort;
		
		unsigned long ulBaudRate;
		
	}StSerialParam;

public:
	CCardReader();
	virtual ~CCardReader();

public:
	//读写卡器的串口是否启动？
	inline bool getState()
	{
		return m_bOpen;
	}
	
	//启动条码扫描操作，启动监听工作线程
	void run();
	
	//设置接收窗口的句柄
	inline void SetCurShowDlgWndHnd(HWND hWnd)
	{
		//错误，返回NULL
		m_hCurShowDlgWinHnd = hWnd;
	}
	
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

	inline bool IsEnable()
	{
		return m_bEnable;
	}
	
	inline void SetEnable(bool bE = true)
	{
		//先清空接收缓冲区
		if (1 == m_nRunMode || 9 == m_nRunMode)
		{
			return ;
		}
		m_spMan.ClearReadBuffer();
		m_spMan.ClearWriteBuffer();
		m_bEnable = bE;
	}

	void SetBasketID(const string &strBasketID)
	{
		m_strBasketID = strBasketID;
		m_bGet = true;
	}

	bool GetBasketID( string &strBasketID)
	{
		if (m_bGet)
		{
			strBasketID = m_strBasketID;
			m_bGet = false;
			return true;
		} 
		else
		{
			return false;
		}
	}
	long open();

protected:
	long close();
	
	long sendData(const unsigned char *pcucData, int nLen);
	long recvData(unsigned char *pucData, int nLen);
	
	//查看、获取串口接收缓冲区有多少个字节等待接收？
	long getComInBuffBytes(int &nCnt);
	
	//工作线程
	static unsigned int __stdcall workThread(void* lp);
	
	//读取条码
	long readBarcode(string  &strBarcode);
	
	
	//获取接收窗口的句柄
	inline HWND GetCurShowDlgWndHnd()
	{
		return m_hCurShowDlgWinHnd;
	}

	long UnpackFrameData(StCardReaderID & stCRID);

private:
	bool CheckXOR(unsigned char *pucDataFrame, int nLen);

private:
	StSerialParam		m_stSP;//读卡器的串口参数
	
	//串口管理器
	CSerialPort		m_spMan;
	
	//串口是否正常
	bool m_bOpen;
	
	CRITICAL_SECTION m_cs;	//关键段
	
	bool m_bEnable;			//是否使能读卡功能？
	
	static LoggerPtr	m_pLogger;
	
	HANDLE m_hThread;
	
	//接收窗口的句柄
	HWND m_hCurShowDlgWinHnd;
	
	int m_nRunMode;	//运行模式

	string m_strBasketID;
	bool m_bGet;

	bool m_bExit;
};

#endif // !defined(AFX_CARDREADER_H__EFAFD5F1_12BB_49A6_8B06_C60F403932A1__INCLUDED_)
