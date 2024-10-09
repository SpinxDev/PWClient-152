/********************************************************************
	created:	2005/08/31
	created:	31:8:2005   11:06
	file name:	Factory.h
	author:		yaojun
	
	purpose:	common factory
*********************************************************************/

#pragma once

#include <hashmap.h>
#include <ABaseDef.h>

template
<
	typename Identifier,
	class AbstractProduct,
	typename ProductCreator = AbstractProduct * (*)(),
	// it is better to nested template to accept a 
	// error handler class, but VC6 does not support nested
	// template. so we use ProductCreator instead.
	// if you don't offer a ErrorHandler, the default behavior
	// is ASSERT and return NULL
	ProductCreator ErrorHandler = 0
>
class Factory
{
protected:
	typedef abase::hash_map<Identifier, ProductCreator> AssocMap;
	typedef typename AssocMap::iterator AssocMapIter;
	typedef typename AssocMap::const_iterator AssocMapConstIter;
	AssocMap associations_;
public:
	Factory() {}
	~Factory() {}
	
	AbstractProduct * CreateObject(const Identifier& id) const
	{
		AssocMapConstIter iter = associations_.find(id);
		if (iter != associations_.end())
		{
			return (iter->second)();
		}
		if (ErrorHandler)
			return ErrorHandler();
		
		ASSERT(!"no match id in factory");
		return 0;
	}

	bool Register(const Identifier& id, ProductCreator creator)
	{
		return associations_.insert(AssocMap::value_type(id, creator)).second;
	}
	
	bool UnRegister(const Identifier& id)
	{
		return associations_.erase(id) == 1;
	}
};
