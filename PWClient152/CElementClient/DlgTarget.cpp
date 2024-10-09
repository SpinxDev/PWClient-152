// Filename	: DlgTarget.cpp
// Creator	: Tom Zhou
// Date		: October 12, 2005

#include "DlgTarget.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_Monster.h"
#include "EC_NPC.h"
#include "EC_World.h"
#include "EC_Faction.h"
#include "EC_Skill.h"
#include "AFI.h"
#include "AUILabel.h"
#include "AUICommon.h"
#include "AUICheckBox.h"
#include "AUIProgress.h"
#include "elementdataman.h"
#include "A2DSprite.h"
#include "EC_GameSession.h"
#include "EC_ForceMgr.h"
#include "DlgHost.h"
#include "EC_CountryConfig.h"
#include "auto_delete.h"
#include "EC_ElsePlayer.h"
#include "EC_UIHelper.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTarget, CDlgBase)

AUI_ON_COMMAND("change",		OnCommand_change)
AUI_ON_COMMAND("Size",			OnCommand_size)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("traceplayer",	OnCommand_traceplayer)
AUI_ON_COMMAND("tracepet",		OnCommand_tracepet)
AUI_ON_COMMAND("showhp",		OnCommand_showhp)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTarget, CDlgBase)

AUI_ON_EVENT("Size",		WM_MOUSEMOVE,		OnEventMouseMove)
AUI_ON_EVENT("Size",		WM_LBUTTONDOWN,		OnEventLButtonDown_Size)
AUI_ON_EVENT("Size",		WM_LBUTTONUP,		OnEventLButtonUp_Size)

AUI_ON_EVENT("Size",		WM_LBUTTONDBLCLK,	OnEventLButtonDBlclk_Size)

AUI_ON_EVENT("Size",		WM_RBUTTONDOWN,		OnEventRButtonDown_size)
AUI_ON_EVENT("Size",		WM_RBUTTONUP,		OnEventRButtonUp_size)

AUI_ON_EVENT("Size",		WM_RBUTTONDBLCLK,	OnEventRButtonDBlclk_Size)

AUI_ON_EVENT("Size",		WM_MBUTTONDOWN,		OnEventLButtonDown_Size)
AUI_ON_EVENT("Size",		WM_MBUTTONUP,		OnEventLButtonUp_Size)

AUI_ON_EVENT("Size",		WM_MBUTTONDBLCLK,	OnEventLButtonDBlclk_Size)

AUI_END_EVENT_MAP()

int CDlgTarget::m_nMouseLastX = 0;
int CDlgTarget::m_nMouseOffset = 0;
int CDlgTarget::m_nMouseOffsetThis = 0;
bool CDlgTarget::m_bShowFaction = true;
bool CDlgTarget::m_bShowPercent = true;
bool CDlgTarget::m_bShowMonsterHP = true;
A2DSprite* CDlgTarget::m_pA2DSpritePetLevel = NULL;

CDlgTarget::CDlgTarget()
{
}

CDlgTarget::~CDlgTarget()
{
	A3DRELEASE(m_pA2DSpritePetLevel);
}

bool CDlgTarget::OnInitDialog()
{
	int i;

	if( !m_pA2DSpritePetLevel )
	{
		m_pA2DSpritePetLevel = new A2DSprite;
		if( !m_pA2DSpritePetLevel ) return AUI_ReportError(__LINE__, __FILE__);

		bool bval = m_pA2DSpritePetLevel->Init(m_pA3DDevice, "Pet\\PetLevel.tga", 0);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

		int nWidth = m_pA2DSpritePetLevel->GetWidth();
		int nHeight = m_pA2DSpritePetLevel->GetHeight() / 3;

		A3DRECT a_rc[3];
		for( i = 0; i < 3; i++ )
			a_rc[i].SetRect(0, i * nHeight, nWidth, (i + 1) * nHeight);
		bval = m_pA2DSpritePetLevel->ResetItems(3, a_rc);
		if (!bval) return AUI_ReportError(__LINE__, __FILE__);
	}

	AString strName;
	for(i = 0; ; i++ )
	{
		strName.Format("St_%d", i + 1);
		PAUIIMAGEPICTURE pIcon = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( !pIcon ) break;
		strName.Format("St_Txt_%d", i + 1);
		PAUIOBJECT pObj = GetDlgItem(strName);
		pIcon->SetDataPtr(pObj, "AUIObject");
		m_vecImgState.push_back(pIcon);
	}

	// 记录主动攻击和非主动攻击的图片
	if (AString("Win_HpOther") == GetName() || AString("Win_HpOtherB") == GetName()) {
		GetDlgItem("Img_BackAA")->Show(false);
		GetDlgItem("Img_AA")->Show(false);
		/*
		GetDlgItem("Img_BackAA")->GetProperty("Image File", &m_aggressiveHPImg);
		GetDlgItem("Back")->GetProperty("Image File", &m_unaggressiveHPImg);
		GetDlgItem("Img_AA")->GetProperty("Image File", &m_aggressiveSymbolImg);
		GetDlgItem("Img_PA")->GetProperty("Image File", &m_unaggressiveSymbolImg);
		*/
		m_bAggressive = false;
	}

	return true;
}

void CDlgTarget::OnCommand_change(const char * szCommand)
{
	m_bShowFaction = !m_bShowFaction;
}

void CDlgTarget::OnCommand_size(const char * szCommand)
{
	m_bShowPercent = !m_bShowPercent;
}

void CDlgTarget::OnCommand_CANCEL(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgTarget::OnCommand_tracepet(const char * szCommand)
{
	int idSelObj = GetHostPlayer()->GetSelectedTarget();
	CECPlayerMan *pPlayerMan = GetWorld()->GetPlayerMan();
	CECPlayer *pPlayer = (CECPlayer *)pPlayerMan->GetPlayer(idSelObj);
	if( !pPlayer )
	{
		return;
	}
	GetHostPlayer()->SelectTarget(pPlayer->GetCurPetID());
}

void CDlgTarget::OnCommand_traceplayer(const char * szCommand)
{
	int idSelObj = GetHostPlayer()->GetSelectedTarget();
	CECNPC *pNPC = GetWorld()->GetNPCMan()->GetNPC(idSelObj);
	if (!pNPC)
	{
		return;
	}
	GetHostPlayer()->SelectTarget(pNPC->GetMasterID());
}

void CDlgTarget::OnCommand_showhp(const char *szCommand)
{
	m_bShowMonsterHP = !m_bShowMonsterHP;
}

void CDlgTarget::RefreshTargetStat()
{
	int idSelObj = GetHostPlayer()->GetSelectedTarget();
	bool bCheck = ((PAUICHECKBOX)m_pAUIManager->GetDialog(
		"Win_HideHP")->GetDlgItem("Chk_CloseHP"))->IsChecked();

	// dismiss if target is not selectable
	if( idSelObj == 0 || bCheck || !GetHostPlayer()->CanSelectTarget(idSelObj) || GetGameUIMan()->IsCustomizeCharacter())
	{
		PAUIDIALOG pShow = m_pAUIManager->GetDialog("Win_HpOther");
		if( pShow->IsShow() ) pShow->Show(false);
		
		pShow = m_pAUIManager->GetDialog("Win_HpOtherB");
		if( pShow->IsShow() ) pShow->Show(false);
		
		pShow = m_pAUIManager->GetDialog("Win_HpmpOther");
		if( pShow->IsShow() ) pShow->Show(false);

		pShow = m_pAUIManager->GetDialog("Win_HpmpOtherB");
		if( pShow->IsShow() ) pShow->Show(false);

		RefreshTargetOfTarget(0);

		return;
	}

	if (ISPLAYERID(idSelObj)){
		RefreshAsPlayer(idSelObj);
	}else if (ISNPCID(idSelObj)){
		CECNPC *pNPC = GetWorld()->GetNPCMan()->GetNPC(idSelObj);
		if (pNPC && pNPC->GetClonedMaster()){
			RefreshAsPlayer(idSelObj);
		}else{
			RefreshASNPC(idSelObj);
		}
	}

	if (ISPLAYERID(idSelObj)) {
		CECPlayer* pTarget = GetWorld()->GetPlayerMan()->GetPlayer(idSelObj);
		if (!pTarget || pTarget == GetHostPlayer()) {
			RefreshTargetOfTarget(0);
		} else {
			RefreshTargetOfTarget(pTarget->GetSelectedTarget());
		}
	} else if (ISNPCID(idSelObj)) {
		CECNPC *pTarget = GetWorld()->GetNPCMan()->GetNPC(idSelObj);
		RefreshTargetOfTarget(pTarget ? pTarget->GetSelectedTarget() : 0);
	} 
}

class CDlgTargetShowPlayerPolicy{
public:
	virtual ~CDlgTargetShowPlayerPolicy()=0{}
	virtual bool Exist()const{ return false; }
	virtual bool HasPet()const{ return false; }
	virtual int	 GetCurHP()const{ return 0; }
	virtual int  GetMaxHP()const{ return 1; }
	virtual void SetRenderCallback(PAUIIMAGEPICTURE pImage, DWORD dwRotate){}
	virtual bool GetShowName()const{ return false; }
	virtual ACString GetNameToShow()const{ return _AL(""); }
	virtual DWORD GetNameColor()const=0;
	virtual int  GetFactionID()const{ return 0; }
	virtual const S2C::IconStates * GetIconStates() const{ return NULL; }
	virtual int  GetProfession()const=0;
	virtual int	 GetGender()const=0;
	virtual int	 GetForce()const{ return 0;}
	virtual int  GetLevel() const = 0;
	static CDlgTargetShowPlayerPolicy * Create(int idSelObj);
};

class CDlgTargetShowPlayerAsPlayerPolicy : public CDlgTargetShowPlayerPolicy{
protected:
	CECPlayer *	m_pPlayer;

	CECWorld * GetWorld()const{
		return g_pGame->GetGameRun()->GetWorld();
	}
public:
	CDlgTargetShowPlayerAsPlayerPolicy(int idSelObj)
		: m_pPlayer(NULL)
	{
		if (idSelObj){
			m_pPlayer = GetWorld()->GetPlayerMan()->GetPlayer(idSelObj);
		}
	}
	virtual bool Exist()const{
		return m_pPlayer != NULL;
	}
	virtual bool HasPet()const{
		return m_pPlayer->GetCurPetID()
			&& GetWorld()->GetNPCMan()->GetNPC(m_pPlayer->GetCurPetID());
	}
	virtual int	 GetCurHP()const{
		return m_pPlayer->GetBasicProps().iCurHP;
	}
	virtual int  GetMaxHP()const{
		return max(m_pPlayer->GetExtendProps().bs.max_hp, 1);
	}
	virtual void SetRenderCallback(PAUIIMAGEPICTURE pImage, DWORD dwRotate){
		if (pImage){
			pImage->SetRenderCallback(PlayerRenderPortrait,	(DWORD)m_pPlayer, dwRotate);
		}
	}
	virtual bool GetShowName()const{
		return m_pPlayer->GetShowName();
	}
	virtual ACString GetNameToShow()const{
		ACHAR szText[300];
		AUI_ConvertChatString(g_pGame->GetGameRun()->GetPlayerName(m_pPlayer->GetCharacterID(), true), szText);
		return szText;
	}
	virtual DWORD GetNameColor()const{
		return m_pPlayer->GetNameColor();
	}
	virtual int  GetFactionID()const{
		return m_pPlayer->GetFactionID();
	}
	virtual const S2C::IconStates * GetIconStates() const{
		return &m_pPlayer->GetIconStates();
	}
	virtual int GetProfession()const{
		return m_pPlayer->GetProfession();
	}
	virtual int	GetGender()const{
		return m_pPlayer->GetGender();
	}
	virtual int GetForce()const{
		return m_pPlayer->GetForce();
	}
	virtual int GetLevel()const{
		return m_pPlayer->GetBasicProps().iLevel;
	}
};

class CDlgTargetShowNPCAsPlayerPolicy : public CDlgTargetShowPlayerAsPlayerPolicy{
	CECNPC	*	m_pNPC;
public:
	CDlgTargetShowNPCAsPlayerPolicy(int idSelObj)
		: CDlgTargetShowPlayerAsPlayerPolicy(0)
		, m_pNPC(NULL)
	{
		if (idSelObj){
			if (m_pNPC = GetWorld()->GetNPCMan()->GetNPC(idSelObj)){
				m_pPlayer = m_pNPC->GetClonedMaster();
			}
		}
	}
	virtual int	 GetCurHP()const{
		return m_pNPC->GetBasicProps().iCurHP;
	}
	virtual int  GetMaxHP()const{
		return max(m_pNPC->GetExtendProps().bs.max_hp, 1);
	}
	virtual ACString GetNameToShow()const{
		return m_pNPC->GetNameToShow();
	}
	virtual const S2C::IconStates * GetIconStates() const{
		return &m_pNPC->GetIconStates();
	}
	virtual int  GetLevel()const{
		return m_pNPC->GetBasicProps().iLevel;
	}
};


CDlgTargetShowPlayerPolicy * CDlgTargetShowPlayerPolicy::Create(int idSelObj){
	return ISNPCID(idSelObj)
		? new CDlgTargetShowNPCAsPlayerPolicy(idSelObj)
		: new CDlgTargetShowPlayerAsPlayerPolicy(idSelObj);
}

void CDlgTarget::RefreshAsPlayer(int idSelObj){
	PAUIDIALOG pShow = NULL;
	if( m_bShowFaction )
	{
		pShow = m_pAUIManager->GetDialog("Win_HpmpOtherB");
		m_pAUIManager->GetDialog("Win_HpmpOther")->Show(false);
	}
	else
	{
		pShow = m_pAUIManager->GetDialog("Win_HpmpOther");
		m_pAUIManager->GetDialog("Win_HpmpOtherB")->Show(false);
	}
	m_pAUIManager->GetDialog("Win_HpOther")->Show(false);
	m_pAUIManager->GetDialog("Win_HpOtherB")->Show(false);
	
	CDlgTargetShowPlayerPolicy *pShowPolicy = CDlgTargetShowPlayerPolicy::Create(idSelObj);
	auto_delete<CDlgTargetShowPlayerPolicy> _dummy(pShowPolicy);
	if (!pShowPolicy->Exist()){
		pShow->Show(false);
		return;
	}
	if (pShow && pShow->GetDlgItem("Btn_Pet")){
		pShow->GetDlgItem("Btn_Pet")->Show(pShowPolicy->HasPet());
	}	
	
	PAUIIMAGEPICTURE pImage = dynamic_cast<PAUIIMAGEPICTURE>(pShow->GetDlgItem("Size"));
	if (GetHostPlayer()->IsAllResReady()){
		pShowPolicy->SetRenderCallback(pImage, m_nMouseOffset + m_nMouseOffsetThis);
	}else{
		pImage->SetRenderCallback(NULL, 0);
	}
		
	ACString strText;
	if (pShowPolicy->GetShowName()){
		strText = pShowPolicy->GetNameToShow();
	}
	PAUIOBJECT pObj = pShow->GetDlgItem("Txt_Name");
	pObj->SetText(strText);
	pObj->SetColor(pShowPolicy->GetNameColor());
	
	if( m_bShowFaction )
	{
		strText.Empty();
		if (pShowPolicy->GetShowName())
		{
			Faction_Info *pInfo = NULL;
			int idFaction = pShowPolicy->GetFactionID();
			CECFactionMan *pFMan = GetGame()->GetFactionMan();
			if( idFaction ) pInfo = pFMan->GetFaction(idFaction);
			if( idFaction <= 0 )
				strText = GetStringFromTable(251);
			else if( pInfo )
				strText = pInfo->GetName();
		}		
		pObj = pShow->GetDlgItem("Txt_Faction");
		pObj->SetText(strText);
	}
	
	CDlgTarget* pTarget = dynamic_cast<CDlgTarget*>(pShow);
	
	CECIconStateMgr* pMgr = GetGameUIMan()->GetIconStateMgr();
	if(pTarget && pMgr)
	{
		pMgr->RefreshStateIcon( pTarget->m_vecImgState.begin(), pTarget->m_vecImgState.size(), pShowPolicy->GetIconStates(),
			// two rows in Player target
			IconLayoutBig(pTarget->m_vecImgState.size() / 2), false );
	}
	
	PAUIIMAGEPICTURE pImagePic = (PAUIIMAGEPICTURE)pShow->GetDlgItem("Image_Prof");
	pImagePic->SetHint(GetGameRun()->GetProfName(pShowPolicy->GetProfession()));
	pImagePic->FixFrame(pShowPolicy->GetProfession() + pShowPolicy->GetGender() * NUM_PROFESSION);
	
	// refresh the force icon
	if(pTarget) pTarget->RefreshForceStatus(pShowPolicy);	
	
	if (!pShow->IsShow()){
		pShow->Show(true, false, false);
	}
	
	int iCurHP = pShowPolicy->GetCurHP();
	int iMaxHP = pShowPolicy->GetMaxHP();
	if( iCurHP > 0 && iCurHP < iMaxHP / 40 ) iCurHP = iMaxHP / 40;
	PAUIPROGRESS pProgress = (PAUIPROGRESS)pShow->GetDlgItem("Prgs_HP");
	pProgress->SetProgress(int((double)iCurHP * AUIPROGRESS_MAX / iMaxHP));
}

void CDlgTarget::RefreshASNPC(int idSelObj){
	PAUIDIALOG pShow = NULL;
	if( m_bShowPercent )
	{
		pShow = m_pAUIManager->GetDialog("Win_HpOtherB");
		m_pAUIManager->GetDialog("Win_HpOther")->Show(false);
	}
	else
	{
		pShow = m_pAUIManager->GetDialog("Win_HpOther");
		m_pAUIManager->GetDialog("Win_HpOtherB")->Show(false);
	}
	m_pAUIManager->GetDialog("Win_HpmpOther")->Show(false);
	m_pAUIManager->GetDialog("Win_HpmpOtherB")->Show(false);
	
	CECNPC *pNPC = GetWorld()->GetNPCMan()->GetNPC(idSelObj);
	if( !pNPC )
	{
		pShow->Show(false);
		return;
	}
	
	const ROLEBASICPROP& bp = pNPC->GetBasicProps();
	const ROLEEXTPROP& ep = pNPC->GetExtendProps();
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pShow->GetDlgItem("Img_PetLevel");
	pImage->Show(false);
	pImage->SetHint(_AL(""));
	SetShowLevel(pShow, 0); // hide at default
	if (pNPC->IsPetNPC())
	{
		if (pShow && pShow->GetDlgItem("Btn_Player"))
		{
			pShow->GetDlgItem("Btn_Player")->Show(true);
		}
	}
	else
	{
		if (pShow && pShow->GetDlgItem("Btn_Player"))
		{
			pShow->GetDlgItem("Btn_Player")->Show(false);
		}
	}
	if( pNPC->IsMonsterNPC() )
	{
		elementdataman* pElementDataMan = GetGame()->GetElementDataMan();
		DATA_TYPE dt;
		MONSTER_ESSENCE* pEssence = (MONSTER_ESSENCE*)pElementDataMan->get_data_ptr(
			pNPC->GetTemplateID(), ID_SPACE_ESSENCE,dt); 
		if (pEssence && dt == DT_MONSTER_ESSENCE )
		{
			int nShowLevel = pEssence->show_level;
			if( nShowLevel )
			{
				SetShowLevel(pShow, nShowLevel);
			}
			if( GetHostPlayer()->GetProfession() == PROF_HAG && bp.iLevel != 150 )
			{
				int nHostLevel = GetHostPlayer()->GetBasicProps().iLevel;
				if( pEssence->id_pet_egg_captured != 0 )
				{
					if( pEssence->level > nHostLevel )
					{
						pImage->SetCover(m_pA2DSpritePetLevel, 2);
						pImage->SetHint(GetStringFromTable(777));
					}
					else if( pEssence->level < nHostLevel )
					{
						pImage->SetCover(m_pA2DSpritePetLevel, 0);
						pImage->SetHint(GetStringFromTable(775));
					}
					else if( pEssence->level == nHostLevel )
					{
						pImage->SetCover(m_pA2DSpritePetLevel, 1);
						pImage->SetHint(GetStringFromTable(776));
					}
					pImage->Show(true);
				}
			}
			if (pEssence->aggressive_mode == 1) {
				((CDlgTarget*)pShow)->SetAggressive(true);
			} else {
				((CDlgTarget*)pShow)->SetAggressive(false);				
			}
		}
	} else {
		((CDlgTarget*)pShow)->SetAggressive(false);	
	}
	
	
	int iCurHP = bp.iCurHP;
	int iMaxHP = max(ep.bs.max_hp, 1);
	
	ACHAR szName[40];
	ACString strText;
	const ACHAR *pszName = pNPC->GetNameToShow();
	PAUIOBJECT pName = pShow->GetDlgItem("Txt_Name");
	
	AUI_ConvertChatString(pszName, szName);
	if( pNPC->IsServerNPC() || pNPC->IsPetNPC() )
		strText = szName;
	else if( bp.iLevel == 150 )
		strText.Format(_AL("[?] %s"), szName);
	else
		strText.Format(_AL("[%d] %s"), bp.iLevel, szName);
	pShow->GetDlgItem("Img_Demon")->Show(bp.iLevel == 150);
	pName->SetText(strText);
	pName->SetColor(pNPC->GetNameColor());
	
	CECIconStateMgr* pMgr = GetGameUIMan()->GetIconStateMgr();
	CDlgTarget* pTarget = dynamic_cast<CDlgTarget*>(pShow);
	if(pTarget && pMgr)
	{
		pMgr->RefreshStateIcon( pTarget->m_vecImgState.begin(), pTarget->m_vecImgState.size(), &pNPC->GetIconStates(),
			// only one row in NPC target
			IconLayoutBig(pTarget->m_vecImgState.size()), false );
	}
	
	int j;
	PAUILABEL pLabel;
	ACString strProp = _AL("");
	
	if( m_bShowPercent )
	{
		if( pNPC->IsMonsterNPC() )
		{
			const MONSTER_ESSENCE *pEssence = ((CECMonster *)pNPC)->GetDBEssence();
			if( a_strlen(pEssence->prop) > 0 )
			{
				pLabel = (PAUILABEL)pShow->GetDlgItem("Txt_El");
				pLabel->SetText(GetStringFromTable(517 + a_atoi(pEssence->prop)));
			}
			
			ACString strLabel = pEssence->label;
			strLabel.TrimLeft();
			strLabel.TrimRight();
			if (!strLabel.IsEmpty()){
				strProp += _AL(",");
				strProp += strLabel;
			}
			
			if( (pEssence->immune_type & 63) == 63 )
			{
				strProp += _AL(",");
				strProp += GetStringFromTable(358);
			}
			else if( (pEssence->immune_type & 62) == 62 )
			{
				strProp += _AL(",");
				strProp += GetStringFromTable(357);
			}
			else
				for( j = 0; j < 6 ; j++ )
					if( (pEssence->immune_type & ( 1 << j ) ) != 0 )
					{
						strProp += _AL(",");
						strProp += GetStringFromTable(351 + j);
					}
					
					const ROLEBASICPROP& bp = GetHostPlayer()->GetBasicProps();
					const ROLEEXTPROP& ep = GetHostPlayer()->GetExtendProps();
					int nNicetyMe = ep.ak.attack;
					int nEvadeMe = ep.df.armor;
					int nNicetyHim = pEssence->attack;
					int nEvadeHim = pEssence->armor;
					int nPercent1 = (100 * nNicetyMe) / (nNicetyMe + nEvadeHim / 2);
					int nPercent2 = 100 - (100 * nNicetyHim) / (nNicetyHim + nEvadeMe / 2);
					
					pLabel = (PAUILABEL)pShow->GetDlgItem("Txt_Info");
					strText.Format(GetStringFromTable(500), nPercent1, '%', nPercent2, '%');
					pLabel->SetText(strText);
		}
		else
		{
			pShow->GetDlgItem("Txt_El")->SetText(_AL(""));
			pShow->GetDlgItem("Txt_Info")->SetText(_AL(""));
		}
	}
	if( pNPC->GetRandomProp() > 0 )
	{
		strProp += _AL(",");
		strProp += GetStringFromTable(340 + pNPC->GetRandomProp());
	}
	if( strProp.GetLength() > 0 )
		strProp.CutLeft(a_strlen(_AL(",")));
	pLabel = (PAUILABEL)pShow->GetDlgItem("Txt_Att");
	pLabel->SetText(strProp);
	
	pLabel = (PAUILABEL)pShow->GetDlgItem("Txt_HP");
	if (m_bShowMonsterHP && pNPC->IsMonsterNPC())
	{
		pLabel->Show(true);
		strProp.Format(_AL("%d/%d"), iCurHP, iMaxHP);
		pLabel->SetText(strProp);
	}
	else
	{
		pLabel->Show(false);
	}

	if( !pShow->IsShow() ) pShow->Show(true, false, false);
	
	PAUIPROGRESS pProgress = (PAUIPROGRESS)pShow->GetDlgItem("Prgs_HP");
	if( iCurHP > 0 && iCurHP < iMaxHP / 40 ) iCurHP = iMaxHP / 40;
	pProgress->SetProgress(int((double)iCurHP * AUIPROGRESS_MAX / iMaxHP));
}

void CDlgTarget::RefreshTargetOfTarget(int idTarget) {
	if (GetGameUIMan()->IsShowTargetOfTarget()) {
		CDlgTargetOfTarget::ShowTargetOfTarget(idTarget);
	} else {
		CDlgTargetOfTarget::ShowTargetOfTarget(0);
	}
}

void CDlgTarget::OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( (wParam & MK_LBUTTON) )
	{
		m_nMouseOffsetThis = GET_X_LPARAM(lParam) - m_nMouseLastX;
		if( m_nMouseOffset + m_nMouseOffsetThis > 70 )
			m_nMouseOffsetThis = 70 - m_nMouseOffset;
		else if( m_nMouseOffset + m_nMouseOffsetThis < -70 )
			m_nMouseOffsetThis = -70 - m_nMouseOffset;
	}
}

void CDlgTarget::OnEventLButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(pObj);
	ChangeFocus(GetDlgItem("Image_Prof"));
	
	m_nMouseLastX = GET_X_LPARAM(lParam);
	m_nMouseOffset += m_nMouseOffsetThis;
	m_nMouseOffsetThis = 0;
}

void CDlgTarget::OnEventLButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(NULL);
	ChangeFocus(GetDlgItem("Image_Prof"));
}

void CDlgTarget::OnEventRButtonDown_size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ChangeFocus(GetDlgItem("Image_Prof"));
}

void CDlgTarget::OnEventRButtonUp_size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ChangeFocus(GetDlgItem("Image_Prof"));
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	GetGameUIMan()->PopupPlayerContextMenu(GetHostPlayer()->GetSelectedTarget(), x, y);
}

void CDlgTarget::OnEventLButtonDBlclk_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ChangeFocus(GetDlgItem("Image_Prof"));
}

void CDlgTarget::OnEventRButtonDBlclk_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ChangeFocus(GetDlgItem("Image_Prof"));
}

void CDlgTarget::SetShowLevel(PAUIDIALOG pShow, int nShowLevel)
{
	if(!pShow)
	{
		ASSERT(false);
		return;
	}

	PAUIOBJECT pLastLeft = NULL;
	PAUIOBJECT pLastRight = NULL;

	AString objName;
	
	int id = 1;
	while(true)
	{
		objName.Format("Img_Left%d", id);
		PAUIOBJECT pLeft = pShow->GetDlgItem(objName);
		if(!pLeft) break;
		objName.Format("Img_Right%d", id);
		PAUIOBJECT pRight = pShow->GetDlgItem(objName);
		if(!pRight) break;
		
		bool bShow = (nShowLevel > 0 && id == nShowLevel);
		pLeft->Show(bShow);
		pRight->Show(bShow);

		pLastLeft = pLeft;
		pLastRight = pRight;
		
		id++;
	}

	// show the highest bar if no match
	if(nShowLevel >= id)
	{
		if(pLastLeft) pLastLeft->Show(true);
		if(pLastRight) pLastRight->Show(true);
	}
}

void CDlgTarget::RefreshForceStatus(CDlgTargetShowPlayerPolicy *pShowPolicy)
{
	if (!pShowPolicy->Exist()){
		return;
	}
	PAUIIMAGEPICTURE pImgForce = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Force"));
	if(!pImgForce)
		return;

	if (!pShowPolicy->GetShowName())
	{
		pImgForce->SetCover(NULL, 0);
		pImgForce->SetData(0);
		pImgForce->SetHint(_AL(""));
		return;
	}

	// check whether we need update
	int forceID = pShowPolicy->GetForce();
	int lastForceID = pImgForce->GetData();
	if(forceID != lastForceID)
	{
		const FORCE_CONFIG* pConfig = NULL;
		if(forceID)
		{
			pConfig = CECForceMgr::GetForceData(forceID);
		}

		ACString strHint;
		A2DSprite* pSprite = NULL;
		// try to load the flag icon
		if(pConfig)
		{
			CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();
			if(pMgr) pSprite = pMgr->GetForceIcon(m_pA3DDevice, pConfig, CECForceMgr::FORCE_ICON_NORMAL);
			strHint = pConfig->name;
		}

		pImgForce->SetCover(pSprite, 0);
		pImgForce->SetData(forceID);
		pImgForce->SetHint(strHint);
	}
}

void CDlgTarget::SetAggressive(bool bAggressive) {
	if (bAggressive == m_bAggressive) {
		return;
	}
	m_bAggressive = bAggressive;
	if (m_bAggressive) {
		GetDlgItem("Back")->Show(false);
		GetDlgItem("Img_PA")->Show(false);
		GetDlgItem("Img_BackAA")->Show(true);
		GetDlgItem("Img_AA")->Show(true);
	} else {
		GetDlgItem("Back")->Show(true);
		GetDlgItem("Img_PA")->Show(true);
		GetDlgItem("Img_BackAA")->Show(false);
		GetDlgItem("Img_AA")->Show(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgTargetOfTarget, CDlgBase)
AUI_ON_COMMAND("Btn_Player", OnCommand_SelectMaster)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTargetOfTarget, CDlgBase)
AUI_ON_EVENT("*",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("*",	WM_MOUSEMOVE,	OnEventMouseMove)
AUI_ON_EVENT("*",	WM_LBUTTONUP,	OnEventLButtonUp)
AUI_END_EVENT_MAP()

CDlgTargetOfTarget::CDlgTargetOfTarget() {
	m_iTargetID = 0;
	m_iMasterOfTargetID = 0;
	m_pPrgHP = NULL;
	m_pLblName = NULL;
	m_bClick = false;
}

bool CDlgTargetOfTarget::OnInitDialog() {
	DDX_Control("Prgs_HP", m_pPrgHP);
	DDX_Control("Txt_Name", m_pLblName);
	return CDlgBase::OnInitDialog();
}

void CDlgTargetOfTarget::ShowTargetOfTarget(int iTargetID) {

	CDlgTargetOfTarget* pDlgPlayer = (CDlgTargetOfTarget*)CECUIHelper::GetGameUIMan()->GetDialog("Win_HpOtherPlayer");
	CDlgTargetOfTarget* pDlgNPC = (CDlgTargetOfTarget*)CECUIHelper::GetGameUIMan()->GetDialog("Win_HpOtherMonster");

	if (iTargetID == 0) {
		if (pDlgPlayer->IsShow()) {
			pDlgPlayer->Show(false);
			// 对话框关闭时，两个对话框的位置保持一致
			pDlgNPC->SetPosEx(pDlgPlayer->GetPos().x, pDlgPlayer->GetPos().y);
		}
		if (pDlgNPC->IsShow()) {
			pDlgNPC->Show(false);
			// 对话框关闭时，两个对话框的位置保持一致
			pDlgPlayer->SetPosEx(pDlgNPC->GetPos().x, pDlgNPC->GetPos().y);
		}
		return;
	}

	POINT lastPos;
	if (pDlgPlayer->IsShow()) {
		lastPos = pDlgPlayer->GetPos();
	} else {
		lastPos = pDlgNPC->GetPos();
	}

	CDlgTargetOfTarget* pDlgToShow = NULL;
	if (ISNPCID(iTargetID)) {
		CECNPC* pNPC = CECUIHelper::GetWorld()->GetNPCMan()->GetNPC(iTargetID);
		if (!pNPC) {
			return;
		}
		if (pNPC->GetClonedMaster()) {
			pDlgPlayer->ShowPlayer(iTargetID);
			pDlgToShow = pDlgPlayer;
		} else if (pNPC->IsPetNPC()) {
			pDlgNPC->ShowPet(pNPC);
			pDlgToShow = pDlgNPC;
		} else {
			pDlgNPC->ShowNPC(pNPC);
			pDlgToShow = pDlgNPC;
		}
	} else if (ISPLAYERID(iTargetID)) {
		CECPlayer* pPlayer = CECUIHelper::GetWorld()->GetPlayerMan()->GetPlayer(iTargetID);
		if (!pPlayer) {
			return;
		}
		pDlgPlayer->ShowPlayer(iTargetID);
		pDlgToShow = pDlgPlayer;
	}

	if (pDlgToShow) {
		pDlgToShow->SetTargetID(iTargetID);
		if (!pDlgToShow->IsShow()) {
			pDlgToShow->Show(true);
			pDlgToShow->SetPosEx(lastPos.x, lastPos.y);
		}
	}
	if (pDlgToShow != pDlgPlayer && pDlgPlayer->IsShow()) {
		pDlgPlayer->Show(false);
	}
	if (pDlgToShow != pDlgNPC && pDlgNPC->IsShow()) {
		pDlgNPC->Show(false);
	}
}

void CDlgTargetOfTarget::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	m_bClick = true;
	m_iLButtonDownX = GET_X_LPARAM(lParam);
	m_iLButtonDownY = GET_Y_LPARAM(lParam);
}

void CDlgTargetOfTarget::OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	if (!m_bClick) {
		return;
	}
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	if (abs(x - m_iLButtonDownX) > 5 || abs(y - m_iLButtonDownY) > 5) {
		m_bClick = false;
	}
}

void CDlgTargetOfTarget::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	if (m_bClick && AString(pObj->GetName()) != AString("Btn_Player")) {
		GetHostPlayer()->SelectTarget(m_iTargetID);
		m_bClick = false;
	}
}

void CDlgTargetOfTarget::OnCommand_SelectMaster(const char* szCommand) {
	GetHostPlayer()->SelectTarget(m_iMasterOfTargetID);
}

void CDlgTargetOfTarget::ShowPlayer(int id) {

	CDlgTargetShowPlayerPolicy* pPlayerPolicy = CDlgTargetShowPlayerPolicy::Create(id);
	auto_delete<CDlgTargetShowPlayerPolicy> _dummy(pPlayerPolicy);


	if (!pPlayerPolicy || !pPlayerPolicy->Exist()) {
		ASSERT(false && "cloned player not found");
	}

	// 设置血条
	int iCurHP = pPlayerPolicy->GetCurHP();
	int iMaxHP = pPlayerPolicy->GetMaxHP();
	m_pPrgHP->SetProgress(m_pPrgHP->GetRangeMax() * (double)iCurHP / iMaxHP);

	// 设置名字
	ACString strName = pPlayerPolicy->GetNameToShow();
	m_pLblName->SetText(strName);
	m_pLblName->SetColor(pPlayerPolicy->GetNameColor());

	// 设置等级
	int iLevel = pPlayerPolicy->GetLevel();
	AUILabel* pLblLevel = (AUILabel*)GetDlgItem("Txt_LV");
	if (pLblLevel) {
		pLblLevel->SetText(ACString().Format(_AL("%d"), iLevel));
	}

	// 设置职业文字
	int iProf = pPlayerPolicy->GetProfession();
	AUILabel* pLblProf = (AUILabel*)GetDlgItem("Txt_Job");
	if (pLblProf) {
		pLblProf->SetText(GetGameRun()->GetProfName(iProf));
	}

	// Faction
	AUIObject* pLblFaction = GetDlgItem("Txt_Faction");
	if (pLblFaction) {
		Faction_Info *pInfo = NULL;
		int idFaction = pPlayerPolicy->GetFactionID();
		CECFactionMan *pFMan = GetGame()->GetFactionMan();
		if( idFaction ) {
			pInfo = pFMan->GetFaction(idFaction);
		}
		if( idFaction <= 0 ) {
			pLblFaction->SetText(GetStringFromTable(251));
		} else if( pInfo ) {
			pLblFaction->SetText(pInfo->GetName());
		} else {
			pLblFaction->SetText(_AL(""));
		}
	}	
}

void CDlgTargetOfTarget::ShowNPC(CECNPC* pNPC) {

	// 设置血条
	int iCurHP = pNPC->GetBasicProps().iCurHP;
	int iMaxHP = pNPC->GetExtendProps().bs.max_hp;
	m_pPrgHP->SetProgress(m_pPrgHP->GetRangeMax() * (double)iCurHP / iMaxHP);

	// 设置名字
	ACString strName = pNPC->GetNameToShow();
	int iLevel = pNPC->GetBasicProps().iLevel;
	if (pNPC->IsServerNPC()) {
		m_pLblName->SetText(strName);
	} else if (iLevel == 150) {
		m_pLblName->SetText(ACString().Format(_AL("[?] %s"), strName));
	} else {
		m_pLblName->SetText(ACString().Format(_AL("[%d] %s"), iLevel, strName));
	}

	// 隐藏选择主人按钮
	AUIObject* pObj = GetDlgItem("Btn_Player");
	if (pObj) {
		GetDlgItem("Btn_Player")->Show(false);
	}
}

void CDlgTargetOfTarget::ShowPet(CECNPC* pNPC) {

	// 设置血条
	int iCurHP = pNPC->GetBasicProps().iCurHP;
	int iMaxHP = pNPC->GetExtendProps().bs.max_hp;
	m_pPrgHP->SetProgress(m_pPrgHP->GetRangeMax() * (double)iCurHP / iMaxHP);

	// 设置名字
	ACString strName = pNPC->GetNameToShow();
	m_pLblName->SetText(ACString().Format(GetStringFromTable(11410), strName));

	// 显示选择主人按钮
	AUIObject* pObj = GetDlgItem("Btn_Player");
	if (pObj) {
		GetDlgItem("Btn_Player")->Show(true);
	}
	m_iMasterOfTargetID = pNPC->GetMasterID();
}

void CDlgTargetOfTarget::SetTargetID(int iTargetID) {
	m_iTargetID = iTargetID;
}