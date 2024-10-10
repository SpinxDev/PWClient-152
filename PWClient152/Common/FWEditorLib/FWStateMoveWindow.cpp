// FWStateMoveWindow.cpp: implementation of the FWStateMoveWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateMoveWindow.h"
#include "FWDialogBoard.h"
#include "FWGlyphList.h"
#include "FWDoc.h"
#include "FWGlyph.h"
#include "FWView.h"
#include "FWCommandList.h"
#include "FWStateSelect.h"


#define new A_DEBUG_NEW




FWStateMoveWindow::FWStateMoveWindow(FWView *pView) :
	FWState(pView)
{
	FWDialogBoard *pBoard = static_cast<FWDialogBoard *>(GetView()->GetFWDialog(TO_STRING(FWDialogBoard)));
	
	pBoard->SetCapture();
	pBoard->SetCanMove(true);
}

FWStateMoveWindow::~FWStateMoveWindow()
{
	GetView()->ReleaseCapture();
}

void FWStateMoveWindow::OnLButtonUp(UINT nFlags, APointI point)
{
	FWDialogBoard *pBoard = 
		static_cast<FWDialogBoard *>(GetView()->GetFWDialog(TO_STRING(FWDialogBoard)));

	pBoard->ReleaseCapture();
	pBoard->SetCanMove(false);

	GetView()->ChangeState(new FWStateSelect(GetView()));
}

