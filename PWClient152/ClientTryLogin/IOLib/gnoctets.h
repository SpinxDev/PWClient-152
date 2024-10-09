#ifndef __OCTETS_H
#define __OCTETS_H

#include <stdlib.h>
#include <algorithm>

namespace GNET
{

class Octets
{
	void *base, *high;
	size_t cap;
public:
	Octets& reserve(size_t size)
	{
		if (size > cap)
		{
			for (size--,cap = 2; size >>= 1; cap <<= 1);
			size = (char*)high - (char*)base;
			base = realloc(base, cap);
			high = (char*)base + size;
		}
		return *this;
	}
	Octets& replace(const void *data, size_t size)
	{
		reserve(size);
		memmove(base, data, size);
		high = (char*)base + size;
		return *this;
	}
	virtual ~Octets () { free(base); }
	Octets () : base(0), high(0), cap(0) {}
	Octets (size_t size) : base(0), high(0), cap(0) { reserve(size); }
	Octets (const void *x, size_t size) : base(0), high(0), cap(0) { replace(x, size); }
	Octets (const void *x, const void *y) : base(0), high(0), cap(0) { replace(x, (char*)y - (char*)x); }
	Octets (const Octets &x) : base(0), high(0), cap(0) { if (x.size()) replace(x.begin(), x.size()); }
	Octets& operator = (const Octets&x) { return replace(x.begin(), x.size()); }
	bool operator == (const Octets &x) const { return x.size() == size() && !memcmp(x.base, base, size()); }
	bool operator != (const Octets &x) const { return ! operator == (x); }
	Octets& swap(Octets &x) { std::swap(base, x.base); std::swap(high, x.high); std::swap(cap, x.cap); return *this; }
	void* begin() { return base; }
	void* end()   { return high; }
	const void* begin() const { return base; }
	const void* end()   const { return high; }
	size_t size() const { return (char*)high - (char*)base; }
	size_t capacity() const { return cap; }
	Octets& clear() { high = base; return *this; }
	Octets& erase(void *x, void *y)
	{
		if (x != y)
		{
			memmove(x, y, (char*)high - (char*)y);
			high = (char*)high - ((char*)y - (char*)x);
		}	
		return *this;
	}
	Octets& insert(void *pos, const void *x, size_t size)
	{
		size_t off = (char*)pos - (char*)base;
		reserve(size + (char*)high - (char*)base);
		if (pos)
		{
			pos = (char*)base + off;
			memmove((char*)pos + size, pos, (char*)high - (char*)pos);
			memmove(pos, x, size);
			high = (char*)high + size;
		} else {
			memmove(base, x, size);
			high = (char*)base + size;
		}
		return *this;
	}
	Octets& insert(void *pos, const void *x, const void *y) { insert(pos, x, (char*)y - (char*)x); return *this; }
	Octets& resize(size_t size) { reserve(size); high = (char*)base + size; return *this; }
};

};

#endif
