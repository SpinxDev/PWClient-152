/*
 * FILE: ABaseDef.h
 *
 * DESCRIPTION: Base definition
 *
 * CREATED BY: duyuxin, 2003/6/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ABASEDEF_H_
#define _ABASEDEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#ifndef ASSERT
#include <assert.h>
#define ASSERT	assert
#endif	//	ASSERT

//	Disable copy constructor and operator = 
#define DISABLE_COPY_AND_ASSIGNMENT(ClassName)	\
	private:\
	ClassName(const ClassName&);\
	ClassName& operator = (const ClassName&);

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	unsigned value
typedef unsigned long		DWORD;		//	32-bit;
typedef unsigned short 		WORD;		//	16-bit;
typedef unsigned char		BYTE;		//	8-bit;

typedef int					BOOL;
#define TRUE				1
#define FALSE				0

#ifdef UNICODE
	typedef wchar_t			ACHAR;
	#define _AL(str)		L##str
#else
	typedef char			ACHAR;
	#define _AL(str)		str
#endif

//	typedef int				INT
//	commone integer ---- int, but the size undetermined, so not defined, use carefully;

//	signed value
typedef long				LONG;		//	32-bit;
typedef short				SHORT;		//	16-bit;
typedef char				CHAR;		//	8-bit;
typedef float				FLOAT;		//	float
typedef double				DOUBLE;		//	double

//	New type define
typedef char				a_char;
typedef unsigned char		a_byte;
typedef __int16				a_short;
typedef unsigned __int16	a_word;
typedef __int32				a_int32;
typedef unsigned __int32	a_uint32;
typedef unsigned __int32	a_dword;
typedef __int64				a_int64;
typedef unsigned __int64	a_uint64;

typedef float				a_float;
typedef double				a_double;
typedef unsigned short 		a_half;		//	Half float

//	We use size_t with a_size typedef
typedef size_t				a_size;

//	The data type that has same size as pointer
#ifdef _WIN64
typedef a_uint64		a_uiptr;
typedef a_int64			a_iptr;
#else
typedef __w64 a_uint32  a_uiptr;
typedef __w64 a_int32	a_iptr;
#endif	//	_WIN64

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_ABASEDEF_H_
