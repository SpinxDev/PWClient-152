#ifndef __SKILL_SKILLWRAPPER_H
#define __SKILL_SKILLWRAPPER_H

#pragma warning(disable:4786)
#include <hashmap.h>

namespace GNET
{

#pragma pack(1)

struct PersistentData
{
	int ability;    // ÊìÁ·¶È
	int level;      // ¼¶±ð

	PersistentData(int _t = 0, int _l = 1) : ability(_t), level(_l){ }
};

typedef abase::hash_map<unsigned int, PersistentData, abase::_hash_function> StorageMap;

struct cmd_skill_data
{
	size_t count;
	struct data
	{
		short id;
		char  level;
		short ability;
	}list[1];
};

#pragma pack()

class SkillWrapper
{
	StorageMap map;
	SkillWrapper();
	static SkillWrapper instance;
public:
	static SkillWrapper* Instance() { return &instance; }
	~SkillWrapper();
	void LoadData(cmd_skill_data* data);
	int  GetLevel(unsigned int id);
	int  SetLevel(unsigned int id, int level);
	int  GetAbility(unsigned int id);
	int  SetAbility(unsigned int id, int ability);
	bool IsOverridden(unsigned int id);
	bool IsOverridden(unsigned int id, unsigned int senior_id);
};

}

#endif

