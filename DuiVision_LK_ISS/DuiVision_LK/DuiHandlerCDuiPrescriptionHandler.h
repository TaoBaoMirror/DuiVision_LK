// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI事件处理类
class CDuiHandlerCDuiPrescriptionHandler : public CDuiHandler
{
public:
	CDuiHandlerCDuiPrescriptionHandler(void);
	virtual ~CDuiHandlerCDuiPrescriptionHandler(void);

	virtual void OnInit();
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	UINT m_uTimerAni;	// 动画定时器
	int m_nAniIndex;	// 动画索引

	//控件消息
	LRESULT OnDuiMsgBtnSearch(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	// 消息处理定义
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerCDuiPrescriptionHandler)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_pre_search"),MSG_BUTTON_DOWN,OnDuiMsgBtnSearch);
	DUI_DECLARE_MESSAGE_END()

};
