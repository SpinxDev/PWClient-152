// File		: DlgPetList.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/26

#include "AFI.h"
#include "DlgPetList.h"
#include "DlgPetDetail.h"
#include "DlgPetRec.h"
#include "DlgPetRename.h"
#include "DlgPetDye.h"
#include "EC_GameUIMan.h"
#include "EC_PetCorral.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "elementdataman.h"
#include "DlgPetEvolution.h"
#include "DlgPetEvolutionAnim.h"
#include "DlgPreviewPetRebuild.h"
#include "EC_ActionSwitcher.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPetList, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("detail*",	OnCommandDetail)
AUI_ON_COMMAND("summon*",	OnCommandSummon)
AUI_ON_COMMAND("recall*",	OnCommandRecall)
AUI_ON_COMMAND("banish*",	OnCommandBanish)
AUI_ON_COMMAND("Btn_Page*",	OnCommandPage)
AUI_ON_COMMAND("Btn_P*",	OnCommandEvolution)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetList, CDlgBase)

AUI_ON_EVENT("Img_Icon*",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Img_Icon*",	WM_RBUTTONUP,	OnEventRButtonUp)

AUI_END_EVENT_MAP()


CDlgPetList::CDlgPetList()
{
}

CDlgPetList::~CDlgPetList()
{
}

bool CDlgPetList::OnInitDialog()
{
	char szText[20];
	int i;
	for(i = 0; i < CDLGPETLIST_SLOT_MAX; i++ )
	{
		sprintf(szText, "Img_Icon%d", i + 1);
		m_pImg_Icon[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
		sprintf(szText, "Lab_Name%d", i + 1);
		m_pLab_Name[i] = (PAUILABEL)GetDlgItem(szText);
		sprintf(szText, "Lab_Level%d", i + 1);
		m_pLab_Level[i] = (PAUILABEL)GetDlgItem(szText);
		sprintf(szText, "Txt_Name%d", i + 1);
		m_pTxt_Name[i] = (PAUILABEL)GetDlgItem(szText);
		sprintf(szText, "Txt_Level%d", i + 1);
		m_pTxt_Level[i] = (PAUILABEL)GetDlgItem(szText);
		sprintf(szText, "Btn_Recall%d", i + 1);
		m_pBtn_Recall[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
		sprintf(szText, "Btn_Summon%d", i + 1);
		m_pBtn_Summon[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
		sprintf(szText, "Btn_Detail%d", i + 1);
		m_pBtn_Detail[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
		sprintf(szText, "Btn_Banish%d", i + 1);
		m_pBtn_Banish[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
		sprintf(szText,"Btn_P%d", i+1);
		m_pBtn_Evolution[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
	}
	for (i = 0; i < CDLGPETLIST_PAGE_MAX; i++) {
		m_pBtn_Page[i] = NULL;
		DDX_Control(AString().Format("Btn_Page%d", i + 1), m_pBtn_Page[i]);
		m_pBtn_Page[i]->SetPushed(false);
	}
	m_nPageIndex = 0;
	m_pBtn_Page[0]->SetPushed(true);

	return true;
}

void CDlgPetList::OnTick()
{
	UpdateList();
}

void CDlgPetList::OnCommand_CANCEL(const char * szCommand)
{
	if( GetGameUIMan()->m_pDlgPetRec->IsShow())
		GetGameUIMan()->m_pDlgPetRec->OnCommand("IDCANCEL");
	else if( GetGameUIMan()->m_pDlgPetRename->IsShow())
		GetGameUIMan()->m_pDlgPetRename->OnCommand("IDCANCEL");
	else if (GetGameUIMan()->m_pDlgPetDye->IsShow())
		GetGameUIMan()->m_pDlgPetDye->OnCommand("IDCANCEL");
	else
		Show(false);
}

void CDlgPetList::OnCommandDetail(const char * szCommand)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	int nSlot = atoi(szCommand + strlen("detail")) - 1;
	nSlot += m_nPageIndex * CDLGPETLIST_SLOT_MAX;
	CECPetData * pPet = pPetCorral->GetPetData(nSlot);
	if( pPet && pPet->GetClass() >= 0)
	{
		char szText[20];
		sprintf(szText, "Win_PetDetail%d", pPet->GetClass());
		CDlgPetDetail *pDlgDetail = (CDlgPetDetail *)GetGameUIMan()->GetDialog(szText);
		if(pDlgDetail)
			pDlgDetail->UpdatePet(nSlot);
	}
}

void CDlgPetList::OnCommandSummon(const char * szCommand)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	int nSlot = atoi(szCommand + strlen("summon")) - 1;
	nSlot += m_nPageIndex * CDLGPETLIST_SLOT_MAX;

	// 飞行-》骑乘
	if (GetHostPlayer()->GetActionSwitcher() && GetHostPlayer()->GetActionSwitcher()->OnFlyToRideAction(nSlot))			
		return;

	GetHostPlayer()->SummonPet(nSlot);
}

void CDlgPetList::OnCommandRecall(const char * szCommand)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	int nSlot = atoi(szCommand + strlen("recall")) - 1;
	nSlot += m_nPageIndex * CDLGPETLIST_SLOT_MAX;
	GetHostPlayer()->RecallPet();
}

void CDlgPetList::OnCommandBanish(const char * szCommand)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	int nSlot = atoi(szCommand + strlen("summon")) - 1;
	nSlot += m_nPageIndex * CDLGPETLIST_SLOT_MAX;
	CECPetData * pPet = pPetCorral->GetPetData(nSlot);
	if( pPet )
	{
		ACString strText;
		strText.Format(GetStringFromTable(693), pPet->GetName());
		PAUIDIALOG pMsg;
		GetGameUIMan()->MessageBox("Game_PetBanish", strText, MB_OKCANCEL, 
			A3DCOLORRGBA(255, 255, 255, 160), &pMsg);
		pMsg->SetData(nSlot);
	}
}
void CDlgPetList::OnCommandEvolution(const char * szCommand)
{
	if (GetHostPlayer()->IsRebuildingPet()) // 正在洗髓或者培训
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10119), MB_OK, 
			A3DCOLORRGBA(255, 255, 255, 160), NULL);
		return;
	}

	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	int nSlot = atoi(szCommand + strlen("Btn_P")) - 1;
	nSlot += m_nPageIndex * CDLGPETLIST_SLOT_MAX;
	CECPetData * pPet = pPetCorral->GetPetData(nSlot);
	if( pPet && pPet->GetClass() >= 0)
	{
		CDlgPetEvolution *pDlgEvolution = (CDlgPetEvolution *)GetGameUIMan()->GetDialog("Win_PetTrain");
		if (pDlgEvolution)
		{
			pDlgEvolution->SetPet(pPet->GetTemplateID(),nSlot);
			pDlgEvolution->Show(true);
		}
	}
}

void CDlgPetList::OnCommandPage(const char* szCommand) {
	int nPageIndex = AString(szCommand + strlen("Btn_Page")).ToInt() - 1;
	ASSERT(0 <= nPageIndex && nPageIndex < CDLGPETLIST_PAGE_MAX && "wrong pet page index");
	int i;
	for (i = 0; i < CDLGPETLIST_PAGE_MAX; i++) {
		m_pBtn_Page[i]->SetPushed(false);
	}
	m_pBtn_Page[nPageIndex]->SetPushed(true);
	m_nPageIndex = nPageIndex;

	UpdateList();
}

void CDlgPetList::UpdateList()
{
	ACString strText;
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	elementdataman* pDB = GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	int i;
	for(i = 0; i < CDLGPETLIST_SLOT_MAX; i++ )
	{
		int nPetSlot = i + m_nPageIndex * CDLGPETLIST_SLOT_MAX;
		if( nPetSlot < pPetCorral->GetPetSlotNum() )
		{
			m_pLab_Level[i]->SetColor(A3DCOLORRGB(255, 203, 74));
			m_pLab_Name[i]->SetColor(A3DCOLORRGB(255, 203, 74));
		}
		else
		{
			m_pLab_Level[i]->SetColor(A3DCOLORRGB(128, 128, 128));
			m_pLab_Name[i]->SetColor(A3DCOLORRGB(128, 128, 128));
		}
		CECPetData * pPet = pPetCorral->GetPetData(nPetSlot);
		if( pPet )
		{
			bool bEnable = ( pPetCorral->GetActivePetIndex() == nPetSlot && GetHostPlayer()->IsOperatingPet() == 0 );
			m_pBtn_Recall[i]->Enable(bEnable);
			
			bEnable = ( pPetCorral->GetActivePetIndex() != nPetSlot && GetHostPlayer()->IsOperatingPet() == 0 );
			m_pBtn_Summon[i]->Enable(bEnable);

			m_pBtn_Detail[i]->Enable(true);

			strText.Format(GetStringFromTable(801), pPet->GetLevel());
			m_pTxt_Level[i]->SetText(strText);
			m_pTxt_Name[i]->SetText(pPet->GetName());
			if( (pPet->GetClass() == GP_PET_CLASS_COMBAT || pPet->GetClass() == GP_PET_CLASS_EVOLUTION) && pPet->GetHPFactor() == 0.0f || IsPetDye(nPetSlot))
				m_pImg_Icon[i]->SetColor(A3DCOLORRGB(128, 128, 128));
			else
				m_pImg_Icon[i]->SetColor(A3DCOLORRGB(255, 255, 255));
			m_pImg_Icon[i]->SetData(1);
			m_pImg_Icon[i]->SetDataPtr((void*)1);
			PET_ESSENCE *pDBEssence = (PET_ESSENCE*)pDB->get_data_ptr(pPet->GetTemplateID(), 
				ID_SPACE_ESSENCE, DataType);

			if( pDBEssence )
			{
				AString strFile;
				af_GetFileTitle(pDBEssence->file_icon, strFile);
				strFile.MakeLower();
				m_pImg_Icon[i]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
			}
			else
			{
				m_pImg_Icon[i]->ClearCover();
				m_pImg_Icon[i]->SetHint(_AL(""));
			}

			m_pBtn_Evolution[i]->Show(pPet->GetClass()==GP_PET_CLASS_COMBAT || pPet->GetClass()==GP_PET_CLASS_EVOLUTION);
		}
		else
		{
			m_pBtn_Summon[i]->Enable(false);
			m_pBtn_Recall[i]->Enable(false);
			m_pBtn_Detail[i]->Enable(false);
			m_pBtn_Evolution[i]->Show(false);
//			m_pBtn_Banish[i]->Enable(false);
			m_pImg_Icon[i]->SetDataPtr(NULL);
			m_pTxt_Level[i]->SetText(_AL(""));
			m_pTxt_Name[i]->SetText(_AL(""));
			m_pImg_Icon[i]->ClearCover();
			m_pImg_Icon[i]->SetHint(_AL(""));
			m_pImg_Icon[i]->SetData(0);
		}
	}
}

int CDlgPetList::GetPetIndex(int nSlot) {
	return nSlot + m_nPageIndex * CDLGPETLIST_SLOT_MAX;
}

void CDlgPetList::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj->GetData() )
		return;

	int nSlot = atoi(pObj->GetName() + strlen("Img_Item")) - 1;
	nSlot += m_nPageIndex * CDLGPETLIST_SLOT_MAX;
	if (IsPetDye(nSlot))	return;
	
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	POINT pt = 
	{
		GET_X_LPARAM(lParam) - p->X,
		GET_Y_LPARAM(lParam) - p->Y,
	};
	
	GetGameUIMan()->m_ptLButtonDown = pt;
	GetGameUIMan()->InvokeDragDrop(this, pObj, pt);
}

bool CDlgPetList::IsPetDye(int nSlot)
{
	return nSlot >= 0
		&& GetGameUIMan()->m_pDlgPetDye->IsShow()
		&& GetGameUIMan()->m_pDlgPetDye->GetPetSlot() == nSlot;
}

void CDlgPetList::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj->GetData() )
		return;
	
	int nSlot = atoi(pObj->GetName() + strlen("Img_Item")) - 1;
	nSlot += m_nPageIndex * CDLGPETLIST_SLOT_MAX;

	// handle right-click
	OnPetDragDrop(nSlot, NULL);
}

void CDlgPetList::OnPetDragDrop(int petIndex, PAUIOBJECT pObjOver)
{
	PAUIDIALOG pDlgOver = NULL;

	// for drag-drop scene
	if( pObjOver )
	{
		pDlgOver = pObjOver->GetParent();
	}
	else
	{
		// for right-click scene
		PAUIDIALOG aPetDlg[] = {
			GetGameUIMan()->m_pDlgPetRec,
			GetGameUIMan()->m_pDlgPetRename,
			GetGameUIMan()->m_pDlgPetDye,
		};

		// auto select a shown one
		for(int i=0;i<sizeof(aPetDlg) / sizeof(aPetDlg[0]);i++)
		{
			if(aPetDlg[i] && aPetDlg[i]->IsShow()) { pDlgOver = aPetDlg[i]; break; }
		}
	}

	// dispatch the operations
	if(pDlgOver == GetGameUIMan()->m_pDlgPetRec)			GetGameUIMan()->m_pDlgPetRec->SetPet(petIndex);
	else if(pDlgOver == GetGameUIMan()->m_pDlgPetRename)	GetGameUIMan()->m_pDlgPetRename->SetPet(petIndex);
	else if(pDlgOver == GetGameUIMan()->m_pDlgPetDye)		GetGameUIMan()->m_pDlgPetDye->SetPet(petIndex);
}