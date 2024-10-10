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
Description : sha256.h
			(head file) head file for sha256.c : Hash Algorithm SHA256

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

#ifndef _SHA256_H
#define _SHA256_H

/*************** Header files *********************************************/
#include <stdlib.h>
#include <memory.h>
#include <string.h>

/*************** Assertions ***********************************************/
////////	Define the Endianness	////////
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN

#if defined(USER_BIG_ENDIAN)
	#define BIG_ENDIAN
#elif defined(USER_LITTLE_ENDIAN)
	#define LITTLE_ENDIAN
#else
	#if 0
		#define BIG_ENDIAN		//	Big-Endian machine with pointer casting
	#elif defined(_MSC_VER)
		#define LITTLE_ENDIAN	//	Little-Endian machine with pointer casting
	#else
		#error
	#endif
#endif

/*************** Macros ***************************************************/
////////	rotate by using shift operations	////////
#if defined(_MSC_VER)
	#define ROTL_DWORD(x, n) _lrotl((x), (n))
	#define ROTR_DWORD(x, n) _lrotr((x), (n))
#else
	#define ROTL_DWORD(x, n) ( (DWORD)((x) << (n)) | (DWORD)((x) >> (32-(n))) )
	#define ROTR_DWORD(x, n) ( (DWORD)((x) >> (n)) | (DWORD)((x) << (32-(n))) )
#endif

////////	reverse the byte order of DWORD(DWORD:4-bytes integer) and WORD.
#define ENDIAN_REVERSE_DWORD(dwS)	( (ROTL_DWORD((dwS),  8) & 0x00ff00ff)	\
									 | (ROTL_DWORD((dwS), 24) & 0xff00ff00) )

////////	move DWORD type to BYTE type and BYTE type to DWORD type
#if defined(BIG_ENDIAN)		////	Big-Endian machine
	#define BIG_B2D(B, D)		D = *(DWORD *)(B)
	#define BIG_D2B(D, B)		*(DWORD *)(B) = (DWORD)(D)
	#define LITTLE_B2D(B, D)	D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
#elif defined(LITTLE_ENDIAN)	////	Little-Endian machine
	#define BIG_B2D(B, D)		D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define BIG_D2B(D, B)		*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
	#define LITTLE_B2D(B, D)	D = *(DWORD *)(B)
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = (DWORD)(D)
#else
	#error ERROR : Invalid DataChangeType
#endif

/*************** Definitions / Macros  ************************************/
////	SHA256에 관련된 상수들
#define SHA256_DIGEST_BLOCKLEN	64		//	in bytes
#define SHA256_DIGEST_VALUELEN	32		//	in bytes

/*************** New Data Types *******************************************/
////////	Determine data types depand on the processor and compiler.
#define BOOL	int					//	1-bit data type
#define BYTE	unsigned char		//	unsigned 1-byte data type
#define WORD	unsigned short int	//	unsigned 2-bytes data type
#define DWORD	unsigned int		//	unsigned 4-bytes data type
#define RET_VAL		DWORD			//	return values

namespace GNET
{
////	SHA256..
typedef struct{
	DWORD		ChainVar[SHA256_DIGEST_VALUELEN/4];	//	Chaining Variable 저장
	DWORD		Count[4];							//	
	BYTE		Buffer[SHA256_DIGEST_BLOCKLEN];		//	Buffer for unfilled block
} SHA256_ALG_INFO;

/*************** Constant (Error Code) ************************************/
////	Error Code - 정리하고, 적당히 출력해야 함.
//#define CTR_SUCCESS					0

/*************** Prototypes ***********************************************/
////	Init/Update/Final 형식을 복호화.
void	SHA256_Init(
		SHA256_ALG_INFO	*AlgInfo);
void	SHA256_Update(
		SHA256_ALG_INFO	*AlgInfo,
		BYTE			*Message,		//	데이타가 입력됨
		DWORD			MessageLen);
void	SHA256_Final(
		SHA256_ALG_INFO	*AlgInfo,
		BYTE			*Digest);		//	해쉬값이 출력됨
}

/*************** END OF FILE **********************************************/
#endif	//	_SHA256_H
