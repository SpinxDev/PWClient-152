// Filename	: AUI_ImageGrid.h
// Creator	: Xu Wenbin
// Date		: 2010/5/19

#pragma once

#include <A2DSprite.h>
#include <AUIDialog.h>
#include <AUIImagePicture.h>
#include <AUIScroll.h>
#include <vector.h>

//
//	AUI_ImageGrid: 以少数均匀排列的 AUIImagePicture，显示个数不限制的 AUIImagePicture
//
class AUI_ImageGrid
{
public:
	AUI_ImageGrid();
	virtual ~AUI_ImageGrid();

	bool IsReady()const;

	void ResetContent();
	bool Init(PAUIDIALOG pDlgParent, const char *pszFirstName, int nMaxRowVisible, int nMaxColVisible, const char *pszScrollName, const char *pszBackground, A2DSprite *pSpriteSelected, A2DSprite *pSpriteMouseOn);
	void Append(DWORD dwData, void *pvData, const ACHAR *pszHint, A2DSprite *pA2DSprite, bool bUpdateView = false);

	int GetMaxRow()const{ return m_nMaxRowVisible; }
	int GetMaxCol()const{ return m_nMaxColVisible; }
	int GetFirstIndex()const{ return m_nFirstIndex; }
	AString GetScrollName()const{ return m_pScroll ? m_pScroll->GetName() : NULL; }
	PAUISCROLL GetScroll()const{ return m_pScroll; }
	PAUIOBJECT GetBkground()const{ return m_pBkground; }
	PAUIDIALOG GetDialog()const{ return m_pDlgOwner; }

	int GetCount()const{ return (int)m_data.size(); }

	PAUIOBJECT OnMouse_Clicked(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	bool OnMouse_Wheel(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	bool CanAcceptMouseWheel(PAUIOBJECT pObj);
	bool IsMouseOn(PAUIOBJECT pObj);
	bool IsSelected(PAUIOBJECT pObj);
	void OnTick();
	
	void Show(bool bShow);
	bool IsShown()const{ return m_bShow; }

	// Reset the images binding to this grid.
	// Set bRelease = true will cause the real release operation.
	void ResetImages(bool bRelease);

private:

	AString MakeName(int nRealRow, int nRealCol)const;
	int GetFirstOffset()const { return m_nFirstOffset; }
	PAUIIMAGEPICTURE GetObject(int nRealRow, int nRealCol);
	PAUIIMAGEPICTURE GetVirtualObject(int nVirtualRow, int nVirtualCol);
	bool GetPrefixIndex(const char *pszName, AString &strPrefix, int &nIndex)const;
	void UpdateView(int m_nFirstOffset);
	int GetBarLevelFromOffset(int nOffset)const;
	int GetOffsetFromBarLevel(int nBarLevel)const;

	struct GridItemData
	{
		DWORD dwData;
		void * pvData;
		ACString strHint;
		A2DSprite *pA2DSprite;
	};
	const GridItemData *GetItemData(int nVirtualRow, int nVirtualCol)const;

	static void GridImageRenderCallback(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);

private:
	
	PAUIDIALOG m_pDlgOwner;	//	控件的父对话框
	AString m_strPrefix;	//	控件的名称前缀，如 Img_1 的前缀是 Img_
	int  m_nFirstIndex;		//	控件的起始下标，如管理的同类控件的范围是 Img_5~Img_15，则返回值是 5
	int  m_nMaxRowVisible;	//	最大可见行，大于此行以后，将使用滚动条切换
	int  m_nMaxColVisible;	//	最大可见列
	PAUIOBJECT m_pBkground;	//	背景控件
	PAUISCROLL m_pScroll;	//	滚动条，用于不同页切换
	int  m_nFirstOffset;	//	当前第一个可见的控件的 index

	typedef abase::vector<PAUIIMAGEPICTURE> ObjectArray;	//	控件列表
	ObjectArray m_objs;

	abase::vector<GridItemData> m_data;	//	所有可见、不可见控件所需数据列表

	bool m_bShow;				//	是否可见

	A2DSprite *	m_pSpriteSelected;	//	鼠标选中时显示的图片
	A2DSprite *	m_pSpriteMouseOn;	//	鼠标经过时显示的图片
	int  m_nSelectedVirtualRow;	//	当前选中项逻辑坐标
	int  m_nSelectedVirtualCol;	//	
};
