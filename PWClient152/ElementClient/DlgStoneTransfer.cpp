// File		: DlgStoneTransfer.cpp 
// Creator	: Wang and Shi
// Date		: 2013/01/14

#include "DlgStoneTransfer.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrStone.h"
#include "ExpTypes.h"
#include "AFI.h"
#include "DlgStoneChange.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include <AUICTranslate.h>
#include "EC_IvtrWeapon.h"
#include "EC_IvtrArmor.h"
#include "EC_IvtrDecoration.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgStoneTransfer, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Transfer",	OnCommandTransfer)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgStoneTransfer, CDlgBase)

AUI_ON_EVENT("Img_Gem*", WM_LBUTTONDOWN,	OnEventLButtonDown_Stone)
AUI_ON_EVENT("Img_Gem*", WM_LBUTTONDBLCLK,  OnEventLButtonDown_Stone)
AUI_ON_EVENT("Img_Item*", WM_LBUTTONDOWN,	OnEventLButtonDown_Material)
AUI_ON_EVENT("Img_Item*", WM_LBUTTONDBLCLK,  OnEventLButtonDown_Material)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgStoneTransfer
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgStoneTransfer> InstallClickShortcut;
//------------------------------------------------------------------------

CDlgStoneTransfer::CDlgStoneTransfer()
{
	memset(m_pStoneImg,0,sizeof(m_pStoneImg));
	memset(m_pMaterialImg,0,sizeof(m_pMaterialImg));
	m_pStoneName = NULL;
	m_pStoneAttribute = NULL;
	m_pMoney = NULL;
	m_iNewStoneIndex = -1;
	m_pTranferBtn = NULL;
	memset(m_pNewStones,0,sizeof(m_pNewStones));
}

CDlgStoneTransfer::~CDlgStoneTransfer()
{
}

bool CDlgStoneTransfer::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallClickShortcut(this));
	return CDlgBase::OnInitDialog();
}
void CDlgStoneTransfer::OnShowDialog()
{
	memset(m_pNewStones,0,sizeof(m_pNewStones));
	m_iNewStoneIndex = -1;

	int i;
	for (i=0;i<RECIPE_NUM;i++)
	{
		m_pStoneImg[i]->ClearCover();
		m_pStoneImg[i]->SetHint(_AL(""));
	}
	for (i=0;i<MATERIAL_NUM;i++)
	{
		m_iMaterailSlot[i] = -1;
	}
	m_pStoneName->SetText(_AL(""));
	m_pStoneAttribute->SetText(_AL(""));

	ClearMaterial();


	EnumRecipe(GetGameUIMan()->m_pDlgStoneChange->GetOldStoneTid());

	bool bHasRecipe = false;

	AUICTranslate trans;
	for (i=0;i<RECIPE_NUM;i++)
	{
		if(!m_recipes[i]) continue;

		CECIvtrItem* pTempItem = CECIvtrItem::CreateItem(m_recipes[i]->id_new_stone,0,1);
		CECIvtrStone* pNewStone = dynamic_cast<CECIvtrStone*>(pTempItem);
		if (pNewStone)
		{
			AString strFile;
			af_GetFileTitle(pNewStone->GetIconFile(), strFile);
			strFile.MakeLower();
			m_pStoneImg[i]->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));
			m_pStoneImg[i]->SetHint(trans.Translate(pNewStone->GetDesc()));

			m_pNewStones[i] = pNewStone;

			bHasRecipe = true;
		}
		else 
			delete pTempItem;
	}	
	
	m_pTranferBtn->Enable(false);

	OnEventLButtonDown_Stone(0, 0, GetDlgItem("Img_Gem1"));

	if (!bHasRecipe)
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10176),MB_OK,A3DCOLORRGB(255,255,255));
		Show(false);
	}
}
void CDlgStoneTransfer::OnHideDialog()
{
	for (int i=0;i<RECIPE_NUM;i++)
	{
		if (m_pNewStones[i])
		{
			delete m_pNewStones[i];
			m_pNewStones[i] = NULL;
		}	
	}

	ClearMaterial();
}

void CDlgStoneTransfer::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Lbl_StoneName",m_pStoneName);
	DDX_Control("Lbl_Attribute",m_pStoneAttribute);
	DDX_Control("Edt_Money",m_pMoney);
	DDX_Control("Btn_Transfer",m_pTranferBtn);

	AString str;
	int i(0);
	for (i=0;i<RECIPE_NUM;i++)
	{
		str.Format("Img_Gem%d",i+1);
		DDX_Control(str,m_pStoneImg[i]);
	}
	for (i=0;i<MATERIAL_NUM;i++)
	{
		str.Format("Img_Item%d",i+1);
		DDX_Control(str,m_pMaterialImg[i]);
	}
}

void CDlgStoneTransfer::OnCommandCANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgStoneTransfer::OnCommandTransfer(const char * szCommand)
{
	bool bCanTransfer = true;
	for (int i=0;i<MATERIAL_NUM;i++)
	{
		int iNeedCount = m_recipes[m_iNewStoneIndex]->materials[i].num;		
		if(iNeedCount<=0) continue;
		
		if (m_iMaterailSlot[i]>=0) // 有材料被拖上来了
		{			
			CECIvtrItem* pItem = GetHostPlayer()->GetPack()->GetItem(m_iMaterailSlot[i]);
			bool bMeet = pItem && pItem->GetCount() >= iNeedCount;

			if(!bMeet) bCanTransfer = false;			
		}
		else				
			bCanTransfer = false;		
	}
	
	if (GetHostPlayer()->GetMoneyAmount()<m_recipes[m_iNewStoneIndex]->money)
		bCanTransfer = false;

	int iNewLevel = m_pNewStones[m_iNewStoneIndex] ? m_pNewStones[m_iNewStoneIndex]->GetDBEssence()->level : 0;
	int iEquipLevel = 0;
	CECIvtrItem* pEquipItem = GetHostPlayer()->GetPack()->GetItem(GetGameUIMan()->m_pDlgStoneChange->GetEquipSlot());
	if (pEquipItem && pEquipItem->IsEquipment())
	{
		switch( pEquipItem->GetClassID() )
		{
		case CECIvtrItem::ICID_WEAPON:
			iEquipLevel = ((CECIvtrWeapon*)pEquipItem)->GetDBEssence()->level;
			break;
		case CECIvtrItem::ICID_ARMOR:
			iEquipLevel = ((CECIvtrArmor*)pEquipItem)->GetDBEssence()->level;
			break;
		case CECIvtrItem::ICID_DECORATION:
			iEquipLevel = ((CECIvtrDecoration*)pEquipItem)->GetDBEssence()->level;
			break;
		}
		
		if( iNewLevel > iEquipLevel )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10179), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160));
			
			return;
		}
	}

	if (!bCanTransfer)
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10175),MB_OK,A3DCOLORRGB(255,255,255));
		return;
	}


	GetGameUIMan()->MessageBox("StoneTransfer",GetGameUIMan()->GetStringFromTable(10174),MB_OKCANCEL,A3DCOLORRGB(255,255,255));
}
void CDlgStoneTransfer::DoTransfer()
{	
	CDlgStoneChange* pDlgChange = GetGameUIMan()->m_pDlgStoneChange;
	int iEquipSlot = pDlgChange->GetEquipSlot();
	int iOldStoneSlot = pDlgChange->GetHoleIndex();
	int iOldStoneTid = pDlgChange->GetOldStoneTid();
	
	if(!m_recipes[m_iNewStoneIndex]) return;

	int iNewStoneTid = m_recipes[m_iNewStoneIndex]->id_new_stone;
	int recipe_tid = m_recipes[m_iNewStoneIndex]->id;

	int material_id[MATERIAL_NUM] = {-1};

	for (int i=0;i<MATERIAL_NUM;i++)
	{
		material_id[i] = m_pMaterialImg[i]->GetData();
	}

	g_pGame->GetGameSession()->c2s_CmdNPCSevStoneTransfer(iEquipSlot,iOldStoneSlot,iOldStoneTid,iNewStoneTid,recipe_tid,material_id,m_iMaterailSlot);

	
}
void CDlgStoneTransfer::OnEventLButtonDown_Stone(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (pObj && strstr(pObj->GetName(),"Img_Gem"))
	{
		int iNewStoneIndex = atoi(pObj->GetName()+strlen("Img_Gem")) - 1;
		
		if(!m_pNewStones[iNewStoneIndex]) return;

		for (int i=0;i<RECIPE_NUM;i++)
			m_pStoneImg[i]->SetCover(NULL,-1,1);		

		m_pStoneImg[iNewStoneIndex]->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);

		SelectRecipe(iNewStoneIndex);
	}
}

void CDlgStoneTransfer::OnEventLButtonDown_Material(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (pObj && strstr(pObj->GetName(),"Img_Item"))
	{
		if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
			return;
		
		CECIvtrItem *pItem = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
		if( pItem )
		{
			pItem->Freeze(false);
			PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
			int iSlot = atoi(pObj->GetName() + strlen("Img_Item")) - 1;
			m_iMaterailSlot[iSlot] = -1;
			pImage->SetDataPtr(NULL);
			UpdateMaterial();
		}
	}
}
void CDlgStoneTransfer::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	int iDst = -1;
	if (pObjOver){
		if (strstr(pObjOver->GetName(), "Img_Item") && 
			pIvtrSrc->GetTemplateID() == (int)pObjOver->GetData()){			
			iDst = atoi(pObjOver->GetName() + strlen("Img_Item"));	//	直接拖拽
		}else{
			return;
		}
	}else if (m_iNewStoneIndex >= 0){
		//	右键点击
		CECInventory *pPack = GetHostPlayer()->GetPack();
		int iFirstMatch(-1), iFirstNotFull(-1), iFirstEmpty(-1);
		for (int i(0); i < MATERIAL_NUM; ++ i)
		{
			int id = m_recipes[m_iNewStoneIndex]->materials[i].id;
			if (id == 0 || id != pIvtrSrc->GetTemplateID()){
				continue;
			}
			if (m_iMaterailSlot[i] < 0){
				if (iFirstEmpty < 0){
					iFirstEmpty = i+1;	//	第一个待填项
				}
				continue;
			}
			int num = m_recipes[m_iNewStoneIndex]->materials[i].num;
			CECIvtrItem* pItem = pPack->GetItem(m_iMaterailSlot[i]);
			if (!pItem){
				if (iFirstEmpty < 0){
					iFirstEmpty = i+1;
				}
				continue;
			}
			if (pItem->GetCount() < num){
				if (iFirstNotFull < 0){
					iFirstNotFull = i+1;
				}
				continue;
			}
			if (iFirstMatch < 0){
				iFirstMatch = i+1;
			}
		}
		if (iFirstEmpty >= 0){
			iDst = iFirstEmpty;			//	优先填充空项
		}else if (iFirstNotFull >= 0){
			iDst = iFirstNotFull;		//	没有空项时、填充虽有内容但未满项
		}else if (iFirstMatch >= 0){
			iDst = iFirstMatch;			//	没有空项、未满项时，替换第一个匹配项
		}
	}
	if (iDst >= 0){
		SetMaterial(iSrc, iDst);
	}
}

void CDlgStoneTransfer::SetMaterial(int iSrc,int iDst)
{
	char szName[20];
	sprintf(szName, "Img_Item%d", iDst);
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
	OnEventLButtonDown_Material(0, 0, pImage);
	
	CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(iSrc);
	m_iMaterailSlot[iDst - 1] = iSrc;
	pImage->SetDataPtr(pItem, "ptr_CECIvtrItem");
	pItem->Freeze(true);

	UpdateMaterial();
}
void CDlgStoneTransfer::ClearMaterial()
{
	int i;
	for (i=0;i<MATERIAL_NUM;i++)
	{
		OnEventLButtonDown_Material(0,0,m_pMaterialImg[i]);
	}

	for (i=0;i<MATERIAL_NUM;i++)
	{
		m_pMaterialImg[i]->ClearCover();
		m_pMaterialImg[i]->SetHint(_AL(""));
		m_pMaterialImg[i]->SetData(0);
		m_pMaterialImg[i]->SetDataPtr(NULL);
		m_pMaterialImg[i]->SetText(_AL(""));
		m_pMaterialImg[i]->SetColor(A3DCOLORRGB(255,255,255));		

		m_iMaterailSlot[i] = -1;
	}
	m_pMoney->SetText(_AL(""));
}
void CDlgStoneTransfer::SelectRecipe(int index)
{	
	if(index == m_iNewStoneIndex) return;

	ClearMaterial();
	m_iNewStoneIndex = index;

	if (index>=0 && index<RECIPE_NUM && m_pNewStones[index])
	{	
		m_pStoneName->SetText(m_pNewStones[index]->GetName());

		m_pStoneAttribute->SetText(GetGameUIMan()->m_pDlgStoneChange->IsWeapon() ? 
			m_pNewStones[index]->GetDBEssence()->weapon_desc:m_pNewStones[index]->GetDBEssence()->armor_desc);

		for (int i=0;i<MATERIAL_NUM;i++)
		{
			if(m_recipes[index]->materials[i].id==0) continue;

			CECIvtrItem* pItem = CECIvtrItem::CreateItem(m_recipes[index]->materials[i].id,0,m_recipes[index]->materials[i].num);
			if (pItem)
			{
				AString strFile;
				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				m_pMaterialImg[i]->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));

				AUICTranslate trans;
				m_pMaterialImg[i]->SetHint(trans.Translate(pItem->GetDesc()));

				ACString strCount;
				strCount.Format(_AL("%d/%d"),0,pItem->GetCount());
				m_pMaterialImg[i]->SetText(strCount);

				m_pMaterialImg[i]->SetData(pItem->GetTemplateID());			

				delete pItem;
			}
		}

		UpdateMaterial();

		m_pMoney->SetText(GetGameUIMan()->GetFormatNumber(m_recipes[index]->money));		
		bool bNoMoney = GetHostPlayer()->GetMoneyAmount()<m_recipes[index]->money;
		m_pMoney->SetColor(bNoMoney ? A3DCOLORRGB(255,0,0):A3DCOLORRGB(0,255,0));
	}
	else
	{
		m_pStoneName->SetText(_AL(""));
		m_pStoneAttribute->SetText(_AL(""));
	}
}
void CDlgStoneTransfer::UpdateMaterial()
{
	bool bCanTransfer = true;
	ACString strCount;
	for (int i=0;i<MATERIAL_NUM;i++)
	{
		int iNeedCount = m_recipes[m_iNewStoneIndex]->materials[i].num;

		if(iNeedCount<=0) continue;

		if (m_iMaterailSlot[i]>=0) // 有材料被拖上来了
		{			
			CECIvtrItem* pItem = GetHostPlayer()->GetPack()->GetItem(m_iMaterailSlot[i]);
			if (pItem && pItem->GetCount() >= iNeedCount)
			{
				m_pMaterialImg[i]->SetColor(A3DCOLORRGB(255,255,255));
			}
			else
			{
				m_pMaterialImg[i]->SetColor(A3DCOLORRGB(192,0,0));
				bCanTransfer = false;
			}

			strCount.Format(_AL("%d/%d"),pItem ? pItem->GetCount():0,iNeedCount);
			m_pMaterialImg[i]->SetText(strCount);
		}
		else
		{
			m_pMaterialImg[i]->SetColor(A3DCOLORRGB(192,0,0));
			strCount.Format(_AL("%d/%d"),0,iNeedCount);
			m_pMaterialImg[i]->SetText(strCount);
			bCanTransfer = false;
		}
	}

	if (GetHostPlayer()->GetMoneyAmount()<m_recipes[m_iNewStoneIndex]->money)
		bCanTransfer = false;

//	m_pTranferBtn->Enable(bCanTransfer);
	m_pTranferBtn->Enable(true);

}
void CDlgStoneTransfer::EnumRecipe(unsigned int oldStone_tid)
{
	memset(m_recipes,0,sizeof(m_recipes));

	abase::vector<STONE_CHANGE_RECIPE*> recipe_vector = GetGameUIMan()->m_pDlgStoneChange->GetRecipe(oldStone_tid);

	unsigned int	i = 0;
	while((i < recipe_vector.size()) && (i < RECIPE_NUM))
	{
	//	int index = a_Random(0,recipe_vector.size()-1);
		m_recipes[i] = recipe_vector[i];
	//	recipe_vector.erase(recipe_vector.begin()+index);
		i++;
	}		
}