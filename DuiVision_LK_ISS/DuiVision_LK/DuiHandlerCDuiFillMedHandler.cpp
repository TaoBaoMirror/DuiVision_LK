#include "StdAfx.h"
#include "DuiHandlerCDuiFillMedHandler.h"
#include "DuiVision_LK.h"
//////////////////////////////////////////////////////////////
// CDuiHandlerCDuiFillMedHandler

CDuiHandlerCDuiFillMedHandler::CDuiHandlerCDuiFillMedHandler(void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
	m_SortType=4;
}

CDuiHandlerCDuiFillMedHandler::~CDuiHandlerCDuiFillMedHandler(void)
{
}

// 初始化
void CDuiHandlerCDuiFillMedHandler::OnInit()
{
	//快速查找药品信息
	list<string>listname;
	long lRes = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->QueryLackMedName(listname);
	CDuiComboBox * p=(CDuiComboBox *)GetControl(_T("fill_combobox"));
	if (p!=NULL)
	{
		if(lRes==0)
		{
			list<string>::iterator iter=listname.begin();
			
			int index=0;
			for (;iter!=listname.end();iter++,index++)
				p->AddItem((*iter).c_str(),_T(""),General::integertostring(index).c_str(),-1);
		//	p->SetComboValue(_T("0"));
		}
		
	}
	showLackMedInfoList();
}

// DUI定时器事件处理
void CDuiHandlerCDuiFillMedHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
		CDuiGridCtrl * pGirdCtrl=(CDuiGridCtrl *)GetControl(_T("gridctrl_fillmed"));
		if(pGirdCtrl==NULL)
			return ;
	}
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgBtnSearch(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CString strmedname="";
	CDuiComboBox * p=(CDuiComboBox *)GetControl(_T("fill_combobox"));
	strmedname=p->GetTitle();

	CString strMedBarcode="";	
	CDuiEdit * pEdit=(CDuiEdit*)GetControl(_T("edit_barcode"));
	strMedBarcode=pEdit->GetTitle();

	CDuiGridCtrl * pGirdCtrl=(CDuiGridCtrl *)GetControl(_T("gridctrl_fillmed"));
	if(pGirdCtrl==NULL)
		return FALSE;
	

	CTime Curtm=CTime::GetCurrentTime();
	CString timestr="";
	int   a,b,c ;
	CTimeSpan ts;

	if (strmedname.GetLength()>0)
	{
		strmedname.Replace(" ","");
		long lRes = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->QueryLackMedInfoList(strmedname.GetBuffer(0),m_SortType);
		strmedname.ReleaseBuffer();
		if (lRes)
		{
			DuiSystem::AddDuiNotifyMsgTask(configPar.getValueParameter("Check_Med_Error_2").c_str(), MB_ICONINFORMATION | 2, _T(""), 5000);
			return FALSE;
		}
		if (lRes==1)
		{
			DuiSystem::AddDuiNotifyMsgTask(configPar.getValueParameter("No_Med").c_str(), MB_ICONINFORMATION | 2, _T(""), 5000);
			return FALSE;
		}
		list<MedInfo> &m_list = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->getMedInfoListMedMan();

		int nNeedNum=0;
		int i=0;
		while(pGirdCtrl->GetRowCount()>0)
			pGirdCtrl->DeleteRow(0);
		for (list<MedInfo>::iterator it = m_list.begin(); it != m_list.end() ; ++it)
		{
			pGirdCtrl->InsertRow(i,_T(""),-1,Color(0,0,0,0),_T(""),-1,_T(""),-1);

			pGirdCtrl->SetSubItem(i,0,it->strMedID.c_str());
			pGirdCtrl->SetSubItem(i,1,it->strChName.c_str());
			pGirdCtrl->SetSubItem(i,2,it->strUnit.c_str());

			pGirdCtrl->SetSubItem(i,3,it->strCurQuantity.c_str());
			nNeedNum = atoi(it->strMaxCapacity.c_str()) - atoi(it->strCurQuantity.c_str());
			pGirdCtrl->SetSubItem(i,4,General::integertostring(nNeedNum).c_str());
			pGirdCtrl->SetSubItem(i,5,it->strWarnThreshold.c_str());

			pGirdCtrl->SetSubItem(i,6,(((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->Convert2Str(atoi(it->strLocID.c_str()))).c_str());
			pGirdCtrl->SetSubItem(i,7,it->strLocMedUnitName.c_str());
			pGirdCtrl->SetSubItem(i,8,it->strFactoryName.c_str());
			pGirdCtrl->SetSubItem(i,9,it->strvalidatetime.c_str());
			pGirdCtrl->SetSubItem(i,10, it->strmedbatch.c_str());
			timestr="";
			timestr=it->strvalidatetime.c_str();
			sscanf(timestr.GetBuffer(timestr.GetLength()),"%d-%d-%d",&a,&b,&c);   
			CTime   time(a,b,c,0,0,0);  		
			ts=time-Curtm;
			// 		if (ts.GetDays()<configPar.getIntParameter("ValidMonth")*30)
			// 			m_listLackMedInfo.SetItemColor(i,RGB(255,0,0));		
			++i;
		}

	}
	if (strMedBarcode.GetLength()>0)
	{
		strMedBarcode.Replace(" ","");
		long lRes = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->QueryLackMedInfoListByCode(strMedBarcode.GetBuffer(0));
		strMedBarcode.ReleaseBuffer();
		if (lRes)
		{
			DuiSystem::AddDuiNotifyMsgTask(configPar.getValueParameter("Check_Med_Error_2").c_str(), MB_ICONINFORMATION | 2, _T(""), 5000);
			return FALSE;
		}
		if (lRes==1)
		{
			DuiSystem::AddDuiNotifyMsgTask(configPar.getValueParameter("No_Med").c_str(), MB_ICONINFORMATION | 2, _T(""), 5000);
			return FALSE;
		}
		list<MedInfo> &m_list = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->getMedInfoListMedMan();

		//m_listLackMedInfo.DeleteAllItems();
		int nNeedNum=0;
		int i=0;

		while(pGirdCtrl->GetRowCount()>0)
			pGirdCtrl->DeleteRow(0);

		for (list<MedInfo>::iterator it = m_list.begin(); it != m_list.end() ; ++it)
		{
			pGirdCtrl->InsertRow(i,_T(""),-1,Color(0,0,0,0),_T(""),-1,_T(""),-1);

			pGirdCtrl->SetSubItem(i,0,it->strMedID.c_str());
			pGirdCtrl->SetSubItem(i,1,it->strChName.c_str());
			pGirdCtrl->SetSubItem(i,2,it->strUnit.c_str());

			pGirdCtrl->SetSubItem(i,3,it->strCurQuantity.c_str());
			nNeedNum = atoi(it->strMaxCapacity.c_str()) - atoi(it->strCurQuantity.c_str());
			pGirdCtrl->SetSubItem(i,4,General::integertostring(nNeedNum).c_str());
			pGirdCtrl->SetSubItem(i,5,it->strWarnThreshold.c_str());

			pGirdCtrl->SetSubItem(i,6,(((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->Convert2Str(atoi(it->strLocID.c_str()))).c_str());
			pGirdCtrl->SetSubItem(i,7,it->strLocMedUnitName.c_str());
			pGirdCtrl->SetSubItem(i,8,it->strFactoryName.c_str());
			pGirdCtrl->SetSubItem(i,9,it->strvalidatetime.c_str());
			pGirdCtrl->SetSubItem(i,10, it->strmedbatch.c_str());
			timestr="";
			timestr=it->strvalidatetime.c_str();
			sscanf(timestr.GetBuffer(timestr.GetLength()),"%d-%d-%d",&a,&b,&c);   
			CTime   time(a,b,c,0,0,0);  		
			ts=time-Curtm;
			// 		if (ts.GetDays()<configPar.getIntParameter("ValidMonth")*30)
			// 			m_listLackMedInfo.SetItemColor(i,RGB(255,0,0));		
			++i;
		}
		
	}
	else if(strMedBarcode.GetLength()<=0&&strmedname.GetLength()<=0)
	{
		showLackMedInfoList();
	}
	return TRUE;
}

void CDuiHandlerCDuiFillMedHandler::showLackMedInfoList()
{
	int i = 0;
	int nNeedNum = 0;
	((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->queryBatchAddMedInfoList(m_SortType);

	list<MedInfo> &m_list = ((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->getMedInfoListBatchAdd();

	CDuiGridCtrl * pGirdCtrl=(CDuiGridCtrl *)GetControl(_T("gridctrl_fillmed"));
	if(pGirdCtrl==NULL)
		return ;
	while(pGirdCtrl->GetRowCount()>0)
		pGirdCtrl->DeleteRow(0);

	CTime Curtm=CTime::GetCurrentTime();
	CString timestr="";
	int   a,b,c ;
	CTimeSpan ts;
	for (list<MedInfo>::iterator it = m_list.begin(); it != m_list.end() ; ++it)
	{
		pGirdCtrl->InsertRow(i,_T(""),-1,Color(0,0,0,0),_T(""),-1,_T(""),-1);

		pGirdCtrl->SetSubItem(i,0,it->strMedID.c_str());
		pGirdCtrl->SetSubItem(i,1,it->strChName.c_str());
		pGirdCtrl->SetSubItem(i,2,it->strUnit.c_str());

		pGirdCtrl->SetSubItem(i,3,it->strCurQuantity.c_str());
		nNeedNum = atoi(it->strMaxCapacity.c_str()) - atoi(it->strCurQuantity.c_str());
		pGirdCtrl->SetSubItem(i,4,General::integertostring(nNeedNum).c_str());
		pGirdCtrl->SetSubItem(i,5,it->strWarnThreshold.c_str());

		pGirdCtrl->SetSubItem(i,6,(((CDuiVision_LKApp*)AfxGetApp())->m_pDBWrap->Convert2Str(atoi(it->strLocID.c_str()))).c_str());
		pGirdCtrl->SetSubItem(i,7,it->strLocMedUnitName.c_str());
		pGirdCtrl->SetSubItem(i,8,it->strFactoryName.c_str());
		pGirdCtrl->SetSubItem(i,9,it->strvalidatetime.c_str());
		pGirdCtrl->SetSubItem(i,10, it->strmedbatch.c_str());
		timestr="";
		timestr=it->strvalidatetime.c_str();
		sscanf(timestr.GetBuffer(timestr.GetLength()),"%d-%d-%d",&a,&b,&c);   
		CTime   time(a,b,c,0,0,0);  		
		ts=time-Curtm;
		if (ts.GetDays()<configPar.getIntParameter("ValidMonth")*30)
			pGirdCtrl->SetRowColor(i,Color(255,255,0,0));		
		++i;
	}
}
LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgRadio1ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=1;
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgRadio2ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=2;
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgRadio3ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=3;
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgRadio4ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=4;
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgRadio5ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=5;
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgRadio6ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=6;
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgGridLBClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (strName==_T("gridctrl_fillmed"))
	{
		CDuiGridCtrl * pGirdCtrl=(CDuiGridCtrl *)GetControl(_T("gridctrl_fillmed"));
		if(pGirdCtrl==NULL)
			return FALSE;
		int row=(int )wParam;
		CString str=pGirdCtrl->GetItemTitle(row,1);
		CDuiText * pt=(CDuiText *)GetControl(_T("text_static_medname"));
		if(pt==NULL)
			return FALSE;
		pt->SetTitle(str);

		CDlgBase * p=DuiSystem::Instance()->GetDuiDialog(_T("dlg_fillsure"));
		if (p==NULL)
		{
			p=DuiSystem::Instance()->CreateDuiDialog(_T("dlg_fillsure"),NULL,_T(""),FALSE,0,TRUE);
			p->ShowWindow(SW_HIDE);
		}

		MedInfo info;
		info.strMedID=(pGirdCtrl->GetItemTitle(row, 0)).GetBuffer(0);
		info.strChName=(pGirdCtrl->GetItemTitle(row, 1)).GetBuffer(0);
		info.strUnit=(pGirdCtrl->GetItemTitle(row, 2)).GetBuffer(0);
		info.strCurQuantity=(pGirdCtrl->GetItemTitle(row, 3)).GetBuffer(0);
		info.strNeedNum=(pGirdCtrl->GetItemTitle(row, 4)).GetBuffer(0);
		info.strLocID=(pGirdCtrl->GetItemTitle(row, 6)).GetBuffer(0);
		info.strLocMedUnitName=(pGirdCtrl->GetItemTitle(row, 7)).GetBuffer(0);
		info.strFactoryName=(pGirdCtrl->GetItemTitle(row, 8)).GetBuffer(0);
		info.strvalidatetime=(pGirdCtrl->GetItemTitle(row, 9)).GetBuffer(0);
		info.strmedbatch=(pGirdCtrl->GetItemTitle(row, 10)).GetBuffer(0);

		CDuiText *pControl1=(CDuiText *)(p->GetControl(_T("static_medname")));
		if(pControl1!=NULL)
			pControl1->SetTitle(info.strChName.c_str());

		CDuiText *pControl2=(CDuiText *)(p->GetControl(_T("static_medunit")));
		if(pControl2!=NULL)
			pControl2->SetTitle(info.strUnit.c_str());

		CDuiText *pControl3=(CDuiText *)(p->GetControl(_T("static_medfactory")));
		if(pControl3!=NULL)
			pControl3->SetTitle(info.strFactoryName.c_str());

		CDuiText *pControl4=(CDuiText *)(p->GetControl(_T("static_medposition")));
		if(pControl4!=NULL)
			pControl4->SetTitle(info.strLocID.c_str());

		CDuiEdit *pControl5=(CDuiEdit *)(p->GetControl(_T("edit_medbatch")));
		if(pControl5!=NULL)
			pControl5->SetTitle(info.strmedbatch.c_str());
		if (p->IsWindowVisible()==FALSE)
		{
			p->ShowWindow(SW_HIDE);
		}
	}
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgBtnUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiGridCtrl * pGirdCtrl=(CDuiGridCtrl *)GetControl(_T("gridctrl_fillmed"));
	if(pGirdCtrl==NULL)
		return FALSE;
	int row=pGirdCtrl->GetSelectRow();
	pGirdCtrl->SetSelectRow(row-1);
	pGirdCtrl->UpdateControl(TRUE);
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgBtnDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiGridCtrl * pGirdCtrl=(CDuiGridCtrl *)GetControl(_T("gridctrl_fillmed"));
	if(pGirdCtrl==NULL)
		return FALSE;
	int row=pGirdCtrl->GetSelectRow();
	pGirdCtrl->SetSelectRow(row+1);
	pGirdCtrl->UpdateControl(TRUE);
	return TRUE;
}

LRESULT CDuiHandlerCDuiFillMedHandler::OnDuiMsgBtnFillMed(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase * p=DuiSystem::Instance()->GetDuiDialog(_T("dlg_fillsure"));
	if (p==NULL)
	{
		p=DuiSystem::Instance()->CreateDuiDialog(_T("dlg_fillsure"),NULL,_T(""),FALSE,0,TRUE);
		p->ShowWindow(SW_HIDE);
	}
	CDuiGridCtrl * pGirdCtrl=(CDuiGridCtrl *)GetControl(_T("gridctrl_fillmed"));
	if(pGirdCtrl==NULL)
		return FALSE;
	int row=pGirdCtrl->GetSelectRow();

	MedInfo info;
	info.strMedID=(pGirdCtrl->GetItemTitle(row, 0)).GetBuffer(0);
	info.strChName=(pGirdCtrl->GetItemTitle(row, 1)).GetBuffer(0);
	info.strUnit=(pGirdCtrl->GetItemTitle(row, 2)).GetBuffer(0);
	info.strCurQuantity=(pGirdCtrl->GetItemTitle(row, 3)).GetBuffer(0);
	info.strNeedNum=(pGirdCtrl->GetItemTitle(row, 4)).GetBuffer(0);
	info.strLocID=(pGirdCtrl->GetItemTitle(row, 6)).GetBuffer(0);
	info.strLocMedUnitName=(pGirdCtrl->GetItemTitle(row, 7)).GetBuffer(0);
	info.strFactoryName=(pGirdCtrl->GetItemTitle(row, 8)).GetBuffer(0);
	info.strvalidatetime=(pGirdCtrl->GetItemTitle(row, 9)).GetBuffer(0);
	info.strmedbatch=(pGirdCtrl->GetItemTitle(row, 10)).GetBuffer(0);

	CDuiText *pControl1=(CDuiText *)(p->GetControl(_T("static_medname")));
	if(pControl1!=NULL)
			pControl1->SetTitle(info.strChName.c_str());

	CDuiText *pControl2=(CDuiText *)(p->GetControl(_T("static_medunit")));
	if(pControl2!=NULL)
		pControl2->SetTitle(info.strUnit.c_str());

	CDuiText *pControl3=(CDuiText *)(p->GetControl(_T("static_medfactory")));
	if(pControl3!=NULL)
		pControl3->SetTitle(info.strFactoryName.c_str());

	CDuiText *pControl4=(CDuiText *)(p->GetControl(_T("static_medposition")));
	if(pControl4!=NULL)
		pControl4->SetTitle(info.strLocID.c_str());

	CDuiEdit *pControl5=(CDuiEdit *)(p->GetControl(_T("edit_medbatch")));
	if(pControl5!=NULL)
		pControl5->SetTitle(info.strmedbatch.c_str());

	CDuiComboBox *pControl6=(CDuiComboBox *)(p->GetControl(_T("combox_medvalidtime")));
	if(pControl6!=NULL)
	{
		pControl6->AddItem(info.strvalidatetime.c_str(),_T(""),_T("0"),-1);
		pControl6->AddItem(info.strvalidatetime.c_str(),_T(""),_T("1"),-1);
		pControl6->AddItem(info.strvalidatetime.c_str(),_T(""),_T("2"),-1);
		pControl6->AddItem(info.strvalidatetime.c_str(),_T(""),_T("3"),-1);
		pControl6->SetComboValue(info.strvalidatetime.c_str());
	}

	int nResponse = p->ShowWindow(SW_SHOW);
	return TRUE;
}
