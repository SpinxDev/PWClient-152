/*
 * FILE: DlgCustomizeEye.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "DlgCustomizeEye.h"
#include "DlgCustomizeThirdEye.h"
#include "EC_CustomizeMgr.h"

#include "EC_Face.h"
#include "AUIImagePicture.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"

#include "FaceBoneController.h"
#include "EC_Game.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "EC_LoginUIMan.h"
#include "elementdataman.h"
#include "EC_CustomizeBound.h"
#include "EC_Global.h"

#include "A3DViewport.h"
#include "WindowsX.h"

AUI_BEGIN_COMMAND_MAP(CDlgCustomizeEye, CDlgCustomizeBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgCustomizeEye, CDlgCustomizeBase)

AUI_ON_EVENT("Img_*",WM_LBUTTONUP, OnLButtonUpListbox)
AUI_ON_EVENT("Img_*", WM_MOUSEWHEEL, OnMouseWheelListbox)
AUI_ON_EVENT("Scroll_*",WM_MOUSEWHEEL, OnMouseWheelListbox)
AUI_ON_EVENT("Edit_*", WM_MOUSEWHEEL, OnMouseWheelListbox)
AUI_ON_EVENT("Img_Brow*",WM_LBUTTONUP, OnLButtonUpListbox)
AUI_ON_EVENT("Img_Brow*", WM_MOUSEWHEEL, OnMouseWheelListbox)

AUI_ON_EVENT("ImgThirdEye_*", WM_LBUTTONUP, OnLButtonUpListBoxThirdEye)
AUI_ON_EVENT("ScrollThirdEye_*", WM_MOUSEWHEEL, OnMouseWheelListboxThirdEye)
AUI_ON_EVENT("EditThirdEye_*", WM_MOUSEWHEEL, OnMouseWheelListboxThirdEye)
AUI_ON_EVENT("ImgThirdEye_*", WM_MOUSEWHEEL, OnMouseWheelListboxThirdEye)

AUI_END_EVENT_MAP()

//-----------------------------------------------------------
CDlgCustomizeEye::CDlgCustomizeEye()
{
}

//--------------------------------------------------------------
CDlgCustomizeEye::~CDlgCustomizeEye()
{
	m_LbxThirdEye.ResetImages(true);

	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}

//--------------------------------------------------------------
AUIStillImageButton * CDlgCustomizeEye::GetSwithButton()
{
	return GetCustomizeMan()->m_pBtnChooseEye;
}

//---------------------------------------------------------------
bool CDlgCustomizeEye::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;

	ClearListCtrlAndA2DSprite();
	
	A2DSprite *pSpriteSelect = GetCustomizeMan()->GetImageSelected();
	A2DSprite *pSpriteMouseOn = GetCustomizeMan()->GetImageMouseOn();
	if (!m_ListBoxEyeBaseTex.Init(this, "Img_01", 1, 3, "Scroll_01", "Edit_01", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxEyeShape.Init(this, "Img_04", 1, 3, "Scroll_02", "Edit_02", pSpriteSelect, pSpriteMouseOn) ||
		!m_LbxBrowTex.Init(this, "Img_Brow01", 1, 3, "Scroll_07", "Edit_07", pSpriteSelect, pSpriteMouseOn) ||
		!m_LbxBrowShape.Init(this, "Img_Brow04", 2, 3, "Scroll_08", "Edit_08", pSpriteSelect, pSpriteMouseOn))
		return false;

	if (!InitResource())
		return false;
	
	if(!InitThirdEye())
	{
		return false;
	}

	UpdateData(false);

	return true;
}

//---------------------------------------------------------------
bool CDlgCustomizeEye::InitResource()
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

	//数据计数

	//遍历数据库
	for( int i = 0; i < nNumFaceData; i++)
	{
		//获取脸型数据id
		unsigned int uFaceDataID = pElementDataMan->get_data_id(eIDSpace, i, dtFaceData);

		if( dtFaceData == DT_FACE_SHAPE_ESSENCE)
		{
			FACE_SHAPE_ESSENCE* pFaceShapeEssence = (FACE_SHAPE_ESSENCE*)pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData);
			
			if( !pFaceShapeEssence)
				continue;
			
			unsigned int uFacePillOnly = pFaceShapeEssence->facepill_only;

			if( !GetCustomizeMan()->m_bLoadFacePillData)
			{
				if( uFacePillOnly != 0)
					continue;
			}

			//当前人物id和性别id
			int uCurCharacterComboID = pFaceShapeEssence->character_combo_id ;
			int uCurGenderID = pFaceShapeEssence->gender_id ;

			//数据类型
			unsigned int uUserDataID = pFaceShapeEssence->user_data ;

			//当前人物的脸部形状
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{			
				//脸部各部分的id
				unsigned int uFaceShapePartID = pFaceShapeEssence->shape_part_id ;

				//**眼型**//
				if (uFaceShapePartID == 1)
				{	
					//数据id
					unsigned int id = pFaceShapeEssence->id ;

					//数据名称
					namechar* pszName = pFaceShapeEssence->name ;

					//形状数据文件
					char* pszShapeFile = pFaceShapeEssence->file_shape;

					//图标文件
					char* pszIconFile = pFaceShapeEssence->file_icon;

					//
					ACHAR* pszHintName = pFaceShapeEssence->name;
			
					
					if( *pszIconFile)
					{
						pszIconFile += 9;//跳过"surfaces\"	
					}
					else
					{
						a_LogOutput(1, "Icon file name is NULL, CDlgCustomizeEye::InitResource()");
						assert(0 && "Icon file name is NULL, CDlgCustomizeEye::InitResource()");
						return false;
					}
		
					//变图标文件为A2DSprite文件
					A2DSprite* pA2DIcon = new A2DSprite; //需要释放
		
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init() , CDlgCustomizeEye::InitResource()");

						delete pA2DIcon;
						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}
					
					//把A2DSprite*对象插入Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxEyeShape.Append(id, NULL, pszHintName, pA2DIcon, true);
				}//if 眼型
				//**眉型**//
				else if (uFaceShapePartID == 2)
				{	
					//数据id
					unsigned int id = pFaceShapeEssence->id ;
					
					//数据名称
					namechar* pszName = pFaceShapeEssence->name ;
					
					//形状数据文件
					char* pszShapeFile = pFaceShapeEssence->file_shape;
					
					//图标文件
					char* pszIconFile = pFaceShapeEssence->file_icon;
					
					//
					ACHAR* pszHintName = pFaceShapeEssence->name;
					
					
					if( *pszIconFile)
					{
						pszIconFile += 9;//跳过"surfaces\"	
					}
					else
					{
						a_LogOutput(1, "Eyebrow's shape icon file name is NULL! CDlgCustomizeEye::InitResource()");
						assert(0 && "Eyebrow's shape icon file name is NULL! CDlgCustomizeEye::InitResource()");
						return false;
					}
					
					//变图标文件为A2DSprite文件
					A2DSprite* pA2DIcon = new A2DSprite; //需要释放
					
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeEye::InitResource()");
						
						delete pA2DIcon;
#ifdef _DEBUG
						_asm int 3
#endif
							return false;
					}
					
					//把A2DSprite*对象插入Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					
					m_LbxBrowShape.Append(id, NULL, pszHintName, pA2DIcon, true);
					
				}// if 眉型

			}//if 当前人物

		}//if 形状数据

		//纹理数据
		else if( dtFaceData == DT_FACE_TEXTURE_ESSENCE)
		{
			FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData); 
			if( !pFaceTextureEssence)
				continue;

			unsigned int uFacePillOnly = pFaceTextureEssence->facepill_only;

			if( !GetCustomizeMan()->m_bLoadFacePillData)
			{
				if( uFacePillOnly != 0)
					continue;
			}
			
			//当前人物id和性别id
			int uCurCharacterComboID = pFaceTextureEssence->character_combo_id ;
			int uCurGenderID = pFaceTextureEssence->gender_id ;

			

			//设置当前人物的脸部纹理
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{
						
				//脸型纹理部分id
				unsigned int uCurTexPartID = pFaceTextureEssence->tex_part_id;
				
				//数据类型
				unsigned int uUserDataID = pFaceTextureEssence->user_data ;

				//**眼皮纹理**//
				if( uCurTexPartID == 1)
				{
					unsigned int id = pFaceTextureEssence->id;
					//图标文件
					char* pszIconFile = pFaceTextureEssence->file_icon;

					//纹理的名字
					ACHAR* pszHintName = pFaceTextureEssence->name;

					//用于插入listbox 字符串
					ACHAR *pszName = _AL("");

					A2DSprite* pA2DIcon;

					if(*pszIconFile)
					{
						pszIconFile += 9;//跳过"surfaces\"			
					}
					else
					{
						a_LogOutput(1, "Eyelid's texture Icon file name is NULL, CDlgCustomizeEye::InitResource()");
						assert(0 && "Eyelid's texture Icon file name is NULL, CDlgCustomizeEye::InitResource()");

						return false;
					}

					//变图标文件为A2DSprite文件
					pA2DIcon = new A2DSprite; //需要释放
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						delete pA2DIcon;

						a_LogOutput(1, "Failed to pA2DIcon->Init(), CDlgCustomizeEye::InitResource()");

						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}

					//把A2DSprite*对象插入Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxEyeBaseTex.Append(id, NULL, pszHintName, pA2DIcon, true);
					
				}//if 眼皮纹理
				//**眉毛纹理**//
				else if( uCurTexPartID == 3)
				{
					unsigned int id = pFaceTextureEssence->id;
					//图标文件
					char* pszIconFile = pFaceTextureEssence->file_icon;
					
					//纹理的名字
					ACHAR* pszHintName = pFaceTextureEssence->name;
					
					//用于插入listbox 字符串
					ACHAR *pszName = _AL("");
					
					A2DSprite* pA2DIcon;
					
					if(*pszIconFile)
					{			
						pszIconFile += 9;//跳过"surfaces\"			
					}
					else
					{
						a_LogOutput(1, "Eyebrow's texture Icon file name is NULL!  CDlgCustomizeEye::InitResource()");
						
						assert(0 && "Eyebrow's texture Icon file name is NULL!  CDlgCustomizeEye::InitResource()");
						return false;
					}
					
					//变图标文件为A2DSprite文件
					pA2DIcon = new A2DSprite; //需要释放
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						delete pA2DIcon;
						
						a_LogOutput( 1, "Failed to  pA2DIcon->Init() ! CDlgCustomizeEye::InitResource()");
						
#ifdef _DEBUG
						_asm int 3
#endif
							
							return false;
					}
					
					//把A2DSprite*对象插入Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					
					m_LbxBrowTex.Append(id, NULL, pszHintName, pA2DIcon, true);
				}//if 眉毛纹理

			}//if 当前人物

		}//if 纹理数据

	}//遍历整个数据库

	return true;
				
}

//-----------------------------------------------------------
void CDlgCustomizeEye::DoDataExchange(bool bSave)
{	
	if(!IsContextReady())
		return;
}

//---------------------------------------------------------------
//计算
void CDlgCustomizeEye::DoCalculate()
{
	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM ();//恢复初始单位骨骼矩阵
	
	GetFace()->CalculateEye();
	GetFace()->CalculateBrow();
	GetFace()->Calculate3Parts();
	
	//合并backup骨骼矩阵
	pBoneCtrler->ApplyBonesOnBakFace(true);
}

//-------------------------------------------------------------
void CDlgCustomizeEye::OnLButtonUpListbox(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!IsContextReady()){
		return;
	}
	// Try update eye texture
	{
		PAUIOBJECT pImage = m_ListBoxEyeBaseTex.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idEyeBaseTex = (int)pImage->GetData();
			GetFace()->UpdateEyeTexture();
			return;
		}
				
	}
	
	// Try update eye shape
	{
		PAUIOBJECT pImage = m_ListBoxEyeShape.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idEyeShape = (int)pImage->GetData();
			Normal2Data();
			UpdateData(false);
			DoCalculate();
			return;
		}
	}
	{
		PAUIOBJECT pImage = m_LbxBrowTex.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idBrowTex = (int)pImage->GetData();
			UpdateData(true);
			GetFace()->UpdateBrowTexture();
			return;
		}
	}
	{		
		PAUIOBJECT pImage = m_LbxBrowShape.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage){			
			GetFaceData()->idBrowShape = (int)pImage->GetData();
			
			UpdateData(true);
			
			GetFaceData()->scaleBrowH = 128;
			GetFaceData()->scaleBrowV = 128;
			GetFaceData()->rotateBrow = 128;
			GetFaceData()->offsetBrowH = 128;
			GetFaceData()->offsetBrowV = 128;
			GetFaceData()->offsetBrowZ = 128;
			GetFaceData()->scaleBrowH2 = 128;
			GetFaceData()->scaleBrowV2 = 128;
			GetFaceData()->rotateBrow2 = 128;
			GetFaceData()->offsetBrowH2 = 128;
			GetFaceData()->offsetBrowV2 = 128;
			GetFaceData()->offsetBrowZ2 = 128;
			
			UpdateData(false);
			
			DoCalculate();
			return;
		}
	}
}

void CDlgCustomizeEye::OnMouseWheelListbox(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_ListBoxEyeBaseTex.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_ListBoxEyeShape.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_LbxBrowTex.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_LbxBrowShape.OnMouse_Wheel(wParam, lParam, pObj))
		return;
}

void CDlgCustomizeEye::OnTick()
{
	m_ListBoxEyeShape.OnTick();
	m_ListBoxEyeBaseTex.OnTick();
	m_LbxThirdEye.OnTick();	
	m_LbxBrowTex.OnTick();
	m_LbxBrowShape.OnTick();
}

//-------------------------------------------------------------
void CDlgCustomizeEye::Normal2Data()
{
	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();
	
	 pFaceData->scaleEyeH= 128;
	 pFaceData->scaleEyeV = 128;
	 pFaceData->rotateEye = 128;
	 pFaceData->offsetEyeH = 128;
	 pFaceData->offsetEyeV = 128;
	 pFaceData->offsetEyeZ = 128;
	 pFaceData->scaleEyeBall = 128;

	 pFaceData->scaleEyeH2 = 128;
	 pFaceData->scaleEyeV2 = 128;
	 pFaceData->rotateEye2 = 128;
	 pFaceData->offsetEyeH2 = 128;
	 pFaceData->offsetEyeV2 = 128;
	 pFaceData->offsetEyeZ2 = 128;
	 pFaceData->scaleEyeBall2 = 128;
}

//---------------------------------------------------------------
void CDlgCustomizeEye::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();

	SetCanMove(false);

	//------------------------------------
	//预先计算除了眼睛和3庭以外的数据
	//------------------------------------

	//初始化骨骼
	CFaceBoneController* pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM();

	GetFace()->CalculateNose();//计算鼻型
	GetFace()->CalculateMouth();//计算嘴型
	GetFace()->CalculateEar();//计算耳型
	GetCustomizeMan()->CalculateAdvaceOption();//计算高级选项

	GetFace()->CalculateBlendFace ();//计算混合脸型
	GetFace()->CalculateFace();//计算脸盘
	
	pBoneCtrler->BackupBoneTM ();//备份之前的数据
	
	GetFace()->CalculateEye();//计算眼型
	GetFace()->CalculateBrow();//计算眉型
	GetFace()->Calculate3Parts();//计算3庭
	
	int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();

	if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
	{
		((CECLoginUIMan*)m_pAUIManager)->ChangeScene((CECLoginUIMan::LOGIN_SCENE)(CECLoginUIMan::LOGIN_SCENE_FACECUSTOMIZE_BEGIN +  GetPlayer()->GetProfession()));
	}
	else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
	{
		GetCustomizeMan()->ChangeScene(FACE_SCENE_HEAD);
	}
	
	//	默认显示第三只眼界面
	ShowThirdEye( GetPlayer()->GetProfession() == PROF_JIANLING || 
				  GetPlayer()->GetProfession() == PROF_MEILING );

	UpdateData(false);
}

void CDlgCustomizeEye::ClearListCtrlAndA2DSprite()
{
	m_ListBoxEyeBaseTex.ResetContent();
	m_ListBoxEyeShape.ResetContent();	
	m_LbxBrowTex.ResetContent();
	m_LbxBrowShape.ResetContent();	
	m_LbxThirdEye.ResetImages(true);
	m_LbxThirdEye.ResetContent();

	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}


void CDlgCustomizeEye::OnLButtonUpListBoxThirdEye(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!IsContextReady()){
		return;
	}
	{
		PAUIOBJECT pImage = m_LbxThirdEye.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idThirdEye = (int)pImage->GetData();
			GetFace()->UpdateThirdEye();
			return;
		}
	}
}

void CDlgCustomizeEye::OnMouseWheelListboxThirdEye(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_LbxThirdEye.OnMouse_Wheel(wParam, lParam, pObj))
		return;
}

bool CDlgCustomizeEye::InitThirdEye()
{	
	// must release the image stored in this grid first
	m_LbxThirdEye.ResetImages(true);

	// initialize image grid if no switch button
	A2DSprite *pSpriteSelect = GetCustomizeMan()->GetImageSelected();
	A2DSprite *pSpriteMouseOn = GetCustomizeMan()->GetImageMouseOn();
	if (!m_LbxThirdEye.Init(this, "ImgThirdEye_01", 1, 3, "ScrollThirdEye_01", "EditThirdEye_01", pSpriteSelect, pSpriteMouseOn) )
		return false;	

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	//数据空间ID
	ID_SPACE eIDSpace = ID_SPACE_FACE;
	
	//获取脸型相关数据数量
	int nNumFaceData;
	nNumFaceData = pElementDataMan->get_data_num(eIDSpace);
	
	//脸型相关的数据类型
	DATA_TYPE dtFaceData;
	
	int nCharacterMask = 1 << GetPlayer()->GetProfession();
	
	//遍历数据库
	for( int i = 0; i < nNumFaceData; i++)
	{
		//获取脸型数据id
		unsigned int uFaceDataID = pElementDataMan->get_data_id(eIDSpace, i, dtFaceData);
		
		if( dtFaceData == DT_FACE_THIRDEYE_ESSENCE)
		{
			FACE_THIRDEYE_ESSENCE* pFaceThirdEye = (FACE_THIRDEYE_ESSENCE*)pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData);
			
			if(!pFaceThirdEye)
				continue;
			
			unsigned int uFacePillOnly = pFaceThirdEye->facepill_only;
			
			if( !GetCustomizeMan()->m_bLoadFacePillData)
			{
				if( uFacePillOnly != 0)
					continue;
			}
			
			//当前人物id和性别id
			int uCurCharacterComboID = pFaceThirdEye->character_combo_id ;
			int uCurGenderID = pFaceThirdEye->gender_id ;
			
			//当前人物的脸部形状
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{
				ACHAR* pszHintName = pFaceThirdEye->name;
				unsigned int id = pFaceThirdEye->id;
				const char *pszIconFile = pFaceThirdEye->file_icon;
				if( *pszIconFile)
				{
					pszIconFile += 9;//跳过"surfaces\"	
				}
				else
				{
					a_LogOutput(1, "ThirdEye's shape icon file name is NULL! CDlgCustomizeThirdEye::InitResource()");
					assert(0 && "ThirdEye's icon file name is NULL! CDlgCustomizeThirdEye::InitResource()");
					return false;
				}
				
				//变图标文件为A2DSprite文件
				A2DSprite* pA2DIcon = new A2DSprite; //在 m_LbxThirdEye 中释放
				
				if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
				{					
					a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeThirdEye::InitResource()");
					
					delete pA2DIcon;
#ifdef _DEBUG
					_asm int 3
#endif
						return false;
				}
				
				m_LbxThirdEye.Append(id, NULL, pszHintName, pA2DIcon, true);
			}
		}
	}

	return true;
}

void CDlgCustomizeEye::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		if(IsContextReady())
		{
			InitThirdEye();
		}
	}
}

void CDlgCustomizeEye::ShowThirdEye(bool bShow)
{	
	PAUIOBJECT pLab = GetDlgItem("Lab_ThirdEye");
	if(pLab) pLab->Show(bShow);
	
	m_LbxThirdEye.Show(bShow);
}

//*********************************
//        CDlgCustomizeEye2
//*********************************
AUI_BEGIN_COMMAND_MAP(CDlgCustomizeEye2, CDlgCustomizeBase)

AUI_ON_COMMAND("OnBtnLeftEye", OnCommandLeftEye)
AUI_ON_COMMAND("OnBtnRightEye", OnCommandRightEye)
AUI_ON_COMMAND("MoveSlider_E", OnCommandEyeMoveSlider)
AUI_ON_COMMAND("EyeCheck", OnCommandEyeSync)

AUI_ON_COMMAND("MoveSlider_B", OnCommandBrowMoveSlider)
AUI_ON_COMMAND("OnBtnLeftBrow", OnCommandBrowSwitchToLeft)
AUI_ON_COMMAND("OnBtnRightBrow", OnCommandBrowSwitchToRight)
AUI_ON_COMMAND("BrowCheck", OnCommandBrowSync)

AUI_ON_COMMAND("Reset", OnCommandReset)

AUI_END_COMMAND_MAP()

CDlgCustomizeEye2::CDlgCustomizeEye2()
{	
	m_pSliderScaleEyeH = NULL;
	m_pSliderScaleEyeV = NULL;
	m_pSliderRotateEye = NULL;
	m_pSliderOffsetEyeH = NULL;
	m_pSliderOffsetEyeV = NULL;
	m_pSliderOffsetEyeZ = NULL;
	m_pSliderScaleEyeBall = NULL;
	
	m_pEyeCheckBoxSync = NULL;
	
	m_pEyeBtnLeft = NULL;
	m_pEyeBtnRight = NULL;
	
	int i(0);
	for( i = 0; i < ARRAY_SIZE(m_pLabelEye); i++)
	{
		m_pLabelEye[i] = NULL;
	}	
	
	m_bEyeSync = true;
	m_nEyeLeftOrRight = 1; // 0 : left, 1 : right

	
	m_pSliderScaleBrowH = NULL;
	m_pSliderScaleBrowV = NULL;
	m_pSliderRotateBrow = NULL;
	m_pSliderOffsetBrowH = NULL;
	m_pSliderOffsetBrowV = NULL;
	m_pSliderOffsetBrowZ = NULL;
	for (i = 0; i < ARRAY_SIZE(m_pLabelBrow); i++)
		m_pLabelBrow[i] = NULL;
	
	m_pBrowCheckBoxSync = NULL;
	m_pBrowBtnLeft = NULL;
	m_pBrowBtnRight = NULL;
	
	m_bBrowSync = true;
	m_nBrowLeftOrRight = 1;
}

AUIStillImageButton * CDlgCustomizeEye2::GetSwithButton()
{
	return NULL;
}

bool CDlgCustomizeEye2::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;
	
	if (!InitResource())
		return false;

	//设置滚动条范围
	SetSliderBound();
	
	if( !GetCustomizeMan()->m_bUnsymmetrical)
	{
		m_pEyeCheckBoxSync->Check(true);
		m_bEyeSync = true;
		m_pEyeCheckBoxSync->Enable(false);
		
		m_pEyeBtnLeft->Enable(false);
		m_pEyeBtnRight->Enable(false);

		m_pBrowCheckBoxSync->Check(true);
		m_pBrowCheckBoxSync->Enable(false);
		
		m_pBrowBtnLeft->Enable(false);
		m_pBrowBtnRight->Enable(false);
	}
	else
	{
		m_pEyeCheckBoxSync->Enable(true);
		m_pEyeBtnLeft->Enable(true);
		m_pEyeBtnRight->Enable(true);
		
		m_pBrowCheckBoxSync->Enable(true);
		
		m_pBrowBtnLeft->Enable(true);
		m_pBrowBtnRight->Enable(true);
	}
	
	LoadDefault();
	
	OnCommandEyeLeftRightHelper(m_nEyeLeftOrRight);
	OnCommandBrowSwithLeftRightHelper(m_nBrowLeftOrRight);
	
	return true;
}

void CDlgCustomizeEye2::SetSliderBound()
{
	CECCustomizeBound * pSliderBound = GetCustomizeMan()->GetSliderBound();
	m_pSliderScaleEyeH->SetTotal(pSliderBound->m_nScaleEyeHMax, pSliderBound->m_nScaleEyeHMin);
	m_pSliderScaleEyeV->SetTotal(pSliderBound->m_nScaleEyeVMax, pSliderBound->m_nScaleEyeVMin);
	m_pSliderRotateEye->SetTotal(pSliderBound->m_nRotateEyeMax, pSliderBound->m_nRotateEyeMin);
	m_pSliderOffsetEyeH->SetTotal(pSliderBound->m_nOffsetEyeHMax, pSliderBound->m_nOffsetEyeHMin);
	m_pSliderOffsetEyeV->SetTotal(pSliderBound->m_nOffsetEyeVMax, pSliderBound->m_nOffsetEyeVMin);
	m_pSliderOffsetEyeZ->SetTotal(pSliderBound->m_nOffsetEyeZMax, pSliderBound->m_nOffsetEyeZMin);
	m_pSliderScaleEyeBall->SetTotal(pSliderBound->m_nScaleEyeBallMax, pSliderBound->m_nScaleEyeBallMin);	
	
	m_pSliderScaleBrowH->SetTotal(pSliderBound->m_nScaleBrowHMax, pSliderBound->m_nScaleBrowHMin);
	m_pSliderScaleBrowV->SetTotal(pSliderBound->m_nScaleBrowVMax, pSliderBound->m_nScaleBrowVMin);
	m_pSliderRotateBrow->SetTotal(pSliderBound->m_nRotateBrowMax, pSliderBound->m_nRotateBrowMin);
	m_pSliderOffsetBrowH->SetTotal(pSliderBound->m_nOffsetBrowHMax, pSliderBound->m_nOffsetBrowHMin);
	m_pSliderOffsetBrowV->SetTotal(pSliderBound->m_nOffsetBrowVMax, pSliderBound->m_nOffsetBrowVMin);
	m_pSliderOffsetBrowZ->SetTotal(pSliderBound->m_nOffsetBrowZMax, pSliderBound->m_nOffsetBrowZMin);
}

void CDlgCustomizeEye2::LoadEyeDefault()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;
	
	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();
	
	pFaceData->scaleEyeH= pMan->m_nDefaultScaleEyeH;
	pFaceData->scaleEyeV = pMan->m_nDefaultScaleEyeV;
	pFaceData->rotateEye = pMan->m_nDefaultRotateEye;
	pFaceData->offsetEyeH = pMan->m_nDefaultOffsetEyeH;
	pFaceData->offsetEyeV = pMan->m_nDefaultOffsetEyeV;
	pFaceData->offsetEyeZ = pMan->m_nDefaultOffsetEyeZ;
	pFaceData->scaleEyeBall = pMan->m_nDefaultScaleEyeBall;
	
	pFaceData->scaleEyeH2 = pMan->m_nDefaultScaleEyeH2;
	pFaceData->scaleEyeV2 = pMan->m_nDefaultScaleEyeV2;
	pFaceData->rotateEye2 = pMan->m_nDefaultRotateEye2;
	pFaceData->offsetEyeH2 = pMan->m_nDefaultOffsetEyeH2;
	pFaceData->offsetEyeV2 = pMan->m_nDefaultOffsetEyeV2;
	pFaceData->offsetEyeZ2 = pMan->m_nDefaultOffsetEyeZ2;
	pFaceData->scaleEyeBall2 = pMan->m_nDefaultScaleEyeBall2;
	
	UpdateData(false);
}

void CDlgCustomizeEye2::LoadBrowDefault()
{
	GetFaceData()->scaleBrowH = GetCustomizeMan()->m_nDefaultScaleBrowH;
	GetFaceData()->scaleBrowV = GetCustomizeMan()->m_nDefaultScaleBrowV;
	GetFaceData()->rotateBrow = GetCustomizeMan()->m_nDefaultRotateBrow;
	GetFaceData()->offsetBrowH = GetCustomizeMan()->m_nDefaultOffsetBrowH;
	GetFaceData()->offsetBrowV = GetCustomizeMan()->m_nDefaultOffsetBrowV;
	GetFaceData()->offsetBrowZ = GetCustomizeMan()->m_nDefaultOffsetBrowZ;
	
	GetFaceData()->scaleBrowH2 = GetCustomizeMan()->m_nDefaultScaleBrowH2;
	GetFaceData()->scaleBrowV2 = GetCustomizeMan()->m_nDefaultScaleBrowV2;
	GetFaceData()->rotateBrow2 = GetCustomizeMan()->m_nDefaultRotateBrow2;
	GetFaceData()->offsetBrowH2 = GetCustomizeMan()->m_nDefaultOffsetBrowH2;
	GetFaceData()->offsetBrowV2 = GetCustomizeMan()->m_nDefaultOffsetBrowV2;
	GetFaceData()->offsetBrowZ2 = GetCustomizeMan()->m_nDefaultOffsetBrowZ2;
	
	UpdateData(false);
}

void CDlgCustomizeEye2::DoDataExchange(bool bSave)
{
	//ddx control	
	
	DDX_Control("Slider_ScaleEyeH", m_pSliderScaleEyeH);
	DDX_Control("Slider_ScaleEyeV", m_pSliderScaleEyeV);
	DDX_Control("Slider_RotateEye", m_pSliderRotateEye);
	DDX_Control("Slider_OffsetEyeH", m_pSliderOffsetEyeH);
	DDX_Control("Slider_OffsetEyeV", m_pSliderOffsetEyeV);
	DDX_Control("Slider_OffsetEyeZ", m_pSliderOffsetEyeZ);
	DDX_Control("Slider_ScaleEyeBall", m_pSliderScaleEyeBall);
	
	for (int i = 0; i < 7; i++)
	{
		AString strControlName;
		strControlName.Format("Label_Eye%d", i + 1);
		DDX_Control(strControlName, m_pLabelEye[i]);
	}
	
	DDX_Control("CheckBox_EyeAdjust", m_pEyeCheckBoxSync);
	DDX_Control("Btn_LeftEye", m_pEyeBtnLeft);
	DDX_Control("Btn_RightEye", m_pEyeBtnRight);
	
	DDX_Control("Slider_ScaleBrowH", m_pSliderScaleBrowH);
	DDX_Control("Slider_ScaleBrowV", m_pSliderScaleBrowV);
	DDX_Control("Slider_RotateBrow", m_pSliderRotateBrow);
	DDX_Control("Slider_OffsetBrowH", m_pSliderOffsetBrowH);
	DDX_Control("Slider_OffsetBrowV", m_pSliderOffsetBrowV);
	DDX_Control("Slider_OffsetBrowZ", m_pSliderOffsetBrowZ);
	{
		for (int i = 0; i < 6; i++)
		{
			AString strControlName;
			strControlName.Format("Label_Brow%d", i + 1);
			DDX_Control(strControlName, m_pLabelBrow[i]);
		}
	}
	DDX_Control("CheckBox_BrowAdjust", m_pBrowCheckBoxSync);
	DDX_Control("Btn_LeftBrow", m_pBrowBtnLeft);
	DDX_Control("Btn_RightBrow", m_pBrowBtnRight);
	
	if(!IsContextReady())
		return;
	
	//ddx data
	
	DDX_CheckBox(bSave, "CheckBox_EyeAdjust", m_bEyeSync);
	DDX_StillImageButtonGroup(bSave, m_nEyeLeftOrRight, 2, "Btn_LeftEye", "Btn_RightEye");
	
	
	if( m_nEyeLeftOrRight == 0) //left
	{
		if( m_bEyeSync)
		{
			DDX_Slider(bSave, "Slider_ScaleEyeH", GetFaceData()->scaleEyeH);
			DDX_Slider(bSave, "Slider_ScaleEyeV", GetFaceData()->scaleEyeV);
			DDX_Slider(bSave, "Slider_RotateEye", GetFaceData()->rotateEye);
			DDX_Slider(bSave, "Slider_OffsetEyeH", GetFaceData()->offsetEyeH);
			DDX_Slider(bSave, "Slider_OffsetEyeV", GetFaceData()->offsetEyeV);
			DDX_Slider(bSave, "Slider_OffsetEyeZ", GetFaceData()->offsetEyeZ);
			DDX_Slider(bSave, "Slider_ScaleEyeBall", GetFaceData()->scaleEyeBall);
		}
		
		DDX_Slider(bSave, "Slider_ScaleEyeH", GetFaceData()->scaleEyeH2);
		DDX_Slider(bSave, "Slider_ScaleEyeV", GetFaceData()->scaleEyeV2);
		DDX_Slider(bSave, "Slider_RotateEye", GetFaceData()->rotateEye2);
		DDX_Slider(bSave, "Slider_OffsetEyeH", GetFaceData()->offsetEyeH2);
		DDX_Slider(bSave, "Slider_OffsetEyeV", GetFaceData()->offsetEyeV2);
		DDX_Slider(bSave, "Slider_OffsetEyeZ", GetFaceData()->offsetEyeZ2);
		DDX_Slider(bSave, "Slider_ScaleEyeBall", GetFaceData()->scaleEyeBall2);
	}
	else //right
	{
		if (m_bEyeSync)
		{
			DDX_Slider(bSave, "Slider_ScaleEyeH", GetFaceData()->scaleEyeH2);
			DDX_Slider(bSave, "Slider_ScaleEyeV", GetFaceData()->scaleEyeV2);
			DDX_Slider(bSave, "Slider_RotateEye", GetFaceData()->rotateEye2);
			DDX_Slider(bSave, "Slider_OffsetEyeH", GetFaceData()->offsetEyeH2);
			DDX_Slider(bSave, "Slider_OffsetEyeV", GetFaceData()->offsetEyeV2);
			DDX_Slider(bSave, "Slider_OffsetEyeZ", GetFaceData()->offsetEyeZ2);
			DDX_Slider(bSave, "Slider_ScaleEyeBall", GetFaceData()->scaleEyeBall2);
			
		}
		
		DDX_Slider(bSave, "Slider_ScaleEyeH", GetFaceData()->scaleEyeH);
		DDX_Slider(bSave, "Slider_ScaleEyeV", GetFaceData()->scaleEyeV);
		DDX_Slider(bSave, "Slider_RotateEye", GetFaceData()->rotateEye);
		DDX_Slider(bSave, "Slider_OffsetEyeH", GetFaceData()->offsetEyeH);
		DDX_Slider(bSave, "Slider_OffsetEyeV", GetFaceData()->offsetEyeV);
		DDX_Slider(bSave, "Slider_OffsetEyeZ", GetFaceData()->offsetEyeZ);
		DDX_Slider(bSave, "Slider_ScaleEyeBall", GetFaceData()->scaleEyeBall);
	}
	
	DDX_CheckBox(bSave, "CheckBox_BrowAdjust", m_bBrowSync);
	if (bSave)
	{
		bool bSaveLeft = m_nBrowLeftOrRight == 0 || m_bBrowSync;
		bool bSaveRight = m_nBrowLeftOrRight == 1 || m_bBrowSync;
		if (bSaveLeft)
		{
			DDX_Slider(bSave, "Slider_ScaleBrowH", GetFaceData()->scaleBrowH2);
			DDX_Slider(bSave, "Slider_ScaleBrowV", GetFaceData()->scaleBrowV2);
			DDX_Slider(bSave, "Slider_RotateBrow", GetFaceData()->rotateBrow2);
			DDX_Slider(bSave, "Slider_OffsetBrowH", GetFaceData()->offsetBrowH2);
			DDX_Slider(bSave, "Slider_OffsetBrowV", GetFaceData()->offsetBrowV2);
			DDX_Slider(bSave, "Slider_OffsetBrowZ", GetFaceData()->offsetBrowZ2);
		}
		if (bSaveRight)
		{
			DDX_Slider(bSave, "Slider_ScaleBrowH", GetFaceData()->scaleBrowH);
			DDX_Slider(bSave, "Slider_ScaleBrowV", GetFaceData()->scaleBrowV);
			DDX_Slider(bSave, "Slider_RotateBrow", GetFaceData()->rotateBrow);
			DDX_Slider(bSave, "Slider_OffsetBrowH", GetFaceData()->offsetBrowH);
			DDX_Slider(bSave, "Slider_OffsetBrowV", GetFaceData()->offsetBrowV);
			DDX_Slider(bSave, "Slider_OffsetBrowZ", GetFaceData()->offsetBrowZ);
		}
	}
	else
	{
		if (m_nBrowLeftOrRight == 1)
		{
			DDX_Slider(bSave, "Slider_ScaleBrowH", GetFaceData()->scaleBrowH2);
			DDX_Slider(bSave, "Slider_ScaleBrowV", GetFaceData()->scaleBrowV2);
			DDX_Slider(bSave, "Slider_RotateBrow", GetFaceData()->rotateBrow2);
			DDX_Slider(bSave, "Slider_OffsetBrowH", GetFaceData()->offsetBrowH2);
			DDX_Slider(bSave, "Slider_OffsetBrowV", GetFaceData()->offsetBrowV2);
			DDX_Slider(bSave, "Slider_OffsetBrowZ", GetFaceData()->offsetBrowZ2);
		}
		else
		{
			DDX_Slider(bSave, "Slider_ScaleBrowH", GetFaceData()->scaleBrowH);
			DDX_Slider(bSave, "Slider_ScaleBrowV", GetFaceData()->scaleBrowV);
			DDX_Slider(bSave, "Slider_RotateBrow", GetFaceData()->rotateBrow);
			DDX_Slider(bSave, "Slider_OffsetBrowH", GetFaceData()->offsetBrowH);
			DDX_Slider(bSave, "Slider_OffsetBrowV", GetFaceData()->offsetBrowV);
			DDX_Slider(bSave, "Slider_OffsetBrowZ", GetFaceData()->offsetBrowZ);
		}
	}
}

void CDlgCustomizeEye2::DoCalculate()
{
	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM ();//恢复初始单位骨骼矩阵
	
	GetFace()->CalculateEye();
	GetFace()->CalculateBrow();
	GetFace()->Calculate3Parts();
	
	//合并backup骨骼矩阵
	pBoneCtrler->ApplyBonesOnBakFace(true);
}

//---------------------------------------------------------------
void CDlgCustomizeEye2::OnCommandLeftEye(const char* szCommand)
{
	OnCommandEyeLeftRightHelper(0);	
}

//---------------------------------------------------------------
void CDlgCustomizeEye2::OnCommandRightEye(const char* szCommand)
{
	OnCommandEyeLeftRightHelper(1);	
}

void CDlgCustomizeEye2::OnCommandEyeLeftRightHelper(int nCheckedIndex)
{
	UpdateData(true);
	
	CheckStillImageButton(nCheckedIndex, 2, "Btn_LeftEye", "Btn_RightEye");
	m_nEyeLeftOrRight = nCheckedIndex;
	
	UpdateData(false);
	
	
	int stringTableIndexBase = (m_nEyeLeftOrRight == 0) ? 2501 : 2401;
	for (int i = 0; i < 7; i++)
	{
		const ACHAR* pszName = 
			m_pAUIManager->GetStringFromTable(stringTableIndexBase + i);
		m_pLabelEye[i]->SetText(pszName);
	}
}

//----------------------------------------------------------------
void CDlgCustomizeEye2::OnCommandEyeMoveSlider(const char* szCommand)
{
	static DWORD nLastTime = 0;
	
	DWORD nThisTime = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	if(  nThisTime - nLastTime > 30 )
	{
		//更新滚动条控件
		UpdateData(true);
		DoCalculate();
		
		nLastTime = nThisTime;
	}
}

//---------------------------------------------------------------
void CDlgCustomizeEye2::OnCommandEyeSync(const char* szCommand)
{	
	UpdateData(true);
}

//---------------------------------------------------------------
void CDlgCustomizeEye2::OnCommandReset(const char* szCommand)
{
	LoadEyeDefault();
	LoadBrowDefault();
	DoCalculate();
}

void CDlgCustomizeEye2::LoadDefault()
{
	LoadEyeDefault();
	LoadBrowDefault();
}

void CDlgCustomizeEye2::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	
	SetCanMove(false);

	CFaceBoneController* pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM();
	
	GetFace()->CalculateNose();//计算鼻型
	GetFace()->CalculateMouth();//计算嘴型
	GetFace()->CalculateEar();//计算耳型
	GetCustomizeMan()->CalculateAdvaceOption();//计算高级选项
	
	GetFace()->CalculateBlendFace ();//计算混合脸型
	GetFace()->CalculateFace();//计算脸盘
	
	pBoneCtrler->BackupBoneTM ();//备份之前的数据
	
	GetFace()->CalculateEye();//计算眼型
	GetFace()->CalculateBrow();//计算眉型
	GetFace()->Calculate3Parts();//计算3庭

	int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();
	
	if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
	{
		((CECLoginUIMan*)m_pAUIManager)->ChangeScene((CECLoginUIMan::LOGIN_SCENE)(CECLoginUIMan::LOGIN_SCENE_FACECUSTOMIZE_BEGIN +  GetPlayer()->GetProfession()));
	}
	else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
	{
		GetCustomizeMan()->ChangeScene(FACE_SCENE_HEAD);
	}
	UpdateData(false);
}

bool CDlgCustomizeEye2::InitResource()
{
	return true;
}

void CDlgCustomizeEye2::OnCommandBrowMoveSlider(const char *szCommand)
{
	BEGIN_SLIDER_TIME_CHECKED;
	
	UpdateData(true);
	DoCalculate();
	
	END_SLIDER_TIME_CHECKED;
}

void CDlgCustomizeEye2::OnCommandBrowSwithLeftRightHelper(int nCheckedIndex)
{
	UpdateData(true);
	
	CheckStillImageButton(nCheckedIndex, 2, "Btn_LeftBrow", "Btn_RightBrow");
	m_nBrowLeftOrRight = nCheckedIndex;
	
	UpdateData(false);
	
	
	int stringTableIndexBase = (m_nBrowLeftOrRight == 0) ? 2601 : 2701;
	for (int i = 0; i < 6; i++)
	{
		const ACHAR* pszName = 
			m_pAUIManager->GetStringFromTable(stringTableIndexBase + i);
		m_pLabelBrow[i]->SetText(pszName);
	}
}


void CDlgCustomizeEye2::OnCommandBrowSwitchToLeft(const char *)
{
	OnCommandBrowSwithLeftRightHelper(0);
}

void CDlgCustomizeEye2::OnCommandBrowSwitchToRight(const char *)
{
	OnCommandBrowSwithLeftRightHelper(1);
}

void CDlgCustomizeEye2::OnCommandBrowSync(const char* szCommand)
{	
	UpdateData(true);
}
