#ifndef _FWMANAGER_H_
#define _FWMANAGER_H_

class FWDoc;
class FWView;
class A3DGFXExMan;
class A3DEngine;
class A3DDevice;
typedef unsigned long DWORD;

class FWManager  
{
	FWDoc * m_pDoc;
	FWView * m_pView;
	bool m_bInited;
public:
	FWManager();
	virtual ~FWManager();

	bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice);
	void Release();
	void Tick(DWORD dwTickTime);
	void Render(A3DGFXExMan *pGfxExMan);
	
	void SetVisible(bool bVisible);
	bool IsVisible();

	FWDoc * GetDoc();
	FWView * GetView();
};

bool AfxInitFWEditor(A3DDevice *pDevice);
void AfxReleaseFWEditor();

#endif 