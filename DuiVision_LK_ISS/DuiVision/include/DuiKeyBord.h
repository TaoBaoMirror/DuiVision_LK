#pragma once
class CDuiKeyBord :  public CDlgBase
{
public:
	static LPCTSTR GetClassName() { return _T("CDuiKeyBord");}
	virtual BOOL IsClass(LPCTSTR lpszName)
	{
		if(_tcscmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}
	
	CDuiKeyBord(UINT nIDTemplate, CWnd* pParent = NULL);
	~CDuiKeyBord(void);
	DECLARE_DYNAMIC(CDuiKeyBord)
	void SetInputControl(CDuiEdit *P){m_pInputControl=P;}
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
public:
	DUI_DECLARE_ATTRIBUTES_BEGIN()
	DUI_DECLARE_ATTRIBUTES_END()

	CDuiEdit * m_pInputControl;
	
};
