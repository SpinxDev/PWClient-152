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
	//	Ĭ�ϸ�������¶���˵��������Ϊ1��
	for (int i = 0; i < TW_MAX; ++ i)
		m_wordsProp[i] = 1.0f;

	//	������г�����������
	m_words.clear();
}

bool CECPetWords::LoadWords(const char *szFile)
{
	//	���س�������Ҫ˵�Ļ�

	bool bRet(false);

	//	�������
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
	//	�����龰�ͳ���ID����ѯ�Ƿ��л�Ҫ˵������NULL��ʾû�У�����β�˵�����ʿ��ƣ�

	const ACHAR *szRet = NULL;

	WordsID w;
	w.id = idPet;
	w.type = type;
	
	WordsMap::iterator it = m_words.find(w);
	if (it != m_words.end())
	{
		//	�����л�˵

		//	������ʣ��ж��Ƿ��˵��
		float f = a_Random(0.0f, 1.0f);
		if (f <= m_wordsProp[type])
		{
			//	����˵�����������ѡһ�仰
			int i = a_Random() % it->second.size();
			szRet = it->second[i];
		}
	}

	return szRet;
}

typedef abase::hash_map<ACString, CECPetWords::TYPE_WORDS> TypeMaps;	//	���ļ��е� summon ת��Ϊ TW_SUMMON ���͵�ӳ���
void InitTypeMaps(TypeMaps &typeMaps)
{
	//	��������ת��ӳ���

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
	//	��ȡ�������ݣ�����ʱ˵ĳ�仰�ĸ�������г���һ����
	//

	bool bRet(false);
	
	while (true)
	{
		//	��ȡ��������ͷ
		wchar_t * szTypeHead = L"[TYPE]";
		wchar_t * szListHead = L"[LIST]";

		if (!ScriptFile.MatchToken(szTypeHead, true))
		{
			AString strTemp = AC2AS(szTypeHead);
			a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, type head mark \"%s\" missed", strTemp);
			break;
		}

		//	��ʼ������ת�����飬���ļ��е� summon ת��Ϊ TW_SUMMON ���͵�ӳ���
		TypeMaps typeMaps;
		InitTypeMaps(typeMaps);
		
		//	��ȡ��������
		while (ScriptFile.PeekNextToken(true))
		{
			//	��������˵���б���Ϊ��ֹ������ʹ�� Peek ������ Get��
			if (!a_strcmp(ScriptFile.m_szToken, szListHead))
				break;

			//	��ȡ������Ƿ�Ϊ��Ч�ַ�
			ScriptFile.GetNextToken(true);

			TypeMaps::iterator it = typeMaps.find(ScriptFile.m_szToken);
			if (it == typeMaps.end())
			{
				//	������Ч�У�������ע�ͻ���У����Ӵ���
				ScriptFile.SkipLine();
				continue;
			}

			//	���Դӱ��к����ַ�����ȡ����
			float fVal = 0.0f;
			if (!ScriptFile.GetNextToken(false) ||
				swscanf(ScriptFile.m_szToken, L"%f", &fVal) != 1 ||
				fVal < 0.0f || fVal > 1.0f)
			{
				//	����û�������ַ������ڷǷ���
				a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, invalid probability at line %d", ScriptFile.GetCurLine());

				//	���ӱ��У����������еķ���
				ScriptFile.SkipLine();
				continue;
			}

			//	�ɹ���Ӹ���
			m_wordsProp[it->second] = fVal;

			//	���ӱ�������˵��������
			ScriptFile.SkipLine();

			//	������һ��
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
		//	��ȡ��������ͷ
		wchar_t * szListHead = L"[LIST]";
		
		if (!ScriptFile.MatchToken(szListHead, true))
		{
			a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, \"%s\" missed", szListHead);
			break;
		}

		//	��ʼ������ת�����飬���ļ��е� summon ת��Ϊ TW_SUMMON ���͵�ӳ���
		TypeMaps typeMaps;
		InitTypeMaps(typeMaps);

		WordsID w;

		//	��ȡ��������
		while (ScriptFile.GetNextToken(true))
		{
			//	�Ȼ�ȡ����ID
			int idPet(0);
			if (swscanf(ScriptFile.m_szToken, L"%d", &idPet) != 1)
			{
				//	������ע���л����
				ScriptFile.SkipLine();
				continue;
			}
			if (idPet <= 0)
			{
				//	����ID�Ƿ���������Ч��
				a_LogOutput(1, "CECPetWords::LoadWords(), File Content error, invalid pet id(%d) at line %d", idPet, ScriptFile.GetCurLine());

				//	���ӱ��У����������еķ���
				ScriptFile.SkipLine();
				continue;
			}

			//	�ٻ�ȡ���к���ĺ���ʱ��
			if (!ScriptFile.GetNextToken(false))
			{
				//	�޷���ȡ˵��ʱ����������Ч��
				a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, incomplete format at line %d", ScriptFile.GetCurLine());

				//	���ӱ��У����������еķ���
				ScriptFile.SkipLine();
				continue;
			}
			
			TypeMaps::iterator it = typeMaps.find(ScriptFile.m_szToken);
			if (it == typeMaps.end())
			{
				//	�������Ͳ���ʶ����Ч��
				AString strToken = AC2AS(ScriptFile.m_szToken);
				a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, Unknown words type string \"%s\" at line %d", strToken, ScriptFile.GetCurLine());

				//	���ӱ��У����������еķ���
				ScriptFile.SkipLine();
				continue;
			}

			//	����ȡ����ĺ�������
			if (!ScriptFile.GetNextToken(false))
			{
				//	û�к������ݣ�������Ч��
				a_LogOutput(1, "CECPetWords::LoadWords(), File Format error, pet said NO words at line %d", ScriptFile.GetCurLine());

				//	���ӱ��У����������еķ���
				ScriptFile.SkipLine();
				continue;
			}

			//	���Խ�����������ӵ�����������
			w.id = idPet;
			w.type = it->second;
			WordsMap::iterator it2 = m_words.find(w);
			if (it2 != m_words.end())
			{
				//	ͬһIDͬһ���ͣ������ж�仰�������ó��� MAX_WORDSCOUNT
				if (it2->second.size() >= MAX_WORDSCOUNT)
				{
					//	�����޶���
					a_LogOutput(1, "CECPetWords::LoadWords(), The pet already has enough words to say, Words ignored (id = %d) at line %d", idPet, ScriptFile.GetCurLine());
					
					//	���ӱ��У����������еķ���
					ScriptFile.SkipLine();
					continue;
				}
			}
			m_words[w].push_back(ScriptFile.m_szToken);

			//	���ӱ����������ݣ�����еĻ���
			ScriptFile.SkipLine();

			//	������һ��
		}

		bRet = true;
		break;
	}
	return bRet;
}