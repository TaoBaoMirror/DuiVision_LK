#include "StdAfx.h"
#include "DuiHandlerCDuiMedManagerHandler.h"

//////////////////////////////////////////////////////////////
// CDuiHandlerCDuiMedManagerHandler

CDuiHandlerCDuiMedManagerHandler::CDuiHandlerCDuiMedManagerHandler(void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerCDuiMedManagerHandler::~CDuiHandlerCDuiMedManagerHandler(void)
{
}

// ��ʼ��
void CDuiHandlerCDuiMedManagerHandler::OnInit()
{
}

// DUI��ʱ���¼�����
void CDuiHandlerCDuiMedManagerHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
