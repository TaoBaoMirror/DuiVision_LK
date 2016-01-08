
#if !defined(DBManage_define)
#define DBManage_define

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DBMysql.h"


class CKoKoonDBManage
{	
public:
	CKoKoonDBManage();
	~CKoKoonDBManage();
	long getConnectState();						        
	long RunSQL(const string &strSQL);				   
	long freeResultSet();									
	string getResultByFieldIndex(int fieldIndex);		
	string getResultByFieldName(string fieldName);		
	long nextResult();										
	bool isEndResult();
	long getRowNum();
private:
	int m_DBType;                   //数据库类型
	CDBMysql m_DBMysql;            //MySQL数据库操作实例
};

#endif
