#ifndef __BYTEORDER_H
#define __BYTEORDER_H

namespace GNET
{

#if defined BYTE_ORDER_BIG_ENDIAN
	#define byteorder_16(x)	(x)
	#define byteorder_32(x)	(x)
	#define byteorder_64(x)	(x)
#elif defined __GNUC__
	inline unsigned short byteorder_16(unsigned short x)
	{
		__asm__ ("xchg %b0, %h0" : "=q"(x) : "0"(x));
		return x;
	}
	inline unsigned long byteorder_32(unsigned long x)
	{
		__asm__ ("bswap %0" : "=r"(x) : "0"(x));
		return x;
	}
	inline unsigned long long byteorder_64(unsigned long long x)
	{
		union
		{
			unsigned long long __ll;
			unsigned long __l[2];
		} i, o;
		i.__ll = x;
		o.__l[0] = byteorder_32(i.__l[1]);
		o.__l[1] = byteorder_32(i.__l[0]);
		return o.__ll;
	}
#elif defined WIN32
	inline unsigned __int16 byteorder_16(unsigned __int16 x)
	{
		__asm ror x, 8
		return x;
	}
	inline unsigned __int32 byteorder_32(unsigned __int32 x)
	{
		__asm mov eax, x
		__asm bswap eax
		__asm mov x, eax
		return x;
	}
	inline unsigned __int64 byteorder_64(unsigned __int64 x)
	{
		union
		{
			unsigned __int64 __ll;
			unsigned __int32 __l[2];
		} i, o;
		i.__ll = x;
		o.__l[0] = byteorder_32(i.__l[1]);
		o.__l[1] = byteorder_32(i.__l[0]);
		return o.__ll;
	}
#endif

};

#endif
