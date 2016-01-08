#include "stdafx.h"
#include <process.h>

#include "RfidMan.h"

#pragma comment(lib, "RFIDAPI.lib")

bool g_bRun = true;

unsigned int __stdcall ThreadFun(void * lp)
{

	CRfidMan *pRfidMan = (CRfidMan*)lp;

	DWORD dwStart = GetTickCount();

	while(g_bRun)
	{
		if( (GetTickCount() - dwStart) > 3000 ) //每3秒查看一下连接是否断线
		{
			//检测通讯链路的连接状态
			if (!SAAT_HeartSend(pRfidMan->GetComHandle()))
			{
				//设备连接已断开，则重连
				if( !pRfidMan->ReConnect() )//重连失败
				{//重连被用户取消
					pRfidMan->SetConneted(false);
				}
				else//重连成功				
				{
					pRfidMan->SetConneted(true);
				}
			}

			dwStart = GetTickCount();
		}
	}

	return 0;
}


//定时器回调处理函数
void WINAPI TimerCallbackFunc(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
{
	CRfidMan *pRridMan = (CRfidMan*)dwUser;

	if (pRridMan->GetTagRecordList().size())
	{
		TagRecord tr = pRridMan->GetTagRecordList().front();
		if( (GetTickCount() - tr.dwStartTm) > (LED_TURN_ON_INTER_TIME)) //每10秒查看一下是否需要熄灯操作
		{
			pRridMan->LedTurnOff(tr.strLeft, tr.strRight);
			
			pRridMan->GetTagRecordList().pop_front();
		}
	}
	
	return;
}


CRfidMan::CRfidMan()
{

#if 0

	//读取配置文件，获取参数

#else

	m_ucFreq = 0x0A;

	//m_bConType;
	
	m_strIP = "192.168.0.238";
	
	m_nNetPort = 7086;
#endif
	
	m_hCom = INVALID_HANDLE_VALUE;

	if (Connect())
	{
		m_bIsConnected = true;
	}

	InitializeCriticalSection(&m_cs);

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFun, this, 0, 0);

	m_hTimer = NULL;
	//m_hTimer = timeSetEvent(LED_TURN_OFF_INTER_TIME, 1,(LPTIMECALLBACK)TimerCallbackFunc, (DWORD)this,TIME_PERIODIC);

}

CRfidMan::~CRfidMan()
{
	g_bRun = false;

	Sleep(1);

	if(INVALID_HANDLE_VALUE != m_hCom)
	{
		SAAT_Close( m_hCom );
		m_hCom = INVALID_HANDLE_VALUE;
	}

	if (NULL != m_hTimer)
	{
		timeKillEvent(m_hTimer);
	}

	DeleteCriticalSection(&m_cs);
}


HANDLE CRfidMan::GetComHandle()
{
	return m_hCom;
}

//重连，间隔1s，不断重连（调用Connect()函数），直到连接成功为止
bool CRfidMan::ReConnect()
{
	//先关闭，句柄清空
	if( m_hCom != INVALID_HANDLE_VALUE)
	{
		SAAT_Close( m_hCom );
		m_hCom = INVALID_HANDLE_VALUE;
	}

	return Connect();
}

//设置是否已建立连接
void CRfidMan::SetConneted(bool bFlag)
{
	m_bIsConnected = bFlag;
}


//连接
bool CRfidMan::Connect()
{
	if (m_hCom != INVALID_HANDLE_VALUE)
		return TRUE;

	bool bResult = false;	//连接初始化返回值
	char *szPara;

	szPara = (char*)m_strIP.c_str();
	
	//////////////////////////////////////////////////////////////////////////
	//TCP参数初始化
	//功能：	通过参数传入，初始化TCP，为打开连接做准备
	//参数：
	//		m_hCom 		为保存打开的端口句柄
	//		szPara		本机的IP地址，仅在网络通讯模式下有效
	//		nNetPort	网络SOCKET端口
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bResult = SAAT_TCPInit(&m_hCom, szPara, m_nNetPort);//网口初始化函数

	if( !bResult )
		return false; 

	//创建基于以太网接口之一的连接
	if (SAAT_Open(m_hCom))
	{
		//用于配置读写器的载波参数
		//SAAT_YRFParaSet (m_hCom,0x00, m_ucFreq);
		return true;
	}
	else
	{
		return false;
	}
}

void Transform(unsigned char*pcBuf, unsigned char &nLen, string &strTagID)
{
	//十六进制，二位，占一个字节
	unsigned char szData[3] = {0};
	unsigned char uc;

	unsigned char szBuf[16] = {0};
	unsigned char szTmp[16] = {0};
	memset(szBuf, 0, sizeof(szBuf));
	memset(szTmp, 0, sizeof(szTmp));

	//转成十进制数值
	int nVal = atoi(strTagID.c_str());

	//格式化成十六进制字符串
	sprintf((char*)szTmp, "%X", nVal);

	//十六进制字符串，若是奇数个数字符，则在最前面填零凑成偶数个数
	if (0 == (strlen((const char*)szTmp))%2)
	{
		nLen = (unsigned char)(strlen((const char*)szTmp)/2);
		strncpy((char *)szBuf, (char *)szTmp, strlen((char*)szTmp));
	}
	else
	{
		nLen = (unsigned char)( (strlen((const char*)szTmp)+1) / 2);

		strncpy((char *)szBuf+1, (char *)szTmp, strlen((char*)szTmp));
		szBuf[0] = '0';
	}

	for (int i = 0; i < nLen; ++i)
	{
		memset(szData, 0, 3);
		//二位十六进制，占一个字节
		strncpy((char*)szData, (const char*)(szBuf + i*2), 2);
		//sscanf((const char *)szData, "%x", &uc);
		uc = (unsigned char)strtol((const char *)szData, NULL, 16);
		pcBuf[i] = uc;
	}
}

//选择篮子上的两个标签，进行点灯
bool CRfidMan::LedTurnOn(string strLeft, 
						 string strRight)
{
	bool bRes = false;
	unsigned char szBuf[16] = {0};
	unsigned char ucLen = 0;

	TagRecord tr;

	if (m_bIsConnected)
	{
		EnterCriticalSection(&m_cs);

		Transform(szBuf, ucLen, strLeft);

		//被动读卡模式-查找标签,用于设置标签蜂鸣器或者LED灯状态
		//蜂鸣器、LED使能（1字节 ，相应位设置1表示使能）
		//一个字节8bit，高6位保留，LED,BUZZER.
		bRes = SAAT_YTagSelect(m_hCom, 0x03, 0x01, szBuf, ucLen);
		if (!bRes)
		{
			LeaveCriticalSection(&m_cs);
			return false;
		}

		tr.strLeft = strLeft;
		tr.dwStartTm = GetTickCount();

		//
		Sleep(100);

		memset(szBuf, 0, sizeof(szBuf));
		ucLen = 0;
		Transform(szBuf, ucLen, strRight);

		bRes = SAAT_YTagSelect(m_hCom, 0x03, 0x01, szBuf, ucLen);
		
		if (!bRes)
		{
			LeaveCriticalSection(&m_cs);
			return false;
		}

		LeaveCriticalSection(&m_cs);

		tr.strRight = strRight;

		m_listTagRecord.push_back(tr);
	}
	else
	{
		return false;
	}

	return true;
}


bool CRfidMan::LedTurnOff(string strLeft, string strRight)
{
	bool bRes = false;
	unsigned char szBuf[16] = {0};
	unsigned char ucLen = 0;

	if (m_bIsConnected)
	{
		EnterCriticalSection(&m_cs);

		Transform(szBuf, ucLen, strLeft);
		
		//被动读卡模式-查找标签,用于设置标签蜂鸣器或者LED灯状态
		//蜂鸣器、LED使能（1字节 ，相应位设置1表示使能）
		//一个字节8bit，高6位保留，LED,BUZZER.
		bRes = SAAT_YTagSelect(m_hCom, 0x00, 0x01, szBuf, ucLen);
		if (!bRes)
		{
			LeaveCriticalSection(&m_cs);
			return false;
		}
		
		Sleep(100);
		
		memset(szBuf, 0, sizeof(szBuf));
		ucLen = 0;
		Transform(szBuf, ucLen, strRight);
		
		bRes = SAAT_YTagSelect(m_hCom, 0x00, 0x01, szBuf, ucLen);
		
		if (!bRes)
		{
			LeaveCriticalSection(&m_cs);
			return false;
		}

		LeaveCriticalSection(&m_cs);
	}
	else
	{
		return false;
	}
	
	return true;
}




