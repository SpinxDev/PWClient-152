#ifndef __STACKINFOCOLLECTION_H
#define __STACKINFOCOLLECTION_H
#pragma warning( disable : 4786 )
#include <windows.h>
#include <map>
#include "gnmarshal.h"
#include "infocollection.h"

using namespace GNET;

void do_collect();

class StackInfoCollection : public InfoCollection
{
#ifndef BRIEF
	class CallerStackInfo : public Marshal
	{
	public:
		DWORD	count;
		DWORD	size;
		DWORD	pattern;
		CallerStackInfo() { }
		CallerStackInfo(DWORD _count, DWORD _size, DWORD _pattern) : count(_count), size(_size), pattern(_pattern) { }
		OctetsStream& marshal(OctetsStream& os) const
		{
			return os << CompactUINT(count) << CompactUINT(size) << pattern;
		}
		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			return os >> CompactUINT(count) >> CompactUINT(size) >> pattern;
		}
	};
	typedef std::multimap<DWORD, CallerStackInfo> ITEM;
#else
	typedef std::map<DWORD, DWORD> ITEM;
#endif
	ITEM item;
	HANDLE hMutex;
	StackInfoCollection() { hMutex = CreateMutex(NULL, FALSE, NULL); }
	static StackInfoCollection instance;
public:
	static StackInfoCollection& GetInstance() { return instance; }
	bool TryLock() { return WaitForSingleObject(hMutex, 0) == WAIT_OBJECT_0; }
	void Lock() { WaitForSingleObject(hMutex, INFINITE); }
	void Unlock() { ReleaseMutex(hMutex); }
#ifndef BRIEF
	void Append(DWORD caller, DWORD count, DWORD size, DWORD pattern);
#else
	void Append(DWORD count, DWORD pattern);
#endif
	void Dump();
	OctetsStream& marshal(OctetsStream& os) const;
	const OctetsStream& unmarshal(const OctetsStream& os);
	Octets GetOctets();
};

#endif
