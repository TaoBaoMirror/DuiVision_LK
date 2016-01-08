#include "StdAfx.h"
#include "DuiHandlerCDuiPrescriptionHandler.h"
#include "DuiVision_LK.h"
//////////////////////////////////////////////////////////////
// CDuiHandlerCDuiPrescriptionHandler

CDuiHandlerCDuiPrescriptionHandler::CDuiHandlerCDuiPrescriptionHandler(void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerCDuiPrescriptionHandler::~CDuiHandlerCDuiPrescriptionHandler(void)
{
	DuiSystem::RemoveDuiTimer(_T("test"));
}

// 初始化
void CDuiHandlerCDuiPrescriptionHandler::OnInit()
{
	m_uTimerAni=DuiSystem::AddDuiTimer(5000,_T("test"));
}

// DUI定时器事件处理
void CDuiHandlerCDuiPrescriptionHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
		//DuiSystem::AddDuiNotifyMsgTask(_T("这是由定时器触发的提示框！\n触发周期：5秒\n此窗口2秒钟后自动关闭..."), MB_ICONINFORMATION | 2, _T(""), 2000);
	}
}


LRESULT CDuiHandlerCDuiPrescriptionHandler::OnDuiMsgBtnSearch(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiGridCtrl *pChestA=(CDuiGridCtrl*)GetControl(_T("gridctrl_pre_1"));
	CDuiGridCtrl *pChestB=(CDuiGridCtrl*)GetControl(_T("gridctrl_pre_2"));
	if(pChestA==NULL || pChestB==NULL)
		return FALSE;
	//清空列表
	while(pChestA->GetRowCount()!=0)
		pChestA->DeleteRow(0);
	while(pChestB->GetRowCount()!=0)
		pChestB->DeleteRow(0);

	CString strID=_T("");
	CDuiEdit *p = (CDuiEdit*)GetControl(_T("edit_hisid"));
	if (p==NULL)
		return FALSE;
	strID=p->GetEditText();
	if (!strID.Compare(""))
	{
		DuiSystem::AddDuiNotifyMsgTask(configPar.getValueParameter("No_empty_pre").c_str(), MB_ICONINFORMATION | 2, _T(""), 5000);
		return FALSE;
	}

	long lRes = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->queryPreInfoMedListByHisID(strID.GetBuffer(0));	
	strID.ReleaseBuffer();
	if (lRes!=1&&lRes!=0)
	{
		DuiSystem::AddDuiNotifyMsgTask(configPar.getValueParameter("Pre_query_error").c_str(), MB_ICONINFORMATION | 2, _T(""), 5000);
		return FALSE;
	}
	if (lRes==1)
	{
		DuiSystem::AddDuiNotifyMsgTask(configPar.getValueParameter("NO_PRE_MED").c_str(), MB_ICONINFORMATION | 2, _T(""), 5000);
		return FALSE;
	}
	PreInfo		&stPreInfo = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->getPreInfo();
	list<FetchMedInfo> &listFMI = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->getPreMedInfoList();

	int nRowCntChestA = 0;
	int nRowCntChestB = 0;
	bool bNoPutFlagA = false;
	bool bNoPutFlagB = false;
	int nStartRowA = 0;
	int nStartRowB = 0;

	//存入数据
	for(list<FetchMedInfo>::iterator iter = listFMI.begin(); iter != listFMI.end(); ++iter)
	{
		if (atoi(iter->strLocID.c_str()) <= 120)
		{
			pChestA->InsertRow(nRowCntChestA,_T(""),-1,Color(0,0,0,0),_T(""),-1,_T(""),-1);
			pChestA->SetSubItem(nRowCntChestA,0,iter->strMedID.c_str());
			pChestA->SetSubItem(nRowCntChestA,1,iter->strName.c_str());
			pChestA->SetSubItem(nRowCntChestA,2,iter->strUnit.c_str());
			pChestA->SetSubItem(nRowCntChestA,3,iter->strCurQuantity.c_str());

			CString strtemp="";
			if(_ttoi(iter->strFetchNum.c_str())<=0)
				strtemp.Format("%g",atof(iter->strHeterNum.c_str()));
			else
				strtemp+=(iter->strFetchNum.c_str());
			strtemp+=(iter->strLocMedUnitName.c_str());

			pChestA->SetSubItem(nRowCntChestA,4,strtemp);
			pChestA->SetSubItem(nRowCntChestA,5, configPar.getValueParameter("No_send_med").c_str());
			pChestA->SetSubItem(nRowCntChestA,6,(((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->Convert2Str(atoi(iter->strLocID.c_str()))).c_str());
			pChestA->SetSubItem(nRowCntChestA,7,iter->strFactoryName.c_str());

	
			//一开始的行，就未上架
			if (0 == nRowCntChestA && 0 == atoi(iter->strLocID.c_str()))
			{
				bNoPutFlagA = true;
			}

			//有一开始，就未上架
			if (bNoPutFlagA)
			{
				//终于有上架
				if (0 != atoi(iter->strLocID.c_str()))
				{
					nStartRowA = nRowCntChestA;
					bNoPutFlagA = false;
				} 
			}
			++nRowCntChestA;
		} 
		else
		{
			pChestB->InsertRow(nRowCntChestB,_T(""),-1,Color(0,0,0,0),_T(""),-1,_T(""),-1);
			pChestB->SetSubItem(nRowCntChestB,0,iter->strMedID.c_str());
			pChestB->SetSubItem(nRowCntChestB,1,iter->strName.c_str());
			pChestB->SetSubItem(nRowCntChestB,2,iter->strUnit.c_str());
			pChestB->SetSubItem(nRowCntChestB,3,iter->strCurQuantity.c_str());

			CString strtemp="";
			if(_ttoi(iter->strFetchNum.c_str())<=0)
				strtemp.Format("%g",atof(iter->strHeterNum.c_str()));
			else
				strtemp+=(iter->strFetchNum.c_str());
			strtemp+=(iter->strLocMedUnitName.c_str());

			pChestB->SetSubItem(nRowCntChestB,4,strtemp);
			pChestB->SetSubItem(nRowCntChestB,5, configPar.getValueParameter("No_send_med").c_str());
			pChestB->SetSubItem(nRowCntChestB,6,(((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->Convert2Str(atoi(iter->strLocID.c_str()))).c_str());
			pChestB->SetSubItem(nRowCntChestB,7,iter->strFactoryName.c_str());

			//一开始的行，就未上架
			if (0 == nRowCntChestB && 0 == atoi(iter->strLocID.c_str()))
			{
				bNoPutFlagB = true;
			}
			//有一开始，就未上架
			if (bNoPutFlagB)
			{
				//终于有上架
				if (0 != atoi(iter->strLocID.c_str()))
				{
					nStartRowB = nRowCntChestB;
					bNoPutFlagB = false;
				} 
			}
			++nRowCntChestB;
		}
	}
	return TRUE;
}
