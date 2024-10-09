// Filename	: DlgLevel2UpgradeShow.cpp
// Creator	: Han Guanghui
// Date		: 2013/10/13

#include "DlgLevel2UpgradeShow.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_LoginPlayer.h"
#include "EC_World.h"
#include "EC_TaoistRank.h"

#include "roleinfo"

#include "AFI.h"
#include "AUIImagePicture.h"
#include "A3DGFXExMan.h"
#include "A3DCamera.h"
#include <A2DSprite.h>

CDlgLevel2UpgradeShow::CDlgLevel2UpgradeShow()
: m_pPlayer(NULL)
, m_pImg_Char(NULL)
, m_nNewLevel2(0)
, m_pointJustReadyGfxPlayed(false)
, m_bFitWindowNextTick(false)
{
	m_pointJustReadyGfxPlayTimer.Reset(1000);
	m_hideTimer.Reset(6000);
}

bool CDlgLevel2UpgradeShow::OnInitDialog()
{
	DDX_Control("Img_Char", m_pImg_Char);
	if (AUIImagePicture *pImgBG = dynamic_cast<AUIImagePicture *>(GetDlgItem("Img_BG"))){
		if (A2DSprite *pImageSprite = pImgBG->GetImage()){
			pImageSprite->SetLinearFilter(true);
		}
	}
	return CDlgBase::OnInitDialog();
}

void CDlgLevel2UpgradeShow::OnShowDialog(){
	if (!CanShow()){
		OnCommand_CANCEL(NULL);
		return;
	}
	CDlgBase::OnShowDialog();
	LoadPlayer();
	PlayGfx();
	FitWindow();
}

void CDlgLevel2UpgradeShow::SetNewLevel2(int nLevel2){
	m_nNewLevel2 = nLevel2;
}

bool CDlgLevel2UpgradeShow::CanShow()const{
	if (const CECTaoistRank* newTaoist = CECTaoistRank::GetTaoistRank(m_nNewLevel2)){
		if (newTaoist->IsGodRank()){
			return newTaoist == CECTaoistRank::GetGodRankBegin();
		}else if (newTaoist->IsEvilRank()){
			return newTaoist == CECTaoistRank::GetEvilRankBegin();
		}else{
			return true;
		}
	}
	return false;
}

void CDlgLevel2UpgradeShow::LoadPlayer(){	
	if (!m_pPlayer)	{
		m_pPlayer = new CECLoginPlayer(GetGame()->GetGameRun()->GetWorld()->GetPlayerMan());
		GNET::RoleInfo info = GetGame()->GetGameRun()->GetSelectedRoleInfo();		
		if (!m_pPlayer->Load(info)){
			m_pPlayer->Release();
			delete m_pPlayer;
			m_pPlayer = NULL;
		}else{
			m_pPlayer->SetPos(A3DVECTOR3(0));
			m_pPlayer->ChangeModelMoveDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0));
			m_pPlayer->GetPlayerModel()->SetAutoUpdateFlag(false);
			m_pPlayer->SitDown();
		}
	}
}

void CDlgLevel2UpgradeShow::ReleasePlayer(){
	m_pImg_Char->SetRenderCallback(NULL, 0);
	A3DRELEASE(m_pPlayer);
}

void CDlgLevel2UpgradeShow::OnHideDialog(){
	ReleasePlayer();
	CDlgBase::OnHideDialog();
}

bool CDlgLevel2UpgradeShow::Release(){
	ReleasePlayer();
	return CDlgBase::Release();
}

bool CDlgLevel2UpgradeShow::CanHide()const{
	return m_hideTimer.IsTimeArrived();
}

void CDlgLevel2UpgradeShow::OnTick(){
	CDlgBase::OnTick();
	if (CanHide()){
		OnCommand_CANCEL(NULL);
		return;
	}
	if (!m_pointJustReadyGfxPlayed && m_pointJustReadyGfxPlayTimer.IsTimeArrived()){
		PlayPointJustReadyGfxAtLevel(m_nNewLevel2);
		m_pointJustReadyGfxPlayed = true;
	}	
	if (m_bFitWindowNextTick){
		FitWindow();
	}
	if (m_pPlayer){
		m_pPlayer->Tick(GetGame()->GetTickTime());
		m_pImg_Char->SetRenderCallback(PlayerRenderDemonstration, (DWORD)m_pPlayer, 0, GetCameraParameter());
	}
}

DWORD CDlgLevel2UpgradeShow::GetCameraParameter()const{	
	int nLeftRight = 50;
	int nHeight = 0;
	switch (m_pPlayer->GetRace()){
	case RACE_HUMAN:
	case RACE_ELF:
		nHeight = -50;
		break;
	case RACE_ORC:
		nHeight = -40;
		break;
	case RACE_GHOST:
	case RACE_LING:
	case RACE_OBORO:
		nHeight = 50;
		break;
	}
	int nDistance = 0;
	switch (m_pPlayer->GetRace()){
	case RACE_HUMAN:
	case RACE_ORC:
	case RACE_ELF:
		nDistance = -10;
		break;
	case RACE_GHOST:
	case RACE_LING:
		nDistance = -30;
		break;
	case RACE_OBORO:
		nDistance = -60;
		break;
	}
	return ((nLeftRight&0xff) << 16) | ((nDistance&0xff) << 8) | (nHeight & 0xff);
}

void CDlgLevel2UpgradeShow::FitWindow(){
	float fScaleX = GetAUIManager()->GetRect().Width() / (float)GetDefaultSize().cx;
	float fScaleY = GetAUIManager()->GetRect().Height() / (float)GetDefaultSize().cy;
	float fScale = a_Min(fScaleX, fScaleY);
	float fDialogScale = fScale / GetAUIManager()->GetWindowScale();
	SetScale(fDialogScale);
	if (PAUIOBJECT pObj = GetDlgItem("Img_PureBlack")){
		int width = GetAUIManager()->GetRect().Width();
		int height = GetAUIManager()->GetRect().Height();
		pObj->SetSize(width+2, height+2);
		pObj->SetPos((GetRect().Width()-width)/2-1, (GetRect().Height()-height)/2-1);
	}
	m_bFitWindowNextTick = false;
}

void CDlgLevel2UpgradeShow::Resize(A3DRECT rcOld, A3DRECT rcNew){
	CDlgBase::Resize(rcOld, rcNew);
	SetPosEx(0, 0, alignCenter, alignCenter);
	m_bFitWindowNextTick = true;
}

void CDlgLevel2UpgradeShow::PlayGfx(){
	PlayBaseRankGfx();
	PlayGodRankGfx();
	PlayEvilRankGfx();
	m_pointJustReadyGfxPlayTimer.Reset();
	m_pointJustReadyGfxPlayed = false;
	m_hideTimer.Reset();
}

void CDlgLevel2UpgradeShow::PlayBaseRankGfx(){
	const CECTaoistRank* it = CECTaoistRank::GetBaseRankBegin();
	while (it){
		if (it->GetID() < m_nNewLevel2){
			PlayPointReadyGfxAtLevel(it->GetID());
			PlayLineReadyGfxToLevel(it->GetID());
		}else if (it->GetID() == m_nNewLevel2){
			PlayLineJustReadyGfxToLevel(it->GetID());
		}else{
			PlayPointNotReadyGfxAtLevel(it->GetID());
			PlayLineNotReadyGfxToLevel(it->GetID());
		}
		it = it->GetNext();
	}
}

void CDlgLevel2UpgradeShow::PlayGodRankGfx(){
	const CECTaoistRank* newRank = CECTaoistRank::GetTaoistRank(m_nNewLevel2);
	if (newRank->IsGodRank()){
		const CECTaoistRank* it = CECTaoistRank::GetGodRankBegin();
		while (it){
			if (it->GetID() < m_nNewLevel2){
				PlayPointReadyGfxAtLevel(it->GetID());
				PlayLineReadyGfxToLevel(it->GetID());
			}else if (it->GetID() == m_nNewLevel2){
				PlayLineJustReadyGfxToLevel(it->GetID());
			}else{
				PlayPointNotReadyGfxAtLevel(it->GetID());
				PlayLineNotReadyGfxToLevel(it->GetID());
			}
			it = it->GetNext();
		}
	}else{
		const CECTaoistRank* it = CECTaoistRank::GetGodRankBegin();
		while (it){
			PlayPointNotReadyGfxAtLevel(it->GetID());
			PlayLineNotReadyGfxToLevel(it->GetID());
			it = it->GetNext();
		}
	}
}

void CDlgLevel2UpgradeShow::PlayEvilRankGfx(){
	const CECTaoistRank* newRank = CECTaoistRank::GetTaoistRank(m_nNewLevel2);
	if (newRank->IsEvilRank()){
		const CECTaoistRank* it = CECTaoistRank::GetEvilRankBegin();
		while (it){
			if (it->GetID() < m_nNewLevel2){
				PlayPointReadyGfxAtLevel(it->GetID());
				PlayLineReadyGfxToLevel(it->GetID());
			}else if (it->GetID() == m_nNewLevel2){
				PlayLineJustReadyGfxToLevel(it->GetID());
			}else{
				PlayPointNotReadyGfxAtLevel(it->GetID());
				PlayLineNotReadyGfxToLevel(it->GetID());
			}
			it = it->GetNext();
		}
	}else{
		const CECTaoistRank* it = CECTaoistRank::GetEvilRankBegin();
		while (it){
			PlayPointNotReadyGfxAtLevel(it->GetID());
			PlayLineNotReadyGfxToLevel(it->GetID());
			it = it->GetNext();
		}
	}
}

void CDlgLevel2UpgradeShow::PlayPointReadyGfxAtLevel(int iLevel2){
	if (AUIImagePicture * pImage = GetPointControlAtLevel(iLevel2)){
		pImage->SetGfx(GetPointReadyGfxFile());
	}
}

void CDlgLevel2UpgradeShow::PlayPointJustReadyGfxAtLevel(int iLevel2){
	if (AUIImagePicture * pImage = GetPointControlAtLevel(iLevel2)){
		pImage->SetGfx(GetPointJustReadyGfxFile());
	}
}

void CDlgLevel2UpgradeShow::PlayPointNotReadyGfxAtLevel(int iLevel2){
	if (AUIImagePicture * pImage = GetPointControlAtLevel(iLevel2)){
		pImage->SetGfx(GetPointNotReadyGfxFile());
	}
}

void CDlgLevel2UpgradeShow::PlayLineReadyGfxToLevel(int iLevel2){
	if (AUIImagePicture * pImage = GetLineControlToLevel(iLevel2)){
		pImage->SetGfx(GetLineReadyGfxFile(iLevel2));
	}
}

void CDlgLevel2UpgradeShow::PlayLineNotReadyGfxToLevel(int iLevel2){
	if (AUIImagePicture * pImage = GetLineControlToLevel(iLevel2)){
		pImage->SetGfx(GetLineNotReadyGfxFile(iLevel2));
	}
}

void CDlgLevel2UpgradeShow::PlayLineJustReadyGfxToLevel(int iLevel2){
	if (AUIImagePicture * pImage = GetLineControlToLevel(iLevel2)){
		pImage->SetGfx(GetLineJustReadyGfxFile(iLevel2));
	}
}

AUIImagePicture * CDlgLevel2UpgradeShow::GetPointControlAtLevel(int iLevel2){
	AUIImagePicture *result = NULL;
	int index = GetPointControlIndexForLevel(iLevel2);
	if (index >= 0){
		AString strName;
		strName.Format("Gfx_Point%02d", index);
		result = dynamic_cast<AUIImagePicture *>(GetDlgItem(strName));
	}
	return result;
}

AUIImagePicture * CDlgLevel2UpgradeShow::GetLineControlToLevel(int iLevel2){
	AUIImagePicture *result = NULL;
	int index = GetLineControlIndexForLevel(iLevel2);
	if (index >= 0){
		AString strName;
		strName.Format("Gfx_Line%02d", index);
		result = dynamic_cast<AUIImagePicture *>(GetDlgItem(strName));
	}
	return result;
}

int CDlgLevel2UpgradeShow::GetPointControlIndexForLevel(int iLevel2){
	int result(-1);
	const CECTaoistRank * p = CECTaoistRank::GetTaoistRank(iLevel2);
	if (p->IsBaseRank()){
		result = iLevel2+1;	//	1 ~ 9
	}else if (iLevel2 == CECTaoistRank::GetGodRankBegin()->GetID()){
		result = 10;
	}else if (iLevel2 == CECTaoistRank::GetEvilRankBegin()->GetID()){
		result = 11;
	}
	return result;
}

int CDlgLevel2UpgradeShow::GetLineControlIndexForLevel(int iLevel2){
	int result(-1);
	const CECTaoistRank * p = CECTaoistRank::GetTaoistRank(iLevel2);
	if (p->IsBaseRank()){
		result = iLevel2;	//	1 ~ 8
	}else if (iLevel2 == CECTaoistRank::GetGodRankBegin()->GetID()){
		result = 9;
	}else if (iLevel2 == CECTaoistRank::GetEvilRankBegin()->GetID()){
		result = 10;
	}
	return result;
}

const char * CDlgLevel2UpgradeShow::GetPointReadyGfxFile(){
	return "界面\\修真界面_点爆炸_持续01.gfx";
}

const char * CDlgLevel2UpgradeShow::GetPointNotReadyGfxFile(){
	return "";
}

const char * CDlgLevel2UpgradeShow::GetPointJustReadyGfxFile(){
	return "界面\\修真界面_点爆炸_起始01.gfx";
}

static const int LINE_GFX_COUNT = 10;
const char * CDlgLevel2UpgradeShow::GetLineReadyGfxFile(int iLevel2){
	static const char *s_szLineGfx[LINE_GFX_COUNT] = {"界面\\修真界面_竖条_持续01.gfx",
		"界面\\修真界面_竖条_持续02.gfx",
		"界面\\修真界面_竖条_持续03.gfx",
		"界面\\修真界面_竖条_持续04.gfx",
		"界面\\修真界面_竖条_持续05.gfx",
		"界面\\修真界面_竖条_持续06.gfx",
		"界面\\修真界面_竖条_持续07.gfx",
		"界面\\修真界面_竖条_持续08.gfx",
		"界面\\修真界面_竖条_持续09.gfx",
		"界面\\修真界面_竖条_持续10.gfx",
	};	
	int index = GetLineControlIndexForLevel(iLevel2);
	if (index >=1  && index <= LINE_GFX_COUNT){
		return s_szLineGfx[index-1];
	}
	return "";
}

const char * CDlgLevel2UpgradeShow::GetLineNotReadyGfxFile(int iLevel2){
	return "";
}

const char * CDlgLevel2UpgradeShow::GetLineJustReadyGfxFile(int iLevel2){
	static const char *s_szLineGfx[LINE_GFX_COUNT] = {"界面\\修真界面_竖条_起始01.gfx",
		"界面\\修真界面_竖条_起始02.gfx",
		"界面\\修真界面_竖条_起始03.gfx",
		"界面\\修真界面_竖条_起始04.gfx",
		"界面\\修真界面_竖条_起始05.gfx",
		"界面\\修真界面_竖条_起始06.gfx",
		"界面\\修真界面_竖条_起始07.gfx",
		"界面\\修真界面_竖条_起始08.gfx",
		"界面\\修真界面_竖条_起始09.gfx",
		"界面\\修真界面_竖条_起始10.gfx",
	};	
	int index = GetLineControlIndexForLevel(iLevel2);
	if (index >=1  && index <= LINE_GFX_COUNT){
		return s_szLineGfx[index-1];
	}
	return "";
}
