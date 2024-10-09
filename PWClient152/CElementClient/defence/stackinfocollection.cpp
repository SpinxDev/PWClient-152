//#include "stdafx.h"
#include <stdio.h>
#include "infocollection.h"
#include "stackinfocollection.h"
#include "defence.h"

StackInfoCollection StackInfoCollection::instance;

#ifndef BRIEF

void StackInfoCollection::Append(DWORD caller, DWORD count, DWORD size, DWORD pattern)
{
	for (ITEM::iterator it = item.lower_bound(caller); (*it).first == caller; ++it)
	{
		CallerStackInfo &csi = (*it).second;
		if ( csi.size == size && csi.pattern == pattern )
		{
			csi.count += count;
			return;
		}
	}
	item.insert( std::make_pair(caller, CallerStackInfo(count, size, pattern)) );
}

Marshal::OctetsStream& StackInfoCollection::marshal(Marshal::OctetsStream& os) const
{
	os << CompactUINT(item.size());
	for(ITEM::const_iterator it = item.begin(); it != item.end(); ++it)
		os << *it;
	return os;
}
const Marshal::OctetsStream& StackInfoCollection::unmarshal(const Marshal::OctetsStream& os)
{
	item.clear();
	size_t size;
	for (os >> CompactUINT(size); size > 0; --size)
	{
		ITEM::value_type tmp;
		os >> tmp;
		item.insert( item.end(), tmp );
	}
	return os;
}

Octets StackInfoCollection::GetOctets()
{
	Marshal::OctetsStream os;
	Lock();
	marshal( os );
	item.clear();
	Unlock();
	return (Octets&)os;
}

void StackInfoCollection::Dump()
{
	Lock();
	for(ITEM::const_iterator it = item.begin(); it != item.end(); ++it)
		printf("caller[%08x] count[%08x] size[%08x] pattern[%08x]\n",
			(*it).first, (*it).second.count, (*it).second.size, (*it).second.pattern );
	Unlock();
}

#else

void StackInfoCollection::Append(DWORD count, DWORD pattern)
{
	item[pattern] += count;
}

Marshal::OctetsStream& StackInfoCollection::marshal(Marshal::OctetsStream& os) const
{
	os << CompactUINT(item.size());
	for(ITEM::const_iterator it = item.begin(); it != item.end(); ++it)
		os << (*it).first << CompactUINT((*it).second);
	return os;
}
const Marshal::OctetsStream& StackInfoCollection::unmarshal(const Marshal::OctetsStream& os)
{
	item.clear();
	size_t size;
	for (os >> CompactUINT(size); size > 0; --size)
	{
		ITEM::value_type tmp;
		os >> tmp.first >> CompactUINT(tmp.second);
		item.insert( tmp );
	}
	return os;
}
Octets StackInfoCollection::GetOctets()
{
	Marshal::OctetsStream os;
	Lock();
	marshal( os );
	item.clear();
	Unlock();
	return (Octets&)os;
}

void StackInfoCollection::Dump()
{
	Lock();
	for(ITEM::const_iterator it = item.begin(); it != item.end(); ++it)
		printf("count[%08x] pattern[%08x]\n",
			(*it).second, (*it).first );
	Unlock();
}

#endif

void do_collect()
{
	FILE *fp = fopen("C:\\collect.bin", "wb");
	Octets o = StackInfoCollection::GetInstance().GetOctets();
	fwrite(o.begin(), o.size(), 1, fp);
	fclose(fp);
}

