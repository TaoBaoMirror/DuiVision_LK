#include "stdafx.h"

#include <process.h>
#include "FxCpuMan.h"
#include "General.h"

LoggerPtr CFxCpuMan::m_rootLoggerPtr = Logger::getLogger("CFxCpuMan");

#define IDLE_JOB_TIME 1			//空闲时差
#define OPEN_CLOSE_DOOR_TIME 1000	//开关门时差
#define RUN_CHUNK_INT_WAIT_TIME 100 //走斗间隔等待时间
#define HANDLE_INT_WAIT_TIME 1		//存取操作间隔等待时间

typedef enum tagAckType
{
	IDLE_JOB = 0x00,				//空闲时，处理-----监控伺服、安全光幕异常？

	FRONT_DOOR_OPEN_ACK,			//前门打开
	FRONT_DOOR_CLOSE_ACK,			//前门关闭
		
	BACK_DOOR_OPEN_ACK,				//后门打开
	BACK_DOOR_CLOSE_ACK,			//后门关闭
		
	A_WINDOW_RUN_CHUNK_DONE_ACK,	//A窗走斗---纯粹
	B_WINDOW_RUN_CHUNK_DONE_ACK,	//B窗走斗---纯粹
	C_WINDOW_RUN_CHUNK_DONE_ACK,	//C窗走斗---纯粹
	D_WINDOW_RUN_CHUNK_DONE_ACK,	//D窗走斗---纯粹

	A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK,	//A窗走斗---存取操作
	B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK,	//B窗走斗---存取操作
	C_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK,	//C窗走斗---存取操作
	D_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK,	//D窗走斗---存取操作
		
	A_WINDOW_HANDLE_DONE_ACK,		//A窗存取操作完成
	B_WINDOW_HANDLE_DONE_ACK,		//B窗存取操作完成
	C_WINDOW_HANDLE_DONE_ACK,		//C窗存取操作完成
	D_WINDOW_HANDLE_DONE_ACK		//D窗存取操作完成
		
}EnAckType;


unsigned int __stdcall ThreadFunForMonitorPLC(void * lp)
{
	CFxCpuMan* pFxCpuMan = (CFxCpuMan*)lp;
	
	CActFXCPU3 *pFxCom = pFxCpuMan->GetActFxCpu();
	long lFlag = 0;

	long lRes;

	//实时监测 -------- 伺服是否异常？安全光幕异常？
	while (true)
	{
		Sleep(IDLE_JOB_TIME);

		//监控 前门 安全光幕 异常告警 ？
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M55", &lFlag);	
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}

		if (1 == lFlag)
		{
			
			//操作异常报警
			//通知UI界面，显示操作失败信息		
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_FRONT_MSG, 
				FRONT_SAFE_RASTER_ABNORMAL, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				FRONT_SAFE_RASTER_ABNORMAL, 0);
			//前门 安全光幕异常
			pFxCpuMan->SetSafeSensorAbnormFront();
			
			
		}
		else
		{
			//前门 安全光幕正常，通知UI界面			
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_FRONT_MSG, 
				FRONT_SAFE_RASTER_NORMAL, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				FRONT_SAFE_RASTER_NORMAL, 0);

			//前门 安全光幕正常
			pFxCpuMan->SetSafeSensorAbnormFront(false);			

		}

		//监控 后门 安全光幕 异常告警 ？
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M56", &lFlag);
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}

		if (1 == lFlag)
		{
			//操作异常报警
			//通知UI界面，显示操作失败信息			
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_BACK_MSG, 
				BACK_SAFE_RASTER_ABNORMAL, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				BACK_SAFE_RASTER_ABNORMAL, 0);
			//前门 安全光幕异常
			pFxCpuMan->SetSafeSensorAbnormBack();
			
			//continue;
		}
		else
		{
			//后门 安全光幕正常，通知UI界面			
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_BACK_MSG, 
				BACK_SAFE_RASTER_NORMAL, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				BACK_SAFE_RASTER_NORMAL, 0);

			//后门 安全光幕正常
			pFxCpuMan->SetSafeSensorAbnormBack(false);
		}


		//左伺服 是否 异常告警？
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M51", &lFlag);
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}

		if (1 == lFlag)
		{
			//操作异常报警
			//通知UI界面，显示操作失败信息
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				LEFT_SERVO_ABNORMAL, 0);
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SERVO_A_MSG, 
				LEFT_SERVO_ABNORMAL, 0);
			//PLC异常
			pFxCpuMan->SetErrorLeftServo();

		}
		else
		{
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				LEFT_SERVO_NORMAL, 0);
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SERVO_A_MSG, 
				LEFT_SERVO_NORMAL, 0);
			//PLC正常
			pFxCpuMan->SetErrorLeftServo(false);

		}
		
	
		//右伺服 是否 异常告警？
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M52", &lFlag);
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}
		if (1 == lFlag)
		{
			//操作异常报警
			//通知UI界面，显示操作失败信息
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				RIGHT_SERVO_ABNORMAL, 0);
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SERVO_B_MSG, 
				RIGHT_SERVO_ABNORMAL, 0);
			//PLC异常
			pFxCpuMan->SetErrorRightServo();
			
			//continue;
		}
		else
		{			
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				RIGHT_SERVO_NORMAL, 0);
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SERVO_B_MSG, 
				RIGHT_SERVO_NORMAL, 0);
			//PLC正常
			pFxCpuMan->SetErrorRightServo(false);
		}
		//判断急停按钮是否按下
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M90", &lFlag);
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}
		if (1 == lFlag)
		{
			pFxCpuMan->m_rootLoggerPtr->error("急停按钮已按下");
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_BACK_MSG, 
				SCRAM_BUTTON_DOWN, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				SCRAM_BUTTON_DOWN, 0);
		}
		else
		{
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_BACK_MSG, 
				SCRAM_BUTTON_UP, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				SCRAM_BUTTON_UP, 0);
		}
	}

	return 0;
}


unsigned int __stdcall ThreadFunForFxCpuAckServoA(void * lp)
{
	CFxCpuMan* pFxCpuMan = (CFxCpuMan*)lp;

	CActFXCPU3 *pFxCom = pFxCpuMan->GetActFxCpu();
	long lRes;
	long lFlag;
	long lDoorFlag;

	while (true)
	{
		switch(pFxCpuMan->GetAckTypeServoA())
		{
		case IDLE_JOB://正常空闲时
			{
				Sleep(IDLE_JOB_TIME);				
				continue;
			}
			break;
		case FRONT_DOOR_OPEN_ACK://打开前门
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//前门打开是否完成？
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M24", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				if (1 != lFlag)
				{
					Sleep(20);

					//前门打开异常
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M53", &lDoorFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					if(1 == lDoorFlag)
					{
						//操作异常报警
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							FRONT_DOOR_OPEN_FALSE, 0);
						
						//PLC异常
						pFxCpuMan->SetErrorPLC();
						//异常了，也要恢复到IDLE_JOB状态，PLC已经告诉了操作异常了，不必重新等待操作结果
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					}
					else
					{
						continue;
					}										
					
				}
				else
				{
					//正常;
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						FRONT_DOOR_OPEN_TRUE, 0);
				}
			}
			break;
		case FRONT_DOOR_CLOSE_ACK://关闭前门
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//前门关闭是否完成？
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M25", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				if (1 != lFlag)
				{
					Sleep(20);

					//前门关闭异常
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M54", &lDoorFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					if (1 == lDoorFlag)
					{
						//操作异常报警
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							FRONT_DOOR_CLOSE_FALSE, 0);
						
						//PLC异常
						pFxCpuMan->SetErrorPLC();
						//异常了，也要恢复到IDLE_JOB状态，PLC已经告诉了操作异常了，不必重新等待操作结果
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					} 
					else
					{
						continue;
					}
										
				}
				else
				{
					//正常;
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);

					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						FRONT_DOOR_CLOSE_TRUE, 0);
				}
			}
			break;
		case BACK_DOOR_OPEN_ACK://打开后门
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//后门打开是否完成？
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M26", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				if (1 != lFlag)
				{
					Sleep(20);

					//后门打开异常
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M55", &lDoorFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					if (1 == lDoorFlag)
					{
						//操作异常报警
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							BACK_DOOR_OPEN_FALSE, 0);
						
						//PLC异常
						pFxCpuMan->SetErrorPLC();
						//异常了，也要恢复到IDLE_JOB状态，PLC已经告诉了操作异常了，不必重新等待操作结果
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					} 
					else
					{
						continue;
					}	
					
				}
				else
				{
					//正常;
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						BACK_DOOR_OPEN_TRUE, 0);
				}
			}
			break;
		case BACK_DOOR_CLOSE_ACK://关闭后门
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//后门关闭是否完成？
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M27", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				if (1 != lFlag)
				{
					Sleep(20);
					
					//后门关闭异常
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M56", &lDoorFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					if (1 == lDoorFlag)
					{
						//操作异常报警
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							BACK_DOOR_CLOSE_FALSE, 0);
						
						//PLC异常
						pFxCpuMan->SetErrorPLC();
						//异常了，也要恢复到IDLE_JOB状态，PLC已经告诉了操作异常了，不必重新等待操作结果
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					} 
					else
					{
						continue;
					}					
					
				}
				else
				{
					//正常;
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						BACK_DOOR_CLOSE_TRUE, 0);
				}
			}
			break;
		case A_WINDOW_RUN_CHUNK_DONE_ACK://A窗走斗---纯粹
			{							
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
			//	Sleep(2000);
				//A窗走斗是否完成
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M4", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}				
				//是否“A窗走斗完成”
				if (1 != lFlag)//暂时尚未完成
				{
					//如果左伺服异常
					if (pFxCpuMan->GetErrorLeftServo())
					{
						//伺服异常了，则不死等了，以免僵死运行在一个状态机中
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_RUN_CHUNK_DONE_FALSE, 0);

						
					}
					else//无异常，则继续等待，也许是短暂的光幕异常造成的，待会就可以了。
					{
						continue;
					}
					
				}
				else
				{
					//正常;
					//跳至到----空闲状态
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);	
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_A_RUN_CHUNK_DONE_TRUE, 0);
					
									
				}
			}
			break;
		case C_WINDOW_RUN_CHUNK_DONE_ACK://C窗走斗---纯粹
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//C窗走斗是否完成
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M4", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//是否“C窗走斗完成”
				if (1 != lFlag)
				{					
					//如果左伺服异常
					if (pFxCpuMan->GetErrorLeftServo())
					{
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_RUN_CHUNK_DONE_FALSE, 0);
						
						//伺服异常了，则不死等了，以免僵死运行在一个状态机中
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					}
					else//无异常，则继续等待，也许是短暂的光幕异常造成的，待会就可以了。
					{
						continue;
					}
				}
				else
				{

					//正常;
					//跳至到----空闲状态
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_A_RUN_CHUNK_DONE_TRUE, 0);
					
										
				}
			}
			break;
		case A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK://A窗走斗--和--存取操作
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//A窗走斗是否完成
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M4", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//是否“A窗走斗完成”
				if (1 != lFlag)
				{
					//如果左伺服异常
					if (pFxCpuMan->GetErrorLeftServo())
					{
						//伺服异常了，则不死等了，以免僵死运行在一个状态机中
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, 0);
						
						
					}
					else//无异常，则继续等待，也许是短暂的光幕异常造成的，待会就可以了。
					{
						continue;
					}
				}
				else
				{
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_TRUE, 0);

					//正常;
					//跳至到----A窗存取操作
					pFxCpuMan->SetAckTypeServoA(A_WINDOW_HANDLE_DONE_ACK);					
				}
			}
			break;
		case C_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK://C窗--走斗--和--存取操作
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//C窗走斗是否完成
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M4", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//是否“C窗走斗完成”
				if (1 != lFlag)
				{					
					//如果左伺服异常
					if (pFxCpuMan->GetErrorLeftServo())
					{

						//伺服异常了，则不死等了，以免僵死运行在一个状态机中
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, 0);
						
						
					}
					else//无异常，则继续等待，也许是短暂的光幕异常造成的，待会就可以了。
					{
						continue;
					}
				}
				else
				{
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_TRUE, 0);
					
					//正常;
					//跳至到----C窗存取操作
					pFxCpuMan->SetAckTypeServoA(C_WINDOW_HANDLE_DONE_ACK);					
				}
			}
			break;
		case A_WINDOW_HANDLE_DONE_ACK://A窗存取操作
			{
				//尚未软操作确认“存取操作完成”
				if (!pFxCpuMan->GetSoftSureServoA())
				{
					Sleep(HANDLE_INT_WAIT_TIME);
					
					//“A窗存取操作”完成 的硬操作是否完成？
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M20", &lFlag);
					//lRes = pFxCom->GetDevice("M12", &lFlag);		
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					//尚未硬操作确认“存取操作完成”
					if (1 != lFlag)
					{
						///继续下一轮，查看是否该操作是否完成？
						continue;
					}
					else//硬操作确认“存取操作完成”
					{
						pFxCpuMan->m_rootLoggerPtr->error("A窗左侧硬件确认按钮已按下");
						
						//下一轮，进入“空闲处理”
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
											
						//软“存取操作完成”复位到----“尚未软确认‘存取操作完成’”
						pFxCpuMan->SetSoftSureServoA(false);
						//通知UI界面，显示操作成功信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_HANDLE_HAD_DONE, 0);
					}
				}
				else//软操作，已经“存取操作完成”
				{
					pFxCpuMan->m_rootLoggerPtr->error("A窗左侧软件确认按钮已按下");
					//下一轮，进入“空闲处理”
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);

					//软“取药完成”复位到----“尚未软确认‘存取操作完成’”
					pFxCpuMan->SetSoftSureServoA(false);

					//软操作确认，是由UI界面确认的，所以不需要发送消息了；
				}		
			}
			break;
		case C_WINDOW_HANDLE_DONE_ACK://C窗存取操作
			{
				//尚未软操作确认“存取操作完成”
				if (!pFxCpuMan->GetSoftSureServoA())
				{
					Sleep(HANDLE_INT_WAIT_TIME);
					
					//“C窗存取操作”完成 的硬操作是否完成？
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("X22", &lFlag);
					//lRes = pFxCom->GetDevice("M14", &lFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}	
					//尚未硬操作确认“存取操作完成”
					if (1 != lFlag)
					{
						//继续下一轮，查看是否该操作是否完成？
						continue;
					}
					else//硬操作确认“存取操作完成”
					{
						
						pFxCpuMan->m_rootLoggerPtr->error("C窗左侧确认按钮已按下");
						//下一轮，进入“空闲处理”
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
						
						//软“存取操作完成”复位到----“尚未软确认‘存取操作完成’”
						pFxCpuMan->SetSoftSureServoA(false);
						//通知UI界面，显示操作成功信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_HANDLE_HAD_DONE, 0);
					}
					
				}
				else//软操作，已经“存取操作完成”
				{
					//下一轮，进入“空闲处理”
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					
					//软“存取操作完成”复位到----“尚未软确认‘存取操作完成’”
					pFxCpuMan->SetSoftSureServoA(false);
					
					//软操作确认，是由UI界面确认的，所以不需要发送消息了；
				}
			}
			break;
		default:
			break;
		}
	}

	return 0;
}


unsigned int __stdcall ThreadFunForFxCpuAckServoB(void * lp)
{
	CFxCpuMan* pFxCpuMan = (CFxCpuMan*)lp;

	CActFXCPU3 *pFxCom = pFxCpuMan->GetActFxCpu();
	long lRes;
	long lFlag;


	while (true)
	{
		switch(pFxCpuMan->GetAckTypeServoB())
		{
		case IDLE_JOB://正常空闲时
			{
				Sleep(IDLE_JOB_TIME);
				
				continue;
			}
			break;
		case B_WINDOW_RUN_CHUNK_DONE_ACK://B窗走斗---纯粹
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//B窗走斗是否完成
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M5", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//是否“B窗走斗完成”
				if (1 != lFlag)
				{					
					//如果右伺服异常
					if (pFxCpuMan->GetErrorRightServo())
					{

						//伺服异常了，则不死等了，以免僵死运行在一个状态机中
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_RUN_CHUNK_DONE_FALSE, 0);
						
						
					}
					else//无异常，则继续等待，也许是短暂的光幕异常造成的，待会就可以了。
					{
						continue;
					}
				}
				else
				{

					//正常;
					//跳至到----空闲状态
					pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_B_RUN_CHUNK_DONE_TRUE, 0);
					
										
				}
			}
			break;
		case D_WINDOW_RUN_CHUNK_DONE_ACK://D窗走斗---纯粹
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//D窗走斗是否完成
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M5", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//是否“D窗走斗完成”
				if (1 != lFlag)
				{					
					//如果右伺服异常
					if (pFxCpuMan->GetErrorRightServo())
					{

						//伺服异常了，则不死等了，以免僵死运行在一个状态机中
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_RUN_CHUNK_DONE_FALSE, 0);
						
						
					}
					else//无异常，则继续等待，也许是短暂的光幕异常造成的，待会就可以了。
					{
						continue;
					}
				}
				else
				{

					//正常;
					//跳至到----空闲状态
					pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_B_RUN_CHUNK_DONE_TRUE, 0);
				
										
				}
			}
			break;
		case B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK://B窗--走斗--和--存取操作
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//B窗走斗是否完成
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M5", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//是否“B窗走斗完成”
				if (1 != lFlag)
				{
					//如果右伺服异常
					if (pFxCpuMan->GetErrorRightServo())
					{

						//伺服异常了，则不死等了，以免僵死运行在一个状态机中
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, 0);
						
						
					}
					else//无异常，则继续等待，也许是短暂的光幕异常造成的，待会就可以了。
					{
						continue;
					}
				}
				else
				{
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_TRUE, 0);
					
					//正常;
					//跳至到----B窗存取操作
					pFxCpuMan->SetAckTypeServoB(B_WINDOW_HANDLE_DONE_ACK);					
				}
			}
			break;
		case D_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK://D窗--走斗--和--存取操作
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//D窗走斗是否完成
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M5", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//是否“D窗走斗完成”
				if (1 != lFlag)
				{					
					//如果右伺服异常
					if (pFxCpuMan->GetErrorRightServo())
					{
						//通知UI界面，显示操作失败信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, 0);
						
						//伺服异常了，则不死等了，以免僵死运行在一个状态机中
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					}
					else//无异常，则继续等待，也许是短暂的光幕异常造成的，待会就可以了。
					{
						continue;
					}
				}
				else
				{
					//通知UI界面，显示操作成功信息
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_TRUE, 0);
					
					//正常;
					//跳至到----D窗后右存取操作
					pFxCpuMan->SetAckTypeServoB(D_WINDOW_HANDLE_DONE_ACK);					
				}
			}
			break;
		case B_WINDOW_HANDLE_DONE_ACK://B窗存取操作
			{
				//尚未软操作确认“存取操作完成”
				if (!pFxCpuMan->GetSoftSureServoB())
				{
					Sleep(HANDLE_INT_WAIT_TIME);
					
					//“B窗存取操作”完成 的硬操作是否完成？
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M21", &lFlag);
					pFxCpuMan->LeaveCS();
					//lRes = pFxCom->GetDevice("M13", &lFlag);					
					if (lRes)
					{
						continue;
					}
					//尚未硬操作确认“存取操作完成”
					if (1 != lFlag)
					{
						//继续下一轮，查看是否该操作是否完成？
						continue;
					}
					else//硬操作确认“存取操作完成”
					{
						
						pFxCpuMan->m_rootLoggerPtr->error("B窗右侧确认按钮已按下");
						//下一轮，进入“空闲处理”
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						
						//软“存取操作完成”复位到----“尚未软确认‘存取操作完成’”
						pFxCpuMan->SetSoftSureServoB(false);

						//通知UI界面，显示操作成功信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_HANDLE_HAD_DONE, 0);
					}					
				}
				else//软操作，已经“存取操作完成”
				{
					//下一轮，进入“空闲处理”
					pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					
					//软“存取操作完成”复位到----“尚未软确认‘存取操作完成’”
					pFxCpuMan->SetSoftSureServoB(false);
					
					//软操作确认，是由UI界面确认的，所以不需要发送消息了；
				}
			}
			break;
		case D_WINDOW_HANDLE_DONE_ACK://D窗存取操作
			{
				//尚未软操作确认“存取操作完成”
				if (!pFxCpuMan->GetSoftSureServoB())
				{
					Sleep(HANDLE_INT_WAIT_TIME);
					
					//“D窗存取操作”完成 的硬操作是否完成？
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("X23", &lFlag);
					//lRes = pFxCom->GetDevice("M15", &lFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					//尚未硬操作确认“存取操作完成”
					if (1 != lFlag)
					{
						//继续下一轮，查看是否该操作是否完成？
						continue;
					}
					else//硬操作确认“存取操作完成”
					{
						
						pFxCpuMan->m_rootLoggerPtr->error("D窗右侧确认按钮已按下");
						//下一轮，进入“空闲处理”
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						
						//软“存取操作完成”复位到----“尚未软确认‘存取操作完成’”
						pFxCpuMan->SetSoftSureServoB(false);

						//通知UI界面，显示操作成功信息
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_HANDLE_HAD_DONE, 0);
					}
					
				}
				else//软操作，已经“存取操作完成”
				{
					//下一轮，进入“空闲处理”
					pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					
					//软“存取操作完成”复位到----“尚未软确认‘存取操作完成’”
					pFxCpuMan->SetSoftSureServoB(false);
					
					//软操作确认，是由UI界面确认的，所以不需要发送消息了；
				}
			}
			break;
		default:

			break;
		}
	}

	return 0;
}


CFxCpuMan::CFxCpuMan()
{

	m_pSysConfig = m_singletonConfig.getInstance();

	//读取配置信息
	m_nTierNum = configPar.getIntParameter("TierNum");
	m_nTierCapacity = configPar.getIntParameter("TierCapacity");
	m_nPortNum = configPar.getIntParameter("PlcPort");
	m_bDoubleFace = ((1 == configPar.getIntParameter("DoubleFace"))?true:false);
	m_nRunMode = configPar.getIntParameter("RunMode");


	/*!> 获取绝对位置------A window A窗口 */
	m_nChunkPosWindowA[0] = configPar.getIntParameter("A0");
	m_nChunkPosWindowA[1] = configPar.getIntParameter("A1");
	m_nChunkPosWindowA[2] = configPar.getIntParameter("A2");
	m_nChunkPosWindowA[3] = configPar.getIntParameter("A3");
	m_nChunkPosWindowA[4] = configPar.getIntParameter("A4");
	m_nChunkPosWindowA[5] = configPar.getIntParameter("A5");

	/*!> 获取绝对位置------B window B窗口 */
	m_nChunkPosWindowB[0] = configPar.getIntParameter("B0");
	m_nChunkPosWindowB[1] = configPar.getIntParameter("B1");
	m_nChunkPosWindowB[2] = configPar.getIntParameter("B2");
	m_nChunkPosWindowB[3] = configPar.getIntParameter("B3");
	m_nChunkPosWindowB[4] = configPar.getIntParameter("B4");
	m_nChunkPosWindowB[5] = configPar.getIntParameter("B5");

	/*!> 获取绝对位置------C window C窗口 */
	m_nChunkPosWindowC[0] = configPar.getIntParameter("C0");
	m_nChunkPosWindowC[1] = configPar.getIntParameter("C1");
	m_nChunkPosWindowC[2] = configPar.getIntParameter("C2");
	m_nChunkPosWindowC[3] = configPar.getIntParameter("C3");
	m_nChunkPosWindowC[4] = configPar.getIntParameter("C4");
	m_nChunkPosWindowC[5] = configPar.getIntParameter("C5");

	/*!> 获取绝对位置------D window D窗口 */
	m_nChunkPosWindowD[0] = configPar.getIntParameter("D0");
	m_nChunkPosWindowD[1] = configPar.getIntParameter("D1");
	m_nChunkPosWindowD[2] = configPar.getIntParameter("D2");
	m_nChunkPosWindowD[3] = configPar.getIntParameter("D3");
	m_nChunkPosWindowD[4] = configPar.getIntParameter("D4");
	m_nChunkPosWindowD[5] = configPar.getIntParameter("D5");


	m_bIsOpen = false;
	m_bSoftSureServoA = false;
	m_bSoftSureServoB = false;
	m_bErrorPLC = false;
	m_bErrorLeftServo = false;
	m_bErrorRightServo = false;

	m_nSumSlot = m_nTierNum * m_nTierCapacity * 2;

	int m_nAckType = IDLE_JOB;

	InitializeCriticalSection(&m_cs);
}


CFxCpuMan::~CFxCpuMan()
{
	

	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return ;
	}

	//关闭
	if (m_bIsOpen)
	{
		m_pFxCom->Close();	
	}
	DeleteCriticalSection(&m_cs);
}

	long CFxCpuMan::InitOpen(CActFXCPU3 *pFxCom)
	{
		long lRes;

		if (1 == m_nRunMode || 9 == m_nRunMode)
		{
			return 0;
		}

		if (!m_bIsOpen)
		{
			if (NULL == pFxCom)
			{
				return ERROR_PLC_PARAM;
			}
			
			m_pFxCom = pFxCom;

			m_pFxCom->SetActCpuType(520);
			
			//设置端口号
			m_pFxCom->SetActPortNumber(m_nPortNum);
			//设置超时
			m_pFxCom->SetActTimeOut(200);
			
			//打开
			lRes = m_pFxCom->Open();
			if (lRes)
			{
				return ERROR_PLC_OPEN_PORT;
			}
			
			Sleep(50);

//	#if (!PLC_DEBUG_MODE)
			//启动实时监控PLC线程
			HANDLE hThreadPLC = (HANDLE)_beginthreadex(NULL, 0, ThreadFunForMonitorPLC, this, 0, 0);
			
			//启动PLC操作回馈线程--伺服A
			HANDLE hThreadAckServoA = (HANDLE)_beginthreadex(NULL, 0, ThreadFunForFxCpuAckServoA, this, 0, 0);

			//启动PLC操作回馈线程--伺服B
			HANDLE hThreadAckServoB = (HANDLE)_beginthreadex(NULL, 0, ThreadFunForFxCpuAckServoB, this, 0, 0);
			m_rootLoggerPtr->error("启动PLC 监控线程");
			//	#endif
			m_bIsOpen = true;
		}

		return 0;
	}

//  此处需要修改
long CFxCpuMan::OpenDoor(int nOperType, bool bBack)
{
	long lRes;

	switch (nOperType)
	{
	case PRE_FETCH_OPER://处方取药，不管是否有前后，只开前门	
		{
			lRes = OpenFrontDoor();

			//发送“打开前门”命令成功
			if (!lRes)
			{
				//进入到等待“打开前门完成”状态机
				SetAckTypeServoA(FRONT_DOOR_OPEN_ACK);
			}
		}
		break;
	case ADJUST_OPER:	//校正，根据实参bBack来判断开前门，还是开后门？
		{
			if (bBack)//后门
			{
				lRes = OpenBackDoor();//开后门
				//发送“打开后门”命令成功
				if (!lRes)
				{
					//进入到等待“打开后门完成”状态机
					SetAckTypeServoA(BACK_DOOR_OPEN_ACK);
				}
			}
			else//否则，前门
			{
				lRes = OpenFrontDoor();//开前门
				
				//发送“打开前门”命令成功
				if (!lRes)
				{
					//进入到等待“打开前门完成”状态机
					SetAckTypeServoA(FRONT_DOOR_OPEN_ACK);
				}
			}
		}
		break;
	case BATCH_ADD_OPER://批量加药   为啥没有操作开门
	case MED_MAN_OPER:	//药品管理
		{
			if (m_bDoubleFace)//有前后门
			{
				lRes = OpenBackDoor();//开后门
				//发送“打开后门”命令成功
				if (!lRes)
				{
					//进入到等待“打开后门完成”状态机
					SetAckTypeServoA(BACK_DOOR_OPEN_ACK);
				}
			}
			else//只有前门
			{
				lRes = OpenFrontDoor();//开前门

				//发送“打开前门”命令成功
				if (!lRes)
				{
					//进入到等待“打开前门完成”状态机
					SetAckTypeServoA(FRONT_DOOR_OPEN_ACK);
				}
			}
		}		
		break;
	default:
		break;
	}

	return lRes;
}

//
long CFxCpuMan::CloseDoor(int nOperType, bool bBack)
{
	long lRes;

	switch (nOperType)
	{
	case PRE_FETCH_OPER://处方取药，不管是否有前后，只关前门
		{
			lRes = CloseFrontDoor();
			//发送“关闭前门”命令成功
			if (!lRes)
			{
				//进入到等待“关闭前门完成”状态机
				SetAckTypeServoA(FRONT_DOOR_CLOSE_ACK);
			}
		}		
		break;
	case ADJUST_OPER:	//校正，根据实参来判断前门，还是后门？
		{
			if (bBack)//后门
			{
				lRes = CloseBackDoor();//关后门
				//发送“关闭后门”命令成功
				if (!lRes)
				{
					//进入到等待“关闭后门完成”状态机
					SetAckTypeServoA(BACK_DOOR_CLOSE_ACK);
				}
			}
			else//前门
			{
				lRes = CloseFrontDoor();//关前门
				//发送“关闭前门”命令成功
				if (!lRes)
				{
					//进入到等待“关闭前门完成”状态机
					SetAckTypeServoA(FRONT_DOOR_CLOSE_ACK);
				}
			}
		}
		break;
	case BATCH_ADD_OPER://批量加药
	case MED_MAN_OPER:	//药品管理
		{
			if (m_bDoubleFace)//有前后门
			{
				lRes = CloseBackDoor();//关后门
				//发送“关闭后门”命令成功
				if (!lRes)
				{
					//进入到等待“关闭后门完成”状态机
					SetAckTypeServoA(BACK_DOOR_CLOSE_ACK);
				}
			}
			else//只有前门
			{
				lRes = CloseFrontDoor();//关前门
				//发送“关闭前门”命令成功
				if (!lRes)
				{
					//进入到等待“关闭前门完成”状态机
					SetAckTypeServoA(FRONT_DOOR_CLOSE_ACK);
				}
			}
		}		
		break;
	default:
		break;
	}
	return lRes;
}

//开前门
long CFxCpuMan::OpenFrontDoor()
{
	long lRes;

	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	EnterCS();
	lRes = m_pFxCom->SetDevice("M8", 1);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_OPEN_FRONT_DOOR;
	}
	return 0;
}

//关前门
long CFxCpuMan::CloseFrontDoor()
{
	long lRes;

	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	EnterCS();
	lRes = 	m_pFxCom->SetDevice("M9", 1);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_CLOSE_FRONT_DOOR;
	}
	return 0;
}


//开后门
long CFxCpuMan::OpenBackDoor()
{
	long lRes;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	EnterCS();
	lRes = 	m_pFxCom->SetDevice("M18", 1);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_OPEN_BACK_DOOR;
	}
	return 0;
}


//关后门
long CFxCpuMan::CloseBackDoor()
{
	long lRes;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	EnterCS();
	lRes = 	m_pFxCom->SetDevice("M19", 1);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_CLOSE_BACK_DOOR;
	}
	return 0;
}



//A窗口走斗
long CFxCpuMan::RunChunkWindowA(long nChunkNum)
{
	long lRes;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}


//#if 1
//	long lLow;
//	long lHigh;

//	lLow = m_nChunkPosWindowA[nChunkNum] & 0x0000ffff;
	
//	lHigh = (m_nChunkPosWindowA[nChunkNum] >> 16);

	EnterCS();
//	lRes = m_pFxCom->WriteDeviceBlock("D0",1, &lLow);
	lRes=m_pFxCom->SetDevice("D0",nChunkNum);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_TIER_NUM;
	}

// 	Sleep(2);
// 	EnterCS();
// 	lRes = m_pFxCom->WriteDeviceBlock("D1",1, &lHigh);
// 	LeaveCS();
// 	if (lRes)
// 	{
// 		//
// 		return ERROR_PLC_SEND_TIER_NUM;
// 	}
	
//#else

	/* 
		批量写入软元件 
		参数1：软元件名称
		参数2：写入点数
		参数3：要写入的软元件值
	*/
// 	EnterCS();
// 	lRes = m_pFxCom->WriteDeviceBlock("D0",1, &m_nChunkPosWindowA[nChunkNum]);
// 	LeaveCS();
// 	if (lRes)
// 	{
// 		//
// 		return ERROR_PLC_SEND_TIER_NUM;
// 	}

//#endif

// 	Sleep(10);
// 
// 	/*
// 		设置软元件
// 		参数1：软元件名称
// 		参数2：设置数据
// 	*/
// 	EnterCS();
// 	lRes = m_pFxCom->SetDevice("M0", 1);
// 	LeaveCS();
// 	if (lRes)
// 	{
// 		//
// 		return ERROR_PLC_SEND_RUN_TIER;
// 	}

	return 0;
}

//B窗口走斗
long CFxCpuMan::RunChunkWindowB(long nChunkNum)
{
	long lRes ;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	
// #if 1
// 
// 	long lLow;
// 	long lHigh;
// 	
// 	lLow = m_nChunkPosWindowB[nChunkNum] & 0x0000ffff;
// 	
// 	lHigh = (m_nChunkPosWindowB[nChunkNum] >> 16);
	
	EnterCS();
//	lRes = m_pFxCom->WriteDeviceBlock("D2",1, &lLow);
	lRes=m_pFxCom->SetDevice("D2",nChunkNum);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_TIER_NUM;
	}
	
// 	Sleep(2);
// 	
// 	EnterCS();
// 	lRes = m_pFxCom->WriteDeviceBlock("D3",1, &lHigh);
// 	LeaveCS();
// 	if (lRes)
// 	{
// 		//
// 		return ERROR_PLC_SEND_TIER_NUM;
// 	}
// 	
// #else
// 	EnterCS();
// 	lRes = m_pFxCom->WriteDeviceBlock("D2",1, &m_nChunkPosWindowB[nChunkNum]);
// 	LeaveCS();
// 	if (lRes)
// 	{
// 		//
// 		return ERROR_PLC_SEND_TIER_NUM;
// 	}
// 
// #endif
// 
// 	Sleep(10);
// 
// 	EnterCS();
// 	lRes = m_pFxCom->SetDevice("M1", 1);
// 	LeaveCS();
// 	if (lRes)
// 	{
// 		//
// 		return ERROR_PLC_SEND_RUN_TIER;
// 	}
	
	return 0;
}

//C窗口走斗
long CFxCpuMan::RunChunkWindowC(long nChunkNum)
{
	long lRes ;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
		
#if 1
	long lLow;
	long lHigh;

	lLow = m_nChunkPosWindowC[nChunkNum] & 0x0000ffff;
	
	lHigh = (m_nChunkPosWindowC[nChunkNum] >> 16);

	EnterCS();
	lRes = m_pFxCom->WriteDeviceBlock("D0",1, &lLow);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_TIER_NUM;
	}

	Sleep(2);

	EnterCS();
	lRes = m_pFxCom->WriteDeviceBlock("D1",1, &lHigh);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_TIER_NUM;
	}
	
#else

	/* 
		批量写入软元件 
		参数1：软元件名称
		参数2：写入点数
		参数3：要写入的软元件值
	*/
	EnterCS();
	lRes = m_pFxCom->WriteDeviceBlock("D0",1, &m_nChunkPosWindowC[nChunkNum]);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_TIER_NUM;
	}

#endif

	Sleep(10);

	/*
		设置软元件
		参数1：软元件名称
		参数2：设置数据
	*/
	EnterCS();
	lRes = m_pFxCom->SetDevice("M0", 1);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_RUN_TIER;
	}

	return 0;
}

//D窗口走斗
long CFxCpuMan::RunChunkWindowD(long nChunkNum)
{
	long lRes ;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	
#if 1
	long lLow;
	long lHigh;
	
	lLow = m_nChunkPosWindowD[nChunkNum] & 0x0000ffff;
	
	lHigh = (m_nChunkPosWindowD[nChunkNum] >> 16);
	
	EnterCS();
	lRes = m_pFxCom->WriteDeviceBlock("D2",1, &lLow);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_TIER_NUM;
	}
	
	Sleep(2);
	
	EnterCS();
	lRes = m_pFxCom->WriteDeviceBlock("D3",1, &lHigh);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_TIER_NUM;
	}
	
#else
	
	EnterCS();
	lRes = m_pFxCom->WriteDeviceBlock("D2",1, &m_nChunkPosWindowD[nChunkNum]);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_TIER_NUM;
	}
#endif
		
	Sleep(10);
	
	EnterCS();
	lRes = m_pFxCom->SetDevice("M1", 1);
	LeaveCS();
	if (lRes)
	{
		//
		return ERROR_PLC_SEND_RUN_TIER;
	}
	
	return 0;
}

//nDirect返回：左右柜
//nChunkNum返回：斗数
/*--------------** 左柜，右柜 **-----------------*/
void CFxCpuMan::Convert2ChunkNum(const int nLocID, int &nDirect, int &nChunkNum)
{
//	int nSingleSum;
	
	//单个伺服的药槽个数;(24层，每层5个槽)
//	nSingleSum = m_nTierNum * m_nTierCapacity;

	if (nLocID <= 120)//A窗口
	{
		/*
			每个柜，有12个斗，每个斗有2层，即每个柜有12 x 2 = 24 层
			每层有5个槽，即每个柜有：24 x 5 = 120 个槽
			每次窗口可视2个斗，即4层，
			24层，窗口可视遍历一次，需要 24/4 = 6 节（0-5节）
		*/
		nChunkNum = (nLocID-1) / m_nTierCapacity / 4;
		nDirect = LEFT_CHEST;//是左？右？伺服机(LOC_ID,数据库存储的是从1开始的)
	}
	else if (nLocID <= 240)//B窗口
	{
		nChunkNum = (nLocID - 120-1) / m_nTierCapacity / 4;
		nDirect = RIGHT_CHEST;
	}

	return;
}


//走斗----纯粹走斗
long CFxCpuMan::RunChunk(int nLocID, int nOperType, bool bBack)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	int nDirect, nChunkNum;
	long lRes;
	
	//查看是否越界？
	if (1 > nLocID || m_nSumSlot < nLocID)
	{
		return ERROR_PLC_LOC_ID_BOUND;
	}

	Convert2ChunkNum(nLocID, nDirect, nChunkNum);

	switch (nOperType)
	{
	case PRE_FETCH_OPER:	//只能开前门
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = RunChunkWindowA(nChunkNum);
				//发送“A窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“A窗口走斗完成”状态机
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//发送“B窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“B窗口走斗完成”状态机
					SetAckTypeServoB(B_WINDOW_RUN_CHUNK_DONE_ACK);
				}
				break;
			default:
				break;
			}
		}
		break;
	case ADJUST_OPER:
		{
			if (bBack)//开后门
			{
				switch(nDirect)//开后门
				{
				case LEFT_CHEST:
					lRes = RunChunkWindowC(nChunkNum);
					//发送“C窗走斗”命令成功
					if (!lRes)
					{
						//进入到等待“C窗走斗完成”状态机
						SetAckTypeServoA(C_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				case RIGHT_CHEST:
					lRes = RunChunkWindowD(nChunkNum);
					//发送“D窗走斗”命令成功
					if (!lRes)
					{
						//进入到等待“D窗走斗完成”状态机
						SetAckTypeServoB(D_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				default:
					break;
				}
			} 
			else//只有一扇门，即前门
			{
				switch(nDirect)
				{
				case LEFT_CHEST:
					lRes = RunChunkWindowA(nChunkNum);
					//发送“A窗走斗”命令成功
					if (!lRes)
					{
						//进入到等待“A窗走斗完成”状态机
						SetAckTypeServoA(A_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				case RIGHT_CHEST:
					lRes = RunChunkWindowB(nChunkNum);
					//发送“B窗走斗”命令成功
					if (!lRes)
					{
						//进入到等待“B窗走斗完成”状态机
						SetAckTypeServoB(B_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	case BATCH_ADD_OPER:
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = RunChunkWindowA(nChunkNum);
				//发送“A窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“A窗口走斗完成”状态机
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//发送“B窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“B窗口走斗完成”状态机
					SetAckTypeServoB(B_WINDOW_RUN_CHUNK_DONE_ACK);
				}
				break;
			default:
				break;
			}
		}
		break;
	case MED_MAN_OPER:
		{
			if (m_bDoubleFace)//有两扇门，有前后门
			{
				switch(nDirect)//开后门
				{
				case LEFT_CHEST:
					lRes = RunChunkWindowC(nChunkNum);
					//发送“C窗走斗”命令成功
					if (!lRes)
					{
						//进入到等待“C窗走斗完成”状态机
						SetAckTypeServoA(C_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				case RIGHT_CHEST:
					lRes = RunChunkWindowD(nChunkNum);
					//发送“D窗走斗”命令成功
					if (!lRes)
					{
						//进入到等待“D窗走斗完成”状态机
						SetAckTypeServoB(D_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				default:
					break;
				}
			} 
			else//只有一扇门，即前门
			{
				switch(nDirect)
				{
				case LEFT_CHEST:
					lRes = RunChunkWindowA(nChunkNum);
					//发送“A窗走斗”命令成功
					if (!lRes)
					{
						//进入到等待“A窗走斗完成”状态机
						SetAckTypeServoA(A_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				case RIGHT_CHEST:
					lRes = RunChunkWindowB(nChunkNum);
					//发送“B窗走斗”命令成功
					if (!lRes)
					{
						//进入到等待“B窗走斗完成”状态机
						SetAckTypeServoB(B_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	default:
		break;
	}

	return lRes;
}

//走斗----和-----存取操作
long CFxCpuMan::RunChunkAndHandle(int nLocID, int nOperType)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	int nDirect, nChunkNum;
	long lRes;
	
	//查看是否越界？
	if (1 > nLocID || m_nSumSlot < nLocID)
	{
		return ERROR_PLC_LOC_ID_BOUND;
	}

	Convert2ChunkNum(nLocID, nDirect, nChunkNum);

	switch (nOperType)
	{
	case PRE_FETCH_OPER:
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = RunChunkWindowA(nChunkNum);
				//发送“A窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“A窗口走斗完成---接下来---存取操作”状态机
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//发送“B窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“B窗口走斗完成---接下来---存取操作”状态机
					SetAckTypeServoB(B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			default:
				break;
			}
		}
		break;
	case ADJUST_OPER:		//只能开前门
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = RunChunkWindowA(nChunkNum);
				//发送“A窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“A窗口走斗完成---接下来---存取操作”状态机
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//发送“B窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“B窗口走斗完成---接下来---存取操作”状态机
					SetAckTypeServoB(B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			default:
				break;
			}
		}
		break;
	case BATCH_ADD_OPER:
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = RunChunkWindowA(nChunkNum);
				//发送“A窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“A窗口走斗完成---接下来---存取操作”状态机
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//发送“B窗口走斗”命令成功
				if (!lRes)
				{
					//进入到等待“B窗口走斗完成---接下来---存取操作”状态机
					SetAckTypeServoB(B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			default:
				break;
			}
		}
		break;
	case MED_MAN_OPER:
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = RunChunkWindowA(nChunkNum);
				//发送“A窗走斗”命令成功
				if (!lRes)
				{
					//进入到等待“A窗走斗完成---接下来---存取操作”状态机
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//发送“B窗走斗”命令成功
				if (!lRes)
				{
					//进入到等待“B窗走斗完成---接下来---存取操作”状态机
					SetAckTypeServoB(B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}

	return lRes;
}


//入口参数，为true，切换到前屏幕，为false，切换后屏幕
long CFxCpuMan::SwitchScreen(bool bFront)
{
	long lRes;
	long lFlag;

	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}


	//只有在双屏时，才有屏幕切换动作
	if (m_bDoubleFace)
	{
		//前门
		if (bFront)
		{
			lFlag = 0;
		}
		else//后门
		{
			lFlag = 1;
		}

		//设置切屏
		EnterCS();
		lRes = m_pFxCom->SetDevice("M10", lFlag);
		LeaveCS();
		if (lRes)
		{
			return ERROR_PLC_SWITCH_SCREEN;

			//屏幕切换失败，不必通知UI界面了，返回值就可以知晓了
			//SendMessage();
		}
	}

	return 0;
}

//蜂鸣器控制，默认缺省值为true--打开，false--关闭
long CFxCpuMan::TurnBuzzer(bool bOn)
{
	long lRes;
	long lFlag;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	//打开
	if (bOn)
	{
		lFlag = 0;
	}
	else//关闭
	{
		lFlag = 1;
	}
		
	//设置蜂鸣器开关
	EnterCS();
	lRes = m_pFxCom->SetDevice("M11", lFlag);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_BUZZER;
			
		//蜂鸣器操作失败，不必通知UI界面了，返回值就可以知晓了
		//SendMessage();
	}
	
	return 0;
}

long CFxCpuMan::SetChunkPosWindowA(int nChunk,int nPos)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	string strPos;
	string strWin;
	long lRes;

	strWin = General::integertostring(nChunk);
	strWin = "A" + strWin;

	strPos = General::integertostring(nPos);

	configPar.updateValueParameter(strWin, strPos);

	lRes =  m_pSysConfig->saveConfig(configPar.m_mapParameter.begin(), configPar.m_mapParameter.end());
	
	//配置文件修改成功，同步更新
	if (!lRes)
	{
		m_nChunkPosWindowA[nChunk] = nPos;
	}

	return lRes;
}

long CFxCpuMan::SetChunkPosWindowB(int nChunk,int nPos)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	string strPos;
	string strWin;
	long lRes;
	
	strWin = General::integertostring(nChunk);
	strWin = "B" + strWin;
	
	strPos = General::integertostring(nPos);
	
	configPar.updateValueParameter(strWin, strPos);
	
	lRes =  m_pSysConfig->saveConfig(configPar.m_mapParameter.begin(), configPar.m_mapParameter.end());

	//配置文件修改成功，同步更新
	if (!lRes)
	{
		m_nChunkPosWindowB[nChunk] = nPos;
	}
	
	return lRes;
}
long CFxCpuMan::SetChunkPosWindowC(int nChunk,int nPos)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	string strPos;
	string strWin;
	long lRes;

	strWin = General::integertostring(nChunk);
	strWin = "C" + strWin;
	
	strPos = General::integertostring(nPos);
	
	configPar.updateValueParameter(strWin, strPos);
	
	lRes =  m_pSysConfig->saveConfig(configPar.m_mapParameter.begin(), configPar.m_mapParameter.end());

	//配置文件修改成功，同步更新
	if (!lRes)
	{
		m_nChunkPosWindowC[nChunk] = nPos;
	}
	
	return lRes;
}

long CFxCpuMan::SetChunkPosWindowD(int nChunk,int nPos)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	string strPos;
	string strWin;
	long lRes;
	
	strWin = General::integertostring(nChunk);
	strWin = "D" + strWin;
	
	strPos = General::integertostring(nPos);
	
	configPar.updateValueParameter(strWin, strPos);
	
	lRes =  m_pSysConfig->saveConfig(configPar.m_mapParameter.begin(), configPar.m_mapParameter.end());

	//配置文件修改成功，同步更新
	if (!lRes)
	{
		m_nChunkPosWindowD[nChunk] = nPos;
	}
	
	return lRes;
}


//软确认后，关闭指示灯
long CFxCpuMan::TurnOffAfterSoftSureA()
{
	long lRes;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	
	
	//关闭“硬确认”指示灯
	EnterCS();
	lRes = m_pFxCom->SetDevice("M20", 1);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_TURN_OFF_HARD_SURE_LAMP;
		
		//关闭“硬确认”指示灯失败，不必通知UI界面了，返回值就可以知晓了
		//SendMessage();
	}
	
	return 0;
}


long CFxCpuMan::TurnOffAfterSoftSureB()
{
	long lRes;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	
	
	//关闭“硬确认”指示灯
	EnterCS();
	lRes = m_pFxCom->SetDevice("M21", 1);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_TURN_OFF_HARD_SURE_LAMP;
		
		//关闭“硬确认”指示灯失败，不必通知UI界面了，返回值就可以知晓了
		//SendMessage();
	}
	
	return 0;
}


long CFxCpuMan::TurnOffAfterSoftSureC()
{
	long lRes;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	
	
	//关闭“硬确认”指示灯
	EnterCS();
	lRes = m_pFxCom->SetDevice("M22", 1);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_TURN_OFF_HARD_SURE_LAMP;
		
		//关闭“硬确认”指示灯失败，不必通知UI界面了，返回值就可以知晓了
		//SendMessage();
	}
	
	return 0;
}

long CFxCpuMan::TurnOffAfterSoftSureD()
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	long lRes;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}	
	
	//关闭“硬确认”指示灯
	EnterCS();
	lRes = m_pFxCom->SetDevice("M23", 1);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_TURN_OFF_HARD_SURE_LAMP;
		
		//关闭“硬确认”指示灯失败，不必通知UI界面了，返回值就可以知晓了
		//SendMessage();
	}
	
	return 0;
}


//软确认后，关闭指示灯
long CFxCpuMan::TurnOffAfterSoftSure(int nLocID, int nOperType)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	int nDirect, nChunkNum;
	long lRes;
	
	//查看是否越界？
	if (1 > nLocID || m_nSumSlot < nLocID)
	{
		return ERROR_PLC_LOC_ID_BOUND;
	}

	Convert2ChunkNum(nLocID, nDirect, nChunkNum);

	switch (nOperType)
	{
	case PRE_FETCH_OPER:
//	case ADJUST_OPER:		//只能开前门
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = TurnOffAfterSoftSureA();		
				break;
			case RIGHT_CHEST:
				lRes = TurnOffAfterSoftSureB();				
				break;
			default:
				break;
			}
		}
		break;
	case BATCH_ADD_OPER:
	case MED_MAN_OPER:
		{
			if (m_bDoubleFace)//有两扇门，有前后门
			{
				switch(nDirect)//开后门
				{
				case LEFT_CHEST:
					lRes = TurnOffAfterSoftSureC();					
					break;
				case RIGHT_CHEST:
					lRes = TurnOffAfterSoftSureD();					
					break;
				default:
					break;
				}
			} 
			else//只有一扇门，即前门
			{
				switch(nDirect)
				{
				case LEFT_CHEST:
					lRes = TurnOffAfterSoftSureA();					
					break;
				case RIGHT_CHEST:
					lRes = TurnOffAfterSoftSureB();					
					break;
				default:
					break;
				}
			}
		}
		break;
	default:
		break;
	}

	return lRes;
}

long CFxCpuMan::ChangeScreen(int nOperType, bool bFront)
{
	return 0;
	long lRes;

	switch (nOperType)
	{
	case PRE_FETCH_OPER:	
		{
			//true----前，false----后
			lRes = SwitchScreen(true);
		}
		break;
	case ADJUST_OPER:		//只能开前门
		{

			if (bFront)//前
			{
				//true----前，false----后
				lRes = SwitchScreen(true);
			} 
			else//后
			{
				//true----前，false----后
				lRes = SwitchScreen(true);
			}
		}
		break;
	case BATCH_ADD_OPER:
	case MED_MAN_OPER:
		{
			if (m_bDoubleFace)//有两扇门，有前后门
			{
				//true----前，false----后
				lRes = SwitchScreen(false);
			} 
			else//只有一扇门，即前
			{
				//true----前，false----后
				lRes = SwitchScreen(true);
			}
		}
		break;
	default:
		break;
	}
	
	return lRes;

}


//门是否已经是打开状态了？
long CFxCpuMan::IsDoorOpened(bool &bRes, int nOperType, bool bFront)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	long lRes;

	long lFlag;

	switch(nOperType)
	{
	case PRE_FETCH_OPER:	
		{
			EnterCS();
			lRes = m_pFxCom->GetDevice("M24", &lFlag);
			LeaveCS();
		}
		break;
	case ADJUST_OPER:		//只能开前门
		{
			
			if (bFront)//前
			{
				EnterCS();
				lRes = m_pFxCom->GetDevice("M24", &lFlag);
				LeaveCS();
			} 
			else//后
			{
				EnterCS();
				lRes = m_pFxCom->GetDevice("M26", &lFlag);
				LeaveCS();
			}
		}
		break;
	case BATCH_ADD_OPER:
	case MED_MAN_OPER:
		{
			if (m_bDoubleFace)//有两扇门，有前后门
			{
				//后门
				EnterCS();
				lRes = m_pFxCom->GetDevice("M26", &lFlag);
				LeaveCS();
			} 
			else//只有一扇门，即前
			{
				//前门
				EnterCS();
				lRes = m_pFxCom->GetDevice("M24", &lFlag);
				LeaveCS();
			}
		}
		break;
	default:
		break;
	}

	//PLC读取失败
	if (lRes)
	{
		return ERROR_PLC_GET_DEVICE_FUNC;
	}

	if (1 == lFlag)
	{
		bRes = true;
	} 
	else
	{
		bRes = false;
	}

	return 0;
}

