// Filename	: DlgExplorer.cp
// Creator	: Xiao Zhou
// Date		: 2009/6/10

#define _WIN32_WINNT  0x0500
#include "DlgExplorer.h"
#include "EC_URLOSNavigator.h"
#include "EC_Game.h"
#include "EC_UIManager.h"
#include "EC_Global.h"
#include "htmlhelp.h"
#include "msgtransfun.h"
#include "windowsx.h"
#include "Wininet.h"
#include <urlmon.h>
//include "LuaEvent.h"
#include <AUIManager.h>


#define INRECT_PT(pt,rc) ((rc.left<=pt.x)&&(rc.right>=pt.x)&&(rc.top<=pt.y)&&(rc.bottom>=pt.y))
#define INRECT_XY(x,y,rc) ((rc.left<=x)&&(rc.right>=x)&&(rc.top<=y)&&(rc.bottom>=y))
#define TRACE 
#define WAIT_TIME 500

#pragma comment( lib, "htmlhelp.lib" )
#pragma comment( lib, "urlmon.lib")
//CDlgExplorer dlgExplorer;

//#define new A_DEBUG_NEW
static const int MAX_EXPLORER_MAN = 2;
CExplorerMan *g_pExplorerMan[MAX_EXPLORER_MAN] = {NULL};

typedef HRESULT  (WINAPI* TYP_URLDownloadToFileW)(LPUNKNOWN,LPCWSTR,LPCWSTR,DWORD,LPBINDSTATUSCALLBACK);
TYP_URLDownloadToFileW xURLDownloadToFileW = URLDownloadToFileW; //IE 7.0 的URLMON 无法获取下载文件的大小
#define AGENT_NAME "Microsoft Internet Explorer"


BOOL GetUrlFileSize(const WCHAR* Url, UINT* plength)   
{   
	HINTERNET   m_Session=::InternetOpenA(AGENT_NAME,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);   
	HINTERNET   hHttpFile=NULL;   
	char    szSizeBuffer[32]="\0";   
	DWORD   dwLengthSizeBuffer= sizeof(szSizeBuffer);   
	UINT    dwFileSize =0;   
	BOOL    bSuccessful=FALSE;   
	//Opening   the   Url   and   getting   a   Handle   for   HTTP   file   
	hHttpFile = InternetOpenUrlW(m_Session,(const WCHAR*)Url,NULL,0,0,0);   
	if(hHttpFile)   
	{
		//Getting   the   size   of   HTTP   Files   
		BOOL   bQuery  =  ::HttpQueryInfoA(hHttpFile,HTTP_QUERY_CONTENT_LENGTH,szSizeBuffer, &dwLengthSizeBuffer, NULL);   
		if(bQuery==TRUE)  
		{
			//Allocating   the   memory   space   for   HTTP   file   contents   
			dwFileSize=atol(szSizeBuffer);  
			bSuccessful = true;
		}
		::InternetCloseHandle(hHttpFile);   //   Close   the   connection.     
	}
	else   
	{   
		//Connection   failed.   
		bSuccessful = FALSE;   
	}   
	*plength = dwFileSize;   
	::InternetCloseHandle(m_Session);   
	return   bSuccessful;   
}   


BOOL InitBrowser()
{
	
	if(!xURLDownloadToFileW)
	{
		HMODULE hUrlMon= NULL;//GetModuleHandleA("urlmon.dll");
		int i=10;
		while((hUrlMon)&&(i>0))
		{
			i--;
			FreeLibrary(hUrlMon);
			hUrlMon= GetModuleHandleA("urlmon.dll");
		}
		
		if(!hUrlMon)
		{
			
			char path[512] ={0,};
			GetModuleFileNameA(NULL,path,256);
			char* name1 = strrchr(path,'\\');
			memset(name1,0,name1-path+256);
			strcpy(name1,"\\urlmon.dll");
			hUrlMon = LoadLibraryA(path);
			
			if(!hUrlMon)
			{
				hUrlMon = LoadLibraryA("urlmon.dll");
			}
		}
		xURLDownloadToFileW = (TYP_URLDownloadToFileW)GetProcAddress(hUrlMon,"URLDownloadToFileW");
	}
	
	for (int i = 0; i < MAX_EXPLORER_MAN; ++ i)
	{
		g_pExplorerMan[i] = new CExplorerMan();
		g_pExplorerMan[i]->CreateBrowser();
	}

	return TRUE;
}

BOOL FreeBrowser()
{
	for (int i = 0; i < MAX_EXPLORER_MAN; ++ i)
	{
		CExplorerMan *& pExplorerMan = g_pExplorerMan[i];
		if (pExplorerMan)
		{
			pExplorerMan->ReleaseUI();
			delete pExplorerMan;
			pExplorerMan = NULL;
		}
	}
	
	CPWBrowser::TermContainer();

	return TRUE;
}


AUI_BEGIN_COMMAND_MAP(CDlgExplorer, CDlgBase)
AUI_ON_COMMAND("Btn_Prev", OnCommand_Prev)
AUI_ON_COMMAND("Btn_Next", OnCommand_Next)
AUI_ON_COMMAND("Btn_Refresh", OnCommand_Refresh)
AUI_ON_COMMAND("Btn_NavigateInOS", OnCommand_OSNavigate)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgExplorer, CDlgBase)

AUI_END_EVENT_MAP()


CDlgExplorer::CDlgExplorer()
{
	m_bInTimer       = false;	
	m_pExploreMan    = NULL;
	m_pOSNavigator	 = NULL;
}



BOOL CDlgExplorer::InitTaskList()
{
	return TRUE;
}

void CDlgExplorer::NavigateUrl(const WCHAR* Url, CECURLOSNavigator* pOSNavigator)
{
	if (m_pExploreMan != NULL && m_pExploreMan->GetBrowser()){
		m_pExploreMan->GetBrowser()->NavigateUrl(Url);
		SetOSNavigator(pOSNavigator);
	}else{
		SetOSNavigator(NULL);
	}
}

void CDlgExplorer::NavigateUrl(const CHAR* Url, CECURLOSNavigator* pOSNavigator)
{
	if (m_pExploreMan != NULL && m_pExploreMan->GetBrowser()){
		m_pExploreMan->GetBrowser()->NavigateUrl(Url);
		SetOSNavigator(pOSNavigator);
	}else{
		SetOSNavigator(NULL);
	}
}

void CDlgExplorer::GoForward()
{
	if (NULL == m_pExploreMan)
	{
		return;
	}
	if(m_pExploreMan->GetBrowser())
	{
		m_pExploreMan->GetBrowser()->GoForward();
		SetOSNavigator(NULL);
	}	
}

void CDlgExplorer::GoBackward()
{
	if (NULL == m_pExploreMan)
	{
		return;
	}
	if(m_pExploreMan && m_pExploreMan->GetBrowser())
	{
		m_pExploreMan->GetBrowser()->GoBackward();
		SetOSNavigator(NULL);
	}	
}

void CDlgExplorer::Refresh()
{
	if (NULL == m_pExploreMan)
	{
		return;
	}
	if(m_pExploreMan->GetBrowser())
	{
		m_pExploreMan->GetBrowser()->Refresh();
	}	
}

bool CDlgExplorer::GetDownloadState(abase::vector<AWString>& vecTaskDone, abase::vector<AWString>& vecTaskToDo, AWString& curDownFile, int &curDownProgress)
{
	if (NULL != m_pExploreMan)
	{
		return m_pExploreMan->GetDownloadState(vecTaskDone, vecTaskToDo, curDownFile, curDownProgress);			
	}
	return false;
}

double CDlgExplorer::GetDownloadRate()
{
	if (NULL != m_pExploreMan)
	{
		return m_pExploreMan->GetDownloadRate();
	}
	return 0.0;
}

bool CDlgExplorer::IsDownloading()
{
	if (NULL != m_pExploreMan)
	{
		return m_pExploreMan->IsDownloading();
	}
	return false;
}

void CDlgExplorer::CancelCurDownLoad()
{
	if (NULL != m_pExploreMan)
	{
		m_pExploreMan->CancelCurDownLoad();
	}
}

void CDlgExplorer::GetCurrentUrl()
{
	if (NULL == m_pExploreMan)
	{
		return;
	}
	if(m_pExploreMan->GetBrowser())
	{
		CPWBrowser* pBrowser = m_pExploreMan->GetBrowser();
		if (NULL != pBrowser)
		{			
			pBrowser->GetCurrentUrl();
		}
	}
	return;
}

CDlgExplorer::~CDlgExplorer()
{
	//MSDN documentation says "A thread cannot use DestroyWindow to destroy a window created by a different thread.
	if (m_pExploreMan)
	{
		m_pExploreMan->HideBrowser();
		m_pExploreMan->ReleaseUI();
	}
}

bool CDlgExplorer::OnInitDialog()
{
	m_pWPic = (PAUIWINDOWPICTURE)GetDlgItem("WPic");
	m_pBtn_NavigateInOS = GetDlgItem("Btn_NavigateInOS");
	//m_pFileList = (PAUILISTBOX)GetDlgItem("FileList");
	return true;
}

void CDlgExplorer::OnHideDialog()
{
	if (NULL != m_pExploreMan)
	{
		//m_pExploreMan->ReleaseUI();
		m_pExploreMan->HideBrowser();
	}	
}

void CDlgExplorer::OnShowDialog()
{
	if(m_pExploreMan==NULL)
	{
		for (int i = 0; i < MAX_EXPLORER_MAN; ++ i)
		{
			CExplorerMan * &pExploreMan = g_pExplorerMan[i];
			if (pExploreMan && !pExploreMan->GetUI())
			{
				m_pExploreMan = pExploreMan;
				m_pExploreMan->BindUI(this);
				m_pExploreMan->CreateBrowser();
				m_pExploreMan->ShowBrowser();
				break;
			}
		}
	}
	if(m_pExploreMan!=NULL)
	{
		if (NULL != m_pExploreMan->GetBrowser())
		{
			m_pExploreMan->GetBrowser()->ShowWindow(SW_SHOW);
		}
	}
}

bool CDlgExplorer::Release(){
	ReleaseOSNavigator();
	return CDlgBase::Release();
}

void CDlgExplorer::OnCommand_Prev(const char *szCommand)
{
	GoBackward();
}

void CDlgExplorer::OnCommand_Next(const char *szCommand)
{
	GoForward();
}

void CDlgExplorer::OnCommand_Refresh(const char *szCommand)
{
	Refresh();
}

bool CDlgExplorer::MessageTranslate(AString Control, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool bval = false;
	if(Control == "WPic")
	{
		PAUIMANAGER pAUIManager = GetAUIManager();
		switch( pAUIManager->GetWindowMessageType(msg) )
		{
		case WINDOWMESSAGE_TYPE_MOUSE:
			{
				//POINT ptPos = m_pWPic->GetPos();
				//int x = GET_X_LPARAM(lParam) - m_pWPic->GetOffX() - ptPos.x;
				//int y = GET_Y_LPARAM(lParam) - m_pWPic->GetOffY() - ptPos.y;
				//lParam = int(y / m_fScale) << 16 | int(x / m_fScale);
				//MouseEvent(m_pWPic->GetHWnd(), msg, wParam, lParam);
				//keybd_event( 0, 0, 0, 0 );
				return true;
			}
			
		case WINDOWMESSAGE_TYPE_KEYBOARD:
			{
				//KeyEvent(m_pWPic->GetHWnd(), msg, wParam, lParam);
				//keybd_event( 0, 0, 0, 0 );
				return true;
			}
		}
		
	}
	return TRUE;
}
void CDlgExplorer::OnWinTimer()
{
	if(m_bInTimer)
		return;
	m_bInTimer = true;
	m_bInTimer = false;
}

void CDlgExplorer::KeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	
}
void CDlgExplorer::KeyEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	
}
void CDlgExplorer::MouseEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LONG xPos,yPos;
	RECT rc;
	xPos = (LONG)GET_X_LPARAM(lParam);
	yPos = (LONG)GET_Y_LPARAM(lParam);
	if(IsWindowVisible(hwnd))
	{
		GetWindowRect(hwnd,&rc);
		xPos+=rc.left;
		yPos+=rc.top;
		
		if(INRECT_XY(xPos,yPos,rc))
		{
			MouseEventX(hwnd, uMsg, wParam,MAKELPARAM(xPos,yPos)); //传入的是屏幕坐标
			return;
		}
	}
}

BOOL CALLBACK ThreadEnumChildMouseProcExp(HWND hwnd,LPARAM lParam)
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
			EnumChildWindows(hwnd,ThreadEnumChildMouseProcExp,lParam);
			if(!p_SLparam->flag)
			{
				//::ClientToScreen(hWnd,&w_pt);
				//::GetClientRect(hWnd,&w_crc);
				ptc.x = p_SLparam->pt.x - pt.x;
				ptc.y = p_SLparam->pt.y - pt.y;
				if(p_SLparam->message!=WM_MOUSEMOVE)
					::PostMessage(hwnd,WM_ACTIVATE,WA_CLICKACTIVE,0);
				//SendMessage();
				if(INRECT_PT(ptc,rc1)) //在ClientRect 内
				{
					if(p_SLparam->message !=WM_MOUSEMOVE)
						PostMessage(hwnd,WM_MOUSEMOVE,0,MAKELPARAM(ptc.x,ptc.y));
					if(p_SLparam->message ==WM_LBUTTONUP)
					{
						p_SLparam->message =WM_LBUTTONDOWN;
						PostMessage(hwnd,p_SLparam->message,p_SLparam->wParam,MAKELPARAM(ptc.x,ptc.y));
						p_SLparam->message =WM_LBUTTONUP;
						PostMessage(hwnd,p_SLparam->message,p_SLparam->wParam,MAKELPARAM(ptc.x,ptc.y));
					}
					else
					{
						res= PostMessage(hwnd,p_SLparam->message,p_SLparam->wParam,MAKELPARAM(ptc.x,ptc.y));
					}
					
				}
				else //不在ClientRect内
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
					PostMessage(hwnd,p_SLparam->message+WM_NCMOUSEMOVE-WM_MOUSEMOVE,res,MAKELPARAM(p_SLparam->pt.x,p_SLparam->pt.y));
				}
				
			}
			p_SLparam->flag = TRUE;
		}
	}
	return TRUE;
}

void CDlgExplorer::MouseEventX(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LONG xPos,yPos,sxpos,sypos,nxpos,nypos;
	LRESULT res;
	nxpos= sxpos =xPos = (LONG)GET_X_LPARAM(lParam); //
	nypos= sypos =yPos = (LONG)GET_Y_LPARAM(lParam); //	
	
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
	
	SLparam.pt.x = sxpos;
	SLparam.pt.y = sypos;
	if((!IsIconic(hwnd)))
		EnumChildWindows(hwnd,ThreadEnumChildMouseProcExp,(LPARAM)&SLparam);
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
				if(!IsZoomed(hwnd))
					SendMessage(hwnd,WM_SYSCOMMAND,SC_MAXIMIZE,MAKELPARAM(sxpos,sypos));
				else
					SendMessage(hwnd,WM_SYSCOMMAND,SC_RESTORE,MAKELPARAM(sxpos,sypos));
				return;
			}
			if((uMsg == WM_LBUTTONDOWN)&&(res == HTMINBUTTON))
			{
				if(!IsIconic(hwnd))
					SendMessage(hwnd,WM_SYSCOMMAND,SC_MINIMIZE,MAKELPARAM(sxpos,sypos));
				else
				{
					SendMessage(hwnd,WM_SYSCOMMAND,SC_RESTORE,MAKELPARAM(sxpos,sypos));
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
}


HWND CDlgExplorer::GetCaretWindowPos(LPRECT lprect)
{
	return NULL;
}

BOOL CDlgExplorer::PeekMessage_Filter(LPMSG lpMsg,HWND hWnd)
{
	return TRUE;
}

void CDlgExplorer::OnTick()
{
	if (NULL != m_pExploreMan)
	{
		if (m_pExploreMan->GetBrowser())
		{
			m_pExploreMan->GetBrowser()->OnTick();
		}
	}
	UpdateOSNavigatorUI();
}

BOOL CDlgExplorer::AddDownloadUrl(WCHAR *FileUrl)
{
	return TRUE;
}

BOOL CDlgExplorer::FreeExplorerMan()
{
	m_pExploreMan = NULL;
	return TRUE;
}

void CDlgExplorer::ReleaseOSNavigator(){	
	if (m_pOSNavigator && m_pOSNavigator->AutoRelease()){
		delete m_pOSNavigator;
	}
	m_pOSNavigator = NULL;
}

void CDlgExplorer::SetOSNavigator(CECURLOSNavigator *pOSNavigator){
	if (m_pOSNavigator == pOSNavigator){
		return;
	}
	if (!m_pBtn_NavigateInOS){
		return;
	}
	ReleaseOSNavigator();
	m_pOSNavigator = pOSNavigator;
	UpdateOSNavigatorUI();
}

bool CDlgExplorer::CanDoOSNavigate()const{
	return m_pOSNavigator && m_pOSNavigator->CanNavigateNow();
}

void CDlgExplorer::OnCommand_OSNavigate(const char *szCommand){
	if (!CanDoOSNavigate()){
		return;
	}
	m_pOSNavigator->OnClickNavigate();
	UpdateOSNavigatorUI();
}

void CDlgExplorer::UpdateOSNavigatorUI(){
	if (!m_pBtn_NavigateInOS){
		return;
	}
	if (!m_pOSNavigator){
		if (m_pBtn_NavigateInOS->IsShow()){
			m_pBtn_NavigateInOS->Show(false);
		}
		return;
	}
	if (!m_pBtn_NavigateInOS->IsShow()){
		m_pBtn_NavigateInOS->Show(true);
	}
	m_pBtn_NavigateInOS->Enable(m_pOSNavigator->CanNavigateNow());
}

//-----------------------------------------------------------------------------
//*****************************************************************************


//*****************************************************************************
//-----------------------------------------------------------------------------
CBSCallbackImpl::CBSCallbackImpl(CExplorerMan* pDlgExplorer, HANDLE hEventStop)
{
	
	m_pDlgExplorer = pDlgExplorer;  // the window handle to display status
	
	m_hEventStop = hEventStop;  // the event object to signal to stop
	
	m_ulObjRefCount = 1;
	
	m_filesize = 0;
}
//-----------------------------------------------------------------------------
// IUnknown
STDMETHODIMP CBSCallbackImpl::QueryInterface(REFIID riid, void **ppvObject)
{
	TRACE(_T("IUnknown::QueryInterface\n"));
	
	*ppvObject = NULL;
	
	// IUnknown
	if (::IsEqualIID(riid, __uuidof(IUnknown)))
	{
		TRACE(_T("IUnknown::QueryInterface(IUnknown)\n"));
		
		*ppvObject = this;
	}
	// IBindStatusCallback
	else if (::IsEqualIID(riid, __uuidof(IBindStatusCallback)))
	{
		TRACE(_T("IUnknown::QueryInterface(IBindStatusCallback)\n"));
		
		*ppvObject = static_cast<IBindStatusCallback *>(this);
	}
	
	if (*ppvObject)
	{
		(*reinterpret_cast<LPUNKNOWN *>(ppvObject))->AddRef();
		
		return S_OK;
	}
	
	return E_NOINTERFACE;
}                                             
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CBSCallbackImpl::AddRef()
{
	TRACE(_T("IUnknown::AddRef\n"));
	
	return ++m_ulObjRefCount;
}
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CBSCallbackImpl::Release()
{
	TRACE(_T("IUnknown::Release\n"));
	
	return --m_ulObjRefCount;
}
//-----------------------------------------------------------------------------
// IBindStatusCallback
STDMETHODIMP CBSCallbackImpl::OnStartBinding(DWORD, IBinding *)
{
	TRACE(_T("IBindStatusCallback::OnStartBinding\n"));
	
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::GetPriority(LONG *)
{
	TRACE(_T("IBindStatusCallback::GetPriority\n"));
	
	return E_NOTIMPL;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnLowResource(DWORD)
{
	TRACE(_T("IBindStatusCallback::OnLowResource\n"));
	
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnProgress(ULONG ulProgress,
										 ULONG ulProgressMax,
										 ULONG ulStatusCode,
										 LPCWSTR szStatusText)
{
#ifdef _DEBUG
	static const LPCTSTR plpszStatus[] = 
	{
	   	    _T("BINDSTATUS_FINDINGRESOURCE"),  // 1
			_T("BINDSTATUS_CONNECTING"),
			_T("BINDSTATUS_REDIRECTING"),
			_T("BINDSTATUS_BEGINDOWNLOADDATA"),
			_T("BINDSTATUS_DOWNLOADINGDATA"),
			_T("BINDSTATUS_ENDDOWNLOADDATA"),
			_T("BINDSTATUS_BEGINDOWNLOADCOMPONENTS"),
			_T("BINDSTATUS_INSTALLINGCOMPONENTS"),
			_T("BINDSTATUS_ENDDOWNLOADCOMPONENTS"),
			_T("BINDSTATUS_USINGCACHEDCOPY"),
			_T("BINDSTATUS_SENDINGREQUEST"),
			_T("BINDSTATUS_CLASSIDAVAILABLE"),
			_T("BINDSTATUS_MIMETYPEAVAILABLE"),
			_T("BINDSTATUS_CACHEFILENAMEAVAILABLE"),
			_T("BINDSTATUS_BEGINSYNCOPERATION"),
			_T("BINDSTATUS_ENDSYNCOPERATION"),
			_T("BINDSTATUS_BEGINUPLOADDATA"),
			_T("BINDSTATUS_UPLOADINGDATA"),
			_T("BINDSTATUS_ENDUPLOADINGDATA"),
			_T("BINDSTATUS_PROTOCOLCLASSID"),
			_T("BINDSTATUS_ENCODING"),
			_T("BINDSTATUS_VERFIEDMIMETYPEAVAILABLE"),
			_T("BINDSTATUS_CLASSINSTALLLOCATION"),
			_T("BINDSTATUS_DECODING"),
			_T("BINDSTATUS_LOADINGMIMEHANDLER"),
			_T("BINDSTATUS_CONTENTDISPOSITIONATTACH"),
			_T("BINDSTATUS_FILTERREPORTMIMETYPE"),
			_T("BINDSTATUS_CLSIDCANINSTANTIATE"),
			_T("BINDSTATUS_IUNKNOWNAVAILABLE"),
			_T("BINDSTATUS_DIRECTBIND"),
			_T("BINDSTATUS_RAWMIMETYPE"),
			_T("BINDSTATUS_PROXYDETECTING"),
			_T("BINDSTATUS_ACCEPTRANGES"),
			_T("???")  // unknown
	};
#endif
	
	TRACE(_T("IBindStatusCallback::OnProgress\n"));
	
	TRACE(_T("ulProgress: %lu, ulProgressMax: %lu\n"),  ulProgress, ulProgressMax);
	
	TRACE(_T("ulStatusCode: %lu "), ulStatusCode);
	
	if(ulStatusCode == BINDSTATUS_CONNECTING)
	{
		m_filesize = 0;
	}
	if (ulStatusCode < UF_BINDSTATUS_FIRST ||
		ulStatusCode > UF_BINDSTATUS_LAST)
	{
		ulStatusCode = UF_BINDSTATUS_LAST + 1;
	}
	if(((ulStatusCode ==BINDSTATUS_DOWNLOADINGDATA)||(ulStatusCode ==BINDSTATUS_CACHEFILENAMEAVAILABLE))&&(m_filesize ==0))
	{
		if(ulProgress == ulProgressMax)
		{
			if(!m_pDlgExplorer->GetDownloadingUrlFileSize(&m_filesize))
				m_filesize = 1;
		}
	}
#ifdef _DEBUG
	TRACE(_T("(%s), szStatusText: %ls\n"),
		plpszStatus[ulStatusCode - UF_BINDSTATUS_FIRST],
		szStatusText);
#endif
	
	if (m_pDlgExplorer != NULL)
	{
		// inform the dialog box to display current status,
		// don't use PostMessage
		//DOWNLOADSTATUS downloadStatus = { ulProgress, ulProgressMax, ulStatusCode, szStatusText };
		if(ulProgress == ulProgressMax)
		{
			if((m_filesize>ulProgressMax)&&(m_filesize>1))
			{
				ulProgressMax = m_filesize;
			}
		}
		m_pDlgExplorer->SetDownloadStatus(ulProgress, ulProgressMax, ulStatusCode, szStatusText );
		//::SendMessage(m_hWnd, WM_USER_DISPLAYSTATUS, 0, reinterpret_cast<LPARAM>(&downloadStatus));
	}
	
	if (m_hEventStop != NULL)
	{
		if (::WaitForSingleObject(m_hEventStop, 0) == WAIT_OBJECT_0)
		{
			TRACE(_T("Canceled.\n"));
			//::SendMessage(m_hWnd, WM_USER_DISPLAYSTATUS, 0, NULL);
			return E_ABORT;  // canceled by the user
		}
	}
	
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnStopBinding(HRESULT, LPCWSTR)
{
	TRACE(_T("IBindStatusCallback::OnStopBinding\n"));
	
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::GetBindInfo(DWORD *, BINDINFO *)
{
	TRACE(_T("IBindStatusCallback::GetBindInfo\n"));
	
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnDataAvailable(DWORD, DWORD,
											  FORMATETC *, STGMEDIUM *)
{
	TRACE(_T("IBindStatusCallback::OnDataAvailable\n"));
	
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnObjectAvailable(REFIID, IUnknown *)
{
	TRACE(_T("IBindStatusCallback::OnObjectAvailable\n"));
	
	return S_OK;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ExplorerMan
//-----------------------------------------------------------------------------
CExplorerMan::CExplorerMan()
{
	m_bDownLoading         = false;
	m_bUpdateDownData      = false;
	m_bUpdateDownLoadState = false;
	m_bCancelCurDownLoad   = false;
	
	m_pDlg = NULL;
	m_bInTimer = false;
	m_pBrowser = NULL;
	m_hDownloadThread = NULL;	
	m_dwDownloadThreadId = 0;	
	
	m_mutex = CreateMutex(NULL,FALSE,NULL);
	m_event = CreateEvent(NULL,FALSE,FALSE,NULL);
	OnInitDialog();
	InitTaskList();
	
}

CExplorerMan::~CExplorerMan()
{
	
	if(m_hDownloadThread)
	{
		// (m_hDownloadThread);
		// 等待线程退出
		SetEvent(m_event);
		WaitForSingleObject(m_hDownloadThread,INFINITE);
	}
	CloseHandle(m_mutex);
	CloseHandle(m_event);
	
	//MSDN documentation says "A thread cannot use DestroyWindow to destroy a window created by a different thread.
	if(m_pBrowser!=NULL)
	{
		if(m_pBrowser->CanDelete(GetCurrentThreadId()))
		{
			delete m_pBrowser;
		}
		else
		{
			m_pBrowser->DelayDestroy();
			m_pBrowser = NULL;
		}
	}
	
}

BOOL CExplorerMan::InitTaskList()
{
	m_vecTaskDone.clear();
	m_vecTaskToDo.clear();
	return TRUE;
}

void CExplorerMan::NavigateUrl(WCHAR* Url)
{
	if(m_pBrowser)
		m_pBrowser->NavigateUrl(Url);
}

void CExplorerMan::GoForward()
{
	if(m_pBrowser)
		m_pBrowser->GoForward();	
}
void CExplorerMan::GoBackward()
{
	if(m_pBrowser)
		m_pBrowser->GoBackward();	
}
void CExplorerMan::Refresh()
{
	if(m_pBrowser)
		m_pBrowser->Refresh();	
}

bool CExplorerMan::GetDownloadState(abase::vector<AWString>& vecTaskDone, abase::vector<AWString>& vecTaskToDo, AWString& curDownFile, int &curDownProgress)
{
	DWORD res = WaitForSingleObject(m_mutex, IGNORE);
	if ((res== WAIT_OBJECT_0)||(res==WAIT_ABANDONED))
	{
		int index = 0;
		AWString str;
		abase::vector<DOWNLOADER_TASK>::const_iterator iter = m_vecTaskDone.begin();
		for (; iter != m_vecTaskDone.end(); ++iter)
		{
			index = (*iter).localfile.ReverseFind(L'/');
			if (index >= 0)
			{
				str = (*iter).localfile.Right((*iter).localfile.GetLength() - index - 1);
				vecTaskDone.push_back(str);
			}				
		}
		for (iter = m_vecTaskToDo.begin(); iter != m_vecTaskToDo.end(); ++iter)
		{
			index = (*iter).localfile.ReverseFind(L'\\');
			if (index >= 0)
			{
				str = (*iter).localfile.Right((*iter).localfile.GetLength() - index - 1);
				vecTaskToDo.push_back(str);
			}
		}
		
		index = m_currentTask.localfile.ReverseFind(L'\\');
		if (index >= 0)
		{
			curDownFile = m_currentTask.localfile.Right(m_currentTask.localfile.GetLength() - index - 1);
		}
		curDownProgress = (m_currentTask.status.ulProgressMax > 0.001)?m_currentTask.status.ulProgress/m_currentTask.status.ulProgressMax:0;
		
		ReleaseMutex(m_mutex);
		return true;
	}
	return false;
}

double CExplorerMan::GetDownloadRate()
{	
	double rate = 0.0;
	if (m_currentTask.status.ulProgressMax > 0)
	{
		rate = (double)m_currentTask.status.ulProgress / (double)m_currentTask.status.ulProgressMax;
	}
	return rate;
}

bool CExplorerMan::IsDownloading()
{
	return m_bDownLoading;
}

void CExplorerMan::CancelCurDownLoad()
{
	m_bCancelCurDownLoad = true;
	::SetEvent(m_event);
}


bool CExplorerMan::OnInitDialog()
{
	memset(m_wsPath,0,MAX_PATH*sizeof(WCHAR));
	GetCurrentDirectory(MAX_PATH,m_wsPath);
	if(m_wsPath[wcslen(m_wsPath)-1]!=_T('\\'))
	{
		wcscat(m_wsPath,_T("\\MyMusics\\"));
	}
	else
	{
		wcscat(m_wsPath,_T("MyMusics\\"));
	}
	return true;
}

void CExplorerMan::HideBrowser()
{
	if (m_pBrowser)
		m_pBrowser->ShowWindow(SW_HIDE);
}
void CExplorerMan::ShowBrowser()
{
	if (m_pBrowser)
		m_pBrowser->ShowWindow(SW_SHOW);
}
BOOL CExplorerMan::CreateBrowser()
{
	
	if(m_pBrowser==NULL)
	{
		//创建浏览器与程序UI同一线程（有问题，假如浏览器窗口死掉，造成程序死掉）
		if(m_pDlg)
		{
			if(GetWindowThreadProcessId(g_pGame->GetGameInit().hWnd,NULL) ==  GetCurrentThreadId())
			{
				m_pBrowser = new CPWBrowser(this);
				PAUIWINDOWPICTURE m_pWPic = m_pDlg->GetWPic();
				RECT rc;
				SIZE sz = m_pWPic->GetSize();
				POINT pt =m_pWPic->GetPos();
				POINT ptc ={0,};
				RECT rc1;
				GetClientRect(g_pGame->GetGameInit().hWnd,&rc1);
				ClientToScreen(g_pGame->GetGameInit().hWnd,&ptc);
				rc.top =pt.y +rc1.top+ptc.y;
				rc.bottom =pt.y+sz.cy +rc1.top+ptc.y;
				rc.left = pt.x +rc1.left+ptc.x;
				rc.right = pt.x+sz.cx +rc1.left+ptc.x;
				
				if(m_pBrowser->Create(g_pGame->GetGameInit().hWnd,rc)!=NULL)
				{
					m_pWPic->SetHWnd(m_pBrowser->m_hWnd);
					m_pWPic->SetMessageHandler(m_pDlg);
				}	
				
			}
			else
			{
				return FALSE;
			}
			//DWORD tid = GetCurrentThreadId();
		}
		else
		{
			DWORD tid1 = GetCurrentThreadId();
			DWORD tid2 = GetWindowThreadProcessId(g_pGame->GetGameInit().hWnd,NULL);
			if(tid1 ==  tid2)
			{
				m_pBrowser = new CPWBrowser(this);
				RECT rc;
				rc.left = -100;
				rc.right = 0;
				rc.bottom = -100;
				rc.top =0;
				m_pBrowser->Create(g_pGame->GetGameInit().hWnd,rc);
				m_pBrowser->ShowWindow(SW_HIDE);
			}
			//return FALSE;
		}
	}
	else
	{
		if(m_pDlg)
		{
			PAUIWINDOWPICTURE m_pWPic = m_pDlg->GetWPic();
			m_pWPic->SetHWnd(m_pBrowser->m_hWnd);
			m_pWPic->SetMessageHandler(m_pDlg);
		}
	}
	if(m_pBrowser)
	{	
		if(m_pBrowser->IsWindow())
		{
			//		m_pBrowser->ShowWindow(SW_SHOWNORMAL);
			//		RECT rc;
			//		m_pBrowser->GetWindowRect(&rc);
			//		rc.top -=rc.bottom;
			//		rc.bottom =0;
			// 		rc.left -= rc.right;
			// 		rc.right =0;
			//  	POINT pt =m_pWPic->GetPos();
			//  	rc.top +=100;
			//  	rc.bottom +=100;
			// 		rc.left +=100;
			// 		rc.right+=100;
			//      m_pBrowser->SetWindowPos(HWND_BOTTOM,&rc,SWP_NOSIZE|SWP_NOMOVE);
			//		m_pBrowser->MoveWindow(&rc,FALSE);	
		}
	}
	return TRUE;
}

void CExplorerMan::OnWinTimer()
{
	if(m_bInTimer)
		return;
	m_bInTimer = true;
	
	if((m_pDlg))
	{
		if(m_pDlg->IsActive())
		{
			PAUIWINDOWPICTURE m_pWPic = m_pDlg->GetWPic();
			RECT rc;
			SIZE sz = m_pWPic->GetSize();
			POINT pt =m_pWPic->GetPos();

			//	宽屏处理
			A3DRECT AUIViewport = m_pDlg->GetAUIManager()->GetRect();
			pt.x += AUIViewport.left;
			pt.y += AUIViewport.top;

			POINT ptc ={0,};
			RECT rc1;
			GetClientRect(g_pGame->GetGameInit().hWnd,&rc1);
			ClientToScreen(g_pGame->GetGameInit().hWnd,&ptc);
		
			rc.top =pt.y +rc1.top+ptc.y;
			rc.bottom =pt.y+sz.cy +rc1.top+ptc.y;
			rc.left = pt.x +rc1.left+ptc.x;
			rc.right = pt.x+sz.cx +rc1.left+ptc.x;

			//
			if(m_pBrowser->IsReady())
			{
				if(IsWindow(m_pBrowser->m_hWnd))
				{
					//m_pBrowser->MoveWindow(&rc,TRUE);	
					m_pBrowser->SetWindowPos(HWND_TOP,&rc,SWP_ASYNCWINDOWPOS|SWP_NOACTIVATE);
					m_pBrowser->SetActive(true);
				}
			}
			//SetActiveWindow(m_pBrowser->m_hWnd);	
			if(m_pWPic->IsFocus())
			{
				m_pBrowser->ShowWindow(SW_SHOW);
				if(GetActiveWindow()==g_pGame->GetGameInit().hWnd)
				SetActiveWindow(m_pBrowser->m_hWnd);
			}
		}
		else
		{
			if(m_pDlg->IsShow())
			{			
				PAUIWINDOWPICTURE m_pWPic = m_pDlg->GetWPic();
				RECT rc;
				SIZE sz = m_pWPic->GetSize();
				POINT pt =m_pWPic->GetPos();
				
				//	宽屏处理
				A3DRECT AUIViewport = m_pDlg->GetAUIManager()->GetRect();
				pt.x += AUIViewport.left;
				pt.y += AUIViewport.top;
				
				POINT ptc ={0,};
				RECT rc1;
				GetClientRect(g_pGame->GetGameInit().hWnd,&rc1);
				ClientToScreen(g_pGame->GetGameInit().hWnd,&ptc);
				rc.top =pt.y +rc1.top+ptc.y;
				rc.bottom =pt.y+sz.cy +rc1.top+ptc.y;
				rc.left = pt.x +rc1.left+ptc.x;
				rc.right = pt.x+sz.cx +rc1.left+ptc.x;
				//
				if(m_pBrowser->IsReady())
				{
					if(IsWindow(m_pBrowser->m_hWnd))
					{
						//m_pBrowser->MoveWindow(&rc,TRUE);
						m_pBrowser->ShowWindow(SW_SHOW);
						m_pBrowser->SetWindowPos(HWND_TOP,&rc,SWP_ASYNCWINDOWPOS|SWP_NOACTIVATE);
						m_pBrowser->SetActive(false);
					}
				}
			}
			else
			{
			m_pBrowser->ShowWindow(SW_HIDE);
		}

		}
	}
	else
	{
		m_pBrowser->ShowWindow(SW_HIDE);
	
	}
	m_bInTimer = false;
	
}
BOOL CExplorerMan::BindUI(CDlgExplorer *pDlg)
{
	if(!ReleaseUI())
		return FALSE;
	m_pDlg = pDlg;
	return TRUE;
}
BOOL CExplorerMan::ReleaseUI()
{
	if(m_pDlg==NULL)
		return TRUE;
	assert(m_pDlg->GetExplorerMan()==this);
	if(m_pDlg->FreeExplorerMan())
	{
		m_pDlg = NULL;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CExplorerMan::AddDownloadUrl(WCHAR *FileUrl)
{
	DWORD Res =0; 
	Res = WaitForSingleObject(m_mutex,WAIT_TIME);	
	DOWNLOADER_TASK task;
	switch (Res) 
	{
	case WAIT_FAILED:
		//提示错误发生，无法添加任务
		break;
	case WAIT_TIMEOUT:
		//提示阻塞，无法添加任务
		break;
	case WAIT_OBJECT_0:
	case WAIT_ABANDONED:
		WCHAR *name;
		name = wcsrchr(FileUrl,_T('/'));
		if(name==NULL)
		{
			
		}
		else
		{
			name ++;
			task.url = FileUrl;
			task.localfile = AWString(m_wsPath);
			task.localfile+=name;
			if(m_vecTaskToDo.size() > 0)
			{
				m_vecTaskToDo.insert(m_vecTaskToDo.end(), task);
			}
			else
			{
				m_vecTaskToDo.insert(m_vecTaskToDo.begin(), task);
			}
			m_bUpdateDownData = true;
			
			DWORD index;
			index = m_vecTaskToDo.size()-1;
			if((m_hDownloadThread==NULL)&&(m_dwDownloadThreadId==0))
			{
				m_hDownloadThread = CreateThread(NULL,0,CExplorerMan::DownloadThread,(LPVOID)this,0,&m_dwDownloadThreadId);
			}
		}
		ReleaseMutex(m_mutex);
		break;
	default:
		break;
	}
	return TRUE;
}

unsigned long WINAPI  CExplorerMan::DownloadThread(LPVOID pDlgExplorer)
{
	CExplorerMan *hDlgExplorer = (CExplorerMan *)pDlgExplorer;
	int iRet = -1;
	__try
	{
		hDlgExplorer->ThreadRun();
		iRet = 0;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hDlgExplorer->m_hDownloadThread = NULL;
		hDlgExplorer->m_dwDownloadThreadId =0;
	}
	return iRet;
}

void CExplorerMan::ThreadRun()
{
	for(;;)
	{
		DWORD Res = WaitForSingleObject(m_mutex,INFINITE);
		switch(Res)
		{
		case WAIT_OBJECT_0:		
			if(m_vecTaskToDo.size()>0)
			{
				
				DOWNLOADER_TASK *ptask = m_vecTaskToDo.begin();
				DOWNLOADING_TASK task;
				task.url = ptask->url;
				task.localfile = ptask->localfile;
				m_currentTask = task;
				//如果有下载列表，就开始下载
				//Sleep(1000);
				CBSCallbackImpl bsc(this, m_event);				
				m_vecTaskToDo.erase(m_vecTaskToDo.begin());								
				ReleaseMutex(m_mutex);

				m_bDownLoading         = true;
				m_bUpdateDownLoadState = true;

				const HRESULT hr = xURLDownloadToFileW(NULL,
					task.url,(LPWSTR)((LPCWSTR)task.localfile),
					0,
					&bsc);
				if(hr == E_ABORT)
				{
					ReleaseMutex(m_mutex);
					if (!m_bCancelCurDownLoad)
					{
						return;
					}					
				}
				m_bDownLoading         = false;
				m_bUpdateDownLoadState = true;

				if (!m_bCancelCurDownLoad)
				{
					if(m_vecTaskDone.size()>0)
					{
						DOWNLOADER_TASK taskx;
						taskx.url = task.url;
						taskx.localfile = task.url;
						taskx.id = 0;
						m_vecTaskDone.insert(m_vecTaskDone.end(), taskx);
					}
					else
					{
						DOWNLOADER_TASK taskx;
						taskx.url = task.url;
						taskx.localfile = task.url;
						taskx.id = m_vecTaskDone.size();
						m_vecTaskDone.insert(m_vecTaskDone.begin(), taskx);
					}
				}
				else
				{
					m_bCancelCurDownLoad = false;
					_wremove(task.localfile.GetBuffer(0));
				}
				
				m_bUpdateDownData = true;
			}
			else
			{
				m_dwDownloadThreadId =0;
				m_hDownloadThread =NULL;
				ReleaseMutex(m_mutex);
				return;
				//ExitThread(0);
			}		
			break;
		case WAIT_ABANDONED:
			m_dwDownloadThreadId =0;
			m_hDownloadThread =NULL;
			ReleaseMutex(m_mutex);
			ExitThread(-1); //不明错误
			break;
		case  WAIT_FAILED:
			m_dwDownloadThreadId =0;
			m_hDownloadThread =NULL;
			ExitThread(-1); //不明错误
			break;
		}
	}
}


void CExplorerMan::SetDownloadStatus(ULONG ulProgress,ULONG ulProgressMax,LONG ulStatusCode,LPCWSTR szStatusText)
{
	m_currentTask.status.ulProgress = ulProgress;
	m_currentTask.status.ulProgressMax = ulProgressMax;
	m_currentTask.status.ulStatusCode = ulStatusCode;
	memset(m_szDownStatusText,0,sizeof(WCHAR)*STATUS_TXT_LEN);
	if(szStatusText!=NULL)
	{		
		if(wcslen(szStatusText)<STATUS_TXT_LEN-1)
		{
			wcscpy(m_szDownStatusText,szStatusText);
		}
		else
		{
			memcpy(m_szDownStatusText,szStatusText,sizeof(WCHAR)*(STATUS_TXT_LEN-1));
		}
	}	
}
BOOL CExplorerMan::GetDownloadingUrlFileSize(UINT* plength)
{
	return GetUrlFileSize(m_currentTask.url,plength);
}

void CExplorerMan::OnTick()
{
	if (m_bUpdateDownData)
	{
		m_bUpdateDownData = false;
		//LuaEvent::UpdateExplorerDownloadStatus();
	}	
	if (m_bUpdateDownLoadState)
	{
		m_bUpdateDownLoadState = false;
		//LuaEvent::UpdateDownLoadStatus();
	}	
}
