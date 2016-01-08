#include "stdafx.h"
#include "XMLConfig.h"
#include "General.h"
#include "Shlwapi.h"	//PathRemoveFileSpec

#pragma comment(lib,"shlwapi.lib")   

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;       
#endif

//////////////////////////////////////////////////////////////////////////
//
/*
#define ER_NO_CONFIG_FILE	120001	//没有配置文件
#define ER_CONFIG_FILE		120002	//配置文件错误
#define ER_LOAD_CONFIG_FILE	120003	//加载配置文件失败
#define ER_UPDATE_CONFIG	120004	//更新配置文件失败
*/

//////////////////////////////////////////////////////////////////////////

LoggerPtr CXMLConfig::rootLogger = Logger::getLogger("CXMLConfig");

CXMLConfig* SingletonMode<CXMLConfig>::m_Object = NULL;
size_t SingletonMode<CXMLConfig>::m_Counter = 0;


/*
修改记录
2010.11.16
去掉读绝对路径
*/
CXMLConfig::CXMLConfig() 
{
	rootLogger->trace("function XMLConfig start..............");
// 	CString strPath=General::GetDialogExePath();
// 	string strProperty =strPath .GetBuffer(0);	
// 	strProperty+=+"config\\log4cxx.properties";
// 	PropertyConfigurator::configure(strProperty);
	log4cxx::PropertyConfigurator::configure("log4cxx.properties");
	configFile = 0;  //配置文件指针
	//fileName = getAppPath() + "\\config.xml";
	filePath = getAppPath() +"\\";
	rootElement = 0;	//根元素
	
	rootLogger->trace("function XMLConfig end..............");
}

CXMLConfig::~CXMLConfig()
{
	rootLogger->trace("function ~XMLConfig start..............");
	
	if (!configFile)
	{
		delete configFile;
		configFile = 0;
	}
	if (!rootElement)
	{
		delete rootElement;
		rootElement = 0;
	}
	rootLogger->trace("function ~XMLConfig end..............");
}

/************************************************************************/
/* 取得当前系统运行路径，返回路径
/* 

/* 2009.10.13
/************************************************************************/
string CXMLConfig::getAppPath()			
{
	rootLogger->trace("function getAppPath start..............");
	TCHAR modulePath[MAX_PATH];
	memset(modulePath, 0, MAX_PATH);
    GetModuleFileName(NULL, modulePath, MAX_PATH); 
	PathRemoveFileSpec(modulePath);
	rootLogger->trace("function getAppPath end..............");
    return (string)modulePath;
}

/************************************************************************/
/* 初始化，打开配置文件，失败返回错误码，成功返回0
/* 
   2010.1.19 增加fileName
/* 2009.10.13
/************************************************************************/
long CXMLConfig::initConfig(string fileName) 
{
	rootLogger->trace("function initConfig start..............");
	string strFile = filePath + fileName;
	ifstream fin(strFile.c_str());  
	if (!fin)   
	{  
		rootLogger->error("config file not exist.");
		return ER_NO_CONFIG_FILE;
	}
	configFile = new TiXmlDocument(strFile);
	bool loadOkay = configFile->LoadFile();
	if (!loadOkay)
	{
		delete configFile;
		configFile = 0;
		rootLogger->error("failed to open config file!");
		return ER_CONFIG_FILE;
	}

	if (configFile)
	{
		rootElement = configFile->RootElement();		
	}

	rootLogger->trace("function initConfig end..............");
	return 0;
}

/************************************************************************/
/* 把配置文件加载至内存，失败返回错误码，成功返回0
/* bAttributeAsKey: 是否将Attribute值作为map的key值保存 true : 是

  2010.1.20 增加bAttributeAsKey
/* 2009.10.12
/************************************************************************/
long CXMLConfig::loadConfig(bool bAttributeAsKey)						
{
	rootLogger->trace("function loadConfig start..............");
	if (rootElement)
	{
		string key(rootElement->Value());
		rootLogger->debug("rootElement: " + key);
		TiXmlElement *pChild = 0;		//根元素的子节点
		TiXmlAttribute *codeAttribute;
		pChild = rootElement->FirstChildElement(); 
		while (pChild)
		{
			if(pChild->GetText())
			{	
				if (bAttributeAsKey)
				{
					codeAttribute = pChild->FirstAttribute();
					key = codeAttribute->Value();
				}else
				{
					key = pChild->Value();
				}
				
				string value = pChild->GetText();
				rootLogger->trace(key + " = " + value);
				configPar.setValueParameter(key, value);
				rootLogger->trace(key + " == " + configPar.getValueParameter(key));
			}	
			pChild = pChild->NextSiblingElement();
		}
	}else	
	{
		rootLogger->error("failed to load config file!");
		return ER_LOAD_CONFIG_FILE;
	}
	rootLogger->trace("function loadConfig end..............");
	return 0;
}

/************************************************************************/
/* 更新配置值，失败返回错误码，成功返回0
/* 
    2010.1.29
/* 2009.10.13
/************************************************************************/
long CXMLConfig::updateConfig(string key, string value)
{
	rootLogger->trace("function updateConfig start..............");
	if (rootElement)
	{
		TiXmlElement *pChild = 0;		//根元素的子节点
		pChild = rootElement->FirstChildElement(key);
		if (pChild)
		{
			rootLogger->debug(pChild->GetText());
			pChild->Clear();
			TiXmlText *NodeContent = new TiXmlText(value); 
			pChild->LinkEndChild(NodeContent);
		}
	} else
	{
		rootLogger->error("failed to update configure");
		return ER_UPDATE_CONFIG;
	}
	rootLogger->trace("function updateConfig end..............");
	return 0;
}

/************************************************************************/
/* 保存配置信息，(更新键值至map中)，同时保存至配置文件，失败返回错误码，成功返回0
/* map<string, string> : 第一string为要改的字段名，第二string为值

/* 2009.10.13
/************************************************************************/
long CXMLConfig::saveConfig(map<string, string>::const_iterator begn, map<string, string>::const_iterator end)							
{	
	rootLogger->trace("function saveConfig start..............");
	while (begn != end)
	{
		//begn->first;
		long lRet = updateConfig(begn->first,begn->second);
		if (lRet)
		{
			return lRet;
		}
		begn++;
	}
	configFile->SaveFile();
	rootLogger->trace("function saveConfig end..............");
	return 0;
}

void CXMLConfig::scanDevice()
{
	rootLogger->trace("function scanDevice start..............");

	rootLogger->trace("function scanDevice end..............");
}