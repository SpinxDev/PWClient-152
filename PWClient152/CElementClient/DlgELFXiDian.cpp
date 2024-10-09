// Filename	: DlgELFXiDian.cpp
// Creator	: Chen Zhixin
// Date		: October 07, 2008

#include "WindowsX.h"
#include "AF.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "AUICTranslate.h"
#include "DlgELFXiDian.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "elementdataman.h"

#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_HostGoblin.h"
#include "EC_IvtrGoblin.h"
#include "EC_ShortcutMgr.h"

#define new A_DEBUG_NEW
#define INTERAL_LONG	500
#define INTERAL_SHORT	200
#define TEMPERED_TIME	3000

AUI_BEGIN_COMMAND_MAP(CDlgELFXiDian, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("confirm",		OnCommand_Confirm)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFXiDian, CDlgBase)

AUI_ON_EVENT("Btn_Add*",	WM_LBUTTONDOWN,		OnEventLButtonDownAdd)
AUI_ON_EVENT("Btn_Minus*",	WM_LBUTTONDOWN,		OnEventLButtonDownMinus)
AUI_ON_EVENT("Img_ELF",		WM_LBUTTONDOWN,		OnEventLButtonDown_ELF)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFXiDian
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFXiDian> ELFXiDianClickShortcut;
//------------------------------------------------------------------------

CDlgELFXiDian::CDlgELFXiDian()
{

}

CDlgELFXiDian::~CDlgELFXiDian()
{
}

bool CDlgELFXiDian::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFXiDianClickShortcut(this));

	m_pTxt_TotleCount = (PAUILABEL)GetDlgItem("Txt_TotalCount");
	m_pTxt_CostCount = (PAUILABEL)GetDlgItem("Txt_CostCount");
	m_pImg_ELF = (PAUIIMAGEPICTURE)GetDlgItem("Img_ELF");
	m_pTxt_ELFName = (PAUIOBJECT)GetDlgItem("Txt_ELFname");
	m_pBtn_Confirm = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_AG");
	return true;
}

void CDlgELFXiDian::OnShowDialog()
{

	for (int i=0; i<4; i++)
	{
		m_iSetting[i] = 0;
	}
	m_iCost = 0;
	m_iMaxPro[0] = 0;
	m_iMaxPro[1] = 0;
	m_iMaxPro[2] = 0;
	m_iMaxPro[3] = 0;
	UpdateData(false);
}

void CDlgELFXiDian::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
	DDX_EditBox(bSave, "Num_1", m_iSetting[0]);
	DDX_EditBox(bSave, "Num_2", m_iSetting[1]);
	DDX_EditBox(bSave, "Num_3", m_iSetting[2]);
	DDX_EditBox(bSave, "Num_4", m_iSetting[3]);
}

void CDlgELFXiDian::OnTick(void)
{
	UpdateData(true);
	m_iCost = 0;
	CECIvtrGoblin *pIvtrGoblin = (CECIvtrGoblin *)m_pImg_ELF->GetDataPtr();
	if (pIvtrGoblin)
	{
		m_iMaxPro[0] = pIvtrGoblin->GetEssence().data.strength;
		m_iMaxPro[1] = pIvtrGoblin->GetEssence().data.agility;
		m_iMaxPro[2] = pIvtrGoblin->GetEssence().data.vitality;
		m_iMaxPro[3] = pIvtrGoblin->GetEssence().data.energy;
	}
	//CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();

	for (int i = 0; i < 4; i++)
	{
		if (m_iSetting[i] > m_iMaxPro[i])
		{
			m_iSetting[i] = m_iMaxPro[i];
		}
		m_iCost += m_iSetting[i];
	}
	UpdateData(false);
}

bool CDlgELFXiDian::Render(void)
{

	RefreshHostDetails();

	return CDlgBase::Render();
}

void CDlgELFXiDian::RefreshHostDetails()
{
 	if( !IsShow() ) return;

 	ACHAR szText[512];

 	a_sprintf(szText, _AL("%d"), GetHostPlayer()->GetPack()->GetItemTotalNum(23552) + GetHostPlayer()->GetPack()->GetItemTotalNum(24337));
 	m_pTxt_TotleCount->SetText(szText);

	a_sprintf(szText, _AL("%d"), m_iCost);
	m_pTxt_CostCount->SetText(szText);

}

void CDlgELFXiDian::OnEventLButtonDownAdd(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	const char *szCommand = pObj->GetName();
	if( 0 == stricmp(szCommand, "Btn_AddStr") )
	{
		if (m_iSetting[0] < m_iMaxPro[0])
		{
			m_iSetting[0]++;
		}
		else
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(7165), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
	else if( 0 == stricmp(szCommand, "Btn_AddAgi") )
	{
		if (m_iSetting[1] < m_iMaxPro[1])
		{
			m_iSetting[1]++;
		}
		else
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(7165), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
	else if( 0 == stricmp(szCommand, "Btn_AddVit") )
	{
		if (m_iSetting[2] < m_iMaxPro[2])
		{
			m_iSetting[2]++;
		}
		else
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(7165), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
	else
	{
		if (m_iSetting[3] < m_iMaxPro[3])
		{
			m_iSetting[3]++;
		}
		else
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(7165), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
	UpdateData(false);
}

void CDlgELFXiDian::OnEventLButtonDownMinus(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	const char *szCommand = pObj->GetName();
	if( 0 == stricmp(szCommand, "Btn_MinusStr") )
	{
		if (m_iSetting[0] > 0)
		{
			m_iSetting[0]--;
		}
		else
			m_iSetting[0] = m_iMaxPro[0];
	}
	else if( 0 == stricmp(szCommand, "Btn_MinusAgi") )
	{
		if (m_iSetting[1] > 0)
		{
			m_iSetting[1]--;
		}
		else
			m_iSetting[1] = m_iMaxPro[1];
	}
	else if( 0 == stricmp(szCommand, "Btn_MinusVit") )
	{
		if (m_iSetting[2] > 0)
		{
			m_iSetting[2]--;
		}
		else
			m_iSetting[2] = m_iMaxPro[2];
	}
	else
	{
		if (m_iSetting[3] > 0)
		{
			m_iSetting[3]--;
		}
		else
			m_iSetting[3] = m_iMaxPro[3];
	}
	UpdateData(false);
}

void CDlgELFXiDian::SetELF(CECIvtrItem *pItem1, int nSlot)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)pItem1;
	OnEventLButtonDown_ELF(0, 0, NULL);
	m_pImg_ELF->SetDataPtr(pELF);
	m_nSlot = nSlot;
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pELF->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_ELF->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pELF->GetDesc();
	if( szDesc )
		m_pImg_ELF->SetHint(trans.Translate(szDesc));
	else
		m_pImg_ELF->SetHint(_AL(""));
	m_pTxt_ELFName->SetText(pELF->GetName());
}

void CDlgELFXiDian::OnCommand_CANCEL(const char * szCommand)
{
	OnEventLButtonDown_ELF(0, 0, NULL);
	ResetPoint();
	Show(false);
	if (0 != stricmp(szCommand, "NotEndNPCService"))
	{
		GetGameUIMan()->EndNPCService();
		GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	}
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgELFXiDian::OnCommand_Confirm(const char * szCommand)
{
	int iItemMax = GetHostPlayer()->GetPack()->GetItemTotalNum(23552) + GetHostPlayer()->GetPack()->GetItemTotalNum(24337);
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)m_pImg_ELF->GetDataPtr();
	if (!pELF)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7160), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (m_iCost == 0)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7161), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (m_iCost > iItemMax)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7162), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	GetGameSession()->c2s_CmdNPCSevGoblinReturnStatusPt(m_nSlot, GetHostPlayer()->GetPack()->FindItem(23552), m_iSetting[0], m_iSetting[1], m_iSetting[2], m_iSetting[3]);

	ResetPoint();
	OnEventLButtonDown_ELF(0, 0, NULL);
}

void CDlgELFXiDian::OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pELF = (CECIvtrItem *)m_pImg_ELF->GetDataPtr();
	if( pELF )
		pELF->Freeze(false);
	m_pImg_ELF->ClearCover();
	m_pImg_ELF->SetHint(GetStringFromTable(7302));
	m_pImg_ELF->SetDataPtr(NULL);
	m_pTxt_ELFName->SetText(_AL(""));
	m_nSlot = -1;
	ResetPoint();
}

void CDlgELFXiDian::ResetPoint()
{
	m_iSetting[0] = 0;
	m_iSetting[1] = 0;
	m_iSetting[2] = 0;
	m_iSetting[3] = 0;
	UpdateData(false);
}

void CDlgELFXiDian::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_ELF", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN )
		this->SetELF(pIvtrSrc, iSrc);
}