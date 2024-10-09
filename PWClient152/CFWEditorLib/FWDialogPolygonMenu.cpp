// FWDialogPolygonMenu.cpp: implementation of the FWDialogPolygonMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogPolygonMenu.h"
#include "FWDoc.h"
#include "FWGlyphList.h"
#include "FWCommandList.h"
#include "FWView.h"
#include "FWGlyphPolygon.h"
#include "FWCommandRemoveHandle.h"
#include "FWCommandDuplicateHandle.h"


FW_IMPLEMENT_DYNCREATE(FWDialogPolygonMenu, FWDialogBase)

BEGIN_COMMAND_MAP_FW(FWDialogPolygonMenu, FWDialogBase)

ON_COMMAND_FW("DuplicateHandle", FWDialogPolygonMenu::OnCommandDuplicateHandle)
ON_COMMAND_FW("RemoveHandle", FWDialogPolygonMenu::OnCommandRemoveHandle)

END_COMMAND_MAP_FW()


FWDialogPolygonMenu::FWDialogPolygonMenu()
{

}

FWDialogPolygonMenu::~FWDialogPolygonMenu()
{

}

void FWDialogPolygonMenu::OnCommandDuplicateHandle()
{
	ASSERT(GetDocument()->GetGlyphList()->GetSelHeadPosition());
	ASSERT(AString("FWGlyphPolygon") == GetDocument()->GetGlyphList()->GetSelHead()->GetRuntimeClass()->m_lpszClassName);
	
	FWGlyphPolygon *pPolygon = static_cast<FWGlyphPolygon *>(GetDocument()->GetGlyphList()->GetSelHead());
	APointI pt(GetPos().x, GetPos().y);
	GetView()->DPtoLP(&pt);
	
	int nHandle = pPolygon->GetHitHandle(pt);
	if (nHandle < 1) return;
	
	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandDuplicateHandle(pPolygon, nHandle));

}

void FWDialogPolygonMenu::OnCommandRemoveHandle()
{
	ASSERT(GetDocument()->GetGlyphList()->GetSelHeadPosition());
	ASSERT(AString("FWGlyphPolygon") == GetDocument()->GetGlyphList()->GetSelHead()->GetRuntimeClass()->m_lpszClassName);
	
	FWGlyphPolygon *pPolygon = static_cast<FWGlyphPolygon *>(GetDocument()->GetGlyphList()->GetSelHead());
	APointI pt(GetPos().x, GetPos().y);
	GetView()->DPtoLP(&pt);
	
	// do not delete this handle or the remaining 
	// ones can not form a closed Polygon
	if (pPolygon->GetHandleCount() < 4) return;
	
	int nHandle = pPolygon->GetHitHandle(pt);
	if (nHandle < 1) return;
	
	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandRemoveHandle(pPolygon, nHandle));

}

void FWDialogPolygonMenu::Show(bool bShow, bool bModal)
{
	ASSERT(GetDocument()->GetGlyphList()->GetSelHeadPosition());
	ASSERT(AString("FWGlyphPolygon") == GetDocument()->GetGlyphList()->GetSelHead()->GetRuntimeClass()->m_lpszClassName);
	
	FWGlyphPolygon *pPolygon = static_cast<FWGlyphPolygon *>(GetDocument()->GetGlyphList()->GetSelHead());
	APointI point(GetPos().x, GetPos().y);
	GetView()->DPtoLP(&point);
	
	int nHandle = pPolygon->GetHitHandle(point);

	bool bEnable = (nHandle > 0);
	EnableMenuItem("DuplicateHandle", bEnable);
	EnableMenuItem("RemoveHandle", bEnable && pPolygon->GetHandleCount() >= 4);


	FWDialogBase::Show(bShow, bModal);
}
