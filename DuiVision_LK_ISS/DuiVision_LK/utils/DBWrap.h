// DataBaseMan.h: interface for the CDataBaseMan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBWRAP_H__83F43D60_570D_405C_8229_0A1141BE834A__INCLUDED_)
#define AFX_DBWRAP_H__83F43D60_570D_405C_8229_0A1141BE834A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "KoKoonDBManage.h"

using namespace std;


typedef struct tagMedInfo
{
	string strBarcode;		//药品条码

	string strMedID;		//药品ID
	string strHisID;		//药品HIS 的ID
	string strChName;		//药品名称
	string strPyCode;		//药品中文拼音编码
	string strUnit;			//药品规格
	string strFactoryName;	//厂家

	string strTypeUnit;		//药品大单位，如：盒

	string strLocID;		//位置
	string strCurQuantity;	//当前数量
	string strWarnThreshold;//告警阈值
	string strMaxCapacity;  //最大容量
	string strLocMedUnitName; //当前位置药品单位
	string strMedType;      //药品类型
	string strMedMachineID;  //机器ID
	string strNeedNum;         //需加量
	string strvalidatetime;
	string strmedbatch;
	void SetValue(tagMedInfo obj)
    {  
		 strBarcode=obj.strBarcode;		//药品条码		
		 strMedID=obj.strMedID;		//药品ID
		 strHisID=obj.strHisID;
		 strChName=obj.strChName;		//药品名称
		 strPyCode=obj.strPyCode;		//药品中文拼音编码
		 strUnit=obj.strUnit;			//药品规格
		 strFactoryName=obj.strFactoryName;	//厂家		
		 strTypeUnit=obj.strTypeUnit;		//药品大单位，如：盒
		 strLocID=obj.strLocID;		//位置
		 strCurQuantity=obj.strCurQuantity;	//当前数量
		 strWarnThreshold=obj.strWarnThreshold;//告警阈值
		 strMaxCapacity=obj.strMaxCapacity;  //最大容量
		 strLocMedUnitName=obj.strLocMedUnitName; //当前位置药品单位
		 strMedType=obj.strMedType;      //药品类型
		 strMedMachineID=obj.strMedMachineID;  //机器ID
		 strvalidatetime=obj.strvalidatetime;
		 strmedbatch=obj.strmedbatch;
		 strNeedNum=obj.strNeedNum;
    } 
	void Clear()
	{
		strBarcode="";		//药品条码		
		strMedID="";		//药品ID
		strHisID="";
		strChName="";		//药品名称
		strPyCode="";		//药品中文拼音编码
		strUnit="";			//药品规格
		strFactoryName="";	//厂家		
		strTypeUnit="";		//药品大单位，如：盒
		strLocID="";		//位置
		strCurQuantity="";	//当前数量
		strWarnThreshold="";//告警阈值
		strMaxCapacity="";  //最大容量
		strLocMedUnitName=""; //当前位置药品单位
		strMedType="";      //药品类型
		strMedMachineID="";  //机器ID
		strvalidatetime="";
		strmedbatch="";
		strNeedNum=="";
	}
}MedInfo;

typedef struct tagFetchMedInfo
{
	bool bNoLack;			//该药品当前是否不缺药，即：当前药柜中，够取药数量
	string strMedID;		//药品ID
	string strHisID;		//药品HIS 的ID
	string strName;			//药品名称
	string strUnit;			//药品规格
	string strFactoryName;	//厂家
	string strTypeUnit;		//药品大单位，如：盒

	string strUseDays;		//服用天数
	string strUseTime;		//每天服用几次
	string strUseQty;		//每次服用数量

	string strFetchNum;		//取药数量
	string strHeterNum;	    //异形药数量
	string strFetchState;	//取药状态
	string strStorageState;	//存储状态

	string strLocID;		//位置
	string strCurQuantity;	//当前数量
	string strWarnThreshold;//告警阈值
	//string strMaxCapacity;  //最大容量
	string strLocMedUnitName; //当前位置药品单位
	string strLackNum;		//缺药数量

	string strvalidatetime; //有效期
	string strmedbatch;       //批号

}FetchMedInfo;

typedef struct tagPreInfo
{
	string strPreID;			//处方ID
	string strHisPreID;			//HIS系统处方ID
	string strBsktID;			//药篮ID
	string strMzNo;				//门诊号
	string strDepartmentName;	//科室名称
	string strFetchWindow;		//取药窗口号
	string strPatientName;		//病人名字
	string strPatientAge;		//病人年纪
	string strPatientSex;		//病人性别

	list<FetchMedInfo> listFetchMedInfo;	//药品列表信息
}PreInfo;



class CDBWrap  
{
public:
	CDBWrap();
	virtual ~CDBWrap();

private:

	string convertSex(int nSex);


public:	
	/************************************************************************/
	/*                            处方取药                                  */
	/************************************************************************/
	//篮子ID ------->处方ID，子处ID(配合盒剂)
	long queryPreIDByBsktID(const string &strBsktID, string &strPreID, string &strSplitID);

	//篮子ID ------->HIS处方ID
	long queryHisIDByBsktID(const string &strBsktID, string &strHisID);
protected:	
	//处方ID-------->处方信息(配合盒剂)
	long queryPreInfoByPreID(const string &strPreID, const string &strSplitID);

	//处方ID-------->处方药品列表(配合盒剂)
	long queryMedListByPreID(const string &strPreID, const string &strSplitID);


	//HIS_ID-------->处方信息(单独使用)------并返回通过HIS_ID查到本系统的Pre_ID
	long queryPreInfoByHisID(const string &strHisID, string &strPreID);
	
	//HIS_ID-------->处方药品列表(单独使用)----传入通过HIS_ID查到本系统的Pre_ID
	long queryMedListByHisID(const string &strPreID);

public:	
	/************************************************************************/
	/*                            批量加药                                  */
	/************************************************************************/
	

	//卡号----篮子号（配合盒剂）
	long queryBsktIDByCardID(const string &strCardID, string &strBsktID);

	/********************************药品管理*************************************/
	
public:
	//处方取药--模块，根据篮子ID，获取处方信息和处方药品列表（配合盒剂）
	long queryPreInfoMedListByBsktID(const string &strBsktID);

	//处方取药--模块，根据HIS_ID，获取处方信息和处方药品列表（单独使用）
	long queryPreInfoMedListByHisID(const string &strHisID);
	
	//批量加药查询，缺药的药品信息
	long queryBatchAddMedInfoList(int sorttype=0);

	//根据药品条码，获取该药品的存储信息;
	long queryMedManInfoListByBarcode(string strBarcode);

	//通过处方ID，获取篮子上绑定的RFID;（配合盒剂）
	long queryRFID_ByPreID(string &strLeft, string &strRight, const string &strPreID);

	//查询所有的空闲的位置；
	long queryFreeLocID();
	
	//指定位置添加药数
	long increaseMedNumByLocID(int nLocID, int nCurQt, int nIncNum);

	//指定位置减少药数
	long decreaseMedNumByLocID(int nLocID, int nCurQt, int nDecNum);
	//查询药品的有效期
	long queryvalidatemedinfo(list<MedInfo>& medlist,int sorttype=0);

	long queryMedIdByName(string strMedId, string strMedName);

	long modifyWarnByLocID(int nLocID, int nNewWarn);

	long modifyMaxCapacityByLocID(int nLocID, int nNewMaxCapacity);

	long modifyCurQtByLocID(int nLocID, int nNewCurQt);

	long modifyValidBatchByLocID(int nLocID,string strvalid,string strbatch);

	long modifyValidBatchByMedID(int medid,string strvalid,string strbatch);	

	//根据LOC_ID,绑定一个药品
	long boundMedIdByLocID(int nLocID, int nMedID,string medunitname);

	long modifyBarcode(int medid,string barcode);
	//根据LOC_ID,解除一个药品
	long unboundMedIdByLocID(int nLocID);

	long queryUnitName(int medid,list<string>& listunitname);

	long queryMedIsExistByMedBarcode(string strBarcode, bool &bExist, string &strMedID, 
				string &strMedPyCode, string &strMedChName, string &strMedUnit, string &strFactoryName);
	
	//查找数据库中已绑定的和未绑定的药品名称
	long QueryMedName(list<string>& listname);

	inline PreInfo& getPreInfo()
	{
		return m_stPreInfo;
	}

	inline list<FetchMedInfo>& getPreMedInfoList()
	{
		return m_stPreInfo.listFetchMedInfo;
	}

	inline list<MedInfo>& getMedInfoListBatchAdd()
	{
		return m_listMedInfoBatchAdd;
	}

	inline list<string>& getVogueMedNameList()
	{
		return m_listVogueMedName;
	}

	inline list<MedInfo>& getMedInfoListMedMan()
	{
		return m_listMedInfoMedMan;
	}

	inline list<int>& getFreeLocIDList()
	{
		return m_listFreeLocID;
	}
	//根据药品名字查找药品
	long vogueSerchMedInfoListMedMan(string strMedPyCodePart,int sorttype=0);

	long QueryLackMedInfoList(string strMedPyCodePart,int sorttype=0);
	long QueryLackMedInfoListByCode(string strMedPyCodePart);

	long QueryLackMedName(list<string>& listname);

	string LocID2Str(int nID);
	int LocStr2ID(string strID);

	long boundLocForMed(int nLocID, int nMedID);

	long unboundLocForMed(int nLocID);

	string Convert2Str(int nLocID);

	int Convert2Int(string strLocID);

private:
	CKoKoonDBManage		m_dbMgr;

	static LoggerPtr m_pLogger;

	list<MedInfo>   m_listInfoMedMan;

	list<int>		m_listFreeLocID;

	bool			m_bSplit;//是拆分处方

	//
	list<MedInfo>	m_listMedInfoBatchAdd;

	//
	PreInfo			m_stPreInfo;

	//
	list<string>    m_listVogueMedName;

	//
	list<MedInfo>   m_listMedInfoMedMan;


	int m_nTierNum;
	int m_nTierCapacity;
	int m_machinetype;  //机器类型
	int m_machineID;    //机器ID
	string m_locationName;//机器地址表
	int m_nUserAlone; //1，单独使用，0 配合盒剂使用

	int IsValidTime;   //1使用，0 不启用
	int ValidMonth;    //有效期月份
	int ValidControlMonth;//到期之前几个月
};




#endif // !defined(AFX_DBWRAP_H__83F43D60_570D_405C_8229_0A1141BE834A__INCLUDED_)
