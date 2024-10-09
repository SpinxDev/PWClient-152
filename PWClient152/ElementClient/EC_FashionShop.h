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
		SALE_TYPE_CHANGED		=	0x01,	//	�������෢���仯
		FASHION_TYPE_CHANGED	=	0x02,	//	ʱװ���ͷ����仯
		PROFESSION_CHANGED		=	0x04,	//	ְҵ�����仯
		GENDER_CHANGED			=	0x08,	//	�Ա����仯
		MALL_ITEMS_CHANGED		=	0x10,	//	�̳����ݷ����仯
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

//	���� CECFactionPVPModel �Ĺ۲���
class CECFashionShop;
typedef CECObserver<CECFashionShop>	CECFashionShopObserver;

//	class CECFashionShop
class CECShopBase;
typedef CECObserver<CECShopBase> CECShopBaseObserver;
class CECFashionShop : public CECObservable<CECFashionShop>, public CECShopBaseObserver
{
public:
	enum enumSaleType{
		ST_NEW_ARRIVE,		//	��װ����
		ST_HOT_SALE,		//	��������
		ST_ON_SALE,			//	��ֵ����
		ST_FLASH_SALE,		//	����
		ST_COUNT,
	};
	enum enumFashionType{
		FT_SUITE,			//	��װ
		FT_HEAD,			//	ñ��
		FT_UPPER_BODY,		//	����
		FT_LOWER_BODY,		//	��װ
		FT_WAIST_N_SHOES,	//	��������ס�Ь�ӵȣ�
		FT_WEAPON,			//	ʱװ����
		FT_COUNT,
	};

	struct FashionSelection{
		union{
			int	itemIndex;
			struct{
				int	itemIndices[FASHION_SUITE_NUM];		//	�� itemIndices ����ǰ������ itemIndex ���ֺ���һ��
				const FASHION_SUITE_ESSENCE *pEssence;	//	ʱװ��װ essence�����ں���ʹ��
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
			LoopSuiteItemAndContinue(pShop, itemIndex, buyIndex);	//	Ĭ�Ϲ��� LoopSuiteItemAndContinue ���룬�ܶ�ʱ����Ҫ����
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

	//	������ CECShopBaseObserver
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

	enumSaleType		m_saleType;			//	��������
	enumFashionType		m_fashionType;		//	ʱװ����
	int					m_profession;		//	ѡ��ְҵ
	int					m_gender;			//	ѡ���Ա�

	typedef	abase::vector<FashionSelection>	SelectedFashion;
	SelectedFashion		m_selectedFashion;	//	ͨ���������������ѯ�õ���ʱװ�б�
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
	
	//	����
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