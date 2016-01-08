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

	/*!> ���� */
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

	/*!> ѡ������ַ���ѡ�� 
		 �����ַ���ģʽ �� ����ģʽ �ǻ����
		 �豸��Ĭ��ģʽ�� ����ģʽ
		 һ�㣬��Թ����û�ʱ���ú������á�
	*/
	long SetCharacterSets(bool bFlag = true);

	long SetAlignMode(unsigned char ucMode = 0);

	long SetOverstriking(bool bFlag = false);

	long SetHorizontalExpand(unsigned char ucTimes);/*!> ����ܷŴ�2-8����Ŀǰֻ���÷Ŵ�2-5�� */
	long SetVerticalExpand(unsigned char ucTimes);/*!> ����ܷŴ�2-8����Ŀǰֻ���÷Ŵ�2-5�� */
	long SetExpand(unsigned char ucTimes);/*!> ����ܷŴ�2-8����Ŀǰֻ���÷Ŵ�2-5�� */
	long SetNormal();

	long Buzzing(unsigned char ucTimes, unsigned char ucTimeInter);

	long CutPaper(unsigned char ucOffset = 12);

	/*!> ��ӡһ�в����� */
	long PrintRow(const string &strText = "");

	/*!> ��ӡ�ַ���ʱ,�������� */
	long PrintString(const string &strText, int nPos = 0);

public:
	long GetState();

	int GetRowWidth()
	{
		return m_nRowWidth;
	}
	
	//��ȡ�豸ʵʱ״̬
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

	unsigned char m_ucBarcodeHeight;/*!> ����ĸ߶�,�豸Ĭ��Ϊ162���Ͽ�����Ϊ200 */
	unsigned char m_ucBarcodeWidth;/*!> ����Ŀ�ȣ��豸Ĭ��Ϊ3���Ͽ�����Ϊ5 */
	unsigned char m_ucLocHRI;/*!> HRI����ʾλ��, �豸Ĭ��Ϊ0(����ӡ)���Ͽ�����Ϊ2(�·�) */

	HANDLE m_hThread;

public:
	int m_nRowWidth;

	static long m_lDeviceFlag;
public:

	CRITICAL_SECTION m_cs;

	int m_nRunMode;	//����ģʽ
};

#endif // !defined(AFX_THERMALPRINTERSPRT_H__97B2E35E_6D39_4E3D_B82A_7A4D54F43F8C__INCLUDED_)
