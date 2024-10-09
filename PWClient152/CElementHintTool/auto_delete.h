// Filename	: auto_delete.h
// Creator	: Xu Wenbin
// Date		: 2010/06/01

#pragma once

template <typename P>
struct auto_delete
{
	P * _ptr;
	
	auto_delete(P* p) : _ptr(p) {}
	
	~auto_delete()
	{
		delete _ptr;
		_ptr = NULL;
	}
};