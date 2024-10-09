// Filename	: EC_WinAPI.h
// Creator	: Xu Wenbin
// Date		: 2014/11/10

#ifndef _ELEMENTCLIENT_EC_WINAPI_H_
#define _ELEMENTCLIENT_EC_WINAPI_H_

#include <ShlObj.h>

//	声明
typedef void (WINAPI *MyRtlCaptureContext)(PCONTEXT);
typedef LPITEMIDLIST (WINAPI *MyILCreateFromPathA)(const char *);
typedef void (WINAPI *MyILFree)(LPITEMIDLIST);

//	函数指针
extern MyRtlCaptureContext	g_pRtlCaptureContext;
extern MyILCreateFromPathA	g_pILCreateFromPathA;
extern MyILFree				g_pILFree;

//	初始化
void InitializeWindowsAPI();

#endif
