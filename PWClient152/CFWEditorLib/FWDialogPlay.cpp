// FWDialogPlay.cpp: implementation of the FWDialogPlay class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogPlay.h"
#include "FWDoc.h"
#include "FWDialogMain.h"
#include "FWDialogBoard.h"
#include "FWDialogEdit.h"
#include "FWPlayer.h"
#include "FWView.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNCREATE(FWDialogPlay, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogPlay, FWDialogBase)

ON_EVENT_FW("BTN_PLAY", WM_LCLICK, FWDialogPlay::OnClickBtnPlay)
ON_EVENT_FW("BTN_PAUSE", WM_LCLICK, FWDialogPlay::OnClickBtnPause)
ON_EVENT_FW("BTN_STOP", WM_LCLICK, FWDialogPlay::OnClickBtnStop)
ON_EVENT_FW("BTN_RETURN", WM_LCLICK, FWDialogPlay::OnClickBtnReturn)

END_EVENT_MAP_FW()




FWDialogPlay::FWDialogPlay()
{

}

FWDialogPlay::~FWDialogPlay()
{

}

void FWDialogPlay::OnClickBtnPlay(WPARAM wParam, LPARAM lParam)
{
	GetView()->GetPlayer()->Start();
}

void FWDialogPlay::OnClickBtnPause(WPARAM wParam, LPARAM lParam)
{
	GetView()->GetPlayer()->Pause();
}

void FWDialogPlay::OnClickBtnStop(WPARAM wParam, LPARAM lParam)
{
	GetView()->GetPlayer()->Stop();
}

void FWDialogPlay::OnClickBtnReturn(WPARAM wParam, LPARAM lParam)
{
	OnClickBtnStop(NULL, NULL);

	GetFWDialog2(FWDialogPlay)->Show(false);

	GetFWDialog2(FWDialogMain)->SetEnable(true);
	GetFWDialog2(FWDialogBoard)->SetEnable(true);
	GetFWDialog2(FWDialogEdit)->SetEnable(true);

	GetView()->GetPlayer()->SetActive(false);
}
