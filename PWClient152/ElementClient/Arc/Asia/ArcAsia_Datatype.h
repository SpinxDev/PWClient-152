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
namespace ARC_ASIA_SDK
{
	typedef void* CC_HANDLE ;

	typedef enum _LANGUAGE_VERSION{
		NONE_LANGUAGE = 0,
		ENGLISH = 1,
		GERMAN = 2,
		FRENCH = 3,
		CHINESEPRC = 100, //中文简体
		CHINETRAD = 101, //中文繁体
	}LANGUAGE_VERSION;
}