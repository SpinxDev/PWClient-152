// CustomFirework.cpp: implementation of the FWFireworkCustom class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFireworkCustom.h"
#include "FWModifier.h"
#include "FWModifierCreator.h"
#include "FWFireworkElement.h"
#include "FWAssemblyCache.h"
#include "FWConfig.h"
#include "FWArchive.h"

#ifdef _FW_EDITOR
	#include "FWDialogCustom.h"
#endif

#define new A_DEBUG_NEW



FW_IMPLEMENT_SERIAL(FWFireworkCustom, FWFireworkLeaf)



FWFireworkCustom::FWFireworkCustom()
{
	m_nComponentCount = 10;
	m_pModifier = FWModifierCreator::GetInstance()->CreateModifier(FWModifierCreator::ID_MODIFIER_DEFAULT);
	m_profile.Init(
		FWAssemblyCache::GetInstance()->GetDefaultProfileIDByType(
		FWAssemblyCache::TYPE_COMPONENT));
}

FWFireworkCustom::~FWFireworkCustom()
{
	SAFE_DELETE(m_pModifier);
}

bool FWFireworkCustom::CopyTo(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime)
{
	BEGIN_FAKE_WHILE;
	
	m_pModifier->RadomParamForAll();
	for (int i = 0; i < m_nComponentCount; i++)
	{
		m_pModifier->RadomParamForSingle();
		
		// create and init component
		FWFireworkElement component;

		if (i == 0) component.SetEnableSound(true);
		
		component.SetModifier(m_pModifier->Clone());
		component.SetAssemblyProfile(m_profile);
		for (int j = 0; j < OFFSET_COUNT; j++)
		{
			component.SetOffsetTime(j, m_fOffsetTime[j]);
			component.SetOffsetPos(j, m_vOffsetPos[j]);
		}
		
		// copy component content to gfx
		CHECK_BREAK(component.CopyTo(pSet, matTM, fStartTime));
	}
	
	END_FAKE_WHILE;
	
	RETURN_FAKE_WHILE_RESULT;
}

void FWFireworkCustom::SetModifier(FWModifier *pSrc)
{
	SAFE_DELETE(m_pModifier);
	m_pModifier = pSrc;
}

#ifdef _FW_EDITOR

const char * FWFireworkCustom::GetDlgToShow()
{
	return TO_STRING(FWDialogCustom);
}

void FWFireworkCustom::PrepareDlg(FWDialogPropBase *pDlg)
{
	FWFireworkLeaf::PrepareDlg(pDlg);

	FWDialogCustom *pCustomDlg = (FWDialogCustom *)pDlg;
	
	pCustomDlg->m_nCount = m_nComponentCount;
	pCustomDlg->SetModifier(m_pModifier);
}

void FWFireworkCustom::UpdateFromDlg(FWDialogPropBase * pDlg)
{
	FWFireworkLeaf::UpdateFromDlg(pDlg);
	
	FWDialogCustom *pCustomDlg = (FWDialogCustom *)pDlg;
	
	m_nComponentCount = pCustomDlg->m_nCount;
	
	SAFE_DELETE(m_pModifier);
	m_pModifier = pCustomDlg->GetModifier()->Clone();
}

#endif

void FWFireworkCustom::Serialize(FWArchive &ar)
{
	FWFireworkLeaf::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << (BYTE) m_nComponentCount;
		ar << m_pModifier;
	}
	else
	{
		BYTE tmp = 0;
		ar >> tmp; m_nComponentCount = tmp;
		
		SAFE_DELETE(m_pModifier);
		ar >> (const FWObject *&)m_pModifier;
	}
}

int FWFireworkCustom::GetParticalCount()
{
	int nCountSingle = FWFireworkLeaf::GetParticalCount();

	return nCountSingle * m_nComponentCount;
}

