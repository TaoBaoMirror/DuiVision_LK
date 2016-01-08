#ifndef __RFID_MAN_H__
#define __RFID_MAN_H__

#include "afxwin.h"
#include "afxcmn.h"

#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#include <string>
#include <list>
using namespace std;


#define  LED_TURN_ON_INTER_TIME (10 * 1000) //led灯的亮灯持续时间10s秒
#define  LED_TURN_OFF_INTER_TIME (1 * 1000)	//关灯定时器每隔1s秒，查看是否有需要熄灯

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

	unsigned char m_ucFreq;	//载波参数(频率)

	string m_strIP;//网口IP，出厂默认为192.168.0.238

	int m_nNetPort;//网口端口号，默认值为7086

	HANDLE m_hCom;//通讯连接句柄，用于保存使用网口方式打开后的连接句柄 

	volatile bool m_bIsConnected;

	list<TagRecord> m_listTagRecord;

	MMRESULT m_hTimer;

	CRITICAL_SECTION m_cs;
	

};


#endif