// DuiVisionDemo.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "XMLConfig.h"
#include "SingletonMode.h"
#include "LampMan.h"
#include "DBWrap.h"

// CDuiVision_LKApp:
// 有关此类的实现，请参阅 DuiVision_LK.cpp
//

class CDuiVision_LKApp : public CWinApp
{
public:
	CDuiVision_LKApp();
	CXMLConfig *m_pSysConfig;
	SingletonMode<CXMLConfig> m_singletonConfig;
	CDBWrap    *m_pDBWrap;
	SingletonMode<CDBWrap> m_singleDBWrap;
// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CDuiVision_LKApp theApp;