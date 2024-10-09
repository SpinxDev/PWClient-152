// Filename	: DlgPrewviewProduce.h
// Creator	: Han Guanghui
// Date		: 2012/05/02

#include "DlgPreviewProduce.h"
#include "AFI.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GPDataType.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "AUICTranslate.h"
#include "DlgProduce.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "EC_HostPlayer.h"
#include "elementdataman.h"
#include "EC_TimeSafeChecker.h"

AUI_BEGIN_COMMAND_MAP(CDlgPreviewProduce, CDlgBase)
AUI_ON_COMMAND("SelectOld",OnCommandSelectOldOne)
AUI_ON_COMMAND("SelectNew",OnCommandSelectNewOne)
AUI_ON_COMMAND("Reproduce",OnCommandReproduce)
AUI_ON_COMMAND("Close", OnCommandCANCEL)
AUI_END_COMMAND_MAP()

static const char* ENERGY_BALL_SMALL = "界面\\生产能量球小.gfx";
static const char* ENERGY_BALL_MIDDLE = "界面\\生产能量球中.gfx";
static const int ANIMATION_LAST_COUNT_MINISECONDS = 1000;

CDlgPreviewProduce::CDlgPreviewProduce():
m_iStopThinkingTime(0),
m_pObjClose(NULL),
m_pButtonSelectOld(NULL),
m_pButtonSelectNew(NULL),
m_pButtonReproduce(NULL),
m_pTextAttributeAfter(NULL),
m_pTextAttributeBefore(NULL),
m_pLabelTime(NULL),
m_bThinking(false),
m_pImgOld(NULL),
m_pImgNew(NULL),
m_pGfxLightLeft(NULL),
m_pGfxLightRight(NULL),
m_pGfxCore(NULL),
m_iAnimationTickStart(0),
m_bAnimationPlaying(false),
m_bCannotProduce(false)
{
}

bool CDlgPreviewProduce::OnInitDialog()
{
	DDX_Control("Close",m_pObjClose);
	DDX_Control("SelectOld",m_pButtonSelectOld);
	DDX_Control("SelectNew",m_pButtonSelectNew);
	DDX_Control("Reproduce",m_pButtonReproduce);
	DDX_Control("AttributeBefore",m_pTextAttributeBefore);
	DDX_Control("AttributeAfter",m_pTextAttributeAfter);
	DDX_Control("ThinkingTime",m_pLabelTime);
	m_pImgOld = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Old"));
	m_pImgNew = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_New"));

	m_pLabelTime->SetText(_AL(""));

	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_OldLight"));
	if (pImg)
		m_pGfxLightLeft = pImg->GetGfx();
	pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_NewLight"));
	if (pImg)
		m_pGfxLightRight = pImg->GetGfx();
	pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Light"));
	if (pImg)
		m_pGfxCore = pImg->GetGfx();

	ShowGfxWhenProducing();

	return true;
}

void CDlgPreviewProduce::ShowGfxBeforeProduce()
{
	A3DGFXEx* pGfx;
	if (m_pImgOld)
	{
		pGfx = m_pImgOld->GetGfx();
		if (pGfx)
		{
			pGfx->SetVisible(false);
		}
	}
	if (m_pImgNew)
	{
		pGfx = m_pImgNew->GetGfx();
		if (pGfx)
		{
			pGfx->SetVisible(false);
		}
	}
	if (m_pGfxLightLeft)
	{
		m_pGfxLightLeft->SetVisible(false);
	}
	if (m_pGfxLightRight)
	{
		m_pGfxLightRight->SetVisible(false);
	}
	if (m_pGfxCore)
	{
		m_pGfxCore->SetVisible(false);
	}
}

void CDlgPreviewProduce::ShowGfxWhenProducing()
{
	A3DGFXEx* pGfx;
	if (m_pImgOld)
	{
		pGfx = m_pImgOld->GetGfx();
		if (pGfx)
		{
			pGfx->SetVisible(false);
		}
	}
	if (m_pImgNew)
	{
		pGfx = m_pImgNew->GetGfx();
		if (pGfx)
		{
			pGfx->SetVisible(false);
		}
	}
	if (m_pGfxLightLeft)
	{
		m_pGfxLightLeft->SetVisible(false);
	}
	if (m_pGfxLightRight)
	{
		m_pGfxLightRight->SetVisible(false);
	}
	if (m_pGfxCore)
	{
		m_pGfxCore->SetVisible(true);
	}
}
void CDlgPreviewProduce::ShowGfxWhenProduced()
{
	if (m_pImgOld)
	{
		m_pImgOld->SetGfx(ENERGY_BALL_SMALL);
		m_pImgOld->Resize();
	}
	if (m_pImgNew)
	{
		m_pImgNew->SetGfx(ENERGY_BALL_SMALL);
		m_pImgNew->Resize();
	}
	if (m_pGfxLightLeft)
	{
		m_pGfxLightLeft->SetVisible(false);
	}
	if (m_pGfxLightRight)
	{
		m_pGfxLightRight->SetVisible(false);
	}
	if (m_pGfxCore)
	{
		m_pGfxCore->SetVisible(true);
	}
}
void CDlgPreviewProduce::ShowGfxWhenSelected(bool bNew)
{
	if (m_pImgOld)
	{
		m_pImgOld->SetGfx(bNew ? "" : ENERGY_BALL_MIDDLE);
		m_pImgOld->Resize();
	}
	if (m_pImgNew)
	{
		m_pImgNew->SetGfx(bNew ? ENERGY_BALL_MIDDLE : "");
		m_pImgNew->Resize();
	}
	if (m_pGfxLightLeft)
	{
		m_pGfxLightLeft->SetVisible(bNew ? false : true);
	}
	if (m_pGfxLightRight)
	{
		m_pGfxLightRight->SetVisible(bNew ? true : false);
	}
	if (m_pGfxCore)
	{
		m_pGfxCore->SetVisible(true);
	}
	AnimationStart();
}
void CDlgPreviewProduce::AnimationStart()
{
	m_iAnimationTickStart = GetTickCount();
	m_bAnimationPlaying = true;
}
void CDlgPreviewProduce::AnimationTick()
{
	if (m_bAnimationPlaying)
	{
		if (CECTimeSafeChecker::ElapsedTimeFor(m_iAnimationTickStart) > ANIMATION_LAST_COUNT_MINISECONDS)
		{
			m_bAnimationPlaying = false;
			if (m_pGfxLightLeft)
			{
				m_pGfxLightLeft->SetVisible(false);
			}
			if (m_pGfxLightRight)
			{
				m_pGfxLightRight->SetVisible(false);
			}
			if (m_pGfxCore)
			{
				m_pGfxCore->SetVisible(false);
			}
			EnableReproduce(true);
		}
	}
}
void CDlgPreviewProduce::AnimationStop()
{
	if (m_bAnimationPlaying){
		m_bAnimationPlaying = false;
	}
}

void CDlgPreviewProduce::OnTick()
{
	if (m_bThinking)
	{
		int leftTime = m_iStopThinkingTime - GetGame()->GetServerAbsTime();
		int leftTimeBackup = leftTime;
		a_ClampFloor(leftTime, 0);
		
		ACString strText;
		strText.Format(GetStringFromTable(9721), GetGameUIMan()->GetFormatTime(leftTime));
		m_pLabelTime->SetText(strText);
	}
	AnimationTick();
}

void CDlgPreviewProduce::OnCommandReproduce(const char *szCommand)
{
	char inherit_type(0);
	if (m_bInheritRefine) inherit_type += INHERIT_REFINE;
	if (m_bInheritHole) inherit_type += INHERIT_HOLE;
	if (m_bInheritStone) inherit_type += INHERIT_STONE;
	if (m_bInheritEngrave) inherit_type += INHERIT_ENGRAVE;

	GetGameUIMan()->m_pDlgProduce->UpgradeEquipment(inherit_type);
	m_bThinking = false;
	m_pLabelTime->SetText(_AL(""));
	m_pButtonSelectOld->Enable(false);
	m_pButtonSelectNew->Enable(false);
	EnableReproduce(false);
}

void CDlgPreviewProduce::OnCommandSelectOldOne(const char *szCommand)
{
	GetGameSession()->c2s_CmdPreviewProduceReply(true);
	m_bThinking = false;
	m_pLabelTime->SetText(_AL(""));
	m_pButtonSelectOld->Enable(false);
	m_pButtonSelectNew->Enable(false);
	m_pTextAttributeAfter->SetText(_AL(""));
	m_pObjClose->Enable(true);
	ShowGfxWhenSelected(false);
}

void CDlgPreviewProduce::OnCommandSelectNewOne(const char *szCommand)
{
	GetGameSession()->c2s_CmdPreviewProduceReply(false);
	m_bThinking = false;
	m_pLabelTime->SetText(_AL(""));
	m_pButtonSelectOld->Enable(false);
	m_pButtonSelectNew->Enable(false);
	EnableReproduce(false);
	m_pTextAttributeBefore->SetText(_AL(""));
	m_pObjClose->Enable(true);
	ShowGfxWhenSelected(true);
}
void CDlgPreviewProduce::OnCannotProduce()
{
	m_bCannotProduce = true;
	EnableReproduce(false);
}
void CDlgPreviewProduce::OnCommandCANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgPreviewProduce::SetOldInfo(CECIvtrItem *pItem, unsigned int itemID, bool inheritRefine, bool inheritHole, bool inheritStone,bool inheritEngrave)
{
	SetData(itemID, "Item_ID");
	m_bInheritRefine = inheritRefine;
	m_bInheritHole = inheritHole;
	m_bInheritStone = inheritStone;
	m_bInheritEngrave = inheritEngrave;

	//	设置新的物品
	CECIvtrEquip* pEquipUp = dynamic_cast<CECIvtrEquip*>(pItem);
	if (pEquipUp){
		AUICTranslate trans;
		m_pTextAttributeBefore->SetText(trans.Translate(pEquipUp->GetPreviewInfo()));
	}else{
		assert(false);
		m_pTextAttributeBefore->SetText(_AL(""));
	}
	m_pTextAttributeAfter->SetText(_AL(""));
	m_pLabelTime->SetText(_AL(""));
	ShowGfxBeforeProduce();
}

void CDlgPreviewProduce::SetPreviewInfo(void* pData)
{
	m_pButtonSelectNew->Enable(true);
	m_pButtonSelectOld->Enable(true);
	m_bThinking = true;
	S2C::produce4_item_info* pInfo = (S2C::produce4_item_info*)pData;
	AUICTranslate trans;
	// gfx
	ShowGfxWhenProduced();

	// 截止思考时间
	m_iStopThinkingTime = GetGame()->GetServerAbsTime() + 30;

	int iItemID = GetData("Item_ID");
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(iItemID,0,1);
	if (!pItem)
		return;
	CECIvtrEquip* pEquip = dynamic_cast<CECIvtrEquip*>(pItem);
	if (!pEquip)
		return;
	
	if( pInfo->content && pInfo->content_length)
		pItem->SetItemInfo((BYTE*) &pInfo->content, pInfo->content_length);
	else 
		((CECIvtrItem *)pItem)->SetItemInfo(NULL, 0);
	// 新属性
	m_pTextAttributeAfter->SetText(trans.Translate(pEquip->GetPreviewInfo()));

	if (pItem)
	{
		delete pItem;
	}

}

void CDlgPreviewProduce::Show(bool bShow, bool bModal /* = false */, bool bActive /* = true */)
{
	if (bShow || m_pObjClose->IsEnabled())
	{
		CDlgBase::Show(bShow,bModal,bActive);
	}
}

bool CDlgPreviewProduce::OnProduceStart(int type)
{
	bool bProcessed(false);
	while (true)
	{
		elementdataman *pDataMan = GetGame()->GetElementDataMan();
		DATA_TYPE dt = DT_INVALID;
		const void *p = pDataMan->get_data_ptr(type, ID_SPACE_RECIPE, dt);
		if (!p || dt != DT_RECIPE_ESSENCE){
			break;
		}		
		NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)GetGameUIMan()->m_pDlgProduce->GetDataPtr("ptr_NPC_MAKE_SERVICE");
		if (!pService){
			ASSERT(false);
			break;
		}
		if (pService->produce_type != 4){
			break;
		}

		if (!IsShow()){
			Show(true);
		}
		
		m_bThinking = false;
		m_pLabelTime->SetText(_AL(""));
		m_pButtonSelectOld->Enable(false);
		m_pButtonSelectNew->Enable(false);
		EnableReproduce(false);
		m_pObjClose->Enable(false);
		ShowGfxWhenProducing();
		AnimationStop();

		bProcessed = true;
		break;
	}
	return bProcessed;
}

bool CDlgPreviewProduce::OnProduceEnd()
{
	bool bProcessed(false);
	if (IsShow() && !m_pObjClose->IsEnabled()){
		if (m_bThinking){
			ThinkingTimeEnd();
		}else{
			m_pButtonSelectOld->Enable(false);
			m_pButtonSelectNew->Enable(false);
			EnableReproduce(true);
			m_pObjClose->Enable(true);
			ShowGfxBeforeProduce();
			AnimationStop();
		}
		bProcessed = true;
	}
	return bProcessed;
}

void CDlgPreviewProduce::ThinkingTimeEnd()
{
	m_bThinking = false;
	m_pTextAttributeAfter->SetText(_AL(""));
	m_pLabelTime->SetText(GetStringFromTable(9720));
	EnableReproduce(true);
	m_pButtonSelectNew->Enable(false);
	m_pButtonSelectOld->Enable(false);
	m_pObjClose->Enable(true);
	ShowGfxWhenSelected(false);
}

void CDlgPreviewProduce::EnableReproduce(bool bEnable)
{
	m_pButtonReproduce->Enable(bEnable && !m_bCannotProduce && 0 != GetHostPlayer()->GetCurServiceNPC());
}