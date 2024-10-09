// PWBrowser.cpp: implementation of the CPWBrowser class.
//
//////////////////////////////////////////////////////////////////////
/*
* FILE: PWBrowser.cpp
*
* DESCRIPTION: ActiveX Container Window Class cpp file
*
* CREATED BY:  linzhehui, 2009/06/10
*
* HISTORY:  2009/06/10, First edition by linzhehui
*            
* Decide to Use ATL to embed the web browser into the game ui for conviniance reasons.
*/
#define _WIN32_WINNT  0x0500

#include "windows.h"
#include "PWBrowser.h"
#include "EC_TimeSafeChecker.h"
//#include "ExplorerMan.h"

BOOL PreTranslateMessage(const MSG& msg,CPWBrowser* pBrowser) 
{
	//add by lzh to solve the msg problem of webbrowser
	if(pBrowser!=NULL)
	{
		if(IsWindow(pBrowser->GetHandle()))
		{
			if((msg.message < WM_KEYFIRST || msg.message > WM_KEYLAST) &&
				(msg.message < WM_MOUSEFIRST || msg.message > WM_MOUSELAST))
			{
				return FALSE;
			}
			HWND hwnd1 = msg.hwnd;
			HWND hParent = hwnd1;
			HWND hBrowser = pBrowser->GetHandle();
			int  loop =0;
			//if(msg.message==WM_KEYDOWN)
			if(msg.hwnd == pBrowser->GetHandle())
			{
				return FALSE;
			}
			while(IsWindow(hParent)&&(loop<4))
			{
				hParent = GetAncestor(hParent,GA_PARENT);
				if(hParent == pBrowser->GetCtrl())
					break;
				loop++;
			}
			if(hParent==pBrowser->GetCtrl())
			{
				LRESULT lTranslated = ::SendMessage(pBrowser->GetCtrl(), WM_FORWARDMSG, 0, (LPARAM)&msg);
				return (lTranslated ? TRUE : FALSE);
			}
			
		}
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

//CComModule CPWBrowser::_Module;
BOOL  CPWBrowser::_bPWBrowserInit = FALSE;
const TCHAR CPWBrowser::szPWB_ClassTitle[]=_T("PW_WebBrowser 浏览器");

struct THREADPARAM 
{
	HWND hParent;
	RECT *prect;
	CPWBrowser *pBrowser;
};

DWORD WINAPI WndThreadProc(LPVOID lpParameter)
{
	
	THREADPARAM *hThreadParam = (THREADPARAM *)lpParameter;
	CPWBrowser *pBrowser = hThreadParam->pBrowser;
	PRECT pRc=hThreadParam->prect;
	pBrowser->CreateX(hThreadParam->hParent,*pRc);
	BOOL bRet;
	MSG  msg;
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (bRet == -1)
		{
			// handle the error and possibly exit
			return bRet;
		}
		else
		{
			 
			if((msg.message==WM_MOUSEACTIVATE)||(msg.message==WM_ACTIVATE)||(msg.message==WM_ACTIVATEAPP )||(msg.message==WM_NCACTIVATE)||(msg.message==WM_CHILDACTIVATE))
			{

			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
		}
	}
	//delete pBrowser;
	return bRet;

}

CPWBrowser::CPWBrowser(CExplorerMan* pParent)
{
	m_bIsFullWindow = FALSE;
	m_hwndBackIE = FALSE;
	m_bIsquit = FALSE;
	m_eventsink = NULL;
	m_backeventsink =NULL;
	m_hthread = NULL;
	m_dwThreadId = 0;
	m_Mode = 1; 
	m_Mutex = NULL;
	m_bInit = FALSE;
	m_hParentWnd = NULL;
	m_pExplorer = pParent;
	m_upDate = FALSE;
	m_upUrl = FALSE;
	m_upGoForward = FALSE;
	m_upRefresh = FALSE;
	m_upGoBackward = FALSE;
	m_upNavigate = FALSE;
	m_upEnableNotifyLua = FALSE;
	m_upMoveWindow = FALSE;
	m_hUrlMutex = NULL;
	m_dwTransparentconstant = 1;
	m_dwTransparent =1;
	m_bDocReady =  FALSE;
	m_dLastErrorTick=0;
	m_bFirstReady = FALSE;
}

CPWBrowser::CPWBrowser()
{
	m_bIsFullWindow = FALSE;
	m_hwndBackIE = FALSE;
	m_bIsquit = FALSE;
	m_eventsink = NULL;
	m_backeventsink =NULL;
	m_hthread = NULL;
	m_dwThreadId = 0;
	m_Mode = 1; 
	m_Mutex = NULL;
	m_bInit = FALSE;
	m_hParentWnd = NULL;
	m_pExplorer = NULL;
	m_upDate = FALSE;
	m_upUrl = FALSE;
	m_upGoForward = FALSE;
	m_upRefresh = FALSE;
	m_upGoBackward = FALSE;
	m_upNavigate = FALSE;
	m_upEnableNotifyLua = FALSE;
	m_upMoveWindow = FALSE;
	m_hUrlMutex = NULL;
	m_dwTransparentconstant =1;
	m_dwTransparent =1;
	m_bDocReady =  FALSE;
	m_dLastErrorTick=0;
	m_bFirstReady = FALSE;
}


CPWBrowser::~CPWBrowser()
{
	m_dwTransparent = 1;
	//这里销毁窗体释放资源
	if(WaitForSingleObject(m_hUrlMutex,INFINITE))
	{
		ReleaseMutex(m_hUrlMutex);
	}
	
	if(IsWindow())
	{
		KillTimer(PWB_Timer1);
		if(!CWindow::DestroyWindow())
		{
			DWORD err = GetLastError();
		}
		
	}
	
	m_pWB2.Detach();
	m_pWB2Back.Detach();

	if(m_eventsink)
	{
		delete m_eventsink;
		m_eventsink = NULL;
	}
	if(m_backeventsink)
	{
		delete m_backeventsink;
		m_backeventsink = NULL;
	}
	if (m_DocHostUIHandler)
	{
		m_DocHostUIHandler = NULL;
	}

	if(m_hthread)
	{
		CloseHandle(m_hthread);
		m_hthread = NULL;
	}
	if(NULL != m_hUrlMutex)
	{
		CloseHandle(m_hUrlMutex);
	}

	m_hParentWnd = NULL;
}

BOOL CPWBrowser::InitContainer(HINSTANCE hInstance)
{
	if(_bPWBrowserInit)
		return _bPWBrowserInit;
	else
	{
		if(_Module.Init(ObjectMap,hInstance) == S_OK)
		{
			
		}
	}
	return FALSE;
}

VOID CPWBrowser::TermContainer()
{
	if(_bPWBrowserInit)
	{
		_Module.Term();
	}
}
BOOL CPWBrowser::InitContainer()
{
	HINSTANCE hinstance;
	if(_bPWBrowserInit)
		return _bPWBrowserInit;
	else
	{
		hinstance = GetModuleHandle(NULL);
		if(_Module.Init(ObjectMap,hinstance) ==S_OK)
		{
			_bPWBrowserInit = TRUE;
		}
	}
	return _bPWBrowserInit;
}

HWND CPWBrowser::Create(HWND hWndParent, RECT& rcPos)
{
	//DWORD tid = GetCurrentThreadId();
	if(InitContainer())
	{
		HWND res = NULL;
		if(!::IsWindow(hWndParent))
			hWndParent = NULL;
		else
			m_hParentWnd = hWndParent;
		switch(m_Mode)
		{
		case 0:
			res = CreateInOtherThread(hWndParent, rcPos);
			m_bInit = TRUE;
			break;
		case 1:
			m_dwThreadId = GetCurrentThreadId();
			DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&m_hthread,0,false,DUPLICATE_SAME_ACCESS);
			res = CWindowImplSafe<CPWBrowser>::Create(hWndParent, rcPos, NULL,WS_POPUP, 0, 0U, NULL);
			m_bInit = TRUE;
			break;
		default:
			return NULL;
			break;
		}
		return res;
	}
	else
		return NULL;
}

VOID CPWBrowser::CreateX(HWND hWndParent, RECT& rcPos)
{
	CWindowImplSafe<CPWBrowser>::Create(hWndParent, rcPos, NULL,WS_POPUP|WS_DISABLED|SW_SHOWNORMAL, WS_EX_NOACTIVATE|WS_EX_STATICEDGE, 0U, NULL);
	m_bThreadCreateOK = TRUE;
}

HWND CPWBrowser::CreateInOtherThread(HWND hWndParent, RECT& rcPos)
{
	m_bThreadCreateOK = FALSE;
	THREADPARAM threadparam;
	threadparam.hParent = hWndParent;
	threadparam.prect = &rcPos;
	threadparam.pBrowser = this;
	m_hthread = CreateThread(NULL,0,WndThreadProc,(void *)(&threadparam),0,&m_dwThreadId);
	
	for(int i=0;i<100;i++)
	{
		if(m_bThreadCreateOK)
		{
			break;
		}
		Sleep(500);
	}
	keybd_event( 0, 0, 0, 0 );
	if(m_bThreadCreateOK)
		return m_hWnd;
	else
		return NULL;
}

void CPWBrowser::SetScrollHeight(int nHeight)
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	m_nScrollHeight = nHeight; 
	//m_pExplorer->SetUIScrollHeight(nHeight);
	ReleaseMutex(m_hUrlMutex);
}

BOOL CALLBACK CPW_EnumChildProc(HWND hwnd, LPARAM lParam)
{
	*(HWND*)lParam = hwnd;
	return FALSE;
}

LRESULT CPWBrowser::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	static BOOL bIntimer = FALSE;
	switch(uMsg)
	{
	case WM_NCACTIVATE:
		if (wParam)
		{
			//	激活IE控件窗口时，也激活主窗口
			::PostMessage(g_pGame->GetGameInit().hWnd, WM_NCACTIVATE, wParam, 0);
		}
		bHandled = FALSE;
		break;

	case WM_COMMAND:
		switch(wParam )
		{
		case 2:
			bHandled = TRUE;
			break;
		default:
			break;
		}
		break;
	case WM_CREATE:
		{	
			
			SetupAuiApiPoints();
			LONG lstyle = GetWindowLong(GWL_EXSTYLE);
			SetWindowLongA(m_hWnd,GWL_EXSTYLE,lstyle|WS_EX_LAYERED);
			COLORREF  colork=0x00ffffff;
			My_SetLayeredWindowAttributes(m_hWnd,colork,m_dwTransparent,LWA_ALPHA);//必须设置这玩意才会有窗口显示
			
			AtlAxWinInit();
			//对于老版本IE,创建两个对象来兼容，后台对象仅仅用于扩展事件，不会有太多资源浪费
			m_hwndIE = CreateWindow(_T("AtlAxWin"), _T("Shell.Explorer.2"), WS_CHILD|WS_VISIBLE, 10, 10, 20, 20, 
				m_hWnd, (HMENU)0, GetModuleHandle(NULL), NULL);
			m_hwndBackIE = CreateWindow(_T("AtlAxWin"), _T("Shell.Explorer.2"), WS_CHILD, 0, 0, 10, 10, 
				m_hWnd, (HMENU)100, GetModuleHandle(NULL), NULL); 
			
			SetWindowText(szPWB_ClassTitle);
			
			CComPtr<IUnknown>	 punkIE;
			CComPtr<IUnknown>	 punkBackIE;
			TCHAR pszBuf[] =_T("about:blank");
			CComVariant vempty, vUrl(pszBuf);
			if (AtlAxGetControl(m_hwndIE, &punkIE) == S_OK)
			{
				m_pWB2 = punkIE;
				if(AtlAxGetControl(m_hwndBackIE, &punkBackIE)== S_OK)
				{
					m_pWB2Back = punkBackIE;
				}
				
				if(((IWebBrowser2*)m_pWB2))
				{

					CComObject<CPWIESink>::CreateInstance(&m_eventsink);
					CComObject<CPWBackIESink>::CreateInstance(&m_backeventsink);
					CComObject<CPWIEDocHostUIHandler>::CreateInstance(&m_DocHostUIHandler);
					m_eventsink->AdviseToIE(punkIE);
					m_eventsink->m_pBrowser = this;
					m_backeventsink->AdviseToIE(punkBackIE);
					m_backeventsink->m_pBrowser = this;
					
					CAxWindow axWindow = m_hwndIE;
					axWindow.SetExternalUIHandler(m_DocHostUIHandler);
					
					HRESULT hr = S_OK;
					if( SUCCEEDED( hr))
					{
						m_pWB2->put_Silent(VARIANT_TRUE);
						m_pWB2->Navigate2 ( &vUrl, &vempty, &vempty, &vempty, &vempty );	
						while(!this->IsFirstReady())
						{
							MSG msg;
							if(GetMessage(&msg, NULL, 0, 0)) 
							{
								TranslateMessage(&msg);
								DispatchMessage(&msg);
							}
						}
					}
					punkIE.Release();
					punkBackIE.Release();
				}
				else
				{
					MessageBox(_T("获取浏览器控件IWebBrowser2接口失败"), _T("警告"), MB_OK|MB_ICONWARNING) ;
					CWindow::DestroyWindow();
				}	
			}
			m_hUrlMutex = CreateMutex(NULL,FALSE,NULL);
			SetTimer(PWB_Timer1,PWB_Timer1_SLAP);
			bHandled = TRUE;
		}
		break;
	case WM_DESTROY:
		bHandled = FALSE;
		break;
	case WM_SIZE:
		::MoveWindow( m_hwndIE, 0, 0, LOWORD( lParam), HIWORD( lParam) , FALSE) ;
		break;
	case WM_TIMER:
		HDC hdc;
		hdc = ::GetDC(m_hWnd);
		int num;
		num = GetDeviceCaps(hdc, BITSPIXEL);
		::ReleaseDC (m_hWnd,hdc);
		if(num>16)
		{
			m_dwTransparentconstant = 1;
		}
		else
		{
			m_dwTransparentconstant = 5;
		}

		if(bIntimer)
		{
			bHandled = TRUE;
			break;
		}
		bIntimer = TRUE;

		if(::IsWindow(m_hwndIE))
		{
			// 对于老版本的IE，需要查询m_hwndIE 的子窗口，如果没有子窗口，销毁并重建窗口
			// 由于ALT 不能保证重购时间的内存泄露，所以销毁这个窗口
			HWND hChild =NULL;
			if(EnumChildWindows(m_hwndIE,CPW_EnumChildProc,(LPARAM)&hChild))
			{
			}
			if(!hChild) //找不到如子窗口，说明已经被脚本关闭
			{
				RECT rc;
				::GetWindowRect(m_hwndIE,&rc);
				HWND hWndtmp = CreateWindow(_T("AtlAxWin"), _T("Shell.Explorer.2"), WS_CHILD|WS_VISIBLE, 10, 10, rc.right-rc.left, rc.bottom-rc.top, m_hWnd, (HMENU)0, GetModuleHandle(NULL), NULL);
				HWND hWndtmp1= m_hwndIE;
				m_hwndIE = hWndtmp;

				CComPtr<IUnknown>	 punkIE;
				if (AtlAxGetControl(m_hwndIE, &punkIE) == S_OK)
				{
					m_pWB2 = punkIE;
					if(((IWebBrowser2*)m_pWB2))
					{
						m_eventsink->UnAdviseToIE();
						m_eventsink->AdviseToIE(punkIE);
						m_eventsink->m_pBrowser = this;
						CAxWindow axWindow = m_hwndIE;
						axWindow.SetExternalUIHandler(m_DocHostUIHandler);
						HRESULT hr = S_OK;
						if( SUCCEEDED( hr))
						{
							m_pWB2->put_Silent(VARIANT_TRUE);
							TCHAR pszBuf[] =_T("about:blank");
							CComVariant vempty, vUrl(pszBuf);
							m_pWB2->Navigate2 ( &vUrl, &vempty, &vempty, &vempty, &vempty );	
						}
						punkIE.Release();
					}
				}
				CAxWindow axWindow = hWndtmp1;
				axWindow.DestroyWindow();
			}
		}
			
		if(m_upDate)
		{
			m_upDate = FALSE;
			CWindow::ShowWindow(m_showcmd);
		}
		if(m_upUrl)
		{
			CComVariant vempty;		
			m_upUrl = FALSE;
			if(((IWebBrowser2*)m_pWB2))
				m_pWB2->Navigate2( &m_Url, &vempty, &vempty, &vempty, &vempty );
	
		}
		if(m_upRefresh)
		{
			m_upRefresh = FALSE;
			if(((IWebBrowser2*)m_pWB2))
				m_pWB2->Refresh();
		}
		if(m_upGoBackward)
		{ 
			m_upGoBackward = FALSE;
			if(((IWebBrowser2*)m_pWB2))
				m_pWB2->GoForward();
		}
		if(m_upGoForward)
		{
			m_upGoForward = FALSE;
			if(((IWebBrowser2*)m_pWB2))
				m_pWB2->GoBack();
		}
		if (m_upNavigate)
		{
			if(WaitForSingleObject(m_hUrlMutex,0)==WAIT_OBJECT_0)
			{
				m_upNavigate = FALSE;
				if(((IWebBrowser2*)m_pWB2))
				{
					BSTR bstrUrl;
					if (S_OK == m_pWB2->get_LocationURL(&bstrUrl))
					{
						m_upEnableNotifyLua = TRUE;
#ifndef OLE2ANSI
						m_LocalUrl = bstrUrl;
#else
						m_LocalUrl = AS2WC(bstrUrl);
#endif			
						m_LocalUrlEx = m_LocalUrl;
						if (m_LocalUrl.GetLength() > 200)
						{
							m_LocalUrl.CutRight(m_LocalUrl.GetLength() - 200);
						}						
					}
				}	
				ReleaseMutex(m_hUrlMutex);
			}
		}
		if (m_upMoveWindow)
		{
			m_upMoveWindow = FALSE;
			FixIE6Impl();
		}

		if (m_pExplorer)
		{
			m_pExplorer->OnWinTimer();
		}
		bHandled = TRUE;
		bIntimer = FALSE;
		break;
	case WM_CLOSE: 
		// In our game, Never Close this window by close window or ALT+F4
		bHandled = TRUE;
		break;

	default:
		break;
	}

	return S_OK;
}


VOID CALLBACK APCMoveWindowProc(ULONG_PTR dwParam)
{
	__try
	{
		CPWBrowser* pbrowser=(CPWBrowser*)dwParam;
		if(IsWindow(pbrowser->m_hWnd))
		{
			pbrowser->MoveWindowOld();
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER )
	{
		
	}
}

VOID CALLBACK APCDestroyWindowProc(ULONG_PTR dwParam)
{
	__try
	{
		CPWBrowser* pbrowser=(CPWBrowser*)dwParam;
		delete pbrowser;
	}
	__except(EXCEPTION_EXECUTE_HANDLER )
	{
		
	}
}

BOOL CPWBrowser::MoveWindow( LPCRECT lpRect, BOOL bRepaint)
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	m_bMoveRepaint = bRepaint;
	m_rc = *lpRect;
	if(GetCurrentThreadId()!=m_dwThreadId)
	{
		if(!QueueUserAPC(APCMoveWindowProc,m_hthread,(ULONG_PTR)this))
		{
			DWORD err = GetLastError();
		}
	}
	else
	{
		MoveWindowOld();
	}
	ReleaseMutex(m_hUrlMutex);
	return TRUE;
}

BOOL CPWBrowser::ShowWindow( int nCmdShow )
{
	if (m_showcmd != (UINT)nCmdShow)
	{
		m_showcmd = nCmdShow;
		m_upDate = TRUE;
	}

	return TRUE;	
}

BOOL CPWBrowser::MoveWindowOld()
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	BOOL res = CWindow::MoveWindow(&m_rc,m_bMoveRepaint);
	ReleaseMutex(m_hUrlMutex);
	return res;
}

BOOL CPWBrowser::AddDownloadUrl(WCHAR *FileUrl)
{
	if (m_pExplorer)
		return m_pExplorer->AddDownloadUrl(FileUrl);
	else
		return FALSE;
}

VOID CPWBrowser::DelayDestroy()
{
	QueueUserAPC(APCDestroyWindowProc,m_hthread,(ULONG_PTR)this);
}

void CPWBrowser::NavigateUrl(const CHAR *Url)
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	if(m_dwThreadId!=GetCurrentThreadId())
	{	
		CComVariant vUrl(Url);
		m_Url =vUrl;
		m_upUrl = TRUE;
	}
	else
	{	
		CComVariant vempty, vUrl(Url);
		if(((IWebBrowser2*)m_pWB2))
		{
			m_pWB2->Navigate2 ( &vUrl, &vempty, &vempty, &vempty, &vempty );	
		}
	}
	ReleaseMutex(m_hUrlMutex);
}

void CPWBrowser::NavigateUrl(VARIANT* pvUrl)
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	if(m_dwThreadId!=GetCurrentThreadId())
	{	
		CComVariant vUrl(*pvUrl);
		m_Url =vUrl;
		m_upUrl = TRUE;
	}
	else
	{	
		CComVariant vempty, vUrl(*pvUrl);
		if(((IWebBrowser2*)m_pWB2))
		{
			m_pWB2->Navigate2 ( &vUrl, &vempty, &vempty, &vempty, &vempty );	
		}
	}
	ReleaseMutex(m_hUrlMutex);
}

void CPWBrowser::NavigateUrl(const WCHAR *Url)
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	if(m_dwThreadId!=GetCurrentThreadId())
	{	
		CComVariant vUrl(Url);
		m_Url =vUrl;
		m_upUrl = TRUE;
	}
	else
	{	
		CComVariant vempty, vUrl(Url);
		if(((IWebBrowser2*)m_pWB2))
		{
			m_pWB2->Navigate2 ( &vUrl, &vempty, &vempty, &vempty, &vempty );	
		}
	}
	ReleaseMutex(m_hUrlMutex);
}

void CPWBrowser::GoForward()
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	if(m_dwThreadId!=GetCurrentThreadId())
	{	
		m_upGoForward = TRUE;
	}
	else
	{
		if(((IWebBrowser2*)m_pWB2))
		{			
			m_pWB2->GoForward();	
		}
	}
	ReleaseMutex(m_hUrlMutex);
}

void CPWBrowser::GoBackward()
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	if(m_dwThreadId!=GetCurrentThreadId())
	{	
		m_upGoBackward = TRUE;
	}
	else
	{
		if(((IWebBrowser2*)m_pWB2))
		{
			m_pWB2->GoBack();	
		}
	}
	ReleaseMutex(m_hUrlMutex);
}


void CPWBrowser::Refresh()
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	if(m_dwThreadId!=GetCurrentThreadId())
	{	
		m_upRefresh = TRUE;
	}
	else
	{	
		if(((IWebBrowser2*)m_pWB2))
		{
			m_pWB2->Refresh();
		}
	}
	ReleaseMutex(m_hUrlMutex);
}

void CPWBrowser::FixIE6()
{
	if(m_dwThreadId!=GetCurrentThreadId())
	{	
		m_upMoveWindow = TRUE;
	}
	else
	{
		FixIE6Impl();
	}
}

void CPWBrowser::FixIE6Impl()
{
	if (::IsWindow(m_hwndIE))
	{
		//	修复IE6打开新浪微博登录页面时错位问题
		RECT rect;
		::GetWindowRect(m_hwndIE, &rect);
		POINT pt;
		pt.x = rect.left;
		pt.y = rect.top;
		HWND hParent = ::GetParent(m_hwndIE);
		::ScreenToClient(hParent, &pt);
		::MoveWindow(m_hwndIE, pt.x, pt.y, rect.right-rect.left+1, rect.bottom-rect.top, TRUE);
		::MoveWindow(m_hwndIE, pt.x, pt.y, rect.right-rect.left, rect.bottom-rect.top, TRUE);
	}
}


void CPWBrowser::GetCurrentUrl()
{
	WaitForSingleObject(m_hUrlMutex,INFINITE);
	UpdateCurrentUrl();
	ReleaseMutex(m_hUrlMutex);
}

void CPWBrowser::OnTick()
{
	if (m_upEnableNotifyLua)
	{
		if(WaitForSingleObject(m_hUrlMutex,0)==WAIT_OBJECT_0)
		{
			m_upEnableNotifyLua = FALSE;
			ReleaseMutex(m_hUrlMutex);
		}
	}
	
	if (m_pExplorer)
	{
		m_pExplorer->OnTick();
	}
}

void CPWBrowser::OnBeforeNavigate2(IDispatch** pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel)
{	
	SetDocReady(FALSE);

	// test the sink event
#ifndef OLE2ANSI
	SetWindowTextW(URL->bstrVal);
	int len = 0;
	len = wcslen(URL->bstrVal);
	if(len>4)
	{		
		if(wcsicmp(URL->bstrVal+len-4,_T(".rwm"))==0)
		{
			AddDownloadUrl(URL->bstrVal);
			*Cancel = VARIANT_TRUE ;
		}
	}
	
#else
	SetWindowTextA(URL->bstrVal);
	int len = 0;
	len = strlen(URL->bstrVal);
	if(len>4)
	{		
		if(stricmp(URL->bstrVal+len-4,(".rwm"))==0)
		{
			AddDownloadUrl(URL->bstrVal);
			*Cancel = VARIANT_TRUE ;
		}
	}
	
#endif

	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->OnBeforeNavigate2(pDisp, URL, Flags, TargetFrameName, PostData, Headers, Cancel);
}

void CPWBrowser::OnNavigateComplete2(IDispatch** pDisp, VARIANT* URL)
{
	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->OnNavigateComplete2(pDisp, URL);
}

void CPWBrowser::OnNewWindow2(IDispatch** ppDisp, VARIANT_BOOL* Cancel)
{	
	*ppDisp = GetBackIE();

	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->OnNewWindow2(ppDisp, Cancel);
}

void CPWBrowser::OnNewWindow3(IDispatch** ppDisp, VARIANT_BOOL* Cancel, unsigned long dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
{	
	*Cancel = VARIANT_TRUE;
	NavigateUrl((WCHAR *)bstrUrl);
	UpdateCurrentUrl();

	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->OnNewWindow3(ppDisp, Cancel, dwFlags, bstrUrlContext, bstrUrl);
}

void CPWBrowser::OnWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL* Cancel)
{	
	if (IsChildWindow != VARIANT_TRUE)
	{
		*Cancel = VARIANT_TRUE;
		NavigateUrl(_T("about:blank"));
	}
	
	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->OnWindowClosing(IsChildWindow, Cancel);
}

void CPWBrowser::OnQuit()
{
	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->OnQuit();
}

void CPWBrowser::DocumentComplete(IDispatch* pDisp,VARIANT* URL)
{
	if (GetWebIDisPatch() != pDisp)
		return;

	SetDocReady(TRUE);
	SetFirstReady();
	FixIE6();
	
	while (pDisp && 0)		//	while 中代码用于使用游戏AUI显示滚动条
	{
		CComQIPtr<IWebBrowser2> spWeb = pDisp;
		if (!pDisp) break;

		CComPtr<IDispatch> spDoc;
		spWeb->get_Document((IDispatch**)&spDoc);
		if (!spDoc) break;
		
		CComPtr<IHTMLDocument2> spDoc2;
		spDoc->QueryInterface(IID_IHTMLDocument2, (void **)&spDoc2);
		if (!spDoc2) break;
		
		CComPtr<IHTMLElement> spBody;
		spDoc2->get_body(&spBody);
		if (!spBody) break;
		
		CComPtr<IHTMLElement2> spElement2;
		spBody->QueryInterface(IID_IHTMLElement2,(void**)&spElement2);
		if (spElement2)
		{
			long scroll_height;
			// 向下滚动100个像素
			spElement2->put_scrollTop( 0 ); 
			// 获得滚动条高度
			spElement2->get_scrollHeight( &scroll_height );
			SetScrollHeight( scroll_height);
			// 获得滚动条宽度
			// pElement->get_scrollWidth( &scroll_width );
			// 获得滚动条位置，从顶端开始
			long scroll_top;
			spElement2->get_scrollTop( &scroll_top );
			SetScrollPos(scroll_top);
		}
		
		CComPtr<IHTMLStyle> spStyle;
		spBody->get_style(&spStyle);
		if (spStyle)
			spStyle->put_overflow(_T("scroll"));

		break;
	}

	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->DocumentComplete(pDisp, URL);
}

void CPWBrowser::OnNavigateError(IDispatch* pDisp,VARIANT* URL,VARIANT* Frame,VARIANT* StatusCode,VARIANT_BOOL* Cancel)
{
	FixIE6();
	
	m_dLastErrorTick = GetTickCount();

	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->OnNavigateError(pDisp, URL, Frame, StatusCode, Cancel);
}

void CPWBrowser::OnFileDownLoad(VARIANT_BOOL ActiveDocument, VARIANT_BOOL* Cancel)
{	
	if (CECTimeSafeChecker::ElapsedTimeFor(m_dLastErrorTick)<1000)
		*Cancel = VARIANT_TRUE;

	if (m_pExplorer && m_pExplorer->GetUI())
		m_pExplorer->GetUI()->OnFileDownLoad(ActiveDocument, Cancel);
}