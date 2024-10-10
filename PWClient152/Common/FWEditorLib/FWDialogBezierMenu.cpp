// FWDialogBezierMenu.cpp: implementation of the FWDialogBezierMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogBezierMenu.h"
#include "FWDoc.h"
#include "FWGlyphList.h"
#include "FWGlyphBezier.h"
#include "FWCommandSetBezierMode.h"
#include "FWCommandList.h"
#include "FWView.h"
#include "FWCommandDuplicateHandle.h"
#include "FWCommandRemoveHandle.h"


#define new A_DEBUG_NEW



FW_IMPLEMENT_DYNCREATE(FWDialogBezierMenu, FWDialogBase)

BEGIN_COMMAND_MAP_FW(FWDialogBezierMenu, FWDialogBase)

ON_COMMAND_FW("SharpHandle", FWDialogBezierMenu::OnCommandSharpHandle)
ON_COMMAND_FW("SmoothHandle", FWDialogBezierMenu::OnCommandSmoothHandle)
ON_COMMAND_FW("BezierHandle", FWDialogBezierMenu::OnCommandBezierHandle)
ON_COMMAND_FW("BezierCornerHandle", FWDialogBezierMenu::OnCommandBezierCornerHandle)
ON_COMMAND_FW("Sharp", FWDialogBezierMenu::OnCommandSharp)
ON_COMMAND_FW("Smooth", FWDialogBezierMenu::OnCommandSmooth)
ON_COMMAND_FW("Bezier", FWDialogBezierMenu::OnCommandBezier)
ON_COMMAND_FW("BezierCorner", FWDialogBezierMenu::OnCommandBezierCorner)
ON_COMMAND_FW("DuplicateHandle", FWDialogBezierMenu::OnCommandDuplicateHandle)
ON_COMMAND_FW("RemoveHandle", FWDialogBezierMenu::OnCommandRemoveHandle)

END_COMMAND_MAP_FW()



FWDialogBezierMenu::FWDialogBezierMenu()
{

}

FWDialogBezierMenu::~FWDialogBezierMenu()
{

}

void FWDialogBezierMenu::InnerOnCommandBezierMenu(AString strCommand)
{
	ASSERT(GetDocument()->GetGlyphList()->GetSelHeadPosition());
	ASSERT(AString("FWGlyphBezier") == GetDocument()->GetGlyphList()->GetSelHead()->GetRuntimeClass()->m_lpszClassName);

	FWGlyphBezier *pBezier = static_cast<FWGlyphBezier *>(GetDocument()->GetGlyphList()->GetSelHead());
	APointI pt(GetPos().x, GetPos().y);

	struct _local_map {const char * szCommand; int nMode;};
	static const _local_map _map[] = 
	{
		{"SharpHandle",				FWGlyphBezier::MODE_SHARP},
		{"SmoothHandle",			FWGlyphBezier::MODE_SMOOTH},
		{"BezierHandle",			FWGlyphBezier::MODE_BEZIER},
		{"BezierCornerHandle",		FWGlyphBezier::MODE_BEZIER_CORNER},
		{"Sharp",					FWGlyphBezier::MODE_SHARP},
		{"Smooth",					FWGlyphBezier::MODE_SMOOTH},
		{"Bezier",					FWGlyphBezier::MODE_BEZIER},
		{"BezierCorner",			FWGlyphBezier::MODE_BEZIER_CORNER},
		{0,	0},
	};

	int nIndex = 0;
	const _local_map *pEntry = _map;
	while (pEntry->szCommand)
	{
		if (strCommand == pEntry->szCommand)
			break;
		pEntry++;
		nIndex++;
	}
	ASSERT(pEntry->szCommand);

	GetView()->DPtoLP(&pt);
	int nHandle = pBezier->GetHitHandle(pt);

	// do not set mode if the handle is still in this mode
	if (nHandle > 0 && pBezier->GetMode(nHandle) == pEntry->nMode) return;

	// execture setmode command
	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandSetBezierMode(pBezier, nHandle, pEntry->nMode));
}


void FWDialogBezierMenu::OnCommandSharpHandle()
{
	InnerOnCommandBezierMenu("SharpHandle");
}

void FWDialogBezierMenu::OnCommandSmoothHandle()
{
	InnerOnCommandBezierMenu("SmoothHandle");
}

void FWDialogBezierMenu::OnCommandBezierHandle()
{
	InnerOnCommandBezierMenu("BezierHandle");
}

void FWDialogBezierMenu::OnCommandBezierCornerHandle()
{
	InnerOnCommandBezierMenu("BezierCornerHandle");
}

void FWDialogBezierMenu::OnCommandSharp()
{
	InnerOnCommandBezierMenu("Sharp");
}

void FWDialogBezierMenu::OnCommandSmooth()
{
	InnerOnCommandBezierMenu("Smooth");
}

void FWDialogBezierMenu::OnCommandBezier()
{
	InnerOnCommandBezierMenu("Bezier");
}

void FWDialogBezierMenu::OnCommandBezierCorner()
{
	InnerOnCommandBezierMenu("BezierCorner");
}

void FWDialogBezierMenu::OnCommandRemoveHandle()
{
	ASSERT(GetDocument()->GetGlyphList()->GetSelHeadPosition());
	ASSERT(AString("FWGlyphBezier") == GetDocument()->GetGlyphList()->GetSelHead()->GetRuntimeClass()->m_lpszClassName);
	
	FWGlyphBezier *pBezier = static_cast<FWGlyphBezier *>(GetDocument()->GetGlyphList()->GetSelHead());
	APointI pt(GetPos().x, GetPos().y);
	GetView()->DPtoLP(&pt);

	// do not delete this handle or the remaining 
	// ones can not form a closed bezier
	if (pBezier->GetHandleCount() < 12) return;

	int nHandle = pBezier->GetHitHandle(pt);
	if (nHandle < 1) return;

	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandRemoveHandle(pBezier, nHandle));
	
}

void FWDialogBezierMenu::OnCommandDuplicateHandle()
{
	ASSERT(GetDocument()->GetGlyphList()->GetSelHeadPosition());
	ASSERT(AString("FWGlyphBezier") == GetDocument()->GetGlyphList()->GetSelHead()->GetRuntimeClass()->m_lpszClassName);
	
	FWGlyphBezier *pBezier = static_cast<FWGlyphBezier *>(GetDocument()->GetGlyphList()->GetSelHead());
	APointI pt(GetPos().x, GetPos().y);
	GetView()->DPtoLP(&pt);

	int nHandle = pBezier->GetHitHandle(pt);
	if (nHandle < 1) return;

	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandDuplicateHandle(pBezier, nHandle));
}

void FWDialogBezierMenu::Show (bool bShow, bool bModal)
{
	ASSERT(GetDocument()->GetGlyphList()->GetSelHeadPosition());
	ASSERT(AString("FWGlyphBezier") == GetDocument()->GetGlyphList()->GetSelHead()->GetRuntimeClass()->m_lpszClassName);
	
	FWGlyphBezier *pBezier = static_cast<FWGlyphBezier *>(GetDocument()->GetGlyphList()->GetSelHead());
	APointI point(GetPos().x, GetPos().y);
	GetView()->DPtoLP(&point);

	int nHandle = pBezier->GetHitHandle(point);
	
	// set enable
	bool bEnable = (nHandle > 0);
	EnableMenuItem("SharpHandle", bEnable);
	EnableMenuItem("SmoothHandle", bEnable);
	EnableMenuItem("BezierHandle", bEnable);
	EnableMenuItem("BezierCornerHandle", bEnable);
	EnableMenuItem("DuplicateHandle", bEnable);
	EnableMenuItem("RemoveHandle", bEnable && pBezier->GetHandleCount() >= 12);
	
	// check xxxHandle menu item 
	struct _local_map {const char * szCommand; int nMode;};
	static const _local_map _map[] = 
	{
		{"SharpHandle",				FWGlyphBezier::MODE_SHARP},
		{"SmoothHandle",			FWGlyphBezier::MODE_SMOOTH},
		{"BezierHandle",			FWGlyphBezier::MODE_BEZIER},
		{"BezierCornerHandle",		FWGlyphBezier::MODE_BEZIER_CORNER},
		{0, 0},
	};
	const _local_map *pEntry = _map;
	int nMode = -1;
	if (nHandle > 0) nMode = pBezier->GetMode(nHandle);
	while (pEntry->szCommand)
	{
		CheckMenuItem(
			pEntry->szCommand, 
			pEntry->nMode == nMode);
		pEntry++;
	}
	
	FWDialogBase::Show(bShow, bModal);
}
