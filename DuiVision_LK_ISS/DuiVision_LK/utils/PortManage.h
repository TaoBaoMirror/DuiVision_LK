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
	//初始化参数，串口(并口)信息等
	int port;		//串口号
	unsigned long baudRate;	//波特率
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
    long writeByte(const BYTE *command, int len);   //发送字节 
    void clearData();  //清端口数据
    long getPortCount(int &cnt);        //获取端口接收字节数
private:
	static LoggerPtr rootLogger;
	CSerialPort port;			//串行端口
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