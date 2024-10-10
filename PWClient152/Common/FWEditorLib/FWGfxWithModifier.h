#ifndef _FWGFXWITHMODIFIER_H_
#define _FWGFXWITHMODIFIER_H_

#include <A3DGFXEx.h>
#include "CodeTemplate.h"
#include "FWConfig.h"

class FWModifier;

// this is the enchanted gfx that support modifier
class FWGfxWithModifier : public A3DGFXEx  
{
protected:
	FWModifier * m_pModifier;	// modifier
	// parentTM should resoter to this value every time
	// before replay
	A3DMATRIX4 m_matParentOld;	
public:
	FWGfxWithModifier();
	FWGfxWithModifier(const A3DGFXEx *pGfx);
	virtual ~FWGfxWithModifier();
	FWGfxWithModifier & operator = (const FWGfxWithModifier & src);
	IMPLEMENT_CLONE_INLINE(FWGfxWithModifier, A3DGFXEx)
public:
	const FWModifier * GetModifier() const;
	void SetModifier(FWModifier * pSrc);
public:
	// base class interface
	virtual bool Start(bool bForceStop = false);
	virtual bool Stop();
	virtual bool TickAnimation(DWORD dwTickTime);

	void SaveOriginalParentTM();
};

#endif 