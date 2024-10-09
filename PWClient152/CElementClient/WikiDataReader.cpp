// Filename	: WikiDataReader.cpp
// Creator	: Feng Ning
// Date		: 2010/09/26

#pragma once

#include "WikiDataReader.h"

class WikiFileInterface
{
public:
	virtual ~WikiFileInterface() {};

	virtual bool		Open(const char* szFile)		= 0;
	virtual void		Close()							= 0;
	virtual bool		SkipLine()						= 0;
	virtual bool		IsEnd() const					= 0;
	virtual int			GetCurLine() const				= 0;
	virtual bool		GetNextToken(bool bCrossLine)	= 0;

	virtual ACString		Token()						= 0;

protected:
	WikiFileInterface(){};
};

template<typename ScriptFile>
class WikiFileInterfaceImpl : public WikiFileInterface
{
public:
	WikiFileInterfaceImpl(){};
	virtual ~WikiFileInterfaceImpl() { Close(); }

	virtual bool		Open(const char* szFile)		{ return m_SF.Open(szFile); }
	virtual void		Close()							{ m_SF.Close(); }
	virtual bool		SkipLine()						{ return m_SF.SkipLine(); }
	virtual bool		IsEnd() const					{ return const_cast<ScriptFile&>(m_SF).IsEnd(); }
	virtual int			GetCurLine() const				{ return const_cast<ScriptFile&>(m_SF).GetCurLine(); }
	virtual bool		GetNextToken(bool bCrossLine)	{ return m_SF.GetNextToken(bCrossLine); }

	virtual ACString		Token();

private:
	ScriptFile m_SF;
};

#include "AScriptFile.h"
typedef WikiFileInterfaceImpl<AScriptFile> WikiFileReader;
ACString WikiFileReader::Token()							{ return AS2AC(m_SF.m_szToken); }

#include "AWScriptFile.h"
typedef WikiFileInterfaceImpl<AWScriptFile> WikiWFileReader;
ACString WikiWFileReader::Token()						{ return WC2AC(m_SF.m_szToken); }

WikiDataReader::WikiDataReader(bool isUnicode)
:m_pFile(NULL)
{
	m_Type.Size = 0;

	if(isUnicode)
	{
		m_pFile = new WikiWFileReader();
	}
	else
	{
		m_pFile = new WikiFileReader();
	}
}

WikiDataReader::~WikiDataReader()
{
	if(m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
	}
}

// init reader by a specific type
void WikiDataReader::Init(const ACHAR* name, size_t size)
{
	ASSERT(name && size > 0);
	
	Release();
	m_Type.Name = name;
	m_Type.Size = size;
}

void WikiDataReader::Release()
{
	m_Type.Size = 0;
	m_Type.Name.Empty();

	m_Members.clear();
	m_Titles.clear();
	m_pFile->Close();
}

// register normal memeber
void WikiDataReader::Register(const ACHAR* name, void* pThis, void* pMember, int typemask)
{
	ASSERT(m_Type.Size && name && pThis && pMember);

	MemberInfo info;
	info.Name = name;
	info.Offset = ((char*)pMember) - ((char*)pThis);
	ASSERT(info.Offset < m_Type.Size);
	info.Typemask = typemask;

	ACString lowName(name);
	lowName.MakeLower();
	m_Members[lowName] = info;

	m_Titles.push_back(lowName);
}

// register array member
void WikiDataReader::Register(const ACHAR* name, void* pThis, void* pMember, int typemask, size_t arraysize, size_t step)
{
	ASSERT(m_Type.Size && name && pThis && pMember);
	ASSERT(arraysize > 0 && step > 0);

	char* ptr = (char*)pMember;
	ACString singleName;
	for(size_t i=0;i<arraysize;i++)
	{
		singleName.Format(_AL("%s%d"), name, i+1);
		Register(singleName, pThis, ptr, typemask);
		ptr += step;
	}
}

bool WikiDataReader::Open(const char* filename, bool title)
{
	m_Filename = filename;

	if (!m_pFile->Open(m_Filename))
	{
		return false;
	}
	
	// read title from first line
	if(title)
	{
		m_Titles.clear();
		
		// read titles
		while (m_pFile->GetNextToken(false))
		{
			ACString token(m_pFile->Token());
			token.MakeLower();
			
			if(!m_Members.empty() &&
				m_Members.find(token) == m_Members.end())
			{
				// can not find a matched title
				ASSERT(false);
			}
			m_Titles.push_back(token);
		}
		
		// skip the title line
		m_pFile->SkipLine();
	}

	return true;
}

bool WikiDataReader::IsEnd() const
{
	return m_pFile->IsEnd();
}

int WikiDataReader::GetCurrentLine() const
{
	return m_pFile->GetCurLine();
}

bool WikiDataReader::ReadObject(void* pThis)
{
	if(!pThis || m_Titles.empty())
	{
		return false;
	}

	size_t titleIndex = 0;
	while (m_pFile->GetNextToken(false) && titleIndex < m_Titles.size())
	{
		const ACString& title = m_Titles[titleIndex];
		Iter itr = m_Members.find(title);

		if(itr != m_Members.end())
		{
			const MemberInfo& info = itr->second;
			char* pMember = (char*)pThis + info.Offset;
			ACString token(m_pFile->Token());

			switch(info.Typemask)
			{
			case TYPE_INTEGER:
				{
					int* pData = (int*)pMember;
					*pData = token.ToInt();
				}
				break;
			case TYPE_FLOAT:
				{
					float* pData = (float*)pMember;
					*pData = token.ToFloat();
				}
				break;
			case TYPE_STRING:
				{
					ACString* pData = (ACString*)pMember;
					*pData = token;
				}
				break;

			default:
				ASSERT(false);
			}
		}

		titleIndex++;
	}

	// move to next line
	m_pFile->SkipLine();

	return true;
}
