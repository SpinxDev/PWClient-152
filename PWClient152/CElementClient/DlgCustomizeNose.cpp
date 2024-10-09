#include "DlgCustomizeNose.h"

#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_CustomizeMgr.h"
#include "AUIListBox.h"
#include "AUISlider.h"
#include "EC_CustomizeMgr.h"
#include "EC_CustomizeBound.h"
#include "FaceBoneController.h"
#include "EC_LoginUIMan.h"
#include "EC_UIManager.h"
#include "elementdataman.h"

#include "A3DViewport.h"
#include "WindowsX.h"


AUI_BEGIN_EVENT_MAP(CDlgCustomizeNoseMouth, CDlgCustomizeBase)

AUI_ON_EVENT("Img_*", WM_LBUTTONUP, OnLButtonUpListboxNose)
AUI_ON_EVENT("Img_*", WM_MOUSEWHEEL, OnMouseWheelListboxNose)
AUI_ON_EVENT("Scroll_*", WM_MOUSEWHEEL, OnMouseWheelListboxNose)
AUI_ON_EVENT("Edit_*", WM_MOUSEWHEEL, OnMouseWheelListboxNose)
AUI_ON_EVENT("Img_Mouth*", WM_LBUTTONUP, OnLButtonUpListboxNose)
AUI_ON_EVENT("Img_Mouth*", WM_MOUSEWHEEL, OnMouseWheelListboxNose)

AUI_END_EVENT_MAP()


AUI_BEGIN_COMMAND_MAP(CDlgCustomizeNoseMouth, CDlgCustomizeBase)
AUI_END_COMMAND_MAP()


CDlgCustomizeNoseMouth::CDlgCustomizeNoseMouth()
{
}

CDlgCustomizeNoseMouth::~CDlgCustomizeNoseMouth()
{
	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}

AUIStillImageButton * CDlgCustomizeNoseMouth::GetSwithButton()
{
	return GetCustomizeMan()->m_pBtnChooseNoseMouth;
}

void CDlgCustomizeNoseMouth::DoCalculate()
{
	//骨骼控制器
	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM ();//恢复初始单位骨骼矩阵
	
	GetFace()->CalculateNose();//计算鼻型
	GetFace()->CalculateMouth();//计算嘴型
	GetFace()->Calculate3Parts();//计算3庭
	
	//合并backup骨骼矩阵
	pBoneCtrler->ApplyBonesOnBakFace(true);

}

bool CDlgCustomizeNoseMouth::InitResource()
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

	unsigned int uNoseShapeCount1 = 0;
	unsigned int uNoseShapeCount2 = 0;
	unsigned int uNoseTexCount = 0;

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
			
			//当前人物的脸部形状
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{			
				//脸部各部分的id
				unsigned int uFaceShapePartID = pFaceShapeEssence->shape_part_id ;

				//**鼻头型**//
				if (uFaceShapePartID == 3)
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
						a_LogOutput(1, "NoseTip's shape icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						assert(0 && "NoseTip's shape icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");

						return false;
					}
		
					//变图标文件为A2DSprite文件
					A2DSprite* pA2DIcon = new A2DSprite; //需要释放
		
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeNoseMouth::InitResource()");	
						
						delete pA2DIcon;
						#ifdef _DEBUG
						_asm int 3
						#endif
						return false;
					}
					
					//把A2DSprite*对象插入Vector
 					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxNoseTip.Append(id, NULL, pszHintName, pA2DIcon, true);
					uNoseShapeCount1++;
					
				}// if 鼻头型

				//**鼻梁型**//
				else if (uFaceShapePartID == 4)
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
						a_LogOutput(1, "NoseBridge's shape icon file is NULL! CDlgCustomizeNoseMouth::InitResource()");

						assert(0 && "NoseBridge's shape icon file is NULL! CDlgCustomizeNoseMouth::InitResource()");

						return false;
					}
		
					//变图标文件为A2DSprite文件
					A2DSprite* pA2DIcon = new A2DSprite; //需要释放
		
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeNoseMouth::InitResource()");

						delete pA2DIcon;
						#ifdef _DEBUG
						_asm int 3
						#endif
						return false;
					}
					
					//把A2DSprite*对象插入Vector
 					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxNoseBridge.Append(id, NULL, pszHintName, pA2DIcon, true);
					uNoseShapeCount2++;
					
				}// if 鼻梁型
//**上唇线**//
				if (uFaceShapePartID == 5)
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
						a_LogOutput(1, "UpLipLine's shape icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						assert(0 && "UpLipLine's shape icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						return false;
					}
		
					//变图标文件为A2DSprite文件
					A2DSprite* pA2DIcon = new A2DSprite; //需要释放
		
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeNoseMouth::InitResource()");

						delete pA2DIcon;
						#ifdef _DEBUG
						_asm int 3
						#endif
						return false;
					}
					
					//把A2DSprite*对象插入Vector
 					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxUpLipLine.Append(id, NULL, pszHintName, pA2DIcon, true);
					
				
				}//if上唇线

				//**唇沟**//
				else if (uFaceShapePartID == 6)
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
						a_LogOutput(1, "MidLipLine's shape icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						assert( 0 && "MidLipLine's shape icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						return false;
					}
		
					//变图标文件为A2DSprite文件
					A2DSprite* pA2DIcon = new A2DSprite; //需要释放
		
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeNoseMouth::InitResource()");
						
						delete pA2DIcon;
						#ifdef _DEBUG
						_asm int 3
						#endif
						return false;
					}
					
					//把A2DSprite*对象插入Vector
 					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxMidLipLine.Append(id, NULL, pszHintName, pA2DIcon, true);
					
				
				}// if 唇沟

				//**下唇线**//
				else if (uFaceShapePartID == 7)
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
						a_LogOutput(1, "DownLipLine's shape icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						assert(0 && "DownLipLine's shape icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						
						return false;
					}
		
					//变图标文件为A2DSprite文件
					A2DSprite* pA2DIcon = new A2DSprite; //需要释放
		
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeNoseMouth::InitResource()");
						
						delete pA2DIcon;
						#ifdef _DEBUG
						_asm int 3
						#endif
						return false;
					}
					
					//把A2DSprite*对象插入Vector
 					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxDownLipLine.Append(id, NULL, pszHintName, pA2DIcon, true);					
				
				}//if 下唇线
			}//if 当前人物
		}//if 脸部形状数据

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

				//**鼻子纹理**//
				if( uCurTexPartID == 5)
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
						a_LogOutput(1, "Nose's texture icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						assert(0 && "Nose's texture icon file name is NULL! CDlgCustomizeNoseMouth::InitResource()");
						return false;
					}

					//变图标文件为A2DSprite文件
					pA2DIcon = new A2DSprite; //需要释放
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeNoseMouth::InitResource()");

						delete pA2DIcon;

						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}

					//把A2DSprite*对象插入Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxNoseTex.Append(id, NULL, pszHintName, pA2DIcon, true);
					uNoseTexCount++;
				}//if 鼻子纹理

			}//if 当前人物

		}//if 纹理数据

	}//遍历整个数据库
	
	return true;
}

bool CDlgCustomizeNoseMouth::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;

	ClearListCtrlAndA2DSprite();
	
	A2DSprite *pSpriteSelect = GetCustomizeMan()->GetImageSelected();
	A2DSprite *pSpriteMouseOn = GetCustomizeMan()->GetImageMouseOn();
	if (!m_ListBoxNoseTex.Init(this, "Img_01", 1, 3, "Scroll_01", "Edit_01", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxNoseTip.Init(this, "Img_04", 1, 3, "Scroll_02", "Edit_02", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxNoseBridge.Init(this, "Img_7", 1, 3, "Scroll_03", "Edit_03", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxUpLipLine.Init(this, "Img_Mouth01", 1, 3, "Scroll_04", "Edit_04", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxMidLipLine.Init(this, "Img_Mouth04", 1, 3, "Scroll_05", "Edit_05", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxDownLipLine.Init(this, "Img_Mouth07", 1, 3, "Scroll_06", "Edit_06", pSpriteSelect, pSpriteMouseOn))
		return false;
	
	if (!InitResource())
		return false;

	return true;
}

void CDlgCustomizeNoseMouth::DoDataExchange(bool bSave)
{
	CDlgCustomizeBase::DoDataExchange(bSave);

	// ddx data
	if (!IsContextReady()) return;
}

void CDlgCustomizeNoseMouth::OnLButtonUpListboxNose(WPARAM wParam, LPARAM lParam, AUIObject *pObj)	
{
	if (!IsContextReady()){
		return ;
	}
	{
		PAUIOBJECT pImage = m_ListBoxNoseTex.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idNoseTex = (int)pImage->GetData();
			UpdateData(true);			
			GetFace()->UpdateNoseTexture();
			return;
		}
	}
	{
		PAUIOBJECT pImage = m_ListBoxNoseTip.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idNoseTipShape = (int)pImage->GetData();
			UpdateData(true);
			
			ResetNoseTip();
			
			UpdateData(false);
			
			DoCalculate();
			return;
		}
	}
	{
		PAUIOBJECT pImage = m_ListBoxNoseBridge.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idNoseBridgeShape = (int)pImage->GetData();
			UpdateData(true);
			
			ResetNoseBridge();
			
			UpdateData(false);
			
			DoCalculate();
			return;
		}
	}
	
	{
		PAUIOBJECT pImage = m_ListBoxUpLipLine.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idMouthUpLipLine = (int)pImage->GetData();
			
			UpdateData(true);
			
			ResetUpLip();
			UpdateData(false);
			
			DoCalculate();
			return;
		}
	}
	{
		PAUIOBJECT pImage = m_ListBoxMidLipLine.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idMouthMidLipLine = (int)pImage->GetData();
			UpdateData(true);
			
			ResetMidLip();
			UpdateData(false);
			
			DoCalculate();
			return;
		}
	}
	{
		PAUIOBJECT pImage = m_ListBoxDownLipLine.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idMouthDownLipLine = (int)pImage->GetData();
			UpdateData(true);
			
			ResetDownLip();
			UpdateData(false);
			
			DoCalculate();
			return;
		}
	}
}

//-----------------------------------------------------------------------------------
void CDlgCustomizeNoseMouth::OnMouseWheelListboxNose(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_ListBoxNoseTex.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_ListBoxNoseTip.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_ListBoxNoseBridge.OnMouse_Wheel(wParam, lParam, pObj))
		return;	
	if (m_ListBoxUpLipLine.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_ListBoxMidLipLine.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_ListBoxDownLipLine.OnMouse_Wheel(wParam, lParam, pObj))
		return;
}

void CDlgCustomizeNoseMouth::OnTick()
{
	m_ListBoxNoseTex.OnTick();
	m_ListBoxNoseTip.OnTick();
	m_ListBoxNoseBridge.OnTick();
	m_ListBoxUpLipLine.OnTick();
	m_ListBoxMidLipLine.OnTick();
	m_ListBoxDownLipLine.OnTick();
}


void CDlgCustomizeNoseMouth::ResetNoseTip()
{
	GetFaceData()->scaleNoseTipH = 128;
	GetFaceData()->scaleNoseTipV = 128;
	GetFaceData()->scaleNoseTipZ = 128;
	GetFaceData()->offsetNoseTipV = 128;


}

void CDlgCustomizeNoseMouth::ResetNoseBridge()
{
	GetFaceData()->scaleBridgeTipH = 128;
	GetFaceData()->offsetBridgeTipZ = 128;

}

void CDlgCustomizeNoseMouth::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	SetCanMove(false);

	//初始化骨骼
	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM();
	
	GetFace()->CalculateEye();//计算眼型
	GetFace()->CalculateBrow();//计算鼻型
	GetFace()->CalculateEar();//计算耳型
	GetCustomizeMan()->CalculateAdvaceOption();	
	GetFace()->CalculateBlendFace ();//计算混合脸型
	GetFace()->CalculateFace();//计算脸盘
	
	pBoneCtrler->BackupBoneTM ();//备份之前的数据
	
	GetFace()->CalculateNose();//计算鼻型
	GetFace()->CalculateMouth();//计算嘴型
	GetFace()->Calculate3Parts();//计算3庭
	
	int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();
	
	if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
	{
		((CECLoginUIMan*)m_pAUIManager)->ChangeScene(
			(CECLoginUIMan::LOGIN_SCENE)(CECLoginUIMan::LOGIN_SCENE_FACECUSTOMIZE_BEGIN + GetPlayer()->GetProfession()));
	}
	else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
	{
		GetCustomizeMan()->ChangeScene(FACE_SCENE_HEAD);
	}
	UpdateData(false);
}

void CDlgCustomizeNoseMouth::ClearListCtrlAndA2DSprite()
{
	m_ListBoxNoseTex.ResetContent();
	m_ListBoxNoseTip.ResetContent();
	m_ListBoxNoseBridge.ResetContent();
	m_ListBoxUpLipLine.ResetContent();
	m_ListBoxMidLipLine.ResetContent();
	m_ListBoxDownLipLine.ResetContent();

	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}


//------------------------------------------------------------------
//复位上嘴唇
void CDlgCustomizeNoseMouth::ResetUpLip(void)
{
	GetFaceData()->thickUpLip = 128;
	GetFaceData()->scaleMouthH = 128;
	GetFaceData()->scaleMouthH2 = 128;
	GetFaceData()->offsetMouthV = 128;
	GetFaceData()->offsetMouthZ = 128;
	
	GetFaceData()->offsetCornerOfMouthSpecial = 128;
	GetFaceData()->offsetCornerOfMouthSpecial2 = 128;
	
	
}

//------------------------------------------------------------------
//复位唇线
void CDlgCustomizeNoseMouth::ResetMidLip(void)
{
	
	GetFaceData()->scaleMouthH = 128;
	GetFaceData()->scaleMouthH2 = 128;
	GetFaceData()->offsetMouthV = 128;
	GetFaceData()->offsetMouthZ = 128;
	GetFaceData()->offsetCornerOfMouthSpecial = 128;
	GetFaceData()->offsetCornerOfMouthSpecial2 = 128;
	
}

//------------------------------------------------------------------
//复位下嘴唇
void CDlgCustomizeNoseMouth::ResetDownLip(void)
{
	GetFaceData()->thickDownLip = 128;
	GetFaceData()->scaleMouthH = 128;
	GetFaceData()->scaleMouthH2 = 128;
	GetFaceData()->offsetMouthV = 128;
	GetFaceData()->offsetMouthZ = 128;
	
	GetFaceData()->offsetCornerOfMouthSpecial = 128;
	GetFaceData()->offsetCornerOfMouthSpecial2 = 128;
}

//********************************************
//			CDlgCustomizeNoseMouth2
//********************************************


AUI_BEGIN_COMMAND_MAP(CDlgCustomizeNoseMouth2, CDlgCustomizeBase)

AUI_ON_COMMAND("Reset", OnCommandReset)
AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("Check", OnCommandCheckSync)
AUI_ON_COMMAND("MoveSlider_ScaleMouthH", OnCommandMoveScaleMouthHSlider)
AUI_ON_COMMAND("MoveSlider_ScaleMouthH2", OnCommandMoveScaleMouthH2Slider)
AUI_ON_COMMAND("MoveSlider_OffsetCornerOfMouthSpecial", OnCommandMoveCornerOfMouthSlider)
AUI_ON_COMMAND("MoveSlider_OffsetCornerOfMouthSpecial2", OnCommandMoveCornerOfMouth2Slider)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgCustomizeNoseMouth2, CDlgCustomizeBase)

AUI_END_EVENT_MAP()

//----------------------------------------------------------
CDlgCustomizeNoseMouth2::CDlgCustomizeNoseMouth2()
{
	//DDX data
	m_bMouthScaleHSync = true;
	m_bMouthOffsetCornerSync = true;

	//ddx control
	m_pSliderThickUpLip = NULL;
	m_pSliderThickDownLip = NULL;
	m_pSliderScaleMouthH = NULL;
	m_pSliderOffsetMouthV = NULL;
	m_pSliderOffsetMouthZ = NULL;
	m_pSliderOffsetCornerOfMouthSpecial = NULL;
	m_pSliderScaleMouthH2 = NULL;
	m_pSliderOffsetCornerOfMouthSpecial2  = NULL;

	m_pCheckBoxScaleMouthH = NULL;
	m_pCheckBoxOffsetCorner = NULL;

	m_pSliderScaleNoseTipH = NULL;
	m_pSliderScaleNoseTipV = NULL;
	m_pSliderScaleNoseTipZ = NULL;
	m_pSliderOffsetNoseTipV = NULL;
	m_pSliderScaleBridgeTipH = NULL;
	m_pSliderOffsetBridgeTipZ = NULL;	
}

AUIStillImageButton * CDlgCustomizeNoseMouth2::GetSwithButton()
{
	return NULL;
}

//----------------------------------------------------------
bool CDlgCustomizeNoseMouth2::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;
	
	if (!InitResource())
		return false;

	//设置滚动条范围
	SetSliderBound();

	if(!GetCustomizeMan()->m_bUnsymmetrical)
	{
		m_pCheckBoxScaleMouthH->Check(true);
		m_pCheckBoxOffsetCorner->Check(true);
		
		m_pCheckBoxScaleMouthH->Enable(false);
		m_pCheckBoxOffsetCorner->Enable(false);
		
	}
	else
	{
		m_pCheckBoxScaleMouthH->Enable(true);
		m_pCheckBoxOffsetCorner->Enable(true);
	}
	
	LoadDefault();

	return true;
}

void CDlgCustomizeNoseMouth2::LoadDefault()
{
	LoadMouthDefault();
	LoadNoseDefault();
}

//----------------------------------------------------------
void CDlgCustomizeNoseMouth2::DoDataExchange(bool bSave)
{
	//ddx control
	DDX_Control("Slider_ThickUpLip", m_pSliderThickUpLip);
	DDX_Control("Slider_ThickDownLip", m_pSliderThickDownLip);
	DDX_Control("Slider_ScaleMouthH", m_pSliderScaleMouthH);
	DDX_Control("Slider_OffsetMouthV", m_pSliderOffsetMouthV);
	DDX_Control("Slider_OffsetMouthZ", m_pSliderOffsetMouthZ);
	DDX_Control("Slider_OffsetCornerOfMouthSpecial", m_pSliderOffsetCornerOfMouthSpecial);
	DDX_Control("Slider_ScaleMouthH2", m_pSliderScaleMouthH2);
	DDX_Control("Slider_OffsetCornerOfMouthSpecial2", m_pSliderOffsetCornerOfMouthSpecial2);

	DDX_Control("CheckBox_ScaleMouthH", m_pCheckBoxScaleMouthH);
	DDX_Control("CheckBox_OffsetCornerOfMouthSpecial",m_pCheckBoxOffsetCorner);
	
	DDX_Control("Slider_ScaleNoseTipH", m_pSliderScaleNoseTipH);
	DDX_Control("Slider_ScaleNoseTipV", m_pSliderScaleNoseTipV);
	DDX_Control("Slider_ScaleNoseTipZ", m_pSliderScaleNoseTipZ);
	DDX_Control("Slider_OffsetNoseTipV", m_pSliderOffsetNoseTipV);
	DDX_Control("Slider_ScaleBridgeTipH", m_pSliderScaleBridgeTipH);
	DDX_Control("Slider_OffsetBridgeTipZ", m_pSliderOffsetBridgeTipZ);
	
	if(!IsContextReady())
		return;
	
	//ddx data

	DDX_CheckBox(bSave, "CheckBox_ScaleMouthH", m_bMouthScaleHSync);
	DDX_CheckBox(bSave, "CheckBox_OffsetCornerOfMouthSpecial", m_bMouthOffsetCornerSync);

	DDX_Slider(bSave, "Slider_ThickUpLip", GetFaceData()->thickUpLip);
	DDX_Slider(bSave, "Slider_ThickDownLip", GetFaceData()->thickDownLip);
	DDX_Slider(bSave, "Slider_ScaleMouthH", GetFaceData()->scaleMouthH);
	DDX_Slider(bSave, "Slider_OffsetMouthV", GetFaceData()->offsetMouthV);
	DDX_Slider(bSave, "Slider_OffsetMouthZ", GetFaceData()->offsetMouthZ);
	DDX_Slider(bSave, "Slider_OffsetCornerOfMouthSpecial", GetFaceData()->offsetCornerOfMouthSpecial);
	DDX_Slider(bSave, "Slider_ScaleMouthH2", GetFaceData()->scaleMouthH2);
	DDX_Slider(bSave, "Slider_OffsetCornerOfMouthSpecial2",  GetFaceData()->offsetCornerOfMouthSpecial2);
	
	DDX_Slider(bSave, "Slider_ScaleNoseTipH", GetFaceData()->scaleNoseTipH);
	DDX_Slider(bSave, "Slider_ScaleNoseTipV", GetFaceData()->scaleNoseTipV);
	DDX_Slider(bSave, "Slider_ScaleNoseTipZ", GetFaceData()->scaleNoseTipZ);
	DDX_Slider(bSave, "Slider_OffsetNoseTipV", GetFaceData()->offsetNoseTipV);
	DDX_Slider(bSave, "Slider_ScaleBridgeTipH", GetFaceData()->scaleBridgeTipH);
	DDX_Slider(bSave, "Slider_OffsetBridgeTipZ", GetFaceData()->offsetBridgeTipZ);		
}

//----------------------------------------------------------
void CDlgCustomizeNoseMouth2::DoCalculate()
{
	//骨骼控制器
	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM ();//恢复初始单位骨骼矩阵
	
	GetFace()->CalculateMouth();//计算嘴型
	GetFace()->CalculateNose();//计算鼻型
	GetFace()->Calculate3Parts();//计算3庭
	
	//合并backup骨骼矩阵
	pBoneCtrler->ApplyBonesOnBakFace(true);
}

//----------------------------------------------------------
bool CDlgCustomizeNoseMouth2::InitResource()
{
	return true;
}

//----------------------------------------------------------
void CDlgCustomizeNoseMouth2::SetSliderBound()
{
	CECCustomizeBound * pSliderBound = GetCustomizeMan()->GetSliderBound();
	m_pSliderThickUpLip->SetTotal(pSliderBound->m_nThickUpLipMax, pSliderBound->m_nThickUpLipMin);
	m_pSliderThickDownLip->SetTotal(pSliderBound->m_nThickDownLipMax, pSliderBound->m_nThickDownLipMin);
	m_pSliderScaleMouthH->SetTotal(pSliderBound->m_nScaleMouthHMax, pSliderBound->m_nScaleMouthHMin);
	m_pSliderOffsetMouthV->SetTotal(pSliderBound->m_nOffsetMouthVMax, pSliderBound->m_nOffsetMouthVMin);
	m_pSliderOffsetMouthZ->SetTotal(pSliderBound->m_nOffsetMouthZMax, pSliderBound->m_nOffsetMouthZMin);
	m_pSliderOffsetCornerOfMouthSpecial->SetTotal(pSliderBound->m_nOffsetCornerOfMouthSpecialMax, pSliderBound->m_nOffsetCornerOfMouthSpecialMin);
	m_pSliderScaleMouthH2->SetTotal(pSliderBound->m_nScaleMouthH2Max, pSliderBound->m_nScaleMouthH2Min);
	m_pSliderOffsetCornerOfMouthSpecial2->SetTotal(pSliderBound->m_nOffsetCornerOfMouthSpecial2Max, pSliderBound->m_nOffsetCornerOfMouthSpecial2Min);
	
	m_pSliderScaleNoseTipH->SetTotal(pSliderBound->m_nScaleNoseTipHMax, pSliderBound->m_nScaleNoseTipHMin);
	m_pSliderScaleNoseTipV->SetTotal(pSliderBound->m_nScaleNoseTipVMax, pSliderBound->m_nScaleNoseTipVMin);
	m_pSliderOffsetNoseTipV->SetTotal(pSliderBound->m_nOffsetNoseTipVMax, pSliderBound->m_nOffsetNoseTipVMin);
	m_pSliderScaleNoseTipZ->SetTotal(pSliderBound->m_nScaleNoseTipZMax, pSliderBound->m_nScaleNoseTipZMin);
	m_pSliderScaleBridgeTipH->SetTotal(pSliderBound->m_nScaleBridgeTipHMax, pSliderBound->m_nScaleBridgeTipHMin);
	m_pSliderOffsetBridgeTipZ->SetTotal(pSliderBound->m_nOffsetBridgeTipZMax, pSliderBound->m_nOffsetBridgeTipZMin);

}
//----------------------------------------------------------
void CDlgCustomizeNoseMouth2::LoadMouthDefault()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;

	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();
	
	pFaceData->thickUpLip = pMan->m_nDefaultThickUpLip;
	pFaceData->thickDownLip = pMan->m_nDefaultThickDownLip;
 	pFaceData->scaleMouthH = pMan->m_nDefaultScaleMouthH;
	pFaceData->scaleMouthH2 = pMan->m_nDefaultScaleMouthH2;
 	pFaceData->offsetMouthV = pMan->m_nDefaultOffsetMouthV;
	pFaceData->offsetMouthZ = pMan->m_nDefaultOffsetMouthZ;
	pFaceData->offsetCornerOfMouthSpecial = pMan->m_nDefaultOffsetCornerOfMouthSpecial;
	pFaceData->offsetCornerOfMouthSpecial2 = pMan->m_nDefaultOffsetCornerOfMouthSpecial2;

	UpdateData(false);
}

void CDlgCustomizeNoseMouth2::LoadNoseDefault()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;
	GetFaceData()->scaleNoseTipH = GetCustomizeMan()->m_nDefaultScaleNoseTipH;
	GetFaceData()->scaleNoseTipV = GetCustomizeMan()->m_nDefaultScaleNoseTipV;
	GetFaceData()->scaleNoseTipZ = GetCustomizeMan()->m_nDefaultScaleNoseTipZ;
	GetFaceData()->offsetNoseTipV = GetCustomizeMan()->m_nDefaultOffsetNoseTipV;
	GetFaceData()->scaleBridgeTipH = GetCustomizeMan()->m_nDefaultScaleBridgeTipH;
	GetFaceData()->offsetBridgeTipZ = GetCustomizeMan()->m_nDefaultOffsetBridegTipZ;
	
	UpdateData(false);
}

//----------------------------------------------------------
void CDlgCustomizeNoseMouth2::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	SetCanMove(false);

	//------------------------------------
	//预先计算除了嘴型和3庭以外的数据
	//------------------------------------

	//初始化骨骼
	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM();
	
	GetFace()->CalculateEye();//计算眼型
	GetFace()->CalculateBrow();//计算眉型
	GetFace()->CalculateEar();//计算耳型
	GetCustomizeMan()->CalculateAdvaceOption();//计算高级选项	
	GetFace()->CalculateBlendFace ();//计算混合脸型
	GetFace()->CalculateFace();//计算脸盘
	
	pBoneCtrler->BackupBoneTM ();//备份之前的数据
	
	GetFace()->CalculateMouth();//计算嘴型
	GetFace()->CalculateNose();//计算鼻型
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

//-----------------------------------------------------------------
void CDlgCustomizeNoseMouth2::OnCommandMoveSlider(const char *szCommand)
{
	BEGIN_SLIDER_TIME_CHECKED;

	UpdateData(true);

	DoCalculate();

	END_SLIDER_TIME_CHECKED;
}

//------------------------------------------------------------------
void CDlgCustomizeNoseMouth2::OnCommandCheckSync(const char *szCommand)
{
	UpdateData(true);
}

//---------------------------------------------------------------------------
void CDlgCustomizeNoseMouth2::OnCommandMoveScaleMouthHSlider(const char *szCommand)
{
	BEGIN_SLIDER_TIME_CHECKED;

	UpdateData(true);
	
	if( m_bMouthScaleHSync)
	{
		GetFaceData()->scaleMouthH2 = GetFaceData()->scaleMouthH;
		UpdateData(false);
	}

	DoCalculate();

	END_SLIDER_TIME_CHECKED;
}

//-----------------------------------------------------------------------------
void CDlgCustomizeNoseMouth2::OnCommandMoveScaleMouthH2Slider(const char *szCommand)
{
	BEGIN_SLIDER_TIME_CHECKED;

	UpdateData(true);
	
	if( m_bMouthScaleHSync)
	{
		GetFaceData()->scaleMouthH = GetFaceData()->scaleMouthH2;
		UpdateData(false);
	}

	DoCalculate();

	END_SLIDER_TIME_CHECKED;
}

//--------------------------------------------------------------------------------
void CDlgCustomizeNoseMouth2::OnCommandMoveCornerOfMouthSlider(const char *szCommand)
{
	BEGIN_SLIDER_TIME_CHECKED;

	UpdateData(true);
	
	if( m_bMouthOffsetCornerSync)
	{
		GetFaceData()->offsetCornerOfMouthSpecial2 = GetFaceData()->offsetCornerOfMouthSpecial;
		UpdateData(false);
	}

	DoCalculate();

	END_SLIDER_TIME_CHECKED;
}

//-----------------------------------------------------------------------------------
void CDlgCustomizeNoseMouth2::OnCommandMoveCornerOfMouth2Slider(const char *szCommand)
{	
	BEGIN_SLIDER_TIME_CHECKED;

	UpdateData(true);
	
	if( m_bMouthOffsetCornerSync)
	{
		GetFaceData()->offsetCornerOfMouthSpecial = GetFaceData()->offsetCornerOfMouthSpecial2;
		UpdateData(false);
	}

	DoCalculate();

	END_SLIDER_TIME_CHECKED;
}

void CDlgCustomizeNoseMouth2::OnCommandReset(const char *szCommand)
{
	LoadNoseDefault();
	LoadMouthDefault();
	DoCalculate();
}