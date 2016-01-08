// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>


#include "DuiVision.h"
#include "utils/General.h"
// ע�����
#define REG_CONFIG_SUBKEY		_T("Software\\DuiVision_LK\\DuiVision\\Config")
#define REG_CONFIG_BKTYPE		_T("BkType")	// ��������(PIC/COLOR)
#define REG_CONFIG_BKPIC_RES	_T("BkPicRes")	// ����ͼƬ��ԴID
#define REG_CONFIG_BKPIC_FILE	_T("BkPicFile")	// ����ͼƬ�ļ���
#define REG_CONFIG_BKCOLOR		_T("BkColor")	// ������ɫ


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
#define WM_BUTTONDOWN  WM_USER+103 //��ť������Ϣ

//����ģʽ��
#define DEBUG_MODE 1
#define PLC_DEBUG_MODE			DEBUG_MODE
#define LAMP_DEBUG_MODE			DEBUG_MODE
#define BARCODE_DEBUG_MODE		DEBUG_MODE
#define THERMAL_PRINTER_MODE	DEBUG_MODE
#define CARD_READER_DEBUG_MODE	DEBUG_MODE