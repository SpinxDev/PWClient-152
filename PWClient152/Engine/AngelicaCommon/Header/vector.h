  /*
 * FILE: vector.h
 *
 * DESCRIPTION: vector
 *
 * CREATED BY: Cui Ming 2002-1-21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/
#ifndef __ABASE_VECTOR_H__
#define __ABASE_VECTOR_H__
#pragma once

#include <assert.h>
#include <stdlib.h>
#include <new>
#pragma warning(disable: 4786) // identifier was truncated in the debug information

namespace abase
{
class default_alloc
{
public: 
	inline static void * allocate(size_t size) { 
		void * rst = ::operator new(size);
		return rst;
	}
	inline static void  deallocate(void * ptr,size_t size) 
	{ 
		::operator delete(ptr);
	}
	inline static void  deallocate(void * ptr) 
	{ 
		::operator delete(ptr);
	}
};


template<class T>
inline void swap(T & lhs,T & rhs)
{
	T tmp;
	tmp = lhs;
	lhs = rhs;
	rhs = tmp;
}

template <class T, class _Allocator=default_alloc>
class vector
{
protected:
	inline T * _M_allocate(size_t __n) { return (T*)_Allocator::allocate(__n * sizeof(T));}
	inline void _M_deallocate(T* __p, size_t __n){ if (__p) _Allocator::deallocate(__p,__n * sizeof(T));}
	inline T* _M_allocate_new(size_t __n,size_t & rst)
	{
		if(__n < 5)
			__n = 5;
		else
			__n += (_cur_size>>1) + 2; 
		rst = __n;
		return _M_allocate(__n);
	}
	
	inline T *_M_allocate_and_copy(size_t __n, T *start, T * finish){
		T * tmp = _M_allocate(__n);
		T * tmp2;
		tmp2 = tmp;
		while(start != finish && (__n--)>0)
		{
			new ((void *)&*tmp2++) T(*start++);
		}
		return tmp;
	}
protected:
	T * _data;
	T * _finish;
	size_t _max_size;
	size_t _cur_size;
public:
	typedef T * iterator;
	typedef const T * const_iterator;

public:
//	explicit vector(int n=10);
	vector();
	vector(size_t n, const T & val);
	vector(const vector<T,_Allocator> & rhs);
	vector(T * _F, T* _L);
	~vector();
	const vector<T,_Allocator> &operator=(const vector<T,_Allocator> & rhs);

public:
	inline const T & operator [](size_t pos) const {assert(pos >=0 && pos < _cur_size);return _data[pos];}
	inline T & operator [](size_t pos) {assert(pos >=0 && pos < _cur_size);return _data[pos];}
	inline const T & at(size_t pos) const {assert(pos >=0 && pos < _cur_size); return _data[pos];}
	inline T & at(size_t pos) {assert(pos >=0 && pos < _cur_size); return _data[pos];}
	inline size_t capacity() const {return _max_size;}
	inline size_t max_size() const{ return size_t(-1) / sizeof(T); }
	inline size_t size() const { return _cur_size;}
	inline T & front() { assert(_data != _finish); return *begin(); }
	inline const T &front() const { assert(_data != _finish); return *begin(); }
	inline T & back(){assert(_data != _finish);return *(end()-1);}
	inline const T & back() const {assert(_data != _finish); return *(end()-1);}
	inline bool empty() const {return size() == 0;}
	inline const T * begin() const {return _data;}
	inline T * begin() {return _data;}
	inline const T * end() const {return _data + _cur_size;}
	inline T * end() { return _data + _cur_size;}
	inline void clear(){ erase(begin(),end());}

	void reserve(size_t __n) {
		if (capacity() < __n) {
			const size_t __old_size = size();
			T * __tmp = _M_allocate_and_copy(__n, _data, _finish);
			for(T * fp = _data;fp < _finish;fp ++)
				fp->~T();
			_M_deallocate(_data, _max_size);
			_data		= __tmp;
			_finish		= __tmp + __old_size;
			_max_size	= __n;
		}
	}
	void push_back(const T & x);
	void pop_back() {_finish--;_cur_size --;_finish->~T();}
	T *insert(T * data,const T& x);
	void insert(T *it, size_t n, const T& x);
	void swap(vector<T,_Allocator> & x);
	T * erase(T *obj);		// delete the obj
	void erase_noorder(T *obj);
	void erase(T *first, T * last);	// delete [first, last)

    //The copy() function try to use current memory buffer, which is different with function operator=(...) 
    void copy(const vector<T,_Allocator> & rhs);

};

template <class T,class _Allocator>
vector<T,_Allocator>::vector()
{
	_max_size = 0;
	_cur_size = 0;
	_data = NULL;
	_finish = _data;
}

template <class T,class _Allocator>
vector<T,_Allocator>::vector(size_t n, const T & val)
{
	_max_size	= n;
	if(n) 
	{
		_data		= _M_allocate(n);
		assert(_data);
	}
	else 
		_data  = NULL;
	_finish		= _data;
	_cur_size	= n;
	while(n--) new ((void*)&*_finish++) T(val);
}

template<class T,class _Allocator>
vector<T,_Allocator>::vector(const vector<T,_Allocator> & rhs)
{
	_max_size	= rhs._max_size;
	_cur_size	= rhs._cur_size;
	if(rhs._data)
		_data = _M_allocate(_max_size);
	else 
		_data = NULL;
	_finish		= _data;
	size_t n = _cur_size;
	const T * it = rhs.begin();
	while(n--) new((void*)&*_finish++) T(*it++);
}

template<class T,class _Allocator>
vector<T,_Allocator>::vector(T * _F, T* _L)
{
	_max_size = 0;
	_cur_size = 0;
	_data = NULL;
	_finish = _data;
	reserve(_L - _F);
	while(_F != _L)
	{
		new ((void*)&*_finish++) T(*_F++);
	}
	_cur_size = _finish - _data;
}


template<class T,class _Allocator>
const vector<T,_Allocator> & vector<T,_Allocator>::operator=(const vector<T,_Allocator> & rhs)
{
	if(this == &rhs) return rhs;
	vector<T,_Allocator> tmp(rhs);
	tmp.swap(*this);
	return *this;
}

template <class T,class _Allocator>
vector<T,_Allocator>::~vector()
{	
	clear();
	_M_deallocate(_data,_max_size);
}

template <class T,class _Allocator>
void vector<T,_Allocator>::push_back(const T & x)
{
	if(_cur_size == _max_size)
	{
		size_t i;
		T * newdata,*tmp1,*tmp2;
		size_t newsize;
		newdata = _M_allocate_new(_cur_size + 1,newsize);
		assert(newdata);
		tmp1 = newdata;
		tmp2 = _data;
		for(i = 0; i < _cur_size;i++)
			new ((void *)&*tmp1++) T(*tmp2++);

		tmp2 = _data;
		for(i = 0; i < _cur_size; i++)
			tmp2++->~T();

		_M_deallocate(_data,_max_size);
		_max_size = newsize;
		_data = newdata;
		_finish = _data + _cur_size;
	}
	
	new ((void *)&*(_finish)) T(x);
	_cur_size ++;
	_finish ++;
}

template <class T,class _Allocator>
T *vector<T,_Allocator>::insert(T * data,const T& x)
{
	ptrdiff_t idx;
	idx = data - _data;
	if(_cur_size == _max_size)
	{
		T * newdata,*tmp1,*tmp2;
		size_t newsize;
		newdata = _M_allocate_new(_cur_size + 1, newsize);
		assert(newdata);
		tmp1 = newdata;
		tmp2 = _data;
						     
		while(tmp2 < data)
			new ((void *)&*tmp1++) T(*tmp2++);

		new ((void *)&*tmp1++) T(x);

		while(tmp2 < _finish)
			new ((void *)&*tmp1++) T(*tmp2++);

		tmp2 = _data;
		for(size_t i = 0; i < _cur_size; i++)
			tmp2++->~T();

		_M_deallocate(_data,_max_size);
		_max_size = newsize;
		_data = newdata;
		_finish = _data + _cur_size;
	}
	else
	{
		if(_finish != data)
		{
			new ((void*)_finish) T(*(_finish -1));
			for(T * tp = _finish - 1;tp > data;tp --)
				*tp = *(tp-1);
			*data = x;
		}
		else
		{
			new((void*)_finish) T(x);
		}

		
	}
	_cur_size ++;
	_finish ++;
	return _data + idx;
}

template <class T,class _Allocator>
void vector<T,_Allocator>::insert(T *it, size_t n, const T& x)
{
	if(!n) return;
	int idx;
	idx = it - _data;
	if(_cur_size + n > _max_size)
	{
		size_t i;
		T * newdata,*tmp1,*tmp2;
		size_t newsize;
		newdata = _M_allocate_new(_cur_size + n, newsize);
		assert(newdata);
		tmp1 = newdata;
		tmp2 = _data;

		while(tmp2 < it)
			new ((void *)&*tmp1++) T(*tmp2++);

		for(i = 0; i< n; i++) new ((void *)&*tmp1++) T(x);

		while(tmp2 < _finish)
			new ((void *)&*tmp1++) T(*tmp2++);

		tmp2 = _data;
		for(i = 0; i < _cur_size; i++)
			tmp2++->~T();

		_M_deallocate(_data,_max_size);
		_max_size = newsize;
		_data = newdata;
		_finish = _data + _cur_size;
	}
	else
	{
		assert(it <= _finish);
		if((size_t)(_finish - it) < n)
		{
			T * p = it + n;
			for(T * it2 = it; it2  != _finish;p ++,it2++)
			{
				new ((void*)p) T(*it2);
			}

			p = _finish;
			for(size_t size = n -(_finish - it); size > 0; --size,++p)
			{
				new ((void*)p) T(x);
			}
			for(p = it; p != _finish; ++p)
			{
				*p = x;
			}
		}
		else
		{
			T * p = _finish;
			T * it2;
			for(it2 = _finish - n; it2  != _finish;p ++,it2++)
			{
				new ((void*)p) T(*it2);
			}

			p = _finish;
			for(it2 = _finish - n ; it2 != it ; )
			{
				*--p = *--it2;
			}

			for(p = it; p != it + n ; ++p)
			{
				*p = x;
			}
		}
	}
	_cur_size += n;
	_finish += n;
	return;
}

template <class T,class _Allocator>
T* vector<T,_Allocator>::erase(T *obj)
{
	if(_cur_size)
	{
		ptrdiff_t idx = obj - _data;

		for(T * fp = obj;fp < _finish - 1;fp++)
		{
			*fp = *(fp+1);
		}
		_cur_size --;
		_finish --;
		_finish->~T();	//destroy the last

		return _data + idx;
	}
	return _finish;

}

template <class T,class _Allocator>
void vector<T,_Allocator>::swap(vector<T,_Allocator> & __x)
{
    abase::swap(_data, __x._data);
    abase::swap(_finish, __x._finish);
    abase::swap(_cur_size, __x._cur_size);
    abase::swap(_max_size, __x._max_size);
}

template <class T,class _Allocator>
void vector<T,_Allocator>::erase(T *first, T * last)
{
	if(first == last) return;
	T * fp;
	T * fp2;
	for(fp = last,fp2 = first; fp < _finish;fp++,fp2++)
	{
		*fp2 = *fp;
	}

	for(;fp2 < _finish; fp2++)
	{
		fp2->~T();
	}
	_finish   -= (last - first);
	_cur_size -= (last - first);
}
template <class T,class _Allocator>
void vector<T,_Allocator>::erase_noorder(T *obj)
{
	int idx = obj - _data;
	assert(idx >=0 && idx < (int)_cur_size);
	_cur_size --;
	_finish --;
	if(_cur_size)
	{
		_data[idx] = *_finish;
	}
	_finish->~T();	//destroy the last
}

template <class T,class _Allocator>
void vector<T,_Allocator>::copy(const vector<T,_Allocator> & rhs)
{
    if (&rhs == this)
    {
        return;
    }
    clear();
    reserve(rhs.size());
    unsigned int i;
    for (i=0; i<rhs.size(); ++i)
    {
        push_back(rhs[i]);
    }
}

template <class T>
class vector_ptr
{
	vector<void *> __ivector;
public:
	vector_ptr(int n=10):__ivector(n){assert("vector_ptr需要一个指针类型" && (T)0  == (void*)0 );}
	vector_ptr(int n, const T & val):__ivector(n,val){assert("vector_ptr需要一个指针类型" && (T)0 == (void*)0 );}
	vector_ptr(const vector_ptr<T> & rhs):__ivector(rhs.__ivector){assert("vector_ptr需要一个指针类型" && (T)0  == (void*)0);}
	const vector_ptr<T> & operator=(const vector_ptr<T> & rhs)
	{
		__ivector = rhs.__ivector;
		return *this;
	}

public:
	
	inline const T & operator [](size_t pos) const {return (const T&)__ivector[pos];}
	inline T & operator [](size_t pos) {return (T&)__ivector[pos];}
	inline const T & at(size_t pos) const {return (const T &)__ivector.at(pos);}
	inline T & at(size_t pos) {return (T&)__ivector.at(pos);}
	inline size_t capacity() const {return __ivector.capacity();}
	inline size_t max_size() const{ return __ivector.max_size();}
	inline size_t size() const { return __ivector.size();}
	inline T & front() { return (T&)__ivector.front();}
	inline const T &front() const { return (const T &)__ivector.front();}
	inline T & back(){return (T&)__ivector.back();}
	inline const T & back() const {return (const T &)__ivector.back();}
	inline bool empty() const {return __ivector.empty();}
	inline const T * begin() const {return (const T*)__ivector.begin();}
	inline T * begin() {return (T*)__ivector.begin();}
	inline const T * end() const {return (const T*)__ivector.end();}
	inline T * end() { return (T*)__ivector.end();}
	inline void clear(){ __ivector.clear();}
	inline void reserve(size_t __n) {__ivector.reserve();}
	inline void push_back(const T & x) {__ivector.push_back(x);}
	inline void pop_back() {__ivector.pop_back();}
	inline T *insert(T * data,const T& x){return (T*)__ivector.insert((void**)data,x);}
	inline void insert(T *it, size_t n, const T& x){__ivector.insert((void**)it,n,x);}
	inline void swap(vector_ptr<T> & x) {__ivector.swap(x.__ivector);}
	inline void erase(T *obj) {__ivector.erase((void**)obj);}
	inline void erase_noorder(T *obj) {__ivector.erase_noorder((void**)obj);}
	inline void erase(T *first, T * last) {__ivector.erase((void**)first,(void**)last);}
};

template<class T>
inline void clear_vector_ptr(vector_ptr<T> & vec)
{
	int i;
	for(i = 0; i < vec.size(); i++)
	{
		delete vec[i];
	}
	vec.clear();
}

}
#endif
