// Filename	: WikiDataReader.h
// Creator	: Feng Ning
// Date		: 2010/09/26

#pragma once

#include "AAssist.h"
#include "hashmap.h"
#include "vector.h"

class WikiFileInterface;
class WikiDataReader
{
private:
	struct MemberInfo
	{
		ACString Name;
		unsigned int Offset;
		int Typemask;
	};
	
	struct TypeInfo
	{
		ACString Name;
		size_t Size;
	};
	
	typedef abase::hash_map<ACString, MemberInfo>::iterator Iter;
	typedef abase::pair<ACString, MemberInfo> Pair;
	
	abase::hash_map<ACString, MemberInfo> m_Members; // registered member
	abase::vector<ACString> m_Titles; // the csv titles
	TypeInfo m_Type; // the type info
	
	AString m_Filename;
	WikiFileInterface* m_pFile;
	
public:
	explicit WikiDataReader(bool isUnicode);
	virtual ~WikiDataReader();
	
	enum TYPE_MASK
	{
		TYPE_INTEGER = 0x01,
		TYPE_FLOAT = 0x02,
		TYPE_STRING = 0x03,
	};
	
	// init reader by a specific type
	void Init(const ACHAR* name, size_t size);

	// 
	void Release();
	
	// register normal memeber
	void Register(const ACHAR* name, void* pThis, void* pMember, int typemask);
	
	// register array member
	void Register(const ACHAR* name, void* pThis, void* pMember, int typemask, size_t arraysize, size_t step);
	
	// open a data file, with or without title line
	bool Open(const char* filename, bool title);
	
	bool IsEnd() const;
	
	int GetCurrentLine() const;
	
	bool ReadObject(void* pThis);
};