#include "StdAfx.h"
#include "DuiKeyBord.h"
#define CAPTION_HEIGHT 31
CString g_strKey[]={"Q","W","E","R","T","Y","U","I","O","P","7","8","9",
				  "A","S","D","F","G","H","J","K","L","<--","4","5","6",
				  "Z","X","C","V","B","N","M","UP","Enter","1","2","3",
				  "Space","Left","Down","Right","0","-","."};
BYTE g_keyvalue[]={'Q','W','E','R','T','Y','U','I','O','P','7','8','9',
				 'A','S','D','F','G','H','J','K','L',VK_BACK ,'4','5','6',
				 'Z','X','C','V','B','N','M',VK_UP,VK_RETURN,'1','2','3',
				 VK_SPACE,VK_LEFT,VK_DOWN,VK_RIGHT,'0',0x6D,0x6E
				};
IMPLEMENT_DYNAMIC(CDuiKeyBord, CDlgBase)
CDuiKeyBord::CDuiKeyBord(UINT nIDTemplate, CWnd* pParent /*= NULL*/)
	:CDlgBase(nIDTemplate,pParent)
{
	m_pInputControl=NULL;
}


CDuiKeyBord::~CDuiKeyBord(void)
{
	
}
BEGIN_MESSAGE_MAP(CDuiKeyBord, CDlgBase)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



void CDuiKeyBord::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType,cx,cy);
	if (m_vecControl.size()==0)
		return;
	int btnwidth=(cx-2*12-10)/13;
	int btnheigth=(cy-4*2-50-CAPTION_HEIGHT)/4;
	//第一排
	int index=0;
	for(int i=0;i<13;i++)
	{
		CDuiButton *p=(CDuiButton *)m_vecControl.at(i);
		p->SetTitle(g_strKey[i]);
		CRect rc;
		if(i>=10)
			rc.left=(btnwidth+2)*i+10;
		else
			rc.left=(btnwidth+2)*i;
		rc.top=CAPTION_HEIGHT;
		rc.right=rc.left+btnwidth;
		rc.bottom=rc.top+btnheigth;
		p->SetRect(rc);
	}
	//第二排
	index=0;
	for(int i=13;i<26;i++,index++)
	{
		CDuiButton *p=(CDuiButton *)m_vecControl.at(i);
		p->SetTitle(g_strKey[i]);
		CRect rc;
		if(i>=23)
			rc.left=(btnwidth+2)*index+10;
		else
			rc.left=(btnwidth+2)*index;
		rc.top=CAPTION_HEIGHT+(btnheigth+2);
		rc.right=rc.left+btnwidth;
		rc.bottom=rc.top+btnheigth;
		p->SetRect(rc);
	}
	//第三排
	index=0;
	for(int i=26;i<38;i++,index++)
	{
		CDuiButton *p=(CDuiButton *)m_vecControl.at(i);
		p->SetTitle(g_strKey[i]);
		CRect rc;
		if(i>=35)
			rc.left=(btnwidth+2)*(index+1)+10;
		else
			rc.left=(btnwidth+2)*(index+1);
		rc.top=CAPTION_HEIGHT+2*(btnheigth+2);
		rc.right=rc.left+btnwidth;
		rc.bottom=rc.top+btnheigth;
		p->SetRect(rc);
	}
	//第四排
	index=0;
	for(int i=38;i<45;i++,index++)
	{
		CDuiButton *p=(CDuiButton *)m_vecControl.at(i);
		p->SetTitle(g_strKey[i]);
		CRect rc;
		if(i==38)
		{
			rc.left=(btnwidth+2)*2;
			rc.top=CAPTION_HEIGHT+3*(btnheigth+2);
			rc.right=rc.left+btnwidth*5+4*2;
			rc.bottom=rc.top+btnheigth;
		}
		else
		{
			if(i>=42)
				rc.left=(btnwidth+2)*(index+6)+10;
			else
				rc.left=(btnwidth+2)*(index+6);
			rc.top=CAPTION_HEIGHT+3*(btnheigth+2);
			rc.right=rc.left+btnwidth;
			rc.bottom=rc.top+btnheigth;
		}	
		p->SetRect(rc);
	}
	CDuiButton *p=(CDuiButton *)m_vecControl.at(45);
	p->SetRect(CRect(cx/2-60,cy-48,cx/2+60,cy-2));
	p->SetTitle(_T("关闭"));
}

void CDuiKeyBord::OnPaint()
{
	__super::OnPaint();
}

BOOL CDuiKeyBord::OnInitDialog()
{
	__super::OnInitDialog();

	//创建45个键盘按钮：
	for (int i=0; i<46;i++)
	{
		CDuiButton *pQ=(CDuiButton *)DuiSystem::CreateControlByName(_T("button"), GetSafeHwnd(), this);
		pQ->OnAttributeImage(_T("skin:IDB_BT_DEFAULT"),TRUE);
		pQ->SetFont(_T("微软雅黑"),16);
		m_vecControl.push_back(pQ);
	}
	return TRUE;
}
LRESULT CDuiKeyBord::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	__super::OnMessage(uID,uMsg,wParam,lParam);
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && (pControlBase->GetControlID() == uID)&&uMsg==MSG_BUTTON_DOWN)
		{
			if (i==m_vecControl.size()-1)
				ShowWindow(SW_HIDE);
			else
			{
				m_pInputControl->GetParentDialog(TRUE)->SetForegroundWindow();  
				m_pInputControl->SetControlFocus(TRUE);		
				keybd_event(g_keyvalue[i], 0, 0, 0);//按下S键  
			}
		}
	}
	return 0;
}
