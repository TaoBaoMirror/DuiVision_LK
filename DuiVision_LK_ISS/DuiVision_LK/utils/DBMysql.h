
#if !defined(DB_MYSQL_H)
#define DB_MYSQL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <mysql.h>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace std;

typedef struct _DB_Param_MySQL			
{
	string strIP;			  
	unsigned int nPort;		  
	string strUser;			  
	string strPassword;		  
	string strDbName;	      	
	string strCharacterSet;   
}DB_Param_MySQL;

class CDBMysql
{
public:
	CDBMysql();
	~CDBMysql();
	long closeDBMysql();								 
	long openDBMysql();						         
	long getConnectState();						        
	long RunSQL(const string &strSQL);				    
	long freeResultSet();									
	string getResultByFieldIndex(int fieldIndex);		
	string getResultByFieldName(string fieldName);		
	long nextResult();										
	bool isEndResult();										
	long getResultNum();								
private:
	static LoggerPtr m_LoggerPtr;                       
	static bool m_isInitMySQL;                         
	static DB_Param_MySQL m_DBparamMySQL;	           
	MYSQL *m_MySQL;                                     
	MYSQL_RES *m_DataSet;					            
	MYSQL_ROW m_row;						            
	unsigned int m_fieldNum;			            
	unsigned long m_rowNum;				               
	map<string, int> m_indexMap;            			
	bool m_isOpen;							            
	
	void ClearData();					                
	int setNameToIndex();				                
	int getIndexByName(string key);			               
};

#endif
