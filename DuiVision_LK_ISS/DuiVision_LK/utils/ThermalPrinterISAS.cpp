// ThermalPrinterISAS.cpp: implementation of the CThermalPrinterISAS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThermalPrinterISAS.h"

#include "General.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


LoggerPtr CThermalPrinterISAS::m_pLogger = Logger::getLogger("CThermalPrinterISAS");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThermalPrinterISAS::CThermalPrinterISAS()
{
	m_strLackHead = configPar.getValueParameter("List_Lack_of_Med").c_str();

	m_nPosMedUnit = 14;
	m_nPosMedLackNum = m_nPosMedUnit + 6;
	m_nPosLoc = m_nPosMedLackNum + 6;
}

CThermalPrinterISAS::~CThermalPrinterISAS()
{

}


//智能存取---缺药
long CThermalPrinterISAS::printLackMed(const StLackMedInfo &lackMed, bool bHead)
{
	long lRes;
	
	m_pLogger->trace("CThermalPrinterISAS::printLackMed start ...");
	
	lRes = m_print.PrintString(lackMed.strMedName);
	if (lRes)
	{
		return lRes;
	}
	
	//一行打印不下，空间不够，则换行
	if (lackMed.strMedName.length() + 1 > m_nPosMedUnit)
	{
		lRes = m_print.PrintRow();
		if (lRes)
		{
			return lRes;
		}
	}
	
	lRes = m_print.PrintString(lackMed.strMedUnit, m_nPosMedUnit);
	if (lRes)
	{
		return lRes;
	}

	//一行打印不下，空间不够，则换行
	if (m_nPosMedUnit + lackMed.strMedUnit.length() + 1 >= m_nPosMedLackNum)
	{
		lRes = m_print.PrintRow();
		if (lRes)
		{
			return lRes;
		}
	}

	lRes = m_print.PrintString(lackMed.strMedLackNum, m_nPosMedLackNum);
	if (lRes)
	{
		return lRes;
	}


	string strSimpLoc = "";

	if (bHead)
	{
		strSimpLoc = lackMed.strLoc;
	} 
	else
	{
		LocStr2Simp(lackMed.strLoc, strSimpLoc);
	}

	lRes = m_print.PrintString(strSimpLoc, m_nPosLoc);
	if (lRes)
	{
		return lRes;
	}
	
	lRes = m_print.PrintRow();
	if (lRes)
	{
		return lRes;
	}
	
	m_pLogger->trace("CThermalPrinterISAS::printLackMed end ...");
	
	return 0;
}

//打印缺药药品列表----智能存取系统
long CThermalPrinterISAS::printLackList(const list<StLackMedInfo> &listLack)
{
	long lRes;
	string strTemp = "";
	
	m_pLogger->trace("CThermalPrinterISAS::printLackList start ...");
	
	if (listLack.empty())
    {
        return 0;
    }
	
	//居中打印
	lRes = m_print.SetAlignMode(ALIGN_MID);
	if (lRes)
	{
		return lRes;
	}

	//字体放大2倍
	lRes = m_print.SetExpand(2);
	if (lRes)
	{
		//出错了
		return lRes;
	}
	
	//打印抬头----普通缺药清单
	lRes = m_print.PrintRow(m_strLackHead);
	if (lRes)
	{
		return lRes;
	}

	//字体恢复正常大小
	lRes = m_print.SetNormal();
	if (lRes)
	{
		//出错了
		return lRes;
	}
	
	//右对齐打印
	lRes = m_print.SetAlignMode(ALIGN_RIGHT);
	if (lRes)
	{
		return lRes;
	}
	
	//打印时间
	CTime time;
	time = CTime::GetCurrentTime();
	strTemp = (LPCSTR)time.Format("%Y-%m-%d %H:%M:%S");
	lRes = m_print.PrintRow(strTemp);
	if (lRes)
	{
		return lRes;
	}
	
	//左对齐打印
	lRes = m_print.SetAlignMode(ALIGN_LEFT);
	if (lRes)
	{
		return lRes;
	}

	lRes = m_print.PrintRow();
	if (lRes)
	{
		//出错了
		return lRes;
	}

	StLackMedInfo stLMI;
	stLMI.strMedName = configPar.getValueParameter("Medicine_name").c_str();
	stLMI.strMedUnit = configPar.getValueParameter("Unit").c_str();
	stLMI.strMedLackNum = configPar.getValueParameter("Lack_num").c_str();
	stLMI.strLoc = configPar.getValueParameter("Position").c_str();

	lRes = printLackMed(stLMI, true);
	if (lRes)
	{
		return lRes;
	}

	string strOldMedName = "";

	for (list<StLackMedInfo>::const_iterator iterLMI = listLack.begin();
	iterLMI != listLack.end();
	++iterLMI)
	{
		if (strOldMedName == iterLMI->strMedName)
		{
			stLMI.strMedName = "";
			stLMI.strMedUnit = "";
			stLMI.strMedLackNum = iterLMI->strMedLackNum;
			stLMI.strLoc = iterLMI->strLoc;
		} 
		else
		{
			stLMI.strMedName = iterLMI->strMedName;
			stLMI.strMedUnit = iterLMI->strMedUnit;
			stLMI.strMedLackNum = iterLMI->strMedLackNum;
			stLMI.strLoc = iterLMI->strLoc;

			strOldMedName = iterLMI->strMedName;
		}

		//打印单个普通缺药信息
		lRes = printLackMed(stLMI);
		if (lRes)
		{
			return lRes;
		}
		
		Sleep(50);
	}

	//切纸
	lRes = m_print.CutPaper();
	if (lRes)
	{
		return lRes;
	}
	
	m_pLogger->trace("CThermalPrinterISAS::printLackList end ...");

	return 0;
}

//位置的格式转换-----柜1第2层5槽---》1-2-5
bool CThermalPrinterISAS::LocStr2Simp(const string &strDescLoc, string & strSimpLoc)
{
	bool bRet;
	int nChestNum;
	int nTierNum;//从1开始
	int nSeqNum;//从1开始
	
	//用来吃掉汉字描述部分
	string strDes;
	
	istringstream instrstr(strDescLoc);
	
	instrstr>>strDes>>nChestNum>>strDes>>nTierNum>>strDes>>nSeqNum>>strDes;
	
	if (1 == nChestNum || 2 == nChestNum)
	{
		strSimpLoc = General::integertostring(nChestNum) + "-" + General::integertostring(nTierNum) + "-" + General::integertostring(nSeqNum);

		bRet = true;
	}
	else
	{
		//出错了
		strSimpLoc = configPar.getValueParameter("Wrong_Position").c_str();
		bRet = false;
	}

	return bRet;
}
