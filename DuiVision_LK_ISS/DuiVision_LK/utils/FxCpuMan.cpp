#include "stdafx.h"

#include <process.h>
#include "FxCpuMan.h"
#include "General.h"

LoggerPtr CFxCpuMan::m_rootLoggerPtr = Logger::getLogger("CFxCpuMan");

#define IDLE_JOB_TIME 1			//����ʱ��
#define OPEN_CLOSE_DOOR_TIME 1000	//������ʱ��
#define RUN_CHUNK_INT_WAIT_TIME 100 //�߶�����ȴ�ʱ��
#define HANDLE_INT_WAIT_TIME 1		//��ȡ��������ȴ�ʱ��

typedef enum tagAckType
{
	IDLE_JOB = 0x00,				//����ʱ������-----����ŷ�����ȫ��Ļ�쳣��

	FRONT_DOOR_OPEN_ACK,			//ǰ�Ŵ�
	FRONT_DOOR_CLOSE_ACK,			//ǰ�Źر�
		
	BACK_DOOR_OPEN_ACK,				//���Ŵ�
	BACK_DOOR_CLOSE_ACK,			//���Źر�
		
	A_WINDOW_RUN_CHUNK_DONE_ACK,	//A���߶�---����
	B_WINDOW_RUN_CHUNK_DONE_ACK,	//B���߶�---����
	C_WINDOW_RUN_CHUNK_DONE_ACK,	//C���߶�---����
	D_WINDOW_RUN_CHUNK_DONE_ACK,	//D���߶�---����

	A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK,	//A���߶�---��ȡ����
	B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK,	//B���߶�---��ȡ����
	C_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK,	//C���߶�---��ȡ����
	D_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK,	//D���߶�---��ȡ����
		
	A_WINDOW_HANDLE_DONE_ACK,		//A����ȡ�������
	B_WINDOW_HANDLE_DONE_ACK,		//B����ȡ�������
	C_WINDOW_HANDLE_DONE_ACK,		//C����ȡ�������
	D_WINDOW_HANDLE_DONE_ACK		//D����ȡ�������
		
}EnAckType;


unsigned int __stdcall ThreadFunForMonitorPLC(void * lp)
{
	CFxCpuMan* pFxCpuMan = (CFxCpuMan*)lp;
	
	CActFXCPU3 *pFxCom = pFxCpuMan->GetActFxCpu();
	long lFlag = 0;

	long lRes;

	//ʵʱ��� -------- �ŷ��Ƿ��쳣����ȫ��Ļ�쳣��
	while (true)
	{
		Sleep(IDLE_JOB_TIME);

		//��� ǰ�� ��ȫ��Ļ �쳣�澯 ��
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M55", &lFlag);	
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}

		if (1 == lFlag)
		{
			
			//�����쳣����
			//֪ͨUI���棬��ʾ����ʧ����Ϣ		
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_FRONT_MSG, 
				FRONT_SAFE_RASTER_ABNORMAL, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				FRONT_SAFE_RASTER_ABNORMAL, 0);
			//ǰ�� ��ȫ��Ļ�쳣
			pFxCpuMan->SetSafeSensorAbnormFront();
			
			
		}
		else
		{
			//ǰ�� ��ȫ��Ļ������֪ͨUI����			
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_FRONT_MSG, 
				FRONT_SAFE_RASTER_NORMAL, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				FRONT_SAFE_RASTER_NORMAL, 0);

			//ǰ�� ��ȫ��Ļ����
			pFxCpuMan->SetSafeSensorAbnormFront(false);			

		}

		//��� ���� ��ȫ��Ļ �쳣�澯 ��
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M56", &lFlag);
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}

		if (1 == lFlag)
		{
			//�����쳣����
			//֪ͨUI���棬��ʾ����ʧ����Ϣ			
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_BACK_MSG, 
				BACK_SAFE_RASTER_ABNORMAL, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				BACK_SAFE_RASTER_ABNORMAL, 0);
			//ǰ�� ��ȫ��Ļ�쳣
			pFxCpuMan->SetSafeSensorAbnormBack();
			
			//continue;
		}
		else
		{
			//���� ��ȫ��Ļ������֪ͨUI����			
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SAFE_SENSOR_BACK_MSG, 
				BACK_SAFE_RASTER_NORMAL, 0);
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				BACK_SAFE_RASTER_NORMAL, 0);

			//���� ��ȫ��Ļ����
			pFxCpuMan->SetSafeSensorAbnormBack(false);
		}


		//���ŷ� �Ƿ� �쳣�澯��
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M51", &lFlag);
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}

		if (1 == lFlag)
		{
			//�����쳣����
			//֪ͨUI���棬��ʾ����ʧ����Ϣ
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				LEFT_SERVO_ABNORMAL, 0);
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SERVO_A_MSG, 
				LEFT_SERVO_ABNORMAL, 0);
			//PLC�쳣
			pFxCpuMan->SetErrorLeftServo();

		}
		else
		{
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				LEFT_SERVO_NORMAL, 0);
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SERVO_A_MSG, 
				LEFT_SERVO_NORMAL, 0);
			//PLC����
			pFxCpuMan->SetErrorLeftServo(false);

		}
		
	
		//���ŷ� �Ƿ� �쳣�澯��
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M52", &lFlag);
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}
		if (1 == lFlag)
		{
			//�����쳣����
			//֪ͨUI���棬��ʾ����ʧ����Ϣ
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				RIGHT_SERVO_ABNORMAL, 0);
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SERVO_B_MSG, 
				RIGHT_SERVO_ABNORMAL, 0);
			//PLC�쳣
			pFxCpuMan->SetErrorRightServo();
			
			//continue;
		}
		else
		{			
			SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
				RIGHT_SERVO_NORMAL, 0);
			SendMessage(pFxCpuMan->GetPlcMonitorDlgWndHnd(), PLC_MONITOR_SERVO_B_MSG, 
				RIGHT_SERVO_NORMAL, 0);
			//PLC����
			pFxCpuMan->SetErrorRightServo(false);
		}
		//�жϼ�ͣ��ť�Ƿ���
		pFxCpuMan->EnterCS();
		lRes = pFxCom->GetDevice("M90", &lFlag);
		pFxCpuMan->LeaveCS();
		if (lRes)
		{
			continue;
		}
		if (1 == lFlag)
		{
			pFxCpuMan->m_rootLoggerPtr->error("��ͣ��ť�Ѱ���");
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
		case IDLE_JOB://��������ʱ
			{
				Sleep(IDLE_JOB_TIME);				
				continue;
			}
			break;
		case FRONT_DOOR_OPEN_ACK://��ǰ��
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//ǰ�Ŵ��Ƿ���ɣ�
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

					//ǰ�Ŵ��쳣
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M53", &lDoorFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					if(1 == lDoorFlag)
					{
						//�����쳣����
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							FRONT_DOOR_OPEN_FALSE, 0);
						
						//PLC�쳣
						pFxCpuMan->SetErrorPLC();
						//�쳣�ˣ�ҲҪ�ָ���IDLE_JOB״̬��PLC�Ѿ������˲����쳣�ˣ��������µȴ��������
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					}
					else
					{
						continue;
					}										
					
				}
				else
				{
					//����;
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						FRONT_DOOR_OPEN_TRUE, 0);
				}
			}
			break;
		case FRONT_DOOR_CLOSE_ACK://�ر�ǰ��
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//ǰ�Źر��Ƿ���ɣ�
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

					//ǰ�Źر��쳣
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M54", &lDoorFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					if (1 == lDoorFlag)
					{
						//�����쳣����
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							FRONT_DOOR_CLOSE_FALSE, 0);
						
						//PLC�쳣
						pFxCpuMan->SetErrorPLC();
						//�쳣�ˣ�ҲҪ�ָ���IDLE_JOB״̬��PLC�Ѿ������˲����쳣�ˣ��������µȴ��������
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					} 
					else
					{
						continue;
					}
										
				}
				else
				{
					//����;
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);

					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						FRONT_DOOR_CLOSE_TRUE, 0);
				}
			}
			break;
		case BACK_DOOR_OPEN_ACK://�򿪺���
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//���Ŵ��Ƿ���ɣ�
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

					//���Ŵ��쳣
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M55", &lDoorFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					if (1 == lDoorFlag)
					{
						//�����쳣����
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							BACK_DOOR_OPEN_FALSE, 0);
						
						//PLC�쳣
						pFxCpuMan->SetErrorPLC();
						//�쳣�ˣ�ҲҪ�ָ���IDLE_JOB״̬��PLC�Ѿ������˲����쳣�ˣ��������µȴ��������
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					} 
					else
					{
						continue;
					}	
					
				}
				else
				{
					//����;
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						BACK_DOOR_OPEN_TRUE, 0);
				}
			}
			break;
		case BACK_DOOR_CLOSE_ACK://�رպ���
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//���Źر��Ƿ���ɣ�
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
					
					//���Źر��쳣
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M56", &lDoorFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					if (1 == lDoorFlag)
					{
						//�����쳣����
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							BACK_DOOR_CLOSE_FALSE, 0);
						
						//PLC�쳣
						pFxCpuMan->SetErrorPLC();
						//�쳣�ˣ�ҲҪ�ָ���IDLE_JOB״̬��PLC�Ѿ������˲����쳣�ˣ��������µȴ��������
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					} 
					else
					{
						continue;
					}					
					
				}
				else
				{
					//����;
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						BACK_DOOR_CLOSE_TRUE, 0);
				}
			}
			break;
		case A_WINDOW_RUN_CHUNK_DONE_ACK://A���߶�---����
			{							
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
			//	Sleep(2000);
				//A���߶��Ƿ����
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M4", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}				
				//�Ƿ�A���߶���ɡ�
				if (1 != lFlag)//��ʱ��δ���
				{
					//������ŷ��쳣
					if (pFxCpuMan->GetErrorLeftServo())
					{
						//�ŷ��쳣�ˣ��������ˣ����⽩��������һ��״̬����
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_RUN_CHUNK_DONE_FALSE, 0);

						
					}
					else//���쳣��������ȴ���Ҳ���Ƕ��ݵĹ�Ļ�쳣��ɵģ�����Ϳ����ˡ�
					{
						continue;
					}
					
				}
				else
				{
					//����;
					//������----����״̬
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);	
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_A_RUN_CHUNK_DONE_TRUE, 0);
					
									
				}
			}
			break;
		case C_WINDOW_RUN_CHUNK_DONE_ACK://C���߶�---����
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//C���߶��Ƿ����
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M4", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//�Ƿ�C���߶���ɡ�
				if (1 != lFlag)
				{					
					//������ŷ��쳣
					if (pFxCpuMan->GetErrorLeftServo())
					{
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_RUN_CHUNK_DONE_FALSE, 0);
						
						//�ŷ��쳣�ˣ��������ˣ����⽩��������һ��״̬����
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					}
					else//���쳣��������ȴ���Ҳ���Ƕ��ݵĹ�Ļ�쳣��ɵģ�����Ϳ����ˡ�
					{
						continue;
					}
				}
				else
				{

					//����;
					//������----����״̬
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_A_RUN_CHUNK_DONE_TRUE, 0);
					
										
				}
			}
			break;
		case A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK://A���߶�--��--��ȡ����
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//A���߶��Ƿ����
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M4", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//�Ƿ�A���߶���ɡ�
				if (1 != lFlag)
				{
					//������ŷ��쳣
					if (pFxCpuMan->GetErrorLeftServo())
					{
						//�ŷ��쳣�ˣ��������ˣ����⽩��������һ��״̬����
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, 0);
						
						
					}
					else//���쳣��������ȴ���Ҳ���Ƕ��ݵĹ�Ļ�쳣��ɵģ�����Ϳ����ˡ�
					{
						continue;
					}
				}
				else
				{
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_TRUE, 0);

					//����;
					//������----A����ȡ����
					pFxCpuMan->SetAckTypeServoA(A_WINDOW_HANDLE_DONE_ACK);					
				}
			}
			break;
		case C_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK://C��--�߶�--��--��ȡ����
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//C���߶��Ƿ����
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M4", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//�Ƿ�C���߶���ɡ�
				if (1 != lFlag)
				{					
					//������ŷ��쳣
					if (pFxCpuMan->GetErrorLeftServo())
					{

						//�ŷ��쳣�ˣ��������ˣ����⽩��������һ��״̬����
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, 0);
						
						
					}
					else//���쳣��������ȴ���Ҳ���Ƕ��ݵĹ�Ļ�쳣��ɵģ�����Ϳ����ˡ�
					{
						continue;
					}
				}
				else
				{
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_A_RUN_CHUNK_FOR_HANDLE_DONE_TRUE, 0);
					
					//����;
					//������----C����ȡ����
					pFxCpuMan->SetAckTypeServoA(C_WINDOW_HANDLE_DONE_ACK);					
				}
			}
			break;
		case A_WINDOW_HANDLE_DONE_ACK://A����ȡ����
			{
				//��δ�����ȷ�ϡ���ȡ������ɡ�
				if (!pFxCpuMan->GetSoftSureServoA())
				{
					Sleep(HANDLE_INT_WAIT_TIME);
					
					//��A����ȡ��������� ��Ӳ�����Ƿ���ɣ�
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M20", &lFlag);
					//lRes = pFxCom->GetDevice("M12", &lFlag);		
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					//��δӲ����ȷ�ϡ���ȡ������ɡ�
					if (1 != lFlag)
					{
						///������һ�֣��鿴�Ƿ�ò����Ƿ���ɣ�
						continue;
					}
					else//Ӳ����ȷ�ϡ���ȡ������ɡ�
					{
						pFxCpuMan->m_rootLoggerPtr->error("A�����Ӳ��ȷ�ϰ�ť�Ѱ���");
						
						//��һ�֣����롰���д���
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
											
						//����ȡ������ɡ���λ��----����δ��ȷ�ϡ���ȡ������ɡ���
						pFxCpuMan->SetSoftSureServoA(false);
						//֪ͨUI���棬��ʾ�����ɹ���Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_HANDLE_HAD_DONE, 0);
					}
				}
				else//��������Ѿ�����ȡ������ɡ�
				{
					pFxCpuMan->m_rootLoggerPtr->error("A��������ȷ�ϰ�ť�Ѱ���");
					//��һ�֣����롰���д���
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);

					//��ȡҩ��ɡ���λ��----����δ��ȷ�ϡ���ȡ������ɡ���
					pFxCpuMan->SetSoftSureServoA(false);

					//�����ȷ�ϣ�����UI����ȷ�ϵģ����Բ���Ҫ������Ϣ�ˣ�
				}		
			}
			break;
		case C_WINDOW_HANDLE_DONE_ACK://C����ȡ����
			{
				//��δ�����ȷ�ϡ���ȡ������ɡ�
				if (!pFxCpuMan->GetSoftSureServoA())
				{
					Sleep(HANDLE_INT_WAIT_TIME);
					
					//��C����ȡ��������� ��Ӳ�����Ƿ���ɣ�
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("X22", &lFlag);
					//lRes = pFxCom->GetDevice("M14", &lFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}	
					//��δӲ����ȷ�ϡ���ȡ������ɡ�
					if (1 != lFlag)
					{
						//������һ�֣��鿴�Ƿ�ò����Ƿ���ɣ�
						continue;
					}
					else//Ӳ����ȷ�ϡ���ȡ������ɡ�
					{
						
						pFxCpuMan->m_rootLoggerPtr->error("C�����ȷ�ϰ�ť�Ѱ���");
						//��һ�֣����롰���д���
						pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
						
						//����ȡ������ɡ���λ��----����δ��ȷ�ϡ���ȡ������ɡ���
						pFxCpuMan->SetSoftSureServoA(false);
						//֪ͨUI���棬��ʾ�����ɹ���Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_A_HANDLE_HAD_DONE, 0);
					}
					
				}
				else//��������Ѿ�����ȡ������ɡ�
				{
					//��һ�֣����롰���д���
					pFxCpuMan->SetAckTypeServoA(IDLE_JOB);
					
					//����ȡ������ɡ���λ��----����δ��ȷ�ϡ���ȡ������ɡ���
					pFxCpuMan->SetSoftSureServoA(false);
					
					//�����ȷ�ϣ�����UI����ȷ�ϵģ����Բ���Ҫ������Ϣ�ˣ�
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
		case IDLE_JOB://��������ʱ
			{
				Sleep(IDLE_JOB_TIME);
				
				continue;
			}
			break;
		case B_WINDOW_RUN_CHUNK_DONE_ACK://B���߶�---����
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//B���߶��Ƿ����
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M5", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//�Ƿ�B���߶���ɡ�
				if (1 != lFlag)
				{					
					//������ŷ��쳣
					if (pFxCpuMan->GetErrorRightServo())
					{

						//�ŷ��쳣�ˣ��������ˣ����⽩��������һ��״̬����
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_RUN_CHUNK_DONE_FALSE, 0);
						
						
					}
					else//���쳣��������ȴ���Ҳ���Ƕ��ݵĹ�Ļ�쳣��ɵģ�����Ϳ����ˡ�
					{
						continue;
					}
				}
				else
				{

					//����;
					//������----����״̬
					pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_B_RUN_CHUNK_DONE_TRUE, 0);
					
										
				}
			}
			break;
		case D_WINDOW_RUN_CHUNK_DONE_ACK://D���߶�---����
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//D���߶��Ƿ����
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M5", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//�Ƿ�D���߶���ɡ�
				if (1 != lFlag)
				{					
					//������ŷ��쳣
					if (pFxCpuMan->GetErrorRightServo())
					{

						//�ŷ��쳣�ˣ��������ˣ����⽩��������һ��״̬����
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_RUN_CHUNK_DONE_FALSE, 0);
						
						
					}
					else//���쳣��������ȴ���Ҳ���Ƕ��ݵĹ�Ļ�쳣��ɵģ�����Ϳ����ˡ�
					{
						continue;
					}
				}
				else
				{

					//����;
					//������----����״̬
					pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_B_RUN_CHUNK_DONE_TRUE, 0);
				
										
				}
			}
			break;
		case B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK://B��--�߶�--��--��ȡ����
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//B���߶��Ƿ����
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M5", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//�Ƿ�B���߶���ɡ�
				if (1 != lFlag)
				{
					//������ŷ��쳣
					if (pFxCpuMan->GetErrorRightServo())
					{

						//�ŷ��쳣�ˣ��������ˣ����⽩��������һ��״̬����
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, 0);
						
						
					}
					else//���쳣��������ȴ���Ҳ���Ƕ��ݵĹ�Ļ�쳣��ɵģ�����Ϳ����ˡ�
					{
						continue;
					}
				}
				else
				{
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_TRUE, 0);
					
					//����;
					//������----B����ȡ����
					pFxCpuMan->SetAckTypeServoB(B_WINDOW_HANDLE_DONE_ACK);					
				}
			}
			break;
		case D_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK://D��--�߶�--��--��ȡ����
			{
				Sleep(RUN_CHUNK_INT_WAIT_TIME);
				
				//D���߶��Ƿ����
				pFxCpuMan->EnterCS();
				lRes = pFxCom->GetDevice("M5", &lFlag);
				pFxCpuMan->LeaveCS();
				if (lRes)
				{
					continue;
				}
				//�Ƿ�D���߶���ɡ�
				if (1 != lFlag)
				{					
					//������ŷ��쳣
					if (pFxCpuMan->GetErrorRightServo())
					{
						//֪ͨUI���棬��ʾ����ʧ����Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_FALSE, 0);
						
						//�ŷ��쳣�ˣ��������ˣ����⽩��������һ��״̬����
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					}
					else//���쳣��������ȴ���Ҳ���Ƕ��ݵĹ�Ļ�쳣��ɵģ�����Ϳ����ˡ�
					{
						continue;
					}
				}
				else
				{
					//֪ͨUI���棬��ʾ�����ɹ���Ϣ
					SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
						SERVO_B_RUN_CHUNK_FOR_HANDLE_DONE_TRUE, 0);
					
					//����;
					//������----D�����Ҵ�ȡ����
					pFxCpuMan->SetAckTypeServoB(D_WINDOW_HANDLE_DONE_ACK);					
				}
			}
			break;
		case B_WINDOW_HANDLE_DONE_ACK://B����ȡ����
			{
				//��δ�����ȷ�ϡ���ȡ������ɡ�
				if (!pFxCpuMan->GetSoftSureServoB())
				{
					Sleep(HANDLE_INT_WAIT_TIME);
					
					//��B����ȡ��������� ��Ӳ�����Ƿ���ɣ�
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("M21", &lFlag);
					pFxCpuMan->LeaveCS();
					//lRes = pFxCom->GetDevice("M13", &lFlag);					
					if (lRes)
					{
						continue;
					}
					//��δӲ����ȷ�ϡ���ȡ������ɡ�
					if (1 != lFlag)
					{
						//������һ�֣��鿴�Ƿ�ò����Ƿ���ɣ�
						continue;
					}
					else//Ӳ����ȷ�ϡ���ȡ������ɡ�
					{
						
						pFxCpuMan->m_rootLoggerPtr->error("B���Ҳ�ȷ�ϰ�ť�Ѱ���");
						//��һ�֣����롰���д���
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						
						//����ȡ������ɡ���λ��----����δ��ȷ�ϡ���ȡ������ɡ���
						pFxCpuMan->SetSoftSureServoB(false);

						//֪ͨUI���棬��ʾ�����ɹ���Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_HANDLE_HAD_DONE, 0);
					}					
				}
				else//��������Ѿ�����ȡ������ɡ�
				{
					//��һ�֣����롰���д���
					pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					
					//����ȡ������ɡ���λ��----����δ��ȷ�ϡ���ȡ������ɡ���
					pFxCpuMan->SetSoftSureServoB(false);
					
					//�����ȷ�ϣ�����UI����ȷ�ϵģ����Բ���Ҫ������Ϣ�ˣ�
				}
			}
			break;
		case D_WINDOW_HANDLE_DONE_ACK://D����ȡ����
			{
				//��δ�����ȷ�ϡ���ȡ������ɡ�
				if (!pFxCpuMan->GetSoftSureServoB())
				{
					Sleep(HANDLE_INT_WAIT_TIME);
					
					//��D����ȡ��������� ��Ӳ�����Ƿ���ɣ�
					pFxCpuMan->EnterCS();
					lRes = pFxCom->GetDevice("X23", &lFlag);
					//lRes = pFxCom->GetDevice("M15", &lFlag);
					pFxCpuMan->LeaveCS();
					if (lRes)
					{
						continue;
					}
					//��δӲ����ȷ�ϡ���ȡ������ɡ�
					if (1 != lFlag)
					{
						//������һ�֣��鿴�Ƿ�ò����Ƿ���ɣ�
						continue;
					}
					else//Ӳ����ȷ�ϡ���ȡ������ɡ�
					{
						
						pFxCpuMan->m_rootLoggerPtr->error("D���Ҳ�ȷ�ϰ�ť�Ѱ���");
						//��һ�֣����롰���д���
						pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
						
						//����ȡ������ɡ���λ��----����δ��ȷ�ϡ���ȡ������ɡ���
						pFxCpuMan->SetSoftSureServoB(false);

						//֪ͨUI���棬��ʾ�����ɹ���Ϣ
						SendMessage(pFxCpuMan->GetCurShowDlgWndHnd(), PLC_RT_STATE_MSG, 
							SERVO_B_HANDLE_HAD_DONE, 0);
					}
					
				}
				else//��������Ѿ�����ȡ������ɡ�
				{
					//��һ�֣����롰���д���
					pFxCpuMan->SetAckTypeServoB(IDLE_JOB);
					
					//����ȡ������ɡ���λ��----����δ��ȷ�ϡ���ȡ������ɡ���
					pFxCpuMan->SetSoftSureServoB(false);
					
					//�����ȷ�ϣ�����UI����ȷ�ϵģ����Բ���Ҫ������Ϣ�ˣ�
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

	//��ȡ������Ϣ
	m_nTierNum = configPar.getIntParameter("TierNum");
	m_nTierCapacity = configPar.getIntParameter("TierCapacity");
	m_nPortNum = configPar.getIntParameter("PlcPort");
	m_bDoubleFace = ((1 == configPar.getIntParameter("DoubleFace"))?true:false);
	m_nRunMode = configPar.getIntParameter("RunMode");


	/*!> ��ȡ����λ��------A window A���� */
	m_nChunkPosWindowA[0] = configPar.getIntParameter("A0");
	m_nChunkPosWindowA[1] = configPar.getIntParameter("A1");
	m_nChunkPosWindowA[2] = configPar.getIntParameter("A2");
	m_nChunkPosWindowA[3] = configPar.getIntParameter("A3");
	m_nChunkPosWindowA[4] = configPar.getIntParameter("A4");
	m_nChunkPosWindowA[5] = configPar.getIntParameter("A5");

	/*!> ��ȡ����λ��------B window B���� */
	m_nChunkPosWindowB[0] = configPar.getIntParameter("B0");
	m_nChunkPosWindowB[1] = configPar.getIntParameter("B1");
	m_nChunkPosWindowB[2] = configPar.getIntParameter("B2");
	m_nChunkPosWindowB[3] = configPar.getIntParameter("B3");
	m_nChunkPosWindowB[4] = configPar.getIntParameter("B4");
	m_nChunkPosWindowB[5] = configPar.getIntParameter("B5");

	/*!> ��ȡ����λ��------C window C���� */
	m_nChunkPosWindowC[0] = configPar.getIntParameter("C0");
	m_nChunkPosWindowC[1] = configPar.getIntParameter("C1");
	m_nChunkPosWindowC[2] = configPar.getIntParameter("C2");
	m_nChunkPosWindowC[3] = configPar.getIntParameter("C3");
	m_nChunkPosWindowC[4] = configPar.getIntParameter("C4");
	m_nChunkPosWindowC[5] = configPar.getIntParameter("C5");

	/*!> ��ȡ����λ��------D window D���� */
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

	//�ر�
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
			
			//���ö˿ں�
			m_pFxCom->SetActPortNumber(m_nPortNum);
			//���ó�ʱ
			m_pFxCom->SetActTimeOut(200);
			
			//��
			lRes = m_pFxCom->Open();
			if (lRes)
			{
				return ERROR_PLC_OPEN_PORT;
			}
			
			Sleep(50);

//	#if (!PLC_DEBUG_MODE)
			//����ʵʱ���PLC�߳�
			HANDLE hThreadPLC = (HANDLE)_beginthreadex(NULL, 0, ThreadFunForMonitorPLC, this, 0, 0);
			
			//����PLC���������߳�--�ŷ�A
			HANDLE hThreadAckServoA = (HANDLE)_beginthreadex(NULL, 0, ThreadFunForFxCpuAckServoA, this, 0, 0);

			//����PLC���������߳�--�ŷ�B
			HANDLE hThreadAckServoB = (HANDLE)_beginthreadex(NULL, 0, ThreadFunForFxCpuAckServoB, this, 0, 0);
			m_rootLoggerPtr->error("����PLC ����߳�");
			//	#endif
			m_bIsOpen = true;
		}

		return 0;
	}

//  �˴���Ҫ�޸�
long CFxCpuMan::OpenDoor(int nOperType, bool bBack)
{
	long lRes;

	switch (nOperType)
	{
	case PRE_FETCH_OPER://����ȡҩ�������Ƿ���ǰ��ֻ��ǰ��	
		{
			lRes = OpenFrontDoor();

			//���͡���ǰ�š�����ɹ�
			if (!lRes)
			{
				//���뵽�ȴ�����ǰ����ɡ�״̬��
				SetAckTypeServoA(FRONT_DOOR_OPEN_ACK);
			}
		}
		break;
	case ADJUST_OPER:	//У��������ʵ��bBack���жϿ�ǰ�ţ����ǿ����ţ�
		{
			if (bBack)//����
			{
				lRes = OpenBackDoor();//������
				//���͡��򿪺��š�����ɹ�
				if (!lRes)
				{
					//���뵽�ȴ����򿪺�����ɡ�״̬��
					SetAckTypeServoA(BACK_DOOR_OPEN_ACK);
				}
			}
			else//����ǰ��
			{
				lRes = OpenFrontDoor();//��ǰ��
				
				//���͡���ǰ�š�����ɹ�
				if (!lRes)
				{
					//���뵽�ȴ�����ǰ����ɡ�״̬��
					SetAckTypeServoA(FRONT_DOOR_OPEN_ACK);
				}
			}
		}
		break;
	case BATCH_ADD_OPER://������ҩ   Ϊɶû�в�������
	case MED_MAN_OPER:	//ҩƷ����
		{
			if (m_bDoubleFace)//��ǰ����
			{
				lRes = OpenBackDoor();//������
				//���͡��򿪺��š�����ɹ�
				if (!lRes)
				{
					//���뵽�ȴ����򿪺�����ɡ�״̬��
					SetAckTypeServoA(BACK_DOOR_OPEN_ACK);
				}
			}
			else//ֻ��ǰ��
			{
				lRes = OpenFrontDoor();//��ǰ��

				//���͡���ǰ�š�����ɹ�
				if (!lRes)
				{
					//���뵽�ȴ�����ǰ����ɡ�״̬��
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
	case PRE_FETCH_OPER://����ȡҩ�������Ƿ���ǰ��ֻ��ǰ��
		{
			lRes = CloseFrontDoor();
			//���͡��ر�ǰ�š�����ɹ�
			if (!lRes)
			{
				//���뵽�ȴ����ر�ǰ����ɡ�״̬��
				SetAckTypeServoA(FRONT_DOOR_CLOSE_ACK);
			}
		}		
		break;
	case ADJUST_OPER:	//У��������ʵ�����ж�ǰ�ţ����Ǻ��ţ�
		{
			if (bBack)//����
			{
				lRes = CloseBackDoor();//�غ���
				//���͡��رպ��š�����ɹ�
				if (!lRes)
				{
					//���뵽�ȴ����رպ�����ɡ�״̬��
					SetAckTypeServoA(BACK_DOOR_CLOSE_ACK);
				}
			}
			else//ǰ��
			{
				lRes = CloseFrontDoor();//��ǰ��
				//���͡��ر�ǰ�š�����ɹ�
				if (!lRes)
				{
					//���뵽�ȴ����ر�ǰ����ɡ�״̬��
					SetAckTypeServoA(FRONT_DOOR_CLOSE_ACK);
				}
			}
		}
		break;
	case BATCH_ADD_OPER://������ҩ
	case MED_MAN_OPER:	//ҩƷ����
		{
			if (m_bDoubleFace)//��ǰ����
			{
				lRes = CloseBackDoor();//�غ���
				//���͡��رպ��š�����ɹ�
				if (!lRes)
				{
					//���뵽�ȴ����رպ�����ɡ�״̬��
					SetAckTypeServoA(BACK_DOOR_CLOSE_ACK);
				}
			}
			else//ֻ��ǰ��
			{
				lRes = CloseFrontDoor();//��ǰ��
				//���͡��ر�ǰ�š�����ɹ�
				if (!lRes)
				{
					//���뵽�ȴ����ر�ǰ����ɡ�״̬��
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

//��ǰ��
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

//��ǰ��
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


//������
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


//�غ���
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



//A�����߶�
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
		����д����Ԫ�� 
		����1����Ԫ������
		����2��д�����
		����3��Ҫд�����Ԫ��ֵ
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
// 		������Ԫ��
// 		����1����Ԫ������
// 		����2����������
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

//B�����߶�
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

//C�����߶�
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
		����д����Ԫ�� 
		����1����Ԫ������
		����2��д�����
		����3��Ҫд�����Ԫ��ֵ
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
		������Ԫ��
		����1����Ԫ������
		����2����������
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

//D�����߶�
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

//nDirect���أ����ҹ�
//nChunkNum���أ�����
/*--------------** ����ҹ� **-----------------*/
void CFxCpuMan::Convert2ChunkNum(const int nLocID, int &nDirect, int &nChunkNum)
{
//	int nSingleSum;
	
	//�����ŷ���ҩ�۸���;(24�㣬ÿ��5����)
//	nSingleSum = m_nTierNum * m_nTierCapacity;

	if (nLocID <= 120)//A����
	{
		/*
			ÿ������12������ÿ������2�㣬��ÿ������12 x 2 = 24 ��
			ÿ����5���ۣ���ÿ�����У�24 x 5 = 120 ����
			ÿ�δ��ڿ���2��������4�㣬
			24�㣬���ڿ��ӱ���һ�Σ���Ҫ 24/4 = 6 �ڣ�0-5�ڣ�
		*/
		nChunkNum = (nLocID-1) / m_nTierCapacity / 4;
		nDirect = LEFT_CHEST;//�����ң��ŷ���(LOC_ID,���ݿ�洢���Ǵ�1��ʼ��)
	}
	else if (nLocID <= 240)//B����
	{
		nChunkNum = (nLocID - 120-1) / m_nTierCapacity / 4;
		nDirect = RIGHT_CHEST;
	}

	return;
}


//�߶�----�����߶�
long CFxCpuMan::RunChunk(int nLocID, int nOperType, bool bBack)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	int nDirect, nChunkNum;
	long lRes;
	
	//�鿴�Ƿ�Խ�磿
	if (1 > nLocID || m_nSumSlot < nLocID)
	{
		return ERROR_PLC_LOC_ID_BOUND;
	}

	Convert2ChunkNum(nLocID, nDirect, nChunkNum);

	switch (nOperType)
	{
	case PRE_FETCH_OPER:	//ֻ�ܿ�ǰ��
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = RunChunkWindowA(nChunkNum);
				//���͡�A�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���A�����߶���ɡ�״̬��
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//���͡�B�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���B�����߶���ɡ�״̬��
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
			if (bBack)//������
			{
				switch(nDirect)//������
				{
				case LEFT_CHEST:
					lRes = RunChunkWindowC(nChunkNum);
					//���͡�C���߶�������ɹ�
					if (!lRes)
					{
						//���뵽�ȴ���C���߶���ɡ�״̬��
						SetAckTypeServoA(C_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				case RIGHT_CHEST:
					lRes = RunChunkWindowD(nChunkNum);
					//���͡�D���߶�������ɹ�
					if (!lRes)
					{
						//���뵽�ȴ���D���߶���ɡ�״̬��
						SetAckTypeServoB(D_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				default:
					break;
				}
			} 
			else//ֻ��һ���ţ���ǰ��
			{
				switch(nDirect)
				{
				case LEFT_CHEST:
					lRes = RunChunkWindowA(nChunkNum);
					//���͡�A���߶�������ɹ�
					if (!lRes)
					{
						//���뵽�ȴ���A���߶���ɡ�״̬��
						SetAckTypeServoA(A_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				case RIGHT_CHEST:
					lRes = RunChunkWindowB(nChunkNum);
					//���͡�B���߶�������ɹ�
					if (!lRes)
					{
						//���뵽�ȴ���B���߶���ɡ�״̬��
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
				//���͡�A�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���A�����߶���ɡ�״̬��
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//���͡�B�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���B�����߶���ɡ�״̬��
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
			if (m_bDoubleFace)//�������ţ���ǰ����
			{
				switch(nDirect)//������
				{
				case LEFT_CHEST:
					lRes = RunChunkWindowC(nChunkNum);
					//���͡�C���߶�������ɹ�
					if (!lRes)
					{
						//���뵽�ȴ���C���߶���ɡ�״̬��
						SetAckTypeServoA(C_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				case RIGHT_CHEST:
					lRes = RunChunkWindowD(nChunkNum);
					//���͡�D���߶�������ɹ�
					if (!lRes)
					{
						//���뵽�ȴ���D���߶���ɡ�״̬��
						SetAckTypeServoB(D_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				default:
					break;
				}
			} 
			else//ֻ��һ���ţ���ǰ��
			{
				switch(nDirect)
				{
				case LEFT_CHEST:
					lRes = RunChunkWindowA(nChunkNum);
					//���͡�A���߶�������ɹ�
					if (!lRes)
					{
						//���뵽�ȴ���A���߶���ɡ�״̬��
						SetAckTypeServoA(A_WINDOW_RUN_CHUNK_DONE_ACK);
					}
					break;
				case RIGHT_CHEST:
					lRes = RunChunkWindowB(nChunkNum);
					//���͡�B���߶�������ɹ�
					if (!lRes)
					{
						//���뵽�ȴ���B���߶���ɡ�״̬��
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

//�߶�----��-----��ȡ����
long CFxCpuMan::RunChunkAndHandle(int nLocID, int nOperType)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	int nDirect, nChunkNum;
	long lRes;
	
	//�鿴�Ƿ�Խ�磿
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
				//���͡�A�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���A�����߶����---������---��ȡ������״̬��
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//���͡�B�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���B�����߶����---������---��ȡ������״̬��
					SetAckTypeServoB(B_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			default:
				break;
			}
		}
		break;
	case ADJUST_OPER:		//ֻ�ܿ�ǰ��
		{
			switch(nDirect)
			{
			case LEFT_CHEST:
				lRes = RunChunkWindowA(nChunkNum);
				//���͡�A�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���A�����߶����---������---��ȡ������״̬��
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//���͡�B�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���B�����߶����---������---��ȡ������״̬��
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
				//���͡�A�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���A�����߶����---������---��ȡ������״̬��
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//���͡�B�����߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���B�����߶����---������---��ȡ������״̬��
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
				//���͡�A���߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���A���߶����---������---��ȡ������״̬��
					SetAckTypeServoA(A_WINDOW_RUN_CHUNK_AND_HANDLE_DONE_ACK);
				}
				break;
			case RIGHT_CHEST:
				lRes = RunChunkWindowB(nChunkNum);
				//���͡�B���߶�������ɹ�
				if (!lRes)
				{
					//���뵽�ȴ���B���߶����---������---��ȡ������״̬��
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


//��ڲ�����Ϊtrue���л���ǰ��Ļ��Ϊfalse���л�����Ļ
long CFxCpuMan::SwitchScreen(bool bFront)
{
	long lRes;
	long lFlag;

	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}


	//ֻ����˫��ʱ��������Ļ�л�����
	if (m_bDoubleFace)
	{
		//ǰ��
		if (bFront)
		{
			lFlag = 0;
		}
		else//����
		{
			lFlag = 1;
		}

		//��������
		EnterCS();
		lRes = m_pFxCom->SetDevice("M10", lFlag);
		LeaveCS();
		if (lRes)
		{
			return ERROR_PLC_SWITCH_SCREEN;

			//��Ļ�л�ʧ�ܣ�����֪ͨUI�����ˣ�����ֵ�Ϳ���֪����
			//SendMessage();
		}
	}

	return 0;
}

//���������ƣ�Ĭ��ȱʡֵΪtrue--�򿪣�false--�ر�
long CFxCpuMan::TurnBuzzer(bool bOn)
{
	long lRes;
	long lFlag;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}

	//��
	if (bOn)
	{
		lFlag = 0;
	}
	else//�ر�
	{
		lFlag = 1;
	}
		
	//���÷���������
	EnterCS();
	lRes = m_pFxCom->SetDevice("M11", lFlag);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_BUZZER;
			
		//����������ʧ�ܣ�����֪ͨUI�����ˣ�����ֵ�Ϳ���֪����
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
	
	//�����ļ��޸ĳɹ���ͬ������
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

	//�����ļ��޸ĳɹ���ͬ������
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

	//�����ļ��޸ĳɹ���ͬ������
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

	//�����ļ��޸ĳɹ���ͬ������
	if (!lRes)
	{
		m_nChunkPosWindowD[nChunk] = nPos;
	}
	
	return lRes;
}


//��ȷ�Ϻ󣬹ر�ָʾ��
long CFxCpuMan::TurnOffAfterSoftSureA()
{
	long lRes;
	
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	
	
	//�رա�Ӳȷ�ϡ�ָʾ��
	EnterCS();
	lRes = m_pFxCom->SetDevice("M20", 1);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_TURN_OFF_HARD_SURE_LAMP;
		
		//�رա�Ӳȷ�ϡ�ָʾ��ʧ�ܣ�����֪ͨUI�����ˣ�����ֵ�Ϳ���֪����
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
	
	
	//�رա�Ӳȷ�ϡ�ָʾ��
	EnterCS();
	lRes = m_pFxCom->SetDevice("M21", 1);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_TURN_OFF_HARD_SURE_LAMP;
		
		//�رա�Ӳȷ�ϡ�ָʾ��ʧ�ܣ�����֪ͨUI�����ˣ�����ֵ�Ϳ���֪����
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
	
	
	//�رա�Ӳȷ�ϡ�ָʾ��
	EnterCS();
	lRes = m_pFxCom->SetDevice("M22", 1);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_TURN_OFF_HARD_SURE_LAMP;
		
		//�رա�Ӳȷ�ϡ�ָʾ��ʧ�ܣ�����֪ͨUI�����ˣ�����ֵ�Ϳ���֪����
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
	
	//�رա�Ӳȷ�ϡ�ָʾ��
	EnterCS();
	lRes = m_pFxCom->SetDevice("M23", 1);
	LeaveCS();
	if (lRes)
	{
		return ERROR_PLC_TURN_OFF_HARD_SURE_LAMP;
		
		//�رա�Ӳȷ�ϡ�ָʾ��ʧ�ܣ�����֪ͨUI�����ˣ�����ֵ�Ϳ���֪����
		//SendMessage();
	}
	
	return 0;
}


//��ȷ�Ϻ󣬹ر�ָʾ��
long CFxCpuMan::TurnOffAfterSoftSure(int nLocID, int nOperType)
{
	if (1 == m_nRunMode || 9 == m_nRunMode)
	{
		return 0;
	}
	int nDirect, nChunkNum;
	long lRes;
	
	//�鿴�Ƿ�Խ�磿
	if (1 > nLocID || m_nSumSlot < nLocID)
	{
		return ERROR_PLC_LOC_ID_BOUND;
	}

	Convert2ChunkNum(nLocID, nDirect, nChunkNum);

	switch (nOperType)
	{
	case PRE_FETCH_OPER:
//	case ADJUST_OPER:		//ֻ�ܿ�ǰ��
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
			if (m_bDoubleFace)//�������ţ���ǰ����
			{
				switch(nDirect)//������
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
			else//ֻ��һ���ţ���ǰ��
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
			//true----ǰ��false----��
			lRes = SwitchScreen(true);
		}
		break;
	case ADJUST_OPER:		//ֻ�ܿ�ǰ��
		{

			if (bFront)//ǰ
			{
				//true----ǰ��false----��
				lRes = SwitchScreen(true);
			} 
			else//��
			{
				//true----ǰ��false----��
				lRes = SwitchScreen(true);
			}
		}
		break;
	case BATCH_ADD_OPER:
	case MED_MAN_OPER:
		{
			if (m_bDoubleFace)//�������ţ���ǰ����
			{
				//true----ǰ��false----��
				lRes = SwitchScreen(false);
			} 
			else//ֻ��һ���ţ���ǰ
			{
				//true----ǰ��false----��
				lRes = SwitchScreen(true);
			}
		}
		break;
	default:
		break;
	}
	
	return lRes;

}


//���Ƿ��Ѿ��Ǵ�״̬�ˣ�
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
	case ADJUST_OPER:		//ֻ�ܿ�ǰ��
		{
			
			if (bFront)//ǰ
			{
				EnterCS();
				lRes = m_pFxCom->GetDevice("M24", &lFlag);
				LeaveCS();
			} 
			else//��
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
			if (m_bDoubleFace)//�������ţ���ǰ����
			{
				//����
				EnterCS();
				lRes = m_pFxCom->GetDevice("M26", &lFlag);
				LeaveCS();
			} 
			else//ֻ��һ���ţ���ǰ
			{
				//ǰ��
				EnterCS();
				lRes = m_pFxCom->GetDevice("M24", &lFlag);
				LeaveCS();
			}
		}
		break;
	default:
		break;
	}

	//PLC��ȡʧ��
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

