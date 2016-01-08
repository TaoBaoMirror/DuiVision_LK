// ThermalPrinterSPRT_RME3.h: interface for the ThermalPrinterSPRT_RME3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THERMALPRINTERSPRT_RME3_H__5FCD036F_4B5F_4239_94DD_B5EC2177A928__INCLUDED_)
#define AFX_THERMALPRINTERSPRT_RME3_H__5FCD036F_4B5F_4239_94DD_B5EC2177A928__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ThermalPrinterSPRT.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include "ErrorCodeRes.h"

#include <string>

using namespace std;
using namespace log4cxx;

class CThermalPrinterSPRT_RME3	: public CThermalPrinterSPRT
{
public:
	CThermalPrinterSPRT_RME3();
	virtual ~CThermalPrinterSPRT_RME3();
public:
	//打印一行下划线
	long printUnderlineRow();

private:
	static LoggerPtr  m_rootLoggerPtr;

};

#endif // !defined(AFX_THERMALPRINTERSPRT_RME3_H__5FCD036F_4B5F_4239_94DD_B5EC2177A928__INCLUDED_)
