// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI�¼�������
class CDuiHandlerCDuiMedManagerHandler : public CDuiHandler
{
public:
	CDuiHandlerCDuiMedManagerHandler(void);
	virtual ~CDuiHandlerCDuiMedManagerHandler(void);

	virtual void OnInit();
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	UINT m_uTimerAni;	// ������ʱ��
	int m_nAniIndex;	// ��������

	// ��Ϣ������
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerCDuiMedManagerHandler)
	DUI_DECLARE_MESSAGE_END()
};
