// Filename	: EC_RecordAccount.h
// Creator	: XuWenbin
// Date		: 2013/9/6

#pragma once

#include <Aassist.h>
#include <vector.h>

//	记录玩家输入的有效用户名
class CECRecordAccount
{
public:
	struct Record
	{
		ACString	account;	//	账号名称
		int			server;		//	对应服务器下标
		Record():server(-1){}
		Record(const ACString &strAccount, int iServer):account(strAccount), server(iServer) {}
		Record(const ACHAR *szAccount, int iServer):account(szAccount), server(iServer) {}
		bool IsValid()const{
			return !account.IsEmpty();
		}
		bool operator == (const ACString &rhs)const{
			return account == rhs;
		}
		bool operator == (const ACHAR *rhs)const{
			return account == rhs;
		}
		bool operator < (const Record &rhs)const{
			return account < rhs.account;
		}
		AString Serialize()const;
		static Record UnSerialize(const AString &);
	};
	typedef abase::vector<Record>	Records;
private:
	CECRecordAccount();
	CECRecordAccount(const CECRecordAccount&);				//	未实现
	CECRecordAccount & operator=(const CECRecordAccount&);	//	未实现
	
	Records	m_records;

	bool	m_bRecord;

	void	Resize(bool bAddNew);
	void	RemoveRecord(int count);

public:
	static CECRecordAccount & Instance();
	
	void Load();
	void Save();
	void Clear();

	void SetCanRecord(bool bCan);
	bool GetCanRecord()const{ return m_bRecord; }
	void Add(const ACHAR *szAccount, int iServer);

	bool Complete(const ACHAR *szAccount, Records &candidates);
	const Record * FindRecord(const ACHAR *szAccount)const;
	const Record * GetLatestRecord()const;
};