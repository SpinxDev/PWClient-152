#include <stdlib.h>
#include <assert.h>
#include "gnstring.h"

namespace GNET
{
	GnetString::GnetString() : capacity(0), length(0), data(NULL) { }
	GnetString::GnetString(const wchar_t* str) : capacity(0), data(NULL)
	{
		length = wcslen(str);
		reserve(length+1);
		wcscpy(data, str);
	}
	GnetString::GnetString(const GnetString& rhs) : capacity(0), data(NULL)
	{
		length = rhs.length;
		reserve(length);
		wcscpy(data, rhs.data);
	}

	GnetString::~GnetString()
	{
		if(data)
		{
			free(data);
			data = NULL;
		}
	}

	wchar_t* GnetString::reserve(size_t size)
	{
		if(size <= capacity)
			return data;
		else
			for (size--,capacity = 2; size >>= 1; capacity <<= 1);
		data = (wchar_t*)realloc(data, capacity*sizeof(wchar_t));
		return data;
	}

	GnetString& GnetString::operator = (const wchar_t* str) 
	{ 
		length = wcslen(str);
		reserve(length+1);
		wcscpy(data, str);
		return *this;
	}

	const wchar_t* GnetString::c_str() const 
	{
		return data;
	}

	size_t GnetString::size() const
	{
		return length;
	}

	GnetString& GnetString::operator << (int x)  
	{ 
		reserve(length+20);
		length += swprintf(data+length, L"%d", x); 
		return *this;
	}

	GnetString& GnetString::operator << (float x)  
	{ 
		reserve(length+20);
		length += _snwprintf(data+length, 19, L"%.1f", x); 
		return *this;
	}

	GnetString& GnetString::operator << (double x)  
	{ 
		reserve(length+20);
		length += _snwprintf(data+length, 19, L"%.1f", x); 
		return *this;
	}

	GnetString& GnetString::operator << (const wchar_t* str)  
	{ 
		int len = wcslen(str);
		reserve(length+len+1);
		wcscpy(data+length, str);
		length += len;
		return *this;
	}
}

