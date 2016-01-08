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

/*!> ��Ϣ�ϴ�������---����ID����ID */
typedef struct tagCardReaderID
{
	unsigned char ucWindowID;
	string strBasketID;
}StCardReaderID;

class CCardReader  
{
public:
	/*!> ���ڲ��� */
	
	typedef struct tagSerialParam
	{
		int nPort;
		
		unsigned long ulBaudRate;
		
	}StSerialParam;

public:
	CCardReader();
	virtual ~CCardReader();

public:
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
		//����ս��ջ�����
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

	long UnpackFrameData(StCardReaderID & stCRID);

private:
	bool CheckXOR(unsigned char *pucDataFrame, int nLen);

private:
	StSerialParam		m_stSP;//�������Ĵ��ڲ���
	
	//���ڹ�����
	CSerialPort		m_spMan;
	
	//�����Ƿ�����
	bool m_bOpen;
	
	CRITICAL_SECTION m_cs;	//�ؼ���
	
	bool m_bEnable;			//�Ƿ�ʹ�ܶ������ܣ�
	
	static LoggerPtr	m_pLogger;
	
	HANDLE m_hThread;
	
	//���մ��ڵľ��
	HWND m_hCurShowDlgWinHnd;
	
	int m_nRunMode;	//����ģʽ

	string m_strBasketID;
	bool m_bGet;

	bool m_bExit;
};

#endif // !defined(AFX_CARDREADER_H__EFAFD5F1_12BB_49A6_8B06_C60F403932A1__INCLUDED_)
