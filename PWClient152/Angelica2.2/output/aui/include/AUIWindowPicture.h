// Filename	: AUIWindowPicture.h
// Creator	: Xiao Zhou
// Date		: 2008/11/27
// Desc		: AUIWindowPicture is the class of desktop window control.

#ifndef _AUIWINDOWPICTURE_H_
#define _AUIWINDOWPICTURE_H_

#include "AUIEditbox.h"
#include "wndthreadmanager.h"
class A2DSprite;

class CPWMessageTranslate
{
public:
	virtual bool MessageTranslate(AString Control, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
private:
};
class AUIWindowPicture : public AUIEditBox
{
public:
	AUIWindowPicture();
	virtual ~AUIWindowPicture();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	/* support properties	
	  "Draw External"		Property->b
	AUIObject.Properties
	*/
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	void SetHWnd(HWND hWnd);
	HWND GetHWnd();
	void SetBkTransparent(bool bTransparent);
	A2DSprite * GetImage();
	float GetScale();
	void ClearTexture();
	void SetDisplayRect(bool bFullWindow, A3DRECT rcRect);
	void SetMessageHandler(CPWMessageTranslate* handler){p_MessageTranslater = handler;};
	void SetWndThreadManager(CWndThreadManager* handler){p_WndThreadManager = handler;};
	int  GetOffX(){return m_nOffX;};
	int  GetOffY(){return m_nOffY;};
	void SetExternalDraw(bool bExternalDraw);
	bool IsExternalDraw();
protected:
	CPWMessageTranslate *p_MessageTranslater;
	CWndThreadManager   *p_WndThreadManager;
	A2DSprite *m_pA2DSprite;
	HWND m_hWnd;
	float m_fScale;
	A3DRECT m_rcDisplayRect;
	bool m_bBkTransparent;
	bool m_bExternalDraw;
};

typedef AUIWindowPicture * PAUIWINDOWPICTURE;

#endif //_AUIWINDOWPICTURE_H_