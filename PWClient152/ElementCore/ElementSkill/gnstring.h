#ifndef __GNET_STRING_H
#define __GNET_STRING_H

#include <wchar.h>
#include <string.h>

namespace GNET
{
class GnetString
{
	size_t capacity;
	size_t length;
	wchar_t* data;

	wchar_t* reserve(size_t size);
public:
	GnetString();
	GnetString(const wchar_t* str);
	GnetString(const GnetString&);
	~GnetString();

	GnetString& operator = (const wchar_t* str);
	const wchar_t* c_str() const ;
	size_t size() const;

	GnetString& operator << (int x);
	GnetString& operator << (float x);
	GnetString& operator << (double x);
	GnetString& operator << (const wchar_t* str);
};

};

#endif
