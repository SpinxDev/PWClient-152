// Filename	: AUIEditBox.h
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIEditBox is the class of edit box control.

#ifndef _AUIEDITBOX_H_
#define _AUIEDITBOX_H_

#include "AUIObject.h"
#include "AUIFrame.h"
#include "AUIImeHook.h"
#include "AUITextArea.h"
#include "AUICommon.h"

// ime part
#define CARET_HEIGHT			12
#define WCHARSIZE				2

// Define language ID
#define LID_TRADITIONAL_CHINESE	0x0404
#define LID_JAPANESE			0x0411
#define LID_KOREAN				0x0412
#define LID_SIMPLIFIED_CHINESE	0x0804

#define AUIEDITBOX_MAXTEXTLEN	512

enum
{
	AUIEDITBOX_RECT_MIN = 0,
	AUIEDITBOX_RECT_FRAME = AUIEDITBOX_RECT_MIN,
	AUIEDITBOX_RECT_UP,
	AUIEDITBOX_RECT_DOWN,
	AUIEDITBOX_RECT_SCROLL,
	AUIEDITBOX_RECT_BAR,
	AUIEDITBOX_RECT_MAX
};

enum
{
	AUIEDITBOX_ALIGN_LEFT,
	AUIEDITBOX_ALIGN_CENTER,
	AUIEDITBOX_ALIGN_RIGHT
};

#define AUIEDITBOX_RECT_NUM				AUIEDITBOX_RECT_MAX
#define AUIEDITBOX_STATE_NORMAL			0
#define AUIEDITBOX_STATE_PRESSUP		1
#define AUIEDITBOX_STATE_PRESSDOWN		2
#define AUIEDITBOX_STATE_PRESSBAR		3

class AUIEditBox : public AUIObject
{
public:
	AUIEditBox();
	virtual ~AUIEditBox();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool InitIndirect(const char *pszFileName, int idType);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	/* support properties	
	  "Is Password"					Property->b
	  "Is ReadOnly"					Property->b
	  "Is Number Only"				Property->b 
	  "Is Multiple Line"			Property->b 
	  "Is Auto Return"				Property->b
	  "Frame mode"					Property->i
	  "Frame Image					Property->fn 
	  "Up Button Image"				Property->fn
	  "Down Button Image"			Property->fn 
	  "Scroll Area Image"			Property->fn 
	  "Scroll Bar Image"			Property->fn
	  "Text Color"					Property->dw
	  "Font Name"					Property->s
	  "Font Height"					Property->i 
	  "Max Text Length"				Property->i
	  "Line Space"					Property->i
	  "Enable Selection"			Property->b
	  "Selection Background Color"	Property->dw 
	  "Text Align"					Property->i
	  AUIObject.Property
	*/
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual void SetFocus(bool bFocus);
	virtual const ACHAR * GetText(void);
	virtual void SetText(const ACHAR *pszText);
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);
	virtual bool IsInHitArea(int x, int y, UINT msg = WM_LBUTTONDOWN);

	virtual bool SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
		A3DCOLOR dwColor, int nShadow = 0, int nOutline = 0,
		bool bBold = false, bool bItalic = false, bool bFreeType = true,
		A3DCOLOR clOutline = 0,
		A3DCOLOR clInnerText = A3DCOLORRGB(255, 255, 255),
		bool bInterpColor = false,
		A3DCOLOR clUpper = 0,
		A3DCOLOR clLower = 0);
	
	void SetTextAndItem(const ACHAR *pszText, const ACHAR* szIvtrItem = L"", A3DCOLOR clIvtrItem = 0xffffffff, const EditboxScriptItem** ppScriptItems = NULL, int nScriptItemCount = 0);
	bool IsAutoReturn(void);
	void SetIsAutoReturn(bool bAutoReturn);
	bool IsWantReturn(void) const;			//manual return
	void SetIsWantReturn(bool bWantReturn);
	bool IsPassword(void);
	void SetIsPassword(bool bPassword);
	bool IsReadOnly(void);
	// bDisableAllMsg is only used when bReadOnly==true
	void SetIsReadOnly(bool bReadOnly, bool bDisableAllMsg = true);
	bool IsNumberOnly();
	void SetIsNumberOnly(bool bNumber);
	bool IsMultipleLine(void);
	// To be compatible with old code
	void SetIsMultipleLine(bool bMultiLine, bool bWantReturn = true);
	void SetMaxLength(int nMaxLen);
	int GetMaxLength();
	void SetLineSpace(int nSpace);
	int GetLineSpace();
	void SetIsEnableSelection(bool bEnabled);
	bool IsEnableSelection();
	void RebuildTextRect(void);

	static void SetHWnd(HWND hWnd);
	static bool GetCurrentIMEState();
	static const ACHAR * GetCurrentIMEName();
	static bool IsIMEAlphaNumeric();
	static void ToggleIME();

	void SetEmotionList(A2DSprite **pA2DSpriteEmotion,
		abase::vector<AUITEXTAREA_EMOTION> **pvecEmotion);
	void SetCurrentEmotion(int nEmotion) { m_nCurEmotionSet = nEmotion; }

	int GetItemCount(EditboxItemType type) const { return m_ItemsSet.GetItemCountByType(type); }
	const EditBoxItemsSet& GetItemSet() {return m_ItemsSet;}
	bool AppendItem(EditboxItemType type, A3DCOLOR cl, const ACHAR* szName, const ACHAR* szInfo, const void* pExtraData = NULL, size_t sz = 0);
	bool HasIvtrItem() const { return m_bHasItem; }
	int GetIvtrItemPack() const { return m_nItemPack; }
	int GetIvtrItemIndex() const { return m_nItemIndex; }

	void SetIvtrItemLocInfo(int nPack, int nIndex)
	{
		m_bHasItem = true;
		m_nItemPack = nPack;
		m_nItemIndex = nIndex;
	}

	void GetScriptItemVec(EditboxScriptItemVec& vec) { m_ItemsSet.GetScriptItemVec(vec); }

	void GetSel(int& nStartChar, int& nEndChar);
	// start=-1 remove any selection; start=0,end=-1 select all;
	void SetSel(int nStartChar, int nEndChar, bool bScrollToCaret);
	void ReplaceSel(const ACHAR* szNewText);
	void ReplaceSel(EditboxItemType type, A3DCOLOR cl, const ACHAR* szName, const ACHAR* szInfo, const void* pExtraData = NULL, size_t sz = 0);

	// if true, m_nMaxLen is the max number of rendered text actually
	void ApplyMaxLengthToRender(bool bApply = true) { m_bMaxLenRendered = bApply; }

protected:
	int			m_nMaxLen;
	int			m_nMaxLenOrg;
	bool		m_bNumber;
	bool		m_bReadOnly;
	bool		m_bPassword;
	bool		m_bMultiline;
	bool		m_bAutoReturn;
	bool		m_bWantReturn;
	A3DRECT		m_rcText;
	int			m_nFrameMode;
	PAUIFRAME	m_pAUIFrame;
	A2DSprite *m_pA2DSprite[AUIEDITBOX_RECT_NUM];
	int			m_nCaretWidthBuf;
	A3DPOINT2	m_ptBar;
	bool		m_bDragging;
	DWORD		m_nScrollDelay;
	int			m_nDragDist;
	int			m_nLineSpace;

	int			m_nLanguage;				// Internal index of language
	bool		m_fIsNT;					// If true, NT. Or Win9X
	UINT		m_nCodePage;				// Code page
	BYTE		m_nCharSet;					// Character set

	// ime operation
	bool		m_bImeOpen;
	int			m_xEndPos;					// End position of buffer 
	int			m_xInsertPos;				// Insert position of buffer
	int			m_xCaretPos;				// Caret positio of buffer
	int			m_xEndBackup;
	int			m_xInsertBackup;
	int			m_xHScrl;					// horz scroll pos, in ansi char 
	int			m_xVScrl;					// vert scroll pos, in ansi char
	int			m_nComSize;					// Composition string size
	int			m_nComCursorPos;			// Current cursor position in composition string
	DWORD		m_nComAttrSize;				// The size of the attribute of the composition string
	DWORD		m_dwProperty;				// Property of IME
	BOOL		m_fStat;					// If true, IME composite string
	BYTE		m_bComAttr[AUIEDITBOX_MAXTEXTLEN+1];	// The attributes of the composition string
	ime_str_val m_ime_var;
	short		m_CharWidthBuf[AUIEDITBOX_MAXTEXTLEN * 2];
	WORD		m_CharHeightBuf[AUIEDITBOX_MAXTEXTLEN * 2];
	AWString	m_szBuffer;				// Text Buffer
	AWString	m_szBackup;				// Buffer for backup last string
	wchar_t		m_szComStr[AUIEDITBOX_MAXTEXTLEN+1];	// Buffer for composition string

	// selected text 
	int			m_xSelBeginPos;				// only for render (caret pos)
	int			m_xSelEndPos;				// only for render (caret pos)
	bool		m_bAllowSelect;	
	bool		m_bSelecting;
	A3DCOLOR	m_clSelectBg;

	// caret operation
	int			m_nBlink;
	bool		m_bShowCaret;
	int			m_nCaretRef;
	A3DPOINT2	m_ptCaret;
	A3DPOINT2	m_ptUnderlnStart;
	A3DPOINT2	m_ptViewOrg;
	int			m_nUnderlnWidth;
	BYTE		m_CharHeight;

	// emotion
	A2DSprite **m_pA2DSpriteEmotion;
	abase::vector<AUITEXTAREA_EMOTION> **m_vecEmotion;
	int m_nCurEmotionSet;

	EditBoxItemsSet m_ItemsSet;
	bool			m_bHasItem;
	int				m_nItemPack;
	int				m_nItemIndex;

	bool			m_bMaxLenRendered; // if true, m_nMaxLen is the max number of text rendered actually

	// ime operations
	void RenderImeWnd();
	void ShowCaret() { m_nCaretRef++; }
	void HideCaret() { m_nCaretRef--; }
	void DisplayCaret();
	void SetCaretPos(int x, int y);
	void DrawUnderline(A3DPOINT2 ptStart, int nWidth);
	void UpdateCaretPos();
	void ShowCaretOnView(bool bForceShow = false);
	BOOL OnImeComposition(UINT wParam, LONG lParam);
	void OnImeStartComposition(UINT wParam, LONG lParam);
	void OnImeEndComposition(UINT wParam, LONG lParam);
	int	 InsertCompStr(bool bResult);
	int  GetWidthOfString(const wchar_t*, int* pHeight = NULL, A3DFTFont* pFont = NULL);
	void ClearBuffer();
	int  GetCombinedCharLength(int); 
	void SetCandiDateWindowPos();
	void SetCompositionWindowPos();
	void SetFont(HKL hKeyboardLayout);
	static void OnSetFocus(bool bIme);
	void OnKeyDown(UINT nChar);
	void OnChar(UINT nChar);
	void RenderUnderline();
	virtual void CalcTexWidthBuf();
	bool CanChangeIme() const { return !m_bPassword && !m_bNumber; }
	bool RenderScroll();

	void CalcInsertPos(int x,int y);
	virtual void RenderEditText(A3DFTFont* pFont, const ACHAR* szText);
	virtual void RenderSelectionTextBg(A3DFTFont* pFont, const ACHAR* szText);
	void DeleteText(int iStartPos, int iEndPos);
	virtual void MoveCaretByKey(UINT uKey);
	virtual bool HandleAccelKey(UINT nChar);

	void CleanSelection();
	bool IsOpOnSelection();
	bool HasAnySelection();
	bool InsertItem(int pos, EditboxItemType type, A3DCOLOR cl, const ACHAR* szName, const ACHAR* szInfo, const void* pExtraData = NULL, size_t sz = 0);

	void DoAlign();

	int ValidInsertText(int nPos, const ACHAR* szAddedText, int nChar);
};

typedef AUIEditBox * PAUIEDITBOX;

#endif //_AUIEDITBOX_H_
