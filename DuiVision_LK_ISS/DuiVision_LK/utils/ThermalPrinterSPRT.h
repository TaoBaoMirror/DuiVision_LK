// ThermalPrinterSPRT.h: interface for the CThermalPrinterSPRT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THERMALPRINTERSPRT_H__97B2E35E_6D39_4E3D_B82A_7A4D54F43F8C__INCLUDED_)
#define AFX_THERMALPRINTERSPRT_H__97B2E35E_6D39_4E3D_B82A_7A4D54F43F8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ThermalPrinter.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include "ErrorCodeRes.h"

#include <string>

using namespace std;
using namespace log4cxx;

typedef enum tagAlignMode
{

	ALIGN_LEFT,
	ALIGN_MID,
	ALIGN_RIGHT

}EnAlignMode;

class CThermalPrinterSPRT : public CThermalPrinter
{
public:
	CThermalPrinterSPRT();
	virtual ~CThermalPrinterSPRT();

public:

	long HT();

	long LF();

	long SetPos(int nX, int nY);

	long RenewDevice();

	long Init();

	/*!> 条码 */
	long SetBarcodeHeight(unsigned char ucHeight);
	long SetBarcodeWidth(unsigned char ucWidth);
	long SetLocHRI(unsigned char ucLoc);

	long InitBarcode();

	long PrintBarcode_128_A(const string &strBarcode);

	long PrintBarcode(const string &strBarcode);

	long StartStop(bool bFlag = true);

	long FlushMove(unsigned char ucNums);

	long PressDisable(bool bFlag = false);

	long FlushSkipLine(unsigned char ucNums);

	long Invert(bool bFlag = false);

	long SetUnderline(unsigned char ucMode);

	/*!> 选择国际字符集选择 
		 国际字符集模式 和 汉字模式 是互斥的
		 设备的默认模式是 汉字模式
		 一般，面对国内用户时，该函数不用。
	*/
	long SetCharacterSets(bool bFlag = true);

	long SetAlignMode(unsigned char ucMode = 0);

	long SetOverstriking(bool bFlag = false);

	long SetHorizontalExpand(unsigned char ucTimes);/*!> 最大能放大2-8倍，目前只设置放大到2-5倍 */
	long SetVerticalExpand(unsigned char ucTimes);/*!> 最大能放大2-8倍，目前只设置放大到2-5倍 */
	long SetExpand(unsigned char ucTimes);/*!> 最大能放大2-8倍，目前只设置放大到2-5倍 */
	long SetNormal();

	long Buzzing(unsigned char ucTimes, unsigned char ucTimeInter);

	long CutPaper(unsigned char ucOffset = 12);

	/*!> 打印一行并换行 */
	long PrintRow(const string &strText = "");

	/*!> 打印字符串时,但不换行 */
	long PrintString(const string &strText, int nPos = 0);

public:
	long GetState();

	int GetRowWidth()
	{
		return m_nRowWidth;
	}
	
	//获取设备实时状态
	inline long getDeviceRTFlag()
	{
		if (1== m_nRunMode || 9 == m_nRunMode)
		{
			return 0;
		} 

		return m_lDeviceFlag;
	}

private:

	inline void EnterCS()
	{
		EnterCriticalSection(&m_cs);
	}
	
	inline void LeaveCS()
	{
		LeaveCriticalSection(&m_cs);
	}
	
private:

	static LoggerPtr  m_rootLoggerPtr;

	unsigned char m_ucBarcodeHeight;/*!> 条码的高度,设备默认为162，老肯设置为200 */
	unsigned char m_ucBarcodeWidth;/*!> 条码的宽度，设备默认为3，老肯设置为5 */
	unsigned char m_ucLocHRI;/*!> HRI的显示位置, 设备默认为0(不打印)，老肯设置为2(下方) */

	HANDLE m_hThread;

public:
	int m_nRowWidth;

	static long m_lDeviceFlag;
public:

	CRITICAL_SECTION m_cs;

	int m_nRunMode;	//运行模式
};

#endif // !defined(AFX_THERMALPRINTERSPRT_H__97B2E35E_6D39_4E3D_B82A_7A4D54F43F8C__INCLUDED_)
