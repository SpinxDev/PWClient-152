/********************************************************************
	created:	2005/08/30
	created:	30:8:2005   20:06
	file name:	ECScriptFunctionBase.h
	author:		yaojun
	
	purpose:	CECScriptFunctionBase represent a function in script
				following keyword "exec"
*********************************************************************/

#pragma once

#include "Factory.h"
#include "SingletonHolder.h"
#include "ECParamList.h"

class CECScriptContext;

class CECScriptFunctionBase : public CECParamList
{
public:
	virtual void Execute(CECScriptContext *pContext) = 0;
};

class CECScriptFunctionNull : public CECScriptFunctionBase
{
public:
	virtual void Execute(CECScriptContext *pContext) {}
};

typedef CECScriptFunctionBase * (* ECScriptFunctionCreator)();

inline CECScriptFunctionBase * CreateECScriptFunctionAssertAndReturnNull()
{
	ASSERT(!"no match id in factory for CECScriptFunctionBase");
	return 0;
}

typedef Factory<AString, CECScriptFunctionBase, ECScriptFunctionCreator, CreateECScriptFunctionAssertAndReturnNull> CECScriptFunctionFactory;
typedef SingletonHolder<CECScriptFunctionFactory> TheECScriptFunctionFacotry;

// a derived checker must use this macro in a .cpp to register itself to factory
#define REGISTER_TO_ECSCRIPTFUNCTION_FACTORY(name, className) \
namespace \
{ \
	CECScriptFunctionBase * CreateObject##className() \
	{ return new className; } \
	const bool bRegistered##className = \
	TheECScriptFunctionFacotry::Instance()->Register(name, CreateObject##className); \
}
