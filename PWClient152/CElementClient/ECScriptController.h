/********************************************************************
	created:	2005/09/05
	created:	5:9:2005   16:56
	file name:	ECScriptController.h
	author:		yaojun
	
	purpose:	controller to init and run scripts
*********************************************************************/


#pragma once

#include <vector.h>

class AString;
class CECScript;
class CECScriptContext;

typedef unsigned long DWORD;

typedef abase::vector<CECScript *> CScriptArray;
typedef CScriptArray::iterator CScriptArrayIterator;

class CECScriptController
{
private:
	CScriptArray m_vecScript;
	CECScriptContext * m_pContext;
	CECScript * m_pCurrentScript;
	bool m_bNewScriptRegisteredToOption;
private:
	typedef abase::vector<AString> CAStringArray;
	bool LoadScript(AString strFullPath);
	bool LoadScriptList(AString strFullPath, CAStringArray &vecFileList);
public:
	CECScriptController();
	~CECScriptController();
	bool Init(CECScriptContext *pContext, AString strRelativePathOfScript);
	void TickRunScript(DWORD dwTickTime);
	void TickRunScript(DWORD dwTickTime,const abase::vector<int>* pnIDS /*是否强制显示该帮助*/);
	void FinishCurrentScript(CECScript* pNewScript);
	CECScript * GetScriptFromID(int nScriptID);
	void RegisterNewScriptToOption();
	bool IsScriptLoaded(int nScriptID);
	CScriptArray & GetScriptArray();
	void ForceActiveScript(int nID);
	const CECScript * GetCurrentScript();
};