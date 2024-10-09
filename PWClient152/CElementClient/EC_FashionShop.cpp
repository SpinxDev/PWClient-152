// Filename	: EC_FashionShop.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/22

#include "EC_FashionShop.h"
#include "EC_Shop.h"
#include "EC_ShopSearch.h"
#include "globaldataman.h"
#include "EC_RoleTypes.h"
#include "EC_Game.h"
#include "elementdataman.h"
#include "EC_IvtrTypes.h"
#include "EC_ProfConfigs.h"

#include <map>

extern CECGame * g_pGame;

//	class CECFashionShopChange
CECFashionShopChange::CECFashionShopChange(unsigned int changeMask)
: m_changeMask(changeMask)
{
}

unsigned int CECFashionShopChange::GetChangeMask()const{
	return m_changeMask;
}

bool CECFashionShopChange::SaleTypeChanged()const{
	return (GetChangeMask() & SALE_TYPE_CHANGED) != 0;
}

bool CECFashionShopChange::FashionTypeChanged()const{
	return (GetChangeMask() & FASHION_TYPE_CHANGED) != 0;
}

bool CECFashionShopChange::ProfessionChanged()const{
	return (GetChangeMask() & PROFESSION_CHANGED) != 0;
}

bool CECFashionShopChange::GenderChanged()const{
	return (GetChangeMask() & GENDER_CHANGED) != 0;
}

bool CECFashionShopChange::MallItemsChanged()const{
	return (GetChangeMask() & MALL_ITEMS_CHANGED) != 0;
}

//	class CECShopSearchFashionShopPolicy
class CECShopSearchFashionShopPolicy : public CECShopSearchPolicyBase
{
	CECFashionShop::enumSaleType		m_saleType;			//	��������
	CECFashionShop::enumFashionType		m_fashionType;		//	ʱװ����
	int									m_profession;		//	ѡ��ְҵ
	int									m_gender;			//	ѡ���Ա�
	elementdataman *					m_elementdataman;	//	Elements.data

public:
	CECShopSearchFashionShopPolicy(CECFashionShop::enumSaleType newSaleType, CECFashionShop::enumFashionType newFashionType, int newProfession, int newGender)
		: m_saleType(newSaleType)
		, m_fashionType(newFashionType)
		, m_profession(newProfession)
		, m_gender(newGender) {
		m_elementdataman = g_pGame->GetElementDataMan();
	}

	virtual bool CanAccept(const CECShopBase *pShop, int index)const{
		//	m_saleType
		//	��������
		int buyIndex = -1;
		if (!pShop->CalcBuyIndex(index, &buyIndex, INT_MAX)){
			return false;
		}
		const GSHOP_ITEM &item = *pShop->GetItem(index);
		if (!pShop->GetFashionShopCategory().MatchItem(item)){
			return false;
		}
		if (!pShop->MatchOwnerNPC(item)){
			return false;
		}
		unsigned int status = item.buy[buyIndex].status;
		bool bMatchStatus(false);
		switch (m_saleType){
		case CECFashionShop::ST_NEW_ARRIVE:
			bMatchStatus = (status == 1);
			break;
		case CECFashionShop::ST_HOT_SALE:
			bMatchStatus = (status == 3);
			break;
		case CECFashionShop::ST_ON_SALE:
			bMatchStatus = (status == 2) ||(status >= 4 && status <= 12);
			break;
		case CECFashionShop::ST_FLASH_SALE:
			bMatchStatus = (status == 13);
			break;
		}
		if (!bMatchStatus){
			return false;
		}
		
		//	���������ж�ʱװ
		bool bValidFashion(false);

		//	�ж��Ƿ�Ϊʱװ
		DATA_TYPE dataType = DT_INVALID;
		const void * p = m_elementdataman->get_data_ptr(item.id, ID_SPACE_ESSENCE, dataType);
		while (dataType == DT_FASHION_ESSENCE){
			const FASHION_ESSENCE *pEssence = static_cast<const FASHION_ESSENCE *>(p);

			//	m_gender
			//	�ж��Ա��Ƿ����
			if (m_gender != pEssence->gender){
				break;
			}			

			//	m_fashionType��m_profession
			//	�ж�ʱװ���ʱװ���������ж�ְҵ��
			//	�Ȼ�ȡʱװ FASHION_SUB_TYPE
			p = m_elementdataman->get_data_ptr(pEssence->id_sub_type, ID_SPACE_ESSENCE, dataType);
			if (!p || dataType != DT_FASHION_SUB_TYPE){
				ASSERT(false);
				break;
			}
			const FASHION_SUB_TYPE *pSubTypeEssence = static_cast<const FASHION_SUB_TYPE *>(p);
			//	������ FASHION_SUB_TYPE �������ж����
			bool bMatchFashionType(false);
			switch (pSubTypeEssence->equip_fashion_mask){
			case EQUIP_MASK64_FASHION_HEAD:
				if (m_fashionType == CECFashionShop::FT_HEAD ||
					m_fashionType == CECFashionShop::FT_SUITE){
					bMatchFashionType = true;
				}
				break;
			case EQUIP_MASK64_FASHION_BODY:
				if (m_fashionType == CECFashionShop::FT_UPPER_BODY ||
					m_fashionType == CECFashionShop::FT_SUITE){
					bMatchFashionType = true;
				}
				break;
			case EQUIP_MASK64_FASHION_LEG:
				if (m_fashionType == CECFashionShop::FT_LOWER_BODY ||
					m_fashionType == CECFashionShop::FT_SUITE){
					bMatchFashionType = true;
				}
				break;
			case EQUIP_MASK64_FASHION_FOOT:
			case EQUIP_MASK64_FASHION_WRIST:
				if (m_fashionType == CECFashionShop::FT_WAIST_N_SHOES ||
					m_fashionType == CECFashionShop::FT_SUITE){
					bMatchFashionType = true;
				}
				break;
			case EQUIP_MASK64_FASHION_WEAPON:
				if (m_fashionType == CECFashionShop::FT_WEAPON ||
					m_fashionType == CECFashionShop::FT_SUITE){
					if (pEssence->character_combo_id & (1 << m_profession)){
						bMatchFashionType = true;
					}
				}
				break;
			}
			if (!bMatchFashionType){
				break;
			}

			bValidFashion = true;
			break;
		}
		return bValidFashion;
	}
};

//	class CECFashionShop
CECFashionShop::CECFashionShop(CECShopBase *pShop, enumSaleType newSaleType, enumFashionType newFashionType, int newProfession, int newGender)
: m_pShop(pShop)
, m_saleType(newSaleType)
, m_fashionType(newFashionType)
, m_profession(newProfession)
, m_gender(newGender){
	if (m_pShop){
		m_pShop->RegisterObserver(this);
	}
	Reselect();
}

CECFashionShop::~CECFashionShop(){
	if (m_pShop){
		m_pShop->UnregisterObserver(this);
	}
}

void CECFashionShop::OnModelChange(const CECShopBase *p, const CECObservableChange *q){
	if (p != m_pShop){
		ASSERT(false);
		return;
	}
	const CECShopBaseChange * pChange = dynamic_cast<const CECShopBaseChange *>(q);
	if (pChange->ItemChanged()){
		Reselect();		
		CECFashionShopChange change(CECFashionShopChange::MALL_ITEMS_CHANGED);
		NotifyObservers(&change);
	}
}

CECShopBase * CECFashionShop::Shop()const{
	return m_pShop;
}

CECFashionShop::enumSaleType CECFashionShop::SaleType()const{
	return m_saleType;
}

CECFashionShop::enumFashionType CECFashionShop::FashionType()const{
	return m_fashionType;
}

bool CECFashionShop::IsFashionTypeSuite()const{
	return FashionType() == FT_SUITE;
}

int	CECFashionShop::Profession()const{
	return m_profession;
}

int	CECFashionShop::Gender()const{
	return m_gender;
}

void CECFashionShop::ChangeSaleType(enumSaleType newSaleType){
	if (SaleType() == newSaleType){
		return;
	}
	m_saleType = newSaleType;
	Reselect();
	CECFashionShopChange change(CECFashionShopChange::SALE_TYPE_CHANGED);
	NotifyObservers(&change);
}

void CECFashionShop::ChangeFashionType(enumFashionType newFashionType){
	if (FashionType() == newFashionType){
		return;
	}
	m_fashionType = newFashionType;
	Reselect();
	CECFashionShopChange change(CECFashionShopChange::FASHION_TYPE_CHANGED);
	NotifyObservers(&change);
}

void CECFashionShop::ChangeProfession(int newProfession){
	if (newProfession < 0 || newProfession >= NUM_PROFESSION){
		ASSERT(false);
		return;
	}
	if (!CECProfConfig::Instance().IsExist(newProfession, Gender())){
		ASSERT(false);
		return;
	}
	if (Profession() == newProfession){
		return;
	}
	m_profession = newProfession;
	Reselect();
	CECFashionShopChange change(CECFashionShopChange::PROFESSION_CHANGED);
	NotifyObservers(&change);
}

void CECFashionShop::ChangeGender(int newGender){
	if (newGender < 0 || newGender >= NUM_GENDER){
		ASSERT(false);
		return;
	}
	if (!CECProfConfig::Instance().IsExist(Profession(), newGender)){
		ASSERT(false);
		return;
	}
	if (Gender() == newGender){
		return;
	}
	m_gender = newGender;
	Reselect();
	CECFashionShopChange change(CECFashionShopChange::GENDER_CHANGED);
	NotifyObservers(&change);
}

void CECFashionShop::ChangeProfessionGender(int newProfession, int newGender){
	if (!CECProfConfig::Instance().IsExist(newProfession, newGender)){
		ASSERT(false);
		return;
	}
	unsigned int changeMask = 0;
	if (Profession() != newProfession){
		changeMask |= CECFashionShopChange::PROFESSION_CHANGED;
	}
	if (Gender() != newGender){
		changeMask |= CECFashionShopChange::GENDER_CHANGED;
	}
	if (changeMask){
		m_profession = newProfession;
		m_gender = newGender;
		
		Reselect();
		
		CECFashionShopChange change(changeMask);
		NotifyObservers(&change);
	}
}

void CECFashionShop::ChangeAll(enumSaleType newSaleType, enumFashionType newFashionType, int newProfession, int newGender){
	if (newProfession < 0 || newProfession >= NUM_PROFESSION){
		ASSERT(false);
		return;
	}
	if (newGender < 0 || newGender >= NUM_GENDER){
		ASSERT(false);
		return;
	}
	if (!CECProfConfig::Instance().IsExist(newProfession, newGender)){
		ASSERT(false);
		return;
	}
	unsigned int changeMask = 0;
	if (SaleType() != newSaleType){
		changeMask |= CECFashionShopChange::SALE_TYPE_CHANGED;
	}
	if (FashionType() != newFashionType){
		changeMask |= CECFashionShopChange::FASHION_TYPE_CHANGED;
	}
	if (Profession() != newProfession){
		changeMask |= CECFashionShopChange::PROFESSION_CHANGED;
	}
	if (Gender() != newGender){
		changeMask |= CECFashionShopChange::GENDER_CHANGED;
	}
	if (changeMask){
		m_saleType = newSaleType;
		m_fashionType = newFashionType;
		m_profession = newProfession;
		m_gender = newGender;

		Reselect();

		CECFashionShopChange change(changeMask);
		NotifyObservers(&change);
	}
}

int	CECFashionShop::Count()const{
	return (int)m_selectedFashion.size();
}

const CECFashionShop::FashionSelection*	CECFashionShop::ItemAt(int index)const{
	if (index < 0 || index >= Count()){
		ASSERT(false);
		return NULL;
	}
	return &m_selectedFashion[index];
}

void CECFashionShop::LoopSelectedFashion(int fashionIndex, int buyIndex, CECSelectedFashionLooper *looper)const{
	if (const CECFashionShop::FashionSelection *pSelectedFashion = ItemAt(fashionIndex)){
		const CECShopBase *pShop = Shop();
		if (IsFashionTypeSuite()){
			for (int i(0); i < FASHION_SUITE_NUM; ++ i){
				int itemIndex = pSelectedFashion->suite.itemIndices[i];
				if (itemIndex < 0){
					continue;
				}
				if (!looper->LoopSuiteItemAndContinue(pShop, itemIndex, buyIndex)){
					break;
				}
			}
		}else if (pSelectedFashion->itemIndex >= 0){
			looper->VisitNonSuiteItem(pShop, pSelectedFashion->itemIndex, buyIndex);
		}
	}
}

void CECFashionShop::Reselect(){
	if (IsFashionTypeSuite()){
		//	��װ�߼����⣬��������
		SelectSuite();
	}else{
		//	�����������
		m_selectedFashion.clear();

		//	���ݵ�ǰ����ʵʩ����
		CECShopSearch searcher(m_pShop);
		searcher.SetPolicy(new CECShopSearchFashionShopPolicy(SaleType(), FashionType(), Profession(), Gender()));
		CECShopSearchResult searchResult(m_pShop);
		if (!searcher.Search(&searchResult)){
			return;
		}

		//	�����������
		m_selectedFashion.reserve(searchResult.Count());
		for (int i(0); i < searchResult.Count(); ++ i){
			m_selectedFashion.push_back(FashionSelection(searchResult[i]));
		}
	}
}

void CECFashionShop::SelectSuite(){
	if (!IsFashionTypeSuite()){
		ASSERT(false);
		return;
	}

	//	�����������
	m_selectedFashion.clear();

	//	���������з��ϵ�ǰ����Ҫ���ʱװ
	CECShopSearch searcher(m_pShop);
	searcher.SetPolicy(new CECShopSearchFashionShopPolicy(SaleType(), FashionType(), Profession(), Gender()));
	CECShopSearchResult searchResult(m_pShop);
	if (!searcher.Search(&searchResult)){
		return;
	}

	//	������������������ٲ�ѯ�ṹ�������� FASHION_SUITE_ESSENCE �п����ж�ʱװ�Ƿ����
	typedef std::multimap<int, int>	FashionID2ItemIndexMap;
	typedef std::pair<FashionID2ItemIndexMap::const_iterator, FashionID2ItemIndexMap::const_iterator> PairCC;
	FashionID2ItemIndexMap	id2index;
	for (int i(0); i < searchResult.Count(); ++ i){
		int	itemIndex = searchResult[i];
		const GSHOP_ITEM &item = *m_pShop->GetItem(itemIndex);
		id2index.insert(std::pair<int, int>(item.id, itemIndex));
	}

	//	���ɨ����װ���� FASHION_SUITE_ESSENCE
	elementdataman * pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE dataType = DT_INVALID;
	unsigned int id = pDataMan->get_first_data_id(ID_SPACE_ESSENCE, dataType);
	while (id){
		if (DT_FASHION_SUITE_ESSENCE == dataType){
			const void *p = pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dataType);
			const FASHION_SUITE_ESSENCE *pSuiteEssence = static_cast<const FASHION_SUITE_ESSENCE *>(p);
			FashionSelection suiteSelection;
			suiteSelection.suite.pEssence = pSuiteEssence;

			bool bSuiteNotAppearInShop(false);	//	��� FASHION_SUITE_ESSENCE ������ID��ʱװ�����Ƿ����̳��г���
			bool bSuiteHasAtLeast2Items(false);	//	��� FASHION_SUITE_ESSENCE ���Ƿ���������2������
			bool bSuiteHasSameBuyType(false);	//	��� FASHION_SUITE_ESSENCE �����������ܷ��ҵ�������ͬ�Ĺ���ѡ�ʱ����أ�

			for (int i(0); i < FASHION_SUITE_NUM; ++ i){
				int suiteItemID = pSuiteEssence->list[i];
				if (suiteItemID > 0){
					PairCC fashion = id2index.equal_range(suiteItemID);
					if (fashion.first == fashion.second){
						bSuiteNotAppearInShop = true;	//	��װ�����á���δ���̳ǵ�ǰ�����³���
						break;
					}
					//	fashion Ϊ���ֵĵ�һ����װ��ʱװ
					if (i+1 == FASHION_SUITE_NUM){
						bSuiteHasAtLeast2Items = false;
						break;
					}
					//	�� fashion �������̳ǹ���λ��
					for (FashionID2ItemIndexMap::const_iterator cit = fashion.first; cit != fashion.second; ++ cit){
						int itemIndex = cit->second;
						suiteSelection.suite.itemIndices[i] = itemIndex;

						//	�����װ�к�������ʱװ
						for (int j = i+1; j < FASHION_SUITE_NUM; ++ j){
							int suiteItemID2 = pSuiteEssence->list[j];
							if (suiteItemID2 <= 0){
								continue;
							}
							PairCC fashion2 = id2index.equal_range(suiteItemID2);
							if (fashion2.first == fashion2.second){
								bSuiteNotAppearInShop = true;
								break;
							}
							bSuiteHasAtLeast2Items = true;

							//	�� fashion2 �������̳ǹ���λ��
							bSuiteHasSameBuyType = false;
							for (FashionID2ItemIndexMap::const_iterator cit2 = fashion2.first; cit2 != fashion2.second; ++ cit2){
								int itemIndex2 = cit2->second;
								suiteSelection.suite.itemIndices[j] = itemIndex2;

								//	Ѱ���� fashion �е�ǰ����λ�õĹ���ѡ����ȫ��ͬ��
								if (m_pShop->HasSameBuyType(itemIndex, itemIndex2)){
									//	���һ��ʱװ�̳ǿɹ�����װ
									bSuiteHasSameBuyType = true;
									break;
								}
							}
							if (!bSuiteHasSameBuyType){
								break;
							}
						}
						if (bSuiteNotAppearInShop){
							break;
						}
					}
				}else{
					suiteSelection.suite.itemIndices[i] = -1;		//	��Ӧλ��û������ʱװ
				}
			}

			if (!bSuiteNotAppearInShop && bSuiteHasAtLeast2Items && bSuiteHasSameBuyType){
				m_selectedFashion.push_back(suiteSelection);
			}
		}
		id = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dataType);
	}

	//	�����������������
	SortSuite();
}

struct CECFashionShopSuiteSorter
{
	bool operator()(const CECFashionShop::FashionSelection &lhs, const CECFashionShop::FashionSelection &rhs)const{
		return lhs.suite.pEssence->show_order > rhs.suite.pEssence->show_order;
	}
};
void CECFashionShop::SortSuite(){
	if (!IsFashionTypeSuite()){
		ASSERT(false);
		return;
	}
	std::sort(m_selectedFashion.begin(), m_selectedFashion.end(), CECFashionShopSuiteSorter());
}

//	class CECFashionShopManager
CECFashionShopManager::CECFashionShopManager()
{
	for (int i(0); i < FASHION_SHOP_COUNT; ++ i){
		m_pFashionShop[i] = NULL;
	}
}

CECFashionShopManager::~CECFashionShopManager(){
	Clear();
}

CECFashionShopManager & CECFashionShopManager::Instance(){
	static CECFashionShopManager s_instance;
	return s_instance;
}

void CECFashionShopManager::Initialize(CECFashionShop::enumSaleType newSaleType, CECFashionShop::enumFashionType newFashionType, int newProfession, int newGender){
	if (m_pFashionShop[0]){
		ASSERT(false);
		return;
	}
	m_pFashionShop[QSHOP_FASHION_SHOP] = new CECFashionShop(&CECQShop::Instance(), newSaleType, newFashionType, newProfession, newGender);
	m_pFashionShop[BACKSHOP_FASHION_SHOP] = new CECFashionShop(&CECBackShop::Instance(), newSaleType, newFashionType, newProfession, newGender);
}

void CECFashionShopManager::Clear(){
	for (int i(0); i < FASHION_SHOP_COUNT; ++ i){
		if (m_pFashionShop[i]){
			delete m_pFashionShop[i];
			m_pFashionShop[i] = NULL;
		}
	}
}

CECFashionShop * CECFashionShopManager::FashionShopAt(int index){
	return m_pFashionShop[index];
}

CECFashionShop * CECFashionShopManager::QShopFashionShop(){
	return FashionShopAt(QSHOP_FASHION_SHOP);
}

CECFashionShop * CECFashionShopManager::BackShopFashionShop(){
	return FashionShopAt(BACKSHOP_FASHION_SHOP);
}