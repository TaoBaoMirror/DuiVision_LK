#ifndef __RFID_MAN_H__
#define __RFID_MAN_H__

#include "afxwin.h"
#include "afxcmn.h"

#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#include <string>
#include <list>
using namespace std;


#define  LED_TURN_ON_INTER_TIME (10 * 1000) //led�Ƶ����Ƴ���ʱ��10s��
#define  LED_TURN_OFF_INTER_TIME (1 * 1000)	//�صƶ�ʱ��ÿ��1s�룬�鿴�Ƿ�����ҪϨ��

#include "RFIDAPIEXPORT.h"

typedef struct tagTagRecord
{
	DWORD dwStartTm;
	string strLeft;
	string strRight;
}TagRecord;

class CRfidMan
{
public:
	CRfidMan();
	~CRfidMan();

public:

	HANDLE GetComHandle();
	bool ReConnect();

	void SetConneted(bool bFlag);

	bool LedTurnOn(string strLeft, string strRight);

	bool LedTurnOff(string strLeft, string strRight);

	list<TagRecord>& GetTagRecordList()
	{
		return m_listTagRecord;
	}

private:

	bool Connect();

private:

	bool m_bConType;

	unsigned char m_ucFreq;	//�ز�����(Ƶ��)

	string m_strIP;//����IP������Ĭ��Ϊ192.168.0.238

	int m_nNetPort;//���ڶ˿ںţ�Ĭ��ֵΪ7086

	HANDLE m_hCom;//ͨѶ���Ӿ�������ڱ���ʹ�����ڷ�ʽ�򿪺�����Ӿ�� 

	volatile bool m_bIsConnected;

	list<TagRecord> m_listTagRecord;

	MMRESULT m_hTimer;

	CRITICAL_SECTION m_cs;
	

};


#endif