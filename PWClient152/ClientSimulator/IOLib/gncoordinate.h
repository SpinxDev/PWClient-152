#ifndef __COORDINATE_H
#define __COORDINATE_H

#include "gnoctets.h"
#include "gnbyteorder.h"

namespace GNET
{

int GetFirstVector( int type, const char * data, int len);
int GetNextVector( int type, const char * data, int len);

class Coordinate
{
public:
	static inline void putbits(unsigned char *&buf, unsigned int val, unsigned int n, unsigned int& l)
	{
		l += n;
		*(unsigned int *)buf = byteorder_32(val << (32 - l)) | *buf;
		buf += l >> 3;
		l &= 7;
	}

	static inline unsigned int getbits(const unsigned char *&buf, const unsigned int n, unsigned int& l)
	{
		unsigned int res = *buf & ( 0xff >> (8 - l) );
		l -= n;
		while ( l >> 31 )
		{
			l += 8;
			res <<= 8;
			res  |= *++buf;
		}
		return res >> l;
	}

	static inline void compressfloat( float f, unsigned char *&buf, unsigned int& l )
	{
		float t;
		if (f >= 0)
		{
			putbits( buf, 0, 1, l );
			t = f;
		}
		else
		{
			putbits( buf, 1, 1, l );
			t = -f;
		}

		if (t < 3.2)
		{
			putbits( buf, 0, 2, l );
			putbits( buf, (unsigned int)(t*10), 5, l);
		}
		else if (t < 102.4)
		{
			putbits( buf, 1, 2, l );
			putbits( buf, (unsigned int)(t*10), 10, l);
		}
		else if (t < 819.2)
		{
			putbits( buf, 2, 2, l );
			putbits( buf, (unsigned int)(t*10), 13, l);
		}
		else
		{
			putbits( buf, 3, 2, l );
			unsigned char * p = (unsigned char*)&t;
			putbits( buf, *p++, 8, l);
			putbits( buf, *p++, 8, l);
			putbits( buf, *p++, 8, l);
			putbits( buf, *p++, 8, l);
		}
	}

	static inline float uncompressfloat( const unsigned char *&buf, int len, unsigned int& l )
	{
		if( len < 1 || (1==len&&l<3) )	return 0;

		unsigned int sign = getbits(buf, 1, l);
		unsigned int mode = getbits(buf, 2, l);
		if( 0 == mode )
		{
			if( len < 1 || (1==len&&l<5) )	return 0;
			return sign ? -0.1f*getbits(buf, 5, l) : 0.1f*getbits(buf, 5, l);
		}
		else if( 1 == mode )
		{
			if( len < 2 || (2==len&&l<2) )	return 0;
			return sign ? -0.1f*getbits(buf, 10, l) : 0.1f*getbits(buf, 10, l);
		}
		else if( 2 == mode )
		{
			if( len < 2 || (2==len&&l<5) )	return 0;
			return sign ? -0.1f*getbits(buf, 13, l) : 0.1f*getbits(buf, 13, l);
		}
		else
		{
			if( len < 4 || (4==len&&l<8) )	return 0;
			float t = 0.f;
			unsigned char * p = (unsigned char*)&t;
			*p++ = getbits( buf, 8, l );
			*p++ = getbits( buf, 8, l );
			*p++ = getbits( buf, 8, l );
			*p++ = getbits( buf, 8, l );
			return sign ? -1.0f*t : t;
		}
	}

	static inline int compress( const unsigned char * coordinate, int len, Octets & dest )
	{
		if( len < 12 )
			return 0;

		unsigned char buffer[12];
		memset( buffer, 0, sizeof(buffer) );
		unsigned char *buf = buffer;
		unsigned int l = 0;

		compressfloat( *((float*)coordinate), buf, l );
		compressfloat( *(((float*)coordinate)+1), buf, l );
		compressfloat( *(((float*)coordinate)+2), buf, l );
		dest.insert( dest.end(), buffer, 0==l ? buf-buffer : buf-buffer+1 );
		return 12;
	}

	static inline int uncompress( const unsigned char * coordinate, int len, Octets & dest )
	{
		unsigned int l = 8;

		const unsigned char * buf = coordinate;
		float xf = uncompressfloat( buf, len, l );
		float yf = uncompressfloat( buf, len-(buf-coordinate), l );
		float zf = uncompressfloat( buf, len-(buf-coordinate), l );
		dest.insert( dest.end(), &xf, sizeof(xf) );
		dest.insert( dest.end(), &yf, sizeof(yf) );
		dest.insert( dest.end(), &zf, sizeof(zf) );
		return buf-coordinate+1;
	}

	static int compress( int type, const Octets & src, Octets & dest )
	{
		dest.reserve( dest.size()+src.size() );
		int offset = GetFirstVector( type, (const char*)src.begin(), src.size() );
		int pos = 0;
		while( offset >= 0 && pos+offset+12<=(int)src.size() )
		{
			if( offset > 0 )
			{
				dest.insert( dest.end(), ((const char*)src.begin())+pos, offset );
				pos += offset;
			}

			int t = compress( ((const unsigned char*)src.begin())+pos, 12, dest );
			if( t <= 0 )
				break;
			pos += t;

			offset = GetNextVector( type, ((const char*)src.begin())+pos, src.size()-pos );
		}
		if( src.size()-pos > 0 )
			dest.insert( dest.end(), ((const char*)src.begin())+pos, src.size()-pos );
		return 0;
	}

	static int uncompress( int type, const Octets & src, Octets & dest )
	{
		dest.reserve( dest.size()+(src.size()<<2) );
		int offset = GetFirstVector( type, (const char*)src.begin(), src.size() );
		int pos = 0;
		while( offset >= 0 && pos+offset<(int)src.size() )
		{
			if( offset > 0 )
			{
				dest.insert( dest.end(), ((const char*)src.begin())+pos, offset );
				pos += offset;
			}

			int t = uncompress( ((const unsigned char*)src.begin())+pos, src.size()-pos, dest );
			if( t <= 0 )
				break;
			pos += t;

			offset = GetNextVector( type, ((const char*)src.begin())+pos, src.size()-pos );
		}
		if( src.size()-pos > 0 )
			dest.insert( dest.end(), ((const char*)src.begin())+pos, src.size()-pos );
		return 0;
	}
};

};

#endif

