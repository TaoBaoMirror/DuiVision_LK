#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"
#include "DuiHandlerCDuiPrescriptionHandler.h"
#include "DuiHandlerCDuiFillMedHandler.h"
#include "DuiHandlerCDuiMedManagerHandler.h"
#include "DuiHandlerCDuiListenHandler.h"
#include "DuiHandlerCDuiSysSetHandler.h"

//////////////////////////////////////////////////////////////
// CDuiHandlerMain

CDuiHandlerMain::CDuiHandlerMain(void) : CDuiHandler()
{
	m_pDlg = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerMain::~CDuiHandlerMain(void)
{
}

// 初始化
void CDuiHandlerMain::OnInit()
{
	
	// 将tab页'处方取药'注册事件处理对象
	CDuiHandlerCDuiPrescriptionHandler* pDuiHandlerCDuiPrescriptionHandler = new CDuiHandlerCDuiPrescriptionHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiPrescriptionHandler, _T("tab.CDuiPrescriptionHandler"));
	pDuiHandlerCDuiPrescriptionHandler->OnInit();
	// 将tab页'批量加药'注册事件处理对象
	CDuiHandlerCDuiFillMedHandler* pDuiHandlerCDuiFillMedHandler = new CDuiHandlerCDuiFillMedHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiFillMedHandler, _T("tab.CDuiFillMedHandler"));
	pDuiHandlerCDuiFillMedHandler->OnInit();
	// 将tab页'药品管理'注册事件处理对象
	CDuiHandlerCDuiMedManagerHandler* pDuiHandlerCDuiMedManagerHandler = new CDuiHandlerCDuiMedManagerHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiMedManagerHandler, _T("tab.CDuiMedManagerHandler"));
	pDuiHandlerCDuiMedManagerHandler->OnInit();
	// 将tab页'监控'注册事件处理对象
	CDuiHandlerCDuiListenHandler* pDuiHandlerCDuiListenHandler = new CDuiHandlerCDuiListenHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiListenHandler, _T("tab.CDuiListenHandler"));
	pDuiHandlerCDuiListenHandler->OnInit();
	// 将tab页'设置'注册事件处理对象
	CDuiHandlerCDuiSysSetHandler* pDuiHandlerCDuiSysSetHandler = new CDuiHandlerCDuiSysSetHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiSysSetHandler, _T("tab.CDuiSysSetHandler"));
	pDuiHandlerCDuiSysSetHandler->OnInit();

	// 启动动画定时器
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
}

// 皮肤消息处理(实现皮肤的保存和获取)
LRESULT CDuiHandlerMain::OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// 获取Skin类型
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// 获取Skin值
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			*(int*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES);
			return TRUE;
		}else
		if(wParam == BKTYPE_COLOR)
		{
			*(COLORREF*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR);
			return TRUE;
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			*(CString*)lParam = reg.GetStringValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE);
			return TRUE;
		}
	}else
	if(Msg == MSG_SET_SKIN_VALUE)	// 设置Skin值
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE, wParam);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES, lParam);
		}else
		if(wParam == BKTYPE_COLOR)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR, lParam);
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			CString* pstrImgFile = (CString*)lParam;
			reg.SetStringValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE, *pstrImgFile);
		}
		return TRUE;
	}
	return FALSE;
}


// DUI定时器事件处理
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}

// 进程间消息处理
LRESULT CDuiHandlerMain::OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 命令行参数,可以对命令行参数进行处理,也可以直接显示主窗口
	DUI_INTERPROCESS_MSG* pInterMsg = (DUI_INTERPROCESS_MSG*)lParam;
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, _T("执行了命令行参数:") + strCmd);
	}else
	{
		CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(_T("dlg_main"));
		if(pDlg)
		{
			pDlg->SetForegroundWindow();
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->ShowWindow(SW_SHOW);
			pDlg->BringWindowToTop();
		}
	}
	return TRUE;
}
