// Filename	: DlgExplorer.h
// Creator	: Xiao Zhou
// Date		: 2009/6/10

#pragma once

#include "PWBrowser.h"
#include "DlgBase.h"
#include <AUIWindowPicture.h>
#include <WndThreadManager.h>
#include <AUIListBox.h>
#include "vector.h"

#define USE_GLOBAL_BROWSER 1 //0采用独立BROWSER，1采用全局BROWSER
class CDlgExplorer;
class CPWBrowser;
struct DOWNLOADSTATUS
{
	ULONG ulProgress;
	ULONG ulProgressMax;
	ULONG ulStatusCode;
	//LPCWSTR szStatusText;
};

struct DOWNLOADER_TASK
{
	AWString url;
	AWString localfile;
	int id;
	//char count;
	//time_t starttime;
};

struct DOWNLOADING_TASK
{
	AWString url;
	AWString localfile;
	DOWNLOADSTATUS status;
};
BOOL InitBrowser();
BOOL FreeBrowser();
#define STATUS_TXT_LEN 256
class CExplorerMan
{
public:
	CExplorerMan();
	virtual ~CExplorerMan();
	virtual bool OnInitDialog();
	BOOL CreateBrowser();
	void HideBrowser();
	void ShowBrowser();
	BOOL AddDownloadUrl(WCHAR *FileUrl);
	void ThreadRun();
	BOOL InitTaskList();
	void NavigateUrl(WCHAR *URL);
	void GoForward();
	void GoBackward();
	void Refresh();
	bool GetDownloadState(abase::vector<AWString>& vecTaskDone, abase::vector<AWString>& vecTaskToDo, AWString& curDownFile, int &curDownProgress);
	double GetDownloadRate();
	bool IsDownloading();
	void CancelCurDownLoad();
	void OnWinTimer();
	void OnTick();
	void SetDownloadStatus(ULONG ulProgress,ULONG ulProgressMax,LONG ulStatusCode,LPCWSTR szStatusText);
	BOOL GetDownloadingUrlFileSize(UINT* plength);   
protected:
	bool              m_bUpdateDownLoadState;
	bool              m_bDownLoading;
	bool              m_bUpdateDownData;
	bool              m_bInTimer;
	bool              m_bCancelCurDownLoad;
	WCHAR             m_szDownStatusText[STATUS_TXT_LEN];	
	CPWBrowser        *m_pBrowser;	
	HANDLE            m_hDownloadThread;
	DWORD             m_dwDownloadThreadId;
	abase::vector<DOWNLOADER_TASK>	m_vecTaskDone;
	abase::vector<DOWNLOADER_TASK>	m_vecTaskToDo;
	DOWNLOADING_TASK                m_currentTask;
	HANDLE           				m_mutex;
	HANDLE                          m_event;
	WCHAR                           m_wsPath[MAX_PATH];
	static unsigned long WINAPI  DownloadThread(LPVOID pDlgExplorer); 
private:
	CDlgExplorer*                   m_pDlg;
public:
	CPWBrowser* GetBrowser(){return m_pBrowser;};
	BOOL BindUI(CDlgExplorer *pDlg);
	BOOL ReleaseUI();
	CDlgExplorer * GetUI(){ return m_pDlg; }
};

class CBSCallbackImpl : public IBindStatusCallback
{
public:
	CBSCallbackImpl(CExplorerMan* pDlgExplorer, HANDLE hEventStop);

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IBindStatusCallback methods
	STDMETHOD(OnStartBinding)(DWORD, IBinding *);
	STDMETHOD(GetPriority)(LONG *);
	STDMETHOD(OnLowResource)(DWORD);
	STDMETHOD(OnProgress)(ULONG ulProgress,
						  ULONG ulProgressMax,
						  ULONG ulStatusCode,
						  LPCWSTR szStatusText);
	STDMETHOD(OnStopBinding)(HRESULT, LPCWSTR);
	STDMETHOD(GetBindInfo)(DWORD *, BINDINFO *);
	STDMETHOD(OnDataAvailable)(DWORD, DWORD, FORMATETC *, STGMEDIUM *);
	STDMETHOD(OnObjectAvailable)(REFIID, IUnknown *);

protected:
	ULONG m_ulObjRefCount;
	UINT  m_filesize;
private:
	CExplorerMan *m_pDlgExplorer;
	HANDLE m_hEventStop;

};

enum
{
	UF_BINDSTATUS_FIRST = BINDSTATUS_FINDINGRESOURCE,
	UF_BINDSTATUS_LAST = BINDSTATUS_ACCEPTRANGES
};

class CECURLOSNavigator;
class CDlgExplorer : public CDlgBase, public CPWMessageTranslate  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
		
public:
	CDlgExplorer();
	virtual ~CDlgExplorer();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release();
	void OnHideDialog();
	void OnTick();

	bool MessageTranslate(AString Control, UINT msg, WPARAM wParam, LPARAM lParam);
	BOOL PeekMessage_Filter(LPMSG lpMsg,HWND hWnd);
	BOOL AddDownloadUrl(WCHAR *FileUrl);
	BOOL InitTaskList();
	void NavigateUrl(const WCHAR *URL, CECURLOSNavigator* pOSNavigator=NULL);
	void NavigateUrl(const CHAR *Url, CECURLOSNavigator	* pOSNavigator=NULL);
	void OnWinTimer();
	BOOL FreeExplorerMan();
	CExplorerMan* GetExplorerMan(){return m_pExploreMan;};
	const PAUIWINDOWPICTURE   GetWPic() {return m_pWPic;};

	void GetCurrentUrl();
	void GoForward();
	void GoBackward();
	void Refresh();
	bool GetDownloadState(abase::vector<AWString>& vecTaskDone, abase::vector<AWString>& vecTaskToDo, AWString& curDownFile, int &curDownProgress);	
	double GetDownloadRate();
	bool IsDownloading();
	void CancelCurDownLoad();

	// event callback
	virtual void OnBeforeNavigate2(IDispatch** pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel){}
	virtual void OnNavigateComplete2(IDispatch** pDisp, VARIANT* URL){}
	virtual void OnNewWindow2(IDispatch** ppDisp, VARIANT_BOOL* Cancel){}
	virtual void OnNewWindow3(IDispatch** ppDisp, VARIANT_BOOL* Cancel, unsigned long dwFlags, BSTR bstrUrlContext, BSTR bstrUrl){}
	virtual void OnWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL* Cancel){}
	virtual void OnQuit(){}
	virtual void DocumentComplete(IDispatch* pDisp,VARIANT* URL){}
	virtual void OnNavigateError(IDispatch* pDisp,VARIANT* URL,VARIANT* Frame,VARIANT* StatusCode,VARIANT_BOOL* Cancel){}
	virtual void OnFileDownLoad(VARIANT_BOOL ActiveDocument, VARIANT_BOOL* Cancel){}

protected:
	bool              m_bInTimer;	
	PAUIWINDOWPICTURE m_pWPic;
	AUIObject *		  m_pBtn_NavigateInOS;
	//PAUILISTBOX     m_pFileList;
	//CPWBrowser      *m_pBrowser;
	CExplorerMan      *m_pExploreMan;
	CWndThreadManager m_WndThreadManager;
	CECURLOSNavigator	* m_pOSNavigator;

	void KeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);
	void KeyEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam);
	void MouseEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam);
	void MouseEventX(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam);
	void MouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);
	HWND GetCaretWindowPos(LPRECT lprect);

	void OnCommand_Prev(const char *szCommand);
	void OnCommand_Next(const char *szCommand);
	void OnCommand_Refresh(const char *szCommand);
	void OnCommand_OSNavigate(const char *szCommand);

	void ReleaseOSNavigator();
	void SetOSNavigator(CECURLOSNavigator *pOSNavigator);
	void UpdateOSNavigatorUI();
	bool CanDoOSNavigate()const;
	/*
	abase::vector<DOWNLOADER_TASK>	*m_pVecTaskDone;
	abase::vector<DOWNLOADER_TASK>	*m_pVecTaskToDo;
	DOWNLOADING_TASK                *m_pCurrentTask;
	*/
};

//extern CDlgExplorer dlgExplorer;
