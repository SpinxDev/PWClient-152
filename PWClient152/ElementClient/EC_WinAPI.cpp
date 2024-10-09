// Filename	: EC_WinAPI.cpp
// Creator	: Xu Wenbin
// Date		: 2014/11/10

#include "EC_WinAPI.h"

MyRtlCaptureContext	g_pRtlCaptureContext = NULL;
MyILCreateFromPathA	g_pILCreateFromPathA = NULL;
MyILFree				g_pILFree = NULL;

void InitializeWindowsAPI(){
	g_pRtlCaptureContext = (MyRtlCaptureContext)GetProcAddress(GetModuleHandleA("kernel32.dll"), "RtlCaptureContext");
	g_pILCreateFromPathA = (MyILCreateFromPathA)GetProcAddress(GetModuleHandleA("shell32.dll"), "ILCreateFromPathA");
	g_pILFree = (MyILFree)GetProcAddress(GetModuleHandleA("shell32.dll"), "ILFree");
}