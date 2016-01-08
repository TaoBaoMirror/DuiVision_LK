// DuiVision_LK.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "DuiVision_LK.h"
#include "DuiHandlerMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDuiVision_LKApp

BEGIN_MESSAGE_MAP(CDuiVision_LKApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CDBWrap* SingletonMode<CDBWrap>::m_Object = NULL;
size_t SingletonMode<CDBWrap>::m_Counter = 0;
// CDuiVision_LKApp ����

CDuiVision_LKApp::CDuiVision_LKApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDuiVision_LKApp ����

CDuiVision_LKApp theApp;


// CDuiVision_LKApp ��ʼ��

BOOL CDuiVision_LKApp::InitInstance()
{
	CWinApp::InitInstance();

	AfxEnableControlContainer();
	log4cxx::PropertyConfigurator::configure("log4cxx.properties");

	m_pSysConfig = m_singletonConfig.getInstance();
	m_pSysConfig->initConfig("config\\config.xml");
	m_pSysConfig->loadConfig();
	int language = configPar.getIntParameter("language"); 
	if (language == 0)
	{
		m_pSysConfig->initConfig("config\\ErrorInfo.xml");
		m_pSysConfig->loadConfig(true);
		m_pSysConfig->initConfig("config\\UIInfo.xml");
		m_pSysConfig->loadConfig();
	}
	else if (language == 1)
	{
		m_pSysConfig->initConfig("config\\ErrorInfoEN.xml");
		m_pSysConfig->loadConfig(true);
		m_pSysConfig->initConfig("config\\UIInfoEN.xml");
		m_pSysConfig->loadConfig();
	}
	/**********************************************/
	m_pSysConfig->initConfig("config\\PlcSets.xml");
	m_pSysConfig->loadConfig();	
	m_pDBWrap = m_singleDBWrap.getInstance();

	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("DuiVision_LK"));

	// ��ʼ��DuiVision�����,����ָ������,dwLangIDΪ0��ʾ�Զ��жϵ�ǰ����
	// 11160��Ӧ�ó���ID,ÿ��DUIӦ�ó���Ӧ��ʹ�ò�ͬ��ID,ID��Ҫ���ڽ��̼�ͨ�Ŵ���������ʱ������Ӧ��
	DWORD dwLangID = 0;
	new DuiSystem(m_hInstance, dwLangID, _T("DuiVision_LK.ui"), 11160, IDD_DUIVISIONAPP_DIALOG, _T(""));

	// ����Ƿ��Ѿ��н���������
	CString strAppMutex = DuiSystem::Instance()->GetConfig(_T("appMutex"));	// �������ļ��л�ȡ����������
	if(!strAppMutex.IsEmpty())
	{
		::CreateMutex(NULL,TRUE, _T("Global\\") + strAppMutex);
		if(ERROR_ALREADY_EXISTS == GetLastError() || ERROR_ACCESS_DENIED == GetLastError())
		{
			// ��ȡ�����в���,�������Ҫ���������п���ֱ���˳�
			CString strCmd = _T("");
			if(__argc > 0)
			{
				strCmd = __targv[0];
				DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("Command line:%s"), strCmd);
			}

			// ���ͽ��̼���Ϣ(lParamΪ1��ʾ����ʾ����,appMutex��ΪӦ����,��Ϣ�������������в���)
			CString strAppName = DuiSystem::Instance()->GetConfig(_T("appMutex"));
			DuiSystem::Instance()->SendInterprocessMessage(0, DuiSystem::Instance()->GetAppID(), 1, strAppName, strCmd);

			return FALSE; // Here we quit this application
		}
	}

	// ����������
	CDlgBase* pMainDlg = DuiSystem::CreateDuiDialog(_T("dlg_main"), NULL, _T(""), TRUE);
	// ��������ע���¼��������
	CDuiHandlerMain* pHandler = new CDuiHandlerMain();
	pHandler->SetDialog(pMainDlg);
	DuiSystem::RegisterHandler(pMainDlg, pHandler);

	// ��ʼ����ʾ��Ϣ����
	DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));
	//��TheAPP�д��������
	CString strKeybord = DuiSystem::Instance()->GetConfig(_T("keybord"));
	if (strKeybord==_T("1"))
		DuiSystem::Instance()->CreateKeyBord(_T("dlg_keybord"));
	// ���շ�ģʽ�Ի��򴴽�������,����Ĭ������
	pMainDlg->Create(pMainDlg->GetIDTemplate(), NULL);
	//pMainDlg->ShowWindow(SW_HIDE);
	INT_PTR nResponse = pMainDlg->RunModalLoop();

	// ����ǰ���ģʽ�Ի�������������,ֻҪ��Ϊ���´���Ϳ���
	//INT_PTR nResponse = pMainDlg->DoModal();

	// �ͷ�DuiVision��������Դ
	DuiSystem::Release();

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
