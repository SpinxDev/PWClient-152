#ifndef _A3DGFXCONTAINER_H_
#define _A3DGFXCONTAINER_H_

#include "A3DGFXElement.h"

class A3DGFXContainer : public A3DGFXElement
{
public:
	A3DGFXContainer(A3DGFXEx* pGfx);
	virtual ~A3DGFXContainer() { assert(m_pBindGfx == 0); }
	
protected:
	AString m_strGfxPath;
	A3DGFXEx* m_pBindGfx;
	float m_fPlaySpeed;
	bool m_bOuterColor;
	bool m_bLoopFlag;
	bool m_bStart;
	//	作为Dummy元素的时候考虑GFX自身Scale，某些情况下有用，粒子喷射的元素，希望能跟随模型比例一起改变
	bool m_bDummyUseGfxScale;

public:
	A3DGFXContainer& operator = (const A3DGFXContainer& src);
	A3DGFXEx* GetBindGfx() const { return m_pBindGfx; }
	const char* GetGfxPath() const { return m_strGfxPath; }
protected:
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release();
	virtual bool Play();
	virtual bool Pause();
	virtual bool Stop();
	virtual bool StopParticleEmit();
	virtual void DoFadeOut();
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual void DummyTick(DWORD dwTick);
	virtual bool Render(A3DViewport*);
	virtual void ResumeLoop();
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
};

#endif
