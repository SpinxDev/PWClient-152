// PWBrowser.h: interface for the CPWBrowser class.
//
//////////////////////////////////////////////////////////////////////
/*
 * FILE: PWBrowser.h
 *
 * DESCRIPTION: ActiveX Container Window Class .h file
 *
 * CREATED BY:  linzhehui, 2009/06/10
 *
 * HISTORY:  2009/06/10, First edition by linzhehui
 *            
 * Decide to Use ATL to embed the web browser into the game ui for conviniance reasons.
 * 
 * Major Upgrade: 2009/09/18, To compatable with xp sp1,win2000 (No NewWindows3 interface in DWebBrowserEvents2)
 *
 * Use NewWindows2 and two Browser Objs to finish the job of NewWindows3, The back browser just get the url and set it to the front one.
 */



#ifndef PWBRWOSER_H
#define PWBRWOSER_H
#undef  SubclassWindow
#include "PWAtl.h"
#include "Dlgbase.h"
#include "DlgExplorer.h"
#include "EC_RTDebug.h"
#include "EC_Global.h"
#include "EC_Game.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern void CreateBrowser(HWND handle);
typedef  BOOL (WINAPI* TYP_SetLayeredWindowAttributes)(HWND hwnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags);
extern TYP_SetLayeredWindowAttributes My_SetLayeredWindowAttributes;
extern VOID SetupAuiApiPoints();
// Realize IDispatchImpl to get the sink Message from the ActiveX control

#define PWSINKID_FRONTEVENTS 0
#define  PWSINKID_BACKEVENTS  1

class ATL_NO_VTABLE CPWIESink;
class ATL_NO_VTABLE CPWBackIESink;
class ATL_NO_VTABLE CPWIEDocHostUIHandler;
class CDlgBase;
class CExplorerMan;

#ifndef LWA_ALPHA
#define LWA_ALPHA               0x00000002
#endif

#define PWB_Timer1 1
#define PWB_Timer1_SLAP 50

class CPWBrowser;
BOOL PreTranslateMessage(const MSG& msg,CPWBrowser* pBrowser);

class CPWBrowser : public CWindowImplSafe<CPWBrowser>
{
public:
	DECLARE_WND_CLASS(_T("PW_WEBBROWSER"))
	BEGIN_MSG_MAP(CPWBrowser)
	MESSAGE_HANDLER(WM_CREATE, OnMessage)
	MESSAGE_HANDLER(WM_SIZE,   OnMessage)
	MESSAGE_HANDLER(WM_DESTROY,OnMessage)
	MESSAGE_HANDLER(WM_COMMAND,OnMessage)
	MESSAGE_HANDLER(WM_TIMER,  OnMessage)
	MESSAGE_HANDLER(WM_CLOSE,  OnMessage)
	MESSAGE_HANDLER(WM_PARENTNOTIFY,  OnMessage)
	MESSAGE_HANDLER(WM_NCACTIVATE,  OnMessage)
	END_MSG_MAP()

	// 动态连接库调用必须提供DLL模块的 hinstance
	BOOL InitContainer(HINSTANCE hInstance);
	// EXE调用如不提供 hinstance，利用GetModuleHandle(NULL) 获取
	BOOL InitContainer();
	
	CPWBrowser();
	CPWBrowser(CExplorerMan* pParent);
	virtual ~CPWBrowser();
	
	HWND GetHandle()
	{
		return m_hWnd;
	}
	HWND Create(HWND hWndParent, RECT& rcPos);
	VOID CreateX(HWND hWndParent, RECT& rcPos);
	HWND CreateInOtherThread(HWND hWndParent, RECT& rcPos);
	BOOL SetWindowTextA(LPCSTR lpString)
	{
		return ::SetWindowTextA(m_hWnd,lpString);
	}
	BOOL SetWindowTextW(LPCWSTR lpString)
	{
		return ::SetWindowTextW(m_hWnd,lpString);
	}
	BOOL IsReady() {return m_bInit;}
	BOOL MoveWindow( LPCRECT lpRect, BOOL bRepaint = TRUE);
	BOOL MoveWindowOld();
	BOOL AddDownloadUrl(WCHAR *FileUrl);
	BOOL CanDelete(DWORD curThreadID) {return m_dwThreadId==curThreadID;};
	void DelayDestroy();
	void NavigateUrl(const WCHAR *Url);
	void NavigateUrl(const CHAR *Url);
	void NavigateUrl(VARIANT *Url); 
	void GoBackward();
	void GoForward();
	void Refresh();
	void FixIE6();
	void GetCurrentUrl();
	BOOL ShowWindow( int nCmdShow );
	void OnBrowserQuit(){m_bIsquit = TRUE;	};
	void SetDocReady(BOOL bReady){m_bDocReady = bReady;};
	HWND GetCtrl(){return m_hwndIE;}
	void SetActive(bool bActive)
	{
		WaitForSingleObject(m_hUrlMutex,INFINITE);
		if((bActive)&&(m_dwTransparent == m_dwTransparentconstant))
		{
			ReleaseMutex(m_hUrlMutex);
			return;
		}
		if((!bActive)&&(m_dwTransparent==0))
		{
			ReleaseMutex(m_hUrlMutex);
			return;
		}
		m_dwTransparent =bActive?m_dwTransparentconstant:0;
		{
			COLORREF  colork=0x00ffffff;
			My_SetLayeredWindowAttributes(m_hWnd,colork,m_dwTransparent,LWA_ALPHA);//必须设置这玩意才会有窗口显示
		}
		ReleaseMutex(m_hUrlMutex);
	}
	static VOID CPWBrowser::TermContainer();

	BOOL IsDocReady()
	{
		WaitForSingleObject(m_hUrlMutex,INFINITE);
		BOOL res = FALSE;
		if(m_pWB2)
		{
			READYSTATE rstate;
			VARIANT_BOOL bBusy;
			HRESULT hr1 = m_pWB2->get_ReadyState(&rstate);
			HRESULT hr2 = m_pWB2->get_Busy(&bBusy);
			if(SUCCEEDED(hr1)&&SUCCEEDED(hr2))
			{
				if((rstate==READYSTATE_COMPLETE)&&(!bBusy))
					res = TRUE;
			}
		}
		ReleaseMutex(m_hUrlMutex);
		return res;
	}
	VOID WaitReady()
	{
		while(!IsDocReady())
		{
			Sleep(50);
		}
	}
	BOOL IsFirstReady(){ return m_bFirstReady; }
	void SetFirstReady(){ m_bFirstReady = TRUE; }
	CComPtr<IDispatch> GetWebIDisPatch()
	{
		CComPtr<IDispatch> spDisp;
		WaitForSingleObject(m_hUrlMutex,INFINITE);

		if(((IWebBrowser2*)m_pWB2))
			m_pWB2->QueryInterface(IID_IDispatch,(void**)&spDisp);

		ReleaseMutex(m_hUrlMutex);
		return spDisp;
	}

	IDispatch *GetBackIE()
	{
		return m_pWB2Back;
	}
	void UpdateCurrentUrl()
	{
		WaitForSingleObject(m_hUrlMutex,INFINITE);
		m_upNavigate = TRUE;
		ReleaseMutex(m_hUrlMutex);
	}

	void OnTick();

	CComPtr<IHTMLDocument> GetDocument()
	{
		CComPtr<IHTMLDocument> spDoc;
		WaitForSingleObject(m_hUrlMutex,INFINITE);

		CComPtr<IUnknown>	 punkIE;
		if (AtlAxGetControl(m_hwndIE, &punkIE) == S_OK)
		{
			CComQIPtr<IWebBrowser2>pWB2 = punkIE;
			if(pWB2)
				pWB2->get_Document((IDispatch**)&spDoc);
		}

		ReleaseMutex(m_hUrlMutex);

		return spDoc;
	}

	CComPtr<IHTMLDocument2> GetDocument2()
	{
		CComPtr<IHTMLDocument2> spDoc2;
		WaitForSingleObject(m_hUrlMutex,INFINITE);

		CComPtr<IHTMLDocument> spDoc = GetDocument();
		if (spDoc)
			spDoc->QueryInterface(IID_IHTMLDocument2,(void**)&spDoc2);

		ReleaseMutex(m_hUrlMutex);
		return spDoc2;
	}
	
	
	CComPtr<IHTMLElement> GetElement()
	{
		CComPtr<IHTMLElement> spElement;
		WaitForSingleObject(m_hUrlMutex,INFINITE);

		CComPtr<IHTMLDocument2> spDoc2 = GetDocument2();
		if (spDoc2)
			spDoc2->get_body(&spElement);

		ReleaseMutex(m_hUrlMutex);
		return spElement;
	}

	CComPtr<IHTMLElement2> GetElement2()
	{
		CComPtr<IHTMLElement2> spElement2;
		WaitForSingleObject(m_hUrlMutex,INFINITE);
		
		CComPtr<IHTMLElement> spElement = GetElement();
		if (spElement)
			spElement->QueryInterface(IID_IHTMLElement2, (void **)&spElement2);

		ReleaseMutex(m_hUrlMutex);
		return spElement2;
	}

	CComPtr<IHTMLStyle> GetStyle()
	{
		CComPtr<IHTMLStyle> spStyle;
		WaitForSingleObject(m_hUrlMutex,INFINITE);

		CComPtr<IHTMLElement> spElement = GetElement();
		if (spElement)
			spElement->get_style(&spStyle);

		ReleaseMutex(m_hUrlMutex);
		return spStyle;
	}

 	CComPtr<IHTMLStyle2> GetStyle2()
	{
		CComPtr<IHTMLStyle2> spStyle2;
		WaitForSingleObject(m_hUrlMutex,INFINITE);

		CComPtr<IHTMLStyle> spStyle = GetStyle();
		if (spStyle)
			spStyle->QueryInterface(IID_IHTMLStyle2,(void**)&spStyle2);

		ReleaseMutex(m_hUrlMutex);
		return spStyle2;
	}

	CComPtr<IHTMLStyle3> GetStyle3()
	{
		CComPtr<IHTMLStyle3> spStyle3;
		WaitForSingleObject(m_hUrlMutex,INFINITE);

		CComPtr<IHTMLStyle> spStyle = GetStyle();
		if (spStyle)
			spStyle->QueryInterface(IID_IHTMLStyle3,(void**)&spStyle3);

		ReleaseMutex(m_hUrlMutex);
		return spStyle3;
	}

	void SetScrollHeight(int nHeight);
	int GetScrollHeight(){return m_nScrollHeight;}

	void SetScrollPos(int nPos) {m_nScrollPos = nPos;}
	int GetScrollPos()
	{
		WaitForSingleObject(m_hUrlMutex,INFINITE);
		DWORD nTry = 0;
		while((nTry<2)&&(!IsDocReady()))
		{
			nTry++;
		}
		if(!IsDocReady())
		{
			ReleaseMutex(m_hUrlMutex);
			return -1;
		}

		CComPtr<IHTMLElement2> spElement2 = GetElement2();
		if (spElement2)
		{
			long nScrollPos;
			spElement2->get_scrollTop( &nScrollPos ); 
			ReleaseMutex(m_hUrlMutex);
			return nScrollPos;
		}

		ReleaseMutex(m_hUrlMutex);
		return 0L;
	}

	void SetExplorer(CExplorerMan* pPtr)
	{
		WaitForSingleObject(m_hUrlMutex,INFINITE);
		if (!pPtr)
		{
			ReleaseMutex(m_hUrlMutex);
			return;
		}
		m_pExplorer = pPtr;
		ReleaseMutex(m_hUrlMutex);
	}

	void MoveScrollPos(int nDelta)
	{
		WaitForSingleObject(m_hUrlMutex,INFINITE);
		DWORD nTry = 0;
		while((nTry<2)&&(!IsDocReady()))
		{
			nTry++;
		}
		if(!IsDocReady())
		{
			ReleaseMutex(m_hUrlMutex);
			return;
		}
		
		CComPtr<IHTMLElement2> spElement2 = GetElement2();
		if (spElement2)
			spElement2->put_scrollTop(nDelta);

		ReleaseMutex(m_hUrlMutex);
	}

	//	event callback
	void OnBeforeNavigate2(IDispatch** pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel);
	void OnNavigateComplete2(IDispatch** pDisp, VARIANT* URL);
	void OnNewWindow2(IDispatch** ppDisp, VARIANT_BOOL* Cancel);
	void OnNewWindow3(IDispatch** ppDisp, VARIANT_BOOL* Cancel, unsigned long dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);
	void OnWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL* Cancel);
	void OnQuit();
	void DocumentComplete(IDispatch* pDisp,VARIANT* URL);
	void OnNavigateError(IDispatch* pDisp,VARIANT* URL,VARIANT* Frame,VARIANT* StatusCode,VARIANT_BOOL* Cancel);
	void OnFileDownLoad(VARIANT_BOOL ActiveDocument, VARIANT_BOOL* Cancel);

protected:
	LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void FixIE6Impl();
private:
	BOOL					m_bFirstReady;
	BOOL                    m_bDocReady;
	BOOL                    m_bIsFullWindow;
	CComQIPtr<IWebBrowser2>	m_pWB2 ;
	CComQIPtr<IWebBrowser2>	m_pWB2Back;
	BOOL   m_bIsquit;
	BOOL   m_upDate;
	UINT   m_showcmd;
	HWND   m_hwndIE;
	HWND   m_hwndBackIE;
	BOOL   m_upUrl;
	BOOL   m_upGoForward;
	BOOL   m_upRefresh;
	BOOL   m_upGoBackward;
	BOOL   m_upNavigate;
	BOOL   m_upEnableNotifyLua;
	BOOL   m_upMoveWindow;
	AWString m_LocalUrl;    // url after cut, 200
	AWString m_LocalUrlEx;  // whole url

	CComVariant  m_Url;
	HANDLE m_Mutex;
	BOOL   m_bThreadCreateOK;
	static BOOL       _bPWBrowserInit;
	static const TCHAR szPWB_ClassTitle[];
	CComObject<CPWIESink>* m_eventsink;
	CComObject<CPWBackIESink>* m_backeventsink;
	// 修改线程控制方式
	HANDLE       m_hthread;
	DWORD        m_dwThreadId;
	UINT         m_Mode; // ThreadMode =0, WinMode =1;
	BOOL         m_bInit;
	RECT         m_rc;
	BOOL         m_bMoveRepaint;
	HWND         m_hParentWnd;
	CComObject<CPWIEDocHostUIHandler> *m_DocHostUIHandler;
	HANDLE       m_hUrlMutex;
	BYTE         m_dwTransparent; //0, 1  
	BYTE         m_dwTransparentconstant; //根据分辨率计算最小的transparent 位。 //16位色是5， 32 位色是1

	CExplorerMan*	m_pExplorer;
	UINT		m_nScrollHeight;
	UINT		m_nScrollPos;
	
	DWORD m_dLastErrorTick;
	DWORD m_ErrorRetry;
};

class ATL_NO_VTABLE CPWIEDocHostUIHandler :
public CComObjectRootEx<CComSingleThreadModel>,//<CComMultiThreadModel>,
public IDispatchImpl<IDocHostUIHandlerDispatch, &IID_IDocHostUIHandlerDispatch, &LIBID_ATLLib>
{
public:
	BEGIN_COM_MAP(CPWIEDocHostUIHandler)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()
	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID,
	DWORD x,
	DWORD y,
	IUnknown __RPC_FAR *pcmdtReserved,
	IDispatch __RPC_FAR *pdispReserved,
	HRESULT __RPC_FAR *dwRetVal)
	{
		*dwRetVal = S_OK;      //This is what the WebBrowser control is looking for.
		//You can show your own context menu here.
		//To disable the right button click, we implement the IID_IDocHostUIHandlerDispatch
		return S_OK;  
	};
	
	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(DWORD __RPC_FAR *pdwFlags,
		DWORD __RPC_FAR *pdwDoubleClick) 
	{return E_NOTIMPL;};
	
	virtual HRESULT STDMETHODCALLTYPE ShowUI(DWORD dwID,IUnknown __RPC_FAR *pActiveObject,
		IUnknown __RPC_FAR *pCommandTarget,
		IUnknown __RPC_FAR *pFrame,
		IUnknown __RPC_FAR *pDoc,
		HRESULT __RPC_FAR *dwRetVal) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE HideUI(void) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE UpdateUI(void) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE EnableModeless(VARIANT_BOOL fEnable) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(VARIANT_BOOL fActivate) 
	{return E_NOTIMPL;} ;
	virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(VARIANT_BOOL fActivate) 
	{return E_NOTIMPL;} ;
	virtual HRESULT STDMETHODCALLTYPE ResizeBorder(long left,
		long top,
		long right,
		long bottom,
		IUnknown __RPC_FAR *pUIWindow,
		VARIANT_BOOL fFrameWindow) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
		DWORD hWnd,
		DWORD nMessage,
		DWORD wParam,
		DWORD lParam,
		BSTR bstrGuidCmdGroup,
		DWORD nCmdID,
		HRESULT __RPC_FAR *dwRetVal) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath( 
		BSTR __RPC_FAR *pbstrKey,
		DWORD dw) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE GetDropTarget( 
		IUnknown __RPC_FAR *pDropTarget,
		IUnknown __RPC_FAR *__RPC_FAR *ppDropTarget) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE GetExternal( 
		IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE TranslateUrl( 
		DWORD dwTranslate,
		BSTR bstrURLIn,
		BSTR __RPC_FAR *pbstrURLOut) 
	{return E_NOTIMPL;} ;
	
	virtual HRESULT STDMETHODCALLTYPE FilterDataObject( 
		IUnknown __RPC_FAR *pDO,
		IUnknown __RPC_FAR *__RPC_FAR *ppDORet) 
	{return E_NOTIMPL;} ;		
				
};

/* CPWIESink :  IDispatch Interface */
//ATL_NO_VTABLE

#ifndef DISPID_NEWWINDOW3
   #define DISPID_NEWWINDOW3 273
#endif

#ifndef DISPID_FILEDOWNLOAD     
   #define DISPID_FILEDOWNLOAD         270
#endif

class ATL_NO_VTABLE CPWIESink :
public CComObjectRootEx<CComSingleThreadModel>,//<CComMultiThreadModel>,
//public IDispatchImpl<IDocHostUIHandlerDispatch, &IID_IDocHostUIHandlerDispatch, &LIBID_ATLLib>,
public IDispEventImpl<PWSINKID_FRONTEVENTS, CPWIESink, &DIID_DWebBrowserEvents2>
{

private:
	CComPtr<IUnknown> m_pUnkIE;

public:
	CPWBrowser   *m_pBrowser;
	CPWIESink()  {m_pUnkIE =NULL;m_pBrowser = NULL;}
	~CPWIESink() {if(m_pUnkIE) m_pUnkIE.Detach();};
	HRESULT AdviseToIE( CComPtr<IUnknown> pUnkIE)
	{
		m_pUnkIE = pUnkIE;
		
		// retrieve information about the default source interface of an object
		// interface ID,LIBID and major and minor version numbers of the type library.
		AtlGetObjectSourceInterface( m_pUnkIE, &m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum) ;
		
		// connect the Dispatch interface.
		HRESULT hr = this->DispEventAdvise( m_pUnkIE);
		
		return hr ;
	}
	HRESULT UnAdviseToIE( )
	{
		if(m_pUnkIE)
		{
			HRESULT hr = this->DispEventUnadvise((IUnknown*)this);
			return hr ;
		}
		return S_OK;
		// connect the Dispatch interface.

	}
	BEGIN_COM_MAP(CPWIESink)
		COM_INTERFACE_ENTRY_IID(DIID_DWebBrowserEvents2, CPWIESink)
		//COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()
		
	BEGIN_SINK_MAP(CPWIESink)
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2)
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW2, OnNewWindow2)	
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW3, OnNewWindow3)
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_WINDOWCLOSING, OnWindowClosing)
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_FILEDOWNLOAD, OnFileDownLoad)
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_NAVIGATEERROR, OnNavigateError)
	SINK_ENTRY_EX(PWSINKID_FRONTEVENTS, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, DocumentComplete)
	END_SINK_MAP()
		
	// OnBeforeNavigate2
	void _stdcall OnBeforeNavigate2( IDispatch** ppDisp, VARIANT* pvUrl, VARIANT*Flags, VARIANT* pvTarget, VARIANT*PostData, VARIANT*Headers, VARIANT_BOOL *pbCancel)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("OnBeforeNavigate2"));
		if (pvUrl->bstrVal && wcscmp(pvUrl->bstrVal, _L("javascript:void(0)")) == 0)
		{
			*pbCancel = VARIANT_TRUE;
			return;
		}
		if (m_pBrowser)
			m_pBrowser->OnBeforeNavigate2(ppDisp, pvUrl, Flags, pvTarget, PostData, Headers, pbCancel);
	}

	void _stdcall OnNavigateComplete2( IDispatch** ppDisp, VARIANT* pvUrl)
	{		
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("OnNavigateComplete2"));
		if (m_pBrowser)
			m_pBrowser->OnNavigateComplete2(ppDisp, pvUrl);
	}
	
	// OnNewWindow2
	void _stdcall OnNewWindow2( IDispatch** ppDisp, VARIANT_BOOL *pbCancel)
	{
		// new window opened
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("OnNewWindow2"));
		if (m_pBrowser)
			m_pBrowser->OnNewWindow2(ppDisp, pbCancel);
	}

	void _stdcall OnNewWindow3(IDispatch** ppDisp,VARIANT_BOOL* Cancel, unsigned long dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("OnNewWindow3"));
		if (m_pBrowser)
			m_pBrowser->OnNewWindow3(ppDisp, Cancel, dwFlags, bstrUrlContext, bstrUrl);		
	}
	void _stdcall OnWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL* Cancel)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("OnWindowClosing"));
		if (m_pBrowser)
			m_pBrowser->OnWindowClosing(IsChildWindow, Cancel);
	}
	// OnQuit
	void _stdcall OnQuit()
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("OnQuit"));
		if (m_pBrowser)
			m_pBrowser->OnQuit();
	}

	void _stdcall DocumentComplete(IDispatch* pDisp,VARIANT* URL)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("DocumentComplete"));
		if (m_pBrowser)
			m_pBrowser->DocumentComplete(pDisp, URL);	
	}
	
	void _stdcall OnNavigateError(IDispatch* pDisp,VARIANT* URL,VARIANT* Frame,VARIANT* StatusCode,VARIANT_BOOL* Cancel)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("OnNavigateError"));
		if (m_pBrowser)
			m_pBrowser->OnNavigateError(pDisp, URL, Frame, StatusCode, Cancel);
	}
	
	void _stdcall OnFileDownLoad(VARIANT_BOOL ActiveDocument, VARIANT_BOOL* Cancel)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("OnFileDownLoad"));
		if (m_pBrowser)
			m_pBrowser->OnFileDownLoad(ActiveDocument, Cancel);		
	}
};

class ATL_NO_VTABLE CPWBackIESink :
public CComObjectRootEx<CComSingleThreadModel>,//<CComMultiThreadModel>,
public IDispEventImpl<PWSINKID_BACKEVENTS, CPWBackIESink, &DIID_DWebBrowserEvents2>
{
private:
	CComPtr<IUnknown> m_pUnkIE;

public:
	CPWBrowser   *m_pBrowser;
	CPWBackIESink()  {m_pUnkIE =NULL;m_pBrowser = NULL;};
	~CPWBackIESink() {if(m_pUnkIE) m_pUnkIE.Detach();};
	HRESULT AdviseToIE( CComPtr<IUnknown> pUnkIE)
	{
		m_pUnkIE = pUnkIE;
		
		// retrieve information about the default source interface of an object
		// interface ID,LIBID and major and minor version numbers of the type library.
		AtlGetObjectSourceInterface( m_pUnkIE, &m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum) ;
		
		// connect the Dispatch interface.
		HRESULT hr = this->DispEventAdvise( m_pUnkIE);
		
		return hr ;
	}
	HRESULT UnAdviseToIE( )
	{
		if(m_pUnkIE)
		{
			HRESULT hr = this->DispEventUnadvise((IUnknown*)this );
			return hr ;
		}
		return S_OK;
		// connect the Dispatch interface.

	}
	BEGIN_COM_MAP(CPWBackIESink)
		COM_INTERFACE_ENTRY_IID(DIID_DWebBrowserEvents2, CPWBackIESink)
	END_COM_MAP()
		
	BEGIN_SINK_MAP(CPWBackIESink)
	SINK_ENTRY_EX(PWSINKID_BACKEVENTS, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
	SINK_ENTRY_EX(PWSINKID_BACKEVENTS, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
	END_SINK_MAP()
		
	// OnBeforeNavigate2
	void _stdcall OnBeforeNavigate2( IDispatch** ppDisp, VARIANT* pvUrl, VARIANT*, VARIANT* pvTarget, VARIANT*, VARIANT*, VARIANT_BOOL *pbCancel)
	{
		// test the sink event 
		if(pvUrl)
		{
			m_pBrowser->NavigateUrl(pvUrl);
		}
			*pbCancel = VARIANT_TRUE ;
		return;
		}
	// OnQuit
	void _stdcall OnQuit()
		{
		return ;
	}
};


#endif //PWBRWOSER_H




