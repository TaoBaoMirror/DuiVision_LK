
#include "stdafx.h"
#include "KoKoonDBManage.h"
#include "General.h"
#pragma comment(lib, "libmysql.lib")

#define DB_TYPE_MYSQL                   1
#define DB_TYPE_SQLSERVER               2
#define DB_TYPE_ORACLE                  3

CKoKoonDBManage::CKoKoonDBManage()
{
	m_DBType = DB_TYPE_MYSQL;
	if(DB_TYPE_MYSQL == m_DBType)
		m_DBMysql.openDBMysql();	
}

CKoKoonDBManage::~CKoKoonDBManage()
{
	if(DB_TYPE_MYSQL == m_DBType)
		m_DBMysql.closeDBMysql();
}


long CKoKoonDBManage::getConnectState()
{
	if(DB_TYPE_MYSQL == m_DBType)
		return m_DBMysql.getConnectState();
	return -1;
}

long CKoKoonDBManage::RunSQL(const string &strSQL)
{
	if(DB_TYPE_MYSQL == m_DBType)
		return m_DBMysql.RunSQL(strSQL);
	return -1;
}

long CKoKoonDBManage::freeResultSet()
{
	if(DB_TYPE_MYSQL == m_DBType)
		return m_DBMysql.freeResultSet();
	return -1;
}

string CKoKoonDBManage::getResultByFieldIndex(int fieldIndex)
{
	if(DB_TYPE_MYSQL == m_DBType)
		return m_DBMysql.getResultByFieldIndex(fieldIndex);
	return "";
}

string CKoKoonDBManage::getResultByFieldName(string fieldName)	 
{
	if(DB_TYPE_MYSQL == m_DBType)
		return m_DBMysql.getResultByFieldName(fieldName);
	return "";
}

long CKoKoonDBManage::nextResult()
{
	if(DB_TYPE_MYSQL == m_DBType)
		return m_DBMysql.nextResult();
	return -1;
}

bool CKoKoonDBManage::isEndResult()										
{
	if(DB_TYPE_MYSQL == m_DBType)
		return m_DBMysql.isEndResult();
	return true;
}

long CKoKoonDBManage::getRowNum()
{
	if(DB_TYPE_MYSQL == m_DBType)
		return m_DBMysql.getResultNum();
	return 0;
}


