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
	LEFT_SERVO_ABNORMAL,		//左伺服异常
	LEFT_SERVO_NORMAL,			//左伺服正常
	RIGHT_SERVO_ABNORMAL,		//右伺服异常
	RIGHT_SERVO_NORMAL,			//右伺服正常

	FRONT_SAFE_RASTER_ABNORMAL,			//前门 安全光幕异常
	FRONT_SAFE_RASTER_NORMAL,			//前门 安全光幕正常
	BACK_SAFE_RASTER_ABNORMAL,			//后门 安全光幕异常
	BACK_SAFE_RASTER_NORMAL,			//后门 安全光幕正常

	FRONT_DOOR_OPEN_FALSE,			//前门打开失败
	FRONT_DOOR_OPEN_TRUE,			//前门打开成功

	FRONT_DOOR_CLOSE_FALSE,			//前门关闭失败
	FRONT_DOOR_CLOSE_TRUE,			//前门关闭成功
		
	BACK_DOOR_OPEN_FALSE,			//后门打开失败
	BACK_DOOR_OPEN_TRUE,			//后门打开成功

	BACK_DOOR_CLOSE_FALSE,			//后门关闭失败
	BACK_DOOR_CLOSE_TRUE,			//后门关闭成功
		
	SERVO_A_RUN_CHUNK_DONE_FALSE,	//走斗失败------纯粹走斗------A伺服
	SERVO_A_RUN_CHUNK_DONE_TRUE,	//走斗成功------纯粹走斗------A伺服

	SERVO_B_RUN_CHUNK_DONE_FALSE,	//走斗失败------纯粹走斗------B伺服
	SERVO_B_RUN_CHUNK_DONE_TRUE,	//走斗成功------纯粹走斗------B伺服

	SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, //存取操作时，走斗失败------A伺服
	SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_TRUE,  //存取操作时，走斗成功------A伺服

	SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, //存取操作时，走斗失败------B伺服
	SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_TRUE,  //存取操作时，走斗成功------B伺服

	SERVO_A_HANDLE_HAD_DONE,		//存取操作硬确认完成------A伺服
	SERVO_B_HANDLE_HAD_DONE,		//存取操作硬确认完成------B伺服
	
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

	//走斗----纯粹走斗
	long RunChunk(int nLocID, int nOperType, bool bBack = false);

	//走斗----和-----存取操作
	long RunChunkAndHandle(int nLocID, int nOperType);

	//入口参数，为true，切换到前屏幕，为false，切换后屏幕
	long SwitchScreen(bool bFront = true);

	//蜂鸣器控制，默认缺省值为true--打开，false--关闭
	long TurnBuzzer(bool bOn = true);

	CActFXCPU3* GetActFxCpu(){ return m_pFxCom;	}

	//设置“软确认取药完成”----- 伺服A
	void SetSoftSureServoA(bool bss = true){ m_bSoftSureServoA = bss; }

	bool GetSoftSureServoA(){	return m_bSoftSureServoA; }

	//设置“软确认取药完成”----- 伺服B
	void SetSoftSureServoB(bool bss = true){ m_bSoftSureServoB = bss; }
	
	bool GetSoftSureServoB(){	return m_bSoftSureServoB; }

	void SetErrorPLC(bool bEP = true){ m_bErrorPLC = bEP; }

	bool GetErrorPLC(){ return m_bErrorPLC; }

	//左伺服异常---设置---获取
	void SetErrorLeftServo(bool bELS = true){ m_bErrorLeftServo = bELS; }
	bool GetErrorLeftServo(){ return m_bErrorLeftServo; }

	//右伺服异常---设置---获取
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
		//错误，返回NULL
		m_hCurShowDlgWndHnd = FindWindow(NULL, strCurShowDlgWndName.c_str());
	}

	HWND GetPlcMonitorDlgWndHnd(){ return m_hPlcMonitorDlgWndHnd;}
	void SetPlcMonitorDlgWndHnd(const HWND &hWndHnd)
	{
		m_hPlcMonitorDlgWndHnd = hWndHnd;
	}

	//设置斗的绝对位置
	long SetChunkPosWindowA(int nChunk,int nPos);
	long SetChunkPosWindowB(int nChunk,int nPos);
	long SetChunkPosWindowC(int nChunk,int nPos);
	long SetChunkPosWindowD(int nChunk,int nPos);

	//获取斗的绝对位置
	int GetChunkPosWindowA(int nChunk){ return m_nChunkPosWindowA[nChunk]; }
	int GetChunkPosWindowB(int nChunk){ return m_nChunkPosWindowB[nChunk]; }
	int GetChunkPosWindowC(int nChunk){ return m_nChunkPosWindowC[nChunk]; }
	int GetChunkPosWindowD(int nChunk){ return m_nChunkPosWindowD[nChunk]; }

	long RunChunkWindowA(long nChunkNum);//A窗口走斗
	long RunChunkWindowB(long nChunkNum);//B窗口走斗
	long RunChunkWindowC(long nChunkNum);//C窗口走斗
	long RunChunkWindowD(long nChunkNum);//D窗口走斗
	
	long TurnOffAfterSoftSureA();
	long TurnOffAfterSoftSureB();
	long TurnOffAfterSoftSureC();
	long TurnOffAfterSoftSureD();

protected:
	//开前门
	long OpenFrontDoor();
	//
	long CloseFrontDoor();

	//
	long OpenBackDoor();
	//
	long CloseBackDoor();

	//nDirect返回：左右柜
	//nChunkNum返回：斗数
	void Convert2ChunkNum(const int nLocID, int &nDirect, int &nChunkNum);
	
public:
	
	long ChangeScreen(int nOperType, bool bFront = true);

	//软确认后，关闭指示灯
	long TurnOffAfterSoftSure(int nLocID, int nOperType);

	//门是否已经是打开状态了？
	long IsDoorOpened(bool &bRes, int nOperType, bool bFront = true);

	static LoggerPtr  m_rootLoggerPtr;
private:
	CActFXCPU3 *m_pFxCom;

	

	int m_nTierNum;	//配置文件-----每个柜子的层数
	int m_nTierCapacity;//每层的槽数

	int m_nSumSlot;//每个柜子的槽数

	int m_nPortNum;//PLC的串口号

	bool m_bDoubleFace;		//是否有双面门

	bool m_bSoftSureServoA;		//软确认“完成”操作-----伺服A
	bool m_bSoftSureServoB;		//软确认“完成”操作-----伺服B

	bool m_bErrorPLC;		//PLC控制异常？

	bool m_bErrorLeftServo;	//左伺服异常
	bool m_bErrorRightServo;	//右伺服异常

	CRITICAL_SECTION m_cs;	//关键段

	bool m_bSafeSensorAbnormFront;//前门 安全光幕异常？
	bool m_bSafeSensorAbnormBack;//后门 安全光幕异常？

	int m_nAckTypeServoA;		//当前PLC反馈状态----A伺服
	int m_nAckTypeServoB;		//当前PLC反馈状态----B伺服

	HWND m_hCurShowDlgWndHnd;

	HWND m_hPlcMonitorDlgWndHnd;

	bool m_bIsOpen;			//是否已经打开了（单粒模式）

	int m_nRunMode;

	CXMLConfig *m_pSysConfig;
    SingletonMode<CXMLConfig> m_singletonConfig;

	long m_nChunkPosWindowA[6];//A窗口的斗的绝对位置
	long m_nChunkPosWindowB[6];//B窗口的斗的绝对位置
	long m_nChunkPosWindowC[6];//C窗口的斗的绝对位置
	long m_nChunkPosWindowD[6];//D窗口的斗的绝对位置
};



#endif	/*!> end of AFX_FX_CPU_MAN_LK_H___5FCD036F_4B5F_4239_94DD_B5EC2177A928__INCLUDED_ */