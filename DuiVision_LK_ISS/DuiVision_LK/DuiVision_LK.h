// DuiVisionDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "XMLConfig.h"
#include "SingletonMode.h"
#include "LampMan.h"
#include "DBWrap.h"

// CDuiVision_LKApp:
// �йش����ʵ�֣������ DuiVision_LK.cpp
//

class CDuiVision_LKApp : public CWinApp
{
public:
	CDuiVision_LKApp();
	CXMLConfig *m_pSysConfig;
	SingletonMode<CXMLConfig> m_singletonConfig;
	CDBWrap    *m_pDBWrap;
	SingletonMode<CDBWrap> m_singleDBWrap;
// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDuiVision_LKApp theApp;