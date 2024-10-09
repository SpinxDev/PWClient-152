/**
 * @file      cc_sdk.h
   @brief     The header file for SDK APIs.
   @author    shenhui@pwrd.com
   @data      2014/07/02
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
#include "CC_Datatype.h"
#include "CC_Error.h"

namespace CC_SDK
{

#ifdef _cplusplus
extern "C"
{
#endif

#ifdef CC_SDK_EXPORTS
#define CC_SDK_API extern "C" __declspec(dllexport)
#else
#define CC_SDK_API extern "C" __declspec(dllimport)
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

// This struct is used for Arc to send message to game client, game client should complete the
// process of the message in 5 seconds, otherwise the process will be forcely terminated.
// When Arc overlay is about to open, message dwMsgId will be sent to hWnd with lParam TRUE
// When Arc overlay is closed, message dwMsgId will be sent to hWnd with lParam FALSE
typedef struct _GAMEWINDOWMESSAGE
{
	HWND hWnd;
	DWORD dwMsgId;
}GAMEWINDOWMESSAGE, *PGAMEWINDOWMESSAGE;

/**
* @brief: initialize SDK.
* @param[in] nPartnerId: useless now, reserve for future.
* @param[in] nAppId: game unique id, allocated by PWE.
* @param[in] pAppWnd: a struct contains game window handle and game defined message id.
*
* @return: If succeed, return a handle of SDK, otherwise return NULL 
* @notes: pAppWnd enables game client to receive notification from Arc when specified event is triggered.
*         At present, game client will be notified when overlay is about to open and is closed.
*/
CC_SDK_API CC_HANDLE CC_Init(unsigned int nPartnerId, unsigned int nAppId, PGAMEWINDOWMESSAGE pAppWnd = NULL) ;

/**
* @brief: exit SDK.
* @param[in] hSDK: handle returned by CC_Init
*
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_UnInit(const CC_HANDLE hSDK) ;

/**
* @brief: Get account name from current logged in Arc Client.
* @param[in] hSDK: handle returned by CC_Init
* @param[out]pszAccount: the account name, can be a username or email address, 3~64 characters.
* @param[in]  nAccountLen: length of pszAccount, in characters. It should be larger than 64.
* @return: If succeed, return S_OK, otherwise return error code and the pszAccount is NULL.
* @notes: 1. The returned account name can be used as input parameter to CC_GetToken.
*         2. In case the API returns CC_ERR_CLIENT_INTERNAL, please retry again after a few seconds(5 seconds is recommended)..
*/
CC_SDK_API HRESULT CC_GetAccountName(const CC_HANDLE hSDK, wchar_t* pszAccount,unsigned int nAccountLen) ;

/**
* @brief: get a token when user are trying to log into game.
* @param[in] hSDK: handle returned by CC_Init.
* @param[in] pszAccount: this is account name used to log into game, this parameter is null-terminated.
* @param[out]pszToken: this is the token return back to caller.
*            Token is a 16 characters string, caller needs to allocate 17 characters buffer at least, the last character is null terminator.
* @param[in] nTokenLen: length of pszToken in byte, should be larger than 32 bytes.
* @param[in] dwTimeout:  timeout in milliseconds.
* @return: If succeed, return S_OK, otherwise return error code.
* @notes: This API should be called in the real game client executable, not the launcher or patcher.
*/
CC_SDK_API HRESULT CC_GetToken(const CC_HANDLE hSDK, wchar_t* pszAccount, wchar_t* pszToken, unsigned int nTokenLen, unsigned long dwTimeout = 60000) ;

/**
* @brief: create a browser tab in game browser.
* @param[in] hSDK: handle returned by CC_Init
* @param[in] pszUrl: url to browser in game browser.
*
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_GotoUrlInOverlay(const CC_HANDLE hSDK, wchar_t* pszUrl) ;

/**
* @brief: open or close ingame overlay.
* @param[in] hSDK: handle returned by CC_Init
* @param[in] bShow: TRUE to display overlay, FALSE to hide overlay.
*
* @return: If succeed, return S_OK, otherwise return error code
* @notes: 1. if return FALSE, error code can be found in CC_Error.h.
*         2. if game is not launched from Arc, overlay will not display.
*/
CC_SDK_API HRESULT CC_ShowOverlay(const CC_HANDLE hSDK, BOOL bShow);


enum ReconnectState
{
	RS_BYPASS_REQUEST,		//open dialog display reconnect and no button
	RS_BYPASS_SUCCESS,		//notify overlay that reconnect succeeded.
	RS_BYPASS_FAIL,			//notify overlay that reconnect failed.
};

enum ReconnectCommand
{
	RC_BYPASS,		//inform game to perform bypass
	RC_CANCEL,		//inform game to cancel bypass
	RC_EXIT,		//inform game to exit
};

/**
* @brief: bypass reconnect call back function.
* @param[in] rcCmd: call back command.
* @param[in] pszAccount: account that log in Arc, used when rcCmd=RC_BYPASS, null terminated.
* @param[in] pszAuth:  token or formated pipe handle, used when rcCmd=RC_BYPASS, null terminated.
*
* @return: void.
* @notes: 
*/
typedef void (_stdcall *ReconnectCallback)(ReconnectCommand rcCmd, const wchar_t* pszAccount, const wchar_t* pszAuth);

//reconnect api, if rsState=RS_REQUEST,callback func must be set
/**
* @brief: bypass reconnect API.
* @param[in] hSDK: handle returned by CC_Init.
* @param[in] rsState: see ReconnectState.
* @param[in] pCallback: call back function, set when rsState==RS_BYPASS_REQUEST.
*
* @return: If succeed, return S_OK, otherwise return error code
* @notes: 
*/
CC_SDK_API HRESULT CC_BypassReconnect(const CC_HANDLE hSDK, ReconnectState rsState, ReconnectCallback pCallback);


#ifdef _cplusplus
}
#endif

}//end of namespace