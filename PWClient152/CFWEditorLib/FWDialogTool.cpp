// FWDialogTool.cpp: implementation of the FWDialogTool class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogTool.h"
#include "FWCommandCombine.h"
#include "FWCommandUnCombine.h"
#include "FWGlyphComposite.h"
#include "AUIStillImageButton.h"
#include "FWStateResize.h"
#include "FWGlyphList.h"
#include "FWStateRotate.h"
#include "FWStateSelect.h"
#include "FWDialogScale.h"
#include "FWDoc.h"
#include "FWCommandList.h"
#include "FWAUIManager.h"
#include "FWView.h"
#include "FWFireworkGraph.h"


#define new A_DEBUG_NEW



FW_IMPLEMENT_DYNCREATE(FWDialogTool, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogTool, FWDialogBase)

ON_EVENT_FW(NULL, WM_UPDATE_TOOLBAR, FWDialogTool::OnUpdateToolbar)
ON_EVENT_FW("BTN_SELECT", WM_LCLICK, FWDialogTool::OnClickBtnSelect)
ON_EVENT_FW("BTN_ROTATE", WM_LCLICK, FWDialogTool::OnClickBtnRotate)
ON_EVENT_FW("BTN_RESIZE", WM_LCLICK, FWDialogTool::OnClickBtnResize)
ON_EVENT_FW("BTN_COMBINE", WM_LCLICK, FWDialogTool::OnClickBtnCombine)
ON_EVENT_FW("BTN_UNCOMBINE", WM_LCLICK, FWDialogTool::OnClickBtnUnCombine)
ON_EVENT_FW("BTN_SCALE", WM_LCLICK, FWDialogTool::OnClickBtnScale)
ON_EVENT_FW("BTN_GRID", WM_LCLICK, FWDialogTool::OnClickBtnGrid)

END_EVENT_MAP_FW()



FWDialogTool::FWDialogTool() :
	m_pBtnSelect(NULL),
	m_pBtnRotate(NULL),
	m_pBtnResize(NULL)

{

}

FWDialogTool::~FWDialogTool()
{

}

void FWDialogTool::OnUpdateToolbar(WPARAM wParam, LPARAM lParam)
{
	// set select/rotate/resize button state
	struct _MAP_ENTRY
	{
		const char * szClassName;
		AUIStillImageButton *pButton;
	};
	static const _MAP_ENTRY _map [] = 
	{
		{"FWState",				m_pBtnSelect},
		{"FWStateSelect",		m_pBtnSelect},
		{"FWStateMove",			m_pBtnSelect},
		{"FWStateMoveHandle",	m_pBtnSelect},
		{"FWStateNetSelect",	m_pBtnSelect},
		{"FWStateRotate",		m_pBtnRotate},
		{"FWStateResize",		m_pBtnResize},
		{NULL, NULL},
	};
	
	
	const _MAP_ENTRY *pEntry = _map;
	while (pEntry->szClassName)
	{
		pEntry->pButton->SetPushed(false);
		pEntry++;
	}
	
	pEntry = _map;
	while (pEntry->szClassName)
	{
		LPCSTR szClassName = GetView()->GetCurrentState()->GetRuntimeClass()->m_lpszClassName;
		if (AString(pEntry->szClassName) == szClassName)
			pEntry->pButton->SetPushed(true);
		pEntry++;
	}
	
	// set showgrid button state
	if (m_pBtnGrid->IsPushed() ^ GetView()->IsShowGrid())
		m_pBtnGrid->SetPushed(GetView()->IsShowGrid());
}

bool FWDialogTool::Init(AUIDialog * pDlg)
{
	FWDialogBase::Init(pDlg);
	
	m_pBtnSelect = (AUIStillImageButton *) GetDlgItem("BTN_SELECT");
	m_pBtnRotate = (AUIStillImageButton *) GetDlgItem("BTN_ROTATE");
	m_pBtnResize = (AUIStillImageButton *) GetDlgItem("BTN_RESIZE");
	m_pBtnGrid = (AUIStillImageButton *) GetDlgItem("BTN_GRID");
	
	m_pBtnSelect->SetPushLike(true);
	m_pBtnRotate->SetPushLike(true);
	m_pBtnResize->SetPushLike(true);
	m_pBtnGrid->SetPushLike(true);
	
	return true;
}


void FWDialogTool::OnClickBtnSelect(WPARAM wParam, LPARAM lParam)
{
	GetView()->ChangeState(new FWStateSelect(GetView()));
}

void FWDialogTool::OnClickBtnRotate(WPARAM wParam, LPARAM lParam)
{
	if (GetDocument()->GetGlyphList()->GetSelCount() == 1)
	{
		GetView()->ChangeState(new FWStateRotate(
			GetView(), GetDocument()->GetGlyphList()->GetSelHead()));
	}
}

void FWDialogTool::OnClickBtnResize(WPARAM wParam, LPARAM lParam)
{
	if (GetDocument()->GetGlyphList()->GetSelCount() == 1)
	{
		GetView()->ChangeState(new FWStateResize(
			GetView(), GetDocument()->GetGlyphList()->GetSelHead()));
	}
}

void FWDialogTool::OnClickBtnCombine(WPARAM wParam, LPARAM lParam)
{
	FWGlyphList *pList = GetDocument()->GetGlyphList();
	if (pList->GetSelCount() >= 2)
	{
		FWGlyphComposite *pComposite = new FWGlyphComposite;
		pComposite->Init(new FWFireworkText, GetDocument()->GetGlyphList()->GetNextName());
		//pComposite->Init(pList->GetHead()->GetFirework(), pList->GetHead()->GetGlyphList());
		
		bool bSucceed = true;
		ALISTPOSITION pos = pList->GetSelHeadPosition();
		while (pos)
		{
			FWGlyph *pGlyph = pList->GetSelNext(pos);
			if (!pGlyph->IsCombinable())
			{
				bSucceed = false;
				break;
			}
			else
			{
				pComposite->AddTail(pGlyph);
			}
			
		}
		if (!bSucceed)
		{
			GetFWAUIManager()->MessageBox("", _AL("只能组合文字和定制效果"), MB_OK, 0xffffff00);
			
			// call removeall() to prevent the destructor of pComposite
			// delete it children which is now should be owned by 
			// document's glyph list
			pComposite->RemoveAll();
			SAFE_DELETE(pComposite);
		}
		else
		{
			GetDocument()->GetCommandList()->ExecuteCommand(
				new FWCommandCombine(GetDocument(), pComposite));
		}
		
	}
}

void FWDialogTool::OnClickBtnUnCombine(WPARAM wParam, LPARAM lParam)
{
	FWGlyphList *pList = GetDocument()->GetGlyphList();
	if (pList->GetSelCount() == 1 && 
		pList->GetSelHead()->GetRuntimeClass() == FW_RUNTIME_CLASS(FWGlyphComposite))
	{
		GetDocument()->GetCommandList()->ExecuteCommand(
			new FWCommandUnCombine(GetDocument(), (FWGlyphComposite *) pList->GetSelHead()));
	}
}

void FWDialogTool::OnClickBtnScale(WPARAM wParam, LPARAM lParam)
{
	FWDialogScale *pDlg = GetFWDialog2(FWDialogScale);
	pDlg->m_fScale = GetDocument()->GetGlyphList()->GetScale();
	pDlg->DoModal(OnCloseDialogScale, this);
}

void FWDialogTool::OnCloseDialogScale(void *pData)
{
	FWDialogTool *pThis = (FWDialogTool *) pData;
	FWDialogScale *pDlg = 
		static_cast<FWDialogScale *>(pThis->GetFWDialog(TO_STRING(FWDialogScale)));
	
	if (pDlg->GetCloseID() == FWDialogBase::FW_IDOK)
		pThis->GetDocument()->GetGlyphList()->SetScale(pDlg->m_fScale);
}

void FWDialogTool::OnClickBtnGrid(WPARAM wParam, LPARAM lParam)
{
	GetView()->SetShowGrid(!GetView()->IsShowGrid());
}
