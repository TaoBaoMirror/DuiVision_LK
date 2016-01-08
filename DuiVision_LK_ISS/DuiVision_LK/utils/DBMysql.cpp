
#include "stdafx.h"
#include "ErrorCodeRes.h"
#include "KoKoonDBManage.h"
#include "General.h"
#pragma comment(lib, "libmysql.lib")

LoggerPtr CDBMysql::m_LoggerPtr = Logger::getLogger("CDBMysql");
bool CDBMysql::m_isInitMySQL=false;
DB_Param_MySQL CDBMysql::m_DBparamMySQL;

CDBMysql::CDBMysql()
{
	ClearData();
	m_MySQL = NULL;
	m_isOpen = false;  
}

CDBMysql::~CDBMysql()
{	
	
}

void CDBMysql::ClearData()
{
	m_DataSet = 0;					
	m_row = NULL;					
	m_fieldNum = 0;				    
	m_rowNum = 0;				    
}

long CDBMysql::openDBMysql()								
{
	try
	{
		if(m_isOpen)
			return 0;
		if(!m_isInitMySQL)
		{
			m_DBparamMySQL.strIP = configPar.getValueParameter("host");
			m_DBparamMySQL.nPort = configPar.getIntParameter("port");
			m_DBparamMySQL.strUser = configPar.getValueParameter("user");
			m_DBparamMySQL.strPassword = configPar.getValueParameter("password");
			m_DBparamMySQL.strDbName = configPar.getValueParameter("db");			
			m_DBparamMySQL.strCharacterSet = configPar.getValueParameter("characterSet");
			mysql_library_init(0, NULL, NULL);
			m_isInitMySQL =true;
		}
		m_MySQL = mysql_init(NULL);

		if (NULL == m_MySQL)
		{
			m_LoggerPtr->error("MySQL初始化失败!");
			return ER_DBMAN_Init;
		}

		try
		{
			my_bool my_true= TRUE;
			mysql_options(m_MySQL,MYSQL_OPT_RECONNECT,&my_true);
			if(NULL == mysql_real_connect(m_MySQL, m_DBparamMySQL.strIP.c_str(),
				m_DBparamMySQL.strUser.c_str(), m_DBparamMySQL.strPassword.c_str(), m_DBparamMySQL.strDbName.c_str(), m_DBparamMySQL.nPort,NULL,0))
			{
				string str = mysql_error(m_MySQL);
				m_LoggerPtr->error("连接MySQL数据库服务器失败! 错误码: " + str);
				return ER_DBMAN_Connect;
			}
			string characterSet = m_DBparamMySQL.strCharacterSet;
			if (characterSet != "")
			{	
				mysql_set_character_set(m_MySQL, characterSet.c_str());	
			}
			string setName = mysql_character_set_name(m_MySQL);
		}
		catch(CException *e)
		{
			m_LoggerPtr->error("连接MySQL数据库服务器失败!");
			e->Delete();
			return ER_DBMAN_Connect;
		}

		ClearData();
		m_isOpen = true;
		return 0;
	}
	catch (...) 
	{
		m_LoggerPtr->fatal("CDBMysql::openDBMysql() error");
		return -1;
	}
}	
		
long CDBMysql::closeDBMysql()								
{
	try
	{	
		if (m_isOpen && m_MySQL != NULL)
		{
			freeResultSet();
			mysql_close(m_MySQL);			
		}
		m_MySQL = NULL;
		m_isOpen = false;
		return 0;
	}
	catch (...) 
	{
		m_LoggerPtr->fatal("CDBMysql::closeDBMysql() error ");
		return -1;
	}
}

long CDBMysql::getConnectState()
{
	try
	{
		if (m_MySQL == NULL)
		{
			m_LoggerPtr->error("MySQL 数据库没有打开!");
			return ER_DBMAN_OpenError;
		}

		int ret = mysql_ping(m_MySQL);
		if (ret != 0)
		{
			m_LoggerPtr->error("Ping MySQL 服务器失败, 错误码: " + General::integertostring(ret));
			return ER_DBMAN_ServerState;
		}
		return 0;
	}
	catch (...) 
	{
		m_LoggerPtr->fatal("CDBMysql::getConnectState() error ");
		return -1;
	}
}

int CDBMysql::getIndexByName(string key)
{
	map<string, int>::iterator iter;	
	iter = m_indexMap.find(key);
	if(iter != m_indexMap.end())
	{
		return iter->second;
	}
	else
	{
		return -1;
	}
}

int CDBMysql::setNameToIndex()
{
	unsigned int i;
	MYSQL_FIELD *fields;
	string key;
	try
	{
		fields = mysql_fetch_fields(m_DataSet); 
		if (!m_indexMap.empty())
		{
			m_indexMap.clear();
		}
		for(i = 0; i < m_fieldNum; i++)
		{	
			key = fields[i].name;
			m_indexMap.insert(pair<string,int>(key,i));
		}
		return 0 ;
	}
	catch (...) 
	{
		string error = mysql_error(m_MySQL); 
		m_LoggerPtr->fatal("CDBMysql::setNameToIndex() error :"+error);
		for(i = 0; i < m_fieldNum; i++)
		{	
			m_LoggerPtr->fatal( fields[i].name);
		}
		return ER_DBMAN_NoRecordset;
	}
}

long CDBMysql::RunSQL(const string &strSQL)
{
	try
	{
		int nRet = 0;  
		if (!m_isOpen)
		{
			if (openDBMysql())
			{
				string error = mysql_error(m_MySQL); 
				m_LoggerPtr->error("打开数据库连接失败! 错误码:"+error);
				return ER_DBMAN_OpenError;
			}
		}

		if (strSQL == "")
		{
			m_LoggerPtr->warn("sql语句为空");
			return ER_DBMAN_SqlIsEmpty;
		}

		try
		{
			nRet = mysql_query(m_MySQL, strSQL.c_str());	
			if (nRet != 0)
			{
				string error = mysql_error(m_MySQL); 
				m_LoggerPtr->error("执行SQL语句失败, 返回值:" + General::integertostring(nRet)+" 错误码:"+error);
				m_LoggerPtr->error("执行失败的SQL语句:" + strSQL);
				int icount=30;
				while(icount>0)
				{
					closeDBMysql();
					Sleep(100);
					if(openDBMysql()==0)
						break ;
					icount--;
				}
				nRet = mysql_query(m_MySQL, strSQL.c_str());	
				if (nRet != 0)
				{
					m_LoggerPtr->error("网络连接中断后，重新连接失败");
					return ER_DBMAN_ExecuteSql;
				}
			}
		}
		catch(CException *e)
		{
			e->Delete();
			m_LoggerPtr->error("执行SQL语句失败, 返回值: " + General::integertostring(nRet));
			m_LoggerPtr->error("执行失败的SQL语句:" + strSQL);
			return ER_DBMAN_ExecuteSql;
		}

		m_DataSet = mysql_store_result(m_MySQL);  
		if (m_DataSet == NULL)
		{
			m_LoggerPtr->debug("没有结果集");
			return 0;
		}
		mysql_data_seek(m_DataSet,0);        
		m_row = mysql_fetch_row(m_DataSet); 
		m_fieldNum = mysql_num_fields(m_DataSet);
		m_rowNum = mysql_num_rows(m_DataSet);
		nRet = setNameToIndex();
		if(nRet != 0)
		{
			m_LoggerPtr->error("setNameToIndex()失败 SQL语句:" + strSQL);
			return ER_DBMAN_NoRecordset;
		}
		return 0;
	}
	catch (...) 
	{
		m_LoggerPtr->fatal("sqlText = " + strSQL);
		string error = mysql_error(m_MySQL); 
		m_LoggerPtr->fatal("CDBMysql::RunSQL 出错,错误码:"+error);
		return -1;
	}
}

long CDBMysql::freeResultSet()									
{
	if (m_DataSet != NULL)
	{
		mysql_free_result(m_DataSet);		
		ClearData();
	}
	return 0;
}

string CDBMysql::getResultByFieldIndex(int fieldIndex)		
{
	string q_value = "";
	try
	{
		if (m_row == NULL ||m_row==0)
		{
			m_LoggerPtr->info("记录集为空"); 
			return "";
		}

		if (fieldIndex >= m_fieldNum || fieldIndex < 0)
		{
			m_LoggerPtr->warn("指定字段超出范围"); 
			return "";
		}

		char *chTmp;
		chTmp = m_row[fieldIndex];

		if (chTmp == 0) 
		{
			q_value = "";
		}
		else
		{
			q_value = m_row[fieldIndex]; 
		}
		return q_value;
	}
	catch (...) 
	{
		m_LoggerPtr->fatal("fieldIndex: "+General::integertostring(fieldIndex)+" FieldValue: " + q_value);
		string str = mysql_error(m_MySQL); 
		m_LoggerPtr->fatal("CDBMysql::getResultByFieldIndex() error :"+str);
		return "";
	}
}

string CDBMysql::getResultByFieldName(string fieldName)	 
{
	string q_value = "";
	try
	{	
		if (m_row == NULL||m_row==0)
		{
			m_LoggerPtr->info("记录集为空"); 
			return "";
		}
		int fieldIndex = getIndexByName(fieldName);
		if (fieldIndex == -1)
		{
			m_LoggerPtr->error("指定字段不存在, fieldName = " + fieldName);
			return "";
		} 
		
		char *chTmp;
		chTmp = m_row[fieldIndex];
		if (chTmp == 0)
		{
			q_value = "";
		}
		else
		{
			q_value = m_row[fieldIndex]; 
		}
		return q_value;
	}
	catch (...) 
	{
		m_LoggerPtr->fatal("fieldName: " + fieldName+" q_value :"+q_value);
		string str = mysql_error(m_MySQL); 
		m_LoggerPtr->fatal("CDBMysql::getResultByFieldName() error :"+str);		
		return "";
	}
}

long CDBMysql::nextResult()
{	
	if (m_DataSet == NULL)
	{
		m_LoggerPtr->error("没有数据集");
		return ER_DBMAN_NoRecordset;
	}

	m_row = mysql_fetch_row(m_DataSet);
	if (m_row == NULL ||m_row==0)
	{
		m_LoggerPtr->info("记录集为空");
		return ER_DBMAN_DataEnd;
	}
	return 0;
}

bool CDBMysql::isEndResult()										
{
	if (m_row != NULL && m_row!=0)
	{
		return false;	
	}
	return true;
}

long CDBMysql::getResultNum()									
{
	return m_rowNum;
}


