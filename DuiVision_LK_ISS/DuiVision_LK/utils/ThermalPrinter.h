#ifndef __LAOKEN_THERMAL_PRINTER_H
#define __LAOKEN_THERMAL_PRINTER_H


#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>


#include <string>
#include <list>


#include "Serialport.h"

using namespace std;
using namespace log4cxx;

/*!> 串口参数 */
typedef struct tagPrinterSerialParam
{
	int nPort;

	unsigned long ulBaudRate;

}PrinterSerialParam;


/*!> 热敏打印机----通用父类 */
class CThermalPrinter
{
public:
public:
	CThermalPrinter();
	~CThermalPrinter();
public:

	/*!> 获取打印机的实时状态 */
	virtual long GetState();

	long Open();

protected:
	long Close();

	long SendCommand(const unsigned char *pcucCmd, int nLen);
	long RecvData(unsigned char *pucCmd, int nLen);
	//查看、获取串口接收缓冲区有多少个字节等待接收？
	long GetComInBuffBytes(int &nCnt);
	//设置串口读写超时时延
	void SetComTimeOut();

	

private:
	static LoggerPtr  m_rootLoggerPtr;

protected:

	PrinterSerialParam m_printerSerialParam;

	CSerialPort m_serialPort;/*!> 底层串口管理器 */
    int m_nRunMode;	//运行模式 
};


#endif  /*!> __LAOKEN_THERMAL_PRINTER_H */