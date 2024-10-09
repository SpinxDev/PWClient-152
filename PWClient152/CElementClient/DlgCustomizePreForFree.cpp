/*
 * FILE: DlgCustomizePreForFree.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/9/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#include "DlgCustomizePreForFree.h"

#include "EC_CustomizeMgr.h"

#include "EC_Game.h"
#include "EC_Face.h"

#include "AUIListBox.h"
#include "FaceBoneController.h"
#include "EC_Game.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "EC_LoginUIMan.h"
#include "EC_GameUIMan.h"
#include "EC_Viewport.h"

#include "elementdataman.h"

#include "A3DViewport.h"
#include "WindowsX.h"

AUI_BEGIN_COMMAND_MAP(CDlgCustomizePreForFree, CDlgCustomizeBase)


AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgCustomizePreForFree, CDlgCustomizeBase)

AUI_ON_EVENT("ListBox_PreCustomize", WM_LBUTTONUP, OnLButtonUpListBoxPreCus)
AUI_ON_EVENT("ListBox_PreCustomize", WM_KEYDOWN, OnKeyDownListBoxPreCus)

AUI_END_EVENT_MAP()

CDlgCustomizePreForFree::CDlgCustomizePreForFree()
{
	m_pListBoxPreCustomize = NULL;
	
}

CDlgCustomizePreForFree::~CDlgCustomizePreForFree()
{
	Clear();
}

void CDlgCustomizePreForFree::Clear()
{

	m_pListBoxPreCustomize = NULL;
		
}

bool CDlgCustomizePreForFree::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;

	//重置列表框
	if(m_pListBoxPreCustomize)
		m_pListBoxPreCustomize->ResetContent();

	if (!InitResource())
		return false;

	UpdateData(false);

	return true;

}

//----------------------------------------------------------
bool CDlgCustomizePreForFree::InitResource()
{
	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= GetGame()->GetElementDataMan ();

	//数据空间ID
	ID_SPACE eIDSpace = ID_SPACE_FACE;		

	//获取脸型相关数据数量
	int nNumFaceData;
	nNumFaceData = pElementDataMan->get_data_num( eIDSpace);

	//脸型相关的数据类型
	DATA_TYPE dtFaceData;
	
	CECFace::FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();

	int nCharacterMask = 1 << GetPlayer()->GetProfession();

	unsigned int uPreCustomizeCount = 0;


	//遍历数据库
	for( int i = 0; i < nNumFaceData; i++)
	{
		//获取脸型数据id
		unsigned int uFaceDataID = pElementDataMan->get_data_id(eIDSpace, i, dtFaceData);

		//与预定义相关的数据
		if( dtFaceData == DT_CUSTOMIZEDATA_ESSENCE)
		{
			CUSTOMIZEDATA_ESSENCE* pCustomizedDataEssence = (CUSTOMIZEDATA_ESSENCE*)pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData);
			if( !pCustomizedDataEssence)
				continue;
			
			//当前人物id和性别id
			int uCurCharacterComboID = pCustomizedDataEssence->character_combo_id ;
			int uCurGenderID = pCustomizedDataEssence->gender_id ;

			int id = pCustomizedDataEssence->id;
			
			ACHAR* pszName = pCustomizedDataEssence->name;
			char* pszFile = pCustomizedDataEssence->file_data;

			//当前人物的预设数据
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{
				m_pListBoxPreCustomize->InsertString(uPreCustomizeCount, pszName);
				m_pListBoxPreCustomize->SetItemDataPtr(uPreCustomizeCount,pszFile);
				m_pListBoxPreCustomize->SetItemHint(uPreCustomizeCount, pszName);
				
				uPreCustomizeCount++;
		
			}

		}//if与预定义相关的数据
		
	}//遍历数据库

	m_pListBoxPreCustomize->SortItems( AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_STRING);

	return true;
}

void CDlgCustomizePreForFree::DoDataExchange(bool bSave)
{
	//ddx control
	DDX_Control("ListBox_PreCustomize", m_pListBoxPreCustomize);

	if (!IsContextReady()) return;
	//ddx data

}

void CDlgCustomizePreForFree::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	SetCanMove(false);

	//改变场景
	int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();

	if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
	{
		((CECLoginUIMan*)m_pAUIManager)->ChangeScene((CECLoginUIMan::LOGIN_SCENE)(CECLoginUIMan::LOGIN_SCENE_BODYCUSTOMIZE_BEGIN + GetPlayer()->GetProfession()));
	}
	else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
	{
		GetCustomizeMan()->ChangeScene(FACE_SCENE_STATURE);
		
	}
	
}

void CDlgCustomizePreForFree::OnLButtonUpListBoxPreCus(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//判断是否点中列表框中的选项
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxPreCustomize->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxPreCustomize->GetCurSel();
	if( nCurSel >= 0 && nCurSel < m_pListBoxPreCustomize->GetCount())
	{
	

		char* pszFile = (char*)m_pListBoxPreCustomize->GetItemDataPtr(nCurSel);
		

		if(!LoadCustomizeData(pszFile))
		{
			int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();

			if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
			{
					((CECLoginUIMan*)m_pAUIManager)->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				
				return;
			}
			else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
			{
				((CECGameUIMan*)m_pAUIManager)->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
		
		}


	}
}

//------------------------------------------------------------------------------------------
void CDlgCustomizePreForFree::OnKeyDownListBoxPreCus(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( wParam != VK_UP && wParam != VK_DOWN)
	{
		return;
	}

	int nCurSel = m_pListBoxPreCustomize->GetCurSel();
	if( nCurSel >= 0 && nCurSel < m_pListBoxPreCustomize->GetCount())
	{
		char* pszFile = (char*)m_pListBoxPreCustomize->GetItemDataPtr(nCurSel);
		

		if(!LoadCustomizeData(pszFile))
		{
			int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();

			if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
			{
				((CECLoginUIMan*)m_pAUIManager)->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				
				return;
			}
			else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
			{
				((CECGameUIMan*)m_pAUIManager)->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
		
		}

	}
}


// 载入形象的个性化数据
bool CDlgCustomizePreForFree::LoadCustomizeData(char* pszFile)
{
	CECPlayer::PLAYER_CUSTOMIZEDATA data;
	memset(&data, 0, sizeof(data));
	data.dwVersion = CUSTOMIZE_DATA_VERSION;

	AIniFile IniFile;
	
	if(!IniFile.Open(pszFile))
	{
		a_LogOutput(1, "CECCustomize::LoadCustomizeData, Failed to open file %s", pszFile);
		return false;
	}

	//3庭3
	AString strSection = "3Parts";
	data.faceData.scaleUp = IniFile.GetValueAsInt(strSection, "scaleUp", data.faceData.scaleUp);
	data.faceData.scaleMiddle = IniFile.GetValueAsInt(strSection, "scaleMiddle", data.faceData.scaleMiddle);
	data.faceData.scaleDown = IniFile.GetValueAsInt(strSection, "scaleDown", data.faceData.scaleDown);

	//脸型融合3
	strSection = "BlendFace";
	data.faceData.idFaceShape1 = IniFile.GetValueAsInt(strSection, "idFaceShape1", data.faceData.idFaceShape1);
	data.faceData.idFaceShape2 = IniFile.GetValueAsInt(strSection, "idFaceShape2", data.faceData.idFaceShape2);
	data.faceData.blendFaceShape = IniFile.GetValueAsInt(strSection, "blendFaceShape", data.faceData.blendFaceShape);

	
	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE, data.faceData.idFaceShape1, 0))
	{
		IniFile.Close();
		return false;
	}

	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE, data.faceData.idFaceShape2, 0))
	{
		IniFile.Close();
		return false;
	}

		

	//脸盘调整3
	strSection = "Face";
	data.faceData.scaleFaceH = IniFile.GetValueAsInt(strSection, "scaleFaceH", data.faceData.scaleFaceH);
	data.faceData.scaleFaceV = IniFile.GetValueAsInt(strSection, "scaleFaceV", data.faceData.scaleFaceV);
	data.faceData.idFaceTex = IniFile.GetValueAsInt(strSection, "idFaceTex", data.faceData.idFaceTex);


	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE, data.faceData.idFaceTex, 0))
	{
		IniFile.Close();
		return false;
	}

	//法令
	strSection = "Faling";
	data.faceData.idFalingSkin = IniFile.GetValueAsInt(strSection, "idFalingSkin", data.faceData.idFalingSkin);
	data.faceData.idFalingTex = IniFile.GetValueAsInt(strSection, "idFalingTex", data.faceData.idFalingTex);

	if(!MakeSureCustomizeData(DT_FACE_FALING_ESSENCE , data.faceData.idFalingSkin))
	{
		IniFile.Close();
		return false;
	}
	
	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE, data.faceData.idFalingTex, 10))
	{
		IniFile.Close();
		return false;
	}

	//额头5
	strSection = "Forehead";
	data.faceData.offsetForeheadH = IniFile.GetValueAsInt(strSection, "offsetForeheadH", data.faceData.offsetForeheadH);
	data.faceData.offsetForeheadV = IniFile.GetValueAsInt(strSection, "offsetForeheadV", data.faceData.offsetForeheadV);
	data.faceData.offsetForeheadZ = IniFile.GetValueAsInt(strSection, "offsetForeheadZ", data.faceData.offsetForeheadZ);
	data.faceData.rotateForehead = IniFile.GetValueAsInt(strSection, "rotateForehead", data.faceData.rotateForehead);
	data.faceData.scaleForehead = IniFile.GetValueAsInt(strSection, "scaleForehead", data.faceData.scaleForehead);

	//颧骨5
	strSection = "YokeBone";
	data.faceData.offsetYokeBoneH = IniFile.GetValueAsInt(strSection, "offsetYokeBoneH", data.faceData.offsetYokeBoneH);
	data.faceData.offsetYokeBoneV = IniFile.GetValueAsInt(strSection, "offsetYokeBoneV", data.faceData.offsetYokeBoneV);
	data.faceData.offsetYokeBoneZ = IniFile.GetValueAsInt(strSection, "offsetYokeBoneZ", data.faceData.offsetYokeBoneZ);
	data.faceData.rotateYokeBone = IniFile.GetValueAsInt(strSection, "rotateYokeBone", data.faceData.rotateYokeBone);
	data.faceData.scaleYokeBone = IniFile.GetValueAsInt(strSection, "scaleYokeBone", data.faceData.scaleYokeBone);
	
	//脸颊4
	strSection = "Cheek";
	data.faceData.offsetCheekH = IniFile.GetValueAsInt(strSection, "offsetCheekH", data.faceData.offsetCheekH);
	data.faceData.offsetCheekV = IniFile.GetValueAsInt(strSection, "offsetCheekV", data.faceData.offsetCheekV);
	data.faceData.offsetCheekZ = IniFile.GetValueAsInt(strSection, "offsetCheekZ", data.faceData.offsetCheekZ);
	data.faceData.scaleCheek = IniFile.GetValueAsInt(strSection, "scaleCheek", data.faceData.scaleCheek);

	//下巴4
	strSection = "Chain";
	data.faceData.offsetChainV = IniFile.GetValueAsInt(strSection, "offsetChainV", data.faceData.offsetChainV);
	data.faceData.offsetChainZ = IniFile.GetValueAsInt(strSection, "offsetChainZ", data.faceData.offsetChainZ);
	data.faceData.rotateChain = IniFile.GetValueAsInt(strSection, "rotateChain", data.faceData.rotateChain);
	data.faceData.scaleChainH = IniFile.GetValueAsInt(strSection, "scaleChainH", data.faceData.scaleChainH);

	//颌骨6
	strSection = "Jaw";
	data.faceData.offsetJawH = IniFile.GetValueAsInt(strSection, "offsetJawH", data.faceData.offsetJawH);
	data.faceData.offsetJawV = IniFile.GetValueAsInt(strSection, "offsetJawV", data.faceData.offsetJawV);
	data.faceData.offsetJawZ = IniFile.GetValueAsInt(strSection, "offsetJawZ", data.faceData.offsetJawZ);
	data.faceData.scaleJawSpecial = IniFile.GetValueAsInt(strSection, "scaleJawSpecial", data.faceData.scaleJawSpecial);
	data.faceData.scaleJawH = IniFile.GetValueAsInt(strSection, "scaleJawH", data.faceData.scaleJawH);
	data.faceData.scaleJawV = IniFile.GetValueAsInt(strSection, "scaleJawV", data.faceData.scaleJawV);

	//眼睛18
	strSection = "Eye";
	data.faceData.idThirdEye	= IniFile.GetValueAsInt(strSection, "idThirdEye", data.faceData.idThirdEye);
	data.faceData.idEyeBaseTex = IniFile.GetValueAsInt(strSection, "idEyeBaseTex", data.faceData.idEyeBaseTex);
	data.faceData.idEyeHighTex = IniFile.GetValueAsInt(strSection, "idEyeHighTex", data.faceData.idEyeHighTex);
	data.faceData.idEyeBallTex = IniFile.GetValueAsInt(strSection, "idEyeBallTex", data.faceData.idEyeBallTex);
	data.faceData.idEyeShape = IniFile.GetValueAsInt(strSection, "idEyeShape", data.faceData.idEyeShape);
	data.faceData.scaleEyeH = IniFile.GetValueAsInt(strSection, "scaleEyeH", data.faceData.scaleEyeH);
	data.faceData.scaleEyeV = IniFile.GetValueAsInt(strSection, "scaleEyeV", data.faceData.scaleEyeV);
	data.faceData.rotateEye = IniFile.GetValueAsInt(strSection, "rotateEye", data.faceData.rotateEye);
	data.faceData.offsetEyeH = IniFile.GetValueAsInt(strSection, "offsetEyeH", data.faceData.offsetEyeH);
	data.faceData.offsetEyeV = IniFile.GetValueAsInt(strSection, "offsetEyeV", data.faceData.offsetEyeV);
	data.faceData.offsetEyeZ = IniFile.GetValueAsInt(strSection, "offseteyeZ", data.faceData.offsetEyeZ);
	data.faceData.scaleEyeBall = IniFile.GetValueAsInt(strSection, "scaleEyeBall", data.faceData.scaleEyeBall);

	data.faceData.scaleEyeH2 = IniFile.GetValueAsInt(strSection, "scaleEyeH2", data.faceData.scaleEyeH2);
	data.faceData.scaleEyeV2 = IniFile.GetValueAsInt(strSection, "scaleEyeV2", data.faceData.scaleEyeV2);
	data.faceData.rotateEye2= IniFile.GetValueAsInt(strSection, "rotateEye2", data.faceData.rotateEye2);
	data.faceData.offsetEyeH2 = IniFile.GetValueAsInt(strSection, "offsetEyeH2", data.faceData.offsetEyeH2);
	data.faceData.offsetEyeV2 = IniFile.GetValueAsInt(strSection, "offsetEyeV2", data.faceData.offsetEyeV2);
	data.faceData.offsetEyeZ2 = IniFile.GetValueAsInt(strSection, "offseteyeZ2", data.faceData.offsetEyeZ2);
	data.faceData.scaleEyeBall2 = IniFile.GetValueAsInt(strSection, "scaleEyeBall2", data.faceData.scaleEyeBall2);
	
	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE, data.faceData.idEyeShape, 1))
	{
		IniFile.Close();
		return false;
	}
	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE, data.faceData.idEyeBaseTex, 1))
	{
		IniFile.Close();
		return false;
	}
	
	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE, data.faceData.idEyeHighTex, 2))
	{
		IniFile.Close();
		return false;
	}
	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE, data.faceData.idEyeBallTex, 6))
	{
		IniFile.Close();
		return false;
	}


	//眉毛14
	strSection = "Brow";
	data.faceData.idBrowTex = IniFile.GetValueAsInt(strSection, "idBrowTex", data.faceData.idBrowTex);
	data.faceData.idBrowShape = IniFile.GetValueAsInt(strSection, "idBrowShape", data.faceData.idBrowShape);
	data.faceData.scaleBrowH = IniFile.GetValueAsInt(strSection, "scaleBrowH", data.faceData.scaleBrowH);
	data.faceData.scaleBrowV = IniFile.GetValueAsInt(strSection, "scaleBrowV", data.faceData.scaleBrowV);
	data.faceData.rotateBrow = IniFile.GetValueAsInt(strSection, "rotateBrow", data.faceData.rotateBrow);
	data.faceData.offsetBrowH = IniFile.GetValueAsInt(strSection, "offsetBrowH", data.faceData.offsetBrowH);
	data.faceData.offsetBrowV = IniFile.GetValueAsInt(strSection, "offsetBrowV", data.faceData.offsetBrowV);
	data.faceData.offsetBrowZ = IniFile.GetValueAsInt(strSection, "offsetBrowZ", data.faceData.offsetBrowZ);
	
	data.faceData.scaleBrowH2 = IniFile.GetValueAsInt(strSection, "scaleBrowH2", data.faceData.scaleBrowH2);
	data.faceData.scaleBrowV2 = IniFile.GetValueAsInt(strSection, "scaleBrowV2", data.faceData.scaleBrowV2);
	data.faceData.rotateBrow2 = IniFile.GetValueAsInt(strSection, "rotateBrow2", data.faceData.rotateBrow2);
	data.faceData.offsetBrowH2 = IniFile.GetValueAsInt(strSection, "offsetBrowH2", data.faceData.offsetBrowH2);
	data.faceData.offsetBrowV2 = IniFile.GetValueAsInt(strSection, "offsetBrowV2", data.faceData.offsetBrowV2);
	data.faceData.offsetBrowZ2 = IniFile.GetValueAsInt(strSection, "offsetBrowZ2", data.faceData.offsetBrowZ2);

	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE,data.faceData.idBrowShape, 2))
	{
		IniFile.Close();
		return false;
	}
	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE, data.faceData.idBrowTex, 3))
	{
		IniFile.Close();
		return false;
	}

	//鼻子9
	strSection = "Nose";
	data.faceData.idNoseTex = IniFile.GetValueAsInt(strSection, "idNoseTex", data.faceData.idNoseTex);
	data.faceData.idNoseTipShape = IniFile.GetValueAsInt(strSection, "idNoseTipShape", data.faceData.idNoseTipShape);
	data.faceData.scaleNoseTipH = IniFile.GetValueAsInt(strSection, "scaleNoseTipH", data.faceData.scaleNoseTipH);
	data.faceData.scaleNoseTipV = IniFile.GetValueAsInt(strSection, "scaleNoseTipV", data.faceData.scaleNoseTipV);
	data.faceData.scaleNoseTipZ = IniFile.GetValueAsInt(strSection, "scaleNoseTipZ", data.faceData.scaleNoseTipZ);
	data.faceData.offsetNoseTipV = IniFile.GetValueAsInt(strSection, "offsetNoseTipV", data.faceData.offsetNoseTipV);
	data.faceData.idNoseBridgeShape = IniFile.GetValueAsInt(strSection, "idNoseBridgeShape", data.faceData.idNoseBridgeShape);
	data.faceData.scaleBridgeTipH = IniFile.GetValueAsInt(strSection, "scaleBridgeTipH", data.faceData.scaleBridgeTipH);
	data.faceData.offsetBridgeTipZ = IniFile.GetValueAsInt(strSection, "OffsetBridgeTipZ", data.faceData.offsetBridgeTipZ);

	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE,data.faceData.idNoseTipShape, 3))
	{
		IniFile.Close();
		return false;
	}

	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE,data.faceData.idNoseBridgeShape, 4))
	{
		IniFile.Close();
		return false;
	}

	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE, data.faceData.idNoseTex, 5))
	{
		IniFile.Close();
		return false;
	}
	

	//嘴11
	strSection = "Mouth";
	data.faceData.idMouthUpLipLine = IniFile.GetValueAsInt (strSection, "idMouthUpLipLine", data.faceData.idMouthUpLipLine);
	data.faceData.idMouthMidLipLine = IniFile.GetValueAsInt(strSection, "idMouthMidLipLine", data.faceData.idMouthMidLipLine);
	data.faceData.idMouthDownLipLine = IniFile.GetValueAsInt(strSection, "idMouthDownLipLine", data.faceData.idMouthDownLipLine);
	data.faceData.thickUpLip = IniFile.GetValueAsInt(strSection, "thickUpLip", data.faceData.thickUpLip);
	data.faceData.thickDownLip = IniFile.GetValueAsInt(strSection, "thickDownLip", data.faceData.thickDownLip);
	data.faceData.scaleMouthH = IniFile.GetValueAsInt(strSection, "scaleMouthH", data.faceData.scaleMouthH);
	data.faceData.offsetMouthV = IniFile.GetValueAsInt(strSection, "offsetMouthV", data.faceData.offsetMouthV);
	data.faceData.offsetMouthZ = IniFile.GetValueAsInt(strSection, "offsetMOuthZ", data.faceData.offsetMouthZ);
	data.faceData.idMouthTex = IniFile.GetValueAsInt(strSection, "idMouthTex", data.faceData.idMouthTex);
	data.faceData.offsetCornerOfMouthSpecial = IniFile.GetValueAsInt(strSection, "offsetCornerOfMouthSpecial", data.faceData.offsetCornerOfMouthSpecial);

	data.faceData.scaleMouthH2 = IniFile.GetValueAsInt(strSection, "scaleMouthH2", data.faceData.scaleMouthH2);
	data.faceData.offsetCornerOfMouthSpecial2 = IniFile.GetValueAsInt(strSection, "offsetCornerOfMouthSpecial2", data.faceData.offsetCornerOfMouthSpecial2);

	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE,data.faceData.idMouthUpLipLine, 5))
	{
		IniFile.Close();
		return false;
	}

	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE,data.faceData.idMouthMidLipLine, 6))
	{
		IniFile.Close();
		return false;
	}

	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE,data.faceData.idMouthDownLipLine, 7))
	{
		IniFile.Close();
		return false;
	}

	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE, data.faceData.idMouthTex, 4))
	{
		IniFile.Close();
		return false;
	}
	
	//耳朵3
	strSection = "Ear";
	data.faceData.idEarShape = IniFile.GetValueAsInt(strSection, "idEarShape", data.faceData.idEarShape);
	data.faceData.scaleEar = IniFile.GetValueAsInt(strSection, "scaleEar", data.faceData.scaleEar);
	data.faceData.offsetEarV = IniFile.GetValueAsInt(strSection, "offsetEarV", data.faceData.offsetEarV);

	if(!MakeSureCustomizeData(DT_FACE_SHAPE_ESSENCE,data.faceData.idEarShape, 8))
	{
		IniFile.Close();
		return false;
	}


	//头发1
	strSection = "Hair";
	data.faceData.idHairModel = IniFile.GetValueAsInt(strSection, "idHairModel", data.faceData.idHairModel);
	data.faceData.idHairTex = IniFile.GetValueAsInt(strSection, "idHairTex", data.faceData.idHairTex);

	if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE,data.faceData.idHairTex, 8))
	{
		IniFile.Close();
		return false;
	}

	if(!MakeSureCustomizeData(DT_FACE_HAIR_ESSENCE,data.faceData.idHairModel))
	{
		IniFile.Close();
		return false;
	}

	//胡子2
	strSection = "Moustache";
	data.faceData.idMoustacheTex = IniFile.GetValueAsInt(strSection, "idMoustacheTex", data.faceData.idMoustacheTex);
	data.faceData.idMoustacheSkin = IniFile.GetValueAsInt(strSection, "idMoustacheSkin", data.faceData.idMoustacheSkin);
	data.faceData.idGoateeTex = IniFile.GetValueAsInt(strSection, "idGoateeTex", data.faceData.idGoateeTex);

	//检查男性胡须
	if( GetPlayer()->GetGender() == 0)
	{

		if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE,data.faceData.idMoustacheTex, 7))
		{
			IniFile.Close();
			return false;
		}

		if(!MakeSureCustomizeData(DT_FACE_TEXTURE_ESSENCE,data.faceData.idGoateeTex, 9))
		{
			IniFile.Close();
			return false;
		}
		
		if(!MakeSureCustomizeData(DT_FACE_MOUSTACHE_ESSENCE,data.faceData.idMoustacheSkin))
		{
			IniFile.Close();
			return false;
		}
	
	}
	
	//各部分颜色7
	strSection = "Color";
	data.faceData.colorHair = IniFile.GetValueAsInt(strSection, "colorHair", data.faceData.colorHair);
	data.faceData.colorFace = IniFile.GetValueAsInt(strSection, "colorFace", data.faceData.colorFace);
	data.faceData.colorEye = IniFile.GetValueAsInt(strSection, "colorEye", data.faceData.colorEye);
	data.faceData.colorBrow = IniFile.GetValueAsInt(strSection, "colorBrow", data.faceData.colorBrow);
	data.faceData.colorMouth = IniFile.GetValueAsInt(strSection, "colorMouth", data.faceData.colorMouth);
	data.faceData.colorEyeBall = IniFile.GetValueAsInt(strSection, "colorEyeBall", data.faceData.colorEyeBall);
	data.faceData.colorMoustache = IniFile.GetValueAsInt(strSection, "colorMoustache", data.faceData.colorMoustache);

	// 身体参数
	strSection = "Body";
	data.bodyID = IniFile.GetValueAsInt(strSection, "bodyID", data.bodyID);
	data.colorBody = IniFile.GetValueAsInt(strSection, "colorBody", data.colorBody);
	data.headScale = IniFile.GetValueAsInt(strSection, "headScale", data.headScale);
	data.upScale = IniFile.GetValueAsInt(strSection, "upScale", data.upScale);
	data.waistScale = IniFile.GetValueAsInt(strSection, "waistScale", data.waistScale);
	data.armWidth = IniFile.GetValueAsInt(strSection, "armWidth", data.armWidth);
	data.legWidth = IniFile.GetValueAsInt(strSection, "legWidth", data.legWidth);
	data.breastScale = IniFile.GetValueAsInt(strSection, "breastScale", data.breastScale);
         
	IniFile.Close();
	
	GetPlayer()->ChangeCustomizeData(data);

	CECFace* pCurFace = GetPlayer()->GetFaceModel();
	
	pCurFace->UpdateAll();
	return true;
}

//确认个性化数据是否合法
//形状part:0-脸型、1-眼型、2-眉型、3-鼻头、4-鼻梁、5-上唇线、6-唇沟、7-下唇线、8-耳型
//纹理part: 0-脸、1-眼皮、2-眼影、3-眉、4-唇、5-鼻、6-眼珠、7-小胡子、8-头发、9-大胡子、10-法令
bool CDlgCustomizePreForFree::MakeSureCustomizeData( DATA_TYPE dt, unsigned int id, unsigned int part)
{
	//妖兽不判断
	if( GetPlayer()->GetProfession() == PROF_ORC)
		return true;

	elementdataman* pElementDataMan;
	pElementDataMan= GetGame()->GetElementDataMan ();
	DATA_TYPE dtFaceData;

	//脸部形状
	if( dt == DT_FACE_SHAPE_ESSENCE)
	{
		FACE_SHAPE_ESSENCE* pEssence = (FACE_SHAPE_ESSENCE*)pElementDataMan->get_data_ptr(id, ID_SPACE_FACE,dtFaceData);
		
		if(!pEssence)
			return false;

		if( !GetCustomizeMan()->m_bLoadFacePillData)
		{
			if( pEssence->facepill_only != 0)
				return false;
		}

		if( dtFaceData !=dt)
		{
			return false;
		}

		if( GetPlayer()->GetGender() != (int)pEssence->gender_id)
		{
			return false;
		}
		if( part != pEssence->shape_part_id)
		{
			return false;
		}
	}

	//脸部纹理
	if( dt == DT_FACE_TEXTURE_ESSENCE)
	{
		if( id == 0)
			return true;

		FACE_TEXTURE_ESSENCE* pEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(id, ID_SPACE_FACE,dtFaceData);

		if( !pEssence)
			return false;

		if( !GetCustomizeMan()->m_bLoadFacePillData)
		{
			if( pEssence->facepill_only != 0)
				return false;
		}

		if( dtFaceData !=dt)
		{
			return false;
		}

		if( GetPlayer()->GetGender() != (int)pEssence->gender_id)
		{
			return false;
		}
		
		if( part != pEssence->tex_part_id)
		{
			return false;
		}

	}
	
	//头发模型
	if( dt == DT_FACE_HAIR_ESSENCE)
	{
		FACE_HAIR_ESSENCE* pEssence = (FACE_HAIR_ESSENCE*)pElementDataMan->get_data_ptr(id, ID_SPACE_FACE,dtFaceData);
		if( !pEssence)
			return false;
		
		if( !GetCustomizeMan()->m_bLoadFacePillData)
		{
			if( pEssence->facepill_only != 0)
				return false;
		}

		if( dtFaceData !=dt)
		{
			return false;
		}

		if( GetPlayer()->GetGender() != (int)pEssence->gender_id)
		{
			return false;
		}
		
	}

	//胡子
	if( dt == DT_FACE_MOUSTACHE_ESSENCE)
	{
		FACE_MOUSTACHE_ESSENCE* pEssence = (FACE_MOUSTACHE_ESSENCE*)pElementDataMan->get_data_ptr(id, ID_SPACE_FACE,dtFaceData);

		if(!pEssence)
			return false;

		if( !GetCustomizeMan()->m_bLoadFacePillData)
		{
			if( pEssence->facepill_only != 0)
				return false;
		}

		if( dtFaceData !=dt)
		{
			return false;
		}

		if( GetPlayer()->GetGender() != (int)pEssence->gender_id)
		{
			return false;
		}
		
	}

	//法令
	if( dt == DT_FACE_FALING_ESSENCE)
	{
		FACE_FALING_ESSENCE* pEssence = (FACE_FALING_ESSENCE*)pElementDataMan->get_data_ptr(id, ID_SPACE_FACE,dtFaceData);
		if(!pEssence)
			return false;

		if( !GetCustomizeMan()->m_bLoadFacePillData)
		{
			if( pEssence->facepill_only != 0)
				return false;
		}
		
		if( dtFaceData !=dt)
		{
			return false;
		}

		if( GetPlayer()->GetGender() != (int) pEssence->gender_id)
		{
			return false;
		}
		
	}


	return true;
}



