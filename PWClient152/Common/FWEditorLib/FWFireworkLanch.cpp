// FWFireworkLanch.cpp: implementation of the FWFireworkLanch class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFireworkLanch.h"
#include "FWArchive.h"

#ifdef _FW_EDITOR
#include "FWDialogLanch.h"
#endif

#define new A_DEBUG_NEW


FW_IMPLEMENT_SERIAL(FWFireworkLanch, FWFireworkLeaf)



FWFireworkLanch::FWFireworkLanch()
{
	m_profile.Init(
		FWAssemblyCache::GetInstance()->GetDefaultProfileIDByType(
			FWAssemblyCache::TYPE_LANCH));
}

FWFireworkLanch::~FWFireworkLanch()
{

}

#ifdef _FW_EDITOR

const char * FWFireworkLanch::GetDlgToShow()
{
	return TO_STRING(FWDialogLanch);
}

#endif