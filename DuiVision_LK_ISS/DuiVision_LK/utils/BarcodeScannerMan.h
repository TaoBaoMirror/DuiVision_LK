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

	/*!> ���ڲ��� */
	
	typedef struct tagSerialParam
	{
		int nPort;
		
		unsigned long ulBaudRate;
		
	}StSerialParam;

	CBarcodeScannerMan();
	virtual ~CBarcodeScannerMan();

	//��д�����Ĵ����Ƿ�������
	inline bool getState()
	{
		return m_bOpen;
	}
	
	//��������ɨ��������������������߳�
	void run();
	
	//���ý��մ��ڵľ��
	inline void SetCurShowDlgWndHnd(HWND hWnd)
	{
		//���󣬷���NULL
		m_hCurShowDlgWinHnd = hWnd;
	}

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
	
	//�鿴����ȡ���ڽ��ջ������ж��ٸ��ֽڵȴ����գ�
	long getComInBuffBytes(int &nCnt);
	
	//�����߳�
	static unsigned int __stdcall workThread(void* lp);
	
	//��ȡ����
	long readBarcode(string  &strBarcode);
	
	
	//��ȡ���մ��ڵľ��
	inline HWND GetCurShowDlgWndHnd()
	{
		return m_hCurShowDlgWinHnd;
	}

private:
	
	StSerialParam		m_stSP;//ɨ��ǹ�Ĵ��ڲ���
	
	//���ڹ�����
	CSerialPort		m_spMan;
	
	//�����Ƿ�����
	bool m_bOpen;

	CRITICAL_SECTION m_cs;	//�ؼ���

	bool m_bEnable;			//�Ƿ�ʹ��ɨ�蹦�ܣ�
		
	static LoggerPtr	m_pLogger;
	
	HANDLE m_hThread;
	
	//���մ��ڵľ��
	HWND m_hCurShowDlgWinHnd;

	int m_nRunMode;	//����ģʽ 

	bool m_bQRCode;	//�Ƿ�Ϊ��άɨ����,1---��ά(QR Code)��0---һά
	
	bool m_bExit;
};

#endif // !defined(AFX_BARCODESCANNERMAN_H__868CD639_3418_4EDE_8C3C_78F1B55167D1__INCLUDED_)
