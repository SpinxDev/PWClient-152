#pragma once

#include "LuaFunc.h"

// ���LUA����
void LuaGetParam(lua_State * L,StringVector& params)
{
	params.clear();
	AString param;
	int i = 0;
	while(true)
	{
		++i;
		if (!LuaBind::CheckValue(LuaBind::TypeWrapper<const char*>(), L, i, param))
		{
			break;
		}
		params.push_back(param);
	}
}

// ��ȡʵ��
LuaFuncFactory& LuaFuncFactory::GetSingleton()
{
	static LuaFuncFactory fac;
	return fac;
}
// ���캯��
LuaFuncFactory::LuaFuncFactory()
{

}
// ��������
LuaFuncFactory::~LuaFuncFactory()
{
	NamespaceLuaFuncUsageMap::iterator nit;
	for( nit=m_usageMap.begin();nit!=m_usageMap.end();++nit )
	{
		delete nit->second;
	}
	m_usageMap.clear();
}
// ��ӡʹ��˵��
void LuaFuncFactory::PrintUsage(const char*pnamespace)
{
	AString strMsg;
	AddConsoleText("----------------lua��ʹ�õĺ����Լ���˵��:------------------\n", LOGCOLOR_GREEN);
	NamespaceLuaFuncUsageMap::iterator nit;
	AString nspace; 
	AString nfunc; 
	AString npattern = pnamespace;
	npattern.MakeLower();
	for( nit=m_usageMap.begin();nit!=m_usageMap.end();++nit )
	{
		nspace = nit->first;
		nspace.MakeLower();
		
		LuaFuncUsageMap::iterator it;
		for( it=nit->second->begin();it!=nit->second->end();++it )
		{
			nfunc = it->first;
			nfunc.MakeLower();
			if(pnamespace && nspace.Find(npattern) == -1 && nfunc.Find(npattern) == -1) continue;

			strMsg.Format("%s.%s��%s\n", nit->first, it->first, it->second);
			AddConsoleText(strMsg, LOGCOLOR_GREEN);
		}
	}
	AddConsoleText("------------------------����--------------------------------\n", LOGCOLOR_GREEN);
}
// ��ע�ắ����ʹ��˵��
void LuaFuncFactory::RegisterUsage(const char*pname,const char*pusage,const char*pnamespace)
{
	NamespaceLuaFuncUsageMap::iterator nit = m_usageMap.find(pnamespace);
	if(nit == m_usageMap.end()) 
	{
		m_usageMap[pnamespace] = new LuaFuncUsageMap();
		nit = m_usageMap.find(pnamespace);
	}
	(*nit->second)[pname] = pusage;
}
// ע��C++�еĺ�����LUA��
void LuaFuncFactory::Register(const char*pname,int (*pfunc)(lua_State *),const char*pnamespace)
{
	NamespaceLuaFuncMap::iterator nit = m_funcMap.find(pnamespace);
	if(nit == m_funcMap.end()) 
	{
		m_funcMap[pnamespace] = new LuaFuncMap();
		nit = m_funcMap.find(pnamespace);
	}
	(*nit->second)[pname] = pfunc;
}
// ȫ�����µ�LUA��
void LuaFuncFactory::Flash()
{
	NamespaceLuaFuncMap::iterator nit;
	for( nit=m_funcMap.begin();nit!=m_funcMap.end();++nit )
	{
		LuaFuncMap::iterator it;
		for( it=nit->second->begin();it!=nit->second->end();++it )
		{
			InitLuaTest(g_LuaStateMan.GetConfigState()->GetVM(), it->first, it->second,nit->first);
		}

		delete nit->second;
	}

	
	m_funcMap.clear();
}
// ͨ��ע��lua����
void LuaFuncFactory::InitLuaTest(lua_State * L,const char*pname,int (*pfunc)(lua_State *),const char*pnamespace,const char*pFileName)
{
	luaL_reg* pTest = new luaL_reg[2];
	pTest[0].name = pname;
	pTest[0].func = pfunc;
	
	pTest[1].name = 0;
	pTest[1].func = 0;
	
	luaL_register(L, pnamespace, pTest);
	lua_pop(L, 1);
	
	ReflashSrciptFile(pFileName);

	delete[] pTest;
}
// ����ע��lua�ļ����Ա㱣������
void LuaFuncFactory::ReflashSrciptFile(const char*pFileName)
{
	if(pFileName)
	{
		// now preload the global lua file
		g_LuaStateMan.GetConfigState()->Lock();
		g_LuaStateMan.GetConfigState()->UnRegister(pFileName);

		// ����Lua�ļ����ڴ�
		FILE* pFile = fopen(pFileName, "rb");
		if( !pFile )
		{
			a_LogOutput(1, "Load lua script file failed! (%s)", pFileName);
			return;
		}

		BYTE BOM[3];
		fread(BOM, sizeof(BYTE), 3, pFile);
		if( BOM[0] != 0xef || BOM[1] != 0xbb || BOM[2] != 0xbf )
		{
			fclose(pFile);
			a_LogOutput(1, "Invalid lua script file format! (%s)", pFileName);
			return;
		}

		fseek(pFile, 0, SEEK_END);
		long nFileSize = ftell(pFile) - 3;
		fseek(pFile, 3, SEEK_SET);

		char* pBuf = new char[nFileSize];
		fread(pBuf, sizeof(char), nFileSize, pFile);
		fclose(pFile);

		CLuaScript * pScript = g_LuaStateMan.GetConfigState()->RegisterBuffer(pFileName, pBuf, nFileSize);
		delete[] pBuf;

		g_LuaStateMan.GetConfigState()->Unlock();
	}
}

// ����lua�к���
void LuaFuncFactory::Call(const char*pnamespace, const char* pfunc, abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	LuaBind::Call(g_LuaStateMan.GetConfigState()->GetVM(), pnamespace, pfunc, args, results);
}

// ���API�ӿ�
void LuaFuncFactory::AddAPI(LuaAPIBase*papi)
{
	m_apis.push_back(papi);
}
// ͳһע������API
void LuaFuncFactory::RegisterAllAPI()
{
	LuaAPI::iterator it = m_apis.begin(),itEnd = m_apis.end();
	for(; it != itEnd; ++it)
	{
		(*it)->Register();
	}
}