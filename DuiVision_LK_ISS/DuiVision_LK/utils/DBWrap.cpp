// DataBaseMan.cpp: implementation of the CDataBaseMan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBWrap.h"
#include "General.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


LoggerPtr CDBWrap::m_pLogger = Logger::getLogger("CDBWrap");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBWrap::CDBWrap()
{
	m_bSplit = false;

	m_nTierNum = configPar.getIntParameter("TierNum");
	m_nTierCapacity = configPar.getIntParameter("TierCapacity");

	//读取配置文件上，有关数据库连接需要的参数
	DBparam.host = configPar.getValueParameter("host");
	DBparam.db = configPar.getValueParameter("db");
	DBparam.user = configPar.getValueParameter("user");
	DBparam.password = configPar.getValueParameter("password");
	DBparam.port = configPar.getIntParameter("port");
	DBparam.characterSet = configPar.getValueParameter("characterSet");
	m_nUserAlone=configPar.getIntParameter("IsUsedAlone");
	IsValidTime=configPar.getIntParameter("IsValidTime");
	ValidMonth=configPar.getIntParameter("ValidMonth");
	
	if(configPar.getIntParameter("ValidControlMonth")>0)
		ValidControlMonth=configPar.getIntParameter("ValidControlMonth");
	else
		ValidControlMonth=0;
	//打开连接数据库
	int res=m_dbMgr.getConnectState();
	if (res==0)
	{
		m_machinetype=configPar.getIntParameter("MachineType");
		m_machineID=configPar.getIntParameter("MachineID");
		string sql="select positionTabName from tbl_device where deviceID='"+General::integertostring(m_machineID)+"'";
		if(m_dbMgr.RunSQL(sql)==0 )
		{
			m_locationName=m_dbMgr.getResultByFieldIndex(0);
		}
		else
		{
			m_pLogger->info(sql);
			m_pLogger->error("CMedicineManage construct function failure");
			m_locationName="";
		}
		m_dbMgr.freeResultSet();
	}
}

CDBWrap::~CDBWrap()
{
	
}

string CDBWrap::convertSex(int nSex)
{
	string strRet;

	switch (nSex)
	{
	case 1:
		strRet = "男";
		break;
	case 2:
		strRet = "女";
		break;
	default:
		strRet = "";
		break;
	}

	return strRet;
}

/************************************************************************/
/*                     通过 卡ID  获取  篮子ID                          */
/************************************************************************/
long CDBWrap::queryBsktIDByCardID(const string &strCardID,string &strBsktID)
{
	m_pLogger->trace("CDBWrap::queryBsktIDByCardID start...");

	string strSql;

	long lRet;

	strSql = " SELECT basketID ";

	strSql += " FROM tbl_prebasket_info ";

	strSql += " WHERE icCard1='" + strCardID + "' OR icCard2='" + strCardID + "'";

	lRet = m_dbMgr.RunSQL(strSql);
	if (lRet)
	{
		//出错了------没有查到该卡号相应的篮子号
		return lRet;
	}

	if (m_dbMgr.getRowNum())
	{
        strBsktID = m_dbMgr.getResultByFieldName("basket_id");        
	}
	m_dbMgr.freeResultSet();
	
	m_pLogger->trace("CDBWrap::queryBsktIDByCardID end...");

	return 0;
}

// 通过 篮子ID 获取 处方ID
long CDBWrap::queryPreIDByBsktID(const string &strBsktID, string &strPreID, string &strSplitID)
{
	m_pLogger->trace("CDBWrap::queryPreIDByBsktID start...");
	
	string strSql = "SELECT preID, preSplitID FROM tbl_prebasket_info WHERE basketID='" 
					+ strBsktID
					+"'";
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		strPreID = "";
		return lRes;
	}

	if (m_dbMgr.getRowNum())
	{
		strPreID = m_dbMgr.getResultByFieldName("pre_id");

		strSplitID = m_dbMgr.getResultByFieldName("split_id");

		if (!strSplitID.compare("0") || !strSplitID.compare(""))
		{
			m_bSplit = false;
		}
		else
		{
			m_bSplit = true;			
		}
	}
	m_dbMgr.freeResultSet();
	
	m_pLogger->trace("CDBWrap::queryPreIDByBsktID end...");

	return 0;
}


//处方ID-------->处方信息(配合盒剂)
long CDBWrap::queryPreInfoByPreID(const string &strPreID, const string &strSplitID)
{
	m_pLogger->trace("CDBWrap::queryPreInfoByPreID start...");

	m_stPreInfo.strPreID = "";
	m_stPreInfo.strBsktID = "";
	
	m_stPreInfo.strMzNo = "";
	m_stPreInfo.strHisPreID = "";
	m_stPreInfo.strDepartmentName = "";
	m_stPreInfo.strFetchWindow = "";
	m_stPreInfo.strPatientName = "";
	m_stPreInfo.strPatientSex = "";
	m_stPreInfo.strPatientAge = "";

	string strSql;


	/*!>
	属性： 门诊号，处方ID，HIS系统ID，篮子ID,处方状态，病人名字，年纪，性别，科室名称，取药窗口号
	表：   处方、病人、拆分处方（篮子映射）
	*/
	//拆分处方
	if (m_bSplit)
	{
		strSql = "SELECT pat.outpatientNo, pre.prescriptionID, pre.prescriptionHisID, split.basketID, split.splitState, pat.patientName,"
				"pat.patientAge, pat.patientSex, pre.departmentName, pre.prescriptionWindow";
				
		strSql += " FROM tbl_prescription_info pre, tbl_patient_info pat, tbl_prescription_split_info split";

		strSql += " WHERE pre.patientID = pat.patientID"
				  " AND pre.prescriptionID = split.prescriptionID"
				  " AND split.prescriptionID = '" + strPreID + "'"
				  " AND split.splitID = '" + strSplitID + "'";
		if(m_nUserAlone==0)
			strSql+=" AND (substr(split.splitState, LENGTH(split.splitState)+1-1, 1) = '7' OR substr(split.splitState, LENGTH(split.splitState)+1-1, 1) = '8')";
	}
	else//主处方( 注意：distributeState字段已取消 )
	{
		strSql = "SELECT pat.outpatientNo, pre.prescriptionID, pre.prescriptionHisID, pre.basketID, pre.distributeState, pat.patientName, "
				"pat.patientAge, pat.patientSex, pre.departmentName, pre.prescriptionWindow";

        strSql += " FROM tbl_prescription_info pre, tbl_patient_info pat";

		strSql += " WHERE pre.prescriptionID = pat.prescriptionID"
				  
                  " AND pre.prescriptionID = '" + strPreID + "'";
		if(m_nUserAlone==0)
			strSql+=" AND (substr(pre.distributeState, LENGTH(pre.distributeState)+1-1, 1) = '7' OR substr(pre.distributeState, LENGTH(pre.distributeState)+1-1, 1) = '8')";
	}

	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//查询处方的病人信息出错了
		return lRes;
	}

	if (m_dbMgr.getRowNum())
	{
		string strPreState;

		if (m_bSplit)
		{
			strPreState = m_dbMgr.getResultByFieldName("splitState");
		}
		else
		{
			strPreState = m_dbMgr.getResultByFieldName("distributeState");
		}
		
		char cPreState = strPreState.at(strPreState.length() - 1);
		if ('8' == cPreState)
		{
			m_dbMgr.freeResultSet();
			//此处方已在本机取过药，本次操作将不更新数据库药品数量，还需要操作吗？
			return -1;
		}

		m_stPreInfo.strPreID = m_dbMgr.getResultByFieldName("prescriptionID");
		m_stPreInfo.strBsktID = m_dbMgr.getResultByFieldName("basketID");
		
		m_stPreInfo.strMzNo = m_dbMgr.getResultByFieldName("outpatientNo");
		m_stPreInfo.strHisPreID = m_dbMgr.getResultByFieldName("prescriptionHisID");
		m_stPreInfo.strDepartmentName = m_dbMgr.getResultByFieldName("departmentName");
		m_stPreInfo.strFetchWindow = m_dbMgr.getResultByFieldName("prescriptionWindow");
		m_stPreInfo.strPatientName = m_dbMgr.getResultByFieldName("patientName");
		m_stPreInfo.strPatientSex = convertSex(  atoi(m_dbMgr.getResultByFieldName("patientSex").c_str())  );
		m_stPreInfo.strPatientAge = m_dbMgr.getResultByFieldName("patientAge");
	}
	m_dbMgr.freeResultSet();

	m_pLogger->trace("CDBWrap::queryPreInfoByPreID end...");

	return 0;
}


//处方ID-------->处方药品列表(配合盒剂)
long CDBWrap::queryMedListByPreID(const string &strPreID, const string &strSplitID)
{
	m_pLogger->trace("CDBWrap::queryMedListByPreID start...");

	long lRes;
	string strSql;

	if (m_bSplit)
	{
		/*!>
		属性：处方ID，发药数量，异形药片数量，服用天数，
			  每天服用次数，每次服用数量，发药状态,		  
			  药品ID，药品中文名，规格，厂家，药品大单位,
			  位置ID,位置上的当前数量
		表：  prescription_medicine_list,tbl_medicine_info,location_chest
		*/
		strSql = "SELECT pre_split_med.prescriptionID, pre_split_med.medicineBoxNum, pre_split_med.medicineUnBoxNum, "
				" pre_split_med.remedyMedicineNum, pre_split_med.distributeState, "
				" med.medicineID, med.medicineProductName,med.medicineFormat, med.medicineFactoryName, med.medicineUnitName, "
				" loc.positionID, loc.curCapacity, loc.alertValue ";
				
		strSql +=" FROM tbl_prescription_split_medicine_info pre_split_med, tbl_medicine_info med "
				 " LEFT JOIN "+m_locationName+" loc ON(med.medicineID = loc.medicineID)";
             
		strSql += " WHERE pre_split_med.medicineID = med.medicineID"
				  
				  " AND pre_split_med.prescriptionID = '" + strPreID + "'"
				  " AND pre_split_med.splitID = '" + strSplitID + "'";
		if(m_nUserAlone==0)
			strSql+=" AND pre_split_med.distributeState = 2";
	}
	else
	{
		/*!>
		属性：处方ID，发药数量，异形药片数量，服用天数，
			  每天服用次数，每次服用数量，发药状态,		  
			  药品ID，药品中文名，规格，厂家，药品大单位,
			  位置ID,位置上的当前数量
		表：  prescription_medicine_list,medicine,location_chest
		*/
		strSql = "SELECT pre_med.prescriptionID, pre_med.medicineBoxNum, pre_med.medicineUnBoxNum, "
				"pre_med.remedyMedicineNum,pre_med.distributeState, "
				" med.medicineID, med.medicineProductName,med.medicineFormat, med.medicineFactoryName, med.medicineUnitName, "
				" loc.positionID, loc.curCapacity, loc.alertValue ";
				
		strSql +=" FROM tbl_prescription_medicine_info pre_med, tbl_medicine_info med "
				 " LEFT JOIN "+m_locationName+" loc ON(med.medicineID = loc.medicineID)";
             
		strSql += " WHERE pre_med.medicineID = med.medicineID"
				  
				  " AND pre_med.prescriptionID = '" + strPreID + "'";
		if(m_nUserAlone==0)
			" AND pre_med.distributeState = 2";
	}


	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//
		return lRes;
	}

	if (m_stPreInfo.listFetchMedInfo.size())
	{
		m_stPreInfo.listFetchMedInfo.clear();
	}
	

	FetchMedInfo stFMI;

	while (!m_dbMgr.isEndResult())
	{
		memset(&stFMI, 0, sizeof(FetchMedInfo));
		
		/*
			处方ID，发药数量，异形药片数量，服用天数，每天服几次，每次服用数量，
			每天服用次数，每次服用数量，发药状态,		  
			药品ID，药品中文名，规格，厂家，药品大单位,
		    位置ID,位置上的当前数量
		*/
		if(m_nUserAlone==0)
		{
			stFMI.strFetchNum = m_dbMgr.getResultByFieldName("remedyMedicineNum");
		}
		else
		{
			stFMI.strFetchNum = m_dbMgr.getResultByFieldName("medicineBoxNum");
		}
		
		stFMI.strHeterNum = m_dbMgr.getResultByFieldName("medicineUnBoxNum");

		stFMI.strMedID = m_dbMgr.getResultByFieldName("medicineID");
		stFMI.strName = m_dbMgr.getResultByFieldName("medicineProductName");
		stFMI.strUnit = m_dbMgr.getResultByFieldName("medicineFormat");
		stFMI.strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");
		
		stFMI.strLocID = m_dbMgr.getResultByFieldName("positionID");
		stFMI.strCurQuantity = m_dbMgr.getResultByFieldName("curCapacity");
		stFMI.strWarnThreshold = m_dbMgr.getResultByFieldName("alertValue");

		m_stPreInfo.listFetchMedInfo.push_back(stFMI);

		m_dbMgr.nextResult();
	}
	m_dbMgr.freeResultSet();

	m_pLogger->trace("CDBWrap::queryMedListByPreID end...");

	return 0;
}



//处方取药--模块，根据篮子ID，获取处方信息和处方药品列表（配合盒剂）
long CDBWrap::queryPreInfoMedListByBsktID(const string &strBsktID)
{
	m_pLogger->trace("CDBWrap::queryPreInfoMedListByBsktID start...");
	
	string strPreID;
	string strSplitID;

	//先篮子ID-------处方ID
	long lRes = queryPreIDByBsktID(strBsktID, strPreID, strSplitID);
	if (lRes)
	{
		//
		return lRes;
	}

	//获取处方信息
	lRes = queryPreInfoByPreID(strPreID, strSplitID);
	if (lRes)
	{
		//
		return lRes;
	}
	
	//获取处方的药品列表
	lRes = queryMedListByPreID(strPreID, strSplitID);
	if (lRes)
	{
		//
		return lRes;
	}
	
	m_pLogger->trace("CDBWrap::queryPreInfoMedListByBsktID end...");
	
	return 0;
}


//HIS_ID-------->处方信息(单独使用)------并返回通过HIS_ID查到本系统的Pre_ID
long CDBWrap::queryPreInfoByHisID(const string &strHisID, string &strPreID)
{
	long lRes;
	string strSql;
	
	m_pLogger->trace("CDBWrap::queryPreInfoByHisID start...");

	//返回值，先预付为空
	strPreID = "";

	m_stPreInfo.strPreID = "";
	m_stPreInfo.strBsktID = "";
	
	m_stPreInfo.strMzNo = "";
	m_stPreInfo.strHisPreID = "";
	m_stPreInfo.strDepartmentName = "";
	m_stPreInfo.strFetchWindow = "";
	m_stPreInfo.strPatientName = "";
	m_stPreInfo.strPatientSex = "";
	m_stPreInfo.strPatientAge = "";
	
	strSql = "SELECT pat.outpatientNo, pre.prescriptionID, pre.prescriptionHisID, pre.basketID, pat.patientName, "
		" pat.patientAge, pat.patientSex, pre.departmentName, pre.prescriptionWindow ";
	
	strSql += " FROM tbl_prescription_info pre,tbl_patient_info pat ";
	
	strSql += " WHERE pre.patientID = pat.patientID " 
        " AND (pre.prescriptionHisID = '" + strHisID + "'"
		" OR pre.prescriptionID= '" + strHisID + "')";
	
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//查询处方的病人信息出错了
		m_pLogger->error(strSql);
		return lRes;
	}
	if (m_dbMgr.getRowNum()==0)
	{
		m_pLogger->debug("未查询到相关处方信息");	
	}
//	m_pLogger->debug("查询到相关处方信息");	
	if (m_dbMgr.getRowNum()!=0)
	{		
		m_stPreInfo.strPreID = m_dbMgr.getResultByFieldName("prescriptionID");
		m_stPreInfo.strBsktID = m_dbMgr.getResultByFieldName("basketID");
		
		m_stPreInfo.strMzNo = m_dbMgr.getResultByFieldName("outpatientNo");
		m_stPreInfo.strHisPreID = m_dbMgr.getResultByFieldName("prescriptionHisID");
		m_stPreInfo.strDepartmentName = m_dbMgr.getResultByFieldName("departmentName");
		m_stPreInfo.strFetchWindow = m_dbMgr.getResultByFieldName("prescriptionWindow");
		m_stPreInfo.strPatientName = m_dbMgr.getResultByFieldName("patientName");
		m_stPreInfo.strPatientSex = convertSex(  atoi(m_dbMgr.getResultByFieldName("patientSex").c_str())  );
		m_stPreInfo.strPatientAge = m_dbMgr.getResultByFieldName("patientAge");

		//返回通过HIS_ID查到本系统的Pre_ID
//		m_pLogger->debug("查询到相关处方信息:"+m_stPreInfo.strPreID+"");
		strPreID = m_stPreInfo.strPreID;
	//	m_pLogger->debug("查询到相关处方信息:"+strPreID+"");
	}
	m_dbMgr.freeResultSet();
	
	m_pLogger->debug("CDBWrap::queryPreInfoByHisID end...");
	
	return 0;
}

//HIS_ID-------->处方药品列表(单独使用)----传入通过HIS_ID查到本系统的Pre_ID
long CDBWrap::queryMedListByHisID(const string &strPreID)
{
	long lRes;
	string strSql;

	m_pLogger->trace("CDBWrap::queryMedListByHisID start...");
	
	/*!>
	属性：处方ID，发药数量，异形药片数量，服用天数，
	每天服用次数，每次服用数量，发药状态,		  
	药品ID，药品中文名，规格，厂家，药品大单位,
	位置ID,位置上的当前数量
	表：  tbl_prescription_medicine_info,tbl_medicine_info,tbl_address_locker
	*/
	strSql = "SELECT pre_med.prescriptionID, pre_med.medicineBoxNum, pre_med.medicineUnBoxNum,"
			 " pre_med.remedyMedicineNum,pre_med.distributeState, "
		     " med.medicineID, med.medicineProductName,med.medicineFormat, med.medicineFactoryName, "
		     " loc.positionID, loc.curCapacity, loc.alertValue, loc.medicineUnitName,med.medicineUnitName as unit_name,med.medicineSplitUnit,loc.medicineValiDate,loc.medicineBatchNumber";
				
	strSql += " FROM tbl_prescription_medicine_info pre_med, tbl_medicine_info med "
			  " LEFT JOIN "+m_locationName+" loc ON(med.medicineID = loc.medicineID)";
				
	strSql += " WHERE pre_med.medicineID = med.medicineID  "			  
		      " AND pre_med.prescriptionID = '" + strPreID + "' ";
	if(m_nUserAlone==0)
			strSql+=" AND pre_med.distributeState = 2";
	strSql += " ORDER BY   loc.curCapacity DESC,med.medicineID ASC";
	
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		return lRes;
	}

	if (m_stPreInfo.listFetchMedInfo.size())
	{
		m_stPreInfo.listFetchMedInfo.clear();
	}

	string strOldMedID = "";//记录之前的药品ID，以便区分是否更换了药品
	string stroldvalidtime="";
	int nCurDispMedSum = 0;	//当前已分发的药品数量
	bool bHadFinish = false;//分发足够了，则完成

	FetchMedInfo stFMI;
	bool NumFlag=false;
	bool HeterFlag=false;


	
	CTime Curtm=CTime::GetCurrentTime();
	CString timestr="";
	int   a,b,c ;
	CTimeSpan ts;
	CString strCurNun="";
   
	while (!m_dbMgr.isEndResult())
	{
	
		//同一种药品
		if (strOldMedID == m_dbMgr.getResultByFieldName("medicineID"))
		{
			//同一种药品，够了，忽略掉
			if (NumFlag==true&&HeterFlag==true)
			{
				m_dbMgr.nextResult();
				continue;
			}
			//同一种药品不同有效期 跳过
			if(IsValidTime==2)
			{
				if (stroldvalidtime!=m_dbMgr.getResultByFieldName("medicineValiDate")&&stroldvalidtime!="")
				{
					m_dbMgr.nextResult();
					continue;
				}
			}
			
		}
		else//交替了一种药品复位
		{
			NumFlag=false;
			HeterFlag=false;
			stroldvalidtime="";
		}
		memset(&stFMI, 0, sizeof(FetchMedInfo));
		
		/*
			处方ID，发药数量，异形药片数量，服用天数，每天服几次，每次服用数量，
			每天服用次数，每次服用数量，发药状态,		  
			药品ID，药品中文名，规格，厂家，药品大单位,
		    位置ID,位置上的当前数量
		*/
	
		stFMI.strFetchNum = m_dbMgr.getResultByFieldName("medicineBoxNum");
		stFMI.strHeterNum = m_dbMgr.getResultByFieldName("medicineUnBoxNum");
		stFMI.strMedID = m_dbMgr.getResultByFieldName("medicineID");
		stFMI.strName = m_dbMgr.getResultByFieldName("medicineProductName");
		stFMI.strUnit = m_dbMgr.getResultByFieldName("medicineFormat");
		stFMI.strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");
	//	stFMI.strTypeUnit = m_dbMgr.getFieldValueByName("medicineUnitName");
		stFMI.strLocMedUnitName=m_dbMgr.getResultByFieldName("medicineUnitName");
		stFMI.strLocID = m_dbMgr.getResultByFieldName("positionID");
		stFMI.strCurQuantity = m_dbMgr.getResultByFieldName("curCapacity");
		stFMI.strWarnThreshold = m_dbMgr.getResultByFieldName("alertValue");

		stFMI.strvalidatetime=m_dbMgr.getResultByFieldName("medicineValiDate");		
		stFMI.strmedbatch=m_dbMgr.getResultByFieldName("medicineBatchNumber");

		if (atoi(stFMI.strLocID.c_str())<1||stFMI.strLocID=="")
		{
			m_dbMgr.nextResult();
			continue;
		}
		if (atoi(stFMI.strFetchNum.c_str())>0)
		{
			if (m_dbMgr.getResultByFieldName("unit_name")==stFMI.strLocMedUnitName)
			{	
				if (IsValidTime>0)
				{		
					timestr="";
					timestr=stFMI.strvalidatetime.c_str();
					sscanf(timestr.GetBuffer(timestr.GetLength()),"%d-%d-%d",&a,&b,&c);   
					CTime   time(a,b,c,0,0,0);  		
					ts=time-Curtm;
					if (ts.GetDays()>ValidControlMonth*30)
					{
						m_stPreInfo.listFetchMedInfo.push_back(stFMI);
					}
				}
				else
					m_stPreInfo.listFetchMedInfo.push_back(stFMI);
				NumFlag=true;
			}
		}
		else
			NumFlag=true;
		if (atoi(stFMI.strHeterNum.c_str())>0)
		{
			if (m_dbMgr.getResultByFieldName("medicineSplitUnit")==stFMI.strLocMedUnitName)
			{		
			//	stFMI.strFetchNum=stFMI.strHeterNum;
				if (IsValidTime>0)
				{		
					timestr="";
					timestr=stFMI.strvalidatetime.c_str();
					sscanf(timestr.GetBuffer(timestr.GetLength()),"%d-%d-%d",&a,&b,&c);   
					CTime   time(a,b,c,0,0,0);  		
					ts=time-Curtm;
					if (ts.GetDays()>ValidControlMonth*30)
					{
						stFMI.strFetchNum="0";
						m_stPreInfo.listFetchMedInfo.push_back(stFMI);
					}
				}
				else
				{
					stFMI.strFetchNum="0";
					m_stPreInfo.listFetchMedInfo.push_back(stFMI);
				}
/*				m_stPreInfo.listFetchMedInfo.push_back(stFMI)*/;
				HeterFlag=true;
			}
		}
		else
			HeterFlag=true;
		strOldMedID = m_dbMgr.getResultByFieldName("medicineID");
		stroldvalidtime=m_dbMgr.getResultByFieldName("medicineValiDate");		
		m_dbMgr.nextResult();
	}
	m_dbMgr.freeResultSet();

	m_pLogger->trace("CDBWrap::queryMedListByHisID end...");
	
	return 0;
}

//处方取药--模块，根据HIS_ID，获取处方信息和处方药品列表（单独使用）
long CDBWrap::queryPreInfoMedListByHisID(const string &strHisID)
{
	long lRes;

	string strPreID;

	m_pLogger->trace("CDBWrap::queryPreInfoMedListByHisID start...");

	lRes = queryPreInfoByHisID(strHisID, strPreID);
	if (lRes)
		return lRes;
	lRes = queryMedListByHisID(strPreID);
	if (lRes)
			return lRes;
	m_pLogger->trace("CDBWrap::queryPreInfoMedListByHisID end...");

	return 0;
}



long CDBWrap::queryBatchAddMedInfoList(int sorttype)
{
	m_pLogger->trace("CDBWrap::queryBatchAddMedInfoList start...");

	m_listMedInfoBatchAdd.clear();

	string strSql;

	strSql = "SELECT med.medicineID,loc.medicineValiDate,loc.medicineBatchNumber, med.medicineHisID,med.medicineProductName,"
			" med.medicineFormat, med.medicineFactoryName, loc.positionID, loc.curCapacity, loc.alertValue,loc.maxCapacity,loc.medicineUnitName ";

	strSql += "  FROM tbl_medicine_info med left JOIN "+m_locationName+" loc ON(med.medicineID = loc.medicineID) ";

	strSql += " WHERE loc.curCapacity < loc.alertValue";  // ？(当前量 < 告警百分比)

//	strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
	if(sorttype==0)		
		strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
	if(sorttype==1)		
		strSql += " ORDER BY loc.positionID desc";
	if(sorttype==2)		
		strSql += " ORDER BY loc.positionID ";
	if(sorttype==3)		
		strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
	if(sorttype==4)		
		strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) ";
	if(sorttype==5)		
		strSql += " ORDER BY loc.curCapacity desc";
	if(sorttype==6)		
			strSql += " ORDER BY loc.curCapacity ";
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}

	if (0 == m_dbMgr.getRowNum())
	{
		//没有查到该处方的药品列表信息；

		//发送PostMessage()消息给UI界面，显示没有查询到匹配的记录项;
		return 1;
	}

	MedInfo stMI;
	while (!m_dbMgr.isEndResult())
	{
		stMI.strCurQuantity = m_dbMgr.getResultByFieldName("curCapacity");
		stMI.strWarnThreshold = m_dbMgr.getResultByFieldName("alertValue");
		if (_ttoi(m_dbMgr.getResultByFieldName("curCapacity").c_str())<_ttoi(m_dbMgr.getResultByFieldName("alertValue").c_str()))
		{
			stMI.strMedID = m_dbMgr.getResultByFieldName("medicineID");
			stMI.strHisID = m_dbMgr.getResultByFieldName("medicineHisID");
			stMI.strChName = m_dbMgr.getResultByFieldName("medicineProductName");
			stMI.strUnit = m_dbMgr.getResultByFieldName("medicineFormat");
			stMI.strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");			
			stMI.strLocID = m_dbMgr.getResultByFieldName("positionID");		
			stMI.strMaxCapacity = m_dbMgr.getResultByFieldName("maxCapacity");
			stMI.strLocMedUnitName=m_dbMgr.getResultByFieldName("medicineUnitName");
			stMI.strvalidatetime=m_dbMgr.getResultByFieldName("medicineValiDate");
			stMI.strmedbatch=m_dbMgr.getResultByFieldName("medicineBatchNumber");
			m_listMedInfoBatchAdd.push_back(stMI);
		}
		m_dbMgr.nextResult();
	}
	m_dbMgr.freeResultSet();

	m_pLogger->trace("CDBWrap::queryBatchAddMedInfoList end...");

	return 0;
}


//根据药品名称，或药品条码，获取该药品的存储信息;
long CDBWrap::queryMedManInfoListByBarcode(string strBarcode)
{
	m_pLogger->trace("CDBWrap::queryMedManInfoList start...");
	
	string strSql;
	if (strBarcode.empty()==true)
	{
		strSql = "SELECT med.medicineID, med.medicineHisID,loc.medicineValiDate,loc.medicineBatchNumber, med.medicineManDefinedCode, med.medicineProductName, med.medicinePinYinCode, med.medicineFormat, med.medicineFactoryName, loc.positionID, loc.curCapacity, loc.alertValue,loc.maxCapacity,med.medicineType,loc.medicineUnitName";
		
		strSql += "  FROM  tbl_medicine_info med  ";
		
		strSql +=" left join "+m_locationName+" loc on (med.medicineID=loc.medicineID)";
		
		strSql +=" WHERE (med.medicineType='"+General::integertostring(m_machinetype)+"' OR med.medicineType='0')";

		strSql += " AND med.medicineManDefinedCode = " + strBarcode;
		
		strSql += " ORDER BY loc.positionID";
	}
	else
	{
		strSql = "SELECT med.medicineID, med.medicineHisID,loc.medicineValiDate,loc.medicineBatchNumber, med.medicineManDefinedCode,"
				" med.medicineProductName,med.medicinePinYinCode, med.medicineFormat, med.medicineFactoryName, loc.positionID, loc.curCapacity,"
				" loc.alertValue,loc.maxCapacity,med.medicineType,loc.medicineUnitName";
		
		strSql += "  FROM  tbl_medicine_info med  ";
		
		strSql +=" left join "+m_locationName+" loc on (med.medicineID=loc.medicineID)";
		
		strSql +=" WHERE (med.medicineType='"+General::integertostring(m_machinetype)+"' OR med.medicineType='0')";
		
		strSql += " AND med.medicineManDefinedCode = " + strBarcode;
				
		strSql += " ORDER BY loc.positionID";
	}
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}
	
	m_listMedInfoMedMan.clear();
	
	MedInfo stMI;
	while (!m_dbMgr.isEndResult())
	{
		stMI.strMedID = m_dbMgr.getResultByFieldName("medicineID");
		stMI.strHisID = m_dbMgr.getResultByFieldName("medicineHisID");
		stMI.strPyCode = m_dbMgr.getResultByFieldName("medicinePinYinCode");
		stMI.strChName = m_dbMgr.getResultByFieldName("medicineProductName");
		stMI.strUnit = m_dbMgr.getResultByFieldName("medicineFormat");
		stMI.strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");
		
		stMI.strLocID = m_dbMgr.getResultByFieldName("positionID");
		stMI.strCurQuantity = m_dbMgr.getResultByFieldName("curCapacity");
		stMI.strWarnThreshold = m_dbMgr.getResultByFieldName("alertValue");
		stMI.strMaxCapacity = m_dbMgr.getResultByFieldName("maxCapacity");
		stMI.strMedType= m_dbMgr.getResultByFieldName("medicineType");
		stMI.strLocMedUnitName= m_dbMgr.getResultByFieldName("medicineUnitName");
		
		stMI.strvalidatetime=m_dbMgr.getResultByFieldName("medicineValiDate");
		stMI.strmedbatch=m_dbMgr.getResultByFieldName("medicineBatchNumber");
		m_listMedInfoMedMan.push_back(stMI);
		
		m_dbMgr.nextResult();		
	}
	m_dbMgr.freeResultSet();

	m_pLogger->trace("CDBWrap::queryMedManInfoList end...");
	
	return 0;
}


//查询所有的空闲的位置；
long CDBWrap::queryFreeLocID()
{
	long lRet;
	string strSql;
	int nLocID;
	
	m_pLogger->trace("CDBWrap::queryFreeLocID start...");	

	m_listFreeLocID.clear();

	strSql = "SELECT  positionID";
	
	strSql +=" FROM  "+m_locationName+" ";
	
	//positionState：为0，药槽已坏停用了，为1是好的可用
	//medicineID：为0，所对应的药槽不放药，其他值:所对应的药槽id存放对应药品id
	strSql += " WHERE positionState = 1 AND medicineID = 0";

	strSql += " ORDER BY positionID";
	
	lRet = m_dbMgr.RunSQL(strSql);
	if (lRet)
	{
		//
		return lRet;
	}
	
	
	while (!m_dbMgr.isEndResult())
	{		
		nLocID = atoi(m_dbMgr.getResultByFieldName("positionID").c_str());

		m_listFreeLocID.push_back(nLocID);

		m_dbMgr.nextResult();
	}
	m_dbMgr.freeResultSet();
	
	m_pLogger->trace("CDBWrap::queryFreeLocID end...");
	
	return 0;
}

long CDBWrap::boundMedIdByLocID(int nLocID, int nMedID,string medunitname)
{
	long lRes;
	string strSql;

	m_pLogger->trace("CDBWrap::boundMedIdByLocID start...");

	strSql="select deviceID,medicineType from tbl_medicine_info where medicineID ='"+General::integertostring(nMedID)+"' ";

	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		m_pLogger->error("执行sql语句出错..boundMedIdByLocID "+strSql);
		return lRes;
	}

	int macid=0;
	string  mactype="";
	string  med_id="";
	if (m_dbMgr.getRowNum()!=0)
	{
		macid=_ttoi(m_dbMgr.getResultByFieldIndex(0).c_str());
		mactype=m_dbMgr.getResultByFieldIndex(1).c_str();
	}
	m_dbMgr.freeResultSet();

	macid=General::AddMachineID(macid,m_machineID);
	if(mactype!=General::integertostring(m_machinetype))
		mactype=General::integertostring(m_machinetype);
	strSql="update tbl_medicine_info set deviceID='"+General::integertostring(macid)+"',medicineType='"+mactype+"' ";
	strSql+=" where medicineID='"+General::integertostring(nMedID)+"'";

	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		m_pLogger->error("执行sql语句出错..boundMedIdByLocID "+strSql);
		return lRes;
	}
	//更新 地址表	
	strSql  = "UPDATE "+m_locationName+" SET medicineID = " + General::integertostring(nMedID);		  
	strSql += ", deviceID = '"+General::integertostring(m_machineID)+"', positionType = '"+General::integertostring(m_machinetype)+"', curCapacity = 0, maxCapacity = 99, alertValue = 60, medicineUnitName='"+medunitname+"'";

	strSql += " WHERE positionID = " + General::integertostring(nLocID);

	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		m_pLogger->error("执行sql语句出错..boundMedIdByLocID "+strSql);
		return lRes;
	}

	m_pLogger->trace("CDBWrap::boundMedIdByLocID end...");

	return 0;
}

//根据LOC_ID,解除一个药品
long CDBWrap::unboundMedIdByLocID(int nLocID)
{
	long lRes;
	string strSql;
	
	m_pLogger->trace("CDBWrap::unboundMedIdByLocID start...");
	strSql ="select med.deviceID,med.medicineType,med.medicineID from  "+m_locationName;
	strSql+=" loc, tbl_medicine_info med where loc.medicineID=med.medicineID and loc.positionID= ";
	strSql+= General::integertostring(nLocID);
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		m_pLogger->error("执行sql语句出错..unboundMedIdByLocID "+strSql);
		return lRes;
	}
	int macid=0;
	string  mactype="";
	string  med_id="";
	if (m_dbMgr.getRowNum()!=0)
	{
		macid=_ttoi(m_dbMgr.getResultByFieldIndex(0).c_str());
		mactype=m_dbMgr.getResultByFieldIndex(1).c_str();
		med_id=m_dbMgr.getResultByFieldIndex(2).c_str();
	}
	m_dbMgr.freeResultSet();

	macid=General::RemoveMachineID(macid,m_machineID);
	if (macid==0)
		mactype="0";
	strSql="update tbl_medicine_info set deviceID='"+General::integertostring(macid)+"',medicineType='"+mactype+"'";
	strSql+=" where medicineID='"+med_id+"'";
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		m_pLogger->error("执行sql语句出错..unboundMedIdByLocID "+strSql);
		return lRes;
	}

	
	strSql  = "UPDATE "+m_locationName+" SET medicineID = 0,";		  
	strSql += " deviceID =0, positionType = 0, curCapacity = 0, maxCapacity = 0, alertValue = 0,medicineUnitName=''";
	
	strSql += " WHERE positionID = " + General::integertostring(nLocID);
	
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		
		return lRes;
	}
	
	m_pLogger->trace("CDBWrap::unboundMedIdByLocID end...");
	
	return 0;
}

long CDBWrap::queryMedIsExistByMedBarcode(string strBarcode, bool &bExist, string &strMedID, 
				string &strMedPyCode, string &strMedChName, string &strMedUnit, string &strFactoryName)
{
	long lRes;
	string strSql;
	
	m_pLogger->trace("CDBWrap::queryMedIsExistByMedBarcode start...");
	
	strSql  = "Select medicineID, medicineProductName, medicinePinYinCode, medicineFormat,medicineFactoryName ";		  
	strSql += " FROM tbl_medicine_info ";
	
	strSql += " WHERE medicineManDefinedCode = " + strBarcode;

	//2014-11-03修改
//	strSql += " LIMIT 1";  
	m_pLogger->trace(strSql);
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//ERROR
		return lRes;
	}

	if (m_dbMgr.getRowNum())
	{
		bExist = true;

		strMedID = m_dbMgr.getResultByFieldName("medicineID");
		strMedPyCode = m_dbMgr.getResultByFieldName("medicinePinYinCode");
		strMedChName = m_dbMgr.getResultByFieldName("medicineProductName");
		strMedUnit = m_dbMgr.getResultByFieldName("medicineFormat");
		strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");
	}
	else
	{
		//没有查到药品信息；
		bExist = false;
		strMedID = "";
		strMedPyCode = "";
		strMedChName = "";
		strMedUnit = "";
		strFactoryName = "";
	}
	m_dbMgr.freeResultSet();
	
	m_pLogger->trace("CDBWrap::queryMedIsExistByMedBarcode end...");
	
	return 0;
}

//指定位置添加药数
long CDBWrap::increaseMedNumByLocID(int nLocID, int nCurQt, int nIncNum)
{
	m_pLogger->trace("CDBWrap::IncreaseMedNumByLocID start...");

	int nNewQt = 0;

	nNewQt = nCurQt + nIncNum;

	string strSql;
	int maxnum=0;
	strSql ="select maxCapacity from "+m_locationName+" WHERE positionID =";
	strSql += General::integertostring(nLocID);
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}
	if (m_dbMgr.getRowNum()==0)
	{
		return 1;
	}
	else
	{
		maxnum=_ttoi(m_dbMgr.getResultByFieldIndex(0).c_str());
	}
	m_dbMgr.freeResultSet();

	nNewQt>maxnum?maxnum:nNewQt;
	strSql = "UPDATE "+m_locationName+" ";
		
	strSql += " SET curCapacity = " + General::integertostring(nNewQt);
	
	strSql +=  " WHERE positionID = " + General::integertostring(nLocID);

	 lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}

	m_pLogger->trace("CDBWrap::IncreaseMedNumByLocID end...");

	return 0;
}

//指定位置减少药数
long CDBWrap::decreaseMedNumByLocID(int nLocID, int nCurQt, int nDecNum)
{
	m_pLogger->trace("CDBWrap::DecreaseMedNumByLocID start...");
	
	int nNewQt = 0;
	
	nNewQt = nCurQt - nDecNum;
	if (nNewQt<0)
	{
		nNewQt=0;
	}
	
	string strSql;
	
	strSql = "UPDATE "+m_locationName+" ";
		
	strSql += " SET curCapacity = " + General::integertostring(nNewQt);
	
	strSql +=  " WHERE positionID = " + General::integertostring(nLocID);
	
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}

	m_pLogger->trace("CDBWrap::DecreaseMedNumByLocID end...");
	
	return 0;
}


long CDBWrap::queryMedIdByName(string strMedId, string strMedName)
{
	m_pLogger->trace("CDBWrap::queryMedIdByName start...");
	
	string strSql;
	
	strSql = "SELECT  medicineID ";
	
	strSql += " FROM  tbl_medicine_info ";
	
	strSql +=  " WHERE medicineProductName = '" + strMedName + "' LIMIT 1";
	
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}

	strMedId = "";
	if (m_dbMgr.getRowNum())
	{
		strMedId = m_dbMgr.getResultByFieldName("medicineID");
	}
	m_dbMgr.freeResultSet();
	m_pLogger->trace("CDBWrap::queryMedIdByName end...");
	
	return 0;
}

long CDBWrap::vogueSerchMedInfoListMedMan(string strMedPyCodePart,int sorttype)
{
	m_pLogger->trace("CDBWrap::vogueSerchMedInfoListMedMan start...");

	m_listMedInfoMedMan.clear();
	string strSql;
	if (strMedPyCodePart.empty()==true)
	{
		strSql = "SELECT med.medicineID,loc.medicineValiDate,loc.medicineBatchNumber, med.medicineHisID, med.medicineManDefinedCode, med.medicineProductName, med.medicinePinYinCode, med.medicineFormat, med.medicineFactoryName, loc.positionID, loc.curCapacity, loc.alertValue,loc.maxCapacity,med.medicineType,loc.medicineUnitName";
		
		strSql += "  FROM  tbl_medicine_info med  ";
		
		strSql +=" left join "+m_locationName+" loc on (med.medicineID=loc.medicineID)";
		
		strSql +=" WHERE (med.medicineType='"+General::integertostring(m_machinetype)+"' OR med.medicineType='0')";
		if(sorttype==0||sorttype==2)		
			strSql += " ORDER BY loc.positionID ";
		if(sorttype==1)		
			strSql += " ORDER BY loc.positionID desc";
		if(sorttype==3)		
			strSql += " ORDER BY loc.medicineValiDate desc";
		if(sorttype==4)		
			strSql += " ORDER BY loc.medicineValiDate ";
		if(sorttype==5)		
			strSql += " ORDER BY loc.curCapacity desc";
		if(sorttype==6)		
			strSql += " ORDER BY loc.curCapacity ";

	}
	else
	{
		strSql = "SELECT med.medicineID, loc.medicineValiDate,loc.medicineBatchNumber , med.medicineHisID, med.medicineManDefinedCode,"
				" med.medicineProductName, med.medicinePinYinCode, med.medicineFormat, med.medicineFactoryName, loc.positionID, loc.curCapacity, loc.alertValue,loc.maxCapacity,med.medicineType,loc.medicineUnitName";
		
		strSql += "  FROM  tbl_medicine_info med  ";
		
		strSql +=" left join "+m_locationName+" loc on (med.medicineID=loc.medicineID)";
		
		strSql +=" WHERE (med.medicineType='"+General::integertostring(m_machinetype)+"' OR med.medicineType='0')";
		
		strSql += " AND ( med.medicinePinYinCode like '%" + strMedPyCodePart + "%'";
		
		strSql += " OR med.medicineProductName like '%" + strMedPyCodePart + "%'";
		
		strSql += " OR med.medicineGenericName like '%" + strMedPyCodePart + "%' )";
		
		if(sorttype==0||sorttype==2)		
			strSql += " ORDER BY loc.positionID ";
		if(sorttype==1)		
			strSql += " ORDER BY loc.positionID desc";
		if(sorttype==3)		
			strSql += " ORDER BY loc.medicineValiDate desc";
		if(sorttype==4)		
			strSql += " ORDER BY loc.medicineValiDate ";
		if(sorttype==5)		
			strSql += " ORDER BY loc.curCapacity desc";
		if(sorttype==6)		
			strSql += " ORDER BY loc.curCapacity ";
	}
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}
	bool bflag=false;
	if (0 == m_dbMgr.getRowNum())
	{
		//没有查到该处方的药品列表信息；
		return 0;
	}
	
	MedInfo stMI;
	while (!m_dbMgr.isEndResult())
	{
		stMI.strMedID = m_dbMgr.getResultByFieldName("medicineID");
		stMI.strHisID = m_dbMgr.getResultByFieldName("medicineHisID");
		stMI.strBarcode = m_dbMgr.getResultByFieldName("medicineManDefinedCode");
		stMI.strChName = m_dbMgr.getResultByFieldName("medicineProductName");
		stMI.strPyCode = m_dbMgr.getResultByFieldName("medicinePinYinCode");
		stMI.strUnit = m_dbMgr.getResultByFieldName("medicineFormat");
		stMI.strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");
		stMI.strLocID = m_dbMgr.getResultByFieldName("positionID");
		stMI.strCurQuantity = m_dbMgr.getResultByFieldName("curCapacity");
		stMI.strWarnThreshold = m_dbMgr.getResultByFieldName("alertValue");
		stMI.strMaxCapacity = m_dbMgr.getResultByFieldName("maxCapacity");	
		stMI.strMedType= m_dbMgr.getResultByFieldName("medicineType");
		stMI.strLocMedUnitName=m_dbMgr.getResultByFieldName("medicineUnitName");
		stMI.strvalidatetime=m_dbMgr.getResultByFieldName("medicineValiDate");
		stMI.strmedbatch=m_dbMgr.getResultByFieldName("medicineBatchNumber");
		m_listMedInfoMedMan.push_back(stMI);	
		m_dbMgr.nextResult();	
	}
	m_dbMgr.freeResultSet();

	m_pLogger->trace("CDBWrap::vogueSerchMedInfoListMedMan start...");

	return 0;
}

//通过处方ID，获取篮子上绑定的RFID;
long CDBWrap::queryRFID_ByPreID(string &strLeft, string &strRight, const string &strPreID)
{
	m_pLogger->trace("CDBWrap::queryRFID_ByPreID start...");

	long lRet;
	string strSql;

	strSql = "SELECT radioFreqency1, radioFreqency2 ";
	
	strSql +=" FROM tbl_prebasket_info ";
	
	strSql += " WHERE preID = " + strPreID;
	
	lRet = m_dbMgr.RunSQL(strSql);
	if (lRet)
	{
		//
		return lRet;
	}
	
	
	while (!m_dbMgr.isEndResult())
	{		
		strLeft = m_dbMgr.getResultByFieldName("radioFreqency1");
		strRight = m_dbMgr.getResultByFieldName("radioFreqency2");
	}
	m_dbMgr.freeResultSet();
	m_pLogger->trace("CDBWrap::queryRFID_ByPreID end...");
	
	return 0;
}


long CDBWrap::modifyWarnByLocID(int nLocID, int nNewWarn)
{
	m_pLogger->trace("CDBWrap::modifyWarnByLocID start...");

	long lRes;

	string strSql;

	strSql = "UPDATE "+m_locationName+" ";

	strSql += " SET alertValue = " + General::integertostring(nNewWarn);

	strSql += " WHERE positionID = " + General::integertostring(nLocID);

	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}

	m_pLogger->trace("CDBWrap::modifyWarnByLocID end...");
	return 0;
}

long CDBWrap::modifyMaxCapacityByLocID(int nLocID, int nNewMaxCapacity)
{
	m_pLogger->trace("CDBWrap::modifyMaxCapacityByLocID start...");
	
	long lRes;
	
	string strSql;
	
	strSql = "UPDATE "+m_locationName+" ";
	
	strSql += " SET maxCapacity = " + General::integertostring(nNewMaxCapacity);
	
	strSql += " WHERE positionID = " + General::integertostring(nLocID);
	
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}
	
	m_pLogger->trace("CDBWrap::modifyMaxCapacityByLocID end...");
	return 0;
}


long CDBWrap::modifyCurQtByLocID(int nLocID, int nNewCurQt)
{
	m_pLogger->trace("CDBWrap::modifyMaxCurQtByLocID start...");
	
	long lRes;
	
	string strSql;
	
	strSql = "UPDATE "+m_locationName+" ";
	
	strSql += " SET curCapacity = " + General::integertostring(nNewCurQt);
	
	strSql += " WHERE positionID = " + General::integertostring(nLocID);
	
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}
	
	m_pLogger->trace("CDBWrap::modifyMaxCurQtByLocID end...");
	return 0;
}

long CDBWrap::boundLocForMed(int nLocID, int nMedID)
{
	m_pLogger->trace("CDBWrap::boundLocForMed start...");
	
	long lRes;
	
	string strSql;
	
	strSql = "UPDATE "+m_locationName+" ";
	
	strSql += " SET medicineID = " + General::integertostring(nMedID);
	strSql += ", deviceID = 2";
	strSql += ", positionType = 1";
	strSql += ", curCapacity = 0";
	strSql += ", maxCapacity = 99";
	strSql += ", alertValue = 30 ";
	
	strSql += " WHERE positionID = " + General::integertostring(nLocID);
	
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}
	
	m_pLogger->trace("CDBWrap::boundLocForMed end...");
	return 0;
}


long CDBWrap::unboundLocForMed(int nLocID)
{
	m_pLogger->trace("CDBWrap::unboundLocForMed start...");
	
	long lRes;
	
	string strSql;
	
	strSql = "UPDATE "+m_locationName+" ";
	
	strSql += " SET medicineID = 0";
	strSql += ", deviceID = 0";
	strSql += ", positionType = 0";
	strSql += ", curCapacity = 0";
	strSql += ", maxCapacity = 0";
	strSql += ", alertValue = 0 ";
	
	strSql += " WHERE positionID = " + General::integertostring(nLocID);
	
	lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		return lRes;
	}
	
	m_pLogger->trace("CDBWrap::unboundLocForMed end...");
	return 0;
}

string CDBWrap::Convert2Str(int nLocID)
{
	string strRet;
	
	int nSingleSum = 0;
	int nTierNum=0;//从1开始
	int nSeqNum=0;//从1开始
	stringstream strstrDes;//描述字符串
	
	nSingleSum = m_nTierNum * m_nTierCapacity;
	
	if (0 == nLocID )
	{
		//出错了
		strRet = configPar.getValueParameter("No_use").c_str();
		return strRet;
	}

	if ((nSingleSum*2) < nLocID)
	{
		//出错了
		strRet = configPar.getValueParameter("Beyond_border").c_str();
		return strRet;
	}
	
	if (nLocID > nSingleSum)
	{
		strstrDes << configPar.getValueParameter("Lattice_2").c_str();
		
		nTierNum = (nLocID - nSingleSum - 1)/m_nTierCapacity + 1;
		nSeqNum = (nLocID -  nSingleSum - 1)%m_nTierCapacity + 1;		
	}
	else
	{
		strstrDes << configPar.getValueParameter("Lattice_1").c_str();
		
		nTierNum = (nLocID - 1)/m_nTierCapacity + 1;
		nSeqNum = (nLocID - 1)%m_nTierCapacity + 1;
	}
	strstrDes << " ";
	strstrDes << nTierNum;
	strstrDes << " ";
	
	strstrDes << configPar.getValueParameter("Layer").c_str();
	strstrDes << " ";

	strstrDes << nSeqNum << " "<< configPar.getValueParameter("Groove").c_str();
	
	strRet = strstrDes.str();
	
	return strRet;
}

int CDBWrap::Convert2Int(string strLocID)
{
	int nRet = 0;
	int nSingleSum = 0;
	int nChestNum=0;
	int nTierNum=0;//从1开始
	int nSeqNum=0;//从1开始
	
	//用来吃掉汉字描述部分
	string strDes;
	
	istringstream instrstr(strLocID);
	
	nSingleSum = m_nTierNum * m_nTierCapacity;
	
	instrstr>>strDes>>nChestNum>>strDes>>nTierNum>>strDes>>nSeqNum>>strDes;
	
	if (1 == nChestNum)
	{
		nRet += 0;
	}
	else if (2 == nChestNum)
	{
		nRet += nSingleSum;
	}
	else
	{
		//出错了
		return 0;
	}
	
	nRet += (nTierNum-1)*m_nTierCapacity + nSeqNum;
	
	return nRet;
}

long CDBWrap::QueryMedName( list<string>& listname )
{
	try
	{
		string sql="select medicineProductName from tbl_medicine_info where (medicineType='"+General::integertostring(m_machinetype)+"')";
		sql+=" or (medicineType='0')";
		sql+=" order by medicineProductName;";
		int res=m_dbMgr.RunSQL(sql);
		if (res!=0)
		{
			m_pLogger->error("QueryMedName 执行sql语句错误:"+sql);
			return res;
		}
		listname.clear();
		while(!m_dbMgr.isEndResult())
		{
			listname.push_back(m_dbMgr.getResultByFieldIndex(0));
			m_dbMgr.nextResult();
		}
		m_dbMgr.freeResultSet();
		return 0;
	}
	catch(...)
	{
		m_pLogger->error("CDBWrap::QueryMedName function catch error...");
		return -1;
	}
	return 0;
}

long CDBWrap::QueryLackMedInfoList( string strMedPyCodePart,int sorttype )
{
	m_pLogger->trace("CDBWrap::QueryLackMedInfoList start...");
	
	m_listMedInfoMedMan.clear();
	string strSql;
	if (strMedPyCodePart.empty()==true)
	{
		strSql = "SELECT med.medicineID,loc.medicineValiDate,loc.medicineBatchNumber, med.medicineHisID, med.medicineManDefinedCode,"
				" med.medicineProductName, med.medicinePinYinCode, med.medicineFormat, med.medicineFactoryName, loc.positionID, loc.curCapacity, loc.alertValue,loc.maxCapacity,med.medicineType,loc.medicineUnitName";
		
		strSql += "  FROM  tbl_medicine_info med  ";
		
		strSql +=" right join "+m_locationName+" loc on (med.medicineID=loc.medicineID)";
		
		strSql +=" WHERE (med.medicineType='"+General::integertostring(m_machinetype)+"' OR med.medicineType='0')";
		
		strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";

		if(sorttype==0)		
			strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
		if(sorttype==1)		
			strSql += " ORDER BY loc.positionID desc";
		if(sorttype==2)		
			strSql += " ORDER BY loc.positionID ";
		if(sorttype==3)		
			strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
		if(sorttype==4)		
			strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) ";
		if(sorttype==5)		
			strSql += " ORDER BY loc.curCapacity desc";
		if(sorttype==6)		
			strSql += " ORDER BY loc.curCapacity ";
	}
	else
	{
		strSql = "SELECT med.medicineID, med.medicineHisID,loc.medicineValiDate,loc.medicineBatchNumber, med.medicineManDefinedCode,"
				" med.medicineProductName, med.medicinePinYinCode, med.medicineFormat, med.medicineFactoryName, loc.positionID, loc.curCapacity, loc.alertValue,loc.maxCapacity,med.medicineType,loc.medicineUnitName";
		
		strSql += "  FROM  tbl_medicine_info med  ";
		
		strSql +=" right join "+m_locationName+" loc on (med.medicineID=loc.medicineID)";
		
		strSql +=" WHERE (med.medicineType='"+General::integertostring(m_machinetype)+"' OR med.medicineType='0')";
		
		strSql += " AND ( med.medicinePinYinCode like '%" + strMedPyCodePart + "%'";
		
		strSql += " OR med.medicineProductName like '%" + strMedPyCodePart + "%'";
		
		strSql += " OR med.medicineGenericName like '%" + strMedPyCodePart + "%' )";
		
		if(sorttype==0)		
			strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
		if(sorttype==1)		
			strSql += " ORDER BY loc.positionID desc";
		if(sorttype==2)		
			strSql += " ORDER BY loc.positionID ";
		if(sorttype==3)		
			strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
		if(sorttype==4)		
			strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) ";
		if(sorttype==5)		
			strSql += " ORDER BY loc.curCapacity desc";
		if(sorttype==6)		
			strSql += " ORDER BY loc.curCapacity ";
	}
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		m_pLogger->error(strSql);
		return lRes;
	}
	bool bflag=false;
	if (0 == m_dbMgr.getRowNum())
	{
		//没有查到该处方的药品列表信息；
		return 1;
	}
	m_listMedInfoMedMan.clear();
	MedInfo stMI;
	while (!m_dbMgr.isEndResult())
	{
		stMI.strMedID = m_dbMgr.getResultByFieldName("medicineID");
		stMI.strHisID = m_dbMgr.getResultByFieldName("medicineHisID");
		stMI.strBarcode = m_dbMgr.getResultByFieldName("medicineManDefinedCode");
		stMI.strChName = m_dbMgr.getResultByFieldName("medicineProductName");
		stMI.strPyCode = m_dbMgr.getResultByFieldName("medicinePinYinCode");
		stMI.strUnit = m_dbMgr.getResultByFieldName("medicineFormat");
		stMI.strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");
		stMI.strLocID = m_dbMgr.getResultByFieldName("positionID");
		stMI.strCurQuantity = m_dbMgr.getResultByFieldName("curCapacity");
		stMI.strWarnThreshold = m_dbMgr.getResultByFieldName("alertValue");
		stMI.strMaxCapacity = m_dbMgr.getResultByFieldName("maxCapacity");	
		stMI.strMedType= m_dbMgr.getResultByFieldName("medicineType");
		stMI.strLocMedUnitName=m_dbMgr.getResultByFieldName("medicineUnitName");
		
		stMI.strvalidatetime=m_dbMgr.getResultByFieldName("medicineValiDate");
		stMI.strmedbatch=m_dbMgr.getResultByFieldName("medicineBatchNumber");
		m_listMedInfoMedMan.push_back(stMI);
		
		m_dbMgr.nextResult();
		
	}
	m_dbMgr.freeResultSet();
	m_pLogger->trace("CDBWrap::QueryLackMedInfoList start...");
	
	return 0;
}

long CDBWrap::QueryLackMedName( list<string>& listname )
{
	try
	{
		string strSql="";
		strSql = "SELECT distinct(med.medicineProductName)";
		
		strSql += "  FROM  tbl_medicine_info med  ";
		
		strSql +=" right join "+m_locationName+" loc on (med.medicineID=loc.medicineID)";
		
		strSql +=" WHERE (med.medicineType='"+General::integertostring(m_machinetype)+"' OR med.medicineType='0')";
			
		strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
		int res=m_dbMgr.RunSQL(strSql);
		if (res!=0)
		{
			m_pLogger->error("QueryMedName 执行sql语句错误:"+strSql);
			return res;
		}
		listname.clear();
		while(!m_dbMgr.isEndResult())
		{
			listname.push_back(m_dbMgr.getResultByFieldIndex(0));
			m_dbMgr.nextResult();
		}
		m_dbMgr.freeResultSet();
		return 0;
	}
	catch(...)
	{
		m_pLogger->error("CDBWrap::QueryMedName function catch error...");
		return -1;
	}
	return 0;
}

long CDBWrap::QueryLackMedInfoListByCode( string strMedPyCodePart )
{
	m_pLogger->trace("CDBWrap::QueryLackMedInfoListByCode start...");
	
	m_listMedInfoMedMan.clear();
	string strSql;

	if (strMedPyCodePart.empty()==true)
	{
		return 0;
	}
	else
	{
		strSql = "SELECT med.medicineID,loc.medicineValiDate,loc.medicineBatchNumber, med.medicineHisID, med.medicineManDefinedCode,"
				" med.medicineProductName, med.medicinePinYinCode, med.medicineFormat, med.medicineFactoryName, loc.positionID, loc.curCapacity, loc.alertValue,loc.maxCapacity,med.medicineType,loc.medicineUnitName";
		
		strSql += "  FROM  tbl_medicine_info med  ";
		
		strSql +=" right join "+m_locationName+" loc on (med.medicineID=loc.medicineID)";
		
		strSql +=" WHERE (med.medicineType='"+General::integertostring(m_machinetype)+"' OR med.medicineType='0')";
		
		strSql += " AND med.medicineManDefinedCode = '"+strMedPyCodePart+"'";
				
		strSql += " ORDER BY (loc.maxCapacity-loc.curCapacity) desc";
	}
	long lRes = m_dbMgr.RunSQL(strSql);
	if (lRes)
	{
		//出错了
		m_pLogger->error(strSql);
		return lRes;
	}
	bool bflag=false;
	if (0 == m_dbMgr.getRowNum())
	{
		//没有查到该处方的药品列表信息；
		return 1;
	}
	
	MedInfo stMI;
	while (!m_dbMgr.isEndResult())
	{
		stMI.strMedID = m_dbMgr.getResultByFieldName("medicineID");
		stMI.strHisID = m_dbMgr.getResultByFieldName("medicineHisID");
		stMI.strBarcode = m_dbMgr.getResultByFieldName("medicineManDefinedCode");
		stMI.strChName = m_dbMgr.getResultByFieldName("medicineProductName");
		stMI.strPyCode = m_dbMgr.getResultByFieldName("medicinePinYinCode");
		stMI.strUnit = m_dbMgr.getResultByFieldName("medicineFormat");
		stMI.strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");
		stMI.strLocID = m_dbMgr.getResultByFieldName("positionID");
		stMI.strCurQuantity = m_dbMgr.getResultByFieldName("curCapacity");
		stMI.strWarnThreshold = m_dbMgr.getResultByFieldName("alertValue");
		stMI.strMaxCapacity = m_dbMgr.getResultByFieldName("maxCapacity");	
		stMI.strMedType= m_dbMgr.getResultByFieldName("medicineType");
		stMI.strLocMedUnitName= m_dbMgr.getResultByFieldName("medicineUnitName");
		

		stMI.strvalidatetime=m_dbMgr.getResultByFieldName("medicineValiDate");
		stMI.strmedbatch=m_dbMgr.getResultByFieldName("medicineBatchNumber");

		m_listMedInfoMedMan.push_back(stMI);
		
		m_dbMgr.nextResult();
		
	}
	m_dbMgr.freeResultSet();
	m_pLogger->trace("CDBWrap::QueryLackMedInfoListByCode end...");
	
	return 0;
}

long CDBWrap::queryHisIDByBsktID( const string &strBsktID, string &strHisID )
{
	string sql="select preHisID from tbl_prebasket_info where basketID='"+strBsktID+"'";
	long res=m_dbMgr.RunSQL(sql);
	if (res)
	{
		//出错了
		m_pLogger->error(sql);
		return res;
	}
	if (0 == m_dbMgr.getRowNum())
	{
		//没有查到该处方的药品列表信息；
		return 1;
	}
	else
	{
		strHisID=m_dbMgr.getResultByFieldIndex(0);
		m_dbMgr.freeResultSet();
	}
	return 0;
}

long CDBWrap::queryUnitName( int medid,list<string>& listunitname )
{
	string sql="select medicineUnitName,medicineSplitUnit from tbl_medicine_info where medicineID= '"+General::integertostring(medid)+"'";
	long res=m_dbMgr.RunSQL(sql);
	if (res)
	{
		m_pLogger->error(sql);
		return res;
	}
	if (0 == m_dbMgr.getRowNum())
	{
		//没有查到该处方的药品列表信息；
		return 1;
	}
	else
	{
		listunitname.push_back(m_dbMgr.getResultByFieldIndex(0));
		listunitname.push_back(m_dbMgr.getResultByFieldIndex(1));
		m_dbMgr.freeResultSet();
	}
	return 0;
}

long CDBWrap::modifyBarcode( int medid,string barcode )
{
	string sql="update tbl_medicine_info set medicineManDefinedCode='"+barcode+"' where medicineID='"+General::integertostring(medid)+"'";
	long res=m_dbMgr.RunSQL(sql);
	if (res)
	{
		m_pLogger->error(sql);
		//出错了
		return res;
	}
	return 0;
}

long CDBWrap::queryvalidatemedinfo( list<MedInfo>& medlist ,int sorttype)
{
	string sql="select b.medicineProductName,b.medicineFormat,b.medicineFactoryName,a.medicineValiDate,a.medicineBatchNumber,"
			"a.positionID,a.curCapacity,a.medicineUnitName from "+m_locationName+" a, tbl_medicine_info b where  a.medicineID=b.medicineID ";
	if(sorttype==0||sorttype==2)		
		sql += " ORDER BY a.positionID ";
	if(sorttype==1)		
		sql += " ORDER BY a.positionID desc";
	if(sorttype==3)		
		sql += " ORDER BY a.medicineValiDate desc";
	if(sorttype==4)		
		sql += " ORDER BY a.medicineValiDate ";
	if(sorttype==5)		
		sql += " ORDER BY a.curCapacity desc";
	if(sorttype==6)		
		sql += " ORDER BY a.curCapacity ";
	long res=m_dbMgr.RunSQL(sql);
	if (res)
	{
		//出错了
		m_pLogger->error(sql);
		return res;
	}
	if (0 == m_dbMgr.getRowNum())
	{
		//没有查到该处方的药品列表信息；
		return 1;
	}
	MedInfo stMI;
	while (!m_dbMgr.isEndResult())
	{
		stMI.Clear();
		stMI.strChName = m_dbMgr.getResultByFieldName("medicineProductName");
		stMI.strUnit = m_dbMgr.getResultByFieldName("medicineFormat");
		stMI.strFactoryName = m_dbMgr.getResultByFieldName("medicineFactoryName");
		stMI.strLocID = m_dbMgr.getResultByFieldName("positionID");
		stMI.strvalidatetime = m_dbMgr.getResultByFieldName("medicineValiDate");
		stMI.strmedbatch = m_dbMgr.getResultByFieldName("medicineBatchNumber");

		stMI.strCurQuantity = m_dbMgr.getResultByFieldName("curCapacity");
		stMI.strLocMedUnitName = m_dbMgr.getResultByFieldName("medicineUnitName");
		medlist.push_back(stMI);
//		m_pLogger->debug("获取监控药品数据条数:"+Utils::itos(medlist.size())+"");
		m_dbMgr.nextResult();
	}
	m_dbMgr.freeResultSet();

	return 0;
}

long CDBWrap::modifyValidBatchByLocID(int nLocID,string strvalid,string strbatch)
{
	string sql="update "+m_locationName+" set medicineValiDate='"+strvalid+"',medicineBatchNumber='"+strbatch+"' where positionID='"+General::integertostring(nLocID)+"' ";
	long res=m_dbMgr.RunSQL(sql);
	if (res)
	{
		//出错了
		m_pLogger->error("药品有效期批号修改失败:"+sql);
		return res;
	}
	m_pLogger->trace("药品有效期批号修改成功"+sql);
	return 0;
}

long CDBWrap::modifyValidBatchByMedID( int medi,string strvalid,string strbatch )
{
	string sql="update tbl_medicine_info set medicineValiDate='"+strvalid+"',medicineBatchNumber='"+strbatch+"' where medicineID='"+General::integertostring(medi)+"' ";
	long res=m_dbMgr.RunSQL(sql);
	if (res)
	{
		//出错了
		m_pLogger->error("药品有效期批号修改失败:"+sql);
		return res;
	}
	m_pLogger->trace("药品有效期批号修改成功"+sql);
	return 0;
}
