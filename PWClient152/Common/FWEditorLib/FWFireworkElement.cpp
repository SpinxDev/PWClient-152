// ComponentFirework.cpp: implementation of the FWFireworkElement class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFireworkElement.h"
#include "FWModifier.h"
#include "A3DGFXEx.h"
#include "A3DParticleSystemEx.h"
#include "FWModifierCreator.h"
#include "FWAssemblyCache.h"
#include "FWGfxWithModifier.h"
#include "FWAssemblySet.h"
#include "FWAlgorithm.h"

#define new A_DEBUG_NEW





FWFireworkElement::FWFireworkElement()
{
	m_pModifier = FWModifierCreator::GetInstance()->CreateModifier(FWModifierCreator::ID_MODIFIER_DEFAULT);
	m_profile.Init(
		FWAssemblyCache::GetInstance()->GetDefaultProfileIDByType(
			FWAssemblyCache::TYPE_COMPONENT));
	m_bEnableSound = false;
}

FWFireworkElement::~FWFireworkElement()
{
	SAFE_DELETE(m_pModifier);
}

bool FWFireworkElement::CopyTo(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime)
{
	FWAssembly *pAssembly = NULL;
	FWGfxWithModifier *pFWGfx = NULL;
	FWModifier *pModifier = NULL;

	BEGIN_FAKE_WHILE;

	// create original gfx
	pAssembly = LoadAndInitAssembly();
	CHECK_BREAK(pAssembly);

	// remove sound if m_nEnableSound is false
	if (!m_bEnableSound) 
		FWAlgorithm::RemoveSoundElement(pAssembly);

	// convert to our own gfx that support modifier
	pFWGfx = new FWGfxWithModifier(pAssembly);
	CHECK_BREAK(pFWGfx);

	// all data have been copyied to pFWGfx, don't need it anymore
	A3DRELEASE(pAssembly); 
	
	// set modifier
	pModifier = m_pModifier->Clone();
	CHECK_BREAK(pModifier);

	pModifier->SetTimeWait((GetSumOfOffsetTime() + fStartTime) * 1000);
	pFWGfx->SetModifier(pModifier);
	
	// update start time and pos
	CHECK_BREAK(UpdateAssemblyStartParam(pFWGfx, matTM, fStartTime));
	
	// this is used for multi-times playback
	// each time the OriginalPos will be copied
	// to modifier
	pFWGfx->SaveOriginalParentTM();
	
	// add to set
	pSet->AddTail(pFWGfx);

	END_FAKE_WHILE;

	BEGIN_ON_FAIL_FAKE_WHILE;

	A3DRELEASE(pAssembly);
	A3DRELEASE(pFWGfx);
	SAFE_DELETE(m_pModifier);

	END_ON_FAIL_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

void FWFireworkElement::SetModifier(FWModifier *pSrc)
{
	SAFE_DELETE(m_pModifier);
	m_pModifier = pSrc;
}
