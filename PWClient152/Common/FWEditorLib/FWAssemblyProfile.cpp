// AssemblyProfile.cpp: implementation of the FWAssemblyProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "FWAssemblyProfile.h"
#include "FWAssemblyCache.h"
#include "A3DGFXEx.h"
#include "FWOutterProperty.h"
#include "FWFormulaList.h"
#include "FWAlgorithm.h"
#include "FWArchive.h"
#include "FWLog.h"
#include <A3DMacros.h>

using namespace FWAlgorithm;

#define new A_DEBUG_NEW




FWAssemblyProfile::FWAssemblyProfile() :
	PROPERTY_INIT_INT(AssemblyID, -1),
	m_fScale(1.f)
{
	m_pOutterPropertyList = new FWOutterPropertyList;
}

FWAssemblyProfile::~FWAssemblyProfile()
{
	SAFE_DELETE(m_pOutterPropertyList);
}

bool FWAssemblyProfile::Load(AScriptFile *pFile)
{
	// let the outter property list to do actual loading work
	return m_pOutterPropertyList->Load(pFile);
}

FWAssembly * FWAssemblyProfile::CreateAssembly() const
{
	FWAssembly * pAssembly = NULL;
	FWFormulaList * pFormulaList = NULL;

	BEGIN_FAKE_WHILE;

	// this will return NULL
	// do not use assert(), if no assembly is prepared
	// in resource, fweditor should also work properly
	CHECK_BREAK(m_nAssemblyID != -1);
	
	// find original one in cache
	FWAssemblyCache *pCache = FWAssemblyCache::GetInstance();
	FWAssemblyInfo *pInfo = pCache->FindByID(m_nAssemblyID);
	CHECK_BREAK(pInfo);
	
	// copy original gfx to local 
	pAssembly = pInfo->CreateAssembly();
	CHECK_BREAK(pAssembly);

	// updata gfx by content
#ifdef _ENABLE_ASSEMBLY_CACHE
	CHECK_BREAK(pInfo->GetFormulaList());
	CHECK_BREAK(pInfo->GetFormulaList()->Compute(pAssembly, m_pOutterPropertyList));
#else
	pFormulaList = pInfo->CreateFormulaList();
	CHECK_BREAK(pFormulaList);
	CHECK_BREAK(pFormulaList->Compute(pAssembly, m_pOutterPropertyList));
#endif
	pAssembly->SetScale(m_fScale);
	
	END_FAKE_WHILE;

	SAFE_DELETE(pFormulaList);

	BEGIN_ON_FAIL_FAKE_WHILE;

	A3DRELEASE(pAssembly);

	FWLog::GetInstance()->Log(
		"FWAssemblyProfile::CreateAssembly(), \
		failed to create assembly from profile! \
		the requested AssemblyID is %d",
		m_nAssemblyID);
	
	END_ON_FAIL_FAKE_WHILE;


	return pAssembly;
}

FWAssemblyProfile & FWAssemblyProfile::operator = (const FWAssemblyProfile & src)
{
	m_nAssemblyID = src.GetAssemblyID();
	*m_pOutterPropertyList = *src.GetOutterPropertyList();
	m_fScale = src.GetScale();
	return *this;
}

bool FWAssemblyProfile::Init(int nID)
{
	FWAssemblyProfile *pTempProfile = NULL;

	BEGIN_FAKE_WHILE;

	CHECK_BREAK(nID != -1);

	FWAssemblyCache *pCache = FWAssemblyCache::GetInstance();

	FWAssemblyInfo *pInfo = pCache->FindByID(nID);
	CHECK_BREAK(pInfo);

#ifdef _ENABLE_ASSEMBLY_CACHE
	*this = *pInfo->GetProfile();
#else
	pTempProfile = pInfo->CreateProfile();
	CHECK_BREAK(pTempProfile);
	*this = *pTempProfile;
#endif


	m_fScale = 1.f;

	END_FAKE_WHILE;

	SAFE_DELETE(pTempProfile);

	RETURN_FAKE_WHILE_RESULT;
}


void FWAssemblyProfile::FillParamArray(FWParamArray &arrayParam) const
{
	m_pOutterPropertyList->FillParamArray(arrayParam);
	
	arrayParam.Add(FWParam(m_fScale, .001f, 10.f, _AL("ÕûÌå±ÈÀý")));
}

void FWAssemblyProfile::UpdateFromParamArray(const FWParamArray &arrayParam)
{
	m_pOutterPropertyList->UpdateFromParamArray(arrayParam);

	FWParam &param = arrayParam.GetAt(arrayParam.GetSize() - 1);
	m_fScale = param.GetVal();
}


void FWAssemblyProfile::Serialize(FWArchive &ar)
{
	if (ar.IsStoring())
	{
		ar << (short)m_nAssemblyID;
		if (m_nAssemblyID != -1)	
		{
			ALISTPOSITION pos = m_pOutterPropertyList->GetHeadPosition();
			while (pos)
			{
				FWOutterProperty *pProp = m_pOutterPropertyList->GetNext(pos);
				SerializeGFX_PROPERTY(pProp->Data, ar);
			}
		}
		ar << m_fScale;
	}
	else
	{
		short tmp = -1;
		ar >> tmp; m_nAssemblyID = tmp;
		if (m_nAssemblyID != -1)
		{
			Init(m_nAssemblyID);
			ALISTPOSITION pos = m_pOutterPropertyList->GetHeadPosition();
			while (pos)
			{
				FWOutterProperty *pProp = m_pOutterPropertyList->GetNext(pos);
				SerializeGFX_PROPERTY(pProp->Data, ar);
			}
		}
		ar >> m_fScale;
	}
}


int FWAssemblyProfile::GetParticalCount() const
{
	FWAssemblyCache *pCache = FWAssemblyCache::GetInstance();
	FWAssemblyInfo *pInfo = pCache->FindByID(m_nAssemblyID);
	if (!pInfo) return 0;

	if (pInfo->ParticalCount != -1)
		return pInfo->ParticalCount;

#ifdef _ENABLE_ASSEMBLY_CACHE

	const FWAssembly *pAssembly = pInfo->GetAssembly();
	int nPartCount = ComputeParticalCount(pAssembly);
	return nPartCount;

#else

	FWAssembly *pAssembly = CreateAssembly();
	int nPartCount = ComputeParticalCount(pAssembly);
	A3DRELEASE(pAssembly);

#ifdef _ENABLE_PARTCOUNT_CACHE 
	if (pInfo->LoadIsParticalCountConstant())
		pInfo->ParticalCount = nPartCount;
#endif

	return nPartCount;
#endif

}

int FWAssemblyProfile::GetSoundCount() const
{
	FWAssemblyCache *pCache = FWAssemblyCache::GetInstance();
	FWAssemblyInfo *pInfo = pCache->FindByID(m_nAssemblyID);
	if (!pInfo) return 0;
	
	if (pInfo->SoundCount != -1)
		return pInfo->SoundCount;
	
#ifdef _ENABLE_ASSEMBLY_CACHE
	
	const FWAssembly *pAssembly = pInfo->GetAssembly();
	int nSoundCount = ComputeSoundCount(pAssembly);
	return nSoundCount;
	
#else
	
	FWAssembly *pAssembly = CreateAssembly();
	int nSoundCount = ComputeSoundCount(pAssembly);
	A3DRELEASE(pAssembly);
	
	pInfo->SoundCount = nSoundCount;
	
	return nSoundCount;
#endif
	
}
