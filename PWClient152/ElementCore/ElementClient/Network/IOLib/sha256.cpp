/***************************************************************************
* Copyright (c) 2000-2004, Future Systems, Inc. / Seoul, Republic of Korea *
* All Rights Reserved.                                                     *
*                                                                          *
* This document contains proprietary and confidential information.  No     *
* parts of this document or the computer program it embodies may be in     *
* any way copied, duplicated, reproduced, translated into a different      *
* programming language, or distributed to any person, company, or          *
* corporation without the prior written consent of Future Systems, Inc.    *
*                              Hyo Sun Hwang                               *
*                372-2 YangJae B/D 6th Floor, Seoul, Korea                 *
*                           +82-2-578-0581 (552)                           *
***************************************************************************/

/*--------------------- [ Version/Command in detais] ---------------------*\
Description : sha256.c
			(C-source file) Hash Algorithm SHA256

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

/*************** Header files *********************************************/
#include "sha256.h"

/*************** Assertions ***********************************************/

/*************** Definitions / Macros  ************************************/

/*************** New Data Types *******************************************/

/*************** Global Variables *****************************************/
	#define KKKK(j)		KK[j]

	static const DWORD	KK[64] = {
		0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,
		0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
		0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,
		0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,
		0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
		0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,
		0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,
		0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
		0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};

/*************** Prototypes ***********************************************/

/*************** Macros ***************************************************/
#undef RR
#undef SS

#define SS(x,n)		ROTR_DWORD(x,n)
#define RR(x,n)		(x>>n)
#define CCHH(x,y,z)	( ( (x)&(y) ) ^ ( (~x)&(z) ) )
#define Maj(x,y,z)	( ( (x)&(y) ) ^ ( (x)&(z) ) ^ ( (y)&(z) ) )
#define Sigma0(x)	( SS(x,2) ^ SS(x,13) ^ SS(x,22) )
#define Sigma1(x)	( SS(x,6) ^ SS(x,11) ^ SS(x,25) )
#define RHO0(x)		( SS(x,7) ^ SS(x,18) ^ RR(x,3) )
#define RHO1(x)		( SS(x,17) ^ SS(x,19) ^ RR(x,10) )

#define FF(A,B,C,D,E,F,G,H,j) {								\
		T1 = H + Sigma1(E) + CCHH(E,F,G) + KKKK(j) + WW[j];	\
		D += T1;											\
		H = T1 + Sigma0(A) + Maj(A,B,C);					\
}

/*************** Function *************************************************
*	SHA256 core fuction
*/
static void SHA256_Transform(DWORD *X, DWORD *DigestValue)
{
	DWORD	a, b, c, d, e, f, g, h, WW[64];
	DWORD	j, T1;

#if defined(BIG_ENDIAN)
	#define GetData(x)	x
#else
	#define GetData(x)	ENDIAN_REVERSE_DWORD(x)
#endif

	for( j=0; j<16; j++)
		WW[j] = GetData(X[j]);
	for( j=16; j<64; j++)
		WW[j] = RHO1(WW[j-2]) + WW[j-7] + RHO0(WW[j-15]) + WW[j-16];

	//
	a = DigestValue[0];
	b = DigestValue[1];
	c = DigestValue[2];
	d = DigestValue[3];
	e = DigestValue[4];
	f = DigestValue[5];
	g = DigestValue[6];
	h = DigestValue[7];

	////
	for( j=0; j<64; j+=8) {
		FF(a,b,c,d,e,f,g,h,j+0);
		FF(h,a,b,c,d,e,f,g,j+1);
		FF(g,h,a,b,c,d,e,f,j+2);
		FF(f,g,h,a,b,c,d,e,j+3);
		FF(e,f,g,h,a,b,c,d,j+4);
		FF(d,e,f,g,h,a,b,c,j+5);
		FF(c,d,e,f,g,h,a,b,j+6);
		FF(b,c,d,e,f,g,h,a,j+7);
	}

	//	chaining variables update
	DigestValue[0] += a;
	DigestValue[1] += b;
	DigestValue[2] += c;
	DigestValue[3] += d;
	DigestValue[4] += e;
	DigestValue[5] += f;
	DigestValue[6] += g;
	DigestValue[7] += h;
}

/*************** Function *************************************************
*	
*/
void	GNET::SHA256_Init(
		GNET::SHA256_ALG_INFO	*AlgInfo)
{
	//
	AlgInfo->ChainVar[0] = 0x6a09e667;
	AlgInfo->ChainVar[1] = 0xbb67ae85;
	AlgInfo->ChainVar[2] = 0x3c6ef372;
	AlgInfo->ChainVar[3] = 0xa54ff53a;
	AlgInfo->ChainVar[4] = 0x510e527f;
	AlgInfo->ChainVar[5] = 0x9b05688c;
	AlgInfo->ChainVar[6] = 0x1f83d9ab;
	AlgInfo->ChainVar[7] = 0x5be0cd19;

	//
	AlgInfo->Count[0] = AlgInfo->Count[1] = 0;
}

/*************** Function *************************************************
*	
*/
void	GNET::SHA256_Update(
		GNET::SHA256_ALG_INFO	*AlgInfo,
		BYTE			*Message,		//	input Message
		DWORD			MessageLen)		//	in BYTEs
{
	DWORD		RemainedLen, PartLen;

	//	Compute the number of hashed bytes mod SHA256_DIGEST_BLOCKLEN
	RemainedLen = (AlgInfo->Count[0] >> 3) % SHA256_DIGEST_BLOCKLEN;
	//	compute the number of bytes that can be filled up
	PartLen = SHA256_DIGEST_BLOCKLEN - RemainedLen;

	//	Update Count (number of toatl data bits)
	if( (AlgInfo->Count[0] += (MessageLen << 3)) < AlgInfo->Count[0] )
		AlgInfo->Count[1]++;
	AlgInfo->Count[1] += (MessageLen >> 29);

	//	핵심 updtae 부분
	if( MessageLen>=PartLen ) {
		memcpy(AlgInfo->Buffer+RemainedLen, Message, (int)PartLen);
		SHA256_Transform((DWORD *)AlgInfo->Buffer, AlgInfo->ChainVar);

		Message += PartLen;
		MessageLen -= PartLen;
		RemainedLen = 0;

		while( MessageLen>=SHA256_DIGEST_BLOCKLEN ) {
			if( (((int)Message)%4)==0 ) {	//	Speed up technique
				SHA256_Transform((DWORD *)Message, AlgInfo->ChainVar);
			}
			else {
				memcpy((BYTE *)AlgInfo->Buffer, Message, (int)SHA256_DIGEST_BLOCKLEN);
				SHA256_Transform((DWORD *)AlgInfo->Buffer, AlgInfo->ChainVar);
			}
			Message += SHA256_DIGEST_BLOCKLEN;
			MessageLen -= SHA256_DIGEST_BLOCKLEN;
		}
	}

	//	Buffer remaining input
	memcpy((BYTE *)AlgInfo->Buffer+RemainedLen, Message, (int)MessageLen);
}

/*************** Function *************************************************
*	
*/
void	GNET::SHA256_Final(
		GNET::SHA256_ALG_INFO	*AlgInfo,
		BYTE			*Digest)	//	output Hash Value
{
	DWORD		i, dwIndex, CountL, CountH;

	//	마지박 블록 처리
	CountL = AlgInfo->Count[0];
	CountH = AlgInfo->Count[1];
	dwIndex = (CountL >> 3) % SHA256_DIGEST_BLOCKLEN;
	AlgInfo->Buffer[dwIndex++] = 0x80;

	if(dwIndex>SHA256_DIGEST_BLOCKLEN-8) {
		memset((BYTE *)AlgInfo->Buffer + dwIndex, 0, (int)(SHA256_DIGEST_BLOCKLEN-dwIndex));

		SHA256_Transform((DWORD *)AlgInfo->Buffer, AlgInfo->ChainVar);

		memset((BYTE *)AlgInfo->Buffer, 0, (int)SHA256_DIGEST_BLOCKLEN-8);
	}
	else
		memset((BYTE *)AlgInfo->Buffer+dwIndex, 0, (int)(SHA256_DIGEST_BLOCKLEN-dwIndex-8));

	//	출력
#if defined(LITTLE_ENDIAN)
	CountL = ENDIAN_REVERSE_DWORD(CountL);
	CountH = ENDIAN_REVERSE_DWORD(CountH);
#endif
	((DWORD *)AlgInfo->Buffer)[SHA256_DIGEST_BLOCKLEN/4-2] = CountH;
	((DWORD *)AlgInfo->Buffer)[SHA256_DIGEST_BLOCKLEN/4-1] = CountL;

	SHA256_Transform((DWORD *)AlgInfo->Buffer, AlgInfo->ChainVar);

	for( i=0; i<SHA256_DIGEST_VALUELEN; i+=4)
		BIG_D2B((AlgInfo->ChainVar)[i/4], &(Digest[i]));
}

/*************** END OF FILE **********************************************/
