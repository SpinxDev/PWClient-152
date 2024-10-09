// Filename	: WikiSearchCommand.h
// Creator	: Feng Ning
// Date		: 2010/04/26

#pragma once

#include "AList2.h"
#include "AAssist.h"
#include "WikiSearcher.h"

#define WIKI_LEVEL_LIMIT 105

struct MONSTER_ESSENCE;
class WikiEquipmentProp;


//------------------------------------------------------------------------
//
// WikiSearchSuite
//
//------------------------------------------------------------------------
// combine several search commands
//------------------------------------------------------------------------
class WikiSearchSuite : public WikiSearcher::SearchCommand
{
public:
	WikiSearchSuite();
	virtual ~WikiSearchSuite();
	virtual WikiSearcher::SearchCommand* Clone() const;
	virtual bool operator()(WikiEntityPtr p) const;
	
	void Add(const WikiSearcher::SearchCommand* pCommand);
	bool IsEmpty() const;
	void Clear();

private:
	APtrList<WikiSearcher::SearchCommand*> m_Cmds;
};


//------------------------------------------------------------------------
//
// WikiEntityPtrProvider
//
//------------------------------------------------------------------------
// provide any WikiEntityPtr that added by user
//------------------------------------------------------------------------
class WikiEntityPtrProvider : public WikiSearcher::ContentProvider
{
public:
	typedef abase::vector<WikiEntityPtr> PtrVector;

	WikiEntityPtrProvider();
	WikiEntityPtrProvider(const PtrVector& entities);

	virtual WikiEntityPtr Next(WikiEntityPtr p);
	virtual WikiEntityPtr Begin();
	virtual WikiSearcher::ContentProvider* Clone() const;
	
	void Clear();
	void Add(WikiEntityPtr p);
	void Swap(PtrVector& entities);
	size_t Size() const { return m_Entities.size(); }

protected:
	PtrVector m_Entities;
	PtrVector::iterator m_CurPtr;
};

//------------------------------------------------------------------------
//
// WikiSearchAll
//
//------------------------------------------------------------------------
// simply return all result
//------------------------------------------------------------------------
class WikiSearchAll : public WikiSearcher::SearchCommand
{
public:
	WikiSearchAll(){};
	virtual bool operator()(WikiEntityPtr p) const {return true;}
	virtual WikiSearcher::SearchCommand* Clone() const {return new WikiSearchAll();}
};

//------------------------------------------------------------------------
//
// WikiSerialDataProvider
//
//------------------------------------------------------------------------
// provide a serial numbers in [start, start+count)
//------------------------------------------------------------------------
class WikiSerialDataProvider : public WikiSearcher::ContentProvider
{
public:
	class Entity : public WikiEntity
	{
	public:
		Entity(int id):m_ID(id) {}
		int GetID() const {return m_ID;}
		virtual bool operator==(const WikiEntity& rhs) const
		{
			const Entity* ptr = dynamic_cast<const Entity*>(&rhs);
			return !ptr ? false : m_ID == ptr->m_ID;
		}
	private:
		int m_ID;	
	};
	
	WikiSerialDataProvider(int start, int count);
	virtual WikiEntityPtr Next(WikiEntityPtr p);
	virtual WikiEntityPtr Begin();
	virtual WikiSearcher::ContentProvider* Clone() const;

private:
	int m_Start;
	int m_Count;
};
//------------------------------------------------------------------------
//
// WikiSearchRandPropOnEquipment
//
//------------------------------------------------------------------------
// search a generated-property on a equipment
//------------------------------------------------------------------------
class WikiSearchRandPropOnEquipment : public WikiSearcher::SearchCommand
{
public:
	WikiSearchRandPropOnEquipment(unsigned int item_id);
	~WikiSearchRandPropOnEquipment();

	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	unsigned int m_ItemID;
	WikiEquipmentProp* m_pEP;
};
//------------------------------------------------------------------------
//
// WikiSearchDropItemOnMonster
//
//------------------------------------------------------------------------
// search a valid item on a monster
//------------------------------------------------------------------------
class WikiSearchDropItemOnMonster : public WikiSearcher::SearchCommand
{
public:
	WikiSearchDropItemOnMonster(const MONSTER_ESSENCE* p);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	const MONSTER_ESSENCE* m_pEssence;
};


//------------------------------------------------------------------------
//
// WikiElementDataProvider
//
//------------------------------------------------------------------------
// provide the id from elementdata
//------------------------------------------------------------------------
class WikiElementDataProvider : public WikiSearcher::ContentProvider
{
public:
	class Entity : public WikiEntity
	{
	public:
		Entity(unsigned  id):m_ID(id) {}
		unsigned  GetID() const {return m_ID;}
		virtual bool operator==(const WikiEntity& rhs) const
		{
			const Entity* ptr = dynamic_cast<const Entity*>(&rhs);
			return !ptr ? false : m_ID == ptr->m_ID;
		}
	private:
		unsigned int m_ID;
	};
	
	WikiElementDataProvider(unsigned int id_space);
	virtual WikiEntityPtr Next(WikiEntityPtr p);
	virtual WikiEntityPtr Begin();
	
	virtual WikiSearcher::ContentProvider* Clone() const;

private:
	unsigned int m_IDSpace;
};

//------------------------------------------------------------------------
//
// WikiSearchBySpecificID
//
//------------------------------------------------------------------------
// search an essence by a specific id
//------------------------------------------------------------------------
class WikiSearchBySpecificID : public WikiSearcher::SearchCommand
{
public:
	WikiSearchBySpecificID(unsigned int id);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	unsigned int m_ID;
};

//------------------------------------------------------------------------
//
// WikiSearchMonsterDropByItemID
//
//------------------------------------------------------------------------
// search monsters who will drop this item
//------------------------------------------------------------------------
class WikiSearchMonsterDropByItemID : public WikiSearcher::SearchCommand
{
public:
	WikiSearchMonsterDropByItemID(unsigned int item_id);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	unsigned int m_ItemID;
};

//------------------------------------------------------------------------
//
// WikiSearchEquipmentByMonsterDrop
//
//------------------------------------------------------------------------
// search equipment by specific monster's drop properties
//------------------------------------------------------------------------
class WikiSearchEquipmentByMonsterDrop : public WikiSearcher::SearchCommand
{
public:
	WikiSearchEquipmentByMonsterDrop(const MONSTER_ESSENCE* pEssence);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	const MONSTER_ESSENCE* m_pEssence;
};

//------------------------------------------------------------------------
//
// WikiSearchRecipeByProductID
//
//------------------------------------------------------------------------
// search recipe who will product this item
//------------------------------------------------------------------------
class WikiSearchRecipeByProductID : public WikiSearcher::SearchCommand
{
public:
	WikiSearchRecipeByProductID(unsigned int item_id);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	unsigned int m_ItemID;
};

//------------------------------------------------------------------------
//
// WikiSearchRecipeByMaterialID
//
//------------------------------------------------------------------------
// search recipe who use this item
//------------------------------------------------------------------------
class WikiSearchRecipeByMaterialID : public WikiSearcher::SearchCommand
{
public:
	WikiSearchRecipeByMaterialID(unsigned int item_id);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	unsigned int m_ItemID;
};

//------------------------------------------------------------------------
//
// WikiSearchByName
//
//------------------------------------------------------------------------
// search by a name pattern
//------------------------------------------------------------------------
class WikiSearchByName : public WikiSearcher::SearchCommand
{
public:
	WikiSearchByName();
	virtual bool operator()(WikiEntityPtr p) const;

	void SetPattern(const ACString& pattern);
	const ACString& GetPattern() const {return m_strPattern;}	
	
	static bool IsSearchPatternValid(const ACHAR *szPattern);

protected:
	virtual ACString GetSearchName(WikiEntityPtr p) const = 0;
	
private:
	ACString m_strPattern;
};

//------------------------------------------------------------------------
//
// WikiSearchEquipmentByName
//
//------------------------------------------------------------------------
// search equipment by its name
//------------------------------------------------------------------------
class WikiSearchEquipmentByName : public WikiSearchByName
{
public:
	virtual ACString GetSearchName(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
};

//------------------------------------------------------------------------
//
// WikiSearchMonsterByName
//
//------------------------------------------------------------------------
// search monster by its name
//------------------------------------------------------------------------
class WikiSearchMonsterByName : public WikiSearchByName
{
public:
	virtual ACString GetSearchName(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
};

//------------------------------------------------------------------------
//
// WikiSearchTaskByName
//
//------------------------------------------------------------------------
// search task by its name
//------------------------------------------------------------------------
class WikiSearchTaskByName : public WikiSearchByName
{
public:
	virtual ACString GetSearchName(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
};

//------------------------------------------------------------------------
//
// WikiSearchRecipeByName
//
//------------------------------------------------------------------------
// search recipe by its name
//------------------------------------------------------------------------
class WikiSearchRecipeByName : public WikiSearchByName
{
public:
	virtual ACString GetSearchName(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
};

//------------------------------------------------------------------------
//
// WikiSearchItemByName
//
//------------------------------------------------------------------------
// search item by its name
//------------------------------------------------------------------------
class WikiSearchItemByName : public WikiSearchByName
{
public:
	WikiSearchItemByName(bool ignoreEquip);
	virtual ACString GetSearchName(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
private:
	bool m_NoEquip;
};

//------------------------------------------------------------------------
//
// WikiSkillDataProvider
//
//------------------------------------------------------------------------
// provide any WikiEntityPtr that added by user
//------------------------------------------------------------------------
class WikiSkillDataProvider : public WikiSearcher::ContentProvider
{
public:
	
	class Entity : public WikiEntity
	{
	public:
		Entity(unsigned int id):m_ID(id) {}
		unsigned int GetID() const {return m_ID;}
		virtual bool operator==(const WikiEntity& rhs) const
		{
			const Entity* ptr = dynamic_cast<const Entity*>(&rhs);
			return !ptr ? false : m_ID == ptr->m_ID;
		}
	private:
		unsigned int m_ID;	
	};
	
	WikiSkillDataProvider();
	virtual WikiEntityPtr Next(WikiEntityPtr p);
	virtual WikiEntityPtr Begin();
	virtual WikiSearcher::ContentProvider* Clone() const;
};

//------------------------------------------------------------------------
//
// WikiSearchSkillByName
//
//------------------------------------------------------------------------
// search skill by its name
//------------------------------------------------------------------------
class WikiSearchSkillByName : public WikiSearchByName
{
public:
	virtual ACString GetSearchName(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
	
	static const ACHAR* GetLocalizedName(int skillid);
};

//------------------------------------------------------------------------
//
// WikiSearchSkillByLevelRequire
//
//------------------------------------------------------------------------
// search a skill whose level require is specific
//------------------------------------------------------------------------
class WikiSearchSkillByLevelRequire : public WikiSearcher::SearchCommand
{
public:
	// set level to 0 will ignore the limitation
	WikiSearchSkillByLevelRequire(int levelmin, int levelmax);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
	
protected:
	bool BinarySearch(	int left, int levelmin, int right, int levelmax, 
		int deep, WikiSkillDataProvider::Entity* ptr, unsigned cur ) const;
	int m_Min;
	int m_Max;
};

//------------------------------------------------------------------------
//
// WikiSearchSkillBySpecificID
//
//------------------------------------------------------------------------
// search an essence by a specific id
//------------------------------------------------------------------------
class WikiSearchSkillBySpecificID : public WikiSearcher::SearchCommand
{
public:
	WikiSearchSkillBySpecificID(unsigned int id);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	unsigned int m_ID;
};

//------------------------------------------------------------------------
//
// WikiSearchSkillByClassLimit
//
//------------------------------------------------------------------------
// search an skill by a class mask
//------------------------------------------------------------------------
class WikiSearchSkillByClassLimit : public WikiSearcher::SearchCommand
{
public:
	WikiSearchSkillByClassLimit(unsigned int clslimit);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;

protected:
	unsigned int m_ClassLimit;
};

//------------------------------------------------------------------------
//
// WikiSearchFeatureByName
//
//------------------------------------------------------------------------
// search feature by its name (by WikiSerialDataProvider)
//------------------------------------------------------------------------
class WikiSearchFeatureByName : public WikiSearchByName
{
public:
	virtual ACString GetSearchName(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
};

//------------------------------------------------------------------------
//
// WikiSearchFeatureByLevel
//
//------------------------------------------------------------------------
// search a feature by level range (by WikiSerialDataProvider)
//------------------------------------------------------------------------
class WikiSearchFeatureByLevel : public WikiSearcher::SearchCommand
{
public:
	// set level to 0 will ignore the limitation.
	WikiSearchFeatureByLevel(int levelmin, int levelmax);
	virtual WikiSearcher::SearchCommand* Clone() const;
	virtual bool operator()(WikiEntityPtr p) const;
	
private:
	int m_Min;
	int m_Max;
};

//------------------------------------------------------------------------
//
// WikiSearchEquipmentGuide
//
//------------------------------------------------------------------------
// search a equipment guide by level and class (by WikiSerialDataProvider)
//------------------------------------------------------------------------
class WikiSearchEquipmentGuide : public WikiSearcher::SearchCommand
{
public:
	// set level to 0 will ignore the limitation.
	WikiSearchEquipmentGuide(int levelmin, int levelmax, unsigned int clslimit);
	virtual WikiSearcher::SearchCommand* Clone() const;
	virtual bool operator()(WikiEntityPtr p) const;
	
private:
	int m_Min;
	int m_Max;
	
	unsigned m_ClassLimit;
};

//------------------------------------------------------------------------
//
// WikiSearchAreaByLevel
//
//------------------------------------------------------------------------
// search a area guide by level range (by WikiSerialDataProvider)
//------------------------------------------------------------------------
class WikiSearchAreaByLevel : public WikiSearcher::SearchCommand
{
public:
	// set level to 0 will ignore the limitation.
	WikiSearchAreaByLevel(int levelmin, int levelmax);
	virtual WikiSearcher::SearchCommand* Clone() const;
	virtual bool operator()(WikiEntityPtr p) const;
	
private:
	int m_Min;
	int m_Max;
};
