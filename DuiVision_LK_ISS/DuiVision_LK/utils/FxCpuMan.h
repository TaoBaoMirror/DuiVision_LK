#ifndef AFX_FX_CPU_MAN_LK_H___5FCD036F_4B5F_4239_94DD_B5EC2177A928__INCLUDED_
#define AFX_FX_CPU_MAN_LK_H___5FCD036F_4B5F_4239_94DD_B5EC2177A928__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "actfxcpu3.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include "XMLConfig.h"
#include "SingletonMode.h"
#include <string>
#include <list>
using namespace std;

#include "Serialport.h"

using namespace std;
using namespace log4cxx;


typedef enum tagMsgPLC
{
	LEFT_SERVO_ABNORMAL,		//���ŷ��쳣
	LEFT_SERVO_NORMAL,			//���ŷ�����
	RIGHT_SERVO_ABNORMAL,		//���ŷ��쳣
	RIGHT_SERVO_NORMAL,			//���ŷ�����

	FRONT_SAFE_RASTER_ABNORMAL,			//ǰ�� ��ȫ��Ļ�쳣
	FRONT_SAFE_RASTER_NORMAL,			//ǰ�� ��ȫ��Ļ����
	BACK_SAFE_RASTER_ABNORMAL,			//���� ��ȫ��Ļ�쳣
	BACK_SAFE_RASTER_NORMAL,			//���� ��ȫ��Ļ����

	FRONT_DOOR_OPEN_FALSE,			//ǰ�Ŵ�ʧ��
	FRONT_DOOR_OPEN_TRUE,			//ǰ�Ŵ򿪳ɹ�

	FRONT_DOOR_CLOSE_FALSE,			//ǰ�Źر�ʧ��
	FRONT_DOOR_CLOSE_TRUE,			//ǰ�Źرճɹ�
		
	BACK_DOOR_OPEN_FALSE,			//���Ŵ�ʧ��
	BACK_DOOR_OPEN_TRUE,			//���Ŵ򿪳ɹ�

	BACK_DOOR_CLOSE_FALSE,			//���Źر�ʧ��
	BACK_DOOR_CLOSE_TRUE,			//���Źرճɹ�
		
	SERVO_A_RUN_CHUNK_DONE_FALSE,	//�߶�ʧ��------�����߶�------A�ŷ�
	SERVO_A_RUN_CHUNK_DONE_TRUE,	//�߶��ɹ�------�����߶�------A�ŷ�

	SERVO_B_RUN_CHUNK_DONE_FALSE,	//�߶�ʧ��------�����߶�------B�ŷ�
	SERVO_B_RUN_CHUNK_DONE_TRUE,	//�߶��ɹ�------�����߶�------B�ŷ�

	SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, //��ȡ����ʱ���߶�ʧ��------A�ŷ�
	SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_TRUE,  //��ȡ����ʱ���߶��ɹ�------A�ŷ�

	SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, //��ȡ����ʱ���߶�ʧ��------B�ŷ�
	SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_TRUE,  //��ȡ����ʱ���߶��ɹ�------B�ŷ�

	SERVO_A_HANDLE_HAD_DONE,		//��ȡ����Ӳȷ�����------A�ŷ�
	SERVO_B_HANDLE_HAD_DONE,		//��ȡ����Ӳȷ�����------B�ŷ�
	
	SCRAM_BUTTON_DOWN,
	SCRAM_BUTTON_UP
}EnMsgPLC;


class CFxCpuMan
{
public:
	typedef enum tagDirect
	{
		LEFT_CHEST,
		RIGHT_CHEST,
	}EnDirect;

public:
	CFxCpuMan();
	~CFxCpuMan();

public:
	
	//
	long InitOpen(CActFXCPU3 *pFxCom);
    //
	long OpenDoor(int nOperType, bool bBack = false);
	//
	long CloseDoor(int nOperType, bool bBack = false);

	//�߶�----�����߶�
	long RunChunk(int nLocID, int nOperType, bool bBack = false);

	//�߶�----��-----��ȡ����
	long RunChunkAndHandle(int nLocID, int nOperType);

	//��ڲ�����Ϊtrue���л���ǰ��Ļ��Ϊfalse���л�����Ļ
	long SwitchScreen(bool bFront = true);

	//���������ƣ�Ĭ��ȱʡֵΪtrue--�򿪣�false--�ر�
	long TurnBuzzer(bool bOn = true);

	CActFXCPU3* GetActFxCpu(){ return m_pFxCom;	}

	//���á���ȷ��ȡҩ��ɡ�----- �ŷ�A
	void SetSoftSureServoA(bool bss = true){ m_bSoftSureServoA = bss; }

	bool GetSoftSureServoA(){	return m_bSoftSureServoA; }

	//���á���ȷ��ȡҩ��ɡ�----- �ŷ�B
	void SetSoftSureServoB(bool bss = true){ m_bSoftSureServoB = bss; }
	
	bool GetSoftSureServoB(){	return m_bSoftSureServoB; }

	void SetErrorPLC(bool bEP = true){ m_bErrorPLC = bEP; }

	bool GetErrorPLC(){ return m_bErrorPLC; }

	//���ŷ��쳣---����---��ȡ
	void SetErrorLeftServo(bool bELS = true){ m_bErrorLeftServo = bELS; }
	bool GetErrorLeftServo(){ return m_bErrorLeftServo; }

	//���ŷ��쳣---����---��ȡ
	void SetErrorRightServo(bool bERS = true){ m_bErrorRightServo = bERS; }
	bool GetErrorRightServo(){ return m_bErrorRightServo; }

	void SetSafeSensorAbnormFront (bool bSSA = true){ m_bSafeSensorAbnormFront = bSSA; }
	bool GetSafeSensorAbnormFront () { return m_bSafeSensorAbnormFront; };

	void SetSafeSensorAbnormBack (bool bSSA = true){ m_bSafeSensorAbnormBack = bSSA; }
	bool GetSafeSensorAbnormBack () { return m_bSafeSensorAbnormBack; };

	int GetAckTypeServoA(){ return m_nAckTypeServoA; }
	void SetAckTypeServoA(int nAckType)
	{
		TRACE("SetAckTypeServoA()   %d\n",nAckType);
		m_nAckTypeServoA = nAckType;
	}
	int GetAckTypeServoB(){ return m_nAckTypeServoB; }
	void SetAckTypeServoB(int nAckType)
	{		
		m_nAckTypeServoB = nAckType;
	}

	inline void EnterCS()
	{
		EnterCriticalSection(&m_cs);
	}

	inline void LeaveCS()
	{
		LeaveCriticalSection(&m_cs);
	}

	HWND GetCurShowDlgWndHnd(){	TRACE("GetCurShowDlgWndHnd()  %x  \n",m_hCurShowDlgWndHnd);return m_hCurShowDlgWndHnd; }
	
	inline void SetCurShowDlgWndHnd(HWND hwnd)
	{
		m_hCurShowDlgWndHnd=hwnd;
	}
	void SetCurShowDlgWndHnd(string strCurShowDlgWndName)
	{
		//���󣬷���NULL
		m_hCurShowDlgWndHnd = FindWindow(NULL, strCurShowDlgWndName.c_str());
	}

	HWND GetPlcMonitorDlgWndHnd(){ return m_hPlcMonitorDlgWndHnd;}
	void SetPlcMonitorDlgWndHnd(const HWND &hWndHnd)
	{
		m_hPlcMonitorDlgWndHnd = hWndHnd;
	}

	//���ö��ľ���λ��
	long SetChunkPosWindowA(int nChunk,int nPos);
	long SetChunkPosWindowB(int nChunk,int nPos);
	long SetChunkPosWindowC(int nChunk,int nPos);
	long SetChunkPosWindowD(int nChunk,int nPos);

	//��ȡ���ľ���λ��
	int GetChunkPosWindowA(int nChunk){ return m_nChunkPosWindowA[nChunk]; }
	int GetChunkPosWindowB(int nChunk){ return m_nChunkPosWindowB[nChunk]; }
	int GetChunkPosWindowC(int nChunk){ return m_nChunkPosWindowC[nChunk]; }
	int GetChunkPosWindowD(int nChunk){ return m_nChunkPosWindowD[nChunk]; }

	long RunChunkWindowA(long nChunkNum);//A�����߶�
	long RunChunkWindowB(long nChunkNum);//B�����߶�
	long RunChunkWindowC(long nChunkNum);//C�����߶�
	long RunChunkWindowD(long nChunkNum);//D�����߶�
	
	long TurnOffAfterSoftSureA();
	long TurnOffAfterSoftSureB();
	long TurnOffAfterSoftSureC();
	long TurnOffAfterSoftSureD();

protected:
	//��ǰ��
	long OpenFrontDoor();
	//
	long CloseFrontDoor();

	//
	long OpenBackDoor();
	//
	long CloseBackDoor();

	//nDirect���أ����ҹ�
	//nChunkNum���أ�����
	void Convert2ChunkNum(const int nLocID, int &nDirect, int &nChunkNum);
	
public:
	
	long ChangeScreen(int nOperType, bool bFront = true);

	//��ȷ�Ϻ󣬹ر�ָʾ��
	long TurnOffAfterSoftSure(int nLocID, int nOperType);

	//���Ƿ��Ѿ��Ǵ�״̬�ˣ�
	long IsDoorOpened(bool &bRes, int nOperType, bool bFront = true);

	static LoggerPtr  m_rootLoggerPtr;
private:
	CActFXCPU3 *m_pFxCom;

	

	int m_nTierNum;	//�����ļ�-----ÿ�����ӵĲ���
	int m_nTierCapacity;//ÿ��Ĳ���

	int m_nSumSlot;//ÿ�����ӵĲ���

	int m_nPortNum;//PLC�Ĵ��ں�

	bool m_bDoubleFace;		//�Ƿ���˫����

	bool m_bSoftSureServoA;		//��ȷ�ϡ���ɡ�����-----�ŷ�A
	bool m_bSoftSureServoB;		//��ȷ�ϡ���ɡ�����-----�ŷ�B

	bool m_bErrorPLC;		//PLC�����쳣��

	bool m_bErrorLeftServo;	//���ŷ��쳣
	bool m_bErrorRightServo;	//���ŷ��쳣

	CRITICAL_SECTION m_cs;	//�ؼ���

	bool m_bSafeSensorAbnormFront;//ǰ�� ��ȫ��Ļ�쳣��
	bool m_bSafeSensorAbnormBack;//���� ��ȫ��Ļ�쳣��

	int m_nAckTypeServoA;		//��ǰPLC����״̬----A�ŷ�
	int m_nAckTypeServoB;		//��ǰPLC����״̬----B�ŷ�

	HWND m_hCurShowDlgWndHnd;

	HWND m_hPlcMonitorDlgWndHnd;

	bool m_bIsOpen;			//�Ƿ��Ѿ����ˣ�����ģʽ��

	int m_nRunMode;

	CXMLConfig *m_pSysConfig;
    SingletonMode<CXMLConfig> m_singletonConfig;

	long m_nChunkPosWindowA[6];//A���ڵĶ��ľ���λ��
	long m_nChunkPosWindowB[6];//B���ڵĶ��ľ���λ��
	long m_nChunkPosWindowC[6];//C���ڵĶ��ľ���λ��
	long m_nChunkPosWindowD[6];//D���ڵĶ��ľ���λ��
};



#endif	/*!> end of AFX_FX_CPU_MAN_LK_H___5FCD036F_4B5F_4239_94DD_B5EC2177A928__INCLUDED_ */