// Filename	: AUIImagePicture.h
// Creator	: Tom Zhou
// Date		: May 19, 2004
// Desc		: AUIImagePicture is the class of still image and animation control.

#ifndef _AUIIMAGEPICTURE_H_
#define _AUIIMAGEPICTURE_H_

#include "AUIPicture.h"
#include "AUIClockIcon.h"

class A2DSprite;
class A3DTexture;
class A3DGFXEx;

#define AUIIMAGEPICTURE_MAXFRAMES	256
#define AUIIMAGEPICTURE_MAXCOVERS	8	

class AUIImagePicture : public AUIPicture
{
public:
	typedef void (* LPFRENDERCALLBACK)(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);

public:
	AUIImagePicture();
	virtual ~AUIImagePicture();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool InitIndirect(const char *pszFileName, int idType);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual void Show(bool bShow);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual bool Resize();
	virtual void OptimizeSpriteResource(bool bLoad);
	virtual bool IsInHitArea(int x, int y, UINT msg = WM_LBUTTONDOWN);

	inline void SetRenderTop(bool bRenderTop) { m_bRenderTop = bRenderTop; }
	inline bool GetRenderTop() { return m_bRenderTop; }
	inline A2DSprite * GetImage() { return m_pA2DSprite; }
	bool SetGfx(const char* szGfx);
	bool SetClock(const char* szClock);
	inline A3DGFXEx* GetGfx() { return m_pGfx; }
	void StartGfx();
	AUIClockIcon * GetClockIcon();
	void FixFrame(int nIndex);
	inline int GetFixFrame() { return m_nFixFrame; }
	void ClearCover();
	void GetCover(A2DSprite **ppA2DSprite, int *pnItem, int nLayer = 0);
	void SetCover(A2DSprite *pA2DSprite, int nItem, int nLayer = 0);
	inline void SetClockLayer(int nLayer) { m_nClockLayer = nLayer; }
	inline void FadeInOut(int nInterval) { m_nFadeInterval = nInterval; }
	void ClearTexture();
	void GetTexture(A3DTexture **ppA3DTexture);
	// if Texture is a RenderTarget (ex. generated from AUIDialog::RenderRTToTexture(..)), alpha blend is different.
	void SetTexture(A3DTexture *pA3DTexture, bool bIsRenderTarget = false);		
	void SetRenderCallback(LPFRENDERCALLBACK pCallback,
		DWORD param1, DWORD param2 = 0, DWORD param3 = 0);
	bool InitIndirectFromMemory(const char *pszFileName, const void *pBuf, int nLen);
	bool SetFrameNumber(int numFrame);

protected:
	struct sImageRotate 
	{
		sImageRotate();
		int			nDegree;
		int			nWidth;
		int			nHeight;
		A2DSprite*  pSprite;
		int			iOffsetX;
		int			iOffsetY;
	};

protected:
	void SetImageDegree(const int nDegree);
	void UpdateImageDegree();
	
protected:
	A2DSprite *m_pA2DSprite;
	AString m_strClock;
	AString m_strGfx;
	A3DGFXEx* m_pGfx;	
	AUIClockIcon m_ClockCounter;
	bool m_bRenderTop;
	int m_nClockLayer;
	int m_nInterval, m_nLastTime;
	int m_nFrameNow, m_nNumFrames, m_nFixFrame;

	int m_nItem[AUIIMAGEPICTURE_MAXCOVERS];
	A2DSprite *m_pA2DSpriteCover[AUIIMAGEPICTURE_MAXCOVERS];
	A3DTexture *m_pA3DTexture;
	bool m_bTextureIsRenderTarget;

	DWORD m_dwParamCallback1;
	DWORD m_dwParamCallback2;
	DWORD m_dwParamCallback3;
	LPFRENDERCALLBACK m_pRenderCallback;

	int				m_nFadeInterval;
	sImageRotate	m_imageRotate;
};

typedef AUIImagePicture * PAUIIMAGEPICTURE;

#endif //_AUIIMAGEPICTURE_H_