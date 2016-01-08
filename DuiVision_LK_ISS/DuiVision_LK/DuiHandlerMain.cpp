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

// ��ʼ��
void CDuiHandlerMain::OnInit()
{
	
	// ��tabҳ'����ȡҩ'ע���¼��������
	CDuiHandlerCDuiPrescriptionHandler* pDuiHandlerCDuiPrescriptionHandler = new CDuiHandlerCDuiPrescriptionHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiPrescriptionHandler, _T("tab.CDuiPrescriptionHandler"));
	pDuiHandlerCDuiPrescriptionHandler->OnInit();
	// ��tabҳ'������ҩ'ע���¼��������
	CDuiHandlerCDuiFillMedHandler* pDuiHandlerCDuiFillMedHandler = new CDuiHandlerCDuiFillMedHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiFillMedHandler, _T("tab.CDuiFillMedHandler"));
	pDuiHandlerCDuiFillMedHandler->OnInit();
	// ��tabҳ'ҩƷ����'ע���¼��������
	CDuiHandlerCDuiMedManagerHandler* pDuiHandlerCDuiMedManagerHandler = new CDuiHandlerCDuiMedManagerHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiMedManagerHandler, _T("tab.CDuiMedManagerHandler"));
	pDuiHandlerCDuiMedManagerHandler->OnInit();
	// ��tabҳ'���'ע���¼��������
	CDuiHandlerCDuiListenHandler* pDuiHandlerCDuiListenHandler = new CDuiHandlerCDuiListenHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiListenHandler, _T("tab.CDuiListenHandler"));
	pDuiHandlerCDuiListenHandler->OnInit();
	// ��tabҳ'����'ע���¼��������
	CDuiHandlerCDuiSysSetHandler* pDuiHandlerCDuiSysSetHandler = new CDuiHandlerCDuiSysSetHandler();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerCDuiSysSetHandler, _T("tab.CDuiSysSetHandler"));
	pDuiHandlerCDuiSysSetHandler->OnInit();

	// ����������ʱ��
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
}

// Ƥ����Ϣ����(ʵ��Ƥ���ı���ͻ�ȡ)
LRESULT CDuiHandlerMain::OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// ��ȡSkin����
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// ��ȡSkinֵ
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
	if(Msg == MSG_SET_SKIN_VALUE)	// ����Skinֵ
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


// DUI��ʱ���¼�����
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}

// ���̼���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �����в���,���Զ������в������д���,Ҳ����ֱ����ʾ������
	DUI_INTERPROCESS_MSG* pInterMsg = (DUI_INTERPROCESS_MSG*)lParam;
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, _T("ִ���������в���:") + strCmd);
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
