// Filename	: AUIStillImageButton.h
// Creator	: Tom Zhou
// Date		: May 18, 2004
// Desc		: AUIStillImageButton is the class of still image button control.

#ifndef _AUISTILLIMAGEBUTTON_H_
#define _AUISTILLIMAGEBUTTON_H_

#include "AUIFrame.h"
#include "AUIObject.h"

#define AUISTILLIMAGEBUTTON_STATE_NORMAL	0
#define AUISTILLIMAGEBUTTON_STATE_CLICK		2

enum
{
	AUISTILLIMAGEBUTTON_FRAME_UP,
	AUISTILLIMAGEBUTTON_FRAME_DOWN,
	AUISTILLIMAGEBUTTON_FRAME_ONHOVER,
	AUISTILLIMAGEBUTTON_FRAME_DISABLED
};

class A3DGFXEx;

class AUIStillImageButton : public AUIObject
{
public:
	AUIStillImageButton();
	virtual ~AUIStillImageButton();

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
	"Frame Mode"			Property->i
	"Up Frame File"			Property->fn
	"Down Frame File"		Property->fn
	"OnHover Frame File"	Property->fn
	"Disabled Frame File"	Property->fn
	"Down Gfx File"			Property->fn
	"OnHover Gfx File"		Property->fn
	"Text Offset X"			Property->i 
	"Text Offset Y"			Property->i
	"Image Degree"			Property->i
	"Vertical Text"			Property->b
	AUIObject.Properties
	*/
	virtual bool GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual bool IsInHitArea(int x, int y, UINT msg = WM_LBUTTONDOWN);
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	PAUIFRAME GetFrame(int type = AUISTILLIMAGEBUTTON_FRAME_UP);
	void SetPushLike(bool bPushLike);
	bool IsPushed();
	void SetPushed(bool bPushed);
	bool SetGfxDown(const char* szGfx);
	bool SetGfxOnHover(const char* szGfx);
	A3DGFXEx* GetGfxDown();
	A3DGFXEx* GetGfxOnHover();
	void StartOnHoverGfx();
	void SetVerticalText(bool bVertical);
	bool IsVerticalText() const;

protected:
	struct sImageRotate 
	{
		sImageRotate();
		int			nDegree;
		int			nWidth;
		int			nHeight;
		A2DSprite*  pSprite[4];
	};

protected:
	void SetImageDegree(const int nDegree);
	void UpdateImageDegree();

protected:
	bool m_bHover;
	bool m_bVerticalText;
	bool m_bPushed;
	bool m_bPushLike;
	int m_nFrameMode;
	int	m_nTextOffsetX, m_nTextOffsetY;
	PAUIFRAME m_pAUIFrameUp, m_pAUIFrameDown, m_pAUIFrameOnHover, m_pAUIFrameDisabled;
	A3DGFXEx *m_pGfxDown, *m_pGfxOnHover;
	AString m_strGfxDown, m_strGfxOnHover;
	sImageRotate	m_imageRotate;

	void SetHover(bool bHover);
};

typedef AUIStillImageButton * PAUISTILLIMAGEBUTTON;

#endif //_AUISTILLIMAGEBUTTON_H_