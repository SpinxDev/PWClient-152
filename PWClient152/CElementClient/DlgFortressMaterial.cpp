#include "DlgFortressMaterial.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"
#include "EC_Inventory.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrConsume.h"
#include "AFI.h"
#include "DlgInputNO.h"
#include "EC_ShortcutMgr.h"
#include "ExpTypes.h"
#include "AUIDef.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressMaterial, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Material", OnCommand_Material)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressMaterial, CDlgBase)
AUI_ON_EVENT("Img_Ma", WM_LBUTTONDOWN, OnEventLButtonDown_Img_Material)
AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgFortressMaterial
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgFortressMaterial> FortressMaterialClickShortcut;
//------------------------------------------------------------------------

CDlgFortressMaterial::CDlgFortressMaterial()
{
}

bool CDlgFortressMaterial::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new FortressMaterialClickShortcut(this));

	AString strName;
	for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
	{
		strName.Format("Lab_M%02d", i+1);
		m_pLab_M[i] = GetDlgItem(strName);
	}

	m_pImg_Ma = (PAUIIMAGEPICTURE)GetDlgItem("Img_Ma");
	return true;
}

void CDlgFortressMaterial::OnShowDialog()
{
	ResetMaterial();
}

void CDlgFortressMaterial::OnCommand_CANCEL(const char *szCommand)
{
	ResetMaterial();
	Show(false);
	
	//	关闭相关对话框
	PAUIDIALOG pDlgInfo = GetGameUIMan()->GetDialog("Win_FortressInfo");
	if (pDlgInfo && pDlgInfo->IsShow())
		pDlgInfo->OnCommand("IDCANCEL");
	
	//	关闭 NPC 服务
	if (GetGameUIMan()->m_pCurNPCEssence != NULL)
		GetGameUIMan()->EndNPCService();
}

void CDlgFortressMaterial::OnCommand_Material(const char *szCommand)
{
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Ma->GetDataPtr("ptr_CECIvtrItem");

	if (pItem)
	{
		int nIndex = (int)m_pImg_Ma->GetData();
		ACString strNumber = m_pImg_Ma->GetText();
		int count = strNumber.ToInt();
		GetGameSession()->c2s_CmdNPCSevFactionFortressHandInMaterial(nIndex, pItem->GetTemplateID(), count);

		ResetMaterial();
	}
}

void CDlgFortressMaterial::OnEventLButtonDown_Img_Material(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ResetMaterial();
}

void CDlgFortressMaterial::ResetMaterial(int nPackIndex, int count)
{
	//	清除原有内容，设置默认值
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Ma->GetDataPtr("ptr_CECIvtrItem");
	if (pItem)
		pItem->Freeze(false);

	m_pImg_Ma->SetDataPtr(NULL, "ptr_CECIvtrItem");
	m_pImg_Ma->SetData(0);
	m_pImg_Ma->SetText(_AL(""));
	m_pImg_Ma->SetCover(NULL, -1);

	for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		m_pLab_M[i]->SetText(_AL(""));

	if (!GetHostPlayer()->GetFactionFortressInfo())
	{
		//	不允许递交这种材料
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9165));
		return;
	}				

	CECInventory *pPack = GetHostPlayer()->GetPack();
	if (nPackIndex >= 0 && nPackIndex < pPack->GetSize() && count > 0)
	{
		CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(nPackIndex);
		if (pItem
			&& pItem->GetClassID() == CECIvtrItem::ICID_FACTIONMATERIAL
			&& pItem->GetCount() >= count)
		{
			CECIvtrFactionMaterial *pMaterial = (CECIvtrFactionMaterial *)pItem;
			const FACTION_MATERIAL_ESSENCE *pEssence = pMaterial->GetDBEssence();
			while (true)
			{
				if (!pEssence)
					break;

				int i(0);

				//	检查最多允许递交多少个材料
				for (i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
				{
					if (pEssence->material_count[i])
					{
						int temp = GetMaxMaterialCanAdd(i) / pEssence->material_count[i];
						if (count > temp)
							count = temp;
					}
				}
				
				if (count <= 0)
				{
					//	不允许递交这种材料
					GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9164));
					break;
				}				
				
				//	更新界面显示
				pItem->Freeze(true);
				
				m_pImg_Ma->SetData(nPackIndex);
				m_pImg_Ma->SetDataPtr(pItem, "ptr_CECIvtrItem");
				
				ACString strText;
				
				strText.Format(_AL("%d"), count);
				m_pImg_Ma->SetText(strText);
				
				AString strFile;
				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				m_pImg_Ma->SetCover(
					GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
				
				for (i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
				{
					strText.Format(_AL("%d"), pEssence->material_count[i]*count);
					m_pLab_M[i]->SetText(strText);
				}

				break;
			}
		}
	}
}

int CDlgFortressMaterial::GetMaterialLimit(int fortressLevel, int iMaterial)
{
	//	根据基地等级查询当前某种基地材料能够允许的最大值，从而影响材料缴纳
	//
    static const int table[] = {5000,10000,20000,30000,40000,50000,60000};
    int index = fortressLevel/5;
	int nMax = sizeof(table)/sizeof(table[0]);
	a_Clamp(index, 0, nMax);
    return table[index];
}

int CDlgFortressMaterial::GetMaxMaterialCanAdd(int iMaterial)
{
	//	获取每种基地材料能够再添加的最大值
	int nMaxCanAdd(0);
	const CECHostPlayer::FACTION_FORTRESS_INFO *pFInfo = GetHostPlayer()->GetFactionFortressInfo();
	if (pFInfo)
	{
		int nMaterialLimit = GetMaterialLimit(pFInfo->level, iMaterial);
		nMaxCanAdd = nMaterialLimit - pFInfo->material[iMaterial];
		a_ClampFloor(nMaxCanAdd, 0);
	}
	return nMaxCanAdd;
}

void CDlgFortressMaterial::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// default drag-drop to this object
	pObjOver = GetDlgItem("Img_Ma");

	if (pObjOver && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_FACTIONMATERIAL)
	{
		if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 )
		{
			GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
				CDlgInputNO::INPUTNO_FACTION_MATERIAL, pIvtrSrc->GetCount());
		}
		else
		{
			this->ResetMaterial(iSrc, pIvtrSrc->GetCount());
		}
	}
}