#pragma once

#include "DlgCustomizeBase.h"
#include "AUI_ImageGrid.h"

class AUIListBox;

class CDlgCustomizeHair : public CDlgCustomizeBase  
{
	AUI_DECLARE_EVENT_MAP()
protected:
	// ddxx control
	AUI_ImageGrid m_ListBoxHair;//头发列表框
	AUI_ImageGrid m_ListBoxHairTex;//头发纹理列表框
	AUI_ImageGrid m_ListBoxMoustache;//小胡子列表控件
	AUI_ImageGrid m_ListBoxGoatee;//大胡子的列表控件
	AUI_ImageGrid m_ListBoxGoateeTex;//大胡子纹理列表框

protected:
	bool InitResource();
	
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual AUIStillImageButton * GetSwithButton();
	virtual void OnShowDialog();
	virtual void OnTick();
	void ClearListCtrlAndA2DSprite();

	bool ChangeHairTexOnHairModelChange();

	bool CanCustomizeHair();
	bool CanCustomizeHairTex();
	bool CanCustomizeGoatee();
	bool CanCustomizeGoateeTex();
	bool CanCustomizeMoustache();

	typedef abase::vector<A2DSprite*>::iterator VecA2DSpriteIter_t;
	abase::vector<A2DSprite*> m_vecA2DSprite;

	
public:
	void OnLButtonUpListboxHair(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnMouseWheelListboxHair(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	CDlgCustomizeHair();
	virtual ~CDlgCustomizeHair();

};

