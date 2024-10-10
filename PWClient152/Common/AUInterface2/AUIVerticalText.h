// Filename	: AUIVerticalText.h
// Creator	: Xiao Zhou
// Date		: 2009/11/30
// Desc		: AUIVerticalText is the class of vertical text control.

#ifndef _AUIVERTICALTEXT_H_
#define _AUIVERTICALTEXT_H_

#include "AUIObject.h"
#include "AUITextArea.h"

enum
{
	AUIVERTICALTEXT_RECT_MIN = 0,
	AUIVERTICALTEXT_RECT_FRAME = AUIVERTICALTEXT_RECT_MIN,
	AUIVERTICALTEXT_RECT_LEFT,
	AUIVERTICALTEXT_RECT_RIGHT,
	AUIVERTICALTEXT_RECT_SCROLL,
	AUIVERTICALTEXT_RECT_BAR,
	AUIVERTICALTEXT_RECT_MAX
};
#define AUIVERTICALTEXT_RECT_NUM			AUIVERTICALTEXT_RECT_MAX

#define AUIVERTICALTEXT_STATE_NORMAL		0
#define AUIVERTICALTEXT_STATE_PRESSLEFT		1
#define AUIVERTICALTEXT_STATE_PRESSRIGHT	2
#define AUIVERTICALTEXT_STATE_PRESSBAR		3

class AUIVerticalText : public AUIObject
{
public:
	AUIVerticalText();
	virtual ~AUIVerticalText();

	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool InitIndirect(const char *pszFileName, int idType);
	virtual bool Release(void);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	/* support properties	
	  "Align"				Property->i
	  "Up Scroll Bar"		Property->b
	  "Left To Right"		Property->b
	  "Text Content"		Property->s
	  "Frame mode"			Property->i 
	  "Frame Image"			Property->fn
	  "Left Button Image"	Property->fn
	  "Right Button Image"	Property->fn
	  "Scroll Area Image"	Property->fn
	  "Scroll Bar Image"	Property->fn
	AUIObject.Properties
	*/
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual void SetText(const ACHAR *pszText);
	virtual void SetSize(int W, int H);
	virtual void SetDefaultSize(int W, int H);
	virtual void SetLineSpace(int nSpace);
	virtual bool SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
		A3DCOLOR dwColor, int nShadow = 0, int nOutline = 0,
		bool bBold = false, bool bItalic = false, bool bFreeType = true,
		A3DCOLOR clOutline = 0,
		A3DCOLOR clInnerText = A3DCOLORRGB(255, 255, 255),
		bool bInterpColor = false,
		A3DCOLOR clUpper = 0,
		A3DCOLOR clLower = 0);
	static bool RegisterPropertyType();
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	void CalcTextLines();
	abase::vector<AUITEXTAREA_EDITBOX_ITEM>& GetEditBoxItems() { return m_EditBoxItems; }

protected:
	int	m_nItemMask;
	int m_nFontWidth;
	int m_nTotalLines;
	int m_nFirstLine;
	int m_nLinesPP;
	int m_nDragDist;
	DWORD m_nScrollDelay;
	bool m_bDragging;
	bool m_bUpScrollBar;
	bool m_bLeftToRight;
	POINT m_ptBar;
	
	EditBoxItemsSet m_ItemsSet;
	abase::vector<AUITEXTAREA_EDITBOX_ITEM> m_EditBoxItems;

	int m_nFrameMode;
	PAUIFRAME m_pAUIFrame;
	A2DSprite *m_pA2DSprite[AUIVERTICALTEXT_RECT_NUM];
	abase::vector<int> m_vecLines;

	bool RenderScroll();
};

typedef AUIVerticalText * PAUIVERTICALTEXT;

#endif //_AUIVerticalText_H_