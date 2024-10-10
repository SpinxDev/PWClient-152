/********************************************************************
	created:	2007/06/11
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#include <ABaseDef.h>
#include "LuaAPI.h"
#include <hashtab.h>
#include <vector.h>

namespace LuaBind
{

typedef struct  
{
	AString   name;
	lua_CFunction func;
}__Script_Api_Registry;
typedef  abase::vector<__Script_Api_Registry> __Script_Api_Lib;
typedef abase::hashtab<__Script_Api_Lib,  AString, abase::_hash_function>  CAPI_MAP;

CAPI_MAP     __g_SCript_Api_Map(32);


void __add_Script_Api(const char * libname,  const char * apiname, lua_CFunction api)
{
		using namespace abase;
		
		__Script_Api_Registry  reg;
		reg.name = apiname;
		reg.func = api;

		CAPI_MAP::pair_type Pair = __g_SCript_Api_Map.get(libname);
		if (Pair.second)
		{
			
			Pair.first->push_back(reg);
		}
		else
		{
			__Script_Api_Lib  lib;
			lib.push_back(reg);
			__g_SCript_Api_Map.put(libname, lib);
		}

}

void Reg2Script(lua_State * L)
{
		CAPI_MAP::iterator it;
		for (it = __g_SCript_Api_Map.begin(); it != __g_SCript_Api_Map.end(); ++it)
		{
			__Script_Api_Lib * pPkg = it.value();

			int count = pPkg->size();
			luaL_reg  * luapkg  = new luaL_reg[count +1];
			for (int i = 0; i < count; ++i)
			{
				luapkg[i].name = (pPkg->at(i)).name;
				luapkg[i].func = (pPkg->at(i)).func;
			}
			luapkg[count].name = NULL;
			luapkg[count].func = NULL;
			
			const AString * pLibName = it.key();
			luaL_register(L, *pLibName, luapkg);
			lua_pop(L, 1);

			delete[] luapkg;
		}
}

void Reg2Script(lua_State * L, const char * libName)
{
		CAPI_MAP::pair_type Pair = __g_SCript_Api_Map.get(libName);
		if (!Pair.second)
		{
			return;
		}

		__Script_Api_Lib * pPkg = Pair.first;

		int count = pPkg->size();
		luaL_reg  * luapkg  = new luaL_reg[count +1];
		for (int i = 0; i < count; ++i)
		{
			luapkg[i].name = (pPkg->at(i)).name;
			luapkg[i].func = (pPkg->at(i)).func;
		}
		luapkg[count].name = NULL;
		luapkg[count].func = NULL;
		
		luaL_register(L, libName, luapkg);
		lua_pop(L, 1);

		delete[] luapkg;

		
}

}