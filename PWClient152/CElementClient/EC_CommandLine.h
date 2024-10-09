#pragma once

#include <AAssist.h>
#include <ABaseDef.h>

// Filename	: EC_CommandLine.h
// Creator	: Xu Wenbin
// Date		: 2011/02/22

//
//	类 CECCommandLine: 查询控制台配置属性
//
class CECCommandLine
{
public:
	static bool	GetSupportSeperateFile();
	static bool	GetRenderWhenNoFocus();
	static bool GetDoNotCheckSmallVersion();
	static bool GetLoadFacePillData();
	static bool GetClearAllCoolDown();

	static ACString GetUser();
	static ACString GetPassword();
	static ACString GetArea();
	static ACString GetToken();
	static char		GetTokenType();
	static ACString GetRole();
	static ACString GetAgent();
	static bool		GetEnableGT(bool &bResult);
	static bool		GetEnableArc(bool &bResult);
	static bool		GetEnableArcAsia(bool &bResult);
	static bool		GetExportServerListZoneIDName();
	static bool		GetEnableMiniClient(bool &bResult);
	static bool		GetEnableLuaDebug();
	static bool		GetRtDebugLevel(int &iLevel);
	static bool		GetRtDebugProtocolsToHide(AString &names);
	static bool		GetEnableLogicCheckInfo(bool &bResult);

	static ACString GetStandardConfig(const ACHAR *szConfig);
	static bool	GetBriefConfig(const ACHAR *szConfig);
	static bool GetEnable(const ACHAR *szConfig, bool &bResult);

private:
	static bool IsSeperator(ACHAR c);
	static bool SearchConfig(const ACHAR *szKey, bool &bKeyOnly, ACString &strValue);
	static bool SearchNextItem(const ACHAR *pCmd, bool bColonAsSeperator, ACString &strItem, const ACHAR * & pCmdNext);
};