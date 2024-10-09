#ifndef __STREAMCOMPRESS_H
#define __STREAMCOMPRESS_H

#include "gnoctets.h"
#include "gnbyteorder.h"

namespace GNET
{

class Compress
{
	enum { CTRL_OFF_EOB = 0, MPPC_HIST_LEN = 8192 };
	unsigned char history[MPPC_HIST_LEN];
	unsigned char *hash[256];
	unsigned char *histptr;
	unsigned int  legacy_in;

#if defined __GNUC__
	void putbits(unsigned int val, unsigned int n)
	{
		register unsigned char *dummy;
		register unsigned int ll;
		__asm__ __volatile__ ( "movd  %%mm2,%0"    : "=g"(ll) );
		__asm__ __volatile__ ( "movd  %0,%%mm3"    : : "g"( byteorder_32(val << (32-(ll+n))) ) );
		__asm__ __volatile__ ( "movd  %%mm1,%0"    : "=g"(dummy) );
		__asm__ __volatile__ ( "movd  %0,%%mm2"    : : "r"(ll+n) );
		__asm__ __volatile__ ( "por   %%mm3,%%mm0" : : );
		__asm__ __volatile__ ( "psllq $3,%%mm1"    : : );
		__asm__ __volatile__ ( "movd  %0,%%mm3"    : : "r"((ll+n)&24));
		__asm__ __volatile__ ( "paddq %%mm2,%%mm1" : : );
		__asm__ __volatile__ ( "movd  %%mm0,%0"    : "=m"(*dummy) : );
		__asm__ __volatile__ ( "pand  %%mm5,%%mm2" : : );
		__asm__ __volatile__ ( "psrlq $3,%%mm1"    : : );
		__asm__ __volatile__ ( "psrld %%mm3,%%mm0" : : );
	}

	void putlit(unsigned int c)
	{
		register unsigned int bits;
		__asm__ __volatile__ (
				"xor    %1,%1           \n"
				"test   $0x80,%2        \n"
				"setne  %b1             \n"
				"setne  %h0             \n"
				"or     $8,%1           \n"
				"and    $0x17f,%0       \n"
				: "=&q"(c), "=q"(bits) : "0"(c)
				);
		putbits(c, bits);
	}

	void putoff(unsigned int off)
	{
		if (off < 64)		putbits(0x3c0 |off      ,10);
		else if (off < 320)	putbits(0xe00 |(off-64) ,12);
		else			putbits(0xc000|(off-320),16);
	}

	unsigned char* compress_block( unsigned char *obuf, size_t isize )
	{
		unsigned char *r = histptr + isize;
		unsigned char *s = histptr;
		__asm__ __volatile__ ( "movd %0,%%mm1" : : "g"(obuf) );
		__asm__ __volatile__ ( "pxor %%mm0,%%mm0" : : );
		__asm__ __volatile__ ( "pxor %%mm2,%%mm2" : : );
		__asm__ __volatile__ ( "movq %%mm1,%%mm4" : : );
		__asm__ __volatile__ ( "movd %0,%%mm5" : : "r"(7) );
		while ( r - s > 2 )
		{
			unsigned char **q = hash + *s;
			unsigned char *p = *q;
			*q = s;
			if ( p >= s )
			{
				putlit(*histptr++);
				s = histptr;
			}
			else if ( *(unsigned short *)p != *(unsigned short *)s++ )
			{
				putlit(*histptr++);
			}
			else if ( *(p+=2) != *++s )
			{
				putlit(*histptr++);
				s = histptr;
			}
			else
			{
				for(p++,s++ ; s<r && *p==*s; p++,s++);
				unsigned int len = s - histptr;
				histptr = s;
				putoff(s - p);
				if (len < 4)		putbits(                   0, 1);
				else if (len < 8)	putbits(0x08    |(len&0x03 ), 4);
				else if (len < 16)	putbits(0x30    |(len&0x07 ), 6);
				else if (len < 32)	putbits(0xe0    |(len&0x0f ), 8);
				else if (len < 64)	putbits(0x3c0   |(len&0x1f ), 10);
				else if (len < 128)	putbits(0xf80   |(len&0x3f ), 12);
				else if (len < 256)	putbits(0x3f00  |(len&0x7f ), 14);
				else if (len < 512)	putbits(0xfe00  |(len&0xff ), 16);
				else if (len < 1024)	putbits(0x3fc00 |(len&0x1ff), 18);
				else if (len < 2048)	putbits(0xff800 |(len&0x3ff), 20);
				else if (len < 4096)    putbits(0x3ff000|(len&0x7ff), 22);
				else if (len < MPPC_HIST_LEN)    putbits(0xffe000|(len&0xfff), 24);
			}
		}
		switch ( r - s )
		{
		case 2: putlit(*histptr++);
		case 1: putlit(*histptr++);
		}
		putoff(CTRL_OFF_EOB);
		unsigned int l;
		__asm__ __volatile__ ( "movd %%mm2,%0" : "=g"(l) );
		if ( l ) putbits(0, 8 - l);
		__asm__ __volatile__ ( "movd %%mm1,%0" : "=g"(obuf) );
		__asm__ __volatile__ ( "emms" : : );
		legacy_in = 0;
		return obuf;
	}
#else
	void putbits(unsigned char *&buf, unsigned int val, unsigned int n, unsigned int& l)
	{
		l += n;
		*(unsigned int *)buf = byteorder_32(val << (32 - l)) | *buf;
		buf += l >> 3;
		l &= 7;
	}

	void putlit(unsigned char *&buf, unsigned int c, unsigned int& l)
	{
		if (c < 0x80) putbits(buf, c, 8, l);
		else putbits(buf, c&0x7f|0x100, 9, l);
	}

	void putoff(unsigned char *&buf, unsigned int off, unsigned int& l)
	{
		if (off < 64)		putbits(buf, 0x3c0 |off      ,10, l);
		else if (off < 320)	putbits(buf, 0xe00 |(off-64) ,12, l);
		else			putbits(buf, 0xc000|(off-320),16, l);
	}

	unsigned char* compress_block( unsigned char *obuf, size_t isize )
	{
		unsigned char *r = histptr + isize;
		unsigned char *s = histptr;
		unsigned l = 0;
		*obuf = 0;
		while ( r - s > 2 )
		{
			unsigned char **q = hash + *s;
			unsigned char *p = *q;
			*q = s;
			if ( p >= s )
			{
				putlit(obuf, *histptr++, l);
				s = histptr;
			}
			else if ( *(unsigned short *)p != *(unsigned short *)s++ )
			{
				putlit(obuf, *histptr++, l);
			}
			else if ( *(p+=2) != *++s )
			{
				putlit(obuf, *histptr++, l);
				s = histptr;
			}
			else
			{
				for(p++,s++ ; s<r && *p==*s; p++,s++);
				unsigned int len = s - histptr;
				histptr = s;
				putoff(obuf, s - p, l);
				if (len < 4)		putbits(obuf,                    0, 1, l);
				else if (len < 8)	putbits(obuf, 0x08    |(len&0x03 ), 4, l);
				else if (len < 16)	putbits(obuf, 0x30    |(len&0x07 ), 6, l);
				else if (len < 32)	putbits(obuf, 0xe0    |(len&0x0f ), 8, l);
				else if (len < 64)	putbits(obuf, 0x3c0   |(len&0x1f ), 10,l);
				else if (len < 128)	putbits(obuf, 0xf80   |(len&0x3f ), 12,l);
				else if (len < 256)	putbits(obuf, 0x3f00  |(len&0x7f ), 14,l);
				else if (len < 512)	putbits(obuf, 0xfe00  |(len&0xff ), 16,l);
				else if (len < 1024)	putbits(obuf, 0x3fc00 |(len&0x1ff), 18,l);
				else if (len < 2048)	putbits(obuf, 0xff800 |(len&0x3ff), 20,l);
				else if (len < 4096)    putbits(obuf, 0x3ff000|(len&0x7ff), 22,l);
				else if (len < MPPC_HIST_LEN)    putbits(obuf, 0xffe000|(len&0xfff), 24,l);
			}
		}
		switch ( r - s )
		{
		case 2: putlit(obuf, *histptr++, l);
		case 1: putlit(obuf, *histptr++, l);
		}
		putoff(obuf, CTRL_OFF_EOB, l);
		if ( l ) putbits(obuf, 0, 8 - l, l);
		legacy_in = 0;
		return obuf;
	}
#endif
public:
	Compress() 
	{ 
		histptr = history;
		legacy_in = 0;
		for ( int i = 0; i < 256; i++ ) 
			hash[i] = history; 
	}
	Compress(const Compress& rhs) : histptr(history + (rhs.histptr - rhs.history)), legacy_in(rhs.legacy_in)
	{
		memcpy ( history, rhs.history, sizeof(history) );
		for ( int i = 0; i < 256; i++ ) hash[i] = history + ( rhs.hash[i] - rhs.history );
	}
	Octets& Update(Octets &in)
	{
		Octets out;
		unsigned char *ibuf = (unsigned char *)in.begin();
		size_t isize = in.size();
		size_t remain = MPPC_HIST_LEN - ( histptr - history ) - legacy_in;
		if ( isize >= remain )
		{
			out.resize( (isize + legacy_in)*9/8 + 6 );
			unsigned char *obuf = (unsigned char *)out.begin();
			memcpy( histptr + legacy_in, ibuf, remain );
			isize -= remain;
			ibuf  += remain;
			obuf   = compress_block(obuf, remain + legacy_in);
			histptr= history;
			for ( ; isize >= MPPC_HIST_LEN; isize -= MPPC_HIST_LEN, ibuf += MPPC_HIST_LEN )
			{
				memcpy( histptr, ibuf, MPPC_HIST_LEN );
				obuf    = compress_block(obuf, MPPC_HIST_LEN);
				histptr = history;
			}
			out.resize( obuf - (unsigned char *)out.begin() );
		}
		memcpy ( histptr + legacy_in, ibuf, isize );
		legacy_in += isize;
		return in.swap(out);
	}

	Octets& Final(Octets &in)
	{
		Octets& out = Update(in);
		size_t osize = out.size();
		out.reserve( osize + legacy_in * 9 / 8 + 6 );
		unsigned char *obuf = ((unsigned char *)out.begin()) + osize;
		obuf = compress_block( obuf, legacy_in );
		out.resize( obuf - (unsigned char *)out.begin() );
		return in.swap(out);
	}
};

class Decompress
{
	enum { CTRL_OFF_EOB = 0, MPPC_HIST_LEN = 8192 };
	unsigned char history[MPPC_HIST_LEN];
	unsigned char *histptr;
	unsigned int l, adjust_l;
	unsigned int blen, blen_total;
	unsigned char *rptr, *adjust_rptr;
	Octets   legacy_in;

	bool passbits(const unsigned int n)
	{
		l    += n;
		blen += n;
		if ( blen < blen_total )
			return true;
		l    = adjust_l;
		rptr = adjust_rptr;
		return false;
	}

	unsigned int fetch()
	{
		rptr += l >> 3;
		l &= 7;
		return byteorder_32(*(unsigned int *)rptr) << l;
	}

	static inline void lamecopy(unsigned char *dst, unsigned char *src, unsigned int len)
	{
		if ( dst - src > 3 )
		{
			while ( len > 3 )
			{
				*(unsigned int *)dst = *(unsigned int *)src;
				dst += 4;
				src += 4;
				len -= 4;
			}
		}
		while (len--) *dst++ = *src++;
	}

public:
	Decompress() : histptr(history), l(0) { } 
	Decompress(const Decompress& rhs) : histptr(history + (rhs.histptr - rhs.history)), l(rhs.l), 
		adjust_l(rhs.adjust_l), blen(rhs.blen), blen_total(rhs.blen_total),
		rptr(rhs.rptr), adjust_rptr(rhs.adjust_rptr), legacy_in(rhs.legacy_in)
	{
		memcpy( history, rhs.history, sizeof(history) );
	}

	Octets& Update(Octets &in)
	{
		legacy_in.insert( legacy_in.end(), in.begin(), in.end() );
		blen_total = legacy_in.size() * 8 - l;
		legacy_in.reserve( legacy_in.size() + 3 );
		rptr = (unsigned char *)legacy_in.begin();
		blen = 7;
		Octets &out = in;
		out.clear();
		unsigned char *histhead = histptr;
		while ( blen_total > blen )
		{
			adjust_l    = l;
			adjust_rptr = rptr;
			unsigned int val = fetch();
			if (val < 0x80000000)
			{
				if ( !passbits(8) ) break;
				*histptr++ = (val>>24);
				continue;
			}
			if (val < 0xc0000000)
			{
				if ( !passbits(9) ) break;
				*histptr++ = ((val>>23)|0x80)&0xff;
				continue;
			}

			unsigned int off, len;
			if ( val >= 0xf0000000 )
			{
				if ( !passbits(10) ) break;
				off = (val>>22)&0x3f;
				if ( off == CTRL_OFF_EOB )
				{
					unsigned int advance = 8 - (l&7);
					if ( advance < 8 )
						if ( !passbits(advance) ) break;
					out.insert( out.end(), histhead, histptr - histhead );
					if ( histptr - history == MPPC_HIST_LEN )
						histptr = history;
					histhead = histptr;
					continue;
				}
			}
			else if (val >= 0xe0000000)
			{
				if ( !passbits(12) ) break;
				off = ((val>>20)&0xff) + 64;
			}
			else if (val >= 0xc0000000)
			{
				if ( !passbits(16) ) break;
				off = ((val>>16)&0x1fff) + 320;
			}

			val = fetch();
			if ( val < 0x80000000 )
			{
				if ( !passbits(1) ) break;
				len = 3;
			}
			else if ( val < 0xc0000000 )
			{
				if ( !passbits(4) ) break;
				len = 4|((val>>28)&3);
			}
			else if ( val < 0xe0000000 )
			{
				if ( !passbits(6) ) break;
				len = 8|((val>>26)&7);
			}
			else if ( val < 0xf0000000 )
			{
				if ( !passbits(8) ) break;
				len = 16|((val>>24)&15);
			}
			else if ( val < 0xf8000000 )
			{
				if ( !passbits(10) ) break;
				len = 32|((val>>22)&0x1f);
			}
			else if ( val < 0xfc000000 )
			{
				if ( !passbits(12) ) break;
				len = 64|((val>>20)&0x3f);
			}
			else if ( val < 0xfe000000 )
			{
				if ( !passbits(14) ) break;
				len = 128|((val>>18)&0x7f);
			}
			else if ( val < 0xff000000 )
			{
				if ( !passbits(16) ) break;
				len = 256|((val>>16)&0xff);
			}
			else if ( val < 0xff800000 )
			{
				if ( !passbits(18) ) break;
				len = 0x200|((val>>14)&0x1ff);
			}
			else if ( val < 0xffc00000 )
			{
				if ( !passbits(20) ) break;
				len = 0x400|((val>>12)&0x3ff);
			}
			else if ( val < 0xffe00000 )
			{
				if ( !passbits(22) ) break;
				len = 0x800|((val>>10)&0x7ff);
			}
			else if ( val < 0xfff00000 )
			{
				if ( !passbits(24) ) break;
				len = 0x1000|((val>>8)&0xfff);
			}
			else
			{
				l    = adjust_l;
				rptr = adjust_rptr;
				break;
			}
			if ( histptr - off < history || histptr + len > history + MPPC_HIST_LEN )
				break;
			lamecopy(histptr, histptr - off, len);
			histptr += len;
		}
		out.insert( out.end(), histhead, histptr - histhead );
		legacy_in.erase( legacy_in.begin(), rptr );
		return out;
	}
};

};
#endif
