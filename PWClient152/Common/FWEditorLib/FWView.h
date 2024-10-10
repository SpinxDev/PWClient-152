#ifndef _FWVIEW_H_
#define _FWVIEW_H_


#include "CodeTemplate.h"
#include "FWConfig.h"
#include <APoint.h>
#include <ARect.h>

class FWDoc;
class FWState;
class FWDialogBoard;
class FWAUIManager;
class FWGlyph;
class FWDialogPropBase;
class A3DDevice;
class A3DEngine;
class A3DViewport;
class A3DCamera;
class FWFlatCollector;
class FWStream;
class FWPlayer;
class A3DGFXExMan;

// FWView is responsible of process all user 
// input. FWAUIManager and FWDialogxxx can be
// considered as a part of FWView, client should
// not access them directly
class FWView  
{
	friend class FWAUIManager;
	friend class FWDialogTool;
	friend class FWDoc;
	friend class FWManager;

protected:
	// current state object
	FWState * m_pCurrentState;

	// format of clipboard, it will be inited by Init()
	UINT m_uClipboardFormat;

	// whether to show the grid on board
	bool m_bShowGrid;

	// pointer to attached doc
	FWDoc * m_pDoc;

	// UI manager
	FWAUIManager * m_pAUIManager;

	// pointer to A3D Device
	A3DDevice *m_pA3DDevice; 
	// pointer to A3d Engine
	A3DEngine *m_pA3DEngine; 
	// A3D camera used for preview
	A3DCamera * m_pA3DCameraPreview; 
	// A3D viewport used for preview
	A3DViewport * m_pA3DViewportPreview; 
	// used for some small line/triangle drawing
	FWFlatCollector * m_pFlatCollecter;
	// used for swithing between main/preview viewport viewport
	A3DViewport * m_pOldViewport; 
	// stream object for glyph drawing
	FWStream * m_pStream;
	// helper for play the final gfx set
	FWPlayer * m_pPlayer;
	// is visible
	bool m_bVisible;
	// is initialized
	bool m_bInited;
protected:
	// internal used property-get
	FWDialogBoard * GetBoard();
	FWAUIManager * GetFWAUIManager();
	FWState * GetCurrentState() { return m_pCurrentState; }

	// show prop dialog for a glyph
	void InnerShowPropDlg(FWDialogPropBase *pDlg, FWGlyph *pGlyph);
	// callback for InnerShowPropDlg()
	static void OnClosePropDlg(void *pData);

	FWView();
	virtual ~FWView();
public:
	void ShowContextMenu(FWGlyph *pGlyph, APointI point);
	// init
	bool Init(A3DEngine *pEngine, A3DDevice *pDevice);

	// release
	void Release();

	A3DEngine * GetA3DEngine() { return m_pA3DEngine; }
	A3DDevice * GetA3DDevice() { return m_pA3DDevice; }
	void SwitchToMainViewport();
	void SwitchToPreviewViewport();
	A3DViewport * GetActiveViewport();
	void PreparePreviewViewport(ARectI rect);
	FWStream * GetFWStream() { return m_pStream; }
	FWFlatCollector * GetFlatCollector() { return m_pFlatCollecter; }
	FWPlayer * GetPlayer() { return m_pPlayer; }

	FWDialogBase * GetFWDialog(const char *szDlgName);

	virtual bool DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// show prop dialog for the given glyph
	void ShowPropDlg(FWGlyph *pGlyph);

	// grid control helper
	// just some property-get/set
	bool IsShowGrid();
	void SetShowGrid(bool bShow);

	// convertion between device coordinates and logic coodinate
	// note : the device coordinate is he game window coordinate in face.
	// the logic coordiantes' origin is at the center of the board dialog
	// and its axises have the same direction as windows
	void DPtoLP(float &x, float &y);
	void DPtoLP(APointI * lpPoint, int nCount = 1);
	void DPtoLP(ARectI * lpRect);
	void LPtoDP(float &x, float &y);
	void LPtoDP(APointI * lpPoint, int nCount = 1);
	void LPtoDP(ARectI * lpRect);

	// return the format id of clipboard format
	// this function will create the format first if it does not exist
	int GetClipboardFormat();

	// return HWND of the game window
	HWND GetHwnd();

	// tick
	void Tick(DWORD dwTickTime);

	// same to render in fweditor
	void OnDraw(A3DGFXExMan * pGfxExMan);

	FWDoc * GetDocument();

	// switch state of the application
	void ChangeState(FWState* pNewState);

	// get dc of the game window
	HDC GetDC();

	// return the client rect of drawing borad in device coordinates
	ARectI GetClientRect(void);
	// return the client rect of drawing borad in logic coordiantes
	ARectI GetClientRectLP(void);

	// set capture for drawing board
	void SetCapture();
	// relase any capture
	bool ReleaseCapture();
	// return pointer to the view if the board have capture
	// or return NULL
	FWView * GetCapture();

	void SetVisible(bool bVisible);
	bool IsVisible();

	// message handler
	UINT SetTimer(UINT nIDEvent, UINT nElapse);
	bool KillTimer(int nIDEvent);
	void OnTimer(UINT nIDEvent);
	void OnRButtonUp(UINT nFlags, APointI point);
	void OnRButtonDown(UINT nFlags, APointI point);
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnLButtonDown(UINT nFlags, APointI point);
	void OnLButtonUp(UINT nFlags, APointI point);
	void OnMouseMove(UINT nFlags, APointI point);
	void OnLButtonDblClk(UINT nFlags, APointI point);
	void MoveCamera(UINT nChar, UINT nRepCnt, UINT nFlags);

};

#endif 