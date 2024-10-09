#include "DlgBlackScreen.h"

#include "EC_Game.h"
#include "EC_Viewport.h"

#include "A3DViewport.h"

bool CDlgBlackScreen::OnInitDialog() {
	return true;
}

void CDlgBlackScreen::OnShowDialog() {
	SetView();
}

void CDlgBlackScreen::Resize(A3DRECT rcOld, A3DRECT rcNew) {
	SetView();
}

void CDlgBlackScreen::SetView() {
	int viewportWidth = GetGame()->GetViewport()->GetA3DViewport()->GetParam()->Width;
	int viewportHeight = GetGame()->GetViewport()->GetA3DViewport()->GetParam()->Height;
	SetPosEx(0, 0);
	SetSize(viewportWidth, viewportHeight);
}

void CDlgBlackScreen::OnChangeLayoutEnd(bool bAllDone) {
	SetView();
}