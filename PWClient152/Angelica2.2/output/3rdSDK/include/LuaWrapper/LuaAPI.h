/********************************************************************
	created:	2007/06/11
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#include "ScriptValue.h"

namespace LuaBind
{

#define 	IMPLEMENT_SCRIPT_API(api)  \
	int      Imp##api(lua_State * L)\
	{\
		abase::vector<CScriptValue> args;\
		abase::vector<CScriptValue> results;\
		GetStack(L, args, 1);\
		api(args, results);\
		SetStack(L, results);\
		return results.size();\
	}

#define    REGISTER_SCRIPT_API(lib, api) \
	{\
		__add_Script_Api(#lib, #api, Imp##api);\
	}
	void __add_Script_Api(const char * libname, const char * apiname, lua_CFunction  api);
	
	/**
	 * \brief register all apis to script
	 * \param[in]
	 * \param[out]
	 * \return
	 * \note should be called only once after implementing apis. 
	 * \warning
	 * \todo   
	 * \author kuiwu 
	 * \date 11/9/2006
	 * \see 
	 */
	void Reg2Script(lua_State * L);
	void Reg2Script(lua_State * L, const char * libName);
}