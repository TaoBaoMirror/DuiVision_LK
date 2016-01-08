/************************************************************************/
/* PortManage.cpp
/* yuzhang																
    
    2010.12.11 �Ӷ�����ģʽΪ1��9ʱ����Ӧ
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
#define ER_PORT_NUM				159001	//�˿ںŴ���
#define ER_OPEN_PORT			159002	//�˿ڴ�ʧ��
#define ER_WRITE_DATA			159003	//д�˿�����ʧ��
#define ER_READ_DATA			159004	//���˿�����ʧ��
#define ER_NOT_OPEN_PORT		159005	//�˿�û�д�

#define ER_CREATE_LISTEN_THREAD	159006	//���������߳�ʧ��
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
/* �򿪶˿ڣ�ʧ�ܷ��ش����룬�ɹ�����0
/* para����ӡ�˿ڲ���

   2010.12.1
   ���ô���ͨ�ų�ʱʱ��
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
        
        // ����ʱ�����ַ�������ʱ��
        comTimeOut.ReadIntervalTimeout = 1000;
        
        // ��ȡÿ�ֽڵĳ�ʱ
        comTimeOut.ReadTotalTimeoutMultiplier = 1000;
        
        // ���������ݵĹ̶���ʱ
        // �ܳ�ʱ = ReadTotalTimeoutMultiplier * �ֽ��� + ReadTotalTimeoutConstant
        comTimeOut.ReadTotalTimeoutConstant = 1000;
        
        // дÿ�ֽڵĳ�ʱ
        comTimeOut.WriteTotalTimeoutMultiplier = 1000;
        
        // д�������ݵĹ̶���ʱ
        comTimeOut.WriteTotalTimeoutConstant = 1000;
        
        // ����ʱ����д���豸����
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
/* д�������˿ڣ�ʧ�ܷ��ش����룬�ɹ�����0
/* strData��д�������

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
/* ���˿�����,�ӵ������ֽڿ�ʼ������������1-9ת��ASCII�����֣�ʧ�ܷ��ش����룬�ɹ�����0
/* strData������������


/* 2011.3.5(��ԭ�л����ϸĽ���ȡ�㷨)
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
		count = port.BytesWaiting(); //���ڽ����ֽ���
		if (count == 0)				//û���ֽ�ֱ�ӷ���
		{
			strData = "";
			rootLogger->debug("count = 0");
			return 0; 
		}
		
		BYTE bt=0;
		while (port.BytesWaiting())
		{
			port.Read(&bt,1);
			if (bt == 1 && port.BytesWaiting())//����������������ͷ
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
/* ���˿�����,�ӵ������ֽڿ�ʼ������������1-9ת��ASCII�����֣�ʧ�ܷ��ش����룬�ɹ�����0
/* strData������������

  2010.11.25 
  ������ֽ���С��12�򷵻ؿ�����
  2010.11.18 �ӵ������ֽڿ�ʼ�������ڶ��õ�����
  2010.11.17 �޸Ķ˿����ݽ���BUG
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
            count = state.cbInQue;  //���ڽ����ֽ���
			if (count == 0)  //û���ֽ�ֱ�ӷ���
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
		count = state.cbInQue;  //���ڽ����ֽ���

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
/* ���˿��ֽ�,���ض�ȡ���ֽ���
/* pBuf: �������ֽ�
   dwCount: Ԥ�ƶ������ֽ���

   2010.11.6
   �޸�һ��BUG,����ֵ�Ƕ�ȡ���ֽ��������Ǵ�����

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
/* ��˿�����
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
/* ������������ӡ����ʧ�ܷ��ش����룬�ɹ�����0
/* command��ָ������
   len: ����

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
/* ��ȡ�˿ڽ����ֽ���,ʧ�ܷ��ش����룬�ɹ�����0
/* cnt: �˿ڽ����ֽ���

/* 2010.12.28
/************************************************************************/
long CPortManage::getPortCount(int &cnt)       //��ȡ�˿ڽ����ֽ���
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
