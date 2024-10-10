#include "skillwrapper.h"
#include "skill.h"

namespace GNET{

	SkillWrapper SkillWrapper::instance;

	SkillWrapper::SkillWrapper()
	{
	}

	SkillWrapper::~SkillWrapper()
	{
		map.clear();
	}

	void SkillWrapper::LoadData(cmd_skill_data* pdata)
	{
		map.clear();
		PersistentData skill;
		for (int i=0; i < (int)pdata->count; i++)
		{
			const cmd_skill_data::data& item = pdata->list[i];
			skill.level = item.level;
			skill.ability = item.ability;
			map[item.id] = skill;
		}
	}

	int  SkillWrapper::GetLevel(unsigned int id)
	{
		StorageMap::iterator it = map.find(id);
		if(it != map.end() )
			return it->second.level;
		return 0;
	}

	int  SkillWrapper::GetAbility(unsigned int id)
	{
		StorageMap::iterator it = map.find(id);
		if(it != map.end() )
			return it->second.ability;
		return 0;
	}

	int  SkillWrapper::SetAbility(unsigned int id, int ability)
	{
		map[id].ability = ability;
		return ability;
	}

	int  SkillWrapper::SetLevel(unsigned int id, int level)
	{
		map[id].level = level;
		return level;
	}

	bool SkillWrapper::IsOverridden(unsigned int id)
	{
		for( StorageMap::iterator it = map.begin(),ie=map.end(); it!=ie; ++it )
		{
			if(IsOverridden(id, it->first)) return true;
		}
		return false;
	}
	bool SkillWrapper::IsOverridden(unsigned int id, unsigned int senior_id)
	{
		const SkillStub* stub =  SkillStub::GetStub(senior_id);
		if(!stub || !stub->is_senior) return false;

		for(size_t i=0; i<stub->pre_skills.size(); i++)
		{
			unsigned int pre_id = stub->pre_skills[i].first;
			if(pre_id <= 0) continue;
			if(id == pre_id) return true;
			if(IsOverridden(id, pre_id)) return true;
		}
		return false;
	}
}
