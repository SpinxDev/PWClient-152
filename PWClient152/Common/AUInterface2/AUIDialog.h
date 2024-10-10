// Filename	: AUIDialog.h
// Creator	: Tom Zhou
// Date		: May 12, 2004
// Desc		: AUIDialog is a class that simulate a Windows dialog.

#ifndef _AUIDIALOG_H_
#define _AUIDIALOG_H_

#include "Windows.h"
#include "AAssist.h"
#include "vector.h"
#include "hashtab.h"
#include "A3DTypes.h"
#include "AUISlider.h"

class A3DEngine;
class A3DDevice;
class A2DSprite;
class A3DFTFont;
class AUIDialog;
class AUISubDialog;
class AScriptFile;
class UIRenderTarget;
typedef class AUIFrame * PAUIFRAME;
typedef class AUIObject * PAUIOBJECT;
typedef struct _AUIOBJECT_PROPERTY * PAUIOBJECT_PROPERTY;
typedef union _AUIOBJECT_SETPROPERTY * PAUIOBJECT_SETPROPERTY;

namespace AudioEngine
{
	class EventInstance;
	class Event;
}

#define AUIDIALOG_MAXNAMELEN	64

//define custom ime message
#define WM_USER_IME_CHANGE_FOCUS	(WM_USER + 0x9877)

enum
{
	AUIDIALOG_TYPE_DIALOG = 0,
	AUIDIALOG_TYPE_MESSAGEBOX,
	AUIDIALOG_TYPE_POPUPMENU
};

enum
{
	AUIDIALOG_FRAME_MIN = 0,
	AUIDIALOG_FRAME_FRAME = AUIDIALOG_FRAME_MIN,
	AUIDIALOG_FRAME_BACK,
	AUIDIALOG_FRAME_TITLE,
	AUIDIALOG_FRAME_CLOSE,
	AUIDIALOG_FRAME_MAX
};
#define AUIDIALOG_FRAME_NUM		AUIDIALOG_FRAME_MAX

enum
{
	AUIDIALOG_CLOSE_MIN = 0,
	AUIDIALOG_CLOSE_NORMAL = AUIDIALOG_CLOSE_MIN,
	AUIDIALOG_CLOSE_ONHOVER,
	AUIDIALOG_CLOSE_CLICK,
	AUIDIALOG_CLOSE_MAX
};
#define AUIDIALOG_CLOSE_NUM		AUIDIALOG_CLOSE_MAX

enum
{
	AUIDIALOG_ALIGN_NONE = 0,
	AUIDIALOG_ALIGN_INSIDE,
	AUIDIALOG_ALIGN_OUTSIDE,
	AUIDIALOG_ALIGN_LEFT,
	AUIDIALOG_ALIGN_RIGHT,
	AUIDIALOG_ALIGN_TOP,
	AUIDIALOG_ALIGN_BOTTOM,
	AUIDIALOG_ALIGN_CENTER
};

enum
{
	AUIDIALOG_RENDERSTATE_NORMAL = 0,
	AUIDIALOG_RENDERSTATE_STATIC,
	AUIDIALOG_RENDERSTATE_DYNAMIC
};

enum
{
	AUIDIALOG_VIEWPORT_RENDER_BOTTOM = 0,
	AUIDIALOG_VIEWPORT_RENDER_TOP
};


typedef struct _AUIOBJECTLISTELEMENT
{
	int id;
	_AUIOBJECTLISTELEMENT *pLast;
	PAUIOBJECT pThis;
	_AUIOBJECTLISTELEMENT *pNext;
	bool bHeapAlloc;
} AUIOBJECTLISTELEMENT, * PAUIOBJECTLISTELEMENT;

class AUIManager;
class A3DViewport;
class A3DTexture;

//////////////////////////////////////////////////////////////////////////
// structure and macros for EVENT MAP mechanism
//////////////////////////////////////////////////////////////////////////
typedef void (AUIDialog::*AUI_ON_EVENT_FUNC)(WPARAM, LPARAM, AUIObject *);

struct AUI_EVENT_MAP_ENTRY
{ 
	AString szObjectName; 
	UINT uMessage; 
	AUI_ON_EVENT_FUNC pFunc; 

	AUI_EVENT_MAP_ENTRY::AUI_EVENT_MAP_ENTRY(const char* objname, UINT msg, AUI_ON_EVENT_FUNC func)
	{
		if( !objname )
			szObjectName = "";
		else
			szObjectName = objname;
		uMessage = msg;
		pFunc = func;
	}
}; 

struct AUI_EVENT_MAP_TYPE
{
	const AUI_EVENT_MAP_TYPE * pBaseMap;
	const AUI_EVENT_MAP_ENTRY * pMapEntries;
};

#define AUI_DECLARE_EVENT_MAP() \
private: \
	static const AUI_EVENT_MAP_ENTRY _eventMapEntries[]; \
protected: \
	static const AUI_EVENT_MAP_TYPE _eventMap; \
	virtual const AUI_EVENT_MAP_TYPE * GetEventMap() const; 

#define AUI_BEGIN_EVENT_MAP(theClass, baseClass) \
const AUI_EVENT_MAP_TYPE * theClass::GetEventMap() const \
{ return &theClass::_eventMap; } \
const AUI_EVENT_MAP_TYPE theClass::_eventMap = \
{ &baseClass::_eventMap, theClass::_eventMapEntries}; \
const AUI_EVENT_MAP_ENTRY theClass::_eventMapEntries[] = \
{

#define AUI_ON_EVENT(objname, message, func) \
	AUI_EVENT_MAP_ENTRY((objname), (message), (AUI_ON_EVENT_FUNC)(&func)),

#define AUI_END_EVENT_MAP() \
	AUI_EVENT_MAP_ENTRY("", 0, 0) \
};

#define AUI_BEGIN_EVENT_MAP_BASE(theClass) \
const AUI_EVENT_MAP_TYPE * theClass::GetEventMap() const \
{ return &theClass::_eventMap; } \
const AUI_EVENT_MAP_TYPE theClass::_eventMap = \
{ NULL, theClass::_eventMapEntries}; \
const AUI_EVENT_MAP_ENTRY theClass::_eventMapEntries[] = \
{

//////////////////////////////////////////////////////////////////////////
// structure and macros for COMMAND MAP mechanism
//////////////////////////////////////////////////////////////////////////

typedef void (AUIDialog::*AUI_ON_COMMAND_FUNC)(const char * szCommand);

struct AUI_COMMAND_MAP_ENTRY
{ 
	AString szCommand; 
	AUI_ON_COMMAND_FUNC pFunc; 

	AUI_COMMAND_MAP_ENTRY(const char* cmd, AUI_ON_COMMAND_FUNC func)
	{
		szCommand = cmd;
		pFunc = func;
	}
}; 

struct AUI_COMMAND_MAP_TYPE
{
	const AUI_COMMAND_MAP_TYPE * pBaseMap;
	const AUI_COMMAND_MAP_ENTRY * pMapEntries;
};

#define AUI_DECLARE_COMMAND_MAP() \
private: \
	static const AUI_COMMAND_MAP_ENTRY _commandMapEntries[]; \
protected: \
	static const AUI_COMMAND_MAP_TYPE _commandMap; \
	virtual const AUI_COMMAND_MAP_TYPE * GetCommandMap() const; 

#define AUI_BEGIN_COMMAND_MAP(theClass, baseClass) \
const AUI_COMMAND_MAP_TYPE * theClass::GetCommandMap() const \
{ return &theClass::_commandMap; } \
const AUI_COMMAND_MAP_TYPE theClass::_commandMap = \
{ &baseClass::_commandMap, theClass::_commandMapEntries}; \
const AUI_COMMAND_MAP_ENTRY theClass::_commandMapEntries[] = \
{

#define AUI_ON_COMMAND(command, func) \
	AUI_COMMAND_MAP_ENTRY((command), (AUI_ON_COMMAND_FUNC)(&func)),

#define AUI_END_COMMAND_MAP() \
	AUI_COMMAND_MAP_ENTRY("", 0) \
};


#define AUI_BEGIN_COMMAND_MAP_BASE(theClass) \
const AUI_COMMAND_MAP_TYPE * theClass::GetCommandMap() const \
{ return &theClass::_commandMap; } \
const AUI_COMMAND_MAP_TYPE theClass::_commandMap = \
{ NULL, theClass::_commandMapEntries}; \
const AUI_COMMAND_MAP_ENTRY theClass::_commandMapEntries[] = \
{

//////////////////////////////////////////////////////////////////////////
// some helper
//////////////////////////////////////////////////////////////////////////


#define IMPLEMENT_GET_CONCRETE_MANAGER(managerClass) \
managerClass * GetConcreteManager() \
{ \
	return dynamic_cast<managerClass *>(m_pManager); \
}

//////////////////////////////////////////////////////////////////////////
// AUIDialog
//////////////////////////////////////////////////////////////////////////


class AUIDialog
{
	friend class AUIObject;
	friend class AUIManager;
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();
protected:
	typedef struct _MENU_ITEM
	{
		bool bCheck;
		bool bEnable;
		AString strCommand;
		ACString strMenuText;
	} MENU_ITEM, * P_MENU_ITEM;
	typedef struct _FRAME
	{
		A3DViewport *pA3DViewport;
		A3DRECT rc;
		int nRenderPriority;
	} FRAME, * P_FRAME;
	typedef abase::hashtab<ACString, int, abase::_hash_function> AUIDIALOG_STRINGTABLE;
	typedef abase::hashtab<PAUIOBJECT, AString, abase::_hash_function> AUIDIALOG_AUIOBJECTTABLE;
public:
	//////////////////////////////////////////////////////////////////////////
	// operation
	//////////////////////////////////////////////////////////////////////////
	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszTempName);
	virtual bool InitFromXML(const char *pszTempName);
	virtual bool Release(void);
	
	virtual bool Tick(void);
	virtual bool Render(void);
	bool RenderOnce(void);

	virtual bool Save(const char *pszFilename);
	
	void AlignTo(AUIDialog *pBase, int nInsideX, int nAlignX, int nInsideY, int nAlignY, int nOffsetX = 0, int nOffsetY = 0);
	inline void Blink(int nInterval) { m_nBlinkInterval = nInterval; }		// Zero is disable blinking.
	
	// 只有bShow为true时，param才起作用
	virtual void Show(bool bShow, bool bModal = false, bool bActive = true);
	virtual void OptimizeSpriteResource(bool bLoad);

	int AddStringToTable(const ACHAR *pszString);
	
	virtual bool AddObjectToList(PAUIOBJECT pAUIObject);
	virtual PAUIOBJECT CreateControl(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, int idControl, bool bAddToDialog = true);
	PAUIOBJECT CloneControl(PAUIOBJECT pAUIObject, bool bAddToDialog = true);
	virtual bool DeleteControl(PAUIOBJECT pAUIObject);
	
	void SetCapture(bool bCapture, int x = -1, int y = -1);
	inline void SetCaptureObject(PAUIOBJECT pObject) { m_pObjCapture = pObject; }
	void AddFrame(A3DViewport *pViewport, A3DRECT &rc, int nRenderPriority = AUIDIALOG_VIEWPORT_RENDER_BOTTOM);
	void DelFrame(A3DViewport *pViewport);
	void ChangeFrame(A3DViewport *pViewport, A3DRECT &rc, int nRenderPriority = AUIDIALOG_VIEWPORT_RENDER_BOTTOM);
	A3DRECT* GetFrameRect(A3DViewport *pViewport);
	void UpdateData(bool bSave = true);

	virtual bool OnCommand(const char *pszCommand);
	inline void SetCommand(const char *pszCommand) { m_strCommand = pszCommand; }
	virtual bool DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool bKeyboard);

	//////////////////////////////////////////////////////////////////////////
	// helper
	//////////////////////////////////////////////////////////////////////////
	void CheckRadioButton(int idGroup, int idCheck);
	int GetCheckedRadioButton(int idGroup);
	void ChangeFocus(PAUIOBJECT pNewFocus);
	void CheckMenuItem(const char *pszCommand, bool bCheck = true);
	void EnableMenuItem(const char *pszCommand, bool bEnable = true);
	void CheckStillImageButton(int nIndexToCheck, int nButtonCount, ...);

	
	//////////////////////////////////////////////////////////////////////////
	// state / property
	//////////////////////////////////////////////////////////////////////////
	inline int GetFileVersion() { return m_nFileVersion; }
	inline bool IsEnabled() { return m_bEnabled; }
	inline bool IsPopup() { return m_bPopup; }
	virtual bool IsShow(void);
	inline bool IsDefaultShow(void) { return m_bDefaultShow; }
	inline bool IsModal(void) { return m_bModal; }
	bool IsActive();
	inline bool IsBackgroundDialog() { return m_bBackgroundDialog; }
	inline bool IsCapture(void) { return m_bCapture; }
	inline bool IsCanMove() { return m_bCanMove; }
	inline bool IsCanChangeZOrder() { return m_bCanChangeZOrder; }
	inline bool IsNoFlush() { return m_bNoFlush; }
	inline bool IsCanOutOfWindow() { return m_bCanOutOfWindow; }
	inline bool IsCanEscClose() { return m_bCanEscClose; }
	bool IsInHitArea(int x, int y);
	virtual bool IsBlocked() { return false; }

	bool IsOptimizeResourceLoad() const { return m_bOptimizeResourceLoad; }
	void SetOptimizeResourceLoad(bool b) { m_bOptimizeResourceLoad = b; }
	bool IsResourceOptimized()const { return m_nOptimizeResourceState == 0; }

	int RetrievePropertyList(PAUIOBJECT_PROPERTY a_Property);
	inline int GetAlpha(void) { return m_nAlpha; }
	inline int GetWholeAlpha(void) { return m_nWholeAlpha; }
	inline int GetType(void) { return m_nType; }
	POINT GetPos(void);
	APointF GetPercentPos(void);
	inline float GetScale(void) { return m_fScale; }
	SIZE GetSize(void);
	SIZE GetDefaultSize(void);
	A3DRECT GetRect(void);
	inline const char * GetName(void) { return m_szName; }
	inline const ACHAR * GetText(void) { return m_strTitle; }
	inline const char * GetFilename(void) { return m_szFilename; }
	inline PAUIOBJECT GetFocus() { return m_pObjFocus; }
	inline bool GetDestroyFlag() { return m_bDestroyFlag; }
	inline PAUIFRAME GetFrame() { return m_pAUIFrameDlg; }
	const ACHAR * GetStringFromTable(int idString);
	void ForceGetData(DWORD &dwData, AString &strName);
	DWORD GetData(AString strName = "");
	void ForceGetDataPtr(void *&pvData, AString &strName);
	void * GetDataPtr(AString strName = "");
	void ForceGetData64(UINT64 &uiData64, AString &strName);
	UINT64 GetData64(AString strName = "");
	void ForceGetDataPtr64(UINT64 &uiDataPtr64, AString &strName);
	UINT64 GetDataPtr64(AString strName = "");
	inline AUIManager * GetAUIManager() { return m_pAUIManager; }
	inline AUISubDialog * GetParentDlgControl() { return m_pParentDlgControl; }
	AUIObject * GetLastCommandGenerator() { return m_pObjLastCommandGenerator; }
	int GetHintDelay() { return m_nHintDelay; }
	bool GetRenderTarget() { return m_pUIRenderTarget != NULL; }
	UIRenderTarget* GetRenderTargetPtr() { return m_pUIRenderTarget; }
	inline bool GetImageCoverOrderly() { return m_bImageCoverOrderly; }
	inline int GetCurImageCover() { return m_nCurImageCover; }
	inline int GetMinDistDlgRToScrL() { return m_nMinDistDlgRToScrL; }
	inline int GetMinDistDlgLToScrR() { return m_nMinDistDlgLToScrR; }
	inline int GetMinDistDlgBToScrT() { return m_nMinDistDlgBToScrT; }
	inline int GetMinDistDlgTToScrB() { return m_nMinDistDlgTToScrB; }

	void SaveRenderTarget();
	/* support properties
	"Name"						Property->c
	"Text Color"				Property->dw
	"Font"						Property->font
	"X"							Property->i
	"Y"							Property->i
	"Width"						Property->i
	"Height"					Property->i
	"Shadow"					Property->i
	"Outline"					Property->i
	"Alpha"						Property->i
	"Frame mode"				Property->i
	"Frame Image"				Property->fn
	"Background Image"			Property->fn
	"Title"						Property->s
	"Title Bar Image"			Property->fn
	"Title Bar X"				Property->i
	"Title Bar Y"				Property->i
	"Title Bar Width"			Property->i
	"Title Bar Height"			Property->i;
	 "Close Button Image"		Property->fn
	 "Close Button X"			Property->i
	 "Close Button Y"			Property->i
	 "Background Dialog"		Property->b
	 "Can Out Of Window"		Property->b
	 "Min Dist DlgR To ScrL"	Property->i
	 "Min Dist DlgL To ScrR"	Property->i
	 "Min Dist DlgT To ScrB"	Property->i
	 "Min Dist DlgB To ScrT"	Property->i
	 "Can Move"					Property->b
	 "Can Change Z-Order"		Property->b
	 "Can Esc Close"			Property->b
	 "Pop Up Dialog"			Property->b
	 "Sound Effect"				Property->fn
	 "Sound Effect Close"		Property->fn
	 "Anomaly"					Property->b
	 "Static Render"			Property->b
	 "Image Cover Orderly"		Property->b
	 "Hint Delay"				Property->i
	*/
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);

	inline void SetEnable(bool bEnable) { m_bEnabled = bEnable; }
	inline void SetPopup(bool bPopup) { m_bPopup = bPopup; }
	inline void SetA3DEngine(A3DEngine *pA3DEngine) { ASSERT(pA3DEngine); m_pA3DEngine = pA3DEngine; }
	inline void SetA3DDevice(A3DDevice *pA3DDevice) { ASSERT(pA3DDevice); m_pA3DDevice = pA3DDevice; }
	inline void SetType(int nType) { m_nType = nType; }
	
	static bool RegisterPropertyType();
	static bool ReadNameFromXML(const char *pszTempName, AString& strName, bool& bHasSubDlg);

	enum AlignMode 
	{
		alignNone,		// 不做额外对齐
		alignMin,		// 定位到父窗口左端/上端
		alignMax,		// 定位到父窗口右端/下端
		alignCenter		// 定位到父窗口中间
	};
	// 如果alignModeX != alignNone，参数x被忽略
	// 如果alignModeY != alignNone，参数y被忽略
	// pParentDlg为NULL时，父窗口为游戏窗口
	void SetPosEx(int x, int y, int alignModeX = alignNone, int alignModeY = alignNone, AUIDialog *pParentDlg = NULL);
	// x, y范围在0至1之间，-2表示最小位置，2表示最大位置
	void SetPercentPosEx(float x, float y);

	void SetScale(float fScale);
	void SetSize(int W, int H);
	void SetDefaultSize(int W, int H);
	bool SetName(const char *pszName);
	inline void SetText(const ACHAR *pszText) { m_strTitle = pszText; }
	inline void SetIsModal(bool bModal)	{ m_bModal = bModal; }
	inline void SetIsDefaultShow(bool bShow) { m_bDefaultShow = bShow; }
	void SetBackgroundDialog(bool bBackgroundDialog);
	inline void SetCanMove(bool bCanMove) { m_bCanMove = bCanMove; }
	inline void SetCanChangeZOrder(bool bCanChangeZOrder) { m_bCanChangeZOrder = bCanChangeZOrder; }
	inline void SetAlpha(int nAlpha) { ASSERT(nAlpha >= 0 && nAlpha <= 255); m_nAlpha = nAlpha; }
	inline void SetWholeAlpha(int nWholeAlpha) { ASSERT(nWholeAlpha >= 0 && nWholeAlpha <= 255); m_nWholeAlpha = nWholeAlpha;}
	inline void SetDestroyFlag(bool bFlag) { m_bDestroyFlag = bFlag; }
	void SetLife(DWORD dwLife);
	inline void SetNoFlush(bool bNoFlush) { m_bNoFlush = bNoFlush; }
	inline void SetCanOutOfWindow(bool bCanOut) { m_bCanOutOfWindow = bCanOut; }
	inline void SetMinDistDlgRToScrL(int iDist) { m_nMinDistDlgRToScrL = iDist; }
	inline void SetMinDistDlgLToScrR(int iDist) { m_nMinDistDlgLToScrR = iDist; }
	inline void SetMinDistDlgBToScrT(int iDist) { m_nMinDistDlgBToScrT = iDist; }
	inline void SetMinDistDlgTToScrB(int iDist) { m_nMinDistDlgTToScrB = iDist; }
	inline void SetCanEscClose(bool bCanEsc) { m_bCanEscClose = bCanEsc; }
	bool SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
		A3DCOLOR dwColor, int nShadow = 0, int nOutline = 0,
		bool bBold = false, bool bItalic = false);
	void SetData(DWORD dwData, AString strName = "");
	void SetDataPtr(void *pvData, AString strName = "");
	void SetData64(UINT64 uiData64, AString strName = "");
	void SetDataPtr64(UINT64 uiDataPtr64, AString strName = "");
	inline void SetAUIManager(AUIManager *pAUIManager) { ASSERT(pAUIManager); m_pAUIManager = pAUIManager; }
	inline void SetParentDlgControl(AUISubDialog *pAUISubDialog) { m_pParentDlgControl = pAUISubDialog; }
	void SetLastCommandGenerator(AUIObject *pObj) { m_pObjLastCommandGenerator = pObj; }
	void SetHintDelay(int nHintDelay) { m_nHintDelay = nHintDelay; }
	void SetRenderTarget(bool bRenderTarget);
	inline void SetImageCoverOrderly(bool bOrderly) { m_bImageCoverOrderly = bOrderly; }

	void FitFont();

	//////////////////////////////////////////////////////////////////////////
	// control related
	//////////////////////////////////////////////////////////////////////////
	PAUIOBJECT GetDlgItem(const char *pszItemName);
	inline PAUIOBJECTLISTELEMENT GetFirstControl(void) { return m_pListHead; }
	inline PAUIOBJECTLISTELEMENT GetNextControl(PAUIOBJECTLISTELEMENT pThis) { ASSERT(pThis); return pThis->pNext; }
	inline PAUIOBJECTLISTELEMENT GetLastControl(void) { return m_pListTail; }
	inline PAUIOBJECTLISTELEMENT GetPrevControl(PAUIOBJECTLISTELEMENT pThis) { ASSERT(pThis); return pThis->pLast; }
	inline PAUIOBJECT GetCaptureObject() { return m_pObjCapture; }


	//////////////////////////////////////////////////////////////////////////
	// constructor / destructor
	//////////////////////////////////////////////////////////////////////////
	AUIDialog();
	virtual ~AUIDialog();


	
	// for compatibility
	// only used for init dialog
	void TranslatePosForAlign(int &x, int &y);

	void MoveAllControls(int dx, int dy);
	inline void UpdateRenderTarget() { m_bUpdateRenderTarget = true; }
	inline bool NeedUpdateRenderTarget() const { return m_bUpdateRenderTarget; }
	void SetFlushRenderTarget(bool b) { m_bFlushRenderTarget = b; }
	int GetRenderState() { return m_nRenderState; }
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

	//////////////////////////////////////////////////////////////////////////
	// Template
	//////////////////////////////////////////////////////////////////////////
	virtual bool ApplyTemplates();

#if _MSC_VER >= 1400
	__declspec(deprecated("** this is a deprecated function for poor performance, use RenderToRT instead**")) \
		bool RenderRTToTexture(A3DTexture* pTexture);
#else
	bool RenderRTToTexture(A3DTexture* pTexture);
#endif

	bool RenderToRT(UIRenderTarget* pRT);

	//////////////////////////////////////////////////////////////////////////
	// Is a descendant of pDlg
	bool IsDescendant(const AUIDialog* pDlg) const;

protected:

	//////////////////////////////////////////////////////////////////////////
	// Overridables
	//////////////////////////////////////////////////////////////////////////
	
	virtual bool InitCustomizeControl(const char *szObjectType, AScriptFile* pASF);
	virtual bool OnInitDialog() { return true; }
	virtual void OnTick() {}
	virtual void OnShowDialog() {}
	virtual void OnHideDialog() {}
	virtual void DoDataExchange(bool bSave) {}


	//////////////////////////////////////////////////////////////////////////
	// DDX helper
	//////////////////////////////////////////////////////////////////////////

	// DDX any control
	template<class Control>
	void DDX_Control(const char *szControlName, Control *& pObj)
	{
		ASSERT(pObj == NULL || pObj == GetDlgItem(szControlName));
		if (!pObj) 
		{
			pObj = dynamic_cast<Control *>(GetDlgItem(szControlName));
			ASSERT(pObj);
		}
	}

	// DDX for subclass any control (Add 2011-08-08 daixinyu)
	// Attention: never use it like below.
	//		...
	//		DDX_Control("xx", pObj);
	//		DDX_Control("xx", obj) ; The memory that pObj pointed to is already released.
	//		...
	template<class Control>
	void DDX_ControlRP(const char *szControlName, Control & obj)
	{
		PAUIOBJECT pObj = GetDlgItem(szControlName);
		ASSERT(pObj);

		AXMLFile xml;
		bool b = pObj->ExportToXml(*xml.GetRootItem());
		ASSERT(b);

		obj.Release();
		obj.SetParent(this);

		b = obj.InitXML(m_pA3DEngine, m_pA3DDevice, xml.GetRootItem()->GetFirstChildItem());
		ASSERT(b);

		b = ReplaceControl(GetDlgItem(szControlName), obj);
		ASSERT(b);
	}
	
	// DDX AUIEditBox
	void DDX_EditBox(bool bSave, const char *szControlName, float & fVal);
	void DDX_EditBox(bool bSave, const char *szControlName, int & nVal);
	void DDX_EditBox(bool bSave, const char *szControlName, ACString &strVal);
	void DDX_EditBox(bool bSave, const char *szControlName, ACHAR *szVal);

	// DDX AUICheckBox
	void DDX_CheckBox(bool bSave, const char *szControlName, bool &bChecked);

	// DDX AUIComboBox
	void DDX_ComboBox(bool bSave, const char *szControlName, int &nIndex);
	template<class Type>
	void DDX_ComboBoxItemData(bool bSave, const char *szControlName, Type &data)
	{
		AUIComboBox *pComboBox = dynamic_cast<AUIComboBox *>(GetDlgItem(szControlName));
		ASSERT(pComboBox);
		if (bSave)
		{
			int nIndex = 0;
			DDX_ComboBox(bSave, szControlName, nIndex);
			if (nIndex >= 0 && nIndex < pComboBox->GetCount())
				data = static_cast<Type>(pComboBox->GetItemData(nIndex));
		}
		else
		{
			int nIndex = 0;
			int nCount = pComboBox->GetCount();
			while (nCount--)
			{
				if (static_cast<Type>(pComboBox->GetItemData(nCount)) == data)
				{
					nIndex = nCount;
					break;
				}
			}
			DDX_ComboBox(bSave, szControlName, nIndex);
		}
	}

	// DDX AUIListBox
	void DDX_ListBox(bool bSave, const char *szControlName, int &nIndex);
	template<class Type>
	void DDX_ListBoxItemData(bool bSave, const char *szControlName, Type &data)
	{
		AUIListBox *pListbox = dynamic_cast<AUIListBox *>(GetDlgItem(szControlName));
		ASSERT(pListbox);
		if (bSave)
		{
			int nIndex = 0;
			DDX_ListBox(bSave, szControlName, nIndex);
			if (nIndex >= 0 && nIndex < pListbox->GetCount())
				data = static_cast<Type>(pListbox->GetItemData(nIndex));
		}
		else
		{
			int nIndex = 0;
			int nCount = pListbox->GetCount();
			while (nCount--)
			{
				if (static_cast<Type>(pListbox->GetItemData(nCount)) == data)
				{
					nIndex = nCount;
					break;
				}
			}
			DDX_ListBox(bSave, szControlName, nIndex);
		}
	}

	// DDX AUIProgress
	void DDX_Progress(bool bSave, const char *szControlName, int &nPos);

	// DDX AUIRadioButton
	void DDX_RadioButton(bool bSave, int nGroupID, int &nCheckedButtonID);

	// DDX AUIScroll
	void DDX_Scroll(bool bSave, const char *szControlName, int &nLevel);

	// DDX AUISlider
	// note : ValType should be number type
	// or the result may be not correct
	template<class ValType>
	void DDX_Slider(bool bSave, const char *szControlName, ValType &levelVal)
	{
		AUISlider * pControl = dynamic_cast<AUISlider *>(GetDlgItem(szControlName));
		ASSERT(pControl);
		if (bSave)
			levelVal = static_cast<ValType>(pControl->GetLevel());
		else
			pControl->SetLevel(static_cast<int>(levelVal));
	}

	// DDX StillImageButton
	void DDX_StillImageButton(bool bSave, const char *szControlName, bool &bPushed);
	void DDX_StillImageButtonGroup(bool bSave, int &nIndexOfPushedButton, int nButtonCount, ...);

	//////////////////////////////////////////////////////////////////////////
	// internal
	//////////////////////////////////////////////////////////////////////////
	
	// EVENT MAP associated
	//bool OnEventMap(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	virtual bool OnEventMap(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	bool ProcessEventMap(const AUI_EVENT_MAP_ENTRY *pMapEntry, UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	
	// COMMAND MAP associated
	//bool OnCommandMap(const char *szCommand);
	virtual bool OnCommandMap(const char *szCommand);
	bool ProcessCommandMap(const AUI_COMMAND_MAP_ENTRY *pMapEntry, const char *szCommand);

protected:

	int m_nFileVersion;
	AUIObject * m_pObjLastCommandGenerator;

	bool m_bEnabled;
	bool m_bPopup;
	bool m_bOptimizeResourceLoad;
	int m_nOptimizeResourceState;

	A3DEngine *m_pA3DEngine;
	A3DDevice *m_pA3DDevice;
	UIRenderTarget *m_pUIRenderTarget;
	bool m_bUpdateRenderTarget;
	bool m_bFlushRenderTarget;
	int m_nRenderState;
	bool m_bImageCoverOrderly;
	int m_nCurImageCover;
	AUIManager *m_pAUIManager;
	AUISubDialog *m_pParentDlgControl;
	abase::vector<FRAME> m_vecFrame;

	AString m_szName;
	AString m_szFilename;

	ACString m_strTitle;
	A3DFTFont * m_pFont;
	ACString m_strFontName;
	int m_nFontHeight;
	float m_fDisplayHeight;
	A3DCOLOR m_color;

	int m_nShadow;
	int m_nOutline;
	bool m_bBold;
	bool m_bItalic;
	int m_nType;
	int m_nState;
	bool m_bCapture;
	bool m_bCanMove;
	bool m_bCanChangeZOrder;
	bool m_bBackgroundDialog;
	bool m_bNoFlush;
	bool m_bCanOutOfWindow;
	bool m_bCanEscClose;
	POINT m_ptLButtonDown;
	bool m_bShow, m_bModal, m_bDefaultShow;
	float m_fScale;
	int m_x, m_y;
	int m_nWidth, m_nHeight;
	int m_nDefaultWidth, m_nDefaultHeight;
	float m_fPercentX, m_fPercentY;
	AString m_strCommand;
	AString m_strSound, m_strSoundClose;

	AudioEngine::Event* m_pAudio;
	AudioEngine::Event* m_pAudioClose;

	bool m_bDestroyFlag;
	int m_nBlinkInterval;
	DWORD m_dwLife, m_dwLifeStart;		// Seconds.
	int m_nHintDelay;
	int m_nMinDistDlgLToScrR;
	int m_nMinDistDlgRToScrL;
	int m_nMinDistDlgBToScrT;
	int m_nMinDistDlgTToScrB;

	PAUIOBJECT m_pObjFocus;
	PAUIOBJECT m_pObjEvent;
	PAUIOBJECT m_pObjCapture;

	DWORD m_dwData;
	void *m_pvData;
	UINT64 m_uiData64;
	UINT64 m_uiDataPtr64;
	AString m_strDataName;
	AString m_strDataPtrName;
	AString m_strData64Name;
	AString m_strDataPtr64Name;

	int m_nCurSel;
	int m_nMaxMenuTextWidth;
	int m_nMaxMenuTextHeight;
	A2DSprite *m_pA2DSpriteHilight;
	abase::vector<MENU_ITEM> m_MenuItem;
	bool RenderMenuItems(void);
	bool DealWindowsPopupMenuMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool m_bAnomaly;
	int m_nAlpha;
	int m_nWholeAlpha;
	int m_nFrameMode;
	PAUIFRAME m_pAUIFrameDlg;
	PAUIFRAME m_pAUIFrameTitle;
	POINT m_pt[AUIDIALOG_FRAME_NUM];
	SIZE m_Size[AUIDIALOG_FRAME_NUM];
	A2DSprite * m_pA2DSprite[AUIDIALOG_FRAME_NUM];
	bool InitFrame(AScriptFile *pASF);
	bool InitFrameXML(AXMLItem *pItem);
	bool ImportMenuItems(AScriptFile *pASF);
	bool InitIndirect(const char *pszFileName, int idType);

	int m_nListSize;
	AUIDIALOG_AUIOBJECTTABLE m_AUIObjectTable;
	PAUIOBJECTLISTELEMENT m_pListHead, m_pListTail;
	bool RenderControls(void);
	bool RenderControlsInViewport(int nIndex);
	bool RenderFrameInSubdialog();

	AUIDIALOG_STRINGTABLE m_StringTable;
	bool ImportStringTable(const char *pszFilename);
	bool ExportStringTable(const char *pszFilename);

	bool TabStop(PAUIOBJECT pAUIObject, int nDir);
	bool ImportHintTable(const char *pszFilename);
	bool DealWindowsMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool IsOutOfWindowParamValid(int nWindowWidth, int nWindowHeight);
	void AdjustFromXY(int nWindowWidth, int nWindowHeight);
	void AdjustFromPercent(int nWindowWidth, int nWindowHeight);

	bool AddObjectToList(PAUIOBJECT pAUIObject, bool bHeapAlloc);
	bool DeleteControl(PAUIOBJECT pObj, PAUIOBJECTLISTELEMENT& pos);
	PAUIOBJECTLISTELEMENT InsertControl(PAUIOBJECTLISTELEMENT pos, AUIObject& obj);
	bool ReplaceControl(PAUIOBJECT pObjOld, AUIObject& obj);

	bool RenderRecurse();
};

typedef AUIDialog * PAUIDIALOG;





#endif //_AUIDIALOG_H_