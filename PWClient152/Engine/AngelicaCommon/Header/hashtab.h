 /*
 * FILE: hashtab.h
 *
 * DESCRIPTION: hash table
 *
 * CREATED BY: Cui Ming 2002-1-21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/

#ifndef __ABASE_HASH_TABLE_H__
#define __ABASE_HASH_TABLE_H__

#include <string.h>
#include <intsafe.h>
#include <basetsd.h>
#include "vector.h"

#pragma warning (disable: 4786)
#pragma warning (disable: 4284)

namespace abase{
enum { __abase_num_primes = 28};	//prime list from stl :)
static const unsigned long __abase_prime_list[__abase_num_primes] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
  1610612741ul, 3221225473ul, 4294967291ul
};
inline size_t __abase_next_prime(size_t __n)
{
	for(int i = 0;i< __abase_num_primes; i++)
	{
		if(__abase_prime_list[i] > __n)
		{
			return __abase_prime_list[i];
		}
	}
	return size_t(-1);
}


template<class T, class U>
struct pair {
	typedef T first_type;
	typedef U second_type;
	T first;
	U second;
	pair(){};
	pair(const T& x, const U& y):first(x),second(y){}
template<class V, class W>
	pair(const pair<V, W>& pr) 
	{
		first = pr.first;
		second = pr.second;
	}
};

template <class _Val,class _Key>
struct Hashtable_node
{
  Hashtable_node* _next;
  _Val _val;
  _Key _key;
  inline Hashtable_node(const _Key & key, const _Val & val, Hashtable_node* next):_val(val),_key(key)
  {
	_next = next;
  }
  void Release(){
	 this->~Hashtable_node();
  }
  
};

struct _hash_function
{
	_hash_function() {}
	_hash_function(const _hash_function & hf) {}

	inline unsigned long operator()(char data) const {return data;}
	inline unsigned long operator()(short data) const {return data;}
	inline unsigned long operator()(int data) const {return data;}
	inline unsigned long operator()(long data) const {return data;}
	inline unsigned long operator()(unsigned char data) const {return data;}
	inline unsigned long operator()(unsigned short data) const {return data;}
	inline unsigned long operator()(unsigned int data) const {return data;}
	inline unsigned long operator()(unsigned long data) const {return data;}
	inline unsigned long operator()(void *data) const {return PtrToUlong(data);}

	inline unsigned long operator()(__int64 data) const
	{
		return (unsigned long)((data >> 32) + (data & 0xffffffff));
	}

	inline unsigned long operator()(unsigned __int64 data) const
	{
		return (unsigned long)((data >> 32) + (data & 0xffffffff));
	}

	inline unsigned long operator()(char *s) const
	{
		unsigned long h = 0;
		for(;*s; s++)
		{
			h = h * 31 + *(unsigned char *)s;
		}
		return h;
	}
	inline unsigned long operator()(const char *s) const
	{
		unsigned long h = 0;
		for(;*s; s++)
		{
			h = h * 31 + *(unsigned char *)s;
		}
		return h;
	}
	inline unsigned long operator()(const wchar_t *s) const
	{
		unsigned long h = 0;
		for(;*s; s++)
		{
			h = h * 31 + *s;
		}
		return h;
	}	

};

struct _hash_string
{
	typedef const char * LPCSTR;
	const char * __str;
	_hash_string(const char * str):__str(str){}
	operator LPCSTR() const { return __str;}
	bool operator ==(const char * str) const { return strcmp(str,__str) == 0;}
	bool operator ==(const _hash_string & rhs) const { return strcmp(rhs,__str) == 0;}
};


/*class definition*/
template <class _Value, class _Key, class _HashFunc,class _Allocator=default_alloc>
class hashtab
{
public:
typedef _Key	key_type;
typedef _Value	value_type;
typedef pair<value_type *, bool> pair_type;
typedef Hashtable_node<_Value,_Key> _Node;
typedef _Value& reference;
typedef _Value* pointer;

template <class _ItValue>
class iterator_template
{
private:
	const hashtab<_Value,_Key,_HashFunc,_Allocator> * _tab;
	_Node *const * _np;
	_Node *  _node;
//	iterator_template<_ItValue> & operator =(const iterator_template<_ItValue> &);
private:
	iterator_template(const hashtab<_Value,_Key,_HashFunc,_Allocator> * __this):_tab(__this),_np(0),_node(0)
	{
		_np = _tab->_buckets.begin();
		if(_np != _tab->_buckets.end())
		{
			_node = *_np;
			if(_node == 0) 
			{
				++*this ;
			}
		}
		else
		{
			_np = NULL;
		}
	}

	iterator_template(_Node* __node, const hashtab<_Value,_Key,_HashFunc,_Allocator> * __this):_tab(__this),_np(0),_node(__node)
	{
		if(__node)
		{
			size_t n = _tab->get_hash(__node->_key);
			_np = _tab->_buckets.begin() + n;
		}
	}
	iterator_template(const hashtab<_Value,_Key,_HashFunc,_Allocator> * __this,int ):_tab(__this),_np(0),_node(0)
	{}
public:
	iterator_template():_tab(0),_np(0),_node(0) {}
public:
	bool operator ==( const iterator_template<_ItValue> & rhs) const
	{
		if(_tab && rhs._tab) {
			assert(_tab == rhs._tab);
			return _tab == rhs._tab && _np == rhs._np && _node == rhs._node;
		}
		return _np == rhs._np && _node == rhs._node;
	}

	bool operator !=( const iterator_template<_ItValue> & rhs) const
	{
		return !operator==(rhs);
	}

	_ItValue * value()const
	{
		if(_np != NULL) return &(_node->_val);
		return NULL;
	}
	
	const _Key * key() const
	{
		if(_np != NULL) return &(_node->_key);
		return NULL;
	}
	
	_ItValue* operator->()const
	{
		if(_np != NULL) return &(_node->_val);
		return NULL;
	}

	iterator_template<_ItValue> & operator ++()
	{
		if(_np == NULL) return *this;
		do 
		{
			if(_node == NULL)
			{
				_np++;
				if(_np == _tab->_buckets.end())
				{
					_np = NULL;
					break;
				}
				_node = *_np;
				if(_node) break;
			}
			else
			{
				_node = _node->_next;
			}
		}while(_node == NULL);
		return *this;
	}
	bool is_eof()  const
	{
		return !_np;
	}

friend class hashtab<_Value,_Key,_HashFunc,_Allocator>;
};

typedef iterator_template<_Value> iterator;
typedef iterator_template<const _Value> const_iterator;
typedef size_t size_type;

//friend hashtab::iterator;
//friend const_iterator;
friend class abase::hashtab<_Value, _Key,_HashFunc,_Allocator>::iterator_template<_Value>;
friend class abase::hashtab<_Value, _Key,_HashFunc,_Allocator>::iterator_template<const _Value>;

private:
	_HashFunc	_hash;
	size_t		_num_elements;
	vector<_Node *,_Allocator> _buckets;
	
	size_t get_hash(const key_type & key) const 
	{
		return _hash(key) % _buckets.size();
	}

	const hashtab<_Value,_Key,_HashFunc,_Allocator> & operator=(const hashtab<_Value,_Key,_HashFunc,_Allocator> & rhs);
	hashtab(const hashtab<_Value,_Key,_HashFunc,_Allocator>& rhs);
public:
	hashtab(size_t __n, 
		const _HashFunc & __hf)
	:_hash(__hf),
	_num_elements(0),
	_buckets(_M_next_size(__n),(_Node *) 0)
	{
	}

	explicit hashtab(size_t __n):_hash(),
	_num_elements(0),
	_buckets(_M_next_size(__n),(_Node *) 0)
	{
	}

	~hashtab()
	{
		clear();
	}


	size_t size() const { return _num_elements; }
	size_t max_size() const { return size_t(-1); }
	bool empty() const { return size() == 0; }
	void clear();
	void resize(size_t __num_elements_hint);
	inline bool put(const key_type & __key , const value_type & __val){
		resize(_num_elements + 1);
		return put_noresize(__key,__val);
	}
	iterator begin() { return iterator(this);}
	iterator end() { return iterator(this,0);}
	const_iterator begin() const { return const_iterator(this);} //$$$ 这里并没有返回const迭代器
	const_iterator end() const{ return const_iterator(this,0);}
	bool put_noresize(const key_type & __key , const value_type & __val);
	pair<value_type *, bool> get(const key_type &__key) const;
	value_type * nGet(const key_type &__key) const;	//特殊的get:)
	bool erase(const key_type &__key);
	iterator erase(const iterator& __it);
	size_type bucket_count() const { return _buckets.size(); }	
	size_type max_bucket_count() const {return __stl_prime_list[(int)__stl_num_primes - 1]; } 

	iterator find(const key_type& __key) 
	{
		size_type __n = get_hash(__key);
		_Node* __first;
		for ( __first = _buckets[__n];
		__first && !(__first->_key == __key);
		__first = __first->_next)
		{}
		return iterator(__first, this);
	} 
	
	const_iterator find(const key_type& __key) const
	{
		size_type __n = get_hash(__key);
		_Node* __first;
		for ( __first = _buckets[__n];
		__first && !(__first->_key == __key);
		__first = __first->_next)
		{}
		return const_iterator(__first, this);
	} 
	reference find_or_insert(const key_type & __key,const value_type& __obj);

	size_type elems_in_bucket(size_type __bucket) const
	{
		size_type __result = 0;
		for (_Node* __cur = _buckets[__bucket]; __cur; __cur = __cur->_next)
			__result += 1;
		return __result;
	}

	template<class _EnumFunc>
	void enum_element(_EnumFunc & __func)
	{	
		for (_Node **fp = _buckets.begin();fp != _buckets.end(); fp ++) {
			_Node* __cur = *fp;
			while (__cur != NULL) {
				__func(__cur->_key,__cur->_val);
				__cur = __cur->_next;
			}
		}
	}

	template<class _EnumFunc>
	void enum_buckets(_EnumFunc & __func)
	{	
		for (_Node **fp = _buckets.begin();fp != _buckets.end(); fp ++) {
			__func(*fp);
		}
	}
private:
	size_t _M_next_size(size_t __n) const 
	{
		return __abase_next_prime(__n);
	}

};

template <class _Value, class _Key, class _HashFunc,class _Allocator>
typename hashtab<_Value,_Key,_HashFunc,_Allocator>::reference 
hashtab<_Value,_Key,_HashFunc,_Allocator>::find_or_insert(const key_type & __key,const value_type& __val)
{
	resize(_num_elements + 1);
	
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];
	
	for (_Node* __cur = __first; __cur; __cur = __cur->_next)
		if(__cur->_key == __key)
			return __cur->_val;
	_Node * __tmp = new (_Allocator::allocate(sizeof(_Node))) _Node(__key,__val,__first);
	_buckets[__n] = __tmp;
	++_num_elements;
	return __tmp->_val;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
bool hashtab<_Value,_Key,_HashFunc,_Allocator>::
	put_noresize(const key_type & __key , const value_type & __val)
{
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];

	for (_Node* __cur = __first; __cur; __cur = __cur->_next) 
		if (__cur->_key == __key)
			return false;

	_Node* __tmp = new (_Allocator::allocate(sizeof(_Node))) _Node(__key,__val,__first);
	_buckets[__n] = __tmp;
	++_num_elements;
	return true;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
void hashtab<_Value,_Key,_HashFunc,_Allocator>::
	resize(size_t __num_elements_hint)
{
	const size_t __old_n = _buckets.size();
	if( __num_elements_hint > __old_n)
	{
		const size_t __n = _M_next_size(__num_elements_hint);
		if(__n > __old_n)
		{
			vector<_Node*,_Allocator> __tmp(__n, (_Node*)(0));
			for (size_t __bucket = 0; __bucket < __old_n; ++__bucket) {
				_Node* __first = _buckets[__bucket];
				while (__first) {
					size_t __new_bucket = _hash(__first->_key) % __n;
					_buckets[__bucket] = __first->_next;
					__first->_next = __tmp[__new_bucket];
					__tmp[__new_bucket] = __first;
					__first = _buckets[__bucket];
				}
			}
			_buckets.swap(__tmp);
		}
	}
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
void hashtab<_Value,_Key,_HashFunc,_Allocator>::
	clear()
{
	for (size_t __i = 0; __i < _buckets.size(); ++__i) {
		_Node* __cur = _buckets[__i];
		while (__cur != NULL) {
			_Node * __next = __cur->_next;
			__cur->Release();
			_Allocator::deallocate(__cur,sizeof(_Node));
			__cur = __next;
		}
		_buckets[__i] = 0;
	}
	_num_elements = 0;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
pair<_Value *, bool> hashtab<_Value,_Key,_HashFunc,_Allocator>::
	get(const key_type &__key) const
{
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];

	for (_Node* __cur = __first; __cur; __cur = __cur->_next) 
		if (__cur->_key == __key)
			return pair<_Value *,bool>(&(__cur->_val),true);
	return pair<_Value *,bool>((_Value *)NULL,false);
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
_Value * hashtab<_Value,_Key,_HashFunc,_Allocator>::
	nGet(const key_type &__key) const
{
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];

	for (_Node* __cur = __first; __cur; __cur = __cur->_next) 
		if (__cur->_key == __key)
			return &(__cur->_val);
	return NULL;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
bool hashtab<_Value,_Key,_HashFunc,_Allocator>::
	erase(const key_type &__key)
{
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];
	_Node *__prev=NULL;

	for (_Node* __cur = __first; __cur; __prev = __cur,__cur = __cur->_next) 
		if (__cur->_key == __key)
		{			
			if(__cur == __first)
			{
				_buckets[__n] = __cur->_next;
			}
			else
			{
				__prev->_next = __cur->_next;
			}
			__cur->Release();
			 _Allocator::deallocate(__cur,sizeof(_Node));
			--_num_elements;
			return true;
		}
	return false;
}
template <class _Value, class _Key, class _HashFunc,class _Allocator>
typename hashtab<_Value,_Key,_HashFunc,_Allocator>::iterator hashtab<_Value,_Key,_HashFunc,_Allocator>::
	erase(const iterator &__it)
{
	iterator it(__it);
	++it;
	_Node* __p = __it._node;
	if (__p && __it._np) {
		_Node* __cur = *__it._np;
		
		if (__cur == __p) {
			*(_Node**)(__it._np) = __cur->_next;
			__cur->Release();
			_Allocator::deallocate(__cur,sizeof(_Node));
			--_num_elements;
		}
		else {
			_Node* __next = __cur->_next;
			while (__next) {
				if (__next == __p) {
					__cur->_next = __next->_next;
					__next->Release();
					 _Allocator::deallocate(__next,sizeof(_Node));
					--_num_elements;
					break;
				}
				else {
					__cur = __next;
					__next = __cur->_next;
				}
			}
		}
	}
	return it;
}

}
#endif
