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
		if( (GetTickCount() - dwStart) > 3000 ) //ÿ3��鿴һ�������Ƿ����
		{
			//���ͨѶ��·������״̬
			if (!SAAT_HeartSend(pRfidMan->GetComHandle()))
			{
				//�豸�����ѶϿ���������
				if( !pRfidMan->ReConnect() )//����ʧ��
				{//�������û�ȡ��
					pRfidMan->SetConneted(false);
				}
				else//�����ɹ�				
				{
					pRfidMan->SetConneted(true);
				}
			}

			dwStart = GetTickCount();
		}
	}

	return 0;
}


//��ʱ���ص�������
void WINAPI TimerCallbackFunc(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
{
	CRfidMan *pRridMan = (CRfidMan*)dwUser;

	if (pRridMan->GetTagRecordList().size())
	{
		TagRecord tr = pRridMan->GetTagRecordList().front();
		if( (GetTickCount() - tr.dwStartTm) > (LED_TURN_ON_INTER_TIME)) //ÿ10��鿴һ���Ƿ���ҪϨ�Ʋ���
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

	//��ȡ�����ļ�����ȡ����

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

//���������1s����������������Connect()��������ֱ�����ӳɹ�Ϊֹ
bool CRfidMan::ReConnect()
{
	//�ȹرգ�������
	if( m_hCom != INVALID_HANDLE_VALUE)
	{
		SAAT_Close( m_hCom );
		m_hCom = INVALID_HANDLE_VALUE;
	}

	return Connect();
}

//�����Ƿ��ѽ�������
void CRfidMan::SetConneted(bool bFlag)
{
	m_bIsConnected = bFlag;
}


//����
bool CRfidMan::Connect()
{
	if (m_hCom != INVALID_HANDLE_VALUE)
		return TRUE;

	bool bResult = false;	//���ӳ�ʼ������ֵ
	char *szPara;

	szPara = (char*)m_strIP.c_str();
	
	//////////////////////////////////////////////////////////////////////////
	//TCP������ʼ��
	//���ܣ�	ͨ���������룬��ʼ��TCP��Ϊ��������׼��
	//������
	//		m_hCom 		Ϊ����򿪵Ķ˿ھ��
	//		szPara		������IP��ַ����������ͨѶģʽ����Ч
	//		nNetPort	����SOCKET�˿�
	//����ֵ�� trueΪ�����ɹ�, falseΪ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	bResult = SAAT_TCPInit(&m_hCom, szPara, m_nNetPort);//���ڳ�ʼ������

	if( !bResult )
		return false; 

	//����������̫���ӿ�֮һ������
	if (SAAT_Open(m_hCom))
	{
		//�������ö�д�����ز�����
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
	//ʮ�����ƣ���λ��ռһ���ֽ�
	unsigned char szData[3] = {0};
	unsigned char uc;

	unsigned char szBuf[16] = {0};
	unsigned char szTmp[16] = {0};
	memset(szBuf, 0, sizeof(szBuf));
	memset(szTmp, 0, sizeof(szTmp));

	//ת��ʮ������ֵ
	int nVal = atoi(strTagID.c_str());

	//��ʽ����ʮ�������ַ���
	sprintf((char*)szTmp, "%X", nVal);

	//ʮ�������ַ������������������ַ���������ǰ������ճ�ż������
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
		//��λʮ�����ƣ�ռһ���ֽ�
		strncpy((char*)szData, (const char*)(szBuf + i*2), 2);
		//sscanf((const char *)szData, "%x", &uc);
		uc = (unsigned char)strtol((const char *)szData, NULL, 16);
		pcBuf[i] = uc;
	}
}

//ѡ�������ϵ�������ǩ�����е��
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

		//��������ģʽ-���ұ�ǩ,�������ñ�ǩ����������LED��״̬
		//��������LEDʹ�ܣ�1�ֽ� ����Ӧλ����1��ʾʹ�ܣ�
		//һ���ֽ�8bit����6λ������LED,BUZZER.
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
		
		//��������ģʽ-���ұ�ǩ,�������ñ�ǩ����������LED��״̬
		//��������LEDʹ�ܣ�1�ֽ� ����Ӧλ����1��ʾʹ�ܣ�
		//һ���ֽ�8bit����6λ������LED,BUZZER.
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




