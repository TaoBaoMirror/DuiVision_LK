// ThermalPrinterISAS.h: interface for the CThermalPrinterISAS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THERMALPRINTERISAS_H__715CC3F8_6B48_4A11_B3C8_F82307A137D4__INCLUDED_)
#define AFX_THERMALPRINTERISAS_H__715CC3F8_6B48_4A11_B3C8_F82307A137D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using namespace std;

#include "ThermalPrinterSPRT_RME3.h"

typedef struct tagLackMedInfo
{
	string strMedName;
	string strMedUnit;
	string strMedLackNum;
	string strLoc;
}StLackMedInfo;

class CThermalPrinterISAS
{
public:
	CThermalPrinterISAS();
	virtual ~CThermalPrinterISAS();

public:

	//��ӡȱҩҩƷ�б�----���ܴ�ȡϵͳ
	long printLackList(const list<StLackMedInfo> &listLack);

	//λ�õĸ�ʽת��-----��1��2��5��---��1-2-5
	bool LocStr2Simp(const string &strDescLoc, string & strSimpLoc);
protected:
	//���ܴ�ȡ---ȱҩ
	long printLackMed(const StLackMedInfo &lackMed, bool bHead = false);

public:
	CThermalPrinterSPRT_RME3 m_print;

private:

	static LoggerPtr m_pLogger;

	string m_strLackHead;

	int m_nPosMedUnit;
	int m_nPosMedLackNum;
	int m_nPosLoc;
};

#endif // !defined(AFX_THERMALPRINTERISAS_H__715CC3F8_6B48_4A11_B3C8_F82307A137D4__INCLUDED_)
