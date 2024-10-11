// Filename	: AUIProgress.h
// Creator	: Tom Zhou
// Date		: June 12, 2004
// Desc		: AUIProgress is the class of progres control.

#ifndef _AUIPROGRESS_H_
#define _AUIPROGRESS_H_

#include "AUIObject.h"
#include "AUIFrame.h"
#include "A3DShader.h"
#include "A3DStream.h"

// ÐÂ´úÂëÓÃGetRangeMax()
#define AUIPROGRESS_MAX 100

class A3DGFXEx;
class AUIProgress : public AUIObject
{
public:
	AUIProgress();
	virtual ~AUIProgress();

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
	"Frame Mode"				Property->i
	"Frame Image File"			Property->fn
	"Fill Image File"			Property->fn
	"Pos Image File"			Property->fn 
	"Inc Image File"			Property->fn
	"Dec Image File"			Property->fn
	"Vertical Progress"			Property->b 
	"Reverse Direction"			Property->b
	"Fill Margin Left"			Property->i
	"Fill Margin Top"			Property->i
	"Fill Margin Right"			Property->i 
	"Fill Margin Bottom"		Property->i
	"Pos Bg Image File"			Property->fn
	AUIObject.Properties
	*/
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	int GetProgress(void);
	void SetProgress(int nProgress);
	void SetActionProgress(DWORD dwMSecs, int nProgress);
	bool IsActionProgress() { return m_bActionProgress; }
	void SetAutoProgress(DWORD dwMSecs, int nPosStart, int nPosEnd);
	bool IsAutoProgress() { return m_bAutoProgress; }

	// default 0-100. 0->nUpper
	void SetRangeMax(int nUpper);
	int GetRangeMax() const;

protected:
	bool RenderProgress(A2DSprite* pA2DSprite, int nStart, int nEnd);
	bool RenderPosImage();

	int m_nProgress;		// From 0 to 100.
	int m_nRangeMax;

	A3DRECT m_rcFillMargin;

	bool m_bActionProgress;
	bool m_bAutoProgress;
	int m_nPosAutoStart, m_nPosAutoEnd, m_nPosActionStart, m_nPosActionTemp;
	DWORD m_dwTimeStart, m_dwTimeEnd;

	int	m_nFrameMode;
	bool m_bVProgress;
	bool m_bReverseDir;
	PAUIFRAME m_pAUIFrame;
	A2DSprite *m_pA2DSpriteBar, *m_pA2DSpritePos, *m_pA2DSpriteInc, *m_pA2DSpriteDec;

	bool m_bCanAutoFixPosImage;
	A3DShader* m_pShaderAutoPos;
	A3DStream* m_pStreamAutoPos;
	AString m_strPosImage;
	AString m_strPosBgImage;

	A3DGFXEx* m_pPosGfx; // by Silas, 2013/04/18
};

typedef AUIProgress * PAUIPROGRESS;

#endif //_AUIPROGRESS_H_