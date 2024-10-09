// FWDialogSimple.cpp: implementation of the FWDialogSimple class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogSimple.h"
#include "FWAssemblyCache.h"
#include "AUIListBox.h"
#include "AUIEditBox.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNCREATE(FWDialogSimple, FWDialogBase)


BEGIN_EVENT_MAP_FW(FWDialogSimple, FWDialogPropBase)

END_EVENT_MAP_FW()



FWDialogSimple::FWDialogSimple()
{

}

FWDialogSimple::~FWDialogSimple()
{

}

int FWDialogSimple::GetProfileType()
{
	return FWAssemblyCache::TYPE_SIMPLE;
}
	