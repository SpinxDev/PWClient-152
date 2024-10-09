// Filename	: EC_PetWords.cpp
// Creator	: Xu Wenbin
// Date		: 2010/11/04

#include "EC_PetWords.h"
#include <AFI.h>
#include <limits.h>
#include <ALog.h>
#include <AChar.h>

CECPetWords::CECPetWords()
{
	Reset();
}

void CECPetWords::Reset()
{
	//	默认各种情况下都会说话（概率为1）
	for (int i = 0; i < TW_MAX; ++ i)
		m_wordsProp[i] = 1.0f;

	//	清除所有宠物聊天内容
	m_words.clear();
}

bool CECPetWords::LoadWords(const char *szFile)
{
	//	加载宠物所有要说的话

	bool bRet(false);

	//	重置清除
	Reset();
	
	AWScriptFile ScriptFile;
	while (af_IsFileExist(szFile))
	{
		if (!ScriptFile.Open(szFile))
		{
			a_LogOutput(1, "CECPetWords::LoadWords(), Failed to open file %s", szFile);
			break;
		}

		if (!ReadType(ScriptFile))
			break;

		if (!ReadContent(ScriptFile))
			break;

		bRet = true;
		break;
	}
	ScriptFile.Close();

	return bRet;
}

const ACHAR * CECPetWords::GetWords(int idPet, TYPE_WORDS type)
{
	//	根据情景和宠物ID，查询是否有话要说，返回NULL表示没有，或这次不说（概率控制）

	const ACHAR *szRet = NULL;

	WordsID w;
	w.id = idPet;
	w.type = type;
	
	WordsMap::iterator it = m_words.find(w);
	if (it != m_words.end())
	{
		//	可能有话说

		//	计算概率，判断是否该说话
		float f = a_Random(0.0f, 1.0f);
		if (f <= m_wordsProp[type])
		{
			//	可以说话，从中随机选一句话
			int i = a_Random() % it->second.size();
			szRet = it->second[i];
		}
	}

	return szRet;
}

typedef abase::hash_map<ACString, CECPetWords::TYPE_WORDS> TypeMaps;	//	将文件中的 summon 转换为 TW_SUMMON 类型的映射表
void InitTypeMaps(TypeMaps &typeMaps)
{
	//	构建类型转换映射表

	typeMaps.clear();

	typeMaps[_AL("summon")]		= CECPetWords::TW_SUMMON;
	typeMaps[_AL("rest")]		= CECPetWords::TW_REST;
	typeMaps[_AL("passive")]	= CECPetWords::TW_PASSIVE;
	typeMaps[_AL("defensive")]	= CECPetWords::TW_DEFENSIVE;
	typeMaps[_AL("offensive")]	= CECPetWords::TW_OFFENSIVE;
	typeMaps[_AL("stop")]		= CECPetWords::TW_STOP;
	typeMaps[_AL("follow")]		= CECPetWords::TW_FOLLOW;
	typeMaps[_AL("fight")]		= CECPetWords::TW_FIGHT;
	typeMaps[_AL("recall")]		= CECPetWords::TW_RECALL;
	typeMaps[_AL("dead")]		= CECPetWords::TW_DEAD;
	typeMaps[_AL("disappear")]	= CECPetWords::TW_DISAPPEAR;	
	typeMaps[_AL("sacrifice")]	= CECPetWords::TW_SACRIFICE;
}

bool CECPetWords::ReadType(AWScriptFile &ScriptFile)
{
	//	读取概率数据（出生时说某句话的概念，对所有宠物一样）
	//

	bool bRet(false);
	
	while (true)
	{
		//	读取概率数据头
		wchar_t * szTypeHead = L"[TYPE]";
		wchar_t * szListHead = L"[LIST]";

		if (!ScriptFile.MatchToken(szTypeHead, true))
		{
			AString strTemp = AC2AS(szTypeHead);
			a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, type head mark \"%s\" missed", strTemp);
			break;
		}

		//	初始化类型转换数组，将文件中的 summon 转换为 TW_SUMMON 类型的映射表
		TypeMaps typeMaps;
		InitTypeMaps(typeMaps);
		
		//	读取概率数据
		while (ScriptFile.PeekNextToken(true))
		{
			//	遇到宠物说话列表则为终止条件（使用 Peek 而不能 Get）
			if (!a_strcmp(ScriptFile.m_szToken, szListHead))
				break;

			//	获取并检查是否为有效字符
			ScriptFile.GetNextToken(true);

			TypeMaps::iterator it = typeMaps.find(ScriptFile.m_szToken);
			if (it == typeMaps.end())
			{
				//	不是有效行，可能是注释或空行，忽视此行
				ScriptFile.SkipLine();
				continue;
			}

			//	尝试从本行后面字符中提取概率
			float fVal = 0.0f;
			if (!ScriptFile.GetNextToken(false) ||
				swscanf(ScriptFile.m_szToken, L"%f", &fVal) != 1 ||
				fVal < 0.0f || fVal > 1.0f)
			{
				//	本行没有其它字符，属于非法行
				a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, invalid probability at line %d", ScriptFile.GetCurLine());

				//	忽视本行，继续其它行的分析
				ScriptFile.SkipLine();
				continue;
			}

			//	成功添加概率
			m_wordsProp[it->second] = fVal;

			//	忽视本行其它说明性内容
			ScriptFile.SkipLine();

			//	继续下一行
		}

		bRet = true;
		break;

	}
	return bRet;
}

bool CECPetWords::ReadContent(AWScriptFile &ScriptFile)
{
	bool bRet(false);

	while (true)
	{
		//	读取喊话数据头
		wchar_t * szListHead = L"[LIST]";
		
		if (!ScriptFile.MatchToken(szListHead, true))
		{
			a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, \"%s\" missed", szListHead);
			break;
		}

		//	初始化类型转换数组，将文件中的 summon 转换为 TW_SUMMON 类型的映射表
		TypeMaps typeMaps;
		InitTypeMaps(typeMaps);

		WordsID w;

		//	读取喊话数据
		while (ScriptFile.GetNextToken(true))
		{
			//	先获取宠物ID
			int idPet(0);
			if (swscanf(ScriptFile.m_szToken, L"%d", &idPet) != 1)
			{
				//	本行是注释行或空行
				ScriptFile.SkipLine();
				continue;
			}
			if (idPet <= 0)
			{
				//	宠物ID非法，属于无效行
				a_LogOutput(1, "CECPetWords::LoadWords(), File Content error, invalid pet id(%d) at line %d", idPet, ScriptFile.GetCurLine());

				//	忽视本行，继续其它行的分析
				ScriptFile.SkipLine();
				continue;
			}

			//	再获取本行后面的喊话时机
			if (!ScriptFile.GetNextToken(false))
			{
				//	无法获取说话时机，属于无效行
				a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, incomplete format at line %d", ScriptFile.GetCurLine());

				//	忽视本行，继续其它行的分析
				ScriptFile.SkipLine();
				continue;
			}
			
			TypeMaps::iterator it = typeMaps.find(ScriptFile.m_szToken);
			if (it == typeMaps.end())
			{
				//	喊话类型不认识，无效行
				AString strToken = AC2AS(ScriptFile.m_szToken);
				a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, Unknown words type string \"%s\" at line %d", strToken, ScriptFile.GetCurLine());

				//	忽视本行，继续其它行的分析
				ScriptFile.SkipLine();
				continue;
			}

			//	最后获取后面的喊话内容
			if (!ScriptFile.GetNextToken(false))
			{
				//	没有喊话内容，属于无效行
				a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, pet said NO words at line %d", ScriptFile.GetCurLine());

				//	忽视本行，继续其它行的分析
				ScriptFile.SkipLine();
				continue;
			}

			//	尝试将本行内容添加到喊话队列中
			w.id = idPet;
			w.type = it->second;
			WordsMap::iterator it2 = m_words.find(w);
			if (it2 != m_words.end())
			{
				//	同一ID同一类型，可以有多句话，但不得超过 MAX_WORDSCOUNT
				if (it2->second.size() >= MAX_WORDSCOUNT)
				{
					//	超出限定数
					a_LogOutput(1, "CECPetWords::LoadWords(), The pet already has enough words to say, Words ignored (id = %d) at line %d", idPet, ScriptFile.GetCurLine());
					
					//	忽视本行，继续其它行的分析
					ScriptFile.SkipLine();
					continue;
				}
			}
			m_words[w].push_back(ScriptFile.m_szToken);

			//	忽视本行其它内容（如果有的话）
			ScriptFile.SkipLine();

			//	继续下一行
		}

		bRet = true;
		break;
	}
	return bRet;
}