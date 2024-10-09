#include "AObject.h"

bool APropertyObject::InitStateTable()
{
	int count;
	count = PropertiesCount();
	for(int i = 0; i < count; i++)
	{
		int state = Properties(i)->GetState();
		m_StateList.push_back(state&AProperty::DEFAULT_VALUE?1:0);
	}
	return true;
}

