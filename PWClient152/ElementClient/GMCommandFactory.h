#pragma once

#include "SingletonHolder.h"
#include "Factory.h"
#include "GMCommand.h"
#include <vector.h>

typedef abase::vector<AString> CGMCommandKeyArray;
typedef Factory<AString, CGMCommand> InnerGMCommandFactory;
class CGMCommandFactory : public InnerGMCommandFactory
{
public:
	void GetRegisteredCommandKeys(CGMCommandKeyArray & keyArray);
	
};

typedef SingletonHolder<CGMCommandFactory> TheGMCommandFactory;

#define REGISTER_TO_GMCOMMAND_FACTORY(name, className) \
namespace \
{ \
	CGMCommand * CreateObject##className() \
	{ return new className; } \
	const bool bRegistered##className = \
		TheGMCommandFactory::Instance()->Register(name, CreateObject##className); \
}