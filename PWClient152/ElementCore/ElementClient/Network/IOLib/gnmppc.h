#ifndef __MPPC_H
#define __MPPC_H

#include "gnoctets.h"

#define	DECOMP_ERROR		-1	/* error detected before decomp. */

namespace GNET
{

class mppc
{
public:
	static size_t compressBound( size_t sourcelen )
	{
		return (((sourcelen*9)/8)+1)+2+3;
	}

	static int compress( unsigned char *dest, int *destLen, const unsigned char *source, int sourceLen )
	{
		int len = mppc::mppc_compress(source,dest,sourceLen,*destLen);
		if( len > 0 && len <= *destLen )
		{
			*destLen = len;
			return 0;
		}
		return -1;
	}

	static int uncompress( unsigned char *dest, int *destLen, const unsigned char *source, int sourceLen)
	{
		int len=mppc::mppc_decompress(source,dest,sourceLen,*destLen);
		if( len > 0 && len <= *destLen )
		{
			*destLen = len;
			return 0;
		}
		return -1;
	}

	static int compress2( unsigned char *dest, int *destLen, const unsigned char *source, int sourceLen )
	{
		int dleft = *destLen;
		*destLen = 0;
		while( sourceLen > 0 && dleft > 2 )
		{
			int slen = (sourceLen > 8192 ? 8192 : sourceLen);
			int dlen = mppc::mppc_compress(source,dest+2,slen,dleft-2);
			if( dlen > 0 && dlen < slen && dlen <= dleft-2 )
			{
				*((unsigned short*)dest) = (dlen|0x8000);
			}
			else if( slen > 0 && slen <= dleft-2 )
			{
				dlen = slen;
				memcpy( dest+2, source, dlen );
				*((unsigned short*)dest) = dlen;
			}
			else
			{
				return -1;
			}
			source += slen;
			sourceLen -= slen;
			dest += (dlen+2);
			dleft -= (dlen+2);
			*destLen += (dlen+2);
		}
		return (0 == sourceLen ? 0 : -1);
	}

	static int uncompress2( unsigned char *dest, int *destLen, const unsigned char *source, int sourceLen)
	{
		int dleft = *destLen;
		*destLen = 0;
		while( sourceLen > 2 && dleft > 0 )
		{
			int slen = (*((unsigned short*)source) & 0x7FFF);
			if( slen <= 0 || slen+2 > sourceLen || slen > 8192 )
				return -1;

			int dlen;
			if( *((unsigned short*)source) & 0x8000 )
			{
				dlen = mppc::mppc_decompress(source+2,dest,slen,dleft);
				if( dlen <= 0 || dlen > dleft || dlen > 8192 )
					return -1;
			}
			else
			{
				dlen = slen;
				if( dlen > dleft )
					return -1;
				memcpy( dest, source+2, dlen );
			}
			source += (slen+2);
			sourceLen -= (slen+2);
			dest += dlen;
			dleft -= dlen;
			*destLen += dlen;
		}
		return (0 == sourceLen ? 0 : -1);
	}

private:
	#define MPPE_HIST_LEN		8192	/* MPPC history size */

	static inline void putbits(unsigned char *&buf, unsigned int val, unsigned int n, unsigned int& l)
	{
		l += n;
		*(unsigned int *)buf = byteorder_32(val << (32 - l)) | *buf;
		buf += l >> 3;
		l &= 7;
	}

	static inline void putlit(unsigned char *&buf, unsigned int c, unsigned int& l)
	{
		if (c < 0x80)
			putbits(buf, c, 8, l);
		else
			putbits(buf, c&0x7f|0x100, 9, l);
	}

	static inline void putoff(unsigned char *&buf, unsigned int off, unsigned int& l)
	{
		if (off < 64)		putbits(buf, 0x3c0 |off      ,10, l);
		else if (off < 320)	putbits(buf, 0xe00 |(off-64) ,12, l);
		else			putbits(buf, 0xc000|(off-320),16, l);
	}

	static int mppc_compress(const unsigned char *ibuf, unsigned char *obuf, int isize, int osize)
	{
		const unsigned char* hash[65536];
		unsigned char *obegin = obuf;
		const unsigned char *ibegin = ibuf;
		const unsigned char *r = ibuf + isize;
		const unsigned char *s = ibuf;
		*obuf = 0;
		unsigned int l = 0;
		while ( r - s > 2 )
		{
			const unsigned char **q = hash + *(unsigned short*)s;
			const unsigned char *p = *q;
			*q = s;
			if ( p < ibegin )
			{
				putlit(obuf, *ibuf++, l);
				s = ibuf;
			}
			else if ( p >= s )
			{
				putlit(obuf, *ibuf++, l);
				s = ibuf;
			}
			else if ( *(unsigned short *)p != *(unsigned short *)s++ )
			{
				putlit(obuf, *ibuf++, l);
			}
			else if ( *(p+=2) != *++s )
			{
				putlit(obuf, *ibuf++, l);
				s = ibuf;
			}
			else
			{
				for ( p++, s++; s < r && *p == *s; p++, s++ );
				unsigned int len = s - ibuf;
				ibuf   = s;
				putoff(obuf, s - p, l);
				if (len < 4)		putbits(obuf,                    0, 1,  l);
				else if (len < 8)	putbits(obuf, 0x08    |(len&0x03 ), 4,  l);
				else if (len < 16)	putbits(obuf, 0x30    |(len&0x07 ), 6,  l);
				else if (len < 32)	putbits(obuf, 0xe0    |(len&0x0f ), 8,  l);
				else if (len < 64)	putbits(obuf, 0x3c0   |(len&0x1f ), 10, l);
				else if (len < 128)	putbits(obuf, 0xf80   |(len&0x3f ), 12, l);
				else if (len < 256)	putbits(obuf, 0x3f00  |(len&0x7f ), 14, l);
				else if (len < 512)	putbits(obuf, 0xfe00  |(len&0xff ), 16, l);
				else if (len < 1024)	putbits(obuf, 0x3fc00 |(len&0x1ff), 18, l);
				else if (len < 2048)	putbits(obuf, 0xff800 |(len&0x3ff), 20, l);
				else if (len < 4096)    putbits(obuf, 0x3ff000|(len&0x7ff), 22, l);
				else if (len < 8192)    putbits(obuf, 0xffe000|(len&0xfff), 24, l);
			}
		}
		switch ( r - s )
		{
		case 2: putlit(obuf, *ibuf++, l);
		case 1: putlit(obuf, *ibuf++, l);
		}
		if ( l ) putbits(obuf, 0, 8 - l, l);
		return obuf - obegin;
	}

	/***************************/
	/*** Decompression stuff ***/
	/***************************/
	static inline void passbits(const unsigned int n, unsigned int& l, unsigned int& blen)
	{
		l    += n;
		blen += n;
	}

	static inline unsigned int fetch(const unsigned char *&buf, unsigned int& l)
	{
		buf += l >> 3;
		l &= 7;
		return byteorder_32(*(unsigned int *)buf) << l; 
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

	static int mppc_decompress(const unsigned char *ibuf, unsigned char *obuf, int isize, int osize)
	{
		if ( isize > (MPPE_HIST_LEN * 9) / 8 + 1 )
			return DECOMP_ERROR;
		unsigned char input[2 * MPPE_HIST_LEN];
		memcpy( input, ibuf, isize );
		ibuf = input;
		const unsigned char *obegin = obuf;
		const unsigned char *oend   = obuf + osize;
		const unsigned int blen_total = isize * 8;
		unsigned int l = 0, blen = 7;

		while ( blen_total > blen )
		{
			unsigned int val = fetch(ibuf, l);
			if (val < 0x80000000)
			{
				if (obuf >= oend) return DECOMP_ERROR;
				*obuf++ = (val>>24);
				passbits(8, l, blen);
				continue;
			}
			if (val < 0xc0000000)
			{
				if (obuf >= oend) return DECOMP_ERROR;
				*obuf++ = ((val>>23)|0x80)&0xff;
				passbits(9, l, blen);
				continue;
			}
			unsigned int off, len;
			if ( val >= 0xf0000000 )
			{
				off = (val>>22)&0x3f;
				val <<= 10;	
				if ( val < 0x80000000 )
				{
					len = 3;
					passbits(11, l, blen);
				}
				else if ( val < 0xc0000000 )
				{
					len = 4|((val>>28)&3);
					passbits(14, l, blen);
				}
				else if ( val < 0xe0000000 )
				{
					len = 8|((val>>26)&7);
					passbits(16, l, blen);
				}
				else if ( val < 0xf0000000 )
				{
					len = 16|((val>>24)&15);
					passbits(18, l, blen);
				}
				else if ( val < 0xf8000000 )
				{
					len = 32|((val>>22)&0x1f);
					passbits(20, l, blen);
				}
				else if ( val < 0xfc000000 )
				{
					len = 64|((val>>20)&0x3f);
					passbits(22, l, blen);
				}
				else if ( val < 0xfe000000 )
				{
					len = 128|((val>>18)&0x7f);
					passbits(24, l, blen);
				}
				else
				{
					passbits(10, l, blen);
					val = fetch(ibuf, l);
					if ( val < 0xff000000 )
					{
						len = 256|((val>>16)&0xff);
						passbits(16, l, blen);
					}
					else if ( val < 0xff800000 )
					{
						len = 0x200|((val>>14)&0x1ff);
						passbits(18, l, blen);
					}
					else if ( val < 0xffc00000 )
					{
						len = 0x400|((val>>12)&0x3ff);
						passbits(20, l, blen);
					}
					else if ( val < 0xffe00000 )
					{
						len = 0x800|((val>>10)&0x7ff);
						passbits(22, l, blen);
					}
					else if ( val < 0xfff00000 )
					{
						len = 0x1000|((val>>8)&0xfff);
						passbits(24, l, blen);
					}
					else
						return DECOMP_ERROR;
				}
			}
			else if (val >= 0xe0000000)
			{
				off = ((val>>20)&0xff) + 64;
				val <<= 12;
				if ( val < 0x80000000 )
				{
					len = 3;
					passbits(13, l, blen);
				}
				else if ( val < 0xc0000000 )
				{
					len = 4|((val>>28)&3);
					passbits(16, l, blen);
				}
				else if ( val < 0xe0000000 )
				{
					len = 8|((val>>26)&7);
					passbits(18, l, blen);
				}
				else if ( val < 0xf0000000 )
				{
					len = 16|((val>>24)&15);
					passbits(20, l, blen);
				}
				else if ( val < 0xf8000000 )
				{
					len = 32|((val>>22)&0x1f);
					passbits(22, l, blen);
				}
				else if ( val < 0xfc000000 )
				{
					len = 64|((val>>20)&0x3f);
					passbits(24, l, blen);
				}
				else 
				{
					passbits(12, l, blen);
					val = fetch(ibuf, l);
					if ( val < 0xfe000000 )
					{
						len = 128|((val>>18)&0x7f);
						passbits(14, l, blen);
					}
					else if ( val < 0xff000000 )
					{
						len = 256|((val>>16)&0xff);
						passbits(16, l, blen);
					}
					else if ( val < 0xff800000 )
					{
						len = 0x200|((val>>14)&0x1ff);
						passbits(18, l, blen);
					}
					else if ( val < 0xffc00000 )
					{
						len = 0x400|((val>>12)&0x3ff);
						passbits(20, l, blen);
					}
					else if ( val < 0xffe00000 )
					{
						len = 0x800|((val>>10)&0x7ff);
						passbits(22, l, blen);
					}
					else if ( val < 0xfff00000 )
					{
						len = 0x1000|((val>>8)&0xfff);
						passbits(24, l, blen);
					}
					else
						return DECOMP_ERROR;
				}		
			}
			else 
			{
				off = ((val>>16)&0x1fff) + 320;
				val <<= 16;
				if ( val < 0x80000000 )
				{
					len = 3;
					passbits(17, l, blen);
				}
				else if ( val < 0xc0000000 )
				{
					len = 4|((val>>28)&3);
					passbits(20, l, blen);
				}
				else if ( val < 0xe0000000 )
				{
					len = 8|((val>>26)&7);
					passbits(22, l, blen);
				}
				else if ( val < 0xf0000000 )
				{
					len = 16|((val>>24)&15);
					passbits(24, l, blen);
				}
				else
				{
					passbits(16, l, blen);
					val = fetch(ibuf, l);
					if ( val < 0xf8000000 )
					{
						len = 32|((val>>22)&0x1f);
						passbits(10, l, blen);
					}
					else if ( val < 0xfc000000 )
					{
						len = 64|((val>>20)&0x3f);
						passbits(12, l, blen);
					}
					else if ( val < 0xfe000000 )
					{
						len = 128|((val>>18)&0x7f);
						passbits(14, l, blen);
					}
					else if ( val < 0xff000000 )
					{
						len = 256|((val>>16)&0xff);
						passbits(16, l, blen);
					}
					else if ( val < 0xff800000 )
					{
						len = 0x200|((val>>14)&0x1ff);
						passbits(18, l, blen);
					}
					else if ( val < 0xffc00000 )
					{
						len = 0x400|((val>>12)&0x3ff);
						passbits(20, l, blen);
					}
					else if ( val < 0xffe00000 )
					{
						len = 0x800|((val>>10)&0x7ff);
						passbits(22, l, blen);
					}
					else if ( val < 0xfff00000 )
					{
						len = 0x1000|((val>>8)&0xfff);
						passbits(24, l, blen);
					}
					else
						return DECOMP_ERROR;
				}
			}
			if ( obuf - off < obegin || obuf + len > oend )
				return DECOMP_ERROR;
			lamecopy(obuf, obuf - off, len);
			obuf += len;
		}
		return obuf - obegin;
	}

};

}

#endif
