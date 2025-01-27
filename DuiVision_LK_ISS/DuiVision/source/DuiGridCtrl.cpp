#include "StdAfx.h"
#include "DuiListCtrl.h"

#define	SCROLL_V	1	// 垂直滚动条控件ID
#define	SCROLL_H	2	// 水平滚动条控件ID
#define	LISTBK_AREA	3	// 背景Area控件ID

CDuiGridCtrl::CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiPanel(hWnd, pDuiObject)
{
	CRect rcBk = CRect(0,0,0,0);
	CControlBase* pControlBase = new CArea(hWnd, this, LISTBK_AREA, rcBk, 100, 100);
 	m_vecControl.push_back(pControlBase);
	m_pControBkArea = (CControlBase*)pControlBase;

	m_strFontTitle = DuiSystem::GetDefaultFont();
	m_nFontTitleWidth = 12;
	m_fontTitleStyle = FontStyleRegular;

	m_clrText = Color(225, 64, 64, 64);
	m_clrTextHover = Color(128, 0, 0);
	m_clrTextDown = Color(0, 112, 235);
	m_clrTitle = Color(255, 32, 32, 32);
	m_clrSeperator = Color(0, 0, 0, 0);
	m_clrRowHover = Color(0, 128, 128, 128);	// 鼠标移动到行显示的背景色,默认是透明色
	m_nRowHeight = 50;
	m_nHeaderHeight = 0;
	m_nLeftPos = 0;

	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);

	m_nBkTransparent = 30;

	m_bDblClk = true;

	m_nHoverRow = 0;
	m_nDownRow = -1;
	m_bEnableDownRow = FALSE;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;

	m_bGridTooltip = TRUE;
	m_nTipRow = -1;
	m_nTipItem = -1;
	m_nTipVirtualTop = 0;

	m_nFirstViewRow = 0;
	m_nLastViewRow = 0;
	m_nVirtualTop = 0;
	m_nVirtualLeft = 0;
	InitializeCriticalSection(&m_cs);
}

CDuiGridCtrl::~CDuiGridCtrl(void)
{
	m_vecControl.clear();
	m_vecRowInfo.clear();
	DeleteCriticalSection(&m_cs);
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}
	if(m_pImageCheckBox != NULL)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}
}

// 图片属性的实现
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, CheckBox, 6)

// 加载XML节点，解析节点中的属性信息设置到当前控件的属性中
BOOL CDuiGridCtrl::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // 使用XML节点初始化div控件
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

	// 加载下层的cloumn节点信息
	for (DuiXmlNode pColumnElem = pXmlElem.child(_T("column")); pColumnElem; pColumnElem=pColumnElem.next_sibling(_T("column")))
	{
		CString strTitle = pColumnElem.attribute(_T("title")).value();
		CString strClrText = pColumnElem.attribute(_T("crtext")).value();
		CString strWidth = pColumnElem.attribute(_T("width")).value();
		CString strAlign = pColumnElem.attribute(_T("align")).value();
		CString strVAlign = pColumnElem.attribute(_T("valign")).value();
		CString strImage= pColumnElem.attribute(_T("bkimage")).value();
		DuiSystem::Instance()->ParseDuiString(strTitle);
		Color clrText = Color(0, 0, 0, 0);
		if(!strClrText.IsEmpty())
		{
			clrText = CDuiObject::StringToColor(strClrText);
		}
		int nWidth = -1;
		if(!strWidth.IsEmpty())
		{
			nWidth = _ttoi(strWidth);
		}
		UINT uAlignment = 0xFFFFUL;
		if(strAlign == _T("left"))
		{
			uAlignment = Align_Left;
		}else
		if(strAlign ==_T("center"))
		{
			uAlignment = Align_Center;
		}else
		if(strAlign == _T("right"))
		{
			uAlignment = Align_Right;
		}
		UINT uVAlignment = 0xFFFFUL;
		if(strVAlign == _T("top"))
		{
			uVAlignment = VAlign_Top;
		}else
		if(strVAlign == _T("middle"))
		{
			uVAlignment = VAlign_Middle;
		}else
		if(strVAlign == _T("bottom"))
		{
			uVAlignment = VAlign_Bottom;
		}

		CString strSkin = _T("");
		if(strImage.Find(_T("skin:")) == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImage);
		}else
		{
			strSkin = strImage;
		}
		InsertColumn(-1, strTitle, nWidth, clrText, uAlignment, uVAlignment,strSkin);
	}

	// 加载下层的row节点信息
	for (DuiXmlNode pRowElem = pXmlElem.child(_T("row")); pRowElem; pRowElem=pRowElem.next_sibling(_T("row")))
	{
		CString strId = pRowElem.attribute(_T("id")).value();
		CString strCheck = pRowElem.attribute(_T("check")).value();
		CString strImage = pRowElem.attribute(_T("image")).value();
		CString strRightImage = pRowElem.attribute(_T("right-img")).value();
		CString strClrText = pRowElem.attribute(_T("crtext")).value();

		int nCheck = -1;
		if(!strCheck.IsEmpty())
		{
			nCheck = _ttoi(strCheck);
		}

		// 左边图片,通过Skin读取
		CString strSkin = _T("");
		if(strImage.Find(_T("skin:")) == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImage);
		}else
		{
			strSkin = strImage;
		}

		int nImageIndex = -1;
		strImage = _T("");
		if(strSkin.Find(_T(".")) != -1)
		{
			// 图片文件
			strImage = strSkin;
		}else
		if(!strSkin.IsEmpty())
		{
			// 图片索引
			nImageIndex = _ttoi(strSkin);
		}

		// 右边图片,通过Skin读取
		CString strRightSkin = _T("");
		if(strRightImage.Find(_T("skin:")) == 0)
		{
			strRightSkin = DuiSystem::Instance()->GetSkin(strRightImage);
		}else
		{
			strRightSkin = strRightImage;
		}

		int nRightImageIndex = -1;
		strRightImage = _T("");
		if(strRightSkin.Find(_T(".")) != -1)
		{
			// 图片文件
			strRightImage = strRightSkin;
		}else
		if(!strRightSkin.IsEmpty())
		{
			// 图片索引
			nRightImageIndex = _ttoi(strRightSkin);
		}

		Color clrText = CDuiObject::StringToColor(strClrText);

		InsertRow(-1, strId, nImageIndex, clrText, strImage, nRightImageIndex, strRightImage, nCheck);

		int nRowIndex = m_vecRowInfo.size()-1;
		int nItemIndex = 0;
		// 加载下层的item节点信息
		for (DuiXmlNode pItemElem = pRowElem.child(_T("item")); pItemElem; pItemElem=pItemElem.next_sibling(_T("item")))
		{
			CString strTitle = pItemElem.attribute(_T("title")).value();
			CString strContent = pItemElem.attribute(_T("content")).value();
			CString strClrText = pItemElem.attribute(_T("crtext")).value();
			CString strImage = pItemElem.attribute(_T("image")).value();
			CString strLink = pItemElem.attribute(_T("link")).value();
			CString strLinkAction = pItemElem.attribute(_T("linkaction")).value();
			CString strFontTitle = pItemElem.attribute(_T("font-title")).value();
			DuiSystem::Instance()->ParseDuiString(strTitle);
			DuiSystem::Instance()->ParseDuiString(strContent);
			DuiSystem::Instance()->ParseDuiString(strLink);
			DuiSystem::Instance()->ParseDuiString(strLinkAction);
			Color clrText = CDuiObject::StringToColor(strClrText);

			// 图片,通过Skin读取
			CString strSkin = _T("");
			if(strImage.Find(_T("skin:")) == 0)
			{
				strSkin = DuiSystem::Instance()->GetSkin(strImage);
			}else
			{
				strSkin = strImage;
			}

			int nImageIndex = -1;
			strImage = _T("");
			if(strSkin.Find(_T(".")) != -1)
			{
				// 图片文件
				strImage = strSkin;
			}else
			if(!strSkin.IsEmpty())
			{
				// 图片索引
				nImageIndex = _ttoi(strSkin);
			}

			BOOL bUseTitleFont = (strFontTitle == _T("1"));

			if(!strLink.IsEmpty())
			{
				SetSubItemLink(nRowIndex, nItemIndex, strLink, strLinkAction, nImageIndex, clrText, strImage);
			}else
			{
				SetSubItem(nRowIndex, nItemIndex, strTitle, strContent, bUseTitleFont, nImageIndex, clrText, strImage);
			}

			// 加载下层的控件节点信息
			GridItemInfo* pItemInfo = GetItemInfo(nRowIndex, nItemIndex);
			for (DuiXmlNode pControlElem = pItemElem.first_child(); pControlElem; pControlElem=pControlElem.next_sibling())
			{
				if((pControlElem != NULL) && (pItemInfo != NULL))
				{
					CString strControlName = pControlElem.name();
					CControlBase* pControl = _CreateControlByName(strControlName);
					if(pControl)
					{
						pControl->Load(pControlElem);
						pControl->SetVisible(FALSE);
						// 将控件指针添加到gridctrl控件的子控件列表中
						m_vecControl.push_back(pControl);
						// 将控件指针添加到单元格的控件列表中(仅用于按照单元格查找子控件)
						pItemInfo->vecControl.push_back(pControl);
					}
				}
			}

			nItemIndex++;
		}
	}

	// 计算横向滚动条
	CalcColumnsPos();
	// 计算每一行的位置和滚动条
	CalcRowsPos();

    return TRUE;
}

// 添加列
BOOL CDuiGridCtrl::InsertColumn(int nColumn, CString strTitle, int nWidth, Color clrText, UINT uAlignment, UINT uVAlignment,CString strImage)
{
	GridColumnInfo columnInfo;
	columnInfo.strTitle = strTitle;
	columnInfo.clrText = clrText;
	columnInfo.nWidth = nWidth;
	columnInfo.uAlignment = uAlignment;
	columnInfo.uVAlignment = uVAlignment;

	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(!DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, columnInfo.pImage))
			columnInfo.pImage=NULL;
	}
	else
		columnInfo.pImage=NULL;
	if(nColumn <= -1 || nColumn >= (int)m_vecColumnInfo.size())
	{
		m_vecColumnInfo.push_back(columnInfo);
	}
	else
	{
		m_vecColumnInfo.insert(m_vecColumnInfo.begin() + nColumn, columnInfo);
	}

	int nXPos = 0;
	int nYPos = 0;

	for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
	{
		GridColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
		int nWidth = columnInfoTemp.nWidth;
		if(nWidth == -1)	// -1表示最后一列为自适应宽度
		{
			nWidth = m_rc.Width() - nXPos;
			if(nWidth < 0)
			{
				nWidth = 100;	// 如果宽度不够,设置一个最小值
			}
		}
		columnInfoTemp.rcHeader.SetRect(nXPos, nYPos, nXPos + nWidth, nYPos + m_nRowHeight);
		nXPos += columnInfoTemp.nWidth;
	}

	// 计算横向滚动条
	CalcColumnsPos();

	UpdateControl(true);
	return true;
}

// 获取总的列宽
int CDuiGridCtrl::GetTotalColumnWidth()
{
	int nTotalWidth = 0;

	for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
	{
		GridColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
		int nWidth = columnInfoTemp.nWidth;
		if(nWidth == -1)	// -1表示最后一列为自适应宽度
		{
			nWidth = m_rc.Width() - nTotalWidth;
			if(nWidth < 0)
			{
				nWidth = 100;	// 如果宽度不够,设置一个最小值
			}
		}
		nTotalWidth += nWidth;
	}

	return nTotalWidth;
}

// 添加行
int CDuiGridCtrl::InsertRow(int nRow, CString strId, int nImageIndex, Color clrText, CString strImage,
							 int nRightImageIndex, CString strRightImage, int nCheck)
{
	GridRowInfo rowInfo;
	rowInfo.strId = strId;
	rowInfo.nCheck = nCheck;
	rowInfo.nImageIndex = nImageIndex;
	rowInfo.sizeImage.SetSize(0, 0);
	rowInfo.nRightImageIndex = nRightImageIndex;
	rowInfo.sizeRightImage.SetSize(0, 0);
	rowInfo.bRowColor = FALSE;
	rowInfo.clrText = clrText;
	rowInfo.nHoverItem = -1;
	if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		rowInfo.bRowColor = TRUE;
	}

	// 左边图片
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, rowInfo.pImage))
		{
			rowInfo.sizeImage.SetSize(rowInfo.pImage->GetWidth() / 1, rowInfo.pImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		rowInfo.pImage = NULL;
		if((rowInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// 右边图片
	if(!strRightImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strRightImage, m_bImageUseECM, rowInfo.pRightImage))
		{
			rowInfo.sizeRightImage.SetSize(rowInfo.pRightImage->GetWidth() / 1, rowInfo.pRightImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		rowInfo.pRightImage = NULL;
		if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeRightImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return InsertRow(nRow, rowInfo);
}

// 添加行
int CDuiGridCtrl::InsertRow(int nRow, GridRowInfo &rowInfo)
{
	int nRetRow = -1;
	if(nRow <= -1 || nRow >= (int)m_vecRowInfo.size())
	{
		EnterCriticalSection(&m_cs);
		m_vecRowInfo.push_back(rowInfo);
		LeaveCriticalSection(&m_cs);
		nRetRow = m_vecRowInfo.size()-1;
	}
	else
	{
		EnterCriticalSection(&m_cs);
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nRow, rowInfo);
		LeaveCriticalSection(&m_cs);
		nRetRow = nRow;
	}

	// 计算所有表格行的位置
	CalcRowsPos();	

	UpdateControl(true);
	return nRetRow;
}

// 设置表格项内容(文字表格项)
BOOL CDuiGridCtrl::SetSubItem(int nRow, int nItem, CString strTitle, CString strContent, BOOL bUseTitleFont, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		GridItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = bUseTitleFont;
		GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strTitle = strTitle;
	itemInfo.strContent = strContent;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	GridColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// 图片
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// 设置表格项内容(链接表格项)
BOOL CDuiGridCtrl::SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		GridItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = FALSE;
		GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strLink = strLink;
	itemInfo.strLinkAction = strLinkAction;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	GridColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// 图片
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// 给表格项添加子控件
BOOL CDuiGridCtrl::AddSubItemControl(int nRow, int nItem, CControlBase* pControl)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridItemInfo* pItemInfo = GetItemInfo(nRow, nItem);
	if(pItemInfo == NULL)
	{
		SetSubItem(nRow, nItem, _T(""));
		pItemInfo = GetItemInfo(nRow, nItem);
	}
	if(pItemInfo == NULL)
	{
		return FALSE;
	}

	if(pControl)
	{
		pControl->SetParent(this);
		pControl->SetVisible(FALSE);
		// 将控件指针添加到gridctrl控件的子控件列表中
		m_vecControl.push_back(pControl);
		// 将控件指针添加到单元格的控件列表中(仅用于按照单元格查找子控件)
		pItemInfo->vecControl.push_back(pControl);
		UpdateControl(true);
	}

	return TRUE;
}

// 删除表格项的指定子控件(通过控件对象指针删除)
BOOL CDuiGridCtrl::DeleteSubItemControl(CControlBase* pControl)
{
	// 查找所有单元格,删除对应的控件对象引用
	EnterCriticalSection(&m_cs);
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* _pControl = *it;
				if(_pControl == pControl)
				{
					itemInfo.vecControl.erase(it);
					break;
				}
			}
		}
	}

	// 删除子控件中对应的控件对象
	RemoveControl(pControl);
	LeaveCriticalSection(&m_cs);
	return TRUE;
}

// 删除表格项的指定子控件(通过控件名和控件ID删除)
BOOL CDuiGridCtrl::DeleteSubItemControl(CString strControlName, UINT uControlID)
{
	// 查找所有单元格,删除对应的控件对象引用
	EnterCriticalSection(&m_cs);
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* _pControl = *it;
				if(_pControl && _pControl->IsThisObject(uControlID, strControlName))
				{
					itemInfo.vecControl.erase(it);
					break;
				}
			}
		}
	}

	// 删除子控件中对应的控件对象
	RemoveControl(strControlName, uControlID);
	LeaveCriticalSection(&m_cs);
	return TRUE;
}

// 删除行
BOOL CDuiGridCtrl::DeleteRow(int nRow)
{
	if(m_vecRowInfo.size()==0)
		return TRUE;
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}

	// 查找行的所有单元格,删除对应的子控件对象
	EnterCriticalSection(&m_cs);
	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
	{
		GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
		vector<CControlBase*>::iterator it;
		for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
		{
			CControlBase* _pControl = *it;
			// 删除单元格包含的子控件
			RemoveControl(_pControl);
		}
	}

	// 删除行信息
	int nIndex = 0;
	vector<GridRowInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nRow)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}
	LeaveCriticalSection(&m_cs);
	// 计算所有表格行的位置
	CalcRowsPos();

	UpdateControl(true);
	return true;
}

// 计算表格行位置
void CDuiGridCtrl::CalcRowsPos()
{
	int nXPos = 0;//m_rc.left;
	int nYPos = 0;//m_rc.top;

	// 计算每一行的位置
	EnterCriticalSection(&m_cs);
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		rowInfoTemp.rcCheck.SetRect(0,0,0,0);

		// 计算单元格位置
		for(size_t j = 0; j < rowInfoTemp.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfoTemp.vecItemInfo.at(j);
			GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
			itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfoTemp.rcRow.top,
					columnInfo.rcHeader.right, rowInfoTemp.rcRow.bottom);
		}

		nYPos += m_nRowHeight;
	}
	LeaveCriticalSection(&m_cs);
	// 需要的总高度大于显示区高度才会显示滚动条
	m_pControScrollV->SetVisible(((int)m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);
}

// 计算表格列位置
void CDuiGridCtrl::CalcColumnsPos()
{
	int nTotalWidth = GetTotalColumnWidth();

	// 需要的总高度大于显示区高度才会显示滚动条
	m_pControScrollH->SetVisible(nTotalWidth > m_rc.Width());
	((CDuiScrollHorizontal*)m_pControScrollH)->SetScrollMaxRange(nTotalWidth);

	// 设置水平滚动条位置
	if(nTotalWidth > m_rc.Width())
	{
		CRect rcTemp = m_rc;
		rcTemp.top = rcTemp.bottom - m_nScrollWidth;
		rcTemp.right = rcTemp.right - m_nScrollWidth;
		m_pControScrollH->SetRect(rcTemp);
	}
}

// 将指定的行滚动到可见范围
BOOL CDuiGridCtrl::EnsureVisible(int nRow, BOOL bPartialOK)
{
	// 如果指定的行已经处于可见范围则直接返回
	if((nRow >= m_nFirstViewRow) && (nRow <= m_nLastViewRow))
	{
		return TRUE;
	}

	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	if(nRow < m_nFirstViewRow)
	{
		pScrollV->SetScrollCurrentPos(nRow * m_nRowHeight);
	}else
	{
		int nFirstRow = nRow - ((m_rc.Height() - m_nHeaderHeight) / m_nRowHeight) + 2;
		if(nFirstRow < 0)
		{
			nFirstRow = 0;
		}
		pScrollV->SetScrollCurrentPos(nFirstRow * m_nRowHeight);
	}

	UpdateControl(true);
	return TRUE;
}

// 获取某一个行信息
GridRowInfo* CDuiGridCtrl::GetRowInfo(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// 获取某一个单元格信息
GridItemInfo* CDuiGridCtrl::GetItemInfo(int nRow, int nItem)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return NULL;
	}

	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	if(nItem >= (int)rowInfo.vecItemInfo.size())
	{
		return NULL;
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);

	return &itemInfo;
}

// 设置某一个行的颜色
void CDuiGridCtrl::SetRowColor(int nRow, Color clrText)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// 设置某一个行的检查框状态
void CDuiGridCtrl::SetRowCheck(int nRow, int nCheck)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.nCheck = nCheck;
}

// 获取某一行的检查框状态
int CDuiGridCtrl::GetRowCheck(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return -1;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return rowInfo.nCheck;
}

// 清空列表
void CDuiGridCtrl::ClearItems()
{
	// 删除所有子控件
	EnterCriticalSection(&m_cs);
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* pControl = *it;
				RemoveControl(pControl);
			}
		}
	}

	m_vecRowInfo.clear();
	LeaveCriticalSection(&m_cs);
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// 从XML设置Font-title属性
HRESULT CDuiGridCtrl::OnAttributeFontTitle(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	DuiFontInfo fontInfo;
	BOOL bFindFont = DuiSystem::Instance()->GetFont(strValue, fontInfo);
	if (!bFindFont) return E_FAIL;

	m_strFontTitle = fontInfo.strFont;
	m_nFontTitleWidth = fontInfo.nFontWidth;	
	m_fontTitleStyle = fontInfo.fontStyle;

	return bLoading?S_FALSE:S_OK;
}

void CDuiGridCtrl::SetControlRect(CRect rc)
{
	int nTotalColumnWidth = GetTotalColumnWidth();

	m_rc = rc;
	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			UINT uControlID = pControlBase->GetControlID();
			if(SCROLL_V == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top += m_nHeaderHeight;
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			if((SCROLL_H == uControlID) && (nTotalColumnWidth > m_rc.Width()))
			{
				rcTemp = m_rc;
				rcTemp.top = rcTemp.bottom - m_nScrollWidth;
				rcTemp.right = rcTemp.right - m_nScrollWidth;
			}else
			if(LISTBK_AREA == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top += m_nHeaderHeight;
				rcTemp.right -= m_nScrollWidth;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// 计算横向滚动条
	CalcColumnsPos();
	// 重新计算所有行的位置
	CalcRowsPos();
}

// 判断指定的坐标位置是否在某一行中
BOOL CDuiGridCtrl::PtInRow(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcRow;
	// rcRow坐标是插入行节点时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
	rc.OffsetRect(m_rc.left, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// 判断指定的坐标位置是否在某一行的检查框中
BOOL CDuiGridCtrl::PtInRowCheck(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcCheck;
	// rcCheck坐标是画图时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
	rc.OffsetRect(m_rc.left - m_nVirtualLeft, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// 判断指定的坐标位置是否在某一行的某个列中,返回列索引
int CDuiGridCtrl::PtInRowItem(CPoint point, GridRowInfo& rowInfo)
{
	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		CRect rc = itemInfo.rcItem;
		// rcItem坐标是画图时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
		rc.OffsetRect(m_rc.left + ((i == 0) ? m_nLeftPos : 0) - m_nVirtualLeft, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
		if(i == 0)
		{
			rc.right -= m_nLeftPos;
		}
		if(rc.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

// 设置单元格的Tooltip
void CDuiGridCtrl::SetGridTooltip(int nRow, int nItem, CString strTooltip)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	CDlgBase* pDlg = GetParentDialog();
	if(pDlg && ((m_nTipRow != nRow) || (m_nTipItem != nItem) || (m_nTipVirtualTop != m_nVirtualTop)))
	{
		GridItemInfo* pGridInfo = GetItemInfo(nRow, nItem);
		if(pGridInfo && (pGridInfo->bNeedTitleTip || pGridInfo->bNeedContentTip))
		{
			CRect rc = pGridInfo->rcItem;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop+m_nHeaderHeight);
			pDlg->SetTooltip(this, strTooltip, rc, TRUE);
		}else
		{
			pDlg->ClearTooltip();
		}
		m_nTipRow = nRow;
		m_nTipItem = nItem;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// 清除Tooltip
void CDuiGridCtrl::ClearGridTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipRow = -1;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
	}
}

// 鼠标移动事件处理
BOOL CDuiGridCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	int nOldHoverRow = m_nHoverRow;
	BOOL bHoverItemChange = FALSE;
	int nOldHoverItem = -1;

	if(m_rc.PtInRect(point))
	{
		if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
					UpdateControl(TRUE);
				}

				if(m_bGridTooltip)	// 设置单元格Tooltip
				{
					GridItemInfo* pGridInfo = GetItemInfo(m_nHoverRow, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(m_nHoverRow, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(m_nHoverRow, rowInfo.nHoverItem, pGridInfo->strContent);
					}else
					{
						ClearGridTooltip();
					}
				}

				return false;
			}
			m_nHoverRow = -1;		
		}

		BOOL bMousenDown = false;
		if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				bMousenDown = true;
				m_nHoverRow = -1;
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
				}

				if(m_bGridTooltip)	// 设置单元格Tooltip
				{
					GridItemInfo* pGridInfo = GetItemInfo(m_nDownRow, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(m_nDownRow, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(m_nDownRow, rowInfo.nHoverItem, pGridInfo->strContent);
					}else
					{
						ClearGridTooltip();
					}
				}
			}		
		}

		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				nOldHoverItem = rowInfo.nHoverItem;
				if(PtInRow(point, rowInfo))
				{
					rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
					m_nHoverRow = i;
					if(nOldHoverItem != rowInfo.nHoverItem)
					{
						bHoverItemChange = TRUE;
					}
					break;
				}
			}
		}
	}
	else
	{
		m_nHoverRow = -1;
	}

	if((nOldHoverRow != m_nHoverRow) || bHoverItemChange)
	{
		UpdateControl(TRUE);
		return true;
	}
	
	return false;
}

// 鼠标左键按下事件处理
BOOL CDuiGridCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// 设置窗口焦点,否则可能无法进行滚动事件的处理
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// 检查框事件只在鼠标放开时候触发
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(MSG_BUTTON_DOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// 如果点击的还是之前点击的行，也同样会发送鼠标点击事件
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// 检查框事件只在鼠标放开时候触发
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_BUTTON_DOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

// 鼠标左键放开事件处理
BOOL CDuiGridCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// 检查框状态改变
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_CHECK, m_nHoverRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// 如果点击的还是之前点击的行，也同样会发送鼠标点击事件
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// 检查框状态改变
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_CHECK, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}
	}

	return false;
}

// 鼠标左键双击事件处理
BOOL CDuiGridCtrl::OnControlLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if(m_rc.PtInRect(point))
	{
		for(size_t i = 0; i < m_vecRowInfo.size(); i++)
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(i);
			if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))
			{
				int nClickItem = PtInRowItem(point, rowInfo);
				SendMessage(MSG_BUTTON_DBLCLK, i, nClickItem);
				return true;
			}
		}
	}

	return false;
}

// 垂直滚动事件处理
BOOL CDuiGridCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(((int)m_vecRowInfo.size() * m_nRowHeight) <= m_rc.Height())
	{
		return false;
	}

	// 更新滚动条,并刷新界面
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	if(pScrollV->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

// 鼠标右键按下事件处理
BOOL CDuiGridCtrl::OnControlRButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// 设置窗口焦点,否则可能无法进行滚动事件的处理
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// 检查框事件只在鼠标放开时候触发
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(MSG_MOUSE_RDOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	} else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// 如果点击的还是之前点击的行，也同样会发送鼠标点击事件
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// 检查框事件只在鼠标放开时候触发
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_MOUSE_RDOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

// 鼠标右键放开事件处理
BOOL CDuiGridCtrl::OnControlRButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			//取反，否则消息不能发送
			if(!PtInRowCheck(point, rowInfo))	// 检查框状态改变
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_MOUSE_RUP, m_nHoverRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// 如果点击的还是之前点击的行，也同样会发送鼠标点击事件
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// 检查框状态改变
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_MOUSE_RUP, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}
	}

	return false;
}

// 消息响应
LRESULT CDuiGridCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(((uID == SCROLL_V) || (uID == SCROLL_H)) && (Msg == MSG_SCROLL_CHANGE))
	{
		// 如果是滚动条的位置变更事件,则刷新显示
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == MSG_BUTTON_DOWN) && (lParam != -1))
	{
		// 点击了行的某个链接
		GridRowInfo* pRowInfo = GetRowInfo(wParam);
		if(pRowInfo && (lParam >= 0) && (lParam < (int)pRowInfo->vecItemInfo.size()))
		{
			GridItemInfo &itemInfo = pRowInfo->vecItemInfo.at(lParam);
			// 转换为MSG_BUTTON_UP消息,因为DuiSystem任务处理时候只处理MSG_BUTTON_UP消息
			if(!itemInfo.strLinkAction.IsEmpty())
			{
				DuiSystem::AddDuiActionTask(uID, MSG_BUTTON_UP, wParam, lParam, GetName(), itemInfo.strLinkAction, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiGridCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{

	// 列表画图方法:
	// 1.列表的虚拟高度为每一行高度*行数
	// 2.列表显示的top坐标由scroll控件记录
	// 3.重画时候,根据top坐标位置计算出显示的第一行的序号,根据显示高度计算出显示的最后一行的序号
	// 4.根据计算出的显示的行,画相应的内容到内存dc中
	// 5.计算出显示的top坐标进行内存dc的拷贝
	int nTotalColumnWidth = GetTotalColumnWidth();	// 总的列宽度
	int nViewWidth = m_rc.Width() - m_nScrollWidth;	// 减去滚动条的显示区域宽度
	CDuiScrollHorizontal* pScrollH = (CDuiScrollHorizontal*)m_pControScrollH;
	int nCurPosH = pScrollH->GetScrollCurrentPos();	// 当前left位置
	int nMaxRangeH = pScrollH->GetScrollMaxRange();
	int nContentWidth = (nTotalColumnWidth > nViewWidth) ? nTotalColumnWidth : nViewWidth;	// 内容部分的宽度(如果总的列宽小于显示区域宽度,则使用显示区域宽度)
	m_nVirtualLeft = (nMaxRangeH > 0) ? (int)((double)nCurPosH*(nContentWidth-nViewWidth)/nMaxRangeH) : 0;	// 当前滚动条位置对应的虚拟的left位置

	int nHeightAll = m_vecRowInfo.size()*m_nRowHeight+ m_nHeaderHeight; // 总的虚拟高度 //m_rc.Height();
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	int nCurPosV = pScrollV->GetScrollCurrentPos();	// 当前top位置
	int nMaxRangeV = pScrollV->GetScrollMaxRange();
	m_nVirtualTop = (nMaxRangeV > 0) ? (int)((double)nCurPosV*(nHeightAll-m_rc.Height())/nMaxRangeV) : 0;	// 当前滚动条位置对应的虚拟的top位置
	if(m_nVirtualTop < 0)
	{
		m_nVirtualTop = 0;
		pScrollV->SetScrollCurrentPos(0);
	}
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;					// 显示的第一行序号
	m_nLastViewRow = (m_nVirtualTop + m_rc.Height() - m_nHeaderHeight) / m_nRowHeight;	// 显示的最后一行序号
	if(m_nLastViewRow >= (int)m_vecRowInfo.size())
	{
		m_nLastViewRow = m_vecRowInfo.size() - 1;
	}
	if(m_nLastViewRow < 0)
	{
		m_nLastViewRow = 0;
	}
	int nHeightView = (m_nLastViewRow - m_nFirstViewRow +1) * m_nRowHeight+ m_nHeaderHeight;	// 显示涉及到的虚拟高度
	int nYViewPos = m_nVirtualTop - (m_nFirstViewRow * m_nRowHeight);		// 内存dc显示到屏幕时候的top位置
	if(nYViewPos < 0)
	{
		nYViewPos = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nTotalColumnWidth, nHeightView);

		Graphics graphics(m_memDC);

		m_memDC.BitBlt(m_nVirtualLeft, 0, nViewWidth, nHeightView, &dc, m_rc.left, m_rc.top, WHITENESS);	// 画白色背景
		DrawVerticalTransition(m_memDC, dc, CRect(m_nVirtualLeft, nYViewPos, nViewWidth+m_nVirtualLeft, m_rc.Height()+nYViewPos/*-m_nHeaderHeight*/),	// 背景透明度
			m_rc, m_nBkTransparent, m_nBkTransparent);

		BSTR bsFontTitle = m_strFontTitle.AllocSysString();
		FontFamily fontFamilyTitle(bsFontTitle);
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);
		::SysFreeString(bsFontTitle);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		SolidBrush solidBrush(m_clrText);			// 正常文字画刷
		SolidBrush solidBrushH(m_clrTextHover);		// 热点文字画刷
		SolidBrush solidBrushD(m_clrTextDown);		// 当前行画刷
		SolidBrush solidBrushT(m_clrTitle);			// 标题文字画刷
		SolidBrush solidBrushS(m_clrSeperator);		// 分割线画刷

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// 设置普通文字的水平和垂直对齐方式
		DUI_STRING_ALIGN_DEFINE();

		strFormat.SetTrimming(StringTrimmingEllipsisWord);	// 以单词为单位去尾,略去部分使用省略号
		//strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormat.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// 不换行
		}

		// 标题字段采用中间对齐
		StringFormat strFormatHeader;
		strFormatHeader.SetAlignment(StringAlignmentCenter);	// 中间对齐
		strFormatHeader.SetLineAlignment(StringAlignmentCenter);	// 中间对齐
		if(!m_bTextWrap)
		{
			strFormatHeader.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// 不换行
		}

		// 画标题行
		if((m_nHeaderHeight > 0) && (m_vecColumnInfo.size() > 0))
		{
			// 画单元格内容
			int nPosItemX = 0;
			for(size_t j = 0; j < m_vecColumnInfo.size(); j++)
			{
				GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
				int nWidth = columnInfo.nWidth;
				if(j == 0)
				{
					nWidth += m_nLeftPos;
				}
				RectF rect((Gdiplus::REAL)nPosItemX, 0, (Gdiplus::REAL)nWidth, (Gdiplus::REAL)(m_nHeaderHeight-1));
				//绘制标题背景图片
				if(columnInfo.pImage!=NULL)
					graphics.DrawImage(columnInfo.pImage, rect,0, 0, columnInfo.pImage->GetWidth(), columnInfo.pImage->GetHeight(), UnitPixel);
				// 画列标题
				CString strTitle = columnInfo.strTitle;
				//	BSTR bsTitle = m_strTitle.AllocSysString();
				BSTR bsTitle = strTitle.AllocSysString();
				if(nWidth!=0)
					graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormatHeader, &solidBrushT);
				::SysFreeString(bsTitle);

				nPosItemX += nWidth;
			}
		}

		if(m_vecRowInfo.size() > 0)
		{
			EnterCriticalSection(&m_cs);
			for(int i = m_nFirstViewRow; i <= m_nLastViewRow && i < (int)m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				SolidBrush solidBrushRow(rowInfo.clrText);	// 行定义的颜色

				int nXPos = 0;
				int nVI = i - m_nFirstViewRow;

				// 鼠标移动到行时候显示的背景颜色(如果设置为全0,则不显示行背景颜色)
				if((m_nHoverRow == i) && (m_clrRowHover.GetValue() != Color(0, 0, 0, 0).GetValue()))
				{
					SolidBrush brush(m_clrRowHover);
					graphics.FillRectangle(&brush, 0, m_nHeaderHeight + nVI*m_nRowHeight, nContentWidth, m_nRowHeight);
				}

				// 画检查框
				int nCheckImgY = 3;
				if((m_sizeCheckBox.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
				{
					nCheckImgY = (m_nRowHeight - m_sizeCheckBox.cy) / 2 + 1;
				}
				if((rowInfo.nCheck != -1) && (m_pImageCheckBox != NULL))
				{
					int nCheckImageIndex = ((m_nHoverRow == i) ? ((rowInfo.nCheck==1) ? 4 : 1) : ((rowInfo.nCheck==1) ? 2 : 0));
					graphics.DrawImage(m_pImageCheckBox, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nCheckImgY, m_sizeCheckBox.cx, m_sizeCheckBox.cy),
						nCheckImageIndex * m_sizeCheckBox.cx, 0, m_sizeCheckBox.cx, m_sizeCheckBox.cy, UnitPixel);
					rowInfo.rcCheck.SetRect(nXPos, i*m_nRowHeight + nCheckImgY, nXPos + m_sizeCheckBox.cx, i*m_nRowHeight + nCheckImgY + m_sizeCheckBox.cy);
					nXPos += (m_sizeCheckBox.cx + 3);
				}

				// 画行左边图片
				int nImgY = 3;
				if(rowInfo.pImage != NULL)
				{
					if((rowInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
					}
					// 使用行数据指定的图片
					graphics.DrawImage(rowInfo.pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy),
						0, 0, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy, UnitPixel);
					nXPos += (rowInfo.sizeImage.cx + 3);
				}else
					if((rowInfo.nImageIndex != -1) && (m_pImage != NULL))
					{
						if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
						}
						// 使用索引图片
						graphics.DrawImage(m_pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
							rowInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
						nXPos += (m_sizeImage.cx + 3);
					}

					// 画行右边图片
					int nRightImageWidth = 0;
					nImgY = 3;
					if(rowInfo.pRightImage != NULL)
					{
						if((rowInfo.sizeRightImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - rowInfo.sizeRightImage.cy) / 2 + 1;
						}
						// 使用行数据指定的图片
						graphics.DrawImage(rowInfo.pRightImage, Rect(nContentWidth-rowInfo.sizeRightImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy),
							0, 0, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy, UnitPixel);
						nRightImageWidth = rowInfo.sizeRightImage.cx + 1;
					}else
						if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL))
						{
							if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
							{
								nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
							}
							// 使用索引图片
							graphics.DrawImage(m_pImage, Rect(nContentWidth-m_sizeImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
								rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
							nRightImageWidth = m_sizeImage.cx + 1;
						}

						// 画单元格内容
						int nPosItemX = (m_nLeftPos != 0) ? m_nLeftPos : nXPos;
						for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
						{
							GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
							GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
							BOOL bSingleLine = (itemInfo.strContent.IsEmpty() || !itemInfo.strLink.IsEmpty());
							RectF rect((Gdiplus::REAL)nPosItemX,
								(Gdiplus::REAL)(m_nHeaderHeight + nVI*m_nRowHeight + 1),
								(Gdiplus::REAL)((j == 0) ? (itemInfo.rcItem.Width() - nPosItemX): itemInfo.rcItem.Width()),
								(Gdiplus::REAL)(bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2)));
							if((int)(rect.GetRight()) > nContentWidth)
							{
								// 最后一列需要减去滚动条宽度
								rect.Width -= m_nScrollWidth;
							}

							// 画单元格图片
							int nItemImageX = 0;
							int nImgY = 3;
							if(itemInfo.pImage != NULL)
							{
								if((itemInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
								{
									nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
								}
								// 使用单元格指定的图片
								graphics.DrawImage(itemInfo.pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
									0, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
								nItemImageX += (itemInfo.sizeImage.cx + 3);
							}else
								if((itemInfo.nImageIndex != -1) && (m_pImage != NULL))
								{
									if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
									{
										nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
									}
									// 使用索引图片
									graphics.DrawImage(m_pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
										itemInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
									nItemImageX += (m_sizeImage.cx + 3);
								}
								rect.Offset((Gdiplus::REAL)nItemImageX, 0);
								rect.Width -= (Gdiplus::REAL)nItemImageX;

								// 画单元格标题或链接内容
								SolidBrush solidBrushItem(m_clrText);
								if((m_nHoverRow == i) && (m_clrTextHover.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 设置了鼠标移动颜色,则使用
								{
									solidBrushItem.SetColor(m_clrTextHover);
								}else
									if((m_nDownRow == i) && (m_clrTextDown.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 设置了鼠标按下颜色,则使用
									{
										solidBrushItem.SetColor(m_clrTextDown);
									}else
										if(itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue())	// 设置了单元格颜色,则使用
										{
											solidBrushItem.SetColor(itemInfo.clrText);
										}else
											if(rowInfo.bRowColor)	// 设置了行颜色,则使用
											{
												solidBrushItem.SetColor(rowInfo.clrText);
											}
											CString strItemTitle = itemInfo.strTitle;
											CString strTempItemstrConten=itemInfo.strContent;
											// 计算是否需要显示tip
											itemInfo.bNeedTitleTip = rect.Width < GetTextBounds(font, strItemTitle).Width;
											itemInfo.bNeedContentTip = rect.Width < GetTextBounds(font, strTempItemstrConten).Width;
											if(!itemInfo.strLink.IsEmpty())
											{
												strItemTitle = itemInfo.strLink;
												if((m_nHoverRow == i) && (rowInfo.nHoverItem == j))
												{
													solidBrushItem.SetColor(m_clrTextHover);
												}else
												{
													solidBrushItem.SetColor((itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue()) ? itemInfo.clrText : m_clrText);
												}
											}

											// 设置单元格文字对齐方式,使用列的对齐方式
											StringFormat strFormatColumn;
											UINT uAlignment = m_uAlignment;
											if(columnInfo.uAlignment != 0xFFFFUL)
											{
												uAlignment = columnInfo.uAlignment;
											}
											if(uAlignment == Align_Left)
											{
												strFormatColumn.SetAlignment(StringAlignmentNear);
											}else
												if(uAlignment == Align_Center)
												{
													strFormatColumn.SetAlignment(StringAlignmentCenter);
												}else
													if(uAlignment == Align_Right)
													{
														strFormatColumn.SetAlignment(StringAlignmentFar);
													}
													UINT uVAlignment = m_uVAlignment;
													if(columnInfo.uVAlignment != 0xFFFFUL)
													{
														uVAlignment = columnInfo.uVAlignment;
													}
													if(uVAlignment == VAlign_Top)
													{
														strFormatColumn.SetLineAlignment(StringAlignmentNear);
													}else
														if(uVAlignment == VAlign_Middle)
														{
															strFormatColumn.SetLineAlignment(StringAlignmentCenter);
														}else
															if(uVAlignment == VAlign_Bottom)
															{
																strFormatColumn.SetLineAlignment(StringAlignmentFar);
															}
															if(!m_bTextWrap)
															{
																strFormatColumn.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// 不换行
															}

															// 根据bUseTitleFont决定用标题字体还是普通字体
															BSTR bsItemTitle = strItemTitle.AllocSysString();
															if (rect.Width!=0)
																graphics.DrawString(bsItemTitle, (INT)wcslen(bsItemTitle),
																itemInfo.bUseTitleFont ? &fontTitle : &font, rect, &strFormatColumn, itemInfo.bUseTitleFont ? &solidBrushT : &solidBrushItem);
															::SysFreeString(bsItemTitle);

															// 画单元格内容
															if(!bSingleLine)
															{
																rect.Offset(0, (Gdiplus::REAL)m_nRowHeight / 2 + 2);
																rect.Height = (Gdiplus::REAL)m_nRowHeight / 2 - 4;
																BSTR bsItemContent = itemInfo.strContent.AllocSysString();
																graphics.DrawString(bsItemContent, (INT)wcslen(bsItemContent), &font, rect, &strFormatColumn, &solidBrushItem);
																::SysFreeString(bsItemContent);
															}

															// 设置单元格子控件的位置
															for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
															{
																CControlBase* pControl = itemInfo.vecControl.at(k);
																if(pControl)
																{
																	CRect rcParent = CRect(nPosItemX, m_nHeaderHeight + nVI*m_nRowHeight + 1,
																		(int)(rect.X+rect.Width), (nVI+1)*m_nRowHeight - 1);
																	if((int)(rect.GetRight()) > nContentWidth)
																	{
																		// 最后一列需要减去滚动条宽度
																		rcParent.right -= m_nScrollWidth;
																	}
																	rcParent.OffsetRect(m_rc.left - m_nVirtualLeft, m_rc.top - (nYViewPos + m_nHeaderHeight));
																	pControl->SetPositionWithParent(rcParent);
																	CRect rcControl = pControl->GetRect();
																	// 只有当前在显示范围内的控件设置为可见
																	if( (rcControl.top < m_rc.top) || (rcControl.bottom > m_rc.bottom) ||
																		(rcControl.left < m_rc.left) || (rcControl.right > m_rc.right) )
																	{
																		pControl->SetVisible(FALSE);
																	}else
																	{
																		pControl->SetVisible(TRUE);
																	}
																}
															}

															if(j == 0)
															{
																// 为了使第二列开始是对齐的,所以第二列开始位置按照第一列的宽度计算
																nPosItemX = itemInfo.rcItem.right;
															}else
															{
																nPosItemX += itemInfo.rcItem.Width();
															}
						}

						// 画分隔线(采用拉伸模式)
						if(m_pImageSeperator != NULL)
						{
							// 使用拉伸模式画图
							graphics.DrawImage(m_pImageSeperator, RectF(0, (Gdiplus::REAL)(m_nHeaderHeight + (nVI+1)*m_nRowHeight), (Gdiplus::REAL)(nContentWidth-2), (Gdiplus::REAL)m_sizeSeperator.cy),
								0, 0, (Gdiplus::REAL)m_sizeSeperator.cx, (Gdiplus::REAL)m_sizeSeperator.cy, UnitPixel);
						}else
							if(m_clrSeperator.GetValue() != Color(0, 0, 0, 0).GetValue())
							{
								// 未指定图片,并且分隔线显色不是全0,则画矩形
								graphics.FillRectangle(&solidBrushS, 0, m_nHeaderHeight + (nVI+1)*m_nRowHeight, nContentWidth-2, 1);
							}
			}

			// 把不在显示范围内的单元格的控件都设置为不可见
			for(int i = 0; i < (int)m_vecRowInfo.size(); i++)
			{
				if((i < m_nFirstViewRow) || (i > m_nLastViewRow))
				{
					GridRowInfo &rowInfo = m_vecRowInfo.at(i);
					for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
					{
						GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
						for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
						{
							CControlBase* pControl = itemInfo.vecControl.at(k);
							if(pControl)
							{
								pControl->SetVisible(FALSE);
							}
						}
					}
				}
			}
			LeaveCriticalSection(&m_cs);
		}
	}

	// 输出到界面DC,使用与的方式合并背景
	// 标题行输出
	if(m_nHeaderHeight > 0)
	{
		dc.BitBlt(m_rc.left,m_rc.top, nViewWidth, m_nHeaderHeight, &m_memDC, m_nVirtualLeft, 0, SRCCOPY);//SRCAND);
	}
	// 内容部分输出
	int nContentHeight = m_rc.Height() - m_nHeaderHeight;
	if(nTotalColumnWidth > m_rc.Width())
	{
		nContentHeight -= m_nScrollWidth;
	}
	dc.BitBlt(m_rc.left,m_rc.top + m_nHeaderHeight, nViewWidth, nContentHeight, &m_memDC, m_nVirtualLeft, nYViewPos + m_nHeaderHeight, SRCCOPY);//SRCAND);
}

CString CDuiGridCtrl::GetItemTitle(int nRow, int nItem)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return NULL;
	}

	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	if(nItem >= (int)rowInfo.vecItemInfo.size())
	{
		return NULL;
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	return itemInfo.strTitle;
}
