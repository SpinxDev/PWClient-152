// Filename	: AUIManager.h
// Creator	: Tom Zhou
// Date		: May 12, 2004
// Desc		: AUIManager is a manager of all AUI controls. We need to derive it
//			  to fit our specified game requirement.

#ifndef _AUIMANAGER_H_
#define _AUIMANAGER_H_

#include "AList2.h"
#include "hashmap.h"
#include "AAssist.h"
#include <ARect.h>
#include <A3DTypes.h>
#include "AUITextArea.h"

#define MB_NULL						0xFFFFFFFF
#define AUIMANAGER_WRONGDIALOGID	(-1)
#define AUIMANAGER_MAX_EMOTIONGROUPS	32
#define AUIMANAGER_DEFAULTWIDTH		1024
#define AUIMANAGER_DEFAULTHEIGHT	768

class A3DFTFontMan;
typedef class AUIFrame * PAUIFRAME;
typedef class AUIObject * PAUIOBJECT;
typedef class AUIDialog * PAUIDIALOG;
class A3DFTFont;
class A3DEngine;
class A3DDevice;
class CWndThreadManager;
class A2DDotLineMan;
class A2DDotLine;

enum
{
	WINDOWMESSAGE_TYPE_MOUSE,
	WINDOWMESSAGE_TYPE_KEYBOARD,
	WINDOWMESSAGE_TYPE_OTHER,
};

struct FONT_PARAM
{
	FONT_PARAM()
	{
		szFontName = _AL("方正细黑一简体");
		nFontSize = 10;
		bBold = false;
		bItalic = false;
		bFreeType = true;
		nShadow = 0;
		nOutline = 0;
		clOutline=0;
	}

	ACString szFontName;
	int nFontSize;
	bool bBold;
	bool bItalic;
	bool bFreeType;
	int nShadow;
	int nOutline;
	A3DCOLOR clOutline;
};

namespace AudioEngine
{
	class EventSystem;
	class EventInstance;
}

class AUIManager
{
	friend class AUIDialog;

public:
	typedef void (* LPLOADINGCALLBACK)();

	AUIManager();
	virtual ~AUIManager();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, 
		const char *pszFilename = NULL, int nDefaultWidth = AUIMANAGER_DEFAULTWIDTH, int nDefaultHeight = AUIMANAGER_DEFAULTHEIGHT);
	virtual bool Release(void);
	virtual bool Tick(DWORD dwDeltaTime);
	virtual bool Render(void);
	virtual bool OnCommand(const char *pszCommand, PAUIDIALOG pDlg);
	virtual bool OnMessageBox(int nRetVal, PAUIDIALOG pDlg);
	virtual bool DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj);
	virtual void RearrangeWindows(A3DRECT rcOld, A3DRECT rcNew);
	virtual void ResizeWindows(A3DRECT rcOld, A3DRECT rcNew);
	virtual int CreateDlg(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszTempName);
	virtual bool LoadDialogLayout(const char *pszFilename);
	virtual PAUIDIALOG GetDialog(int idDialog);
	virtual PAUIDIALOG GetDialog(const char *pszName);
	virtual void SetName(const char* pszName);
	virtual bool DestroyDialog(const char *pszName);

	void SetLoadingCallback(LPLOADINGCALLBACK pCallback) { m_pLoadingCallBack = pCallback; }
	virtual bool Save(const char *pszFilename);
	inline void SetHWnd(HWND hWnd) { m_hWnd = hWnd; }
	inline HWND GetHWnd() { return m_hWnd; }
	int CreateEmptyDlg(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszTempName = NULL);
	bool DestroyAllDialog(void);
	bool DestroyDialog(int idDialog);
	int GetDialogCount();
	inline PAUIDIALOG GetActiveDialog(void) const { return m_pDlgActive; }
	void ClearActiveDialog();
	inline PAUIDIALOG GetHintDialog(void) const { return m_pDlgHint; }
	void ShowAllDialogs(bool bShow);
	void BringWindowToTop(PAUIDIALOG pDlg);
	void InsertDialogAfter(PAUIDIALOG pDlg, PAUIDIALOG pDlgInsertAfter = NULL);
	void InsertDialogBefore(PAUIDIALOG pDlg, PAUIDIALOG pDlgInsertBefore = NULL, PAUIDIALOG pTopSubDialog = NULL);
	bool ShowDialog(int idDialog, bool bShow, bool bModal = false);
	bool HitTest(int x, int y, PAUIDIALOG *ppDlg,
		PAUIOBJECT *ppObj, PAUIDIALOG pDlgExclude = NULL);
	virtual bool MessageBox(const char *pszName, const ACHAR *pszText,
		DWORD dwType, A3DCOLOR color, PAUIDIALOG *ppDlg = NULL, abase::vector<A2DSprite*> *pvecImageList = NULL);
	inline A3DFTFont * GetDefaultFont() { return m_pFont; }
	const ACHAR * GetStringFromTable(int idString);
	inline A3DFTFontMan * GetA3DFTFontMan() { return m_pFontMan; }
	virtual A3DFTFont* GetPateFont();
	static void SetHideIme(bool bHide);
	void GetMouseOn(PAUIDIALOG *ppDlg, PAUIOBJECT *ppObj);
	void SetHint(const ACHAR* szHint);
	const ACHAR* GetHint();
	void SetObjHint(PAUIOBJECT pObj);
	void SetUseCover(bool bUse);
	int GetWindowMessageType(UINT uMsg);
	CWndThreadManager* GetWndThreadManager();
	bool ScoutWindow(HWND hWnd);
	void SaveAllRenderTarget();
	void SetWindowScale(float fWindowScale);
		
	void RespawnMessage() { m_bRespawnMessage = true; }
	float GetWindowScale() { return m_fWindowScale; }
	const ACHAR* GetDefaultFontName()	{ return m_strDefaultFontName; }
	int GetDefaultFontSize()			{ return m_nDefaultFontSize; }
	A3DRECT GetRect()					{ return m_rcWindow; }
	DWORD GetTickTime()					{ return m_dwDeltaTime; }
	A2DSprite** GetDefaultSpriteEmotion()	{ return m_pA2DSpriteEmotion; }
	abase::vector<AUITEXTAREA_EMOTION>** GetDefaultEmotion()	{ return m_vecEmotion; }
	abase::vector<A2DSprite*>* GetImageList() { return m_pvecImageList; }
	void SetImageList(abase::vector<A2DSprite*> *pvecImageList) { m_pvecImageList = pvecImageList; }
	abase::vector<FONT_WITH_SHADOW>* GetHintOtherFonts() { return m_pvecHintOtherFonts; }
	void SetHintOtherFonts(abase::vector<FONT_WITH_SHADOW>* pvecFonts) { m_pvecHintOtherFonts = pvecFonts; }
	FONT_PARAM* GetImagePictureFontParam() { return &m_FontImagePicture; }

	void SetUseFontAdapt(bool bUseAdapt) { m_bUseFontAdapt = bUseAdapt; }
	bool IsUseFontAdapt() { return m_bUseFontAdapt; }

	void SetA3DFTFontMan(A3DFTFontMan* pFTFontMan)
	{
		if (!m_pFontMan)
		{
			m_pFontMan = pFTFontMan;
			m_bOwnFontMan = false;
		}
		else
		{
			assert(false);
		}
	}

	void SetWordWidth(int nWordWidth)
	{
		m_nWordWidth = nWordWidth;
	}

	int GetWordWidth() const {return m_nWordWidth;}

	virtual bool ApplyTemplates();

	A2DDotLineMan* GetDotLineMan();
	A2DDotLine* GetDefaultDotLine();

	void SetAudioEvtSystem(AudioEngine::EventSystem* pEvtSystem);
	void PlayAudioInstance(AudioEngine::Event* pEvent);
	//void StopAudioInstance(AudioEngine::EventInstance* pEvtInst);
	bool LoadAudioEvtInstance(AudioEngine::Event*& pEvent, const char* szEventID);
	//void DestroyAudioEvtInstance(AudioEngine::EventInstance*& pEvtInst);

protected:
	HWND				m_hWnd;
	A3DEngine *			m_pA3DEngine;
	A3DDevice *			m_pA3DDevice;

	LPLOADINGCALLBACK	m_pLoadingCallBack;

	// property
	int					m_nDefaultWidth;
	int					m_nDefaultHeight;
	AString				m_szName;
	float				m_fWindowScale;
	A3DRECT				m_rcWindow;
	int					m_nWordWidth;
	AString				m_strFilename;
	AString				m_strDefaultDotlineFileName;
	abase::hash_map<int, ACString> m_StringTable;
	AString m_szMsgBoxFrame, m_szMsgBoxButton, m_szMsgBoxButtonDown, m_szMsgBoxButtonHover;

	// resources
	A2DSprite *								m_pA2DSpriteEmotion[AUIMANAGER_MAX_EMOTIONGROUPS];
	abase::vector<AUITEXTAREA_EMOTION> *	m_vecEmotion[AUIMANAGER_MAX_EMOTIONGROUPS];
	abase::vector<A2DSprite*> *				m_pvecImageList;

	// font
	bool				m_bUseFontAdapt;
	FONT_PARAM			m_FontHint;
	FONT_PARAM 			m_FontImagePicture;
	FONT_PARAM			m_FontMessageBox;
	A3DFTFontMan *		m_pFontMan;
	bool				m_bOwnFontMan;
	A3DFTFont *			m_pFont;
	A3DFTFont *			m_pPateFont;
	ACString			m_strDefaultFontName;
	int					m_nDefaultFontSize;
	abase::vector<FONT_WITH_SHADOW> *m_pvecHintOtherFonts;	

	// dialog
public:
	APtrList<PAUIDIALOG>					m_DlgZOrder;
	APtrList<PAUIDIALOG>					m_DlgZOrderBack;
protected:
	abase::vector<PAUIDIALOG>				m_aDialog;
	abase::hash_map<AString, PAUIDIALOG>	m_DlgName;

	// manager
	CWndThreadManager *	m_pWndThreadMan;
	A2DDotLineMan *		m_pDotlineMan;
	A2DDotLine*			m_pDotline;

	// audio
	AudioEngine::EventSystem*	m_pAudioEvtSystem;
	//AudioEngine::EventInstance*	m_pCurAudioInst;
	
	// message context
	PAUIDIALOG			m_pDlgActive;
	PAUIDIALOG			m_pDlgHint;
	int					m_nHintDelay;
	DWORD				m_dwDeltaTime;
	PAUIDIALOG			m_pDlgMouseOn;
	PAUIOBJECT			m_pObjMouseOn;
	PAUIOBJECT			m_pObjHint;
	POINT				m_ptObjHint;
	void*				m_pObjHintSel;
	PAUIFRAME			m_pAUIFrameHint;
	A2DSprite *			m_pA2DSpriteCover;
	bool				m_bUseCover;
	ACString			m_strHint;	
	bool				m_bRespawnMessage;
	abase::hash_map<int, int>	m_MouseMsg;
	abase::hash_map<int, int>	m_KeyboardMsg;
	

protected:
	// Helper functions
	bool RenderHint();

	// 计算UI应该采用的缩放比例,ResizeWindow时会自动调用此函数
	// 重载这个函数，对应不同的游戏窗口大小，计算或者修正缩放比例
	virtual void CalcWindowScale();

	// 根据pszTempName(.dtf文件名)在堆上创建具体的Dialog对象
	// 如果要使用AUIManager的自动生成Dialog功能(从.dcf文件生成)，
	// 重载这个函数，为不同的.dtf文件创建适当的Dialog对象，但
	// 记得默认要返回AUIDialog对象，以兼容以前的代码
	virtual PAUIDIALOG CreateDlgInstance(const AString strTemplName);

	virtual void InitOtherFonts(){}

	bool ImportStringTable(const char *pszFilename);
	bool ImportUIParam(const char *pszFilename);
};

typedef AUIManager * PAUIMANAGER;


class AUIManagerDialogIteratorByZOrder {
	AUIManager* m_pAUIManager;
	ALISTPOSITION	m_currentListPosition;
	bool			m_bLoopingBackList;

	APtrList<PAUIDIALOG>& GetList();
	APtrList<PAUIDIALOG>& GetBackList();
	APtrList<PAUIDIALOG>& GetCurrentList();

	void CheckSwitchToBackList();

public:
	AUIManagerDialogIteratorByZOrder(AUIManager*);
	bool GetCurrentAndLoopToNext(AUIDialog*&);
};


#endif //_AUIMANAGER_H_