#include "StdAfx.h"
#include "DuiHandlerCDuiSysSetHandler.h"

//////////////////////////////////////////////////////////////
// CDuiHandlerCDuiSysSetHandler

CDuiHandlerCDuiSysSetHandler::CDuiHandlerCDuiSysSetHandler(void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerCDuiSysSetHandler::~CDuiHandlerCDuiSysSetHandler(void)
{
}

// 初始化
void CDuiHandlerCDuiSysSetHandler::OnInit()
{
}

// DUI定时器事件处理
void CDuiHandlerCDuiSysSetHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
