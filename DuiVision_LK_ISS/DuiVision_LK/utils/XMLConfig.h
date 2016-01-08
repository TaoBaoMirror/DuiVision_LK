#pragma once

#include <string>
#include <map>

#include "tinyxml.h"
#include<fstream>

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include "ErrorCodeRes.h"
#include "SingletonMode.h"

using namespace std;
using namespace log4cxx;


class CXMLConfig
{
public:
	CXMLConfig(); //string strFileName = "config.xml"
	~CXMLConfig();
	static string getAppPath();			//ȡ�õ�ǰϵͳ����·��
	long initConfig(string fileName = "config.xml");		//��ʼ�����������ļ�,����Ҫ��ʼ������ʹ��
	long loadConfig(bool bAttributeAsKey = false);			//�������ļ���map, bAttributeAsKey = true: ��Attributeֵ��Ϊmap��keyֵ����
	long saveConfig(map<string, string>::const_iterator begn, map<string, string>::const_iterator end);	//���¼�ֵ��map�У�ͬʱ�����������ļ�
	void scanDevice();										//ɨ�赽��ֵ���õ�˽�г�Ա

private:
	static LoggerPtr rootLogger;
	TiXmlDocument *configFile;
	string filePath;			//�����ļ���
	TiXmlElement *rootElement;	//��Ԫ��
	//TiXmlElement *pChild = 0;		//��Ԫ�ص��ӽڵ�
	long updateConfig(string key, string value);//��������ֵ
};
