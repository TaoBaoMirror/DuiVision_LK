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

// ��ʼ��
void CDuiHandlerCDuiSysSetHandler::OnInit()
{
}

// DUI��ʱ���¼�����
void CDuiHandlerCDuiSysSetHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
