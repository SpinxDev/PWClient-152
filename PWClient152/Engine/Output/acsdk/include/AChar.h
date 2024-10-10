/*
 * FILE: AChar.h
 *
 * DESCRIPTION: ACHAR routines
 *
 * CREATED BY: duyuxin, 2004/2/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ACHAR_H_
#define _ACHAR_H_

#include <wchar.h>
#include <stdlib.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#ifdef UNICODE

	#define a_strcpy	wcscpy
	#define a_strncpy	wcsncpy
	#define a_strlen	wcslen
	#define	a_strcmp	wcscmp
	#define a_stricmp	wcsicmp
	#define a_strcat	wcscat
	#define a_strrchr	wcsrchr
	#define a_strstr	wcsstr
	#define a_strcspn	wcscspn
	#define a_strupr	_wcsupr
	#define a_strlwr	_wcslwr

	#define a_scanf		wscanf
	#define a_sscanf	swscanf
	#define a_printf	wprintf
	#define a_sprintf	swprintf
	#define a_vsprintf	vswprintf
	#define a_atoi		_wtoi
	#define a_getc		getwc
	#define a_putc		putwc
	#define a_getchar	getwchar
	#define a_putchar	putwchar
	#define a_gets		_getws
	#define a_puts		_putws

	#define a_fputc		fputwc
	#define	a_fgetc		fgetwc
	#define a_fgets		fgetws
	#define a_fputs		fputws
	#define a_fscanf	fwscanf
	#define a_fprintf	fwprintf

#else

	#define a_strcpy	strcpy
	#define a_strncpy	strncpy
	#define a_strlen	strlen
	#define a_strcmp	strcmp
	#define a_stricmp	stricmp
	#define a_strcat	strcat
	#define a_strrchr	strrchr
	#define a_strstr	strstr
	#define a_strcspn	strcspn
	#define a_strupr	strupr
	#define a_strlwr	strlwr

	#define a_scanf		scanf
	#define a_sscanf	sscanf
	#define a_printf	printf
	#define a_sprintf	sprintf
	#define a_vsprintf	vsprintf
	#define a_atoi		atoi
	#define a_getc		getc
	#define a_putc		putc
	#define a_getchar	getchar
	#define a_putchar	putchar
	#define a_gets		gets
	#define a_puts		puts

	#define a_fputc		fputc
	#define	a_fgetc		fgetc
	#define a_fgets		fgets
	#define a_fputs		fputs
	#define a_fscanf	fscanf
	#define a_fprintf	fprintf

#endif	//	UNICODE

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_ACHAR_H_
