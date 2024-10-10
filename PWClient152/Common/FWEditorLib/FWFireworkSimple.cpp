// FWFireworkSimple.cpp: implementation of the FWFireworkSimple class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFireworkSimple.h"
#include "FWAssemblyCache.h"
#include "FWConfig.h"
#include "FWArchive.h"

#ifdef _FW_EDITOR
	#include "FWDialogSimple.h"
#endif

#define new A_DEBUG_NEW



FW_IMPLEMENT_SERIAL(FWFireworkSimple, FWFireworkLeaf)



FWFireworkSimple::FWFireworkSimple()
{
	m_profile.Init(
		FWAssemblyCache::GetInstance()->GetDefaultProfileIDByType(
			FWAssemblyCache::TYPE_SIMPLE));
}

FWFireworkSimple::~FWFireworkSimple()
{

}

#ifdef _FW_EDITOR

const char * FWFireworkSimple::GetDlgToShow()
{
	return TO_STRING(FWDialogSimple);
}

#endif
