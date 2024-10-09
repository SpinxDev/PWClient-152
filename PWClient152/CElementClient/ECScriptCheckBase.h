/********************************************************************
	created:	2005/08/30
	created:	30:8:2005   17:45
	file name:	ECCheckBase.h
	author:		yaojun
	
	purpose:	base class of checker
*********************************************************************/

#pragma once

#include "ECScriptBoolExp.h"
#include "Factory.h"
#include "SingletonHolder.h"
#include <AString.h>
#include "ECParamList.h"

//////////////////////////////////////////////////////////////////////////
// base class for checker
//////////////////////////////////////////////////////////////////////////

class CECScriptCheckBase : public CECScriptBoolExp, public CECParamList
{
public:
	CECScriptCheckBase() {}
	virtual ~CECScriptCheckBase() {}
};

typedef CECScriptCheckBase * (* ECScriptCheckCreator)();

inline CECScriptCheckBase * CreateECScriptCheckAssertAndReturnNull()
{
	ASSERT(!"no match id for factory for CECScriptCheckBase");
	return 0;
}

typedef Factory<AString, CECScriptCheckBase, ECScriptCheckCreator, CreateECScriptCheckAssertAndReturnNull> CECSciptCheckFactory;
typedef SingletonHolder<CECSciptCheckFactory> TheECSciptCheckFactory;

//////////////////////////////////////////////////////////////////////////
// helper for implement checker
//////////////////////////////////////////////////////////////////////////

// a derived checker must use this macro in a .cpp to register itself to factory
#define REGISTER_TO_ECSCRIPTCHECK_FACTORY(name, className) \
namespace \
{ \
	CECScriptCheckBase * CreateObject##className() \
	{ return new className; } \
	const bool bRegistered##className = \
	TheECSciptCheckFactory::Instance()->Register(name, CreateObject##className); \
}



//////////////////////////////////////////////////////////////////////////
// inline functions
//////////////////////////////////////////////////////////////////////////
