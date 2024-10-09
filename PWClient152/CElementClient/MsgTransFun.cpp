#include "windows.h"

/*
在模拟同一线程内的后台窗口操作时，由于焦点切换，造成事件响应的问题
因此用这个函数来修改线程的消息队列，直接篡改消息
*/

//CDlgExplorer *p_dlgExplorer = NULL;
BOOL PeekMessage_Detour(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax,UINT wRemoveMsg)
{
	BOOL res = PeekMessage(lpMsg,hWnd,wMsgFilterMin,wMsgFilterMax,wRemoveMsg);
	/*
	if((p_dlgExplorer)&&(res))
	{
		res = p_dlgExplorer->PeekMessage_Filter(lpMsg,hWnd);
		//篡改消息..
	}
	*/
	return res;
}
