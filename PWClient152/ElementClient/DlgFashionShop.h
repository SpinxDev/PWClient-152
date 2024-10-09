// Filename	: DlgFashionShop.h
// Creator	: Xu Wenbin
// Date		: 2014/5/22

#ifndef _ELEMENTCLIENT_DLGFASHIONSHOP_H_
#define _ELEMENTCLIENT_DLGFASHIONSHOP_H_

#include "DlgBase.h"
#include "EC_Observer.h"
#include "EC_RoleTypes.h"
#include "EC_IvtrTypes.h"
#include "EC_TimeSafeChecker.h"

#include <vector.h>

//	前置声明
class CECFashionShop;
typedef CECObserver<CECFashionShop>	 CECFashionShopObserver;
class CECFashionShopChange;

class CECShoppingCart;
typedef CECObserver<CECShoppingCart> CECShoppingCartObserver;
class CECShoppingCartChange;

class AUIComboBox;
class AUIImagePicture;
class AUIStillImageButton;
class AUIScroll;
class AUIRadioButton;
class A2DSprite;

class CECFashionModel;
class CECShopBase;

//	class CDlgFashionShop
class CDlgFashionShop : public CDlgBase, public CECFashionShopObserver, public CECShoppingCartObserver
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFashionShop();

	bool SetModel(CECShoppingCart *pShoppingCart, CECFashionShop *pFashionShop);
	const CECFashionShop * GetFashionShop()const;

	void ShowFalse();

	//	事件响应
	void OnCommand_CANCEL(const char *szCommand);

	void OnCommand_New(const char * szCommand);
	void OnCommand_Hot(const char * szCommand);
	void OnCommand_Sale(const char * szCommand);
	void OnCommand_FlashSale(const char *szCommand);

	void OnCommand_Suite(const char * szCommand);
	void OnCommand_Head(const char * szCommand);
	void OnCommand_UpperBody(const char * szCommand);
	void OnCommand_LowerBody(const char * szCommand);
	void OnCommand_WaistNShoes(const char * szCommand);
	void OnCommand_Weapon(const char * szCommand);

	void OnCommand_Male(const char * szCommand);
	void OnCommand_Female(const char * szCommand);

	void OnCommand_BuyWearing(const char *szCommand);
	void OnCommand_ShowCart(const char * szCommand);
	void OnCommand_Palette(const char * szCommand);
	void OnCommand_ResetPlayer(const char * szCommand);

	void OnCommand_PrevAd(const char * szCommand);
	void OnCommand_NextAd(const char * szCommand);
	void OnCommand_Rdo_Page(const char * szCommand);

	void OnEventLButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseWheel_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Prof(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_SelectProf(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	//	派生自 CECFashionShopObserver
	virtual void OnRegistered(const CECFashionShop *p);
	virtual void OnModelChange(const CECFashionShop *p, const CECObservableChange *q);
	virtual void OnUnregister(const CECFashionShop *p);
	
	//	派生自 CECShoppingCartObserver
	virtual void OnRegistered(const CECShoppingCart *p);
	virtual void OnModelChange(const CECShoppingCart *p, const CECObservableChange *q);
	virtual void OnUnregister(const CECShoppingCart *p);

	//	试衣间接口（供 CDlgFashionShopItem 调用）
	void Fit(int fashionShopSelectionIndex, int buyIndex);
	void ChangeFashionColor(int equipSlot, unsigned short newColor);
	bool GetFashionColor(int equipSlot, unsigned short &color);
	bool GetFashionBestColor(int equipSlot, unsigned short &color);

	void SelectFashionShopItem(int index);
	int  SelectedFashionShopItem();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool Release();
	virtual void OnChangeLayoutEnd(bool bAllDone);	
	
	bool SetShoppingCart(CECShoppingCart *pShoppingCart);
	bool SetFashionShop(CECFashionShop *pFashionShop);

	bool IsInconsistent(CECFashionShop *pFashionShop, CECShoppingCart *pShoppingCart);
	void SelectProfession(bool bSelect);
	bool IsSelectingProfession();

	void UpdateSaleType();
	void UpdateFashionType();
	void UpdateProfession();
	void UpdateGender();
	void UpdateItems();
	void UpdateFashionShopName();
	void UpdatePlayer();

	void UpdateShoppingCart();
	void UpdateCash();
	void UpdateScrollPosition(bool bForceUpdate=false);
	void UpdateFashionSelection();
	void UpdateBuyWearing();
	void UpdatePalette();
	void UpdateResetPlayer();

	AString GetAdImagePath(int index);
	void ShowAd(int index);
	void UpdateAdButtons();
	bool HasNextAd();
	bool HasPrevAd();

	bool HasPlayer();
	void CreatePlayer(int profession, int gender);
	void ShowPalette(bool bShow);
	bool IsPaletteShown();
	void ResetFashion();
	void ClearWearingItems();
	void UpdateWearingItems(bool bTryWearLastFashion);
	
private:
	CECFashionShop*		m_pFashionShop;			//	时装商城数据模型
	CECShoppingCart*	m_pShoppingCart;		//	购物车数据模型

	PAUIOBJECT			m_pLbl_title;			//	显示当前时装是乾坤袋时装还是鸿利时装

	AUIStillImageButton *	m_pBtn_New;			//	售卖分类控件
	AUIStillImageButton *	m_pBtn_Hot;
	AUIStillImageButton *	m_pBtn_Sale;
	AUIStillImageButton *	m_pBtn_FlashSale;

	AUIStillImageButton *	m_pBtn_Suite;		//	时装分类控件
	AUIStillImageButton *	m_pBtn_Head;
	AUIStillImageButton *	m_pBtn_UpperBody;
	AUIStillImageButton *	m_pBtn_LowerBody;
	AUIStillImageButton *	m_pBtn_WaistNShoes;
	AUIStillImageButton *	m_pBtn_Weapon;

	PAUIOBJECT			m_pBtn_Male;			//	男职业时显示的按钮，用于切换到女职业
	PAUIOBJECT			m_pBtn_Female;			//	女职业时显示的按钮，用于切换到男职业

	AUIImagePicture *	m_pImage_Prof;			//	职业选择控件
	AUIImagePicture *	m_pImg_SelProf[NUM_PROFESSION];

	PAUIOBJECT			m_pTxt_Cash;			//	当前元宝数
	PAUIOBJECT			m_pBtn_ShowCart;		//	去购物车

	AUIScroll	*		m_pScl_Item;			//	滚动条控件
	int					m_nCurrentBarLevel;		//	

	AUIImagePicture	*	m_pImg_Ad;				//	广告图片控件
	PAUIOBJECT			m_pBtn_PrevAd;			//	翻前一页
	PAUIOBJECT			m_pBtn_NextAd;			//	翻下一页
	typedef abase::vector<AUIRadioButton *>	AdPageButtons;
	AdPageButtons		m_AdPageButtons;		//	直达按钮
	int					m_AdCount;				//	广告总数
	int					m_currentAd;			//	当前显示的广告下标
	A2DSprite	*		m_pCurrentAdSprite;		//	当前显示的广告图片
	CECTimeSafeChecker	m_adTimer;				//	广告动画计时器

	PAUIOBJECT			m_pBtn_BuyWearing;		//	添加当前身上穿着到购物车控件
	AUIImagePicture	*	m_pImg_Char;			//	试衣 Player 显示控件
	PAUIOBJECT			m_pBtn_Palette;			//	调色板
	PAUIOBJECT			m_pBtn_ResetPlayer;		//	角色重置
	CECFashionModel	*	m_pFashionModel;		//	试衣间模特

	struct WearingItem	//	模特身上穿的物品在商城中的位置
	{
		int				gshopItemIndex;
		int				gshopBuyIndex;
		WearingItem(): gshopItemIndex(-1), gshopBuyIndex(-1){}
		void Clear(){
			Set(-1, -1);
		}
		void Set(int itemIndex, int buyIndex){
			gshopItemIndex = itemIndex;
			gshopBuyIndex = buyIndex;
		}
		bool Valid(const CECShopBase *pShop)const;
	}m_wearingItems[SIZE_ALL_EQUIPIVTR];

	AUIImagePicture	*	m_pImg_Normal;			//	物品未选中时背景图参考控件
	AUIImagePicture	*	m_pImg_Highlight;		//	物品选中时背景图参考控件
	int					m_fashionSelectionIndex;//	当前选中的时装商城物品下标 m_pFashionShop->ItemAt(m_fashionSelectionIndex)

	bool				m_inRelease;			//	当前正在释放中
};

#endif	//	_ELEMENTCLIENT_DLGFASHIONSHOP_H_