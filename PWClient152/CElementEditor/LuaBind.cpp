/********************************************************************
  created:	   19/4/2006  
  filename:	   LuaBind.cpp
  author:      Wangkuiwu  
  description: 
  Copyright (c) , All Rights Reserved.
*********************************************************************/

#include "LuaBind.h"
#include "LuaState.h"
#include "LuaScript.h"
#include <AString.h>
#include "LuaUtil.h"
namespace LuaBind
{

bool PreLoad()
{	
	return  true;
}


bool Init()
{
	
	if (!g_LuaStateMan.Init())
	{
		return false;
	}
	
	if (!PreLoad())
	{
	  return false;
	}

	return true;
}

void Release()
{
	g_LuaStateMan.Release();
}

}