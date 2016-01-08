#pragma once
#include "ControlBase.h"

class CDuiGroupBox: public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiGroupBox, _T("groupbox"))
public:
	CDuiGroupBox(HWND hWnd, CDuiObject* pDuiObject);
	CDuiGroupBox(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, 
		Color clr = Color(254, 0, 0, 0), CString strFont = DuiSystem::GetDefaultFont(), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular, BOOL bIsVisible = TRUE);
	virtual ~CDuiGroupBox(void);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crback"), m_clrBack,FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crside"), m_clrside,FALSE)
		DUI_INT_ATTRIBUTE(_T("bk-transparent"), m_nBkTransparent, FALSE)
		DUI_INT_ATTRIBUTE(_T("sidewidth"), m_sidewidth, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
private:
	Color				m_clrBack;			// ±³¾°É«
	Color				m_clrText;			// ÎÄ×ÖÑÕÉ«
	Color               m_clrside;           // ±ß¿òÑÕÉ«
	int                 m_sidewidth;        //±ß¿ò¿í¶È
	int					m_nBkTransparent;	// ±³¾°Í¸Ã÷¶È
};

