#include "windows.h"

/*
��ģ��ͬһ�߳��ڵĺ�̨���ڲ���ʱ�����ڽ����л�������¼���Ӧ������
���������������޸��̵߳���Ϣ���У�ֱ�Ӵ۸���Ϣ
*/

//CDlgExplorer *p_dlgExplorer = NULL;
BOOL PeekMessage_Detour(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax,UINT wRemoveMsg)
{
	BOOL res = PeekMessage(lpMsg,hWnd,wMsgFilterMin,wMsgFilterMax,wRemoveMsg);
	/*
	if((p_dlgExplorer)&&(res))
	{
		res = p_dlgExplorer->PeekMessage_Filter(lpMsg,hWnd);
		//�۸���Ϣ..
	}
	*/
	return res;
}
