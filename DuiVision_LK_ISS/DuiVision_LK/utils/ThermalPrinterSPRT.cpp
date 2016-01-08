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



/*!>  ����ģʽ��Ĭ�ϣ� �� �����ַ���   ����
	 ��ӡ����һ�����õ��ֿ⣬������ǹ��������ֿ⣬���ֺ�����˫�ֽ���ɵģ�
	 ÿ����ӡ�����յ���Ӧ���ֽڵ����ݾͻỺ����ֽں͵��ֽڣ�Ȼ��ͬ���ֿ��ڶ�λ���ֲ����������Ǻ���ģʽ��
	�������ַ������滻ASCII���128��255�����ݹ����ʲ�ͬ����������������ǵ��ֽڵķ�Χ��
	�������г�ͻ����Ϊ����ģʽ�ĸ��ֽ��в����Ǹ������ַ������ص������������ʱ���������룬
	�����������ǻ���ģ�����˵��ʹ�ú���ģʽ��ʱ��Ҫ��ӡ�����ַ�����
	Ҫ��ӡ�����ַ�����ʱ��Ҫʹ�ú���ģʽ���ص�����
	��ӡ��һ�ϵ���ǽ���˫�ֽ�ģʽ��Ҳ���ǿ��˺���ģʽ�ģ�����Ҫ����ȥ������
	����ǹ������û�ʹ�õĴ�ӡ���������ʵ��û�����Ҳ���ǵ������ǲ�ʹ�ú��֣�
	������������һ��Ӳ�����أ��ڻ����ײ��Ĳ�λ���صĵ�4λ��һ������Ļ�������4λ����ȡ������ģʽ��
 */

/*!> �ѻ�״̬ --- ����μӿ����ֲ� Page4 */
typedef struct tagOfflineState
{
	union UNState
	{
		struct State
		{
			unsigned char ucRemain:2;/*!> �̶�Ϊ2(10)���޲ο���ֵ */
			unsigned char ucOpen:1;/*!> 0���ϸǹأ�1���ϸ��ѿ�(�����£����ܴ�) */
			unsigned char ucPress:1;/*!> 0��δ����ֽ����1��������ֽ�� */
			unsigned char ucOther:1;/*!> �̶�Ϊ1���޲ο���ֵ */
			unsigned char ucNoPaper:1;/*!> 0����ȱֽ��1��ȱֽ */
			unsigned char ucError:1;/*!> 0��û�г��������1���д������ */
			unsigned char ucHead:1;/*!> �̶�Ϊ0���޲ο���ֵ */
		}v;
		
		unsigned char ucState;
	}s;
}OfflineState;

/*!> ����״̬ --- ����μӿ����ֲ� Page5  */
typedef struct tagErrorState
{
	union UNState
	{
		struct State
		{
			unsigned char ucRemain:2;/*!> �̶�Ϊ2(10)���޲ο���ֵ */
			unsigned char ucUndifined:1;/*!> ���޶��壬δ֪�� */
			unsigned char ucCutterError:1;/*!> 0���е��޴���1���е��д��� */
			unsigned char ucOther:1;/*!> �̶�Ϊ1���޲ο���ֵ */
			unsigned char ucUnrecoverableError:1;/*!> 0���޲��ɻָ�����1���в��ɻָ�����(�е���ס��������) */
			unsigned char ucRecoverableError:1;/*!> 0��û���Զ��ָ�����1�����Զ��ָ�������� */
			unsigned char ucHead:1;/*!> �̶�Ϊ0���޲ο���ֵ */
		}v;
		
		unsigned char ucState;
	}s;
}ErrorState;


/*!> ֽ״̬ --- ����μӿ����ֲ� Page5  */
typedef struct tagPaperState
{
	union UNState
	{
		struct State
		{
			unsigned char ucRemain:2;/*!> �̶�Ϊ2(10)���޲ο���ֵ */
			unsigned char ucBeExhausting:2;/*!> 0����ֽ��3��ֽ���� */
			unsigned char ucOther:1;/*!> �̶�Ϊ1���޲ο���ֵ */
			unsigned char ucExhausted:2;/*!> 0����ֽ��3��ֽ�� */
			unsigned char ucHead:1;/*!> �̶�Ϊ0���޲ο���ֵ */
		}v;
		
		unsigned char ucState;
	}s;
}PaperState;

/*!> 
	 CODE128:  2 �� �������� �� 255
	 �������� + �������� + ���� + �ַ��� + ��������(�255�ֽ�)
	    2     +    1    +    1    +   2  +          255				= 261
		             6            +          255					= 261
		266 - 261 = 5����5�������ֽڣ�������һ���ֽ����ַ�����������
 */
static unsigned char g_szBarcode_128_A_ContextBuf[266] = {0x1D, 0x6B, 0x49, 0x00, 0x7B, 0x41};/*!> ����260���ֽ�Ϊ���� */

static unsigned char g_szBarcodeHeight[] = {0x1D, 0x68, 0x00};/*!> ѡ������߶ȣ�1<= �߶� <=255 */
static unsigned char g_szBarcodeWidth[] = {0x1D, 0x77, 0x00};/*!> ѡ�������ȣ��ο��ֲ�P40��2 <= ��� <= 6 */
static unsigned char g_szLocHRI[] = {0x1D, 0x48, 0x00}; /*!> ѡ��HRI�Ĵ�ӡλ��,0--����ӡHRI��1--�Ϸ���2--�·���3--���¶���ӡHRI */

static unsigned char g_szUnderline[] = {0x1B, 0x2D, 0x00};/*!> ѡ��/ȡ���»���ģʽ, 0--ȡ����1--1���2--2��� */

static unsigned char g_szHT[] = {0x09};/*!> ˮƽ��λ���ƶ���ӡλ�õ���һ��ˮƽ��λ�� */
static unsigned char g_szLF[] = {0x0A};/*!> ��ӡ + ���� */
static unsigned char g_szSetPos[] = {0x1B, 0x24, 0x00, 0x00}; /*!> ���þ��Դ�ӡλ�� */
static unsigned char g_szStartStop[] = {0x1B, 0x3D, 0x00};/*!> ��ͣ����ӡ���Ƿ����PC���͵����ݣ�0--��ֹ��1--����(Ĭ��) */
static unsigned char g_szRenew[] ={0x1B, 0x40};/*!> ��ʼ����ӡ���������ӡ���������ݣ���ӡģʽ����Ϊ�ϵ�ʱ��Ĭ��ֵģʽ�� */
static unsigned char g_szOverstriking[] = {0x1B, 0x45, 0x00};/*!> ѡ��/ȡ���Ӵ֣�0--ȡ��(Ĭ��)��1--ѡ�� */
static unsigned char g_szFlushMove[] = {0x1B, 0x4A, 0x00};/*!> ��ӡ��ֽn����λ */
static unsigned char g_szCharacterSets[] = {0x1B, 0x52, 0x00};/*!> ѡ������ַ�����0--����(Ĭ��)��15--�й� */
static unsigned char g_szAlign[] = {0x1B, 0x61, 0x00};/*!> ���뷽ʽ��0--�����(Ĭ��)��1--�м���룬2--�Ҷ��� */
static unsigned char g_szPressDisable[] = {0x1B, 0x63, 0x35, 0x00};/*!> ����/��ֹ������0--����(Ĭ��)��1--��ֹ */
static unsigned char g_szFlushSkipLine[] = {0x1B, 0x64, 0x00};/*!> ��ӡ���������ݣ�����ǰ��ֽn�� */
static unsigned char g_szInvert[] = {0x1B, 0x7B, 0x00}; /*!> ѡ��/ȡ�����ô�ӡ��0--ȡ��(Ĭ��)��1--ѡ�� */
static unsigned char g_szCut[] = {0x1D, 0x56, 0x42, 0x00}; /*!> ��ֽ--��ֽn����λ���ٰ���ֽ */
static unsigned char g_szBuzzing[] = {0x1B, 0x42, 0x00, 0x00};/*!> ����ӡ������ʾ��n--���д�����t--(t x 50)������룬n,t�����ô���9*/
static unsigned char g_szExpand[] = {0x1D, 0x21, 0x00};/*!> �����ַ���С,0--������
										1--����Ŵ�2����2--����Ŵ�3����3--����Ŵ�4����4--����Ŵ�5����
										16--����Ŵ�2����32--����Ŵ�3����48--����Ŵ�4����64--����Ŵ�5����
										17--�Ŵ�2����34--�Ŵ�3����51--�Ŵ�4����68--�Ŵ�5����
										Ŀǰ�ݶ��Ŵ�5��������ܷŴ�8�����μ������ֲ�Page27˵��
									*/

static unsigned char g_szRTState[] = {0x10, 0x04, 0x00};/*!> 1--�ش���ӡ��ʵʱ״̬��2--�ѻ�״̬��3--����״̬��4--ֽ״̬ 
											������ϣ�����������һ���ֽ�*/
		
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

//��̬��Ա����ʼ��
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



/*!> ˮƽ��λ(�ո��ӡ) ---- �ƶ���ӡλ�õ���һ��ˮƽ��λ���λ�á� */
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


/*!> ��ӡ + ���� */
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

/*!> ��ʼ����ӡ���������ӡ���������ݣ���ӡģʽ����Ϊ�ϵ�ʱ��Ĭ��ֵģʽ�� */
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

/*!> ��ʼ��-----��ֹ��Ϊ�Ĺرմ�ӡ����Դ��������
	����ʱ�����еĲ����������ݴ���RAM�ÿ���ϵ��������ϵ����Ϣ��ʧ����ָ�ΪĬ��ֵ
	Ӳ��һֱ����ʹ��FLASH�������FLASH����������ͷǳ�Ӱ����������
 */
long CThermalPrinterSPRT::Init()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Init() start..............");

	/*!> �������ó�ʼ�� */
	lRes = InitBarcode();
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to InitBarcode(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Init() end..............");
	return 0;
}

/*!> ��ͣ����ӡ���Ƿ����PC���͵����ݣ�0--��ֹ��1--����(Ĭ��) */
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

/*!> ��ӡ��ֽn����λ */
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

/*!> ����/��ֹ������0--����(Ĭ��)��1--��ֹ */
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

/*!> ��ӡ���������ݣ�����ǰ��ֽn�� */
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

/*!> ѡ��/ȡ�����ô�ӡ��0--ȡ��(Ĭ��)��1--ѡ�� */
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

/*!> ѡ��/ȡ���»���ģʽ, 0--ȡ����1--1����»��ߣ�2--2����»��� */
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

/*!> ���ù����ַ�����true--����(�Ͽ�Ĭ��)��false--����(�豸Ĭ��) */
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
		g_szCharacterSets[2] = 15;/*!> ���� 15*/
	}
	else
	{
		g_szCharacterSets[2] = 0;/*!> ���� 0*/
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

/*!> ���ö��뷽ʽ */
long CThermalPrinterSPRT::SetAlignMode(unsigned char ucMode)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetAlignMode() start..............");

	assert(3 > ucMode);

	g_szAlign[2] = ucMode;/*!> ���뷽ʽ��0--�����(Ĭ��)��1--�м���룬2--�Ҷ��� */

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

/*!> ѡ��/ȡ���Ӵ֣�Ĭ��ȡ�� */
long CThermalPrinterSPRT::SetOverstriking(bool bFlag)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetOverstriking() start..............");

	/*!> ѡ��/ȡ���Ӵ֣�0--ȡ��(Ĭ��)��1--ѡ�� */
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

/*!> �ַ�����Ŵ� */
long CThermalPrinterSPRT::SetHorizontalExpand(unsigned char ucTimes)
{
	long lRes;
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetHorizontalExpand() start .............");

	/*!> Ŀǰ��ֻ�Ŵ�5�� */
	assert(1 < ucTimes && 6 > ucTimes);

	/*!> �����ַ���С,0--������
		1--����Ŵ�2����2--����Ŵ�3����3--����Ŵ�4����4--����Ŵ�5����
		16--����Ŵ�2����32--����Ŵ�3����48--����Ŵ�4����64--����Ŵ�5����
		17--�Ŵ�2����34--�Ŵ�3����51--�Ŵ�4����68--�Ŵ�5����
		Ŀǰ�ݶ��Ŵ�5��������ܷŴ�8�����μ������ֲ�Page27˵��
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

/*!> �ַ�����Ŵ� */
long CThermalPrinterSPRT::SetVerticalExpand(unsigned char ucTimes)
{
	long lRes;
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetVerticalExpand() start .............");


	/*!> Ŀǰ��ֻ�Ŵ�2--5�� */
	assert(1 < ucTimes && 6 > ucTimes);

	/*!> �����ַ���С,0--������
		1--����Ŵ�2����2--����Ŵ�3����3--����Ŵ�4����4--����Ŵ�5����
		16--����Ŵ�2����32--����Ŵ�3����48--����Ŵ�4����64--����Ŵ�5����
		17--�Ŵ�2����34--�Ŵ�3����51--�Ŵ�4����68--�Ŵ�5����
		Ŀǰ�ݶ��Ŵ�5��������ܷŴ�8�����μ������ֲ�Page27˵��
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

/*!> �ַ�����Ŵ� */
long CThermalPrinterSPRT::SetExpand(unsigned char ucTimes)
{
	long lRes;
	
	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetExpand() start .............");

	/*!> Ŀǰ��ֻ�Ŵ�2--5�� */
	assert(1 < ucTimes && 6 > ucTimes);

	/*!> �����ַ���С,0--������
		1--����Ŵ�2����2--����Ŵ�3����3--����Ŵ�4����4--����Ŵ�5����
		16--����Ŵ�2����32--����Ŵ�3����48--����Ŵ�4����64--����Ŵ�5����
		17--�Ŵ�2����34--�Ŵ�3����51--�Ŵ�4����68--�Ŵ�5����
		Ŀǰ�ݶ��Ŵ�5��������ܷŴ�8�����μ������ֲ�Page27˵��
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

/*!> �ָ��ַ�������С */
long CThermalPrinterSPRT::SetNormal()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetNormal() start .............");

	
	/*!> �����ַ���С,0--����
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

/*!> ����ӡ������ʾ */
long CThermalPrinterSPRT::Buzzing(unsigned char ucTimes, unsigned char ucTimeInter)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	/*!> ���д���, ������� �����ô���9 */
	assert(10 > ucTimes && 10 > ucTimeInter);
	
	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::Buzzing() start .............");
	
	/*!> ����ӡ������ʾ��nTimes--���д�����nTimeInter--(nTimeInter x 50)������룬nTimes,nTimeInter�����ô���9*/
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

/*!> ��ֽ + ��ֽ */
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
	
	/*!> ��ֽ--��ֽnOffset����λ���ٰ���ֽ */
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

/*!> ѡ������߶ȣ�1<= �߶� <=255 */
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

/*!> ѡ�������ȣ��ο��ֲ�P40��2 <= ��� <= 6 */
long CThermalPrinterSPRT::SetBarcodeWidth(unsigned char ucWidth)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::SetBarcodeWidth() start .............");
	
	assert(1 < ucWidth && 7 > ucWidth);
	
	g_szBarcodeWidth[2] = ucWidth;/*!> Ĭ��Ϊ3 */
	
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

/*!> ѡ��HRI�Ĵ�ӡλ��,0--����ӡHRI��1--�Ϸ���2--�·���3--���¶���ӡHRI */
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
	��ֹÿ���ϵ�����ʱ������ĸ߶ȡ���ȡ�HRIλ�õ����� �ָ���Ĭ��ֵ
	���ò���ʱ�����еĲ��������ݴ���RAM�û��д�뵽ROM��flash���棬
	Ӳ��һֱ����ʹ��FLASH�������FLASH����������ͷǳ�Ӱ����������
 */
long CThermalPrinterSPRT::InitBarcode()
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::InitBarcode() start .............");

	/*!> ��������ĸ߶� */
	lRes = SetBarcodeHeight(m_ucBarcodeHeight);
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SetBarcodeHeight(), code:" + General::integertostring(lRes));
		return lRes;
	}

	/*!> ��������Ŀ�� */
	lRes = SetBarcodeWidth(m_ucBarcodeWidth);
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SetBarcodeWidth(), code:" + General::integertostring(lRes));
		return lRes;
	}

	/*!> ����HRI����ʾλ�� */
	lRes = SetLocHRI(m_ucLocHRI);
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call SetLocHRI(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::InitBarcode() end .............");

	return 0;
}


/*!> 128�� A���ַ��� ��ӡ���� */
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

	/*!> CODE128:  2 �� �������� �� 255 */
	assert(1 < strBarcode.length() && 256 > strBarcode.length());
	
	/*!> ����������� */
	memset(g_szBarcode_128_A_ContextBuf + 6, 0, sizeof(g_szBarcode_128_A_ContextBuf) - 6);

	/*!> ������������ */
	memcpy(g_szBarcode_128_A_ContextBuf + 6, strBarcode.c_str(), strBarcode.length());

	/*!> ���� = �ַ���2���ֽ� + ���ݳ��� */
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


/*!> ��ӡ���� */
long CThermalPrinterSPRT::PrintBarcode(const string &strBarcode)
{
	long lRes;

	if (m_nRunMode == 1 || m_nRunMode == 9)
	{
		return 0;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintBarcode() start .............");

	/*!> Ŀǰ���� 128�� A���ַ��� ��ӡ���� */
	lRes = PrintBarcode_128_A(strBarcode);
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to call PrintBarcode_128_A(), code:" + General::integertostring(lRes));
		return lRes;
	}

	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::PrintBarcode() end .............");

	return 0;
}


/*!> ��ȡ��ӡ����ʵʱ״̬ */
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
	
	/*!> �ش���ӡ��ʵʱ״̬
		2--�ѻ�״̬��3--����״̬��4--ֽ״̬ 
		3--����״̬���ɼ�⵽���е��д��󣻴�ӡ�¶ȹ��ߡ�
		4--ֽ״̬���ɼ�⵽��ֽ����ֽ������
		������ϣ�����������һ���ֽ�
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
		��ʱ����20ms��Ӳ��û��ô����ٶȽ����ݣ����͵����ڽ��ջ������� 
		������ڽ��ջ�������û���ֽڿɶ���ǿ��ȥ��ȡ�����ȡ��������ݡ�
	*/
	Sleep(20);
	lRes = RecvData((unsigned char*)&ps, sizeof(ps));
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to RecvData(), code:" + General::integertostring(lRes));
		return lRes;
	}

	/*!> ֽ�� */
	if (2 == ps.s.v.ucExhausted)
	{
		return ER_NO_PAPER;
	}

	/*!> ֽ���� */
	if (2 == ps.s.v.ucBeExhausting)
	{
		return ER_LESS_PAPER;
	}

	/*!> �ش���ӡ��ʵʱ״̬
		2--�ѻ�״̬��3--����״̬��4--ֽ״̬ 
		3--����״̬���ɼ�⵽���е��д��󣻴�ӡ�¶ȹ��ߡ�
		4--ֽ״̬���ɼ�⵽��ֽ����ֽ������
		������ϣ�����������һ���ֽ�
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
		��ʱ����20ms��Ӳ��û��ô����ٶȽ����ݣ����͵����ڽ��ջ������� 
		������ڽ��ջ�������û���ֽڿɶ���ǿ��ȥ��ȡ�����ȡ��������ݡ�
	*/
	Sleep(20);
	lRes = RecvData((unsigned char*)&es, sizeof(es));
	if (lRes)
	{
		m_rootLoggerPtr->error("failed to RecvData(), code:" + General::integertostring(lRes));
		return lRes;
	}
	
	/*!> �е����� */
	if (1 == es.s.v.ucCutterError)
	{
		return ER_CUT;
	}
	
	/*!> �в��ɻָ��Ĵ������ */
	if (1 == es.s.v.ucUnrecoverableError)
	{
		return ER_CUT;
	}
	m_rootLoggerPtr->trace("function CThermalPrinterSPRT::GetState() end .............");

	return 0;
}

/*!> ��ӡһ�в����� */
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
	
	/*!> ��ӡ�ַ���ʱ������Ҫĩβ����'\0' */
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

