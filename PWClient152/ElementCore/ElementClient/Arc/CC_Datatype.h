/**
 * @file      cc_datatype.h
   @brief     The header file for data.
   @author    shenhui@pwrd.com
   @data      2011/06/15
   @version   01.00.00
 */

/* 
 *  Copyright(C) 2010. Perfect World Entertainment Inc
 *
 *  This PROPRIETARY SOURCE CODE is the valuable property of PWE and 
 *  its licensors and is protected by copyright and other intellectual 
 *  property laws and treaties.  We and our licensors own all rights, 
 *  title and interest in and to the SOURCE CODE, including all copyright 
 *  and other intellectual property rights in the SOURCE CODE.
 */

/*
 * $LastChangedBy: shenhui@pwrd.com
 * $LastChangedDate: 
 * $Revision: 01.00.01
 * $Id:
 * $notes:.
 */

#pragma once
namespace CC_SDK
{
	typedef void* CC_HANDLE ;

	typedef enum _LANGUAGE_VERSION{
		NONE_LANGUAGE = 0, //if all the versions of game clients share the same installer or game folder, please use this value
		ENGLISH = 1, //English
		GERMAN = 2,  //German
		FRENCH = 3,  //French
		PORTUGUESE = 4, //Portuguese
		RUSSIAN = 5, //Russian
		TURKISH = 6, //Turkish
	}LANGUAGE_VERSION;
}