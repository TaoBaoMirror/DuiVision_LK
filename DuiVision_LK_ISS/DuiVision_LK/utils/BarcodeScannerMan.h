// BarcodeScannerMan.h: interface for the CBarcodeScannerMan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BARCODESCANNERMAN_H__868CD639_3418_4EDE_8C3C_78F1B55167D1__INCLUDED_)
#define AFX_BARCODESCANNERMAN_H__868CD639_3418_4EDE_8C3C_78F1B55167D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Serialport.h"


#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
#include "General.h"

#include "ErrorCodeRes.h"

#include <string>
using namespace std;


class CBarcodeScannerMan  
{
public:

	/*!> 串口参数 */
	
	typedef struct tagSerialParam
	{
		int nPort;
		
		unsigned long ulBaudRate;
		
	}StSerialParam;

	CBarcodeScannerMan();
	virtual ~CBarcodeScannerMan();

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
		m_bEnable = bE;
	}


	long TurnOn();
	long TurnOff();
	
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

private:
	
	StSerialParam		m_stSP;//扫描枪的串口参数
	
	//串口管理器
	CSerialPort		m_spMan;
	
	//串口是否正常
	bool m_bOpen;

	CRITICAL_SECTION m_cs;	//关键段

	bool m_bEnable;			//是否使能扫描功能？
		
	static LoggerPtr	m_pLogger;
	
	HANDLE m_hThread;
	
	//接收窗口的句柄
	HWND m_hCurShowDlgWinHnd;

	int m_nRunMode;	//运行模式 

	bool m_bQRCode;	//是否为二维扫描码,1---二维(QR Code)，0---一维
	
	bool m_bExit;
};

#endif // !defined(AFX_BARCODESCANNERMAN_H__868CD639_3418_4EDE_8C3C_78F1B55167D1__INCLUDED_)
