// Filename	: EC_FashionShop.h
// Creator	: Xu Wenbin
// Date		: 2014/5/22

#ifndef _ELEMENTCLIENT_EC_FASHIONSHOP_H_
#define _ELEMENTCLIENT_EC_FASHIONSHOP_H_

#include "EC_Observer.h"
#include <vector.h>
#include "ExpTypes.h"

//	class CECFashionShopChange
class CECFashionShopChange : public CECObservableChange
{
public:
	enum ChangeMask{
		SALE_TYPE_CHANGED		=	0x01,	//	售卖分类发生变化
		FASHION_TYPE_CHANGED	=	0x02,	//	时装类型发生变化
		PROFESSION_CHANGED		=	0x04,	//	职业发生变化
		GENDER_CHANGED			=	0x08,	//	性别发生变化
		MALL_ITEMS_CHANGED		=	0x10,	//	商城数据发生变化
	};
private:
	unsigned int m_changeMask;
public:
	CECFashionShopChange(unsigned int changeMask);

	unsigned int GetChangeMask()const;

	bool SaleTypeChanged()const;
	bool FashionTypeChanged()const;
	bool ProfessionChanged()const;
	bool GenderChanged()const;

	bool MallItemsChanged()const;
};

//	定义 CECFactionPVPModel 的观察者
class CECFashionShop;
typedef CECObserver<CECFashionShop>	CECFashionShopObserver;

//	class CECFashionShop
class CECShopBase;
typedef CECObserver<CECShopBase> CECShopBaseObserver;
class CECFashionShop : public CECObservable<CECFashionShop>, public CECShopBaseObserver
{
public:
	enum enumSaleType{
		ST_NEW_ARRIVE,		//	新装上市
		ST_HOT_SALE,		//	经典热卖
		ST_ON_SALE,			//	超值促销
		ST_FLASH_SALE,		//	闪购
		ST_COUNT,
	};
	enum enumFashionType{
		FT_SUITE,			//	套装
		FT_HEAD,			//	帽子
		FT_UPPER_BODY,		//	上衣
		FT_LOWER_BODY,		//	下装
		FT_WAIST_N_SHOES,	//	配件（手套、鞋子等）
		FT_WEAPON,			//	时装武器
		FT_COUNT,
	};

	struct FashionSelection{
		union{
			int	itemIndex;
			struct{
				int	itemIndices[FASHION_SUITE_NUM];		//	将 itemIndices 放在前，以与 itemIndex 保持含义一致
				const FASHION_SUITE_ESSENCE *pEssence;	//	时装套装 essence，便于后期使用
			}suite;
		};
		FashionSelection(){
			itemIndex = -1;
			for (int i(0); i < FASHION_SUITE_NUM; ++ i){
				suite.itemIndices[i] = -1;
			}
			suite.pEssence = 0;
		}
		FashionSelection(int gshopItemIndex){
			for (int i(0); i < FASHION_SUITE_NUM; ++ i){
				suite.itemIndices[i] = -1;
			}
			suite.pEssence = 0;
			this->itemIndex = gshopItemIndex;
		}
		int FirstSuiteItemIndex()const{
			for (int i(0); i < FASHION_SUITE_NUM; ++ i){
				int itemIndex = suite.itemIndices[i];
				if (itemIndex >= 0){
					return itemIndex;
				}
			}
			return -1;
		}
	};

	class CECSelectedFashionLooper{
	public:
		virtual ~CECSelectedFashionLooper(){}
		virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
			return true;
		}
		virtual void VisitNonSuiteItem(const CECShopBase *pShop, int itemIndex, int buyIndex){
			LoopSuiteItemAndContinue(pShop, itemIndex, buyIndex);	//	默认共用 LoopSuiteItemAndContinue 代码，很多时候不需要区分
		}
	};

private:
	void Reselect();
	void SelectSuite();
	void SortSuite();

	CECFashionShop(const CECFashionShop &);
	CECFashionShop & operator = (const CECFashionShop &);

public:
	CECFashionShop(CECShopBase *pShop, enumSaleType newSaleType, enumFashionType newFashionType, int newProfession, int newGender);
	~CECFashionShop();

	//	派生自 CECShopBaseObserver
	virtual void OnModelChange(const CECShopBase *p, const CECObservableChange *q);

	CECShopBase * Shop()const;

	enumSaleType	SaleType()const;
	enumFashionType FashionType()const;
	bool	IsFashionTypeSuite()const;
	int		Profession()const;
	int		Gender()const;

	void	ChangeSaleType(enumSaleType newSaleType);
	void	ChangeFashionType(enumFashionType newFashionType);
	void	ChangeProfession(int newProfession);
	void	ChangeGender(int newGender);
	void	ChangeProfessionGender(int newProfession, int newGender);

	void	ChangeAll(enumSaleType newSaleType, enumFashionType newFashionType, int newProfession, int newGender);
	
	int		Count()const;
	const FashionSelection*	ItemAt(int index)const;

	void	LoopSelectedFashion(int fashionIndex, int buyIndex, CECSelectedFashionLooper *looper)const;

private:
	CECShopBase	*		m_pShop;

	enumSaleType		m_saleType;			//	售卖分类
	enumFashionType		m_fashionType;		//	时装类型
	int					m_profession;		//	选中职业
	int					m_gender;			//	选中性别

	typedef	abase::vector<FashionSelection>	SelectedFashion;
	SelectedFashion		m_selectedFashion;	//	通过多种组合条件查询得到的时装列表
};

class CECFashionShopManager
{
public:
	enum{
		QSHOP_FASHION_SHOP,
		BACKSHOP_FASHION_SHOP,
		FASHION_SHOP_COUNT,
	};

private:
	CECFashionShop	*	m_pFashionShop[FASHION_SHOP_COUNT];

	CECFashionShopManager();
	
	//	禁用
	CECFashionShopManager(const CECFashionShopManager &);
	CECFashionShopManager & operator = (const CECFashionShopManager &);

public:
	~CECFashionShopManager();
	static CECFashionShopManager & Instance();
	
	void Initialize(CECFashionShop::enumSaleType newSaleType, CECFashionShop::enumFashionType newFashionType, int newProfession, int newGender);
	void Clear();

	CECFashionShop	*	FashionShopAt(int index);
	CECFashionShop	*	QShopFashionShop();
	CECFashionShop	*	BackShopFashionShop();
};

#endif	//	_ELEMENTCLIENT_EC_FASHIONSHOP_H_