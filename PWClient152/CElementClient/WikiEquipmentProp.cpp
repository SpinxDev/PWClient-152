// Filename	: WikiEquipmentProp.cpp
// Creator	: Feng Ning
// Date		: 2010/04/27

#include "WikiEquipmentProp.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "elementdataman.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "AFI.h"

#define new A_DEBUG_NEW

// =======================================================================
// Data Extractor
// =======================================================================
template <typename Essence, typename SubType>
class EssenceExtractor : public WikiEquipmentProp
{
	friend WikiEquipmentProp;

private:
	const Essence* m_pE;
	CECIvtrItem* m_pItem;

	EssenceExtractor(const void* pE):m_pE((const Essence*)pE), m_pItem(NULL)
	{}

public:
	~EssenceExtractor()
	{
		if(m_pItem)
		{
			delete m_pItem;
		}
	}
	
	virtual ACString GetName() const {return m_pE->name;};

	virtual ACString GetTypeName() const
	{	
		DATA_TYPE dataType;
		const SubType* pType = 
			(const SubType*)g_pGame->GetElementDataMan()->get_data_ptr(m_pE->id_sub_type, ID_SPACE_ESSENCE, dataType);
		return pType->name;
	}

	virtual ACString GetDesc() const 
	{
		// lazy initializaton
		if(!m_pItem)
		{
			const_cast<CECIvtrItem*>(m_pItem) = CECIvtrItem::CreateItem(GetID(), 0, 1);
			m_pItem->GetDetailDataFromLocal();
		}

		return const_cast<CECIvtrItem*>(m_pItem)->GetDesc();
	}
	
	virtual AString GetIconFile() const
	{
		// lazy initializaton
		if(!m_pItem)
		{
			const_cast<CECIvtrItem*>(m_pItem) = CECIvtrItem::CreateItem(GetID(), 0, 1);
			m_pItem->GetDetailDataFromLocal();
		}
		
		AString szFile;
		af_GetFileTitle(const_cast<CECIvtrItem*>(m_pItem)->GetIconFile(), szFile);
		szFile.MakeLower();
		return szFile;
	}
	
	virtual unsigned int GetID() const {return m_pE->id;};
	virtual int GetGender() const {return -1;};
	virtual unsigned int GetClass() const {return m_pE->character_combo_id;};
	virtual int GetLevel() const {return m_pE->level;};
	virtual int GetLevelRequire() const {return m_pE->require_level;};
	virtual int GetLevelupAddon() const {return m_pE->levelup_addon;};
	
	virtual unsigned int GetSplit() const {return m_pE->element_id;};
	virtual int GetSplitNumber() const {return m_pE->element_num;};
	
	virtual unsigned int GetDamaged() const {return m_pE->id_drop_after_damaged;};
	virtual int GetDamagedNumber() const {return m_pE->num_drop_after_damaged;};

	virtual unsigned int GetRandPropID(unsigned int i) const
	{
		unsigned int rand_id = 0;

		if(i < GetRandPropSize())
		{
			rand_id = m_pE->addons[i].id_addon;
		}
		
		if(rand_id != 0)
		{
			for(unsigned int j=0;j<i;j++)
			{
				if(rand_id == m_pE->addons[j].id_addon)
				{
					// omit duplicated id
					rand_id = 0;
					break;
				}
			}
		}

		return rand_id;
	};

	virtual unsigned int GetRandPropSize() const {return sizeof(m_pE->addons)/sizeof(m_pE->addons[0]);};
	virtual ACString GetRandPropName(unsigned int i) const { return i >= GetRandPropSize() ?
		_AL(""):g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr()->GetStringFromTable(8661); };
	
};

#define EXTRACTOR_TYPE(TYPE) EssenceExtractor<TYPE##_ESSENCE, TYPE##_SUB_TYPE>

template<>
int EXTRACTOR_TYPE(FASHION)::GetGender() const {return m_pE->gender;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetClass() const {return 0;};
template<>
int EXTRACTOR_TYPE(FASHION)::GetLevelupAddon() const {return 0;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetSplit() const {return 0;};
template<>
int EXTRACTOR_TYPE(FASHION)::GetSplitNumber() const {return 0;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetDamaged() const {return 0;};
template<>
int EXTRACTOR_TYPE(FASHION)::GetDamagedNumber() const {return 0;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetRandPropID(unsigned int i) const { return 0;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetRandPropSize() const {return 0;};
template<>
ACString EXTRACTOR_TYPE(FASHION)::GetRandPropName(unsigned int i) const {return _AL("");};

template<>
unsigned int EXTRACTOR_TYPE(WEAPON)::GetRandPropID(unsigned int i) const
{
	size_t uniqueSize = (sizeof(m_pE->uniques)/sizeof(m_pE->uniques[0]));
	
	unsigned int rand_id = 0;
	
	if(i < uniqueSize)
	{
		rand_id = m_pE->uniques[i].id_unique;
		
		if(rand_id != 0)
		{
			for(unsigned int j=0;j<i;j++)
			{
				if(rand_id == m_pE->uniques[j].id_unique)
				{
					// omit duplicated id
					rand_id = 0;
					break;
				}
			}
		}
	}
	else if(i < GetRandPropSize())
	{
		i -= uniqueSize;
		rand_id = m_pE->addons[i].id_addon;

		if(rand_id != 0)
		{
			for(unsigned int j=0;j<i;j++)
			{
				if(rand_id == m_pE->addons[j].id_addon)
				{
					// omit duplicated id
					rand_id = 0;
					break;
				}
			}
		}
	}

	return rand_id;
}
template<>
unsigned int EXTRACTOR_TYPE(WEAPON)::GetRandPropSize() const
{
	// both unique and addon property
	return (sizeof(m_pE->uniques)/sizeof(m_pE->uniques[0]))
		+ (sizeof(m_pE->addons)/sizeof(m_pE->addons[0]));
};
template<>
ACString EXTRACTOR_TYPE(WEAPON)::GetRandPropName(unsigned int i) const
{
	size_t uniqueSize = (sizeof(m_pE->uniques)/sizeof(m_pE->uniques[0]));
	
	CECGameUIMan* pUIMan = dynamic_cast<CECGameUIMan*>(g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr());
	
	if(i < uniqueSize)
	{
		return pUIMan->GetStringFromTable(8660);
	}
	else if(i < GetRandPropSize())
	{
		return pUIMan->GetStringFromTable(8661);
	}
	
	return _AL("");
};

WikiEquipmentProp* WikiEquipmentProp::CreateProperty(unsigned int id)
{
	DATA_TYPE dataType = DT_INVALID;
	const void* pEP= g_pGame->GetElementDataMan()->get_data_ptr(id, ID_SPACE_ESSENCE, dataType);
	
	if(DT_WEAPON_ESSENCE == dataType)
	{
		return new EXTRACTOR_TYPE(WEAPON)(pEP);
	}
	else if(DT_ARMOR_ESSENCE == dataType)
	{
		return new EXTRACTOR_TYPE(ARMOR)(pEP);
	}
	else if(DT_DECORATION_ESSENCE == dataType)
	{
		return new EXTRACTOR_TYPE(DECORATION)(pEP);
	}
	else if(DT_FASHION_ESSENCE == dataType)
	{
		return new EXTRACTOR_TYPE(FASHION)(pEP);
	}
	
	return NULL;
}