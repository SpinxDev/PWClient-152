#ifndef A3DGFXSOUND_H_
#define A3DGFXSOUND_H_

#include "A3DGFXElement.h"
#include "AM3DSoundBuffer.h"
#include "A3DGFXSoundImp.h"

class A3DGFXSound : public A3DGFXElement
{
public:
	
	explicit A3DGFXSound(A3DGFXEx* pGfx);
	~A3DGFXSound();

public:

	A3DDevice* GetDevice() const { return m_pDevice; }
	A3DGFXEx* GetParentGfx() const { return m_pGfx; }

protected:

	GFXSOUNDIMP m_SoundImp;
	GFXSOUNDIMP22 m_SoundImp22;
	IGFXSOUNDIMP* m_pActiveImp;

protected:

	bool IsSoundValid() const;
	void StopSound();
	void TickSound(DWORD dwTickTime);
	A3DGFXSound& operator = (const A3DGFXSound& src);
	IGFXSOUNDIMP* GetCurImp() const { return m_pActiveImp; }
	void SetupActiveImp();

protected:
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release();
	virtual bool Play();
	virtual bool Stop();
	virtual bool Render(A3DViewport*);
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual void ResumeLoop();
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
};

#endif
