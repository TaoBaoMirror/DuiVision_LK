/************************************************************************/
/* PortManage.cpp
/* yuzhang																
    
    2010.12.11 加对运行模式为1和9时的响应
/* 2009.12.23
/************************************************************************/

#include "stdafx.h"
#include "PortManage.h"
#include "ErrorCodeRes.h"
#include <process.h>
#include "General.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;       
#endif

/////////////////////////////////////////////////////////////////////////////
//ERROR CODE 
/*
#define ER_PORT_NUM				159001	//端口号错误
#define ER_OPEN_PORT			159002	//端口打开失败
#define ER_WRITE_DATA			159003	//写端口数据失败
#define ER_READ_DATA			159004	//读端口数据失败
#define ER_NOT_OPEN_PORT		159005	//端口没有打开

#define ER_CREATE_LISTEN_THREAD	159006	//创建监听线程失败
*/
//
/////////////////////////////////////////////////////////////////////////////

LoggerPtr CPortManage::rootLogger = Logger::getLogger("CPortManage");

CPortManage::CPortManage()
{
	rootLogger->trace("function CPortManage start..............");
	runMode = configPar.getIntParameter("runMode");
    memset(&portPara, 0, sizeof(PortPara));
	rootLogger->trace("function CPortManage end..............");
}

CPortManage::~CPortManage()
{
	rootLogger->trace("function ~CPortManage start..............");
	
	rootLogger->trace("function ~CPortManage end..............");
}

/************************************************************************/
/* 打开端口，失败返回错误码，成功返回0
/* para：打印端口参数

   2010.12.1
   设置串口通信超时时间
/* 2009.12.23
/************************************************************************/
long CPortManage::open(const PortPara &para)
{
	rootLogger->trace("function open start..............");
    if (runMode == 1 || runMode == 9)
	{
		return 0;
	}
    
	try
	{
		if (port.IsOpen())
		{
			rootLogger->trace("function open end..............");
			return 0;
		}

		if (para.port <= 0 || para.port > 255)
		{
            rootLogger->error("port error! port = " + General::integertostring(para.port));
			return ER_PORT_NUM;
		}

        portPara = para;

		port.Open(para.port, para.baudRate);
		/*port.Set0WriteTimeout();
		port.Set0ReadTimeout(); */ 
        COMMTIMEOUTS comTimeOut;
        
        // 接收时，两字符间最大的时延
        comTimeOut.ReadIntervalTimeout = 1000;
        
        // 读取每字节的超时
        comTimeOut.ReadTotalTimeoutMultiplier = 1000;
        
        // 读串口数据的固定超时
        // 总超时 = ReadTotalTimeoutMultiplier * 字节数 + ReadTotalTimeoutConstant
        comTimeOut.ReadTotalTimeoutConstant = 1000;
        
        // 写每字节的超时
        comTimeOut.WriteTotalTimeoutMultiplier = 1000;
        
        // 写串口数据的固定超时
        comTimeOut.WriteTotalTimeoutConstant = 1000;
        
        // 将超时参数写入设备控制
        port.SetTimeouts(comTimeOut); 
	}catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		rootLogger->error("failed to open port" + General::integertostring(para.port) +  ": " + tmp);
		pEx->Delete();
		return ER_OPEN_PORT;
	}	
	rootLogger->trace("function open end..............");
	return 0;
}

void CPortManage::close()
{
	rootLogger->trace("function close start..............");
    if (port.IsOpen())
    {     
       	port.Close();
    }

	rootLogger->trace("function close end..............");
}

/************************************************************************/
/* 写数据至端口，失败返回错误码，成功返回0
/* strData：写入的数据

/* 2009.12.23
/************************************************************************/
long CPortManage::writeData(string strData)
{
	rootLogger->trace("function writeData start..............");
    if (runMode == 1 || runMode == 9)
    {
        return 0;
    }
        
	try
	{
		if (!port.IsOpen())
		{
			rootLogger->error("not open the port");
			return ER_NOT_OPEN_PORT;
		}
		port.Write(strData.c_str(),strData.length());
	}catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		rootLogger->error("failed to send command: " + tmp);
		pEx->Delete();
		return ER_WRITE_DATA;
	}
	rootLogger->trace("function writeData end..............");
	return 0;
}

/************************************************************************/
/* 读端口数据,从第三个字节开始，读出的数字1-9转成ASCII码数字，失败返回错误码，成功返回0
/* strData：读出的数据


/* 2011.3.5(在原有基础上改进读取算法)
/************************************************************************/
long CPortManage::readData(string &strData)
{
	rootLogger->trace("function readData start..............");
	if (runMode == 1 || runMode == 9)
	{
		return 0;
	}
    
    try
    {
        if (!port.IsOpen())
        {
			long lRet = open(portPara);
            if (lRet)
            {
                return lRet;
            }          
        }		
		
		int count = 0;
		count = port.BytesWaiting(); //串口接收字节数
		if (count == 0)				//没有字节直接返回
		{
			strData = "";
			rootLogger->debug("count = 0");
			return 0; 
		}
		
		BYTE bt=0;
		while (port.BytesWaiting())
		{
			port.Read(&bt,1);
			if (bt == 1 && port.BytesWaiting())//读到读卡器的数据头
			{				
				BYTE *buf  = new BYTE[11];
				BYTE b;
				DWORD dwBytesRead = port.Read(buf,11);
				if (dwBytesRead != 0)
				{
					for (int i = 2; i < 11; i++)
					{
						b = buf[i];
						if (b >= '0' && b <= '9')
						{
							strData += b;
						}
					}
					rootLogger->debug("strData=" + strData);
					
					if(strData.length() < 8)
					{
						strData = "";
						rootLogger->debug("strData.length() < 8");
					}		
				}	
				delete []buf;
			}
		}                
    }catch (CSerialException* pEx)
    {
        string tmp(pEx->GetErrorMessage().GetBuffer(10));
        rootLogger->error("failed to read data: " + tmp);
        pEx->Delete();
        return ER_READ_DATA;
    }
	
	rootLogger->trace("function readData end..............");
	return 0;
}

/************************************************************************/
/* 读端口数据,从第三个字节开始，读出的数字1-9转成ASCII码数字，失败返回错误码，成功返回0
/* strData：读出的数据

  2010.11.25 
  如果串字节数小于12则返回空数据
  2010.11.18 从第三个字节开始解析串口读得的数据
  2010.11.17 修改端口数据解析BUG
/* 2009.12.24
/************************************************************************/
/*
long CPortManage::readData(string &strData)
{
	rootLogger->trace("function readData start..............");
	if (runMode == 1 || runMode == 9)
	{
		return 0;
	}
    
    try
    {
        if (!port.IsOpen())
        {
			long lRet = open(portPara);
            if (lRet)
            {
                return lRet;
            }          
        }
        
        int n;
        int count = 0;
        for (n = 0; n < 2; n++)
        {
            COMSTAT state;
            port.GetStatus(state);
            count = state.cbInQue;  //串口接收字节数
			if (count == 0)  //没有字节直接返回
			{
				strData = "";
				rootLogger->debug("count = 0");
				return 0; 
			}
            //rootLogger->debug("count=" + Utils::itos(count)); 
            if (count >= 12)
            {
                break;
            }
            Sleep(100);
        }
        if (n > 1)
        {
            strData = "";
            rootLogger->debug("count < 12");
            return 0;           
        }
        
        //if (count > 0)
        //{
        BYTE *buf  = new BYTE[12];
        BYTE b;
        DWORD dwBytesRead = port.Read(buf,12);

		COMSTAT state;
		port.GetStatus(state);
		count = state.cbInQue;  //串口接收字节数

		if (dwBytesRead == 0)
		{
			rootLogger->debug("dwBytesRead = 0");
			strData = "";
			return 0;
		}
		
        for (int i = 3; i < 11; i++)
        {
            b = buf[i];
            if (b >= '0' && b <= '9')
            {
                strData += b;
            }
        }
        rootLogger->debug("strData=" + strData);
        delete []buf;
        if(strData.length() < 8)
        {
            strData = "";
            rootLogger->debug("strData.length() < 8");
        }
        //}	
    }catch (CSerialException* pEx)
    {
        string tmp(pEx->GetErrorMessage().GetBuffer(10));
        rootLogger->error("failed to read data: " + tmp);
        pEx->Delete();
        return ER_READ_DATA;
    }
	
	rootLogger->trace("function readData end..............");
	return 0;
}
*/

/************************************************************************/
/* 读端口字节,返回读取的字节数
/* pBuf: 读出的字节
   dwCount: 预计读出的字节数

   2010.11.6
   修改一个BUG,返回值是读取的字节数而不是错误码

/* 2010.2.8
/************************************************************************/
DWORD CPortManage::readByte(BYTE* pBuf, DWORD dwCount)
{
    rootLogger->trace("function readByte start..............");

    DWORD dwBytesRead = 0;
    try
    {
        if (!port.IsOpen())
        {
            long lRet = open(portPara);
            if (lRet)
            {
                return lRet;
            }
            //rootLogger->error("not open the port");
            //return ER_NOT_OPEN_PORT;
        }
        dwBytesRead = port.Read(pBuf,dwCount);
    }catch (CSerialException* pEx)
    {
        string tmp(pEx->GetErrorMessage().GetBuffer(10));
        rootLogger->error("failed to read byte: " + tmp);
        pEx->Delete();
        //return ER_READ_DATA;
        return 0;
    }
    rootLogger->trace("function readByte end..............");
    return dwBytesRead;
}

/************************************************************************/
/* 清端口数据
/* 
/* 2010.2.9
/************************************************************************/
void CPortManage::clearData()
{
    if (runMode == 1 || runMode == 9)
    {
        return;
    }
    port.ClearReadBuffer();
}

/************************************************************************/
/* 发送命令至打印机，失败返回错误码，成功返回0
/* command：指令内容
   len: 长度

/* 2009.10.31
/************************************************************************/
long CPortManage::writeByte(const BYTE *command, int len)	
{
	rootLogger->trace("function writeByte start..............");
	try
	{
		if (!port.IsOpen())
		{
            long lRet = open(portPara);
            if (lRet)
            {
                return lRet;
            }
			//rootLogger->error("the port is not opened");
			//return WN_NOT_OPEN_PORT;
		}
		port.Write(command,len);
	}catch (CSerialException* pEx)
	{
		string tmp(pEx->GetErrorMessage().GetBuffer(10));
		rootLogger->error("failed to send command: " + tmp);
		pEx->Delete();
		return ER_SEND_COMMAND;
	}	
	rootLogger->trace("function writeByte end..............");
	return 0;
}

/************************************************************************/
/* 获取端口接收字节数,失败返回错误码，成功返回0
/* cnt: 端口接收字节数

/* 2010.12.28
/************************************************************************/
long CPortManage::getPortCount(int &cnt)       //获取端口接收字节数
{
    if (runMode == 1 || runMode == 9)
    {
        cnt = 0;
        return 0;
    }
    if (!port.IsOpen())
    {
        long lRet = open(portPara);
        if (lRet)
        {
            cnt = 0;
            return lRet;
        }
    }
    COMSTAT state;
    port.GetStatus(state);
    cnt = state.cbInQue; 
    return 0;
}
