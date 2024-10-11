// Filename	: AUIScroll.h
// Creator	: Xiao Zhou
// Date		: May 24, 2005
// Desc		: AUIScroll is the class of scroll control.

#ifndef _AUISCROLL_H_
#define _AUISCROLL_H_

#include "AUIObject.h"
#include "AUIFrame.h"

enum
{
	AUISCROLL_RECT_MIN = 0,
	AUISCROLL_RECT_SCROLL = AUISCROLL_RECT_MIN,
	AUISCROLL_RECT_UP,
	AUISCROLL_RECT_DOWN,
	AUISCROLL_RECT_BAR,
	AUISCROLL_RECT_MAX
};
#define AUISCROLL_RECT_NUM			AUISCROLL_RECT_MAX

#define AUISCROLL_STATE_NORMAL			0
#define AUISCROLL_STATE_PRESSUP			1
#define AUISCROLL_STATE_PRESSDOWN		2
#define AUISCROLL_STATE_PRESSBAR		3
#define AUISCROLL_STATE_PRESSCHANGEPAGE	4

#define AUISCROLL_MIN	0
#define AUISCROLL_MAX	100

class AUIScroll : public AUIObject
{
public:
	AUIScroll();
	virtual ~AUIScroll();

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
	"Up Button Image"		Property->fn
	"Down Button Image"		Property->fn
	"Scroll Area Image"		Property->fn
	"Frame mode"			Property->i
	"Scroll Bar Image"		Property->fn
	AUIObject.Properties
	*/
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	int GetScrollStep(void);
	void SetScrollStep(int nStep);
	int GetBarLength(void);
	void SetBarLength(int nLength);
	bool IsZoomScroll();
	void SetZoomScroll(bool bZoom);
	int GetBarLevel(void);
	void SetBarLevel(int nLevel);
	int GetScrollBegin(void);
	int GetScrollEnd(void);
	void SetScrollRange(int nBegin, int nEnd);
	void SetBarColor(A3DCOLOR color);

protected:
	int m_nFrameMode;
	PAUIFRAME m_pBarFrame;
	POINT m_ptBar;
	int m_nScrollStep;
	int m_nDesLevel;
	int m_nBarHeight;
	bool m_bVScroll;
	bool m_bZoomScroll;
	int m_nBarLength;
	DWORD m_nScrollDelay;
	int m_nBarLevel;
	int m_nScrollBegin;
	int m_nScrollEnd;
	int m_nDragDist;
	A3DRECT m_rcBar;
	bool m_bDragging;
	A2DSprite *m_pA2DSprite[AUISCROLL_RECT_NUM];

	A3DCOLOR m_clrBar;
};

typedef AUIScroll * PAUISCROLL;

#endif //_AUIScroll_H_