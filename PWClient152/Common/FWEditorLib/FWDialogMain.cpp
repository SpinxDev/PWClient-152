// FWDMain.cpp: implementation of the FWDToolbar class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogMain.h"
#include "CodeTemplate.h"
#include "AUIObject.h"
#include "FWAUIManager.h"
#include "FWGlyphFirework.h"
#include "FWStateCreateFirework.h"
#include "FWView.h"
#include "FWGlyphText.h"
#include "FWStateCreateText.h"
#include "FWFireworkSimple.h"
#include "FWFireworkCustom.h"
#include "FWFireworkGraph.h"
#include "FWDoc.h"
#include "FWDialogPlay.h"
#include "FWDialogBoard.h"
#include "FWDialogEdit.h"
#include "FWDialogGraphMenu.h"
#include "FWDialogParam.h"
#include "FWPlayer.h"
#include "FWGlyphList.h"
#include "FWStateSelect.h"
#include "AUIStillImageButton.h"
#include "FWAssemblySet.h"


#define new A_DEBUG_NEW




FW_IMPLEMENT_DYNCREATE(FWDialogMain, FWDialogBase)


BEGIN_EVENT_MAP_FW(FWDialogMain, FWDialogBase)

ON_EVENT_FW("BTN_GRAPH", WM_LCLICK, FWDialogMain::OnClickBtnGraph)
ON_EVENT_FW("BTN_SIMPLE", WM_LCLICK, FWDialogMain::OnClickBtnSimple)
ON_EVENT_FW("BTN_TEXT", WM_LCLICK, FWDialogMain::OnClickBtnText)
ON_EVENT_FW("BTN_CUSTOM", WM_LCLICK, FWDialogMain::OnClickBtnCustom)
ON_EVENT_FW("BTN_PREVIEW", WM_LCLICK, FWDialogMain::OnClickBtnPreview)
ON_EVENT_FW("BTN_LANCH", WM_LCLICK, FWDialogMain::OnClickBtnLanch)

END_EVENT_MAP_FW()

void FWDialogMain::OnClickBtnGraph(WPARAM wParam, LPARAM lParam)
{
	FWAUIManager *pAUIMan = GetFWAUIManager();
	
	PAUISTILLIMAGEBUTTON pBtn = (PAUISTILLIMAGEBUTTON) GetDlgItem("BTN_GRAPH");
	ASSERT(pBtn);

	PAUIDIALOG pDlg = pAUIMan->GetDialog("DLG_FW_GRAPH_MENU");
	ASSERT(pDlg);
	pDlg->SetPosEx(pBtn->GetPos().x + pBtn->GetSize().cx + 3, pBtn->GetPos().y);	


	pDlg->Show(true);
}

void FWDialogMain::OnClickBtnSimple(WPARAM wParam, LPARAM lParam)
{
	FWGlyphFirework *pGlyph = new FWGlyphFirework;
	pGlyph->Init(new FWFireworkSimple, GetDocument()->GetGlyphList()->GetNextName());

	GetView()->ChangeState(new FWStateCreateFirework(GetView(), pGlyph));
}

void FWDialogMain::OnClickBtnText(WPARAM wParam, LPARAM lParam)
{
	FWGlyphText *pGlyph = new FWGlyphText;
	pGlyph->Init(new FWFireworkText, GetDocument()->GetGlyphList()->GetNextName());

	GetView()->ChangeState(new FWStateCreateText(GetView(), pGlyph));
}

void FWDialogMain::OnClickBtnCustom(WPARAM wParam, LPARAM lParam)
{
	FWGlyphFirework *pGlyph = new FWGlyphFirework;
	pGlyph->Init(new FWFireworkCustom, GetDocument()->GetGlyphList()->GetNextName());

	GetView()->ChangeState(new FWStateCreateFirework(GetView(), pGlyph));
}

void FWDialogMain::OnClickBtnPreview(WPARAM wParam, LPARAM lParam)
{
	ARectI rectWnd = GetFWDialog2(FWDialogBoard)->GetClientRect();
	
	GetView()->PreparePreviewViewport(rectWnd);
	
	APointI center = GetView()->GetClientRect().CenterPoint();

	GetView()->GetPlayer()->GetAssemblySet()->CreateFromTemplate(GetDocument());

	GetFWDialog2(FWDialogMain)->SetEnable(false);
	GetFWDialog2(FWDialogBoard)->SetEnable(false);
	GetFWDialog2(FWDialogEdit)->SetEnable(false);

	GetFWDialog2(FWDialogPlay)->Show(true);
	GetFWDialog2(FWDialogPlay)->OnClickBtnPlay(NULL, NULL);
	
	GetView()->GetPlayer()->SetActive(true);
	GetView()->GetPlayer()->Start();

}


#define DEFAULT_BUFFER_SIZE 1024 * 5	
void FWDialogMain::OnOK()
{
}

void FWDialogMain::OnCancel()
{
}

void FWDialogMain::OnClickBtnLanch(WPARAM wParam, LPARAM lParam)
{
	GetView()->ShowPropDlg(GetDocument()->GetGlyphLanch());
}
