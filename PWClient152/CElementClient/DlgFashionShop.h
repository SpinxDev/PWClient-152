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

//	ǰ������
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

	//	�¼���Ӧ
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

	//	������ CECFashionShopObserver
	virtual void OnRegistered(const CECFashionShop *p);
	virtual void OnModelChange(const CECFashionShop *p, const CECObservableChange *q);
	virtual void OnUnregister(const CECFashionShop *p);
	
	//	������ CECShoppingCartObserver
	virtual void OnRegistered(const CECShoppingCart *p);
	virtual void OnModelChange(const CECShoppingCart *p, const CECObservableChange *q);
	virtual void OnUnregister(const CECShoppingCart *p);

	//	���¼�ӿڣ��� CDlgFashionShopItem ���ã�
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
	CECFashionShop*		m_pFashionShop;			//	ʱװ�̳�����ģ��
	CECShoppingCart*	m_pShoppingCart;		//	���ﳵ����ģ��

	PAUIOBJECT			m_pLbl_title;			//	��ʾ��ǰʱװ��Ǭ����ʱװ���Ǻ���ʱװ

	AUIStillImageButton *	m_pBtn_New;			//	��������ؼ�
	AUIStillImageButton *	m_pBtn_Hot;
	AUIStillImageButton *	m_pBtn_Sale;
	AUIStillImageButton *	m_pBtn_FlashSale;

	AUIStillImageButton *	m_pBtn_Suite;		//	ʱװ����ؼ�
	AUIStillImageButton *	m_pBtn_Head;
	AUIStillImageButton *	m_pBtn_UpperBody;
	AUIStillImageButton *	m_pBtn_LowerBody;
	AUIStillImageButton *	m_pBtn_WaistNShoes;
	AUIStillImageButton *	m_pBtn_Weapon;

	PAUIOBJECT			m_pBtn_Male;			//	��ְҵʱ��ʾ�İ�ť�������л���Ůְҵ
	PAUIOBJECT			m_pBtn_Female;			//	Ůְҵʱ��ʾ�İ�ť�������л�����ְҵ

	AUIImagePicture *	m_pImage_Prof;			//	ְҵѡ��ؼ�
	AUIImagePicture *	m_pImg_SelProf[NUM_PROFESSION];

	PAUIOBJECT			m_pTxt_Cash;			//	��ǰԪ����
	PAUIOBJECT			m_pBtn_ShowCart;		//	ȥ���ﳵ

	AUIScroll	*		m_pScl_Item;			//	�������ؼ�
	int					m_nCurrentBarLevel;		//	

	AUIImagePicture	*	m_pImg_Ad;				//	���ͼƬ�ؼ�
	PAUIOBJECT			m_pBtn_PrevAd;			//	��ǰһҳ
	PAUIOBJECT			m_pBtn_NextAd;			//	����һҳ
	typedef abase::vector<AUIRadioButton *>	AdPageButtons;
	AdPageButtons		m_AdPageButtons;		//	ֱ�ﰴť
	int					m_AdCount;				//	�������
	int					m_currentAd;			//	��ǰ��ʾ�Ĺ���±�
	A2DSprite	*		m_pCurrentAdSprite;		//	��ǰ��ʾ�Ĺ��ͼƬ
	CECTimeSafeChecker	m_adTimer;				//	��涯����ʱ��

	PAUIOBJECT			m_pBtn_BuyWearing;		//	��ӵ�ǰ���ϴ��ŵ����ﳵ�ؼ�
	AUIImagePicture	*	m_pImg_Char;			//	���� Player ��ʾ�ؼ�
	PAUIOBJECT			m_pBtn_Palette;			//	��ɫ��
	PAUIOBJECT			m_pBtn_ResetPlayer;		//	��ɫ����
	CECFashionModel	*	m_pFashionModel;		//	���¼�ģ��

	struct WearingItem	//	ģ�����ϴ�����Ʒ���̳��е�λ��
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

	AUIImagePicture	*	m_pImg_Normal;			//	��Ʒδѡ��ʱ����ͼ�ο��ؼ�
	AUIImagePicture	*	m_pImg_Highlight;		//	��Ʒѡ��ʱ����ͼ�ο��ؼ�
	int					m_fashionSelectionIndex;//	��ǰѡ�е�ʱװ�̳���Ʒ�±� m_pFashionShop->ItemAt(m_fashionSelectionIndex)

	bool				m_inRelease;			//	��ǰ�����ͷ���
};

#endif	//	_ELEMENTCLIENT_DLGFASHIONSHOP_H_