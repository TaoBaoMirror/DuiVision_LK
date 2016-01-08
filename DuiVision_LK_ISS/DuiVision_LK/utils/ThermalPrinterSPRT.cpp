// ThermalPrinterSPRT.cpp: implementation of the CThermalPrinterSPRT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <assert.h>

#include <process.h>

#include "ThermalPrinterSPRT.h"
#include "General.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



/*!>  汉字模式（默认） 和 国际字符集   区别
	 打印机有一个内置的字库，储存的是国标编码的字库，这种汉字是双字节组成的，
	 每当打印机接收到对应高字节的内容就会缓存高字节和低字节，然后共同在字库内定位汉字并输出，这就是汉字模式。
	而国际字符集是替换ASCII码的128到255的内容供国际不同地区的文字输出，是单字节的范围。
	这两者有冲突，因为汉字模式的高字节有部分是跟国际字符集有重叠，这样在输出时就容易乱码，
	所以这两者是互斥的，就是说在使用汉字模式的时候不要打印国际字符集，
	要打印国际字符集的时候不要使用汉字模式，关掉它。
	打印机一上电就是接收双字节模式，也就是开了汉字模式的，不需要特意去调整，
	这就是供国内用户使用的打印机。而国际的用户我们也考虑到了他们不使用汉字，
	所以我们做了一个硬件开关，在机器底部的拨位开关的第4位，一般出国的机器都拨4位永久取消汉字模式。
 */

/*!> 脱机状态 --- 具体参加开发手册 Page4 */
typedef struct tagOfflineState
{
	union UNState
	{
		struct State
		{
			unsigned char ucRemain:2;/*!> 固定为2(10)，无参考价值 */
			unsigned char ucOpen:1;/*!> 0：上盖关，1：上盖已开(正常下：不能打开) */
			unsigned char ucPress:1;/*!> 0：未按走纸键，1：按下走纸键 */
			unsigned char ucOther:1;/*!> 固定为1，无参考价值 */
			unsigned char ucNoPaper:1;/*!> 0：不缺纸，1：缺纸 */
			unsigned char ucError:1;/*!> 0：没有出错情况，1：有错误情况 */
			unsigned char ucHead:1;/*!> 固定为0，无参考价值 */
		}v;
		
		unsigned char ucState;
	}s;
}OfflineState;

/*!> 错误状态 --- 具体参加开发手册 Page5  */
typedef struct tagErrorState
{
	union UNState
	{
		struct State
		{
			unsigned char ucRemain:2;/*!> 固定为2(10)，无参考价值 */
			unsigned char ucUndifined:1;/*!> 暂无定义，未知数 */
			unsigned char ucCutterError:1;/*!> 0：切刀无错误，1：切刀有错误 */
			unsigned char ucOther:1;/*!> 固定为1，无参考价值 */
			unsigned char ucUnrecoverableError:1;/*!> 0：无不可恢复错误，1：有不可恢复错误(切刀卡住弹不回来) */
			unsigned char ucRecoverableError:1;/*!> 0：没有自动恢复错误，1：有自动恢复错误出现 */
			unsigned char ucHead:1;/*!> 固定为0，无参考价值 */
		}v;
		
		unsigned char ucState;
	}s;
}ErrorState;


/*!> 纸状态 --- 具体参加开发手册 Page5  */
typedef struct tagPaperState
{
	union UNState
	{
		struct State
		{
			unsigned char ucRemain:2;/*!> 固定为2(10)，无参考价值 */
			unsigned char ucBeExhausting:2;/*!> 0：有纸，3：纸将尽 */
			unsigned char ucOther:1;/*!> 固定为1，无参考价值 */
			unsigned char ucExhausted:2;/*!> 0：有纸，3：纸尽 */
			unsigned char ucHead:1;/*!> 固定为0，无参考价值 */
		}v;
		
		unsigned char ucState;
	}s;
}PaperState;

/*!> 
	 CODE128:  2 ≤ 条码内容 ≤ 255
	 命令类型 + 条码类型 + 长度 + 字符集 + 条码内容(最长255字节)
	    2     +    1    +    1    +   2  +          255				= 261
		             6            +          255					= 261
		266 - 261 = 5，留5个保留字节（其中有一个字节做字符串结束符）
 */
static unsigned char g_szBarcode_128_A_ContextBuf[266] = {0x1D, 0x6B, 0x49, 0x00, 0x7B, 0x41};/*!> 后面260个字节为内容 */

static unsigned char g_szBarcodeHeight[] = {0x1D, 0x68, 0x00};/*!> 选择条码高度，1<= 高度 <=255 */
static unsigned char g_szBarcodeWidth[] = {0x1D, 0x77, 0x00};/*!> 选择条码宽度，参考手册P40，2 <= 宽度 <= 6 */
static unsigned char g_szLocHRI[] = {0x1D, 0x48, 0x00}; /*!> 选择HRI的打印位置,0--不打印HRI，1--上方，2--下方，3--上下都打印HRI */

static unsigned char g_szUnderline[] = {0x1B, 0x2D, 0x00};/*!> 选择/取消下划线模式, 0--取消，1--1点宽，2--2点宽 */

static unsigned char g_szHT[] = {0x09};/*!> 水平定位，移动打印位置到下一个水平定位点 */
static unsigned char g_szLF[] = {0x0A};/*!> 打印 + 换行 */
static unsigned char g_szSetPos[] = {0x1B, 0x24, 0x00, 0x00}; /*!> 设置绝对打印位置 */
static unsigned char g_szStartStop[] = {0x1B, 0x3D, 0x00};/*!> 启停：打印机是否接受PC发送的数据，0--禁止，1--允许(默认) */
static unsigned char g_szRenew[] ={0x1B, 0x40};/*!> 初始化打印机：清除打印缓冲区数据，打印模式被设为上电时的默认值模式。 */
static unsigned char g_szOverstriking[] = {0x1B, 0x45, 0x00};/*!> 选择/取消加粗，0--取消(默认)，1--选择 */
static unsigned char g_szFlushMove[] = {0x1B, 0x4A, 0x00};/*!> 打印走纸n个单位 */
static unsigned char g_szCharacterSets[] = {0x1B, 0x52, 0x00};/*!> 选择国际字符集，0--美国(默认)，15--中国 */
static unsigned char g_szAlign[] = {0x1B, 0x61, 0x00};/*!> 对齐方式，0--左对齐(默认)，1--中间对齐，2--右对齐 */
static unsigned char g_szPressDisable[] = {0x1B, 0x63, 0x35, 0x00};/*!> 允许/禁止按键，0--允许(默认)，1--禁止 */
static unsigned char g_szFlushSkipLine[] = {0x1B, 0x64, 0x00};/*!> 打印缓冲区数据，并向前走纸n行 */
static unsigned char g_szInvert[] = {0x1B, 0x7B, 0x00}; /*!> 选择/取消倒置打印，0--取消(默认)，1--选择 */
static unsigned char g_szCut[] = {0x1D, 0x56, 0x42, 0x00}; /*!> 切纸--进纸n个单位，再半切纸 */
static unsigned char g_szBuzzing[] = {0x1B, 0x42, 0x00, 0x00};/*!> 单打印蜂鸣提示，n--鸣叫次数，t--(t x 50)间隔毫秒，n,t都不得大于9*/
static unsigned char g_szExpand[] = {0x1D, 0x21, 0x00};/*!> 设置字符大小,0--正常，
										1--纵向放大2倍，2--纵向放大3倍，3--纵向放大4倍，4--纵向放大5倍；
										16--横向放大2倍，32--横向放大3倍，48--横向放大4倍，64--纵向放大5倍；
										17--放大2倍，34--放大3倍，51--放大4倍，68--放大5倍；
										目前暂定放大5倍，最大能放大8倍，参见开发手册Page27说明
									*/

static unsigned char g_szRTState[] = {0x10, 0x04, 0x00};/*!> 1--回传打印机实时状态，2--脱机状态，3--错误状态，4--纸状态 
											发送完毕，则立即返回一个字节*/
		
unsigned int __stdcall ThreadFuncForMonitorThermalPrinter(void * lp)
{

	long lRes;

	CThermalPrinterSPRT* pThermalPrinterMan = (CThermalPrinterSPRT*)lp;

	while (true)
	{
		pThermalPrinterMan->m_lDeviceFlag = pThermalPrinterMan->GetState();

		Sleep(1000);
	}

	return 0;
}

//静态成员，初始化
long CThermalPrinterSPRT::m_lDeviceFlag = 0;
/*!>  */
LoggerPtr CThermalPrinterSPRT::m_rootLoggerPtr = Logger::getLogger("CThermalPrinterSPRT");

CThermalPrinterSPRT::CThermalPrinterSPRT()
		   :m_ucBarcodeHeight(100),
			m_ucBarcodeWidth(2),
			m_ucLocHRI(2)
{
	string strCharacterSets;
	long lRes;

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT() start.............");

	m_lDeviceFlag = 0;

	InitializeCriticalSection(&m_cs);
	
	m_printerSerialParam.nPort = configPar.getIntParameter("ThermalPrinter_Port");
	m_printerSerialParam.ulBaudRate = configPar.getIntParameter("ThermalPrinter_Baudrate");
	
	m_nRowWidth = configPar.getIntParameter("rowWidth");
	
	lRes = Open();
	if (lRes)
	{
		CString cstrPort;
		cstrPort.Format(configPar.getValueParameter("Open_printer_Failed").c_str(), m_printerSerialParam.nPort);
		::AfxMessageBox(cstrPort);
	}
	else
	{
		RenewDevice();
		
		InitBarcode();

		m_nRunMode = configPar.getIntParameter("runMode"); 
		
		if (0 == m_nRunMode)
		{
			m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncForMonitorThermalPrinter, this, 0, 0);
		} 
		else
		{
			m_hThread = INVALID_HANDLE_VALUE;
		}
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT() end.............");

}


CThermalPrinterSPRT::~CThermalPrinterSPRT()
{
	m_rootLoggerPtr->trace("function ~CThermalPrinterSPRT() start.............");

	if (0 == m_nRunMode)
	{
		if (INVALID_HANDLE_VALUE != m_hThread)
		{
			DWORD   dwExitCode;
			if(GetExitCodeThread( m_hThread, &dwExitCode ) )
			{
				::TerminateThread(m_hThread,dwExitCode);
				CloseHandle(m_hThread);
			}
		}
	}

	DeleteCriticalSection(&m_cs);

	m_rootLoggerPtr->trace("function ~CThermalPrinterSPRT() end.............");
}



/*!> 水平定位(空格打印) ---- 移动打印位置到下一个水平定位点的位置。 */
long CThermalPrinterSPRT::HT()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::HT() start..............");

	lRes = SendCommand(g_szHT, sizeof(g_szHT));
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::HT() end..............");
	return 0;
}


/*!> 打印 + 换行 */
long CThermalPrinterSPRT::LF()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::LF() start..............");

	lRes = SendCommand(g_szLF, sizeof(g_szLF));
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::LF() end..............");
	return 0;
}


long CThermalPrinterSPRT::SetPos(int nX, int nY)
{
	long lRes;
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetPos() start..............");
	
	g_szSetPos[2] = nX;
	g_szSetPos[3] = nY;
	
	lRes = SendCommand(g_szSetPos, sizeof(g_szSetPos));
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}
	
	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetPos() end..............");
	return 0;
}

/*!> 初始化打印机：清除打印缓冲区数据，打印模式被设为上电时的默认值模式。 */
long CThermalPrinterSPRT::RenewDevice()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::RecoverDevice() start..............");

	EnterCS();
	lRes = SendCommand(g_szRenew, sizeof(g_szRenew));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::RecoverDevice() end..............");
	return 0;
}

/*!> 初始化-----防止人为的关闭打印机电源，再重启
	配置时，所有的参数都仅仅暂存在RAM里，每次上电重启，断电后信息丢失，会恢复为默认值
	硬件一直都在使用FLASH，如果用FLASH来储存参数就非常影响它的寿命
 */
long CThermalPrinterSPRT::Init()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Init() start..............");

	/*!> 条码设置初始化 */
	lRes = InitBarcode();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to InitBarcode(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Init() end..............");
	return 0;
}

/*!> 启停：打印机是否接受PC发送的数据，0--禁止，1--允许(默认) */
long CThermalPrinterSPRT::StartStop(bool bFlag)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::StartStop() start..............");

	if (bFlag)
	{
		g_szStartStop[2] = 1;
	}
	else
	{
		g_szStartStop[2] = 0;
	}

	EnterCS();
	lRes = SendCommand(g_szStartStop, sizeof(g_szStartStop));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::StartStop() end..............");
	return 0;
}

/*!> 打印走纸n个单位 */
long CThermalPrinterSPRT::FlushMove(unsigned char ucNums)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::FlushMove() start..............");

	g_szFlushMove[2] = ucNums;

	EnterCS();
	lRes = SendCommand(g_szFlushMove, sizeof(g_szFlushMove));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::FlushMove() end..............");
	return 0;
}

/*!> 允许/禁止按键，0--允许(默认)，1--禁止 */
long CThermalPrinterSPRT::PressDisable(bool bFlag)
{
	long lRes;

	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PressDisable() start..............");

	if(bFlag)
	{
		g_szPressDisable[3] = 1;
	}
	else
	{
		g_szPressDisable[3] = 0;
	}	

	EnterCS();
	lRes = SendCommand(g_szPressDisable, sizeof(g_szPressDisable));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PressDisable() end..............");
	return 0;
}

/*!> 打印缓冲区数据，并向前走纸n行 */
long CThermalPrinterSPRT::FlushSkipLine(unsigned char ucNums)
{
	long lRes;

	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::FlushSkipLine() start..............");


	g_szFlushSkipLine[2] = ucNums;

	EnterCS();
	lRes = SendCommand(g_szFlushSkipLine, sizeof(g_szFlushSkipLine));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::FlushSkipLine() end..............");
	return 0;
}

/*!> 选择/取消倒置打印，0--取消(默认)，1--选择 */
long CThermalPrinterSPRT::Invert(bool bFlag)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Invert() start..............");

	if (bFlag)
	{
		g_szInvert[2] = 1;
	}
	else
	{
		g_szInvert[2] = 0;
	}

	EnterCS();
	lRes = SendCommand(g_szInvert, sizeof(g_szInvert));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Invert() end..............");
	return 0;
}

/*!> 选择/取消下划线模式, 0--取消，1--1点宽下划线，2--2点宽下划线 */
long CThermalPrinterSPRT::SetUnderline(unsigned char ucMode)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetUnderline() start..............");


	assert(3 > ucMode);

	g_szUnderline[2] = ucMode;

	EnterCS();
	lRes = SendCommand(g_szUnderline, sizeof(g_szUnderline));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetUnderline() end..............");
	return 0;
}

/*!> 设置国际字符集，true--汉字(老肯默认)，false--美国(设备默认) */
long CThermalPrinterSPRT::SetCharacterSets(bool bFlag)
{
	long lRes;
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetCharacterSets() start..............");


	if (bFlag)
	{
		g_szCharacterSets[2] = 15;/*!> 汉字 15*/
	}
	else
	{
		g_szCharacterSets[2] = 0;/*!> 美国 0*/
	}

	EnterCS();
	lRes = SendCommand(g_szCharacterSets, sizeof(g_szCharacterSets));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetCharacterSets() end..............");
	return 0;
}

/*!> 设置对齐方式 */
long CThermalPrinterSPRT::SetAlignMode(unsigned char ucMode)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetAlignMode() start..............");

	assert(3 > ucMode);

	g_szAlign[2] = ucMode;/*!> 对齐方式，0--左对齐(默认)，1--中间对齐，2--右对齐 */

	EnterCS();
	lRes = SendCommand(g_szAlign, sizeof(g_szAlign));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetAlignMode() end..............");
	return 0;
}

/*!> 选择/取消加粗，默认取消 */
long CThermalPrinterSPRT::SetOverstriking(bool bFlag)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetOverstriking() start..............");

	/*!> 选择/取消加粗，0--取消(默认)，1--选择 */
	if (bFlag)
	{
		g_szOverstriking[2] = 1;
	}
	else
	{
		g_szOverstriking[2] = 0;
	}

	EnterCS();
	lRes = SendCommand(g_szOverstriking, sizeof(g_szOverstriking));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetOverstriking() end..............");
	return 0;
}

/*!> 字符横向放大 */
long CThermalPrinterSPRT::SetHorizontalExpand(unsigned char ucTimes)
{
	long lRes;
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetHorizontalExpand() start .............");

	/*!> 目前暂只放大到5倍 */
	assert(1 < ucTimes && 6 > ucTimes);

	/*!> 设置字符大小,0--正常，
		1--纵向放大2倍，2--纵向放大3倍，3--纵向放大4倍，4--纵向放大5倍；
		16--横向放大2倍，32--横向放大3倍，48--横向放大4倍，64--纵向放大5倍；
		17--放大2倍，34--放大3倍，51--放大4倍，68--放大5倍；
		目前暂定放大5倍，最大能放大8倍，参见开发手册Page27说明
	*/
	switch(ucTimes)
	{
	case 2:
		g_szExpand[2] = 1;
		break;
	case 3:
		g_szExpand[2] = 2;
		break;
	case 4:
		g_szExpand[2] = 3;
		break;
	case 5:
		g_szExpand[2] = 4;
		break;
	default:
		break;

	}
	
	EnterCS();
	lRes = SendCommand(g_szExpand, sizeof(g_szExpand));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetHorizontalExpand() end .............");

	return 0;
}

/*!> 字符纵向放大 */
long CThermalPrinterSPRT::SetVerticalExpand(unsigned char ucTimes)
{
	long lRes;
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetVerticalExpand() start .............");


	/*!> 目前暂只放大到2--5倍 */
	assert(1 < ucTimes && 6 > ucTimes);

	/*!> 设置字符大小,0--正常，
		1--纵向放大2倍，2--纵向放大3倍，3--纵向放大4倍，4--纵向放大5倍；
		16--横向放大2倍，32--横向放大3倍，48--横向放大4倍，64--纵向放大5倍；
		17--放大2倍，34--放大3倍，51--放大4倍，68--放大5倍；
		目前暂定放大5倍，最大能放大8倍，参见开发手册Page27说明
	*/
	switch(ucTimes)
	{
	case 2:
		g_szExpand[2] = 16;
		break;
	case 3:
		g_szExpand[2] = 32;
		break;
	case 4:
		g_szExpand[2] = 48;
		break;
	case 5:
		g_szExpand[2] = 64;
		break;
	default:
		break;

	}
	
	EnterCS();
	lRes = SendCommand(g_szExpand, sizeof(g_szExpand));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetVerticalExpand() end .............");

	return 0;
}

/*!> 字符整体放大 */
long CThermalPrinterSPRT::SetExpand(unsigned char ucTimes)
{
	long lRes;
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetExpand() start .............");

	/*!> 目前暂只放大到2--5倍 */
	assert(1 < ucTimes && 6 > ucTimes);

	/*!> 设置字符大小,0--正常，
		1--纵向放大2倍，2--纵向放大3倍，3--纵向放大4倍，4--纵向放大5倍；
		16--横向放大2倍，32--横向放大3倍，48--横向放大4倍，64--纵向放大5倍；
		17--放大2倍，34--放大3倍，51--放大4倍，68--放大5倍；
		目前暂定放大5倍，最大能放大8倍，参见开发手册Page27说明
	*/
	switch(ucTimes)
	{
	case 2:
		g_szExpand[2] = 17;
		break;
	case 3:
		g_szExpand[2] = 34;
		break;
	case 4:
		g_szExpand[2] = 51;
		break;
	case 5:
		g_szExpand[2] = 68;
		break;
	default:
		break;

	}
	EnterCS();
	lRes = SendCommand(g_szExpand, sizeof(g_szExpand));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetExpand() end .............");

	return 0;
}

/*!> 恢复字符正常大小 */
long CThermalPrinterSPRT::SetNormal()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetNormal() start .............");

	
	/*!> 设置字符大小,0--正常
	*/
	g_szExpand[2] = 0;
	
	EnterCS();
	lRes = SendCommand(g_szExpand, sizeof(g_szExpand));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetNormal() end .............");

	return 0;
}

/*!> 单打印蜂鸣提示 */
long CThermalPrinterSPRT::Buzzing(unsigned char ucTimes, unsigned char ucTimeInter)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	/*!> 鸣叫次数, 间隔毫秒 都不得大于9 */
	assert(10 > ucTimes && 10 > ucTimeInter);
	
	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Buzzing() start .............");
	
	/*!> 单打印蜂鸣提示，nTimes--鸣叫次数，nTimeInter--(nTimeInter x 50)间隔毫秒，nTimes,nTimeInter都不得大于9*/
	g_szBuzzing[2] = ucTimes;
	g_szBuzzing[3] = ucTimeInter;

	EnterCS();
	lRes = SendCommand(g_szBuzzing, sizeof(g_szBuzzing));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Buzzing() end .............");

	return 0;
}

/*!> 走纸 + 切纸 */
long CThermalPrinterSPRT::CutPaper(unsigned char ucOffset)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::CutPaper() start .............");

	lRes = getDeviceRTFlag();
	if (lRes)
	{
		return lRes;
	}
	
	/*!> 切纸--进纸nOffset个单位，再半切纸 */
	g_szCut[3] = ucOffset;

	EnterCS();
	lRes = SendCommand(g_szCut, sizeof(g_szCut));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::CutPaper() end .............");

	return 0;
}

/*!> 选择条码高度，1<= 高度 <=255 */
long CThermalPrinterSPRT::SetBarcodeHeight(unsigned char ucHeight)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetBarcodeHeight() start .............");
		
	g_szBarcodeHeight[2] = ucHeight;

	EnterCS();
	lRes = SendCommand(g_szBarcodeHeight, sizeof(g_szBarcodeHeight));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetBarcodeHeight() end .............");

	return 0;
}

/*!> 选择条码宽度，参考手册P40，2 <= 宽度 <= 6 */
long CThermalPrinterSPRT::SetBarcodeWidth(unsigned char ucWidth)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetBarcodeWidth() start .............");
	
	assert(1 < ucWidth && 7 > ucWidth);
	
	g_szBarcodeWidth[2] = ucWidth;/*!> 默认为3 */
	
	EnterCS();
	lRes = SendCommand(g_szBarcodeWidth, sizeof(g_szBarcodeWidth));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetBarcodeWidth() end .............");

	return 0;
}

/*!> 选择HRI的打印位置,0--不打印HRI，1--上方，2--下方，3--上下都打印HRI */
long CThermalPrinterSPRT::SetLocHRI(unsigned char ucLoc)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetLocHRI() start .............");
	
	assert(4 > ucLoc);

	g_szLocHRI[2] = ucLoc;

	EnterCS();
	lRes = SendCommand(g_szLocHRI, sizeof(g_szLocHRI));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetLocHRI() end .............");

	return 0;
}


/*!> 
	防止每次上电重启时，条码的高度、宽度、HRI位置的配置 恢复了默认值
	配置参数时，所有的参数都是暂存在RAM里，没有写入到ROM和flash里面，
	硬件一直都在使用FLASH，如果用FLASH来储存参数就非常影响它的寿命
 */
long CThermalPrinterSPRT::InitBarcode()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::InitBarcode() start .............");

	/*!> 设置条码的高度 */
	lRes = SetBarcodeHeight(m_ucBarcodeHeight);
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SetBarcodeHeight(), code:" + General::integertostring(lRes));
		return lRes;
	}

	/*!> 设置条码的宽度 */
	lRes = SetBarcodeWidth(m_ucBarcodeWidth);
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SetBarcodeWidth(), code:" + General::integertostring(lRes));
		return lRes;
	}

	/*!> 设置HRI的显示位置 */
	lRes = SetLocHRI(m_ucLocHRI);
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SetLocHRI(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::InitBarcode() end .............");

	return 0;
}


/*!> 128码 A类字符集 打印条码 */
long CThermalPrinterSPRT::PrintBarcode_128_A(const string &strBarcode)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintBarcode_128_A() start .............");

	lRes = getDeviceRTFlag();
	if (lRes)
	{
		return lRes;
	}

	/*!> CODE128:  2 ≤ 条码内容 ≤ 255 */
	assert(1 < strBarcode.length() && 256 > strBarcode.length());
	
	/*!> 清空条码内容 */
	memset(g_szBarcode_128_A_ContextBuf + 6, 0, sizeof(g_szBarcode_128_A_ContextBuf) - 6);

	/*!> 拷贝条码内容 */
	memcpy(g_szBarcode_128_A_ContextBuf + 6, strBarcode.c_str(), strBarcode.length());

	/*!> 长度 = 字符集2个字节 + 内容长度 */
	g_szBarcode_128_A_ContextBuf[3] = strBarcode.length() + 2;

	/*!>  */
	EnterCS();
	lRes = SendCommand(g_szBarcode_128_A_ContextBuf, strlen((const char *)g_szBarcode_128_A_ContextBuf));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call (), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintBarcode_128_A() end .............");

	return 0;
}


/*!> 打印条码 */
long CThermalPrinterSPRT::PrintBarcode(const string &strBarcode)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintBarcode() start .............");

	/*!> 目前采用 128码 A类字符集 打印条码 */
	lRes = PrintBarcode_128_A(strBarcode);
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call PrintBarcode_128_A(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintBarcode() end .............");

	return 0;
}


/*!> 获取打印机的实时状态 */
long CThermalPrinterSPRT::GetState()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	//OfflineState os;
	ErrorState es;
	PaperState ps;

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::GetState() start .............");
	
	/*!> 回传打印机实时状态
		2--脱机状态，3--错误状态，4--纸状态 
		3--错误状态，可检测到：切刀有错误；打印温度过高。
		4--纸状态：可检测到：纸尽；纸将尽；
		发送完毕，则立即返回一个字节
	*/
	g_szRTState[2] = 4;

	EnterCS();
	lRes = SendCommand(g_szRTState, sizeof(g_szRTState));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to Send GetState, code:" + General::integertostring(lRes));
		return lRes;
	}

	/*!> 
		延时接受20ms，硬件没那么快的速度将数据，发送到串口接收缓冲区里 
		如果串口接收缓冲区里没有字节可读，强制去读取，会读取到脏的数据。
	*/
	Sleep(20);
	lRes = RecvData((unsigned char*)&ps, sizeof(ps));
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to RecvData(), code:" + General::integertostring(lRes));
		return lRes;
	}

	/*!> 纸尽 */
	if (2 == ps.s.v.ucExhausted)
	{
		return ER_NO_PAPER;
	}

	/*!> 纸将尽 */
	if (2 == ps.s.v.ucBeExhausting)
	{
		return ER_LESS_PAPER;
	}

	/*!> 回传打印机实时状态
		2--脱机状态，3--错误状态，4--纸状态 
		3--错误状态，可检测到：切刀有错误；打印温度过高。
		4--纸状态：可检测到：纸尽；纸将尽。
		发送完毕，则立即返回一个字节
	*/
	g_szRTState[2] = 3;
	EnterCS();
	lRes = SendCommand(g_szRTState, sizeof(g_szRTState));
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to Send GetState, code:" + General::integertostring(lRes));
		
		return lRes;
	}

	/*!> 
		延时接受20ms，硬件没那么快的速度将数据，发送到串口接收缓冲区里 
		如果串口接收缓冲区里没有字节可读，强制去读取，会读取到脏的数据。
	*/
	Sleep(20);
	lRes = RecvData((unsigned char*)&es, sizeof(es));
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to RecvData(), code:" + General::integertostring(lRes));
		return lRes;
	}
	
	/*!> 切刀错误 */
	if (1 == es.s.v.ucCutterError)
	{
		return ER_CUT;
	}
	
	/*!> 有不可恢复的错误出现 */
	if (1 == es.s.v.ucUnrecoverableError)
	{
		return ER_CUT;
	}
	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::GetState() end .............");

	return 0;
}

/*!> 打印一行并换行 */
long CThermalPrinterSPRT::PrintRow(const string &strText)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}
	
	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintRow() start .............");

	lRes = getDeviceRTFlag();
	if (lRes)
	{
		return lRes;
	}

	EnterCS();
	lRes = SendCommand((const unsigned char*)strText.c_str(), strText.length());
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}
	
	/*!> 打印字符串时，必须要末尾加上'\0' */
	lRes = LF();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call LF(), code:" + General::integertostring(lRes));
		return lRes;
	}
	
	
	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintRow() start .............");
	
	return 0;
}

long CThermalPrinterSPRT::PrintString(const string &strText, int nPos)
{
	long lRes;
	int nLen;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintString() start .............");

	lRes = getDeviceRTFlag();
	if (lRes)
	{
		return lRes;
	}

	nLen = strText.length();

	

	if (0 != nPos)
	{
		unsigned int nDots = 12*nPos;
		unsigned char a = nDots % 256;
		unsigned char b = nDots / 256;  
		SetPos(a, b);
	}
	EnterCS();
	lRes = SendCommand((const unsigned char*)strText.c_str(), strText.length());
	LeaveCS();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SendCommand(), code:" + General::integertostring(lRes));
		return lRes;
	}

	
	
	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintString() start .............");
	
	return 0;
}

