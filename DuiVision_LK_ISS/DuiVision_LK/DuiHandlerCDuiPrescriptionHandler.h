// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI�¼�������
class CDuiHandlerCDuiPrescriptionHandler : public CDuiHandler
{
public:
	CDuiHandlerCDuiPrescriptionHandler(void);
	virtual ~CDuiHandlerCDuiPrescriptionHandler(void);

	virtual void OnInit();
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	UINT m_uTimerAni;	// ������ʱ��
	int m_nAniIndex;	// ��������

	//�ؼ���Ϣ
	LRESULT OnDuiMsgBtnSearch(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	// ��Ϣ������
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerCDuiPrescriptionHandler)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_pre_search"),MSG_BUTTON_DOWN,OnDuiMsgBtnSearch);
	DUI_DECLARE_MESSAGE_END()

};
