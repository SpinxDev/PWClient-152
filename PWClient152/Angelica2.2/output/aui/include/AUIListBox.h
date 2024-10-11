// Filename	: AUIListBox.h
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIListBox is the class of list box control.

#ifndef _AUILISTBOX_H_
#define _AUILISTBOX_H_

#include "vector.h"

#include "AAssist.h"
#include "AUICommon.h"
#include "AUIObject.h"
#include "AUIFrame.h"

#define AUILISTBOX_ERROR			(-1)

enum
{
	AUILISTBOX_RECT_MIN = 0,
	AUILISTBOX_RECT_FRAME = AUILISTBOX_RECT_MIN,
	AUILISTBOX_RECT_TEXT = AUILISTBOX_RECT_FRAME,
	AUILISTBOX_RECT_ITEM,
	AUILISTBOX_RECT_HILIGHT,
	AUILISTBOX_RECT_UP,
	AUILISTBOX_RECT_DOWN,
	AUILISTBOX_RECT_VSCROLL,
	AUILISTBOX_RECT_VBAR,
	AUILISTBOX_RECT_LEFT,
	AUILISTBOX_RECT_RIGHT,
	AUILISTBOX_RECT_HSCROLL,
	AUILISTBOX_RECT_HBAR,
	AUILISTBOX_RECT_MAX
};
#define AUILISTBOX_RECT_NUM			AUILISTBOX_RECT_MAX

#define AUILISTBOX_STATE_NORMAL		0
#define AUILISTBOX_STATE_PRESSUP	1
#define AUILISTBOX_STATE_PRESSDOWN	2
#define AUILISTBOX_STATE_PRESSVBAR	3
#define AUILISTBOX_STATE_PRESSLEFT	4
#define AUILISTBOX_STATE_PRESSRIGHT	5
#define AUILISTBOX_STATE_PRESSHBAR	6

#define AUILISTBOX_MAX_COLUMN		20

enum
{
	AUILISTBOX_SORT_ASCENT = 0,
	AUILISTBOX_SORT_DESCENT
};

enum
{
	AUILISTBOX_SORTTYPE_UNISTRING = 0,
	AUILISTBOX_SORTTYPE_STRING,
	AUILISTBOX_SORTTYPE_INT,
	AUILISTBOX_SORTTYPE_FLOAT
};

enum
{
	AUILISTBOX_DATASORTTYPE_DWORD = 0,
	AUILISTBOX_DATASORTTYPE_INT,
};

#define AUILISTBOX_DATA_NUM		AUILISTBOX_MAX_COLUMN

typedef struct _AUILISTBOX_ITEM
{
	ACString strText;
	int nLines;
	bool bSelState;
	A3DCOLOR color[AUILISTBOX_MAX_COLUMN];
	ACString strHint;
	ACString strColHint[AUILISTBOX_MAX_COLUMN];
	DWORD dwData[AUILISTBOX_DATA_NUM];
	void *pvData[AUILISTBOX_DATA_NUM];
	AString strDataName[AUILISTBOX_DATA_NUM];
	AString strDataPtrName[AUILISTBOX_DATA_NUM];
	UINT64 uiData64[AUILISTBOX_DATA_NUM];
	UINT64 uiDataPtr64[AUILISTBOX_DATA_NUM];
	AString strData64Name[AUILISTBOX_DATA_NUM];
	AString strDataPtr64Name[AUILISTBOX_DATA_NUM];
	EditBoxItemsSet itemsSet;
} AUILISTBOX_ITEM, * P_AUILISTBOX_ITEM;

class AUIComboBox;

class AUIListBox : public AUIObject
{
public:
	AUIListBox();
	virtual ~AUIListBox();
	
	void SetSel(int nIndex, bool bSel);

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
	"Frame mode"			Property->i
	"Frame Image"			Property->fn
	"Hilight Image"			Property->fn
	"Up Button Image"		Property->fn
	"Down Button Image" 	Property->fn
	"Scroll Area Image"		Property->fn
	"Scroll Bar Image"		Property->fn
	"Item Image"			Property->fn
	"Auto Wrap"				Property->b
	"Horizontal Scroll"		Property->b 
	"Hover Hilight"			Property->b
	"Multiple Selection"	Property->b
	"Multiple Column Info"	Property->c
	"Row Fixed Height"	Property->i
	AUIObject.Properties
	*/
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual void SetColor(A3DCOLOR color);
	static bool RegisterPropertyType();
	virtual	int GetRequiredWidth();
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

	int AddString(const ACHAR *pszString);
	int InsertString(int nIndex, const ACHAR *pszString);
	int DeleteString(int nIndex);
	int GetCount(void);
	int GetCurSel(void);
	int GetCurColSel(void);
	void SetCurSel(int nIndex);
	int GetSel(int nIndex);
	const ACHAR * GetText(int nIndex);
	int SetText(int nIndex, const ACHAR *pszText);
	A3DCOLOR GetItemTextColor(int nIndex, int nItem = 0);
	int SetItemTextColor(int nIndex, A3DCOLOR color, int nItem = -1);
	void ResetContent(void);
	bool IsMultipleSelection(void);
	int GetSelCount(void);
	int GetSelItems(int nMaxItems, int *a_nIndex);
	int GetNumColumns(void);
	int GetHOverItem();
	int GetHOverItemCol();
	int GetHitArea(int x, int y);
	void * GetItemDataPtr(int nIndex, int nSubIndex = 0, AString strName = "");
	int SetItemDataPtr(int nIndex, void *pData, int nSubIndex = 0, AString strName = "");
	DWORD GetItemData(int nIndex, int nSubIndex = 0, AString strName = "");
	int SetItemData(int nIndex, DWORD dwItemData, int nSubIndex = 0, AString strName = "");
	UINT64 GetItemDataPtr64(int nIndex, int nSubIndex = 0, AString strName = "");
	int SetItemDataPtr64(int nIndex, UINT64 uiDataPtr64, int nSubIndex = 0, AString strName = "");
	UINT64 GetItemData64(int nIndex, int nSubIndex = 0, AString strName = "");
	int SetItemData64(int nIndex, UINT64 uiData64, int nSubIndex = 0, AString strName = "");
	const ACHAR * GetItemHint(int nIndex);
	int SetItemHint(int nIndex, const ACHAR *pszHint);
	const ACHAR * GetItemColHint(int nIndex, int nColumn);
	bool SetItemColHint(int nIndex, int nColumn, const ACHAR *pszHint);
	bool SortItemsByData(int nMode, int nSortType = AUILISTBOX_DATASORTTYPE_DWORD, int nColoum = 0);
	bool SortItems(int nMode, int nSortType = AUILISTBOX_SORTTYPE_UNISTRING, int nColumn = 0);
	bool BringItemToTop(int nItem);
	void SetHOver(bool bHOver);
	void SetForceRenderScroll(bool bForce);
	void SetAutoWrap(bool bAuto);
	AUIComboBox* GetComboBox() { return m_pComboBox; }
	void SetComboBox(AUIComboBox *pComboBox);
	void EnsureVisible(int nIndex);
	void SetColAlign(int nColumn, int nAlign);

	int	 GetFirstItem(){ return m_nFirstItem;}
	void SetFirstItem(int nFirstItem);
	void SetItemMask(int nMask)	{ m_nItemMask = nMask; }
	EditboxItemType GetItemMask() { return (EditboxItemType)m_nItemMask; }
	void SetImageList(abase::vector<A2DSprite*> *pvecImageList);

	void EnableHScroll(bool bEnable=true);
	int  GetScrollLines(){ return m_nScrollLines; }

	// image in item text
	void SetItemTextSmallImage(bool bSmallImage) {m_bItemTextSmallImage = bSmallImage;}
	bool IsItemTextSmallImage() const {return m_bItemTextSmallImage;}

	abase::vector<AUITEXTAREA_NAME_LINK> & GetNameLink() {return m_vNameLinks;};
	abase::vector<AUITEXTAREA_EDITBOX_ITEM>& GetEditBoxItems() { return m_vEBItems; }

protected:
	bool m_bForceRenderScroll;
	bool m_bHOver;
	bool m_bAutoWrap;
	bool m_bEnableHScroll;
	int m_nLinesCurPage;
	POINT m_ptVBar;
	POINT m_ptHBar;
	int m_nDragDist;
	int m_nCurSel;
	int m_nCurColSel;
	int m_nHOverItem;
	int m_nHOverItemCol;
	bool m_bDraggingVScroll;
	bool m_bDraggingHScroll;
	bool m_bRenderHScroll;
	bool m_bRenderVScroll;
	int m_nFirstItem;
	int	m_nScrollLines;
	int m_nHBarMax;
	int m_nHBarPos;
	DWORD m_nScrollDelay;
	int	m_nItemMask;
	bool m_bItemTextSmallImage;
	abase::vector<AUILISTBOX_ITEM> m_Item;
	abase::vector<A2DSprite*> *m_pvecImageList;

	int m_nRowMaxWidth;
	int m_nRowFixedHeight;

	// For multiple column.
	int m_nNumColumns;
	AString m_strCols;
	int	m_nColAlign[AUILISTBOX_MAX_COLUMN];
	int m_nColWidth[AUILISTBOX_MAX_COLUMN];
	bool BuildColumnsInfo(const char *pszColInfo);

	// For multiple selection.
	bool m_bMultipleSelection;
	int m_nNumSelections;

	int m_nFrameMode;
	PAUIFRAME m_pAUIFrame;
	AUIComboBox *m_pComboBox;
	A2DSprite *m_pA2DSprite[AUILISTBOX_RECT_NUM];
	A3DRECT m_rcImage[AUILISTBOX_RECT_NUM];
	
	abase::vector<AUITEXTAREA_NAME_LINK> m_vNameLinks;
	abase::vector<AUITEXTAREA_EDITBOX_ITEM> m_vEBItems;

	bool RenderVScroll(void);
	bool RenderHScroll(void);
	void UpdateScroll(int index = -1);

	void BatchSetSel(int nStart, int nEnd, bool bSel);

	void CalcItemLines(int nIndex);
	void CalcNoWrapScroll(bool bCalcAllLines = true);
	
	int  GetItemWidth(int nIndex);
	void CalcWrapScroll(bool bCalcAllLines = true);
	
	float GetLineHeight();
	int	 GetLinesPerPage();

	void SetHBarPos(int nPos);
	void ClearItemRcInfo();
};

typedef AUIListBox * PAUILISTBOX;

#endif //_AUILISTBOX_H_