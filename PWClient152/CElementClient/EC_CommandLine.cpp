
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
	//	��ȡ��׼��ʽ���������ƣ����ȡuser:a�е�aʱ��szConfigΪuser
	//	Ҫ�� GetCommandLine ���ؽ���� �ո� �� tab ���ֿ��������

	ACString strRet;

	while (true)
	{		
		//	��֤��ƥ��������
		bool bKeyOnly(false);
		ACString strValue;
		if (!SearchConfig(szConfig, bKeyOnly, strValue))
			break;
		
		//	��֤�Ǳ�׼���������������key:value����ʽ���ڣ�
		if (bKeyOnly)
			break;

		//	��ȡ������value
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
	//	��ȡ��׼��ʽ���������ƣ����ȡuser:a�е�aʱ��szConfigΪuser
	//	Ҫ�� GetCommandLine ���ؽ���� �ո� �� tab ���ֿ��������

	bool bRet(false);

	while (true)
	{		
		//	��֤��ƥ��������key
		bool bKeyOnly(false);
		ACString strValue;
		if (!SearchConfig(szConfig, bKeyOnly, strValue))
			break;
		
		//	��֤�Ǽ��������������ֻ��key��
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
	//	��������Ϊ szKey ��������
	//	���ҳɹ�ʱ���� true�����У��������� key:value ��ʽ����ʱ��szKey = key��������bKeyOnly = false��strValue = value
	//	����ʧ��ʱ���� false��bKeyOnly �� strValue ֵ����
	//	ע1������������ key��value Ϊ�� key��'key'��"key"��value��'value'��"value"����ʽ���ڣ����� "user":'foo'��������� szKeyΪuser������strValueΪfoo
	//	ע2��������֮��ķָ���Ϊ�ո�Tab��

	bool bRet(false);

	while (true)
	{
		//	��֤�����Ҳ���
		if (!szKey || !szKey[0])
			break;

		//	��֤ CommandLine
		const ACHAR *szCommandLine = GetCommandLine();
		if (!szCommandLine || !szCommandLine[0])
			break;

		//	������֤��ƥ��������
		const ACHAR *pCmd = szCommandLine;
		const ACHAR *pCmdNext = NULL;
		ACString strTempKey, strTempValue;
		bool bTempKeyOnly(false);
		while (SearchNextItem(pCmd, true, strTempKey, pCmdNext))
		{
			//	�ҵ��µ�������key����֤��ʽ����ȡ�����������ݣ����ڷ��ػ�������
			if (!pCmdNext || !(*pCmdNext) || IsSeperator(*pCmdNext))
			{
				//	�Ѿ�����������ĩβ�����ߵ���������߽�

				//	value ֵΪ��
				strTempValue.Empty();
				bTempKeyOnly = true;

				//	��һ�β���λ�ü�Ϊ��������ĩβ
				pCmd = pCmdNext;
				pCmdNext = NULL;
			}
			else
			{
				//	�����Ǳ�׼������key:value ������������ "m"k��"m"k:"value" ��

				if (*pCmdNext != ':')
				{
					//	�������SearchNextItem ���������£�ֱ�ӷ��� false
					ASSERT(false);
					a_LogOutput(1, "CECCommandLine::SearchConfig(), Unexpected parse result: cmdline = \"%s\", key = \"%s\"", AC2AS(szCommandLine), AC2AS(strTempKey));
					return false;
				}

				//	��ȷ���������':'��ʼ���� value ֵ
				bTempKeyOnly = false;
				if (!(*(pCmdNext+1)) || IsSeperator(*(pCmdNext+1)))
				{
					//	value ֵΪ��
					strTempValue.Empty();

					//	������һ�β���λ��
					pCmd = pCmdNext+1;
					pCmdNext = NULL;
				}
				else
				{
					//	value ��Ϊ��
					pCmd = pCmdNext+1;
					pCmdNext = NULL;
					bool bTemp = SearchNextItem(pCmd, false, strTempValue, pCmdNext);
					if (!bTemp)
					{
						//	�������SearchNextItem ���������£�ֱ�ӷ��� false
						ASSERT(false);
						a_LogOutput(1, "CECCommandLine::SearchConfig(), Unexpected parse result: cmdline = \"%s\", pCmd = \"%s\"", AC2AS(szCommandLine), AC2AS(pCmd));
						return false;
					}

					//	value �ɹ����ң�������һ�β���λ��
					pCmd = pCmdNext;
					pCmdNext = NULL;
				}
			}

			//	��ǰ����������ȷ��ȡ������ƥ��
			if (strTempKey != szKey)
				continue;

			//	ƥ��ɹ������÷���ֵ
			bKeyOnly = bTempKeyOnly;
			if (bKeyOnly) strValue.Empty();
			else strValue = strTempValue;
			bRet = true;
			break;
		}

		//	���ҽ���Ѿ���ȷ��ֵ������ѭ������
		break;
	}

	return bRet;
}

bool CECCommandLine::SearchNextItem(const ACHAR *pCmd, bool bColonAsSeperator, ACString &strItem, const ACHAR * & pCmdNext)
{
	//	������һ����˫���� "" ������ '' ��������ָ���(���� bColonAsSeperator Ϊtrueʱ)�����׼��������key��value�ָ����ֿ�����
	//	pCmd Ϊ�������ַ�����ʵ��ʹ����Ϊ CommandLine ��һ����
	//	bColonAsSeperator Ϊtrueʱ�����ڲ����������key������value�ָ�����Ϊð�ţ�Ϊfalseʱ�����ڲ����������value�����е�ð��Ϊ��ͨ�ַ�
	//	���ҳɹ�ʱ���� true��strItem ���طǿյ� key ��pCmdNext ָ����һ������λ�ã�
	//	��ȷ���ܱ��ɹ����ҵĸ�ʽΪ [key | 'key' | "key"][�ָ��� | ����]�����У�key �в����������Ż�˫���š�'key'�в����������š�"key"�в�����˫���ţ��ָ�������������ָ������ո�Tab������������key��value֮��ķָ�����ð�ţ��� bColonAsSeperator Ϊ true ��Ч��
	//	ע1��������̻�������ʽ���������ǿ³����

	bool bRet(false);

	while (true)
	{
		if (!pCmd)
			break;

		ACHAR cQuote = 0;		//	��¼��ǰƥ�������ʼģʽ��ֵΪ"ʱ��ʾ��"��ʼ��Ϊ'��ʾ��'��ʼ������Ϊ��ͨģʽ
		ACHAR cQuote1 = '\'';
		ACHAR cQuote2 = '\"';

		//	������Ҫ�����Ƿ�����������Ҫ�ټ�һ��ѭ��
		while (true)
		{
			//	������ͷ������ָ���
			while (*pCmd && IsSeperator(*pCmd))
				pCmd ++;
			
			if (!*pCmd)
			{
				//	�Ѿ����ҵ��ַ���ĩβ
				break;
			}
			
			//	�����һ���ַ�����������������ȷ��ƥ��ģʽ
			strItem.Empty();
			ACHAR c = *pCmd++;
			cQuote = (c == cQuote2 ? c : (c == cQuote1 ? c : 0));
			if (!cQuote)
			{
				strItem += c;

				//	����Ƿ��к����ַ�

				if (!*pCmd)
				{
					//	û�к����ַ����� console:1 ���������ñ�����
					bRet = true;
					pCmdNext = pCmd;
					break;
				}
			}
			
			//	��ʼ����ƥ��
			while (*pCmd)
			{
				c = *pCmd++;

				//	����ƥ����ĳ�ַ�

				if (cQuote)
				{
					//	����ģʽ��ͷ��Ҫ��һ����ͬһ���Ž���

					if (c != cQuote)
					{
						strItem += c;
						continue;
					}

					//	����ƥ����ţ�������ĸ�ʽ�Ƿ���ȷ������ȷ������

					if (*pCmd && !IsSeperator(*pCmd) && (!bColonAsSeperator || *pCmd!=':') )
					{
						//	���ǽ�������key��value�ָ�������������ָ�����Ϊ�Ƿ���ʽ

						//	�����Ƿ���ʽ
						while (*pCmd && !IsSeperator(*pCmd))
							pCmd ++;

						//	����������������ѭ������������������ָ�����������ǰ�Ƿ���ʽ������һ�β��ң�
					}

					//	����ĸ�ʽ��ȷ����������ѭ������
					bRet = true;
					pCmdNext = pCmd;
					break;
				}

				//	�����ſ�ͷģʽ����key��value�ָ�����������ָ�������

				if (!IsSeperator(c) && (!bColonAsSeperator || c != ':'))
				{
					strItem += c;
					if (*pCmd)
					{
						//	������������ʱ
						continue;
					}

					//	�������������ҳɹ�
					bRet = true;
					pCmdNext = pCmd;
					break;
				}

				//	�����ָ��������ҳɹ�����������ѭ������
				bRet = true;
				pCmdNext = pCmd-1;
				break;
			}
			if (bRet)
			{
				//	�ɹ����ң���������ѭ������
				break;
			}

			//	ʧ�ܣ���������
		}

		break;
	}

	return bRet;
}