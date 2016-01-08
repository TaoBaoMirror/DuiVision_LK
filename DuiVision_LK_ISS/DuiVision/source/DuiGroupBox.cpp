#include "StdAfx.h"
#include "DuiGroupBox.h"

CDuiGroupBox::CDuiGroupBox(HWND hWnd, CDuiObject* pDuiObject)
	:CControlBaseFont(hWnd,pDuiObject)
{
	m_clrBack = Color(0, 128, 0);
	m_clrText = Color(254, 0, 0, 0);
	m_clrside= Color(254, 0, 0, 0);
	m_nBkTransparent=0;
	m_sidewidth=1;
}

CDuiGroupBox::CDuiGroupBox(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, Color clr /*= Color(254, 0, 0, 0)*/, CString strFont /*= DuiSystem::GetDefaultFont()*/, int nFontWidth /*= 12*/, FontStyle fontStyle /*= FontStyleRegular*/, BOOL bIsVisible /*= TRUE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, m_strTitle, bIsVisible, FALSE, FALSE, strFont, nFontWidth, fontStyle)
{

}

CDuiGroupBox::~CDuiGroupBox(void)
{

}

void CDuiGroupBox::DrawControl(CDC &dc, CRect rcUpdate)
{
	
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();
	if(!m_bUpdate)
	{
		UpdateMemDC(dc,nWidth,nHeight);
		Graphics graphics(m_memDC);
		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, WHITENESS);	// 画白色背景
		DrawVerticalTransition(m_memDC, dc, CRect(0, 0, nWidth, nHeight),	// 背景透明度
			m_rc, m_nBkTransparent, m_nBkTransparent);
		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		SolidBrush solidBrush(m_clrText);

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// 设置水平和垂直对齐方式/	
		DUI_STRING_ALIGN_DEFINE();
		strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		strFormat.SetAlignment(StringAlignmentNear);
		Size size = GetTextBounds(font, strFormat, nWidth, m_strTitle);

		//根据属性定位文字和图片的位置
		RectF rcImage(10,0,(Gdiplus::REAL)(m_sizeImage.cx*4),(Gdiplus::REAL)m_sizeImage.cy);
		RectF rcText((Gdiplus::REAL)(rcImage.X+rcImage.Width+1),0, (Gdiplus::REAL)size.Width,(Gdiplus::REAL)size.Height);
		if(rcImage.Height>rcText.Height)
			rcText.Y=rcImage.Y+(rcImage.Height-rcText.Height)/2;
		if (m_rc.Width()>((rcImage.Width+1+rcText.Width)))
		{
			if (m_uAlignment==Align_Right)
			{
				rcText.X=m_rc.right-10-rcText.Width-m_rc.left;	
				rcImage.X=rcText.X-1-rcImage.Width;
			}
			if (m_uAlignment==Align_Center)
			{
				Gdiplus::REAL pos=(m_rc.right-m_rc.left-(rcImage.Width+1+rcText.Width))/2;
				rcImage.X=pos;
				rcText.X=pos+rcImage.Width+1;
			}
		}
		//图片区域
		if(m_pImage != NULL)
		{
			graphics.DrawImage(m_pImage, rcImage,
				0, 0, (Gdiplus::REAL)(m_sizeImage.cx*4), (Gdiplus::REAL)m_sizeImage.cy, UnitPixel);
		}
		//文字区域
		if (m_strTitle.IsEmpty()==FALSE&&m_strTitle!=_T(""))
		{
			SolidBrush solidBrushH(m_clrText);
			BSTR bsTitle = m_strTitle.AllocSysString();
			graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rcText, &strFormat, &solidBrushH);
			::SysFreeString(bsTitle);
		}
		//边跨区域画线
		//定义画笔
		Pen pen(m_clrside, (Gdiplus::REAL)m_sidewidth);

		//左顶点到底部区域
		graphics.DrawLine(&pen,Point(m_sidewidth/2,rcText.Y+(rcText.Height)/2),Point(m_sidewidth/2,m_rc.Height()));
		//左顶点到文字图片区域
		graphics.DrawLine(&pen,Point(0,rcText.Y+(rcText.Height)/2),Point(rcImage.X-5,rcText.Y+(rcText.Height)/2));
		//文字图片区域到右上点
		graphics.DrawLine(&pen,Point(rcText.X+rcText.Width+m_sidewidth,rcText.Y+(rcText.Height)/2),Point(m_rc.Width(),rcText.Y+(rcText.Height)/2));
		//左下点到右下点
		graphics.DrawLine(&pen,Point(0,m_rc.Height()-m_sidewidth),Point(m_rc.Width(),m_rc.Height()-m_sidewidth));
		//右上点到右下点
		graphics.DrawLine(&pen,Point(m_rc.Width()-m_sidewidth,rcText.Y+(rcText.Height)/2),Point(m_rc.Width()-m_sidewidth,m_rc.Height()));
	}
	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}
