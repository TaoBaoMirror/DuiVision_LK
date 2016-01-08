// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI事件处理类
class CDuiHandlerCDuiListenHandler : public CDuiHandler
{
public:
	CDuiHandlerCDuiListenHandler(void);
	virtual ~CDuiHandlerCDuiListenHandler(void);

	virtual void OnInit();
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	UINT m_uTimerAni;	// 动画定时器
	int m_nAniIndex;	// 动画索引

	//控件消息
	LRESULT OnDuiMsgRadio1ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio2ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio3ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio4ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio5ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio6ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	// 消息处理定义
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerCDuiListenHandler)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio1"),MSG_BUTTON_UP,OnDuiMsgRadio1ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio2"),MSG_BUTTON_UP,OnDuiMsgRadio2ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio3"),MSG_BUTTON_UP,OnDuiMsgRadio3ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio4"),MSG_BUTTON_UP,OnDuiMsgRadio4ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio5"),MSG_BUTTON_UP,OnDuiMsgRadio5ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio6"),MSG_BUTTON_UP,OnDuiMsgRadio6ClickDown);
		
	DUI_DECLARE_MESSAGE_END()
private:
	static unsigned int ThreadListenPro(LPVOID lparam);
	CWinThread *m_pNormalThread;
	long m_lThreadExit;
	int m_SortType;
	bool m_bSearchMed;
};
