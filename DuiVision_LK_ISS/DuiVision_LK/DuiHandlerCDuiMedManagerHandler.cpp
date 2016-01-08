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

// 初始化
void CDuiHandlerCDuiMedManagerHandler::OnInit()
{
}

// DUI定时器事件处理
void CDuiHandlerCDuiMedManagerHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
