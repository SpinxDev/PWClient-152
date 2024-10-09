#include "GMCommandFactory.h"


void CGMCommandFactory::GetRegisteredCommandKeys(CGMCommandKeyArray & keyArray)
{
	keyArray.clear();
	
	AssocMapIter iter = associations_.begin();
	for (; iter != associations_.end(); ++iter)
	{
		keyArray.push_back(iter->first);
	}
}
