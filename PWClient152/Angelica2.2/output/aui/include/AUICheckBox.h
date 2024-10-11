// Filename	: AUICheckBox.h
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUICheckBox is the class of checkbox control.

#ifndef _AUICHECKBOX_H_
#define _AUICHECKBOX_H_

#include "AUIObject.h"

class A2DSprite;

enum
{
	AUICHECKBOX_ITEM_MIN = 0,
	AUICHECKBOX_ITEM_NORMAL = AUICHECKBOX_ITEM_MIN,
	AUICHECKBOX_ITEM_CHECKED,
	AUICHECKBOX_ITEM_ONHOVER,
	AUICHECKBOX_ITEM_CHECKEDONHOVER,
	AUICHECKBOX_ITEM_MAX
};
#define AUICHECKBOX_NUMITEMS	AUICHECKBOX_ITEM_MAX

class AUICheckBox : public AUIObject
{
public:
	AUICheckBox();
	virtual ~AUICheckBox();

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
	"Align"							Property->i
	"Normal Image File"				Property->fn
	"Checked Image File"			Property->fn
	"OnHover Image File"			Property->fn
	"CheckedOnHover Image File"		Property->fn
	"Text Offset X"					Property->i
	"Text Offset Y"					Property->i
	"Vertical Text"					Property->b
	And AUIObject's properties
	*/
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual bool IsInHitArea(int x, int y, UINT msg = WM_LBUTTONDOWN);
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	bool IsChecked(void);
	void Check(bool bCheck);
	bool IsVerticalText() const;
	void SetVerticalText(bool bVertical);

protected:
	bool m_bHover;
	bool m_bCheck;
	bool m_bVerticalText;
	int	m_nTextOffsetX, m_nTextOffsetY;
	A2DSprite *m_pA2DSpriteNormal, *m_pA2DSpriteChecked, *m_pA2DSpriteOnHover, *m_pA2DSpriteCheckedOnHover;

	void SetHover(bool bHover);
	void CalcAlignPos(A2DSprite* pA2DSprite, POINT& ptText, POINT& ptSprite);
	A2DSprite* GetRenderSprite();
	A3DCOLOR GetRenderColor();
};

typedef AUICheckBox * PAUICHECKBOX;

#endif //_AUICHECKBOX_H_