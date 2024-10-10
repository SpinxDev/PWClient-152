// Filename	: EC_RecordAccount.cpp
// Creator	: XuWenbin
// Date		: 2013/9/6

#include "EC_RecordAccount.h"
#include "EC_UIConfigs.h"
#include "AWStringWithWildcard.h"
#include "base64.h"
#include "EC_Split.h"
#include "EC_ServerList.h"

#include <AScriptFile.h>
#include <AFI.h>
#include <ALog.h>
#include <AChar.h>
#include <AMemory.h>

#include <algorithm>

static bool s_bDefaultRecord = false;


static AString Encode(const ACString &str)
{
	if (!str.IsEmpty()){
		AString strOut;
		int ch_inlen = str.GetLength()*sizeof(ACHAR);
		int ch_outlen = (ch_inlen + 2) / 3 * 4 + 1;
		char *ch_out = strOut.GetBuffer(ch_outlen);
		int  len = base64_encode((unsigned char *)(const ACHAR *)str, ch_inlen, ch_out);
		ch_out[len] = '\0';
		strOut.ReleaseBuffer();
		return strOut;
	}
	return "";
}

static ACString Decode(const AString &str)
{
	if (!str.IsEmpty()){
		ACString strOut;
		int ch_inlen = str.GetLength();
		int ch_outlen = ch_inlen * 3 / 4 + sizeof(ACHAR);
		int wch_outlen = ch_outlen / sizeof(ACHAR);
		ACHAR * wch_out = strOut.GetBuffer(wch_outlen);
		int len = base64_decode((char *)(const char *)str, ch_inlen, (unsigned char *)wch_out);
		wch_out[len/sizeof(ACHAR)] = _AL('\0');
		strOut.ReleaseBuffer();
		return strOut;
	}
	return _AL("");
}

static bool IsValidFormat(const char *szFile)
{
	//	判断文件是否是正确格式
	bool bValid(false);
	FILE *file = fopen(szFile, "rb");
	while (file){
		ACHAR ch;
		if (1 != fread(&ch, sizeof(ch), 1, file)){
			break;
		}
		if (ch == 0xfffe || ch == 0xfeff){
			//	Unicode 格式
			break;
		}
		bValid = true;
		break;
	}
	if (file){
		fclose(file);
	}
	return bValid;
}

AString CECRecordAccount::Record::Serialize()const
{
	AString str = Encode(account);
	if (server >= 0){
		const CECServerList::ServerInfo & s = CECServerList::Instance().GetServer(server);
		ACString strServer;
		strServer.Format(_AL("%d:%s,%s,%d"), s.port_min, AS2AC(s.address), s.server_name, s.line);
		str += " ";
		str += Encode(strServer);
	}
	return str;
}

CECRecordAccount::Record CECRecordAccount::Record::UnSerialize(const AString &str)
{
	CECRecordAccount::Record r;
	if (!str.IsEmpty()){
		CECSplitHelperA ss(str, " ");
		r.account = Decode(ss.ToString(0));
		if (ss.Count() > 1){
			ACString strServer = Decode(ss.ToString(1));
			CECSplitHelperW ss2(strServer, _AL(","));
			if (ss2.Count() == 3){
				r.server = CECServerList::Instance().FindServerByAll(AC2AS(ss2.ToString(0)), ss2.ToString(1), ss2.ToInt(2));
			}
		}
	}
	return r;
}

CECRecordAccount::CECRecordAccount()
: m_bRecord(s_bDefaultRecord)
{
}

CECRecordAccount & CECRecordAccount::Instance()
{
	static CECRecordAccount s_dummy;
	return s_dummy;
}

void CECRecordAccount::SetCanRecord(bool bCan)
{
	if (m_bRecord != bCan){
		m_bRecord = bCan;
		if (!bCan){
			Clear();
		}
	}
}

void CECRecordAccount::Resize(bool bAddNew)
{
	int nMaxRecord = CECUIConfig::Instance().GetLoginUI().nMaxRecordAccount;
	if (bAddNew){
		//	要添加新成员时，多空出一个位置来
		-- nMaxRecord;
	}
	int nCurSize = (int)(m_records.size());
	int nRemove(0);
	if (nMaxRecord <= 0){
		nRemove = nCurSize;
	}else if (nCurSize > nMaxRecord){
		nRemove = nCurSize - nMaxRecord;
	}
	RemoveRecord(nRemove);
}

void CECRecordAccount::Add(const ACHAR *szAccount, int iServer)
{
	if (szAccount && GetCanRecord() && CECUIConfig::Instance().GetLoginUI().nMaxRecordAccount > 0){
		ACString strAccount = szAccount;
		if (!strAccount.IsEmpty()){
			//	若账号已添加过，先删除再添加，可以改变顺序
			m_records.erase(std::remove(m_records.begin(), m_records.end(), strAccount), m_records.end());	//	先从原有列表中删除当前账号（如果有的话）
			Resize(true);
			m_records.push_back(Record(szAccount, iServer));
		}
	}
}

void CECRecordAccount::Clear()
{
	m_records.clear();
	Save();	//	清除后立马保存，以达到立马清除本地文件的目的
}

bool CECRecordAccount::Complete(const ACHAR *szAccount, Records &candidates)
{
	candidates.clear();
	if (szAccount){
		ACString strSearch = szAccount;
		if (!strSearch.IsEmpty()){
			if (strSearch.GetLength() >= CECUIConfig::Instance().GetLoginUI().nMinInputToRemindAccount){
				ACString strPattern = strSearch + _AL("*");
				ACStringWithWildcard matcher;
				for (Records::iterator it = m_records.begin(); it != m_records.end(); ++ it)
				{
					if (matcher.IsMatch(it->account, strPattern)){
						candidates.push_back(*it);
					}
				}
				if (candidates.size() > 1){
					std::sort(candidates.begin(), candidates.end());
				}
			}
		}
	}
	return !candidates.empty();
}

const CECRecordAccount::Record * CECRecordAccount::FindRecord(const ACHAR *szAccount)const
{
	Records::const_iterator cit = std::find(m_records.begin(), m_records.end(), szAccount);
	return cit == m_records.end() ? NULL : &*cit;
}

const CECRecordAccount::Record * CECRecordAccount::GetLatestRecord()const
{
	return m_records.empty() ? NULL : &m_records.back();
}

void CECRecordAccount::RemoveRecord(int count)
{
	//	从当前记录中删除 count 个存在时间最长的 record
	if (count <= 0){
		return;
	}
	if (count >= (int)m_records.size()){
		m_records.clear();
		return;
	}
	m_records.erase(m_records.begin(), m_records.begin()+count);
}

void CECRecordAccount::Load()
{
	//	Reset First
	m_bRecord = s_bDefaultRecord;
	m_records.clear();

	AString strFile;
	strFile.Format("%s\\userdata\\accounts.txt", af_GetBaseDir());
	if (!af_IsFileExist(strFile)){
		return;
	}
	if (!IsValidFormat(strFile)){
		return;
	}
	AScriptFile file;
	if (!file.Open(strFile)){
		a_LogOutput(1, "CECRecordAccount::Load, Failed to open %s", strFile);
		return;
	}
	if (file.GetNextToken(true)){
		if (!stricmp(file.m_szToken, "true")){
			m_bRecord = true;
		}else if (!stricmp(file.m_szToken, "false")){
			m_bRecord = false;
		}else{
			a_LogOutput(1, "CECRecordAccount::Load, Invalid format for first token %s", file.m_szToken);
			file.Close();
			return;
		}
	}
	AString str;
	while (file.GetNextToken(true))
	{
		str = file.m_szToken;
		while (file.GetNextToken(false))
		{
			str += " ";
			str += file.m_szToken;
		}
		if (!str.IsEmpty()){
			Record r = Record::UnSerialize(str);
			if (r.IsValid()){
				m_records.push_back(r);
			}
		}
	}
	file.Close();
	Resize(false);
}

void CECRecordAccount::Save()
{
	AString strFile;
	strFile.Format("%s\\userdata\\accounts.txt", af_GetBaseDir());
	FILE *file = fopen(strFile, "w");
	if (!file){
		a_LogOutput(1, "CECRecordAccount::Save, Failed to open %s", strFile);
		return;
	}

	AString str = m_bRecord ? "true" : "false";
	fwrite((const char *)str, sizeof(char), str.GetLength(), file);
	
	char nextLine = '\n';
	fwrite(&nextLine, sizeof(nextLine), 1, file);

	for (Records::iterator it = m_records.begin(); it != m_records.end(); ++ it)
	{
		AString str = it->Serialize();
		fwrite((const char *)str, sizeof(char), str.GetLength(), file);
		fwrite(&nextLine, sizeof(nextLine), 1, file);
	}

	fflush(file);
	fclose(file);
}
