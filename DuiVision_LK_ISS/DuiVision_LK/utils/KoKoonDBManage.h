
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
	int m_DBType;                   //���ݿ�����
	CDBMysql m_DBMysql;            //MySQL���ݿ����ʵ��
};

#endif
