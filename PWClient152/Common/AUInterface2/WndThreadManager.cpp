// WndManager.cpp: implementation of the CWndManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AUI.h"

#include "AList2.h"
//#define _WNDTHREAD_DEBUG
#ifndef _WINDOWS_
#include "windows.h"
#endif

#include "WndThreadManager.h"

#define WS_EX_LAYERED           0x00080000
#define LWA_ALPHA               0x00000002

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#define new A_DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef void (WINAPI *PROCSWITCHTOTHISWINDOW)(HWND,BOOL);

PROCSWITCHTOTHISWINDOW My_SwitchToThisWindow= NULL;


typedef  BOOL (WINAPI* TYP_SetLayeredWindowAttributes)(HWND hwnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags);
BOOL WINAPI WIN9X_SetLayeredWindowAttributes(HWND hwnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags)
{
	return FALSE;
}
TYP_SetLayeredWindowAttributes My_SetLayeredWindowAttributes = WIN9X_SetLayeredWindowAttributes;

VOID SetupAuiApiPoints()
{
	if(My_SetLayeredWindowAttributes==WIN9X_SetLayeredWindowAttributes)
	{		
		HMODULE hUser32 = GetModuleHandleA("user32.dll");	
		if(!hUser32)
			hUser32 = LoadLibraryA("user32.dll");
		if(hUser32)
		{
			My_SetLayeredWindowAttributes = (TYP_SetLayeredWindowAttributes)GetProcAddress(hUser32,"SetLayeredWindowAttributes");
			if(My_SetLayeredWindowAttributes==NULL)
			{
				My_SetLayeredWindowAttributes = WIN9X_SetLayeredWindowAttributes;
			}
		}
	}
}

CWndManager::CWndManager()
{
	m_orgpt.x =0;
	m_orgpt.y =0;
	m_hwnd = NULL;
	m_pid = 0;
	m_tid = 0;
	m_zorder = 0;
	m_oldorder =0;
	m_bInitexstyle = 0;
	m_lwndexstyle = 0;

}
CWndManager::CWndManager(const CWndManager &s)
{
	m_orgpt.x =s.m_orgpt.x;
	m_orgpt.y =s.m_orgpt.y;
	m_hwnd = s.m_hwnd;
	m_pid = s.m_pid;
	m_tid = s.m_tid;
	m_zorder = s.m_zorder;
	m_oldorder = s.m_oldorder;
	m_bInitexstyle = s.m_bInitexstyle;
	m_lwndexstyle = s.m_lwndexstyle;
}
CWndManager::CWndManager(HWND hwnd,DWORD tid, DWORD pid)
{
	m_orgpt.x =0;
	m_orgpt.y =0;
	m_hwnd = hwnd;
	m_pid = pid;
	m_tid = tid;
	m_zorder = 0;
	m_oldorder = 0;
	m_bInitexstyle = 0;
	m_lwndexstyle = 0;
}
CWndManager::CWndManager(HWND hwnd,DWORD tid, DWORD pid, DWORD zorder)
{
	m_orgpt.x =0;
	m_orgpt.y =0;
	m_hwnd = hwnd;
	m_pid = pid;
	m_tid = tid;
	m_zorder = zorder;
	m_oldorder = 0;
	m_bInitexstyle = 0;
	m_lwndexstyle = 0;
}
CWndManager::~CWndManager()
{
	RestoreWindowStyle();
	m_hwnd = NULL;
	m_pid = 0;
	m_tid = 0;
	m_zorder = 0;
	m_oldorder = 0;
	m_bInitexstyle = 0;
}

BOOL CWndManager::IsValid()
{
	DWORD tid,pid;
	if(IsWindow(m_hwnd))
	{
		tid = GetWindowThreadProcessId(m_hwnd,&pid);
		return((tid == m_tid)&&(pid == m_pid));
	}
	else
	{
		return FALSE;
	}
}

BOOL CWndManager::NewWindowStyle()
{
	SetupAuiApiPoints();
	if(!m_bInitexstyle)
	{	
		if(IsWindow(m_hwnd))
		{
			m_lwndexstyle = GetWindowLongPtr(m_hwnd,GWL_EXSTYLE);
			m_bInitexstyle =TRUE;
			COLORREF  colork=0x00000000;
			::SetWindowLong(m_hwnd,GWL_EXSTYLE,WS_EX_LAYERED|m_lwndexstyle);
			My_SetLayeredWindowAttributes(m_hwnd,colork,255,LWA_ALPHA);
		}
	}
	else
	{
		if(IsWindow(m_hwnd))
		{
			DWORD lwndexstyle = GetWindowLongPtr(m_hwnd,GWL_EXSTYLE);

			if(lwndexstyle!=(WS_EX_LAYERED|m_lwndexstyle))
			{
				COLORREF  colork=0x00000000;
				::SetWindowLong(m_hwnd,GWL_EXSTYLE,WS_EX_LAYERED|m_lwndexstyle);
				My_SetLayeredWindowAttributes(m_hwnd,colork,255,LWA_ALPHA);			
			}
		}
	}
	if(GetWindow(m_hwnd,GW_HWNDPREV)==GW_HWNDFIRST)
	{
		SetWindowPos(m_hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE);
		return TRUE;
	}
	return TRUE;
}

BOOL CWndManager::RestoreWindowStyle()
{
	if(m_bInitexstyle)
	{	
		if(IsWindow(m_hwnd))
		{
			::SetWindowLong(m_hwnd,GWL_EXSTYLE,m_lwndexstyle);
		}
		m_bInitexstyle = FALSE;  
	}
	return TRUE;
}

BOOL CWndManager::CaptureWindow(HDC hmemdc,int x0, int y0)
{
	if(IsWindow(m_hwnd)&&IsWindowVisible(m_hwnd))
	{
		HDC hdc = GetWindowDC(m_hwnd);
		RECT rc;
		::GetWindowRect(m_hwnd,&rc);
		BitBlt(hmemdc,x0,y0,rc.right -rc.left,rc.bottom -rc.top,hdc,0,0,SRCCOPY);
		ReleaseDC(m_hwnd,hdc);
		return TRUE;
	}
	return FALSE;
}

BOOL CWndManager::CaptureWindowScr(HDC hmemdc,RECT *h_rc) //窗口相对于屏幕绘制到hmemdc
{
	//NewWindowStyle();
	if(IsWindow(m_hwnd))//&&IsWindowVisible(m_hwnd))
	{
		HDC hdc = GetWindowDC(m_hwnd);
		RECT rc;
		::GetWindowRect(m_hwnd,&rc);
		BitBlt(hmemdc,0,0,rc.right -rc.left,rc.bottom -rc.top,hdc,0,0,SRCCOPY);
		ReleaseDC(m_hwnd,hdc);
		if(h_rc)
		{
			memcpy(h_rc,&rc,sizeof(RECT));
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CWndManager::CaptureWindowOffset(HDC hmemdc, int x0, int y0)
{
	if(IsWindow(m_hwnd)&&IsWindowVisible(m_hwnd))
	{
		HDC hdc = GetWindowDC(m_hwnd);
		RECT rc;
		::GetWindowRect(m_hwnd,&rc);
		BitBlt(hmemdc,rc.left -x0, rc.top -y0,rc.right -rc.left,rc.bottom -rc.top,hdc,0,0,SRCCOPY);
		ReleaseDC(m_hwnd,hdc);
		return TRUE;
	}
	return FALSE;
}


CWndManager * CWndManager::POINTER()
{
	return this;
}


// WndThreadManager.cpp: implementation of the CWndThreadManager class.
//
//////////////////////////////////////////////////////////////////////

//#include "winspy.h"

#ifndef _WINDOWS_
#include "windows.h"
#endif
#include "windowsx.h"
#include "WndThreadManager.h"
#include "TCHAR.H"



typedef void _declspec(dllimport)(* TP_InstallHook)(BOOL bActive,DWORD dwThreadId);
#define INRECT_PT(pt,rc) ((rc.left<=pt.x)&&(rc.right>=pt.x)&&(rc.top<=pt.y)&&(rc.bottom>=pt.y))
#define INRECT_XY(x,y,rc) ((rc.left<=x)&&(rc.right>=x)&&(rc.top<=y)&&(rc.bottom>=y))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char* CWndThreadManager::m_strHookModName ="winhook.dll";
BOOL CWndThreadManager::m_binit = FALSE;
BOOL CWndThreadManager::m_binstall = FALSE;
HMODULE CWndThreadManager::m_hmod = NULL;
TP_InstallHook CWndThreadManager::m_installhook = NULL;





static BOOL CALLBACK EnumFreshThreadWnds(HWND hwnd,LPARAM lParam)
{
	HWND hwndAncestor;
	hwndAncestor = GetAncestor(hwnd,GA_ROOT); //获取根窗口
	CWndThreadManager * WndThreadManager=(CWndThreadManager *)lParam;
	if(IsWindowVisible(hwndAncestor))
	{
		WndThreadManager->FreshWindow(hwndAncestor,EnumFreshThreadWnds);
		return TRUE;
	}
	else
		return TRUE;
}


CWndThreadManager::CWndThreadManager()
{
	m_topwindow = NULL;
	m_hmutex = NULL;
	m_pid =0;
	m_tid =0;
	m_synctype = THREADHANDLE;
	memset(&m_guithreadinfo,0,sizeof(m_guithreadinfo));
	m_guithreadinfo.cbSize = sizeof(m_guithreadinfo);
	/*
	m_rc.top =0;
	m_rc.bottom =0;
	m_rc.left =0;
	m_rc.right =0;
	*/
	m_freshtic =0;
	memset(m_mutexchar,0,sizeof(m_mutexchar));
}

CWndThreadManager::CWndThreadManager(DWORD tid)
{
#ifdef _DEBUG
	throw "CWndThreadManager(DWORD tid)方法 目前不支持"; 
#endif
}

CWndThreadManager::CWndThreadManager(HWND hwnd)
{
	CWndThreadManager();
	InitFromWindow(hwnd);
}

CWndThreadManager::~CWndThreadManager()
{
	m_wndsList.RemoveAll();
	if(m_hmutex!=NULL)
	{
		CloseHandle(m_hmutex);
		m_hmutex = NULL;
	}
	int n = sizeof(CWndThreadManager);
	int n1 = sizeof(WNDLIST);
}

BOOL CWndThreadManager::InitFromWindow(HWND hwnd)
{
	DWORD tid,pid;
	tid =GetWindowThreadProcessId(hwnd,&pid);
	if((m_tid!=tid)&&(tid !=0))
	{
		this->Reset();
		sprintf(m_mutexchar,"%016X",m_tid);
		m_hmutex =CreateMutexA(NULL,FALSE,m_mutexchar);
		if((m_hmutex)&&(GetLastError()!=ERROR_ALREADY_EXISTS))
		{
			//if(IsWindow(hwnd))
			{
				//m_tid = GetWindowThreadProcessId(hwnd,&m_pid);
				m_tid = tid;
				m_pid = pid;
			}
			ReFreshWindows();
			return (m_tid!=0);		
		}
		else
		{
			throw "InitFromWindow, 目标窗口线程已经被控制";
			return FALSE;
		}
	}
	else
		return TRUE;
}

BOOL CWndThreadManager::FreshWindow(HWND hwnd,WNDENUMPROC lpfn)
{
	if(lpfn != EnumFreshThreadWnds)
	{
		//由于要被CALLBACK 函数调用，所以不能设置为私有成员函数
		throw "CWndThreadManager::FreshWindow 禁止直接调用";
	}
	HWND hroot = GetAncestor(hwnd,GA_ROOT);
	ALISTPOSITION obj = FindWindow(hwnd);
	if(obj!=NULL)
	{
		if(m_wndsList.GetAt(obj).m_zorder ==0)

		{
			m_wndsList.GetAt(obj).m_zorder = m_tmpZorder;
			m_tmpZorder++;
			/*
			if(m_wndsList.GetAt(obj).m_oldorder != m_wndsList.GetAt(obj).m_zorder)
			{
				m_wndsList.GetAt(obj).m_oldorder =  m_wndsList.GetAt(obj).m_zorder;
				m_freshtic = GetTickCount();
			}
			*/
			//需要通知增加窗口
		}
		else
		{
			//已经有ROOT 窗口
		}		
	}
	else
	{
		CWndManager p(hwnd,m_tid,m_pid,m_tmpZorder);
		m_wndsList.AddTail(p);
		m_tmpZorder++;
		/*
		char classname[33] = {0,};
		RealGetWindowClassA(hwnd,classname,sizeof(classname));
		if(strcmp(classname,"#32768")==0) //Menu
		{
			//WNDCLASSEXA wcs;
			//OutputDebugString("It's Menu added \n");
			//GetClassInfoExA(NULL,classname,&wcs);
			{
				//DWORD fun1 = 0x77d56819;
				//DWORD fun2 = 0x77D5683A;
				//if(((DWORD)wcs.lpfnWndProc == 0x77d56819)||((DWORD)wcs.lpfnWndProc == 0x77D5683A))
				{
					try{
						
						if(IsWindow(hwnd))
						{
							//if(!SwitchToThisWindow)
							//{		
							//	HMODULE hUser32 = GetModuleHandleA("user32.dll");	
							//	SwitchToThisWindow = (PROCSWITCHTOTHISWINDOW)GetProcAddress(hUser32,"SwitchToThisWindow");
							//}
							
							//if (!SwitchToThisWindow)
							//	return TRUE;
							//if(this->m_topwindow)
							{
								SetWindowPos(hwnd,HWND_BOTTOM,0,0,3000,0,SWP_NOSIZE|SWP_NOACTIVATE);
								//SwitchToThisWindow(this->m_topwindow,TRUE);	
							}
						}
					}
					catch(...)
					{
					}
				}
			}
		}
		*/
	}

	return TRUE;
}

ALISTPOSITION CWndThreadManager::FindWindow(HWND hwnd)
{
#ifdef _WNDTHREAD_DEBUG
	OutputDebugStringA("Into FindWindow\n");
#endif
	ALISTPOSITION obj;
	for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
	{   
		if(m_wndsList.GetAt(obj).m_hwnd == hwnd)
		{
#ifdef _WNDTHREAD_DEBUG
			OutputDebugStringA("Out FindWindow\n");
#endif
			return obj;
		}
	}
#ifdef _WNDTHREAD_DEBUG
	OutputDebugStringA("Out FindWindow\n");
#endif
	return NULL;
}

BOOL CWndThreadManager::Reset()
{
	m_wndsList.RemoveAll();
	if(m_hmutex)
	{
		CloseHandle(m_hmutex);
		m_hmutex = NULL;
	}
	m_tid = 0;
	CWndThreadManager();
	return TRUE;
}

DWORD CWndThreadManager::ReFreshWindows()
{
#ifdef _WNDTHREAD_DEBUG
	OutputDebugStringA("Into ReFreshWindows\n");
#endif
	if(!m_hmutex)
	{
#ifdef _WNDTHREAD_DEBUG
		OutputDebugStringA("Out ReFreshWindows !m_hmute\n");
#endif
		return 0;
	}
	DWORD res=0;
	ALISTPOSITION obj;
	WaitForSingleObject(m_hmutex,INFINITE);
	GetGUIThreadInfo(m_tid,&m_guithreadinfo);
	for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
	{   
		m_wndsList.GetAt(obj).m_zorder = 0;
	}
	m_tmpZorder =1;
	if(EnumThreadWindows(m_tid,EnumFreshThreadWnds,(LPARAM)this) !=0)
	{
		BOOL flag = FALSE;
		if(m_wndsList.GetCount()>0)
		{
			for(obj = m_wndsList.GetHeadPosition();obj!=NULL;)   
			{   
				if(m_wndsList.GetAt(obj).m_zorder == 0)
				{	
					DWORD x=0;
					x=m_wndsList.GetCount();
					//obj =
					ALISTPOSITION tmp = obj;
					m_wndsList.GetNext(tmp);
					m_wndsList.RemoveAt(obj);
					obj=tmp;
					//需要通知删除窗口
					x=m_wndsList.GetCount();
					//m_wndsList.GetAt(obj).NewWindowStyle();
				}
				else
				{
					if(m_wndsList.GetAt(obj).m_oldorder != m_wndsList.GetAt(obj).m_zorder)
					{
						//顺序改变或新加窗口
						m_wndsList.GetAt(obj).m_oldorder =  m_wndsList.GetAt(obj).m_zorder;
						flag = TRUE;						
					}
					m_wndsList.GetAt(obj).NewWindowStyle();
					m_wndsList.GetAt(obj).m_htid = this;
					m_wndsList.GetNext(obj);
				}
			}
		}
		else
		{
			flag = TRUE;
		}
		if(flag)
			m_freshtic = GetTickCount();
		m_wndsList.sort();
		res = 0;
		/*
		m_rc.top =0;
		m_rc.bottom =0;
		m_rc.left =0;
		m_rc.right =0;
		//RECT rc;
		WORD num =0;
		for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
		{   
			if(num == 0)
			{	
				GetWindowRect(m_wndsList.GetAt(obj).m_hwnd,&m_rc);
				if(!(((m_rc.right-m_rc.left)>0)&&((m_rc.bottom-m_rc.top)>0)))
				{
				}
				else
				{
					num ++;
				}
				//需要通知删除窗口
			}
			else
			{
				GetWindowRect(m_wndsList.GetAt(obj).m_hwnd,&rc);				
				if(((rc.right-rc.left)>0)&&((rc.bottom-rc.top)>0))
				{
					if(m_rc.left > rc.left)
						m_rc.left = rc.left;
					if(m_rc.top > rc.top)
						m_rc.top = rc.top;
					if(m_rc.right < rc.right)
						m_rc.right = rc.right;
					if(m_rc.bottom < rc.bottom)
						m_rc.bottom = rc.bottom;
				}
				
			}
		}
		*/
	}
	else
	{
		//发生错误，应该通知删除
		res = GetLastError();
	}
	ReleaseMutex(m_hmutex);
#ifdef _WNDTHREAD_DEBUG
	OutputDebugStringA("Out ReFreshWindows\n");
#endif
	return res;
}

DWORD CWndThreadManager::GetWindowNums()
{
	return m_wndsList.GetCount();
}

VOID CWndThreadManager::CaptureWindows(BOOL bCapture)
{
	ALISTPOSITION obj;
	if(!m_hmutex)
		return ;
	WaitForSingleObject(m_hmutex,INFINITE);
	if(bCapture)
	{
		for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
		{   
			m_wndsList.GetAt(obj).NewWindowStyle();
		}
	}
	else
	{
		for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
		{   
			m_wndsList.GetAt(obj).RestoreWindowStyle();
		}
	}
	ReleaseMutex(m_hmutex);
}

BOOL CWndThreadManager::GetPicInOne(HDC hmemdc,RECT *h_rc)
{
	if(!m_hmutex)
		return 0;
	//m_wndsList.sort();
	ALISTPOSITION obj;
	WORD order;
	WaitForSingleObject(m_hmutex,INFINITE);
	WORD num =0;
	for(obj = m_wndsList.GetTailPosition(); obj!=NULL; m_wndsList.GetPrev(obj))   
	{   
		order =m_wndsList.GetAt(obj).m_zorder;
		if(order!=0)
		{	
			RECT rc;
			m_wndsList.GetAt(obj).CaptureWindowScr(hmemdc,&rc);

			if(num ==0)
			{
				if(!(((rc.right-rc.left)>0)&&((rc.bottom-rc.top)>0)))
				{
				}
				else
				{
					memcpy(h_rc,&rc,sizeof(RECT));
					num ++;
				}
			}
			else
			{
				if(((rc.right-rc.left)>0)&&((rc.bottom-rc.top)>0))
				{
					if(h_rc->left > rc.left)
						h_rc->left = rc.left;
					if(h_rc->top > rc.top)
						h_rc->top = rc.top;
					if(h_rc->right < rc.right)
						h_rc->right = rc.right;
					if(h_rc->bottom < rc.bottom)
						h_rc->bottom = rc.bottom;
				}	
			}
		}
	}
 	ReleaseMutex(m_hmutex);
	return TRUE;
}


HWND CWndThreadManager::GetWndAtPos(WORD zorder)
{
	ALISTPOSITION obj;
	for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
	{   
		if(m_wndsList.GetAt(obj).m_zorder == zorder)
		{
			return m_wndsList.GetAt(obj).m_hwnd;
		}
	}
	return NULL;
}

BOOL CWndThreadManager::IsValidGuiThread()
{
	return (GetWindowNums()>0);
}

#define PXMAX  2048
#define PXMIN -2048
#define PYMAX  2048
#define PYMIN -2048 //以后修改成2倍屏幕尺寸


BOOL CALLBACK ThreadEnumChildMouseProc(HWND hwnd,LPARAM lParam)
{
	if(IsWindowVisible(hwnd))
	{
		LRESULT res;
		RECT rc,rc1;
		POINT pt,ptc;
		pt.x =0;
		pt.y =0;
		SimuMouseEvent* p_SLparam =(SimuMouseEvent *)lParam;
		GetWindowRect(hwnd,&rc);
		GetClientRect(hwnd,&rc1);
		ClientToScreen(hwnd,&pt);
		char str[32]={0,};
		//::GetWindowText(hwnd,str,32);
		//if((rc.left<=p_SLparam->pt.x)&&(rc.right>=p_SLparam->pt.x)&&(rc.top<=p_SLparam->pt.y)&&(rc.bottom>=p_SLparam->pt.y))
		if(INRECT_PT(p_SLparam->pt,rc))
		{		
			EnumChildWindows(hwnd,ThreadEnumChildMouseProc,lParam);
			if(!p_SLparam->flag)
			{
				//::ClientToScreen(hWnd,&w_pt);
				//::GetClientRect(hWnd,&w_crc);
				ptc.x = p_SLparam->pt.x - pt.x;
				ptc.y = p_SLparam->pt.y - pt.y;
				if(p_SLparam->message!=WM_MOUSEMOVE)
					::PostMessage(hwnd,WM_ACTIVATE,WA_CLICKACTIVE,0);
				if(INRECT_PT(ptc,rc1)) //在ClientRect 内
				{
					if(p_SLparam->message !=WM_MOUSEMOVE)
						SendMessage(hwnd,WM_MOUSEMOVE,0,MAKELPARAM(ptc.x,ptc.y));
					res= PostMessage(hwnd,p_SLparam->message,p_SLparam->wParam,MAKELPARAM(ptc.x,ptc.y));
				}
				else //不在ClientRect 内
				{
					res = SendMessage(hwnd,WM_NCHITTEST,0,MAKELPARAM(p_SLparam->pt.x,p_SLparam->pt.y));				
					if((p_SLparam->message == WM_LBUTTONDOWN)&&(res == HTMAXBUTTON))
					{
						SendMessage(hwnd,WM_SYSCOMMAND,SC_MAXIMIZE,MAKELPARAM(p_SLparam->pt.x,p_SLparam->pt.y));
						p_SLparam->flag = TRUE;
						return TRUE;
					}
					if((p_SLparam->message == WM_LBUTTONDOWN)&&(res == HTMINBUTTON))
					{
						SendMessage(hwnd,WM_SYSCOMMAND,SC_MINIMIZE,MAKELPARAM(p_SLparam->pt.x,p_SLparam->pt.y));
						p_SLparam->flag = TRUE;
						return TRUE;
					}
					if((p_SLparam->message == WM_LBUTTONDOWN)&&(res == HTCLOSE))
					{
						SendMessage(hwnd,WM_SYSCOMMAND,SC_CLOSE,MAKELPARAM(p_SLparam->pt.x,p_SLparam->pt.y));
						p_SLparam->flag = TRUE;
						return TRUE;
					}
					/*
					switch(res)
					{
					case HTBORDER:
					case HTBOTTOM :
					case HTBOTTOMLEFT:
					case HTBOTTOMRIGHT:
					case HTCAPTION:
					case HTCLIENT :
					case HTCLOSE:
					case HTERROR:
					case HTGROWBOX:
					case HTHELP:
					case HTHSCROLL:
					case HTLEFT:
					case HTMENU:
					case HTMAXBUTTON:
					case HTMINBUTTON:
					case HTNOWHERE:
					case HTRIGHT:
					case HTSYSMENU:
					case HTTOP:
					case HTTOPLEFT:
					case HTTOPRIGHT:
					case HTTRANSPARENT:
					case HTVSCROLL:
						break;
					default:
						break;
					}
					*/
					PostMessage(hwnd,p_SLparam->message+WM_NCMOUSEMOVE-WM_MOUSEMOVE,res,MAKELPARAM(p_SLparam->pt.x,p_SLparam->pt.y));
				}
				/*		
				::PostMessage(hwnd,WM_ACTIVATE,WA_CLICKACTIVE,0);
				{
					::PostMessage(hwnd,p_SLparam->message,p_SLparam->wParam,MAKELPARAM(p_SLparam->pt.x-rc.left,p_SLparam->pt.y-rc.top));
				}
				*/
			}
			p_SLparam->flag = TRUE;
		}
	}
	return TRUE;
}


VOID CWndThreadManager::MouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam) //传入屏幕坐标，WndThreadManager 自动对应到窗口
{

	LONG xPos,yPos;
	RECT rc;
	xPos = (LONG)GET_X_LPARAM(lParam);
	yPos = (LONG)GET_Y_LPARAM(lParam);
	ALISTPOSITION obj;
	for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
	{   
		if(m_wndsList.GetAt(obj).m_zorder != 0)
		{
			
			if(IsWindowVisible(m_wndsList.GetAt(obj).m_hwnd))
			{
				GetWindowRect(m_wndsList.GetAt(obj).m_hwnd,&rc);
				if(INRECT_XY(xPos,yPos,rc))
				{
					//xPos -= rc.left;
					//yPos -= rc.top;
					//HWND hwnd =GetForegroundWindow();
					//SetForegroundWindow(m_wndsList.GetAt(obj).m_hwnd);
					MouseEvent(m_wndsList.GetAt(obj).m_hwnd, uMsg, wParam,MAKELPARAM(xPos,yPos),1); //传入的是屏幕坐标
					//SetForegroundWindow(hwnd);
					return;
				}
			}
		}
	}
	
}


VOID CWndThreadManager::MouseEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam,WORD method) 
{
	MouseEventX(hwnd,uMsg,wParam,lParam,method);

	if((uMsg != WM_MOUSEMOVE)&&(uMsg!=WM_NCMOUSEMOVE))
	{				
		if(!My_SwitchToThisWindow)
		{		
			HMODULE hUser32 = GetModuleHandleA("user32.dll");	
			My_SwitchToThisWindow = (PROCSWITCHTOTHISWINDOW)GetProcAddress(hUser32,"SwitchToThisWindow");
		}
		if (!My_SwitchToThisWindow)
			return;
		if(this->m_topwindow)
			My_SwitchToThisWindow(this->m_topwindow,TRUE);
	}

}





//参数为0时传入窗口相对坐标，WndThreadManager要修改参数
VOID CWndThreadManager::MouseEventX(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam,WORD method) 
{
	LONG xPos,yPos,sxpos,sypos,nxpos,nypos;
	LRESULT res;
	nxpos= sxpos =xPos = (LONG)GET_X_LPARAM(lParam); //
	nypos= sypos =yPos = (LONG)GET_Y_LPARAM(lParam); //	
	switch(method)
	{
	case 0:	//方法0，直接发消息
	case 1:
		if(uMsg!=WM_MOUSEMOVE)
		{
			int xxx;
			xxx =0;
			
		}
		SimuMouseEvent SLparam;
		RECT w_crc;
		POINT w_pt,w_ptc;
		w_pt.x = 0;
		w_pt.y = 0;
		w_ptc.x = 0;
		w_ptc.y = 0;

		SLparam.message = uMsg;
		SLparam.wParam = wParam;
		SLparam.flag = FALSE;
	
		if(method ==0)
		{		
			RECT rc_rect;
			GetWindowRect(hwnd,&rc_rect);
			sxpos += rc_rect.left;
			sypos += rc_rect.top;
		}
		
		SLparam.pt.x = sxpos;
		SLparam.pt.y = sypos;
		if(!IsIconic(hwnd))
			EnumChildWindows(hwnd,ThreadEnumChildMouseProc,(LPARAM)&SLparam);
		res =0;
		if(!SLparam.flag)
		{	
			::ClientToScreen(hwnd,&w_pt);
			::GetClientRect (hwnd,&w_crc);
			w_ptc.x = sxpos - w_pt.x;
			w_ptc.y = sypos - w_pt.y;
			if(INRECT_PT(w_ptc,w_crc)) //在ClientRect 内
			{
				::PostMessage(hwnd, uMsg,wParam,MAKELPARAM(w_ptc.x,w_ptc.y));
			}
			else //不在ClientRect 内
			{
				SetActiveWindow(hwnd);
				res = SendMessage(hwnd,WM_NCHITTEST,0,MAKELPARAM(sxpos,sypos));

				if((uMsg == WM_LBUTTONDOWN)&&(res == HTMAXBUTTON))
				{
					ALISTPOSITION obj;
					for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
					{   
						if(m_wndsList.GetAt(obj).m_hwnd == hwnd)
						{
							m_wndsList.GetAt(obj).RestoreWindowStyle();
							break;
						}
					}
					if(!IsZoomed(hwnd))
						SendMessage(hwnd,WM_SYSCOMMAND,SC_MAXIMIZE,MAKELPARAM(sxpos,sypos));
					else
						SendMessage(hwnd,WM_SYSCOMMAND,SC_RESTORE,MAKELPARAM(sxpos,sypos));
					if(obj!=NULL)
						m_wndsList.GetAt(obj).NewWindowStyle();
					return;
				}
				if((uMsg == WM_LBUTTONDOWN)&&(res == HTMINBUTTON))
				{
					if(!IsIconic(hwnd))
						SendMessage(hwnd,WM_SYSCOMMAND,SC_MINIMIZE,MAKELPARAM(sxpos,sypos));

					else
					{
						ALISTPOSITION obj;
						for(obj = m_wndsList.GetHeadPosition();obj!=NULL;m_wndsList.GetNext(obj))   
						{   
							if(m_wndsList.GetAt(obj).m_hwnd == hwnd)
							{	
								m_wndsList.GetAt(obj).RestoreWindowStyle();
								break;
							}
						}
						SendMessage(hwnd,WM_SYSCOMMAND,SC_RESTORE,MAKELPARAM(sxpos,sypos));
						if(obj!=NULL)
							m_wndsList.GetAt(obj).NewWindowStyle();
					}
					return ;
				}
				
				if((uMsg == WM_LBUTTONDOWN)&&(res == HTCLOSE))
				{
						
					PostMessage(hwnd,WM_SYSCOMMAND,SC_CLOSE,MAKELPARAM(sxpos,sypos));
					return ;
				}
				if((res==HTMENU)&&(uMsg == WM_LBUTTONDOWN))
				{
					MENUBARINFO info;
					GetMenuBarInfo(hwnd,OBJID_MENU,0,&info);
					if(!info.fBarFocused)
					{	
						PostMessage(hwnd,WM_SYSKEYDOWN,VK_MENU,0X20380001);
						PostMessage(hwnd,WM_SYSKEYUP  ,VK_MENU,0XC0380001);	
						//SendMessage(hwnd,WM_MOUSEACTIVATE,MA_ACTIVATE,HTMENU);
						//SendMessage(hwnd,WM_ACTIVATEAPP,WA_CLICKACTIVE,NULL);
						//SendMessage(hwnd,WM_NCACTIVATE,TRUE,NULL);
						//SendMessage(hwnd,WM_ACTIVATE,WA_ACTIVE,NULL);
						//SendMessage(hwnd,WM_ACTIVATE,WA_CLICKACTIVE,NULL);
					}
				}
				PostMessage(hwnd,uMsg+WM_NCMOUSEMOVE-WM_MOUSEMOVE,res,MAKELPARAM(sxpos,sypos));							
				if((uMsg == WM_MOUSEMOVE))
				{
					PostMessage(hwnd,WM_SETCURSOR,(WPARAM)hwnd,res);
					return ;
				}
			}
		}
		return ;
		break;
	case 2:	//方法1，通过HOOK,发了消息后在目标进程中处理		

		if((nxpos <PXMAX)&&(nxpos>=PXMIN)&&(nypos <PYMAX)&&(nypos>=PYMIN))
		{
			if(nxpos>=0)
			{
				nxpos += PXMAX;
			}
			else
			{
				nxpos += PXMIN;
			}
			if(nypos>=0)
			{
				nypos += PYMAX;
			}
			else
			{
				nypos +=PYMIN;
			}	
		}
		::PostMessage(hwnd,uMsg,wParam,MAKELPARAM(nxpos,nypos));
		break;

	}
}

DWORD CWndThreadManager::GetFreshTicCount()
{
	return m_freshtic;
}

DWORD CWndThreadManager::GetWindowList(HWND *wndlist, DWORD numo_lists ,DWORD *num_returned, BOOL porder)
{
	ALISTPOSITION obj;
	if((numo_lists < m_wndsList.GetCount())||(numo_lists ==0))
	{
		*num_returned = 0;
		return 1;
	}
	int i=0;
	if(porder)
	{
		
		for(obj = m_wndsList.GetHeadPosition();obj!=NULL;)   
		{
			wndlist[i] = m_wndsList.GetAt(obj).m_hwnd;
			i++;
			m_wndsList.GetNext(obj);
		}
	}
	else
	{
		for(obj = m_wndsList.GetTailPosition(); obj!=m_wndsList.GetHeadPosition(); )   
		{   			
			wndlist[i] = m_wndsList.GetAt(obj).m_hwnd;
			i++;
			m_wndsList.GetPrev(obj);
		}
	}
	*num_returned = m_wndsList.GetCount();
	return 0;
}

VOID CWndThreadManager::KeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam) //传入键盘消息，WndThreadManager 自动对应到窗口
{
	/*
	GUITHREADINFO guithreadinfo;
	memset(&guithreadinfo,0,sizeof(guithreadinfo));
	guithreadinfo.cbSize =sizeof(guithreadinfo);
	if(GetGUIThreadInfo(m_tid,&guithreadinfo))
	//PostThreadMessage(m_tid,uMsg, wParam, lParam);
	*/	
	if(IsWindow(m_guithreadinfo.hwndCaret))
	{
		KeyEvent(m_guithreadinfo.hwndCaret, uMsg, wParam, lParam,1);
	}
}

VOID CWndThreadManager::KeyEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam,WORD method) //返回窗口相对坐标，WndThreadManager不用修改参数
{
	if(method ==0)
	{
		if(uMsg!=WM_CHAR)
		{
			HWND hwndroot;
			if(IsWindow(m_guithreadinfo.hwndCaret))
			{
				hwndroot = GetAncestor(m_guithreadinfo.hwndCaret,GA_ROOT);
				//if(hwndroot == hwnd)
				PostMessage(m_guithreadinfo.hwndCaret,uMsg,wParam,lParam);
			}
			else
			{
				if(IsWindow(m_guithreadinfo.hwndActive))
				{
					hwndroot = GetAncestor(m_guithreadinfo.hwndActive,GA_ROOT);
					if(hwndroot == hwnd)
						PostMessage(m_guithreadinfo.hwndActive,uMsg,wParam,lParam);
				}
			}	
		}
		else
		{
			//输入中文可能需要在这里做手脚
		}
	}
	else
	{
		if(IsWindow(m_guithreadinfo.hwndCaret))
			PostMessage(m_guithreadinfo.hwndCaret,uMsg,wParam,lParam);
	}
}

BOOL CWndThreadManager::InitHookDll()
{
	if(m_binit)
	{
	}
	else
	{
		if(!m_hmod)
		{
			m_hmod = GetModuleHandleA(m_strHookModName);
			if(!m_hmod)
			{
				m_hmod = LoadLibraryA(m_strHookModName);
			}
		}
		if(m_hmod)
		{
			m_installhook = (TP_InstallHook)GetProcAddress(m_hmod,"InstallHook");
			if(m_installhook)
			{
				m_binit = TRUE;
			}
			else
			{
				MessageBoxA(GetActiveWindow(),"运行所需动态库文件错误","警告",MB_OK|MB_ICONWARNING);
			}
		}
		else
		{
			MessageBoxA(GetActiveWindow(),"运行所需动态库文件丢失或错误","警告",MB_OK|MB_ICONWARNING);
		}
	}
	return m_binit;
}


BOOL CWndThreadManager::InstallGlobalHook()
{
	if(m_binstall)
		return m_binstall;
	if(InitHookDll())
	{
		m_installhook(TRUE,0);
		m_binstall = TRUE;
	}
	return m_binstall;
}

BOOL CWndThreadManager::UnInstallGlobalHook()
{
	if(m_binstall)
	{
		m_installhook(FALSE,0);
		m_binstall = FALSE;
	}
	return TRUE;
}
VOID CWndThreadManager::SetTopMostWindow(HWND hwnd)
{
	m_topwindow = hwnd;
}

HWND CWndThreadManager::GetCaretWindowPos(LPRECT lprect)
{
	GUITHREADINFO guiinfo;
	memset(&guiinfo,0,sizeof(GUITHREADINFO));
	guiinfo.cbSize =sizeof(GUITHREADINFO);
	GetGUIThreadInfo(m_tid,&guiinfo);
	HWND hwnd= guiinfo.hwndCaret;
	if(hwnd)
	{
		HWND hwndx = GetAncestor(hwnd,GA_ROOT);
		memcpy(lprect,&(guiinfo.rcCaret),sizeof(RECT));
		RECT rcbase;
		POINT pt={0,0};
		::GetWindowRect(hwndx,&rcbase);
		::ClientToScreen(hwnd,&pt);
		pt.x-=rcbase.left;
		pt.y-=rcbase.top;
		lprect->left+=pt.x;
		lprect->right+=pt.x;
		lprect->top+=pt.y;
		lprect->bottom+=pt.y;
		return hwndx;
	}
	else
	{
		return NULL;
	}
}

VOID CWndThreadManager::IMECharInPut(HWND hwnd, WCHAR ch)
{
	GUITHREADINFO guiinfo;
	memset(&guiinfo,0,sizeof(GUITHREADINFO));
	guiinfo.cbSize =sizeof(GUITHREADINFO);
	GetGUIThreadInfo(m_tid,&guiinfo);
	HWND hwndCaret= guiinfo.hwndCaret;
	if(hwndCaret)
	{
		HWND hwndx = GetAncestor(hwndCaret,GA_ROOT);
		if(hwnd == hwndx)
		{			
			WPARAM wparam;
			LPARAM lparam;
			if(IsWindowUnicode(hwndCaret))
			{
				wparam = ch;
				lparam = 0x00020001;
				PostMessage(hwndCaret,WM_CHAR,wparam,lparam);
			}
			else
			{

				wparam = (ch&0xff00)>>8;
				lparam = 0x00020001;
				PostMessage(hwndCaret,WM_CHAR,wparam,lparam);
				wparam = (ch&0xff);
				PostMessage(hwndCaret,WM_CHAR,wparam,lparam);
			}
		}
	}
	else
	{
		return ;
	}

}
