  /*
 * FILE: hashmap.h
 *
 * DESCRIPTION: hash map , implements with hash table
 *
 * CREATED BY: Cui Ming 2002-1-21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/
#ifndef __ABASE_HASH_MAP_H__
#define __ABASE_HASH_MAP_H__

#include <string.h>
#include "hashtab.h"

#ifdef MSVC
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#endif

namespace abase{
template <class _Key, class _Value, class _HashFunc = _hash_function, class _Allocator = default_alloc>
class hash_map
{
private:
	typedef hashtab<pair<const _Key,_Value>,_Key,_HashFunc,_Allocator> _Ht;
	_Ht _M_ht;
public:
	typedef typename _Ht::key_type key_type;
	typedef _Value data_type;
	typedef _Value mapped_type;
	typedef typename _Ht::value_type value_type;
	typedef size_t size_type;

	typedef typename _Ht::iterator iterator;
	typedef typename _Ht::const_iterator const_iterator;
public:

	hash_map() : _M_ht(100) {}
	explicit hash_map(size_type __n): _M_ht(__n) {}
	
public:
	size_type size() const { return _M_ht.size(); }
	size_type max_size() const { return _M_ht.max_size(); }
	bool empty() const { return _M_ht.empty(); }
	void swap(hash_map& __hs) { _M_ht.swap(__hs._M_ht); }

	iterator begin() { return _M_ht.begin(); }
	iterator end() { return _M_ht.end(); }
	const_iterator begin() const { return _M_ht.begin(); }
	const_iterator end() const { return _M_ht.end(); }

	iterator find(const key_type& __key) {return _M_ht.find(__key);}
	const_iterator find(const key_type& __key) const {return _M_ht.find(__key);}
	
	_Value& operator[](const key_type& __key) {
		return _M_ht.find_or_insert(__key,value_type(__key, _Value())).second;
	}
	pair<iterator, bool> insert(const value_type& x)
	{
		if(_M_ht.put(x.first,x)) 
		{
			return pair<iterator, bool>(_M_ht.find(x.first),true);
		}
		else
		{
			return pair<iterator, bool>(_M_ht.end(),false);
		}
	}
	
	size_type erase(const key_type& __key) {return _M_ht.erase(__key); }
	iterator erase(iterator __it) { return _M_ht.erase(__it); }
	void clear() { _M_ht.clear(); }
	void resize(size_type __hint) { _M_ht.resize(__hint); }
	size_type bucket_count() const { return _M_ht.bucket_count(); }
	size_type max_bucket_count() const { return _M_ht.max_bucket_count(); }
	size_type elems_in_bucket(size_type __n) const { return _M_ht.elems_in_bucket(__n); }

	bool operator==(const hash_map& rhs)
	{
			return _M_ht == rhs._M_ht;
	}
};

}

#endif
