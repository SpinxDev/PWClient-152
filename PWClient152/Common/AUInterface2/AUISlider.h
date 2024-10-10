// Filename	: AUISlider.h
// Creator	: Tom Zhou
// Date		: June 14, 2004
// Desc		: AUISlider is the class of progres control.

#ifndef _AUISLIDER_H_
#define _AUISLIDER_H_

#include "AUIObject.h"
#include "AUIFrame.h"

#define AUISLIDE_MIN	0
#define AUISLIDE_MAX	100

class AUISlider : public AUIObject
{
public:
	AUISlider();
	virtual ~AUISlider();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool InitIndirect(const char *pszFileName, int idType);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	virtual A3DRECT GetRect(void);
	/* support properties
	"Frame Mode"			Property->i
	"Frame Image File"		Property->fn
	"Bar Image File"		Property->fn
	"Total Levels"			Property->i
	"Vertical Slider"		Property->b
	AUIObject.Properties
	*/
	virtual bool GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	int GetLevel(void);
	void SetLevel(int nLevel);
	int GetBegin(void);
	int GetTotal(void);
	void SetTotal(int nTotal, int nBegin = AUISLIDE_MIN);
	void SetDragging(bool bDragging, int nDragDist = -1);
	bool IsVerticalSlider();
	void SetVerticalSlider(bool bVertical);

protected:
	int m_nLevel;
	int m_nBegin;
	int m_nTotal;
	int m_nDragDist;
	A3DRECT m_rcBar;
	bool m_bDragging;
	int m_nFrameMode;
	PAUIFRAME m_pAUIFrame;
	A2DSprite * m_pA2DSprite;
	bool m_bVertical;
};

typedef AUISlider * PAUISLIDER;

#endif //_AUISLIDER_H_