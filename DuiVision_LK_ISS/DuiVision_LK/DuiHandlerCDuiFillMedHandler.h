// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI事件处理类
class CDuiHandlerCDuiFillMedHandler : public CDuiHandler
{
public:
	CDuiHandlerCDuiFillMedHandler(void);
	virtual ~CDuiHandlerCDuiFillMedHandler(void);

	virtual void OnInit();
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	UINT m_uTimerAni;	// 动画定时器
	int m_nAniIndex;	// 动画索引
	int m_SortType;
	//控件消息
	LRESULT OnDuiMsgBtnSearch(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio1ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio2ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio3ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio4ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio5ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgRadio6ClickDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgGridLBClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnFillMed(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	// 消息处理定义
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerCDuiFillMedHandler)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_fill_search"),MSG_BUTTON_DOWN,OnDuiMsgBtnSearch);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio1"),MSG_BUTTON_UP,OnDuiMsgRadio1ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio2"),MSG_BUTTON_UP,OnDuiMsgRadio2ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio3"),MSG_BUTTON_UP,OnDuiMsgRadio3ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio4"),MSG_BUTTON_UP,OnDuiMsgRadio4ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio5"),MSG_BUTTON_UP,OnDuiMsgRadio5ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("Radio6"),MSG_BUTTON_UP,OnDuiMsgRadio6ClickDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("gridctrl_fillmed"),MSG_BUTTON_DOWN,OnDuiMsgGridLBClick);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_fill_up"),MSG_BUTTON_DOWN,OnDuiMsgBtnUp);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_fill_dowm"),MSG_BUTTON_DOWN,OnDuiMsgBtnDown);
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_fill_add"),MSG_BUTTON_DOWN,OnDuiMsgBtnFillMed);
	DUI_DECLARE_MESSAGE_END()
private:
	void showLackMedInfoList();
};
