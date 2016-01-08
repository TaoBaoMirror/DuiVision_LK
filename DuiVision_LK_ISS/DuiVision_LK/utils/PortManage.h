#if !defined(PortManage_define)
#define PortManage_define

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_	VER > 1000
// PortManage.h : header file
//

#include "Serialport.h"
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include <string>

#include "KoKoonDBManage.h"
#include "SingletonMode.h"
#include "General.h"
//#include "TCPClient.h"
//#include "ThermalPrinter.h"

using namespace std;
using namespace log4cxx;

struct PortPara
{
	//��ʼ������������(����)��Ϣ��
	int port;		//���ں�
	unsigned long baudRate;	//������
    PortPara& operator=(const PortPara &ptPara)
    {
        this->port = ptPara.port;
        this->baudRate = ptPara.baudRate;
        return *this;
    }
};

class CPortManage
{
public:
	CPortManage();
	~CPortManage();
	long open(const PortPara &para);
	void close();
	long writeData(string strData);
	long readData(string &strData); //
    
    DWORD readByte(BYTE* pBuf, DWORD dwCount);
    long writeByte(const BYTE *command, int len);   //�����ֽ� 
    void clearData();  //��˿�����
    long getPortCount(int &cnt);        //��ȡ�˿ڽ����ֽ���
private:
	static LoggerPtr rootLogger;
	CSerialPort port;			//���ж˿�
    int runMode; 
    PortPara portPara;
};

//////////////////////////////////////////////////////////////////////////
//
/*
#pragma comment(lib, "RssInterface.lib")

extern "C" __declspec( dllimport ) int __stdcall Start(char * m_IP);
extern "C" __declspec( dllimport ) int __stdcall SendCardcmd(int basketId, int windowId);
*/

#endif