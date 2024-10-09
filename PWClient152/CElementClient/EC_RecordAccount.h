// Filename	: EC_RecordAccount.h
// Creator	: XuWenbin
// Date		: 2013/9/6

#pragma once

#include <Aassist.h>
#include <vector.h>

//	��¼����������Ч�û���
class CECRecordAccount
{
public:
	struct Record
	{
		ACString	account;	//	�˺�����
		int			server;		//	��Ӧ�������±�
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
	CECRecordAccount(const CECRecordAccount&);				//	δʵ��
	CECRecordAccount & operator=(const CECRecordAccount&);	//	δʵ��
	
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