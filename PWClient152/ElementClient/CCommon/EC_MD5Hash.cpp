//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "EC_MD5Hash.h"

namespace PATCH
{

	void MD5Hash::transform (const unsigned char block[64])
	{
		#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
		#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
		#define H(x, y, z) ((x) ^ (y) ^ (z))
		#define I(x, y, z) ((y) ^ ((x) | (~z)))
		#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

		unsigned int a = state[0], b = state[1], c = state[2], d = state[3], x[16];

		memcpy (x, block, 64);

		/* Round 1 */
		#define FF(a, b, c, d, x, s, ac) { \
			(a) += F ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		FF (a, b, c, d, x[ 0],   7, 0xd76aa478); /* 1 */
		FF (d, a, b, c, x[ 1],  12, 0xe8c7b756); /* 2 */
		FF (c, d, a, b, x[ 2],  17, 0x242070db); /* 3 */
		FF (b, c, d, a, x[ 3],  22, 0xc1bdceee); /* 4 */
		FF (a, b, c, d, x[ 4],   7, 0xf57c0faf); /* 5 */
		FF (d, a, b, c, x[ 5],  12, 0x4787c62a); /* 6 */
		FF (c, d, a, b, x[ 6],  17, 0xa8304613); /* 7 */
		FF (b, c, d, a, x[ 7],  22, 0xfd469501); /* 8 */
		FF (a, b, c, d, x[ 8],   7, 0x698098d8); /* 9 */
		FF (d, a, b, c, x[ 9],  12, 0x8b44f7af); /* 10 */
		FF (c, d, a, b, x[10],  17, 0xffff5bb1); /* 11 */
		FF (b, c, d, a, x[11],  22, 0x895cd7be); /* 12 */
		FF (a, b, c, d, x[12],   7, 0x6b901122); /* 13 */
		FF (d, a, b, c, x[13],  12, 0xfd987193); /* 14 */
		FF (c, d, a, b, x[14],  17, 0xa679438e); /* 15 */
		FF (b, c, d, a, x[15],  22, 0x49b40821); /* 16 */
		#undef FF

		/* Round 2 */
		#define GG(a, b, c, d, x, s, ac) { \
			(a) += G ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		GG (a, b, c, d, x[ 1],   5, 0xf61e2562); /* 17 */
		GG (d, a, b, c, x[ 6],   9, 0xc040b340); /* 18 */
		GG (c, d, a, b, x[11],  14, 0x265e5a51); /* 19 */
		GG (b, c, d, a, x[ 0],  20, 0xe9b6c7aa); /* 20 */
		GG (a, b, c, d, x[ 5],   5, 0xd62f105d); /* 21 */
		GG (d, a, b, c, x[10],   9,  0x2441453); /* 22 */
		GG (c, d, a, b, x[15],  14, 0xd8a1e681); /* 23 */
		GG (b, c, d, a, x[ 4],  20, 0xe7d3fbc8); /* 24 */
		GG (a, b, c, d, x[ 9],   5, 0x21e1cde6); /* 25 */
		GG (d, a, b, c, x[14],   9, 0xc33707d6); /* 26 */
		GG (c, d, a, b, x[ 3],  14, 0xf4d50d87); /* 27 */
		GG (b, c, d, a, x[ 8],  20, 0x455a14ed); /* 28 */
		GG (a, b, c, d, x[13],   5, 0xa9e3e905); /* 29 */
		GG (d, a, b, c, x[ 2],   9, 0xfcefa3f8); /* 30 */
		GG (c, d, a, b, x[ 7],  14, 0x676f02d9); /* 31 */
		GG (b, c, d, a, x[12],  20, 0x8d2a4c8a); /* 32 */
		#undef GG 
	
		/* Round 3 */
		#define HH(a, b, c, d, x, s, ac) { \
			(a) += H ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		HH (a, b, c, d, x[ 5],   4, 0xfffa3942); /* 33 */
		HH (d, a, b, c, x[ 8],  11, 0x8771f681); /* 34 */
		HH (c, d, a, b, x[11],  16, 0x6d9d6122); /* 35 */
		HH (b, c, d, a, x[14],  23, 0xfde5380c); /* 36 */
		HH (a, b, c, d, x[ 1],   4, 0xa4beea44); /* 37 */
		HH (d, a, b, c, x[ 4],  11, 0x4bdecfa9); /* 38 */
		HH (c, d, a, b, x[ 7],  16, 0xf6bb4b60); /* 39 */
		HH (b, c, d, a, x[10],  23, 0xbebfbc70); /* 40 */
		HH (a, b, c, d, x[13],   4, 0x289b7ec6); /* 41 */
		HH (d, a, b, c, x[ 0],  11, 0xeaa127fa); /* 42 */
		HH (c, d, a, b, x[ 3],  16, 0xd4ef3085); /* 43 */
		HH (b, c, d, a, x[ 6],  23,  0x4881d05); /* 44 */
		HH (a, b, c, d, x[ 9],   4, 0xd9d4d039); /* 45 */
		HH (d, a, b, c, x[12],  11, 0xe6db99e5); /* 46 */
		HH (c, d, a, b, x[15],  16, 0x1fa27cf8); /* 47 */
		HH (b, c, d, a, x[ 2],  23, 0xc4ac5665); /* 48 */
		#undef HH 

		/* Round 4 */
		#define II(a, b, c, d, x, s, ac) { \
			(a) += I ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		II (a, b, c, d, x[ 0],   6, 0xf4292244); /* 49 */
		II (d, a, b, c, x[ 7],  10, 0x432aff97); /* 50 */
		II (c, d, a, b, x[14],  15, 0xab9423a7); /* 51 */
		II (b, c, d, a, x[ 5],  21, 0xfc93a039); /* 52 */
		II (a, b, c, d, x[12],   6, 0x655b59c3); /* 53 */
		II (d, a, b, c, x[ 3],  10, 0x8f0ccc92); /* 54 */
		II (c, d, a, b, x[10],  15, 0xffeff47d); /* 55 */
		II (b, c, d, a, x[ 1],  21, 0x85845dd1); /* 56 */
		II (a, b, c, d, x[ 8],   6, 0x6fa87e4f); /* 57 */
		II (d, a, b, c, x[15],  10, 0xfe2ce6e0); /* 58 */
		II (c, d, a, b, x[ 6],  15, 0xa3014314); /* 59 */
		II (b, c, d, a, x[13],  21, 0x4e0811a1); /* 60 */
		II (a, b, c, d, x[ 4],   6, 0xf7537e82); /* 61 */
		II (d, a, b, c, x[11],  10, 0xbd3af235); /* 62 */
		II (c, d, a, b, x[ 2],  15, 0x2ad7d2bb); /* 63 */
		II (b, c, d, a, x[ 9],  21, 0xeb86d391); /* 64 */
		#undef II 
		#undef ROTATE_LEFT 
		#undef I
		#undef H
		#undef G
		#undef F
		state[0] += a;
		state[1] += b;
		state[2] += c;
		state[3] += d;
		memset (x, 0, sizeof(x));
	}

	void MD5Hash::Update(const char *input, unsigned int inputlen)
	{
		unsigned int i, index, partlen;
		index = (unsigned int)((count[0] >> 3) & 0x3F);
		if ((count[0] += ((unsigned int)inputlen << 3)) < ((unsigned int)inputlen << 3))
			count[1]++;
		count[1] += ((unsigned int)inputlen >> 29);
		partlen = 64 - index;
		if (inputlen >= partlen) {
			memcpy(&buffer[index], input, partlen);
			transform (buffer);
			for (i = partlen; i + 63 < inputlen; i += 64)
				transform (&((const unsigned char*)input)[i]);
			index = 0;
		} else
			i = 0;
		memcpy(&buffer[index], &input[i], inputlen-i);
	}

	void MD5Hash::init()
	{
		count[0] = count[1] = 0;
		state[0] = 0x67452301;
		state[1] = 0xefcdab89;
		state[2] = 0x98badcfe;
		state[3] = 0x10325476;
	}
	
	MD5Hash::MD5Hash() 
	{ 
		init(); 
	}

	const unsigned char* MD5Hash::Final()
	{
		static unsigned char padding[64] = {
			0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		unsigned char bits[8];
		unsigned int index, padlen;
		memcpy(bits, count, 8);
		index = (unsigned int)((count[0] >> 3) & 0x3f);
		padlen = (index < 56) ? (56 - index) : (120 - index);
		Update ((const char*)padding, padlen);
		Update ((const char*)bits, 8);
		memcpy(digest, state, 16);
		return digest;
	}


	bool MD5Hash::Equal(const char* asciidigest)
	{
		char buf[33];
		for(int i=0;i<16;i++){
			sprintf(buf+i*2, "%02x", digest[i]);
		}
		if(strnicmp(asciidigest, buf, 32)==0)
			return true;
		return false;
	}
	
	char* MD5Hash::GetString(char* buffer, unsigned int& length)
	{
		if(!buffer || length < 33)
			return NULL;
		length = 32;
		for(int i=0;i<16;i++){
			sprintf(buffer+i*2, "%02x", digest[i]);
		}
		buffer[32] = 0;
		return buffer;
	}

	bool MD5Hash::Equal(MD5Hash& hash)
	{
		if(memcmp((const void*)digest, (const void*)hash.GetDigest(), 16)==0)
			return true;
		return false;
	}
}
