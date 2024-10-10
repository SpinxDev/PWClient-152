#include "FGuid.h"

#include <ObjBase.h>

using namespace AudioEngine;

Guid::Guid(void)
{
}

Guid::~Guid(void)
{
}

bool Guid::BuildFromString(const char* sz)
{
	unsigned int uData4;
	unsigned int uData5;
	unsigned int uData6;
	unsigned int uData7;	
	sscanf_s(sz, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", &guid.Data1,&guid.Data2,&guid.Data3,
		&guid.Data4[0],
		&guid.Data4[1],
		&guid.Data4[2],
		&guid.Data4[3],
		&uData4,
		&uData5,
		&uData6,
		&uData7);
	guid.Data4[4] = (char)uData4;
	guid.Data4[5] = (char)uData5;
	guid.Data4[6] = (char)uData6;
	guid.Data4[7] = (char)uData7;
	return true;
}

const char* Guid::GetString() const
{
	char sz[1024] = {0};	
	sprintf_s(sz, 1024, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", guid.Data1,guid.Data2,guid.Data3,
		guid.Data4[0],
		guid.Data4[1],
		guid.Data4[2],
		guid.Data4[3],
		guid.Data4[4],
		guid.Data4[5],
		guid.Data4[6],
		guid.Data4[7]);
	m_str = sz;
	return m_str.c_str();
}



//////////////////////////////////////////////////////////////////////////


GuidGen::GuidGen(void)
{
}

GuidGen::~GuidGen(void)
{
}

GuidGen& GuidGen::GetInstance()
{
	static GuidGen instance;
	return instance;
}

Guid GuidGen::Generate()
{
	GUID guid;
	CoCreateGuid(&guid);
	Guid g;
	g.guid = guid;
	return g;
}