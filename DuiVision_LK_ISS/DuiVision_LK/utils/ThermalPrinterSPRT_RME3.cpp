// ThermalPrinterSPRT_RME3.cpp: implementation of the ThermalPrinterSPRT_RME3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ThermalPrinterSPRT_RME3.h"

#include "General.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



/*!>  */
LoggerPtr CThermalPrinterSPRT_RME3::m_rootLoggerPtr = Logger::getLogger("CThermalPrinterSPRT_RME3");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThermalPrinterSPRT_RME3::CThermalPrinterSPRT_RME3()
{

	
}

CThermalPrinterSPRT_RME3::~CThermalPrinterSPRT_RME3()
{

}

//打印一行下划线
long CThermalPrinterSPRT_RME3::printUnderlineRow()
{
	long lRes;
	
	m_rootLoggerPtr->trace("CThermalPrinterSPRT_RME3::printUnderlineRow start ...");
	
	for (int i = 0; i < GetRowWidth(); ++i)
	{
		lRes = PrintString("-", i);
		if (lRes)
		{
			return lRes;
		}
	}
	
	lRes = PrintRow();
	if (lRes)
	{
		return lRes;
	}

	m_rootLoggerPtr->trace("CThermalPrinterSPRT_RME3::printUnderlineRow start ...");
	
	return 0;
}
