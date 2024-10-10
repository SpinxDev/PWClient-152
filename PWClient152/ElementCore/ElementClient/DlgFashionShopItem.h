// Filename	: DlgFashionShopItem.h
// Creator	: Xu Wenbin
// Date		: 2014/5/23

#ifndef _ELEMENTCLIENT_DLGFASHIONSHOPITEM_H_
#define _ELEMENTCLIENT_DLGFASHIONSHOPITEM_H_

#include "DlgBase.h"

class AUIStillImageButton;
class AUIImagePicture;

class CECShopBase;
class CECFashionShop;
class CECShoppingCart;

struct _GSHOP_ITEM;
typedef struct _GSHOP_ITEM GSHOP_ITEM;

class CDlgFashionShop;

class CDlgFashionShopItem : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgFashionShopItem();

	void OnCommand_Time(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_GivePresent(const char *szCommand);
	void OnCommand_AskForPresent(const char *szCommand);
	void OnCommand_Buy(const char * szCommand);
	void OnCommand_AddToShopCart(const char * szCommand);

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	virtual void OnChangeLayoutEnd(bool bAllDone);

	void SetItem(PAUIOBJECT positionController, const CECFashionShop* pFashionShop, int fashionSelectionIndex, CECShoppingCart *pShoppingCart);
	void ClearItem();

	int  FashionShopSelectionIndex();
	void SetSelected(bool bSelected, AUIImagePicture *pImg_Normal, AUIImagePicture *pImg_Hightlight);

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual bool Render();

	bool	  CanBuy();
	bool	  CanBuyDirectly();
	bool      CanGivePresent();
	bool      CanAskForPresent();
	bool      CanGivingFor();

	PAUIOBJECT				m_pTxt_ItemName;
	PAUIOBJECT				m_pTxt_Price;
	PAUIOBJECT				m_pTxt_NoDiscountPrice;
	AUIStillImageButton *	m_pBtn_Buy;
	AUIStillImageButton	*	m_pBtn_AddToShopCart;

	enum {TIME_TYPE_COUNT = 3};
	AUIStillImageButton *	m_pBtn_Time[TIME_TYPE_COUNT];
	AUIImagePicture *		m_pImg_Item;
	AUIImagePicture *       m_pImg_Bg01;			// ���۵ײ�ͼ��
	AUIImagePicture *		m_pImg_Bg02;			// �����ۿ�
	AUIImagePicture *       m_pImg_Bg03;			// ��Ʒ
	AUIImagePicture *       m_pImg_Bg04;			// ����
	AUIStillImageButton *	m_pBtn_GivePresent;
	AUIStillImageButton *	m_pBtn_AskForPresent;

	PAUIOBJECT				m_pPositionController;	//	���ݴ˿ؼ�������CDlgFashionShop��ĳ��ռλ�ؼ�����λ����ʾ��ǰ�Ի���
	const CECFashionShop*	m_pFashionShop;			//	����ʱװ�̳ǣ�����Դ��Ǭ�����������̳ǵȣ�
	int						m_fashionSelectionIndex;//	������Ʒ m_pFashionShop->ItemAt(m_fashionSelectionIndex)
	CECShoppingCart	*		m_pShoppingCart;		//	�ռ���Ʒ�Ĺ��ﳵ

	int						m_nTimeSelect;
	enum {BUY_TYPE_COUNT = 4};
	int						m_BuyType[BUY_TYPE_COUNT];

private:
	int  GetBuyIndex()const;
	int	 CalculatePrice()const;
	bool HasDiscount()const;
	unsigned int GetUniformStatus()const;
	bool HasGift()const;
	bool HasFlashSale()const;
	ACString CalculateItemHint()const;
	bool IsFashionSuite()const;

	void SetItemHint(const ACHAR *szHint);
	void SetItemIcon(const char *icon);
	void SetItemName(const ACHAR *szName);
	void UpdatePrice();
	void SortBuyType(int buyType[BUY_TYPE_COUNT], const CECShopBase *pShop, int itemIndex);
	void InitTimeSelection(const GSHOP_ITEM *pGShopItem);
	void HideSingleTimeSelection(const GSHOP_ITEM *pGShopItem);
	int  GetFirstBuyTypeIndex()const;
	int  GetBuyTypeCount()const;
	void OnTimeSelectChange();

	CDlgFashionShop * FashionShopDialog();
};

#endif	//	_ELEMENTCLIENT_DLGFASHIONSHOPITEM_H_