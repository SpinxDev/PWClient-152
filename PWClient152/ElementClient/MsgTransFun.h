#include "windows.h"

#define PEEKMSG_HOOK

#ifdef PEEKMSG_HOOK
extern BOOL PeekMessage_Detour(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax,UINT wRemoveMsg);
#else
#define PeekMessage_Detour PeekMessage
#endif