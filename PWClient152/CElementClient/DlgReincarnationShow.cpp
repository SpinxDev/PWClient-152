// Filename	: CDlgFullScreenEffectShow.cpp
// Creator	: Han Guanghui
// Date		: 2013/10/13

#include "DlgReincarnationShow.h"
#include "AUIImagePicture.h"
#include "A3DGFXExMan.h"
#include "AFI.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_LoginPlayer.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include <A3DSkin.h>
#include "EC_LoginPlayer.h"
#include "EC_UIConfigs.h"
#include "A3DCamera.h"
#include "roleinfo"

#define new A_DEBUG_NEW

#define MAP_DEFAULT_WIDTH 1024
#define MAP_DEFAULT_HEIGHT 768

AUI_BEGIN_COMMAND_MAP(CDlgFullScreenEffectShow, CDlgBase)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFullScreenEffectShow, CDlgBase)
AUI_END_EVENT_MAP()

CDlgFullScreenEffectShow::CDlgFullScreenEffectShow():
m_pPlayer(NULL),
m_nEffect(REINCARNATION_EFFECT)
{

}

void CDlgFullScreenEffectShow::OnShowDialog()
{
	CDlgBase::OnShowDialog();
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Gfx"));
	if (pImg) {
		if (m_nEffect >= 0 && m_nEffect < (int)CECUIConfig::Instance().GetGameUI().strFullScreenGfxForeground.size())
			pImg->SetGfx(CECUIConfig::Instance().GetGameUI().strFullScreenGfxForeground[m_nEffect]);
	}
	pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Bg"));
	if (pImg) {
		if (m_nEffect >= 0 && m_nEffect < (int)CECUIConfig::Instance().GetGameUI().strFullScreenGfxBackground.size())
			pImg->SetGfx(CECUIConfig::Instance().GetGameUI().strFullScreenGfxBackground[m_nEffect]);
	}
	if (!m_pPlayer)
	{
		m_pPlayer = new CECLoginPlayer(GetGame()->GetGameRun()->GetWorld()->GetPlayerMan());
		GNET::RoleInfo info = GetGame()->GetGameRun()->GetSelectedRoleInfo();
		
		if (!m_pPlayer->Load(info))
		{
			m_pPlayer->Release();
			delete m_pPlayer;
			m_pPlayer = NULL;
		}
		else
		{
			m_pPlayer->SetPos(A3DVECTOR3(0));
			m_pPlayer->ChangeModelMoveDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0));
			m_pPlayer->GetPlayerModel()->SetAutoUpdateFlag(false);
			m_pPlayer->SitDown();
		}
	}
	ResizeWindow(NULL);
}

bool CDlgFullScreenEffectShow::Release(){
	A3DRELEASE(m_pPlayer);
	return CDlgBase::Release();
}

void CDlgFullScreenEffectShow::OnHideDialog()
{
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Gfx"));
	if (pImg) pImg->SetGfx(NULL);
	pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Bg"));
	if (pImg) pImg->SetGfx(NULL);
	A3DRELEASE(m_pPlayer);
	CDlgBase::OnHideDialog();
}

void CDlgFullScreenEffectShow::OnTick()
{
	CDlgBase::OnTick();
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Gfx"));
	if (pImg) {
		A3DGFXEx* pGfx = pImg->GetGfx();
		if (pGfx && pGfx->GetState() == ST_STOP) OnCommand_CANCEL(NULL); 
	}
	if (m_pPlayer == NULL) return;
	int nHeight = 0;
	int nLeftRight = 50;
	switch (m_pPlayer->GetProfession()){
	case PROF_WARRIOR:
	case PROF_MAGE:
	case PROF_HAG:
	case PROF_ORC:
		nHeight = 0;
		break;
	case PROF_ARCHOR:
	case PROF_ANGEL:
		nHeight = (static_cast<char>(-20) & 0xff);
		break;
	case PROF_MONK:
	case PROF_GHOST:
	case PROF_JIANLING:
	case PROF_MEILING:
	case PROF_YEYING:
	case PROF_YUEXIAN:
		nHeight = (static_cast<char>(50) & 0xff);
		break;
	default: break;
	}
	int param3 = (nLeftRight << 16) | (0 << 8) | nHeight;
	pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Char"));
	if (m_pPlayer && pImg)
	{
		m_pPlayer->Tick(GetGame()->GetTickTime());
		
		pImg->SetRenderCallback(
			PlayerRenderDemonstration,
			(DWORD)m_pPlayer,
			0,
			param3);
	}
}

void CDlgFullScreenEffectShow::ResizeWindow(A3DRECT *prc)
{
	RECT rc;
	if( prc )
	{
		rc.left = prc->left;
		rc.top = prc->top;
		rc.right = prc->right;
		rc.bottom = prc->bottom;
	}
	else
	{
		A3DVIEWPORTPARAM param = *g_pGame->GetViewport()->GetA3DViewport()->GetParam();
		rc.left = param.X;
		rc.top = param.Y;
		rc.right = param.X + param.Width;
		rc.bottom = param.Y + param.Height;
	}


	const int dw = MAP_DEFAULT_WIDTH; // default width
	const int dh = MAP_DEFAULT_HEIGHT; // default height
	const float dr = (float)dw / dh; // default ratio
	
	const int w = rc.right - rc.left;
	const int h = rc.bottom - rc.top;
	const float r = (float)w / h; // current ratio

	PAUIOBJECT pObj = NULL;

	// resize the dlg size	
	SetSize(w, h);
	SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

	PAUIOBJECT pBG = GetDlgItem("Img_Bg");
	pBG->SetSize( w, h );
	pBG->SetPos( 0, 0 ); 

	pObj = GetDlgItem("Img_Char");
	pObj->SetSize(w,h);
	pObj->SetPos( 0, 0 ); 

	pObj = GetDlgItem("Img_Gfx");
	pObj->SetSize(w,h);
	pObj->SetPos( 0, int(h * .1f) ); 
}

