// FWAssemblySet.cpp: implementation of the FWAssemblySet class.
//
//////////////////////////////////////////////////////////////////////

#include "FWAssemblySet.h"
#include "A3DGFXEx.h"
#include "CodeTemplate.h"
#include "A3DGFXExMan.h"
#include <A3DGFXElement.h>
#include "FWAssemblySet.h"
#include "FWFireworkLanch.h"
#include "FWTemplate.h"
#include "FWGlyphFirework.h"
#include "FWOutterProperty.h"
#include "FWGlyphList.h"

#define new A_DEBUG_NEW




FWAssemblySet::FWAssemblySet(int iPoolSize) :
	FWAssemblySetBase(iPoolSize),
	m_dwTimeStart(0)
{

}

FWAssemblySet::~FWAssemblySet()
{
	DeleteAll();
}

void FWAssemblySet::DeleteAll()
{
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWAssembly *pAssembly = GetNext(pos);
		A3DRELEASE(pAssembly);
	}
	RemoveAll();
}

bool FWAssemblySet::TickAnimation(DWORD dwTickTime)
{
	if (GetTickCount() - m_dwTimeStart > 1000 * 30)
	{
		Stop();
		return true;
	}

	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWAssembly *pAssembly = GetNext(pos);
		pAssembly->TickAnimation(dwTickTime);
	}
	return true;
}

bool FWAssemblySet::Start(bool bForceStop)
{
	m_dwTimeStart = GetTickCount();

	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWAssembly *pAssembly = GetNext(pos);
		// do not start a paused gfx, if bForceStop is not set
		if (bForceStop || !bForceStop && pAssembly->GetState() == ST_PAUSE)
			pAssembly->Start(bForceStop);
	}
	return true;
}

bool FWAssemblySet::Pause()
{
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWAssembly *pAssembly = GetNext(pos);
		// do not force ST_STOP gfx to be the state ST_PAUSE
		if (pAssembly->GetState() == ST_PLAY)
			pAssembly->Pause();
	}
	return true;
}

void FWAssemblySet::Stop()
{
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWAssembly *pAssembly = GetNext(pos);
		pAssembly->Stop();
	}
}


bool FWAssemblySet::IsAllAtState(int nState) const
{
	if (GetCount() == 0)
	{
		if (nState = ST_STOP)
			return true;
		else
			return false;
	}
	
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWAssembly *pAssembly = GetNext(pos);
		if (pAssembly->GetState() != nState)
			return false;
	}
	return true;
}


void FWAssemblySet::RegisterGfx(A3DGFXExMan *pGfxExMan)
{
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWAssembly *pAssembly = GetNext(pos);
		if (pAssembly->GetState() != ST_STOP)
			pGfxExMan->RegisterGfx(pAssembly);
	}
}

bool FWAssemblySet::CreateFromTemplate(FWTemplate *pTemplate)
{
	BEGIN_FAKE_WHILE;

	DeleteAll();

	// get lanch prop
	const FWFireworkLanch *pFirework = static_cast<const FWFireworkLanch *>(pTemplate->m_pGlyphLanch->GetFirework());
	const FWAssemblyProfile & profile = pFirework->GetAssemblyProfile();
	const FWOutterPropertyList *pPropList = profile.GetOutterPropertyList();

	
	// assume the starttime and startpos of lanch gfx is 0!

	// get start time
	const FWOutterProperty *pProp = pPropList->FindByName("上升时间");
	CHECK_BREAK(pProp);
	float fStartTime = pProp->Data;
	
	// get the height that other gfx should be offset
	pProp = pPropList->FindByName("上升距离");
	CHECK_BREAK(pPropList);
	A3DMATRIX4 matOffset;
	matOffset.Translate(0.f, float(pProp->Data), 0.f);
	
	// build set
	A3DMATRIX4 matTM = matOffset * pTemplate->m_matParentTMMain;
	CHECK_BREAK(pTemplate->m_pGlyphList->FillGfxSet(this, matTM, fStartTime));
	
	// copy lanch effect only if starttime is bigger enough
	if (fStartTime > .1f)
		pTemplate->m_pGlyphLanch->GetFirework()->CopyTo(this, pTemplate->m_matParentTMLanch, 0.f);

	END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

