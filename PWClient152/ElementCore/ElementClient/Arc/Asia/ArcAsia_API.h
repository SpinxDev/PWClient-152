/**
 * @file      cc_sdk.h
   @brief     The header file for SDK APIs.
   @author    shenhui@pwrd.com
   @data      2013/03/20
   @version   02.00.00
 */

/* 
 *  Copyright(C) 2013. Perfect World Entertainment Inc
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
 * $Revision: 02.00.00
 * $Id:
 * $notes:.
 */

#pragma once

#include <windows.h>
#include <tchar.h>
#include "ArcAsia_Datatype.h"
#include "ArcAsia_Error.h"
#define ARC_ASIA_SDK_STATICLIB

namespace ARC_ASIA_SDK
{

#ifdef _cplusplus
extern "C"
{
#endif

#ifndef ARC_ASIA_SDK_STATICLIB
#ifdef CC_SDK_EXPORTS
#define CC_SDK_API extern "C" __declspec(dllexport)
#else
#define CC_SDK_API extern "C" __declspec(dllimport)
#endif
#else
#define CC_SDK_API
#endif

#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

/****************************************************************************/
/*                       Interface declaration                              */
/****************************************************************************/

/**
* @brief: launch application from game
* @param[in] pszGameAbbr: the abbreviation of game name
* @param[in] nLanguage: language info of game client.
*
* @return: If succeed, return S_OK, otherwise return error code.
* @notes: abbr is unique for each game and provided by PWE.
*/
CC_SDK_API HRESULT CC_LaunchClient(wchar_t* pszGameAbbr, LANGUAGE_VERSION nLanguage) ;

/**
* @brief: initialize SDK.
* @param[in] nPartnerId: useless now, reserve for future.
* @param[in] nAppId: game unique id, allocated by PWE.
*
* @return: If succeed, return a handle of SDK, otherwise return NULL 
* @notes: none.
*/
CC_SDK_API CC_HANDLE CC_Init(unsigned int nPartnerId, unsigned int nAppId) ;

/**
* @brief: exit SDK.
* @param[in] hSDK: handle returned by CC_Init
*
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_UnInit(const CC_HANDLE hSDK) ;

/**
* @brief: get a token when user are trying to log into game.
* @param[in] hSDK: handle returned by CC_Init.
* @param[in] pszAccount: this is account name used to log into game, this parameter is null-terminated.
* @param[out]pszToken: this is the token return back to caller.
*            Token is a 16 characters null-terminated string at present, so caller need allocate at least 17Bytes buffer.
* @param[in] nTokenLen: length of pszToken in byte, should be larger than 32 bytes.
* @param[in] dwTimeout:  timeout in milliseconds.
* @return: If succeed, return S_OK, otherwise return error code
* @notes: This API should be called in the real game client executable, not the launcher or patcher.
*/
//CC_SDK_API HRESULT CC_GetToken(const CC_HANDLE hSDK, wchar_t* pszAccount, wchar_t* pszToken, unsigned int nTokenLen, unsigned long dwTimeout = 60*1000) ;


/**
* @brief: 游戏试图自动登录时候从Arc获取登录信息或者再次获取token.
* @param[in] hSDK: handle returned by CC_Init
* @param[out] pszAccount: Arc返回的真实的用户帐号或用户名
* @param[in] nAccountLen: pszAccount所指向的缓冲区最大字符数, 最小为128
* @param[out] pszToken: Arc返回的一次一密的token
* @param[in] nTokenLen: pszToken所指向的缓冲区最大字符数 最小为64
* @param[out] pszAgent: Arc返回的Agent类型
* @param[in] nAgentLen: pszAgent所指向的缓冲区最大字符数 最小为16
* @param[in] dwTimeout:  timeout in milliseconds.
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/

CC_SDK_API HRESULT CC_GetTokenEx(__in const CC_HANDLE hSDK, __in wchar_t* pszAccount, __in unsigned int nAccountLen,\
							     __out wchar_t* pszToken, __in unsigned int nTokenLen, \
							     __out wchar_t* pszAgent, __in unsigned int nAgentLen,
							     __in unsigned long dwTimeout = 60*1000);


/**
* @brief: 获取Arc启动游戏时指定的服务器信息.
* @param[in] hSDK: handle returned by CC_Init
* @param[out] pszServer: 启动游戏时指定的游戏服务器，为空时Arc未指定特定的服务器
* @param[in] nServerLen: pszServer所指向的缓冲区最大字符数, 最小为64
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_GetServerInfo(__in const CC_HANDLE hSDK, __out wchar_t* pszServer, __in unsigned int nServerLen);

/**
* @brief: create a browser tab in game browser.
* @param[in] hSDK: handle returned by CC_Init
* @param[in] pszUrl: url to browser in game browser.
*
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_GotoUrlInOverlay(__in const CC_HANDLE hSDK, __in wchar_t* pszUrl) ;


/**
* @brief: Arc启动游，游戏在初始化Sdk成功之后，因某种原因使得游戏登录的账号与Arc传递的账号不一致（登录不成功，用户主动切换账号，掉线重登录等), 游戏通知Arc账号已切换。
* @param[in] hSDK: handle returned by CC_Init
* @param[in] pszNewAccount: 游戏新登录的账号
* @param[in] pszTicket: 与账号关联的ticket
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_UserChanged(__in const CC_HANDLE hSDK, __in wchar_t* pszNewAccount, __in wchar_t* pszTicket);

#ifdef _cplusplus
}
#endif

}//end of namespace