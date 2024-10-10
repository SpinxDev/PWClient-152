// Filename	: AUITextArea.h
// Creator	: Tom Zhou
// Date		: December 17, 2004
// Desc		: AUITextArea is the class of text area control.

#ifndef _AUITEXTAREA_H_
#define _AUITEXTAREA_H_

#include "vector.h"

#include "AAssist.h"
#include "AUIObject.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIEditBox.h"

class EditBoxItemBase;
class EditBoxItemsSet;
class A3DFlatCollector;
class AUITextTagCacheManager;
enum
{
	AUITEXTAREA_RECT_MIN = 0,
	AUITEXTAREA_RECT_FRAME = AUITEXTAREA_RECT_MIN,
	AUITEXTAREA_RECT_UP,
	AUITEXTAREA_RECT_DOWN,
	AUITEXTAREA_RECT_SCROLL,
	AUITEXTAREA_RECT_BAR,
	AUITEXTAREA_RECT_MAX
};
#define AUITEXTAREA_RECT_NUM			AUITEXTAREA_RECT_MAX

#define AUITEXTAREA_STATE_NORMAL		0
#define AUITEXTAREA_STATE_PRESSUP		1
#define AUITEXTAREA_STATE_PRESSDOWN		2
#define AUITEXTAREA_STATE_PRESSBAR		3
#define AUITEXTAREA_EMOTHION_MAXFRAME	20

typedef struct _AUITEXTAREA_NAME_LINK
{
	A3DRECT rc;
	ACString strName;
} AUITEXTAREA_NAME_LINK, * P_AUITEXTAREA_NAME_LINK;

typedef struct _AUITEXTAREA_CHANNEL_LINK
{
	A3DRECT rc;
	ACString strName;
} AUITEXTAREA_CHANNEL_LINK, * P_AUITEXTAREA_CHANNEL_LINK;

struct AUITEXTAREA_EDITBOX_ITEM
{
	A3DRECT rc;
	EditBoxItemBase* m_pItem;
};

typedef struct _AUITEXTAREA_IMAGE
{
	A2DSprite *pSprite;
	bool bSelfRelease;
} AUITEXTAREA_IMAGE, * P_AUITEXTAREA_IMAGE;

typedef struct _AUITEXTAREA_EMOTION
{
	int nStartPos;
	int nNumFrames;
	int	nTotalTick;
	int	nFrameTick[AUITEXTAREA_EMOTHION_MAXFRAME];
	ACString strHint;
} AUITEXTAREA_EMOTION, * P_AUITEXTAREA_EMOTION;

class AUITextArea : public AUIObject
{
public:
	AUITextArea();
	virtual ~AUITextArea();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool InitIndirect(const char *pszFileName, int idType);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual void SetText(const ACHAR *pszText);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	/* support properties
	"Frame mode"			Property->i
	"Frame Image"			Property->fn
	"Up Button Image"		Property->fn
	"Down Button Image"		Property->fn
	"Scroll Area Image"		Property->fn
	"Scroll Bar Image"		Property->fn
	"Left Scroll Bar"		Property->b
	"Text Transparent"		Property->b
	"Trans Scroll Wheel"	Property->b
	"Link Underline Mode"	Property->i
	"Link Underline Color"	Property->dw
	AUIObject.Properties
	*/
	virtual bool GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual bool IsInHitArea(int x, int y, UINT msg = WM_LBUTTONDOWN);
	virtual int GetRequiredHeight();
	virtual bool SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
		A3DCOLOR dwColor, int nShadow = 0, int nOutline = 0,
		bool bBold = false, bool bItalic = false, bool bFreeType = true,
		A3DCOLOR clOutline = 0,
		A3DCOLOR clInnerText = A3DCOLORRGB(255, 255, 255),
		bool bInterpColor = false,
		A3DCOLOR clUpper = 0,
		A3DCOLOR clLower = 0);
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	void SetMaxLines(int nLines);
	void ScrollToTop();
	void ScrollToBottom();
	void AppendText(const ACHAR *pszText, int nMsgIndex = 0, const ACHAR* szIvtrItem = L"", A3DCOLOR clIvtrItem = 0xffffffff, 
		const EditboxScriptItem** ppScriptItems = NULL, int nScriptItemCount = 0,
		UnderLineMode ulMode = UM_USE_AUIOBJECT_OPTION, A3DCOLOR clUnderLine = 0xff000000);
	void SetOnlyShowText(bool bShow);
	void SetForceRenderScroll(bool bForce);
	void SetLeftScrollBar(bool bLeft);
	void SetFirstLine(int nFirstLine);
	int GetFirstLine() { return m_nFirstLine; }
	int GetLines() { return m_nLines; }
	int GetLinesPP() { return m_nLinesPP;  }
	int GetHitArea(int x, int y);
	abase::vector<AUITEXTAREA_NAME_LINK> & GetNameLink();
	abase::vector<AUITEXTAREA_EDITBOX_ITEM>& GetEditBoxItems() { return m_EditBoxItems; }
	abase::vector<AUITEXTAREA_CHANNEL_LINK>& GetChannelLinks() { return m_vecChannelLink; }
	void SetEmotionList(A2DSprite **pA2DSpriteEmotion,
		abase::vector<AUITEXTAREA_EMOTION> **pvecEmotion);
	void SetImageList(abase::vector<A2DSprite*> *pvecImageList);
	void SetOtherFonts(abase::vector<FONT_WITH_SHADOW>* pvecFonts);
	void SetItemMask(int nMask)	{ m_nItemMask = nMask; }
	EditboxItemType GetItemMask() { return (EditboxItemType)m_nItemMask; }
	void ReserveBuffer(const unsigned int iSize);
	void UpdateAppend();

	const EditBoxItemsSet& AUITextArea::GetItemSet() {return m_ItemsSet;}
	
	A3DRECT GetTextRenderRect() const;

protected:
	bool m_bForceRenderScroll;
	bool m_bTextTransparent;
	bool m_bTransScrollWheel;
	POINT m_ptBar;
	int m_nDragDist;
	bool m_bDragging;
	DWORD m_nScrollDelay;
	bool m_bOnlyShowText;
	bool m_bLeftScrollBar;
	bool m_bScrollToBottom;
	int	m_nItemMask;
	abase::vector<AUITEXTAREA_NAME_LINK> m_vecNameLink;
	abase::vector<AUITEXTAREA_CHANNEL_LINK> m_vecChannelLink;
	int m_nFirstLine, m_nLines, m_nLinesPP;

	A2DSprite **m_pA2DSpriteEmotion;
	abase::vector<AUITEXTAREA_EMOTION> **m_vecEmotion;

	abase::vector<AUITEXTAREA_EDITBOX_ITEM> m_EditBoxItems;
	EditBoxItemsSet m_ItemsSet;

	abase::vector<A2DSprite*> *m_pvecImageList;
	abase::vector<FONT_WITH_SHADOW> *m_pvecOtherFonts;

	int m_nFrameMode;
	PAUIFRAME m_pAUIFrame;
	A2DSprite *m_pA2DSprite[AUITEXTAREA_RECT_NUM];

	bool RenderScroll();
	void BuildRenderCache();
	void AppendRenderCache(const ACHAR* pszText);

protected:

	AUITextTagCacheManager* m_pTagCacheMan;
	int m_nMaxLines;
	int m_ItemUnderLineMode;
	A3DCOLOR m_clItemUnderLine;

};

typedef AUITextArea * PAUITEXTAREA;

#endif	//	_AUITEXTAREA_H_