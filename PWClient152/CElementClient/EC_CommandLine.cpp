
// Filename	: EC_CommandLine.cpp
// Creator	: Xu Wenbin
// Date		: 2011/02/22

#include "EC_CommandLine.h"
#include <AChar.h>
#include <ALog.h>
#include <windows.h>

bool CECCommandLine::IsSeperator(ACHAR c)
{
	return (c == ' ' || c == '\t');
}

ACString CECCommandLine::GetUser()
{
	return GetStandardConfig(_AL("user"));
}

ACString CECCommandLine::GetPassword()
{
	return GetStandardConfig(_AL("pwd"));
}

ACString CECCommandLine::GetArea()
{
	return GetStandardConfig(_AL("area"));
}

ACString CECCommandLine::GetToken()
{
	ACString strToken = GetStandardConfig(_AL("token"));
	if (strToken.IsEmpty())
		strToken = GetStandardConfig(_AL("token2"));
	return strToken;
}

char CECCommandLine::GetTokenType()
{
	char type = 0;
	if (!GetStandardConfig(_AL("token")).IsEmpty())
		type = 1;
	else if (!GetStandardConfig(_AL("token2")).IsEmpty())
		type = 2;
	return type;
}

ACString CECCommandLine::GetRole()
{
	return GetStandardConfig(_AL("role"));
}

ACString CECCommandLine::GetAgent()
{
	return GetStandardConfig(_AL("agent"));
}

bool CECCommandLine::GetEnableGT(bool &bResult)
{
	return GetEnable(_AL("gt"), bResult);
}

bool CECCommandLine::GetEnableArc(bool &bResult)
{
	return GetEnable(_AL("coreclient"), bResult);
}

bool CECCommandLine::GetEnableArcAsia(bool &bResult)
{
	return GetEnable(_AL("arcasia"), bResult);
}

bool CECCommandLine::GetExportServerListZoneIDName()
{
	return GetBriefConfig(_AL("exportserver"));
}

bool CECCommandLine::GetEnableMiniClient(bool &bResult)
{
	return GetEnable(_AL("miniclient"), bResult);
}

bool CECCommandLine::GetEnableLogicCheckInfo(bool &bResult)
{
	return GetEnable(_AL("logiccheck"), bResult);
}

bool CECCommandLine::GetEnableLuaDebug()
{
	return GetBriefConfig(_AL("luadebug"));
}

bool CECCommandLine::GetRtDebugLevel(int &iLevel)
{
	ACString str = GetStandardConfig(_AL("rtdebug"));
	if (str.IsEmpty()) return false;
	iLevel = str.ToInt();
	return true;
}

bool CECCommandLine::GetRtDebugProtocolsToHide(AString &names)
{
	ACString str = GetStandardConfig(_AL("rtdebug_hide"));
	if (str.IsEmpty()) return false;
	names = AC2AS(str);
	return true;
}

ACString CECCommandLine::GetStandardConfig(const ACHAR *szConfig)
{
	//	获取标准格式的配置名称，如获取user:a中的a时，szConfig为user
	//	要求 GetCommandLine 返回结果以 空格 或 tab 键分开多个配置

	ACString strRet;

	while (true)
	{		
		//	验证并匹配配置项
		bool bKeyOnly(false);
		ACString strValue;
		if (!SearchConfig(szConfig, bKeyOnly, strValue))
			break;
		
		//	验证是标准配置项（即配置项以key:value的形式存在）
		if (bKeyOnly)
			break;

		//	获取配置项value
		strRet = strValue;

		break;
	}

	return strRet;
}

bool CECCommandLine::GetSupportSeperateFile()
{
	return GetBriefConfig(_AL("sepfile"));
}

bool CECCommandLine::GetRenderWhenNoFocus()
{
	return GetBriefConfig(_AL("rendernofocus"));
}

bool CECCommandLine::GetDoNotCheckSmallVersion()
{
	return GetBriefConfig(_AL("nocheck"));
}

bool CECCommandLine::GetLoadFacePillData()
{
	return GetBriefConfig(_AL("facepill"));
}

bool CECCommandLine::GetClearAllCoolDown()
{
	return GetBriefConfig(_AL("nocooldown"));
}

bool CECCommandLine::GetBriefConfig(const ACHAR *szConfig)
{
	//	获取标准格式的配置名称，如获取user:a中的a时，szConfig为user
	//	要求 GetCommandLine 返回结果以 空格 或 tab 键分开多个配置

	bool bRet(false);

	while (true)
	{		
		//	验证并匹配配置项key
		bool bKeyOnly(false);
		ACString strValue;
		if (!SearchConfig(szConfig, bKeyOnly, strValue))
			break;
		
		//	验证是简单配置项（即配置项只有key）
		if (!bKeyOnly)
			break;

		bRet = true;
		break;
	}

	return bRet;
}

bool CECCommandLine::GetEnable(const ACHAR *szConfig, bool &bResult)
{
	ACString str = GetStandardConfig(szConfig);
	if (str.IsEmpty()) return false;
	int nEnable = str.ToInt();
	bResult = (nEnable != 0);
	return true;
}

bool CECCommandLine::SearchConfig(const ACHAR *szKey, bool &bKeyOnly, ACString &strValue)
{
	//	查找名称为 szKey 的配置项
	//	查找成功时返回 true，其中，配置项以 key:value 形式存在时（szKey = key），返回bKeyOnly = false，strValue = value
	//	查找失败时返回 false，bKeyOnly 及 strValue 值不变
	//	注1：允许配置项 key、value 为以 key、'key'、"key"，value、'value'、"value"等形式存在，例如 "user":'foo'，输入参数 szKey为user，返回strValue为foo
	//	注2：配置项之间的分隔符为空格、Tab键

	bool bRet(false);

	while (true)
	{
		//	验证待查找参数
		if (!szKey || !szKey[0])
			break;

		//	验证 CommandLine
		const ACHAR *szCommandLine = GetCommandLine();
		if (!szCommandLine || !szCommandLine[0])
			break;

		//	查找验证并匹配配置项
		const ACHAR *pCmd = szCommandLine;
		const ACHAR *pCmdNext = NULL;
		ACString strTempKey, strTempValue;
		bool bTempKeyOnly(false);
		while (SearchNextItem(pCmd, true, strTempKey, pCmdNext))
		{
			//	找到新的配置项key，验证格式并获取完整配置内容（用于返回或跳过）
			if (!pCmdNext || !(*pCmdNext) || IsSeperator(*pCmdNext))
			{
				//	已经是配置内容末尾、或者到达配置项边界

				//	value 值为空
				strTempValue.Empty();
				bTempKeyOnly = true;

				//	下一次查找位置即为此配置项末尾
				pCmd = pCmdNext;
				pCmdNext = NULL;
			}
			else
			{
				//	可能是标准配置项key:value 或错误的配置项 "m"k、"m"k:"value" 等

				if (*pCmdNext != ':')
				{
					//	程序错误，SearchNextItem 处理不当导致，直接返回 false
					ASSERT(false);
					a_LogOutput(1, "CECCommandLine::SearchConfig(), Unexpected parse result: cmdline = \"%s\", key = \"%s\"", AC2AS(szCommandLine), AC2AS(strTempKey));
					return false;
				}

				//	正确的配置项，从':'后开始查找 value 值
				bTempKeyOnly = false;
				if (!(*(pCmdNext+1)) || IsSeperator(*(pCmdNext+1)))
				{
					//	value 值为空
					strTempValue.Empty();

					//	设置下一次查找位置
					pCmd = pCmdNext+1;
					pCmdNext = NULL;
				}
				else
				{
					//	value 不为空
					pCmd = pCmdNext+1;
					pCmdNext = NULL;
					bool bTemp = SearchNextItem(pCmd, false, strTempValue, pCmdNext);
					if (!bTemp)
					{
						//	程序错误，SearchNextItem 处理不当导致，直接返回 false
						ASSERT(false);
						a_LogOutput(1, "CECCommandLine::SearchConfig(), Unexpected parse result: cmdline = \"%s\", pCmd = \"%s\"", AC2AS(szCommandLine), AC2AS(pCmd));
						return false;
					}

					//	value 成功查找，设置下一次查找位置
					pCmd = pCmdNext;
					pCmdNext = NULL;
				}
			}

			//	当前配置项已正确获取，尝试匹配
			if (strTempKey != szKey)
				continue;

			//	匹配成功，设置返回值
			bKeyOnly = bTempKeyOnly;
			if (bKeyOnly) strValue.Empty();
			else strValue = strTempValue;
			bRet = true;
			break;
		}

		//	查找结果已经正确赋值，跳出循环即可
		break;
	}

	return bRet;
}

bool CECCommandLine::SearchNextItem(const ACHAR *pCmd, bool bColonAsSeperator, ACString &strItem, const ACHAR * & pCmdNext)
{
	//	查找下一个被双引号 "" 或单引号 '' 或被配置项分隔符(参数 bColonAsSeperator 为true时)、或标准配置项内key、value分隔符分开的项
	//	pCmd 为待查找字符串，实际使用中为 CommandLine 的一部分
	//	bColonAsSeperator 为true时，用于查找配置项的key，其与value分隔符即为冒号；为false时，用于查找配置项的value，其中的冒号为普通字符
	//	查找成功时返回 true，strItem 返回非空的 key ，pCmdNext 指下向一个查找位置；
	//	正确并能被成功查找的格式为 [key | 'key' | "key"][分隔符 | 结束]，其中，key 中不包含单引号或双引号、'key'中不包含单引号、"key"中不包括双引号；分隔符包含配置项分隔符（空格、Tab）、配置项内key和value之间的分隔符（冒号，在 bColonAsSeperator 为 true 有效）
	//	注1：处理过程会跳过格式错误项，以增强鲁棒性

	bool bRet(false);

	while (true)
	{
		if (!pCmd)
			break;

		ACHAR cQuote = 0;		//	记录当前匹配项的起始模式：值为"时表示以"开始、为'表示以'开始、否则为普通模式
		ACHAR cQuote1 = '\'';
		ACHAR cQuote2 = '\"';

		//	可能需要跳过非法配置项，因此需要再加一层循环
		while (true)
		{
			//	跳过开头配置项分隔符
			while (*pCmd && IsSeperator(*pCmd))
				pCmd ++;
			
			if (!*pCmd)
			{
				//	已经查找到字符串末尾
				break;
			}
			
			//	处理第一个字符，检查引号情况、并确定匹配模式
			strItem.Empty();
			ACHAR c = *pCmd++;
			cQuote = (c == cQuote2 ? c : (c == cQuote1 ? c : 0));
			if (!cQuote)
			{
				strItem += c;

				//	检查是否有后续字符

				if (!*pCmd)
				{
					//	没有后续字符，如 console:1 出现在配置表的最后
					bRet = true;
					pCmdNext = pCmd;
					break;
				}
			}
			
			//	开始后续匹配
			while (*pCmd)
			{
				c = *pCmd++;

				//	处理匹配中某字符

				if (cQuote)
				{
					//	引号模式开头，要求一定以同一符号结束

					if (c != cQuote)
					{
						strItem += c;
						continue;
					}

					//	遇到匹配符号，检查后面的格式是否正确，不正确则跳过

					if (*pCmd && !IsSeperator(*pCmd) && (!bColonAsSeperator || *pCmd!=':') )
					{
						//	不是结束、或key和value分隔符、或配置项分隔符，为非法格式

						//	跳过非法格式
						while (*pCmd && !IsSeperator(*pCmd))
							pCmd ++;

						//	遇到结束（将跳出循环）、或遇到配置项分隔符（跳过当前非法格式进行下一次查找）
					}

					//	后面的格式正确，跳出所有循环返回
					bRet = true;
					pCmdNext = pCmd;
					break;
				}

				//	非引号开头模式，以key和value分隔符或配置项分隔符结束

				if (!IsSeperator(c) && (!bColonAsSeperator || c != ':'))
				{
					strItem += c;
					if (*pCmd)
					{
						//	后续还有内容时
						continue;
					}

					//	解析结束，查找成功
					bRet = true;
					pCmdNext = pCmd;
					break;
				}

				//	遇到分隔符，查找成功，跳出所有循环返回
				bRet = true;
				pCmdNext = pCmd-1;
				break;
			}
			if (bRet)
			{
				//	成功查找，跳出所有循环返回
				break;
			}

			//	失败，继续尝试
		}

		break;
	}

	return bRet;
}