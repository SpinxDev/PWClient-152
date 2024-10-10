/********************************************************************
  created:	   9/5/2006  
  filename:	   LuaDlgAPI.h
  author:      Wangkuiwu  
  description: 
  Copyright (c) , All Rights Reserved.
*********************************************************************/

#ifndef _LUA_DLGAPI_H_
#define _LUA_DLGAPI_H_

class AUILuaManager;

class CLuaState;

void InitDlgApi(CLuaState * pState);

extern inline AUILuaManager* GetAuiManager();


#endif