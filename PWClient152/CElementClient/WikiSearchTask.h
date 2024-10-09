// Filename	: WikiSearchTask.h
// Creator	: Feng Ning
// Date		: 2010/10/29

#pragma once

#include "TaskTemplMan.h"
#include "WikiSearcher.h"

//------------------------------------------------------------------------
//
// WikiTaskDataProvider
//
//------------------------------------------------------------------------
// provide the task iterator
//------------------------------------------------------------------------
class WikiTaskDataProvider : public WikiSearcher::ContentProvider
{
public:
	class Entity : public WikiEntity
	{
	public:
		Entity(TaskTemplMap::iterator itr):m_Itr(itr) {}
		TaskTemplMap::iterator GetIterator() const {return m_Itr;}
		virtual bool operator==(const WikiEntity& rhs) const
		{
			const Entity* ptr = dynamic_cast<const Entity*>(&rhs);
			return !ptr ? false : m_Itr == ptr->m_Itr;
		}
	private:
		TaskTemplMap::iterator m_Itr;
	};
	
	WikiTaskDataProvider();
	virtual WikiEntityPtr Next(WikiEntityPtr p);
	virtual WikiEntityPtr Begin();
	
	virtual WikiSearcher::ContentProvider* Clone() const;

protected:
	Entity* GetNextValidEntity(TaskTemplMap::iterator next);
};

//------------------------------------------------------------------------
//
// WikiSearchTaskByMonster
//
//------------------------------------------------------------------------
// search a task that related to a specific monster
//------------------------------------------------------------------------
class WikiSearchTaskByMonster : public WikiSearcher::SearchCommand
{
public:
	WikiSearchTaskByMonster(unsigned int monster_id);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	bool IsValid(ATaskTempl* pTempl) const;
	unsigned int m_MonsterID;
};

//------------------------------------------------------------------------
//
// WikiSearchTaskByItemID
//
//------------------------------------------------------------------------
// search a task that related to a specific item
//------------------------------------------------------------------------
class WikiSearchTaskByItemID : public WikiSearcher::SearchCommand
{
public:
	WikiSearchTaskByItemID(unsigned int item_id);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	bool IsValid(ATaskTempl* pTempl) const;
	unsigned int m_ItemID;
};

//------------------------------------------------------------------------
//
// WikiSearchTaskBySpecificID
//
//------------------------------------------------------------------------
// search a task by a specific id
//------------------------------------------------------------------------
class WikiSearchTaskBySpecificID : public WikiSearcher::SearchCommand
{
public:
	WikiSearchTaskBySpecificID(unsigned int id);
	virtual bool operator()(WikiEntityPtr p) const;
	virtual WikiSearcher::SearchCommand* Clone() const;
protected:
	unsigned int m_ID;
};

//------------------------------------------------------------------------
//
// WikiSearchTaskByLevel
//
//------------------------------------------------------------------------
// search a task by level range
//------------------------------------------------------------------------
class WikiSearchTaskByLevel : public WikiSearcher::SearchCommand
{
public:
	// set level to 0 will ignore the limitation
	WikiSearchTaskByLevel(int levelmin, int levelmax);
	virtual WikiSearcher::SearchCommand* Clone() const;
	virtual bool operator()(WikiEntityPtr p) const;
	
private:
	int m_Min;
	int m_Max;
};

//------------------------------------------------------------------------
//
// WikiSearchTaskByClassLimit
//
//------------------------------------------------------------------------
// search a task by a class mask
//------------------------------------------------------------------------
class WikiSearchTaskByClassLimit : public WikiSearcher::SearchCommand
{
public:
	WikiSearchTaskByClassLimit(unsigned int clslimit);
	virtual WikiSearcher::SearchCommand* Clone() const;
	virtual bool operator()(WikiEntityPtr p) const;
	
private:
	unsigned int m_ClassLimit;
};

//------------------------------------------------------------------------
//
// WikiSearchTaskForGuide
//
//------------------------------------------------------------------------
// search a task for wiki guide
//------------------------------------------------------------------------
class WikiSearchTaskForGuide : public WikiSearcher::SearchCommand
{
public:
	WikiSearchTaskForGuide();
	virtual WikiSearcher::SearchCommand* Clone() const;
	virtual bool operator()(WikiEntityPtr p) const;
};

class WikiTaskDataChecker
{
public:
	WikiTaskDataChecker():m_bInit(false){}
	
	bool operator()(ATaskTempl* pTempl);
	
protected:
	std::set<unsigned int> m_Task;
	bool m_bInit;
	
	void Init();
};

extern WikiTaskDataChecker g_Checker;