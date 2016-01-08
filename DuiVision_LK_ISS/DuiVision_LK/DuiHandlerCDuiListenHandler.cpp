#include "StdAfx.h"
#include "DuiHandlerCDuiListenHandler.h"
#include "DBWrap.h"
//////////////////////////////////////////////////////////////
// CDuiHandlerCDuiListenHandler

CDuiHandlerCDuiListenHandler::CDuiHandlerCDuiListenHandler(void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
	m_pNormalThread=NULL;
	m_lThreadExit=0;

	m_bSearchMed=false;
	m_SortType=4;
}

CDuiHandlerCDuiListenHandler::~CDuiHandlerCDuiListenHandler(void)
{
	m_lThreadExit=-1;
	Sleep(100);
	DWORD   dwExitCode;
	if(GetExitCodeThread( m_pNormalThread->m_hThread, &dwExitCode ) )
	{
		::TerminateThread( m_pNormalThread->m_hThread,dwExitCode);
		CloseHandle( m_pNormalThread->m_hThread);
	}
}

// 初始化
void CDuiHandlerCDuiListenHandler::OnInit()
{
	if (m_pNormalThread==NULL)
	{
		m_pNormalThread=AfxBeginThread(ThreadListenPro,this);
		if (m_pNormalThread==NULL)
		{
			return ;
		}
	}
}

// DUI定时器事件处理
void CDuiHandlerCDuiListenHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}

unsigned int CDuiHandlerCDuiListenHandler::ThreadListenPro(LPVOID lparam)
{
	CDuiHandlerCDuiListenHandler *pThis=(CDuiHandlerCDuiListenHandler*)lparam;
	long res;
	int count=0;
	list<MedInfo>m_listMedinfo;
	list<MedInfo>::iterator iter=m_listMedinfo.begin();
	int index=0;
	CTime Curtm=CTime::GetCurrentTime();
	CString timestr="";
	int   a,b,c ;
	CTimeSpan ts;
	CString strCurNun="";
	CDBWrap *pDWrap=new CDBWrap();
	CDuiGridCtrl* pGirdCtrl=(CDuiGridCtrl*)(pThis->GetControl(_T("gridctrl_Listen")));
	while(pThis->m_lThreadExit!=-1)
	{
		if(pGirdCtrl==NULL)
		{
			Sleep(1000);
			continue;
		}
		if (count>300000||count==0)
		{
			m_listMedinfo.clear();
			res = pDWrap->queryvalidatemedinfo(m_listMedinfo,pThis->m_SortType);		
			if (res==0)
			{
				while(pGirdCtrl->GetRowCount()!=0)
					pGirdCtrl->DeleteRow(0);
				index=0;
				iter=m_listMedinfo.begin();	
				for (;iter!=m_listMedinfo.end()&&(pThis->m_lThreadExit!=-1);iter++)
				{
					pGirdCtrl->InsertRow(index,_T(""),-1,Color(0,0,0,0),_T(""),-1,_T(""),-1);
				
					pGirdCtrl->SetSubItem(index,0,iter->strChName.c_str());
					pGirdCtrl->SetSubItem(index,1,iter->strUnit.c_str());
					pGirdCtrl->SetSubItem(index,2,(iter->strFactoryName.c_str()));
					strCurNun=iter->strCurQuantity.c_str();
					strCurNun+=iter->strLocMedUnitName.c_str();

					pGirdCtrl->SetSubItem(index,3,strCurNun);
					pGirdCtrl->SetSubItem(index,4,iter->strvalidatetime.c_str());

					timestr="";
					timestr=iter->strvalidatetime.c_str();
					sscanf(timestr.GetBuffer(timestr.GetLength()),"%d-%d-%d",&a,&b,&c);   
					CTime   time(a,b,c,0,0,0);  		
					ts=time-Curtm;
					if (ts.GetDays()<configPar.getIntParameter("ValidMonth")*30)
						pGirdCtrl->SetRowColor(index,Color(255,255,0,0));			

					pGirdCtrl->SetSubItem(index,5,iter->strmedbatch.c_str());
					pGirdCtrl->SetSubItem(index,6,pDWrap->Convert2Str(atoi(iter->strLocID.c_str())).c_str());
					index++;
				}
			}
			pThis->m_bSearchMed=false;
			count=1;
		}
		if (pThis->m_bSearchMed==true)
		{
			m_listMedinfo.clear();
			res = pDWrap->queryvalidatemedinfo(m_listMedinfo,pThis->m_SortType);		
			if (res==0)
			{
				while(pGirdCtrl->GetRowCount()!=0)
					pGirdCtrl->DeleteRow(0);
				index=0;
				iter=m_listMedinfo.begin();	
				for (;iter!=m_listMedinfo.end()&&(pThis->m_lThreadExit!=-1);iter++)
				{
					pGirdCtrl->InsertRow(index,_T(""),-1,Color(0,0,0,0),_T(""),-1,_T(""),-1);

					pGirdCtrl->SetSubItem(index,0,iter->strChName.c_str());
					pGirdCtrl->SetSubItem(index,1,iter->strUnit.c_str());
					pGirdCtrl->SetSubItem(index,2,(iter->strFactoryName.c_str()));
					strCurNun=iter->strCurQuantity.c_str();
					strCurNun+=iter->strLocMedUnitName.c_str();

					pGirdCtrl->SetSubItem(index,3,strCurNun);
					pGirdCtrl->SetSubItem(index,4,iter->strvalidatetime.c_str());

					timestr="";
					timestr=iter->strvalidatetime.c_str();
					sscanf(timestr.GetBuffer(timestr.GetLength()),"%d-%d-%d",&a,&b,&c);   
					CTime   time(a,b,c,0,0,0);  		
					ts=time-Curtm;
					if (ts.GetDays()<configPar.getIntParameter("ValidMonth")*30)
						pGirdCtrl->SetRowColor(index,Color(255,255,0,0));				

					pGirdCtrl->SetSubItem(index,5,iter->strmedbatch.c_str());
					pGirdCtrl->SetSubItem(index,6,pDWrap->Convert2Str(atoi(iter->strLocID.c_str())).c_str());
					index++;
				}
			}
			pThis->m_bSearchMed=false;
			count=1;
		}
		else
		{
			Sleep(1);
			count++;
		}
	}
	delete pDWrap;
	pDWrap=NULL;
	return 0;
}

LRESULT CDuiHandlerCDuiListenHandler::OnDuiMsgRadio1ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=1;
	m_bSearchMed=true;
	return TRUE;
}

LRESULT CDuiHandlerCDuiListenHandler::OnDuiMsgRadio2ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=2;
	m_bSearchMed=true;
	return TRUE;
}

LRESULT CDuiHandlerCDuiListenHandler::OnDuiMsgRadio3ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=3;
	m_bSearchMed=true;
	return TRUE;
}

LRESULT CDuiHandlerCDuiListenHandler::OnDuiMsgRadio4ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=4;
	m_bSearchMed=true;
	return TRUE;
}

LRESULT CDuiHandlerCDuiListenHandler::OnDuiMsgRadio5ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=5;
	m_bSearchMed=true;
	return TRUE;
}

LRESULT CDuiHandlerCDuiListenHandler::OnDuiMsgRadio6ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_SortType=6;
	m_bSearchMed=true;
	return TRUE;
}
