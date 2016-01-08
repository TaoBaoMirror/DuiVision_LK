// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>


#include "DuiVision.h"
#include "utils/General.h"
// 注册表定义
#define REG_CONFIG_SUBKEY		_T("Software\\DuiVision_LK\\DuiVision\\Config")
#define REG_CONFIG_BKTYPE		_T("BkType")	// 背景类型(PIC/COLOR)
#define REG_CONFIG_BKPIC_RES	_T("BkPicRes")	// 背景图片资源ID
#define REG_CONFIG_BKPIC_FILE	_T("BkPicFile")	// 背景图片文件名
#define REG_CONFIG_BKCOLOR		_T("BkColor")	// 背景颜色


#pragma comment(lib,"log4cxx.lib")


typedef enum tagSysRunState
{
	NORMAL_SYS_RUN,
	WARN_SYS_RUN,
	ERROR_SYS_RUN
}EnSysRunState;

typedef enum tagDlgType
{
	DLG_MAIN,
	DLG_PRE_FETCH,
	DLG_BATCH_ADD,
	DLG_MED_MAN,
	DLG_ADJUST
}EnDlgType;

typedef enum tagOper
{
	PRE_FETCH_OPER,
	BATCH_ADD_OPER,
	MED_MAN_OPER,
	ADJUST_OPER
}EnOper;


#define PLC_RT_STATE_MSG  (WM_USER+1)
#define PLC_MONITOR_SERVO_A_MSG	  (WM_USER+2)
#define PLC_MONITOR_SERVO_B_MSG	  (WM_USER+3)
#define PLC_MONITOR_SAFE_SENSOR_FRONT_MSG	  (WM_USER+4)
#define PLC_MONITOR_SAFE_SENSOR_BACK_MSG	  (WM_USER+5)
#define WM_LK_BARCODE	(WM_USER+6)
#define WM_LK_DLG2DLG   (WM_USER+7)
#define WM_LK_CARD_READER (WM_USER+8)
#define WM_LK_VALID      (WM_USER+9)
#define WM_LK_TASKSURE   (WM_USER+10)
#define WM_SWITCHWINDOW	WM_USER+1002
#define WM_BUTTONDOWN  WM_USER+103 //按钮按下消息

//调试模式下
#define DEBUG_MODE 1
#define PLC_DEBUG_MODE			DEBUG_MODE
#define LAMP_DEBUG_MODE			DEBUG_MODE
#define BARCODE_DEBUG_MODE		DEBUG_MODE
#define THERMAL_PRINTER_MODE	DEBUG_MODE
#define CARD_READER_DEBUG_MODE	DEBUG_MODE