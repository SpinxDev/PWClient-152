// WndManager.h: interface for the CWndManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDMANAGER_H__D85EA015_058F_4675_99A1_D2509FFBFA20__INCLUDED_)
#define AFX_WNDMANAGER_H__D85EA015_058F_4675_99A1_D2509FFBFA20__INCLUDED_
#include "AList2.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ROOT �����ڹ����� 
class CWndManager  
{
public:
	CWndManager * POINTER();
	BOOL CaptureWindowOffset(HDC hmemdc,int x0, int y0);
	BOOL CaptureWindowScr(HDC hmemdc,RECT* h_rc =NULL);
	BOOL CaptureWindow(HDC hmemdc,int x0=0,int y0=0);
	BOOL NewWindowStyle();
	BOOL RestoreWindowStyle();
	BOOL IsValid();
	HWND m_hwnd;
	DWORD m_tid;
	DWORD m_pid;
	PVOID m_htid; //ָ�룬ָ��ThreadManager
	POINT m_orgpt;
	
	
	CWndManager(HWND hwnd,DWORD tid, DWORD pid);
	CWndManager(HWND hwnd,DWORD tid, DWORD pid, DWORD zorder);
	CWndManager(const CWndManager &s);
	
	CWndManager();
	virtual ~CWndManager();
	DWORD m_zorder;  //��ROOT���ڵ�Z ORDER
	DWORD m_oldorder;//��¼ԭ���� ORDER

	//�Ƚϴ���Z ORDER ��������
	bool operator > (const CWndManager & s){
		if(this->m_tid != s.m_tid)
			throw "CWndManager���ش��󣬲�ͬTID�´��ڲ��ܱȽ�";
		return (this->m_zorder>s.m_zorder);
	}
	bool operator < (const CWndManager & s){
		if(this->m_tid != s.m_tid)
			throw "CWndManager���ش��󣬲�ͬTID�´��ڲ��ܱȽ�";
		return (this->m_zorder<s.m_zorder);
	}
	bool operator >= (const CWndManager & s){
		if(this->m_tid != s.m_tid)
			throw "CWndManager���ش��󣬲�ͬTID�´��ڲ��ܱȽ�";
		return (this->m_zorder>=s.m_zorder);
	}
	bool operator <= (const CWndManager & s){
		if(this->m_tid != s.m_tid)
			throw "CWndManager���ش��󣬲�ͬTID�´��ڲ��ܱȽ�";
		return (this->m_zorder<=s.m_zorder);
	}

	//�Ƚϴ��ھ�����ڸ����б�
	bool operator == (const CWndManager & s){
		if(this->m_tid != s.m_tid)
			throw "CWndManager���ش��󣬲�ͬTID�´��ڲ��ܱȽ�";
		return ((this->m_tid == s.m_tid)&&(this->m_hwnd == s.m_hwnd));
	}
	bool operator == (const HWND  s){
		return (this->m_hwnd == s);
	}

	/*
	void* operator new (size_t size){
		void* p;
		//p=new(size);
		return p;
	}
	*/
private:
	BOOL m_bInitexstyle;
	LONG_PTR m_lwndexstyle;
};


#endif // !defined(AFX_WNDMANAGER_H__D85EA015_058F_4675_99A1_D2509FFBFA20__INCLUDED_)


// wndThreadManager.h: interface for the CWndThreadManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADMANAGER_H__FBADEA6C_0E74_4725_9021_ECC70C230404__INCLUDED_)
#define AFX_THREADMANAGER_H__FBADEA6C_0E74_4725_9021_ECC70C230404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//using namespace std;
typedef void _declspec(dllimport)(* TP_InstallHook)(BOOL bActive, DWORD dwThreadId);
//typedef AList2<CWndManager> WNDLIST;

class WNDLIST:public AList2<CWndManager>
{
public:
	void sort()
	{
		int n= GetCount();
		for(int i=0;i<n-1;i++)
		{
			for(int j=0;j<n-i-1;j++)
			{
				if(GetByIndex(j)>GetByIndex(j+1))
				{
					swap(j,j+1);
					//bool flag = GetByIndex(j)<GetByIndex(j+1);
				}
			}
		}
	}
	bool swap(int index1, int index2)
	{
		if(index1==index2)
			return true;
		ALISTPOSITION tmp1 = FindByIndex(index1);
		if(tmp1==NULL)
			return false;
		ALISTPOSITION tmp2 = FindByIndex(index2);
		if(tmp2==NULL)
			return false;		
		
		if((s_NODE*)tmp1==m_pHead)
		{
			m_pHead = (s_NODE*)tmp2;
		}
		else
		{
			if((s_NODE*)tmp2==m_pHead)
			{
				m_pHead = (s_NODE*)tmp1;
			}
		}

		if((s_NODE*)tmp1==m_pTail)
		{
			m_pTail = (s_NODE*)tmp2;
		}
		else
		{
			if((s_NODE*)tmp2==m_pTail)
			{
				m_pTail = (s_NODE*)tmp1;
			}
		}

		s_NODE* pPrev1 = ((s_NODE*)tmp1)->pPrev;
		s_NODE* pNext1 = ((s_NODE*)tmp1)->pNext;
		s_NODE* pPrev2 = ((s_NODE*)tmp2)->pPrev;
		s_NODE* pNext2 = ((s_NODE*)tmp2)->pNext;

		((s_NODE*)tmp1)->pNext = (((s_NODE*)tmp2)->pNext!=(s_NODE*)tmp1)?((s_NODE*)tmp2)->pNext:(s_NODE*)tmp2;
		((s_NODE*)tmp1)->pPrev = (((s_NODE*)tmp2)->pPrev!=(s_NODE*)tmp1)?((s_NODE*)tmp2)->pPrev:(s_NODE*)tmp2;
		((s_NODE*)tmp2)->pNext =  pNext1!=(s_NODE*)tmp2?pNext1:(s_NODE*)tmp1;
		((s_NODE*)tmp2)->pPrev =  pPrev1!=(s_NODE*)tmp2?pPrev1:(s_NODE*)tmp1;
		
		if((pPrev1!=(s_NODE*)tmp2)&&(pPrev1))
		{
			((s_NODE*)pPrev1)->pNext = (s_NODE*)tmp2;
		}
		if((pNext1!=(s_NODE*)tmp2)&&(pNext1))
		{
			((s_NODE*)pNext1)->pPrev = (s_NODE*)tmp2;
		}
		if((pPrev2!=(s_NODE*)tmp1)&&(pPrev2))
		{
			((s_NODE*)pPrev2)->pNext = (s_NODE*)tmp1;
		}
		if((pNext2!=(s_NODE*)tmp1)&&(pNext2))
		{
			((s_NODE*)pNext2)->pPrev = (s_NODE*)tmp1;
		}
		/*

		if((((s_NODE*)tmp1)->pPrev!=(s_NODE*)tmp2)&&(((s_NODE*)tmp1)->pNext!=(s_NODE*)tmp2))
		{
			((s_NODE*)tmp1)->pNext =  ((s_NODE*)tmp2)->pNext;
			((s_NODE*)tmp1)->pPrev =  ((s_NODE*)tmp2)->pPrev;
			((s_NODE*)tmp2)->pNext =  pNext1;
			((s_NODE*)tmp2)->pPrev =  pPrev1;

			if(pPrev1)
			{
				((s_NODE*)pPrev1)->pNext = (s_NODE*)tmp2;
			}
			if(pNext1)
			{
				((s_NODE*)pNext1)->pPrev = (s_NODE*)tmp2;
			}
			if(pPrev2)
			{
				((s_NODE*)pPrev2)->pNext = (s_NODE*)tmp1;
			}
			if(pNext2)
			{
				((s_NODE*)pNext2)->pPrev = (s_NODE*)tmp1;
			}
		}
		else
		{
			if(((s_NODE*)tmp1)->pPrev==(s_NODE*)tmp2)
			{

			}
			else
			{

			}

		}
		*/
		return true;
	}
};

class PtrWrapper  //��װָ��
{
private:
    CWndManager* px;//ָ����X��ָ��
	
public:
	//���켰�������캯��
	PtrWrapper(CWndManager* x = 0) : px(x) {}
	PtrWrapper(const PtrWrapper& pw) : px(pw.px) {}
	//��������
	~PtrWrapper() {}
	PtrWrapper& operator= (const PtrWrapper& pw) { px = pw.px; }
	
	//���������()���ض���X��ָ��
	const CWndManager* operator()()  const { return px; }
	CWndManager* operator()() { return px; }
	/*
	//�����߼������ == , < , >
	bool operator== (const PtrWrapper& pw1, const PtrWrapper& pw2) {
		return (pw1.operator()() && pw2.operator()()) ? *pw1() == *pw2() : false;
	}
	
	bool operator< (const PtrWrapper& pw1, const PtrWrapper& pw2) {
		return (pw1() && pw2()) ? *pw1() < *pw2() : false;
	}
	
	bool operator> (const PtrWrapper& pw1, const PtrWrapper& pw2) {
		return (pw1() && pw2()) ? !(*pw1() < *pw2()) : false;
	}
	*/
	bool operator== (const PtrWrapper& pw1) {
		return (px && pw1() ? (*px)==(*pw1()): false);
	}
	bool operator< (const PtrWrapper& pw1) {
		return (px && pw1() ? *px < *pw1() : false);
	}
	bool operator> (const PtrWrapper& pw1) {
		return (px && pw1() ? *px > *pw1() : false);
	}
};

typedef struct SimuMouseEvent
{
	POINT pt;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	BOOL  flag;
}SimuMouseEvent;

class CWndThreadManager  
{
	enum SYNCHRONIZETYPE
	{
		THREADHANDLE,
		THREADWINDOW
	};

public:
	VOID SetTopMostWindow(HWND hwnd);
	DWORD GetWindowList(HWND *wndlist, DWORD numo_lists ,DWORD *num_returned, BOOL porder =TRUE);
	DWORD GetFreshTicCount();
	VOID MouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);
	VOID MouseEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam,WORD method =0);
	VOID MouseEventX(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam,WORD method);
	VOID KeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);
	VOID KeyEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam,WORD method =0);
	VOID IMECharInPut(HWND hwnd, WCHAR ch);
	BOOL IsValidGuiThread();
	HWND GetWndAtPos(WORD zorder);
	BOOL GetPicInOne(HDC hmemdc,RECT *h_rc = NULL);
	BOOL Reset();
	DWORD GetWindowNums();
	DWORD ReFreshWindows();
	ALISTPOSITION FindWindow(HWND hwnd);
	//BOOL AddWindow(HWND hwnd);
	BOOL InitFromWindow(HWND hwnd);
	CWndThreadManager();
	CWndThreadManager(DWORD tid);
	CWndThreadManager(HWND hwnd);
	virtual ~CWndThreadManager();
	SYNCHRONIZETYPE m_synctype;
	//ͨ�� GetGUIThreadInfo ��ȡĿ���̵߳������Ϣ
	BOOL FreshWindow(HWND hwnd, WNDENUMPROC lpfn);
	HWND GetCaretWindowPos(LPRECT lprect);
	//static public members
	static BOOL InitHookDll();
	static BOOL InstallGlobalHook();
	static BOOL UnInstallGlobalHook();
	DWORD   m_tid;    //�߳�ID
private:
	HWND m_topwindow;
	VOID CaptureWindows(BOOL bCapture);
	HANDLE m_hmutex;
	CHAR   m_mutexchar[17];
	DWORD  m_tmpZorder;

	WNDLIST m_wndsList;

	DWORD   m_pid;    
	HWND    m_hwnd;   //���²�ROOT����
	HANDLE  m_thread; //���߳���ͬ��
	HDC     m_hmemdc;
	DWORD   m_freshtic;
	GUITHREADINFO m_guithreadinfo;
	//static private members
	static TP_InstallHook m_installhook;
	static BOOL m_binit;
	static BOOL m_binstall;
	static HMODULE m_hmod;
	static char *  m_strHookModName;
};

typedef  BOOL (WINAPI* TYP_SetLayeredWindowAttributes)(HWND hwnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags);
extern TYP_SetLayeredWindowAttributes My_SetLayeredWindowAttributes;
extern VOID SetupAuiApiPoints();

#endif // !defined(AFX_THREADMANAGER_H__FBADEA6C_0E74_4725_9021_ECC70C230404__INCLUDED_)


/*�쳣������
��MAIN �� WINMAIN ������� try catch ģ��,Ϊ����MsgBox���κ��쳣λ�ö�����ʾ
	try
	{
	   //WINMAIN() FUNCTION BODY
	}
	catch(char *str)
	{
		HANDLE handle =CreateThread(NULL,0,MsgThreadProc,str,0,NULL);
		WaitForSingleObject(handle,INFINITE);
	}
�����Ϣ��ʾ�̺߳���
	DWORD WINAPI MsgThreadProc( LPVOID lpParameter)
	{
		char *str = (char *)lpParameter;
		MessageBox(NULL,str,"������Ϣ",MB_OK);
		return 0;
	}
*/