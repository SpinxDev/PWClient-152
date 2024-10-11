 // Filename	: AUITreeView.h
// Creator	: Tom Zhou
// Date		: December 1, 2004
// Desc		: AUITreeView is the class of tree view control.

#ifndef _AUITREEVIEW_H_
#define _AUITREEVIEW_H_

#include "vector.h"

#include "A3DMacros.h"
#include "AAssist.h"
#include "AUIObject.h"
#include "AUIFrame.h"
#include "AUICommon.h"

#define AUITREEVIEW_ERROR	(-1)

enum
{
	AUITREEVIEW_RECT_MIN = 0,
	AUITREEVIEW_RECT_FRAME = AUITREEVIEW_RECT_MIN,
	AUITREEVIEW_RECT_TEXT = AUITREEVIEW_RECT_FRAME,
	AUITREEVIEW_RECT_HILIGHT,
	AUITREEVIEW_RECT_UP,
	AUITREEVIEW_RECT_DOWN,
	AUITREEVIEW_RECT_LEFT,		
	AUITREEVIEW_RECT_RIGHT,	
	AUITREEVIEW_RECT_HSCROLL,
	AUITREEVIEW_RECT_VSCROLL,
	AUITREEVIEW_RECT_HBAR,
	AUITREEVIEW_RECT_VBAR,
	AUITREEVIEW_RECT_PLUS,
	AUITREEVIEW_RECT_MINUS,
	AUITREEVIEW_RECT_LEAF,
	AUITREEVIEW_RECT_LEAFCHECK,
	AUITREEVIEW_RECT_MAX
};
#define AUITREEVIEW_RECT_NUM			AUITREEVIEW_RECT_MAX

// Tree View status.
enum
{
	AUITREEVIEW_STATE_NORMAL = 0,
	AUITREEVIEW_STATE_PRESSUP,
	AUITREEVIEW_STATE_PRESSDOWN,
	AUITREEVIEW_STATE_PRESSVBAR,
	AUITREEVIEW_STATE_PRESSLEFT,
	AUITREEVIEW_STATE_PRESSRIGHT,
	AUITREEVIEW_STATE_PRESSHBAR
};

// Tree View actions.
enum
{
	AUITREEVIEW_EXPAND_EXPAND = 0,
	AUITREEVIEW_EXPAND_COLLAPSE,
	AUITREEVIEW_EXPAND_TOGGLE
};

// Item status.
#define AUITREEVIEW_IS_EXPANDED		0x0001
#define AUITREEVIEW_IS_SELECTED		0x0002
#define AUITREEVIEW_IS_BLINKING		0x0004
#define AUITREEVIEW_IS_LEAFCHECK	0x0008

class _AUITREEVIEW_ITEM
{
	friend class AUITreeView;

public:
	_AUITREEVIEW_ITEM()
	{
		strText = _AL("");
		dwData = 0;
		pvData = NULL;
		uiData64 = 0;
		uiDataPtr64 = 0;
		color = A3DCOLORRGB(255, 255, 255);

		pParent = NULL;
		pFirstChild = NULL;
		pLastChild = NULL;
		pLast = NULL;
		pNext = NULL;

		nIndent = 0;
		dwStatus = 0;
		nLines = 0;
		nItemHeight = 0;
	}

protected:
	ACString strText;
	int nLines;
	int nItemHeight;
	DWORD dwData;
	void *pvData;
	A3DCOLOR color;
	A3DRECT rcItem;
	ACString strHint;
	AString strDataName;
	AString strDataPtrName;
	EditBoxItemsSet itemsSet;
	UINT64 uiData64;
	UINT64 uiDataPtr64;
	AString strData64Name;
	AString strDataPtr64Name;

	_AUITREEVIEW_ITEM *pParent;
	_AUITREEVIEW_ITEM *pFirstChild;
	_AUITREEVIEW_ITEM *pLastChild;
	_AUITREEVIEW_ITEM *pLast;
	_AUITREEVIEW_ITEM *pNext;

	int nIndent;
	DWORD dwStatus;
};

typedef _AUITREEVIEW_ITEM AUITREEVIEW_ITEM;
typedef AUITREEVIEW_ITEM * P_AUITREEVIEW_ITEM;
typedef abase::vector<P_AUITREEVIEW_ITEM> VEC_AUITREEVIEW_ITEM;

// For InsertItem() method.
#define AUITREEVIEW_ROOT	((P_AUITREEVIEW_ITEM)0xFFFF0000)
#define AUITREEVIEW_FIRST	((P_AUITREEVIEW_ITEM)0xFFFF0001)
#define AUITREEVIEW_LAST	((P_AUITREEVIEW_ITEM)0xFFFF0002)

class AUITreeView : public AUIObject
{
public:
	AUITreeView();
	virtual ~AUITreeView();

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
	"Text Transparent"			Property->b
	"Trans Scroll Wheel"		Property->b
	"Text Indent"				Property->i
	"Auto Wrap"					Property->b
	"Long Hilight Bar"			Property->b
	"Frame mode"				Property->i
	"Frame Image"				Property->fn
	"Hilight Image"				Property->fn
	"Up Button Image"			Property->fn
	"Down Button Image"			Property->fn
	"Scroll Area Image"			Property->fn
	"Scroll Bar Image"			Property->fn
	"Plus Symbol Image"			Property->fn
	"Minus Symbol Image"		Property->fn
	"Leaf Symbol Image"			Property->fn
	"Leaf Check Symbol Image"	Property->fn
	AUIObject.Properties
	*/
	virtual bool GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual void SetColor(A3DCOLOR color);
	virtual bool IsInHitArea(int x, int y, UINT msg = WM_LBUTTONDOWN);
	virtual int GetRequiredHeight();
	virtual bool Resize();
	virtual void SetSize(int W, int H);
	virtual void SetDefaultSize(int W, int H);
	virtual bool SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
		A3DCOLOR dwColor, int nShadow = 0, int nOutline = 0,
		bool bBold = false, bool bItalic = false, bool bFreeType = true,
		A3DCOLOR clOutline = 0,
		A3DCOLOR clInnerText = A3DCOLORRGB(255, 255, 255),
		bool bInterpColor = false,
		A3DCOLOR clUpper = 0,
		A3DCOLOR clLower = 0);
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	int GetCount();
	P_AUITREEVIEW_ITEM GetSelectedItem();
	const VEC_AUITREEVIEW_ITEM & GetDisplayItems();
	void SetOnlyShowText(bool bShow);
	void SetAutoWrap(bool bAutoWarp);
	int GetItemIndent(P_AUITREEVIEW_ITEM pItem);
	DWORD GetItemState(P_AUITREEVIEW_ITEM pItem);
	A3DRECT GetItemRect(P_AUITREEVIEW_ITEM pItem);
	bool ItemHasChildren(P_AUITREEVIEW_ITEM pItem);
	const ACHAR * GetItemText(P_AUITREEVIEW_ITEM pItem);
	bool SetItemText(P_AUITREEVIEW_ITEM pItem, const ACHAR *pszText);
	DWORD GetItemData(P_AUITREEVIEW_ITEM pItem, AString strName = "");
	void SetItemData(P_AUITREEVIEW_ITEM pItem, DWORD dwData, AString strName = "");
	void * GetItemDataPtr(P_AUITREEVIEW_ITEM pItem, AString strName = "");
	void SetItemDataPtr(P_AUITREEVIEW_ITEM pItem, void *pvData, AString strName = "");
	UINT64 GetItemData64(P_AUITREEVIEW_ITEM pItem, AString strName = "");
	void SetItemData64(P_AUITREEVIEW_ITEM pItem, UINT64 uiData64, AString strName = "");
	UINT64 GetItemDataPtr64(P_AUITREEVIEW_ITEM pItem, AString strName = "");
	void SetItemDataPtr64(P_AUITREEVIEW_ITEM pItem, UINT64 uiDataPtr64, AString strName = "");
	A3DCOLOR GetItemTextColor(P_AUITREEVIEW_ITEM pItem);
	void SetItemTextColor(P_AUITREEVIEW_ITEM pItem, A3DCOLOR color);
	const ACHAR * GetItemHint(P_AUITREEVIEW_ITEM pItem);
	void SetItemHint(P_AUITREEVIEW_ITEM pItem, const ACHAR *pszHint);
	void SetFirstItem(int nFirstItem);
	void SetItemMask(int nMask)	{ m_nItemMask = nMask; }
	EditboxItemType GetItemMask() { return (EditboxItemType)m_nItemMask; }
	void SetImageList(abase::vector<A2DSprite*> *pvecImageList);

	void CheckLeafItem(P_AUITREEVIEW_ITEM pItem, bool bCheck);
	bool GetLeafItemCheck(P_AUITREEVIEW_ITEM pItem);

	P_AUITREEVIEW_ITEM GetRootItem();
	P_AUITREEVIEW_ITEM GetParentItem(P_AUITREEVIEW_ITEM pItem);
	P_AUITREEVIEW_ITEM GetFirstChildItem(P_AUITREEVIEW_ITEM pItem);
	P_AUITREEVIEW_ITEM GetPrevSiblingItem(P_AUITREEVIEW_ITEM pItem);
	P_AUITREEVIEW_ITEM GetNextSiblingItem(P_AUITREEVIEW_ITEM pItem);
	P_AUITREEVIEW_ITEM GetItemByIndexArray(int nDepth, int *pIndexArray);

	bool DeleteAllItems();
	bool DeleteItem(P_AUITREEVIEW_ITEM pItem, bool bRebuildItems = true);
	P_AUITREEVIEW_ITEM InsertItem(const ACHAR *pszText, P_AUITREEVIEW_ITEM pParent = AUITREEVIEW_ROOT, P_AUITREEVIEW_ITEM pAfter = AUITREEVIEW_LAST);
	bool MoveItem(P_AUITREEVIEW_ITEM pItem, P_AUITREEVIEW_ITEM pParent = AUITREEVIEW_ROOT, P_AUITREEVIEW_ITEM pAfter = AUITREEVIEW_LAST);
	void SelectItem(P_AUITREEVIEW_ITEM pItem);
	void Expand(P_AUITREEVIEW_ITEM pItem, int idAction, bool bRecursive = false);
	void BlinkItem(P_AUITREEVIEW_ITEM pItem, bool bBlink);
	P_AUITREEVIEW_ITEM HitTest(int x, int y, bool *pbOnIcon = NULL);	// x and y is local coordination.
	void EnsureVisible(int nIndex);

	int GetFirstItem(){ return m_nFirstItem;}

	int GetHitArea(int x, int y);

	abase::vector<AUITEXTAREA_NAME_LINK> & GetNameLink() {return m_vNameLinks;};
	abase::vector<AUITEXTAREA_EDITBOX_ITEM>& GetEditBoxItems() { return m_vEBItems; }

protected:
	bool m_bTextTransparent;
	bool m_bTransScrollWheel;
	bool m_bOnlyShowText;
	bool m_bAutoWrap;
	bool m_bLongHilightBar;
	bool m_bRenderVScroll;
	bool m_bRenderHScroll;
	int m_nState;
	POINT m_ptVBar;
	POINT m_ptHBar;
	bool m_bDraggingVScroll;
	bool m_bDraggingHScroll;
	int m_nDragDist;
	int m_nFirstItem;
	int m_nScrollLines;
	int m_nRealLines;
	int m_nHBarPos;
	int m_nHBarMax;
	DWORD m_nScrollDelay;
	int	m_nItemMask;
	int m_nItemIndent, m_nItemHeight;
	int m_nTextIndent;
	VEC_AUITREEVIEW_ITEM m_vecVisibleItems;
	VEC_AUITREEVIEW_ITEM m_vecDisplayItems;
	abase::vector<A2DSprite*> *m_pvecImageList;

	int m_nCount;
	AUITREEVIEW_ITEM m_RootItem;
	P_AUITREEVIEW_ITEM m_pSelItem;

	int m_nFrameMode;
	PAUIFRAME m_pAUIFrame;
	A2DSprite *m_pA2DSprite[AUITREEVIEW_RECT_NUM];
	A3DRECT m_rcText;

	abase::vector<AUITEXTAREA_NAME_LINK> m_vNameLinks;
	abase::vector<AUITEXTAREA_EDITBOX_ITEM> m_vEBItems;

	bool RenderVScroll();
	bool RenderHScroll();
	bool RebuildVisibleItems();
	bool AddVisibleItem(P_AUITREEVIEW_ITEM pItem);
	void SetChildrenColor(P_AUITREEVIEW_ITEM pItem, A3DCOLOR color);
	void SetHBarPos(int nPos);

	int  GetLastVisibleItem();
	void ClearItemRcInfo();
};

typedef AUITreeView * PAUITREEVIEW;

#endif	//	_AUITREEVIEW_H_