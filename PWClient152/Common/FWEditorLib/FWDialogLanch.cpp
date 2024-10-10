// FWDialogLanch.cpp: implementation of the FWDialogLanch class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogLanch.h"
#include "FWAssemblyCache.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNCREATE(FWDialogLanch, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogLanch, FWDialogPropBase)

END_EVENT_MAP_FW()



FWDialogLanch::FWDialogLanch()
{

}

FWDialogLanch::~FWDialogLanch()
{

}

int FWDialogLanch::GetProfileType()
{
	return FWAssemblyCache::TYPE_LANCH;
}
