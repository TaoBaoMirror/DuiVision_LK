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

/*!> ���ڲ��� */
typedef struct tagPrinterSerialParam
{
	int nPort;

	unsigned long ulBaudRate;

}PrinterSerialParam;


/*!> ������ӡ��----ͨ�ø��� */
class CThermalPrinter
{
public:
public:
	CThermalPrinter();
	~CThermalPrinter();
public:

	/*!> ��ȡ��ӡ����ʵʱ״̬ */
	virtual long GetState();

	long Open();

protected:
	long Close();

	long SendCommand(const unsigned char *pcucCmd, int nLen);
	long RecvData(unsigned char *pucCmd, int nLen);
	//�鿴����ȡ���ڽ��ջ������ж��ٸ��ֽڵȴ����գ�
	long GetComInBuffBytes(int &nCnt);
	//���ô��ڶ�д��ʱʱ��
	void SetComTimeOut();

	

private:
	static LoggerPtr  m_rootLoggerPtr;

protected:

	PrinterSerialParam m_printerSerialParam;

	CSerialPort m_serialPort;/*!> �ײ㴮�ڹ����� */
    int m_nRunMode;	//����ģʽ 
};


#endif  /*!> __LAOKEN_THERMAL_PRINTER_H */