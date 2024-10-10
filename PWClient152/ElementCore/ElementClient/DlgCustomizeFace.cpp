
#include "DlgCustomizeFace.h"
#include "EC_Game.h"
#include "EC_Face.h"
#include "EC_CustomizeMgr.h"
#include "AUIImagePicture.h"
#include "AUIListBox.h"
#include "FaceBoneController.h"
#include "EC_Game.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "EC_LoginUIMan.h"
#include "EC_CustomizeBound.h"
#include "A3DViewport.h"
#include "WindowsX.h"
#include "elementdataman.h"

AUI_BEGIN_COMMAND_MAP(CDlgCustomizeFace, CDlgCustomizeBase)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgCustomizeFace, CDlgCustomizeBase)

AUI_ON_EVENT("E_Face*", WM_LBUTTONUP, OnLButtonUpListbox)
AUI_ON_EVENT("E_Face*", WM_MOUSEWHEEL, OnMouseWheelListbox)
AUI_ON_EVENT("Img_*", WM_LBUTTONUP, OnLButtonUpListbox)
AUI_ON_EVENT("Img_*", WM_MOUSEWHEEL, OnMouseWheelListbox)
AUI_ON_EVENT("Scroll_*", WM_MOUSEWHEEL, OnMouseWheelListbox)
AUI_ON_EVENT("Edit_*", WM_MOUSEWHEEL, OnMouseWheelListbox)
AUI_ON_EVENT("E_Ear*", WM_LBUTTONUP, OnLButtonUpListbox)
AUI_ON_EVENT("E_Ear*", WM_MOUSEWHEEL, OnMouseWheelListbox)

AUI_END_EVENT_MAP()

CDlgCustomizeFace::CDlgCustomizeFace()
{
}

CDlgCustomizeFace::~CDlgCustomizeFace()
{
	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}

bool CDlgCustomizeFace::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;

	ClearListCtrlAndA2DSprite();
	
	A2DSprite *pSpriteSelect = GetCustomizeMan()->GetImageSelected();
	A2DSprite *pSpriteMouseOn = GetCustomizeMan()->GetImageMouseOn();
	if (!m_ListBoxFaceShape.Init(this, "E_Face01", 3, 3, "Scroll_02", "Edit_02", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxFalingTex.Init(this, "Img_01", 1, 3, "Scroll_01", "Edit_01", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxEar.Init(this, "E_Ear01", 3, 3, "Scroll_03", "Edit_03", pSpriteSelect, pSpriteMouseOn))
		return false;
	
	if (!InitResource())
		return false;

	return true;
}

void CDlgCustomizeFace::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();

	SetCanMove(false);
	
	//��ʼ������
	CFaceBoneController* pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM();	
	
	GetFace()->CalculateEye();//��������
	GetFace()->CalculateBrow();//����ü��
	GetFace()->CalculateNose();//�������
	GetFace()->CalculateMouth();//��������
	GetFace()->CalculateFace();
	GetCustomizeMan()->CalculateAdvaceOption();//����߼�ѡ��
	
	pBoneCtrler->BackupBoneTM ();//����֮ǰ������
	
	GetFace()->CalculateBlendFace();
	GetFace()->CalculateEar();//�������
	GetFace()->Calculate3Parts();

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

void CDlgCustomizeFace::OnLButtonUpListbox(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (!IsContextReady()){
		return;
	}

	PAUIOBJECT pImage = m_ListBoxFalingTex.OnMouse_Clicked(wParam, lParam, pObj);
	if (pImage){
		GetFaceData()->idFalingTex = (int)pImage->GetData();
		UpdateData(true);
		GetFace()->UpdateFalingTexture();
		return;
	}
	
	pImage = m_ListBoxFaceShape.OnMouse_Clicked(wParam, lParam, pObj);
	if (pImage){
		if( GetPlayer()->GetProfession() != PROF_ORC){//��������
			GetFaceData()->idFaceShape1 = (unsigned short)pImage->GetData();
			GetFaceData()->idFaceShape2 = (unsigned short)pImage->GetData();
			UpdateData(true);
			DoCalculate();
		}
		return;
	}
	pImage = m_ListBoxEar.OnMouse_Clicked(wParam, lParam, pObj);
	if (pImage)
	{
		// ѡ�ж��θ���
		GetFaceData()->idEarShape = (int)pImage->GetData();		
		GetFaceData()->offsetEarV = 128;
		GetFaceData()->scaleEar = 128;
		UpdateData(false);
		DoCalculate();
		return;
	}
}

//-------------------------------------------------------------------------------
void CDlgCustomizeFace::OnMouseWheelListbox(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (m_ListBoxFalingTex.OnMouse_Wheel(wParam, lParam, pObj)){
		return;
	}
	if (m_ListBoxFaceShape.OnMouse_Wheel(wParam, lParam, pObj)){
		return;
	}
	if (m_ListBoxEar.OnMouse_Wheel(wParam, lParam, pObj)){
		return;
	}
}

void CDlgCustomizeFace::OnTick()
{
	m_ListBoxFaceShape.OnTick();
	m_ListBoxFalingTex.OnTick();
	m_ListBoxEar.OnTick();
}

void CDlgCustomizeFace::DoCalculate()
{
	//	UpdateData(true);
	
	//����������
	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM ();//�ָ���ʼ��λ��������
	
	GetFace()->CalculateBlendFace();
	GetFace()->CalculateEar();//�������
	GetFace()->Calculate3Parts();
	
	//�ϲ�backup��������
	pBoneCtrler->ApplyBonesOnBakFace(true);
}

AUIStillImageButton * CDlgCustomizeFace::GetSwithButton()
{
	return GetCustomizeMan()->m_pBtnChooseFace;
}

bool CDlgCustomizeFace::InitResource()
{
	//��ȡԪ�����ݹ�����
	elementdataman* pElementDataMan;
	pElementDataMan= GetGame()->GetElementDataMan ();

	//���ݿռ�ID
	ID_SPACE eIDSpace = ID_SPACE_FACE;		

	//��ȡ���������������
	int nNumFaceData;
	nNumFaceData = pElementDataMan->get_data_num( eIDSpace);

	//������ص���������
	DATA_TYPE dtFaceData;
	
	CECFace::FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();

	int nCharacterMask = 1 << GetPlayer()->GetProfession();

	for( int i = 0; i < nNumFaceData; i++)
	{
	
		//��ȡ��������id
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

			//��ǰ����id���Ա�id
			int uCurCharacterComboID = pFaceShapeEssence->character_combo_id ;
			int uCurGenderID = pFaceShapeEssence->gender_id ;

			//��������
			unsigned int uUserDataID = pFaceShapeEssence->user_data ;

			//��ǰ�����������״
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{			
				//���������ֵ�id
				unsigned int uFaceShapePartID = pFaceShapeEssence->shape_part_id ;

				//**����**//
				if (uFaceShapePartID == 0)
				{	
					//����id
					unsigned int id = pFaceShapeEssence->id ;

					//��������
					namechar* pszName = pFaceShapeEssence->name ;

					//��״�����ļ�
					char* pszShapeFile = pFaceShapeEssence->file_shape;

					//ͼ���ļ�
					char* pszIconFile = pFaceShapeEssence->file_icon;

					//
					ACHAR* pszHintName = pFaceShapeEssence->name;
									
					if( *pszIconFile)
					{
						pszIconFile += 9;//����"surfaces\"	
					}
					else
					{
						a_LogOutput(1, "Face's shape icon file is NULL, CDlgCustomizeFace::InitResource()");
						assert(0 && "Face's shape icon file is NULL, CDlgCustomizeFace::InitResource()");
						return false;
					}
		
					//��ͼ���ļ�ΪA2DSprite�ļ�
					A2DSprite* pA2DIcon = new A2DSprite; //��Ҫ�ͷ�
		
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init(),  CDlgCustomizeFace::InitResource()");

						delete pA2DIcon;
						#ifdef _DEBUG
						_asm int 3
						#endif
						return false;
					}
					
					//��A2DSprite*�������Vector
 					m_vecA2DSprite.push_back(pA2DIcon);					
					m_ListBoxFaceShape.Append(id, NULL, pszHintName, pA2DIcon, true);

				}//if ����
				//**����**//
				else if (uFaceShapePartID == 8)
				{
					//����id
					unsigned int id = pFaceShapeEssence->id ;
					
					//��������
					namechar* pszName = pFaceShapeEssence->name ;
					
					//��״�����ļ�
					char* pszShapeFile = pFaceShapeEssence->file_shape;
					
					//ͼ���ļ�
					char* pszIconFile = pFaceShapeEssence->file_icon;
					
					//
					ACHAR* pszHintName = pFaceShapeEssence->name;
					
					
					if( *pszIconFile)
					{
						pszIconFile += 9;//����"surfaces\"	
					}
					else
					{
						a_LogOutput(1, "Ear's shape Icon file name is NULL! CDlgCustomizeFace::InitResource()");
						assert(0 && "Ear's shape Icon file name is NULL! CDlgCustomizeFace::InitResource()");
						
						return false;
					}
					
					//��ͼ���ļ�ΪA2DSprite�ļ�
					A2DSprite* pA2DIcon = new A2DSprite; //��Ҫ�ͷ�
					
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						
						a_LogOutput(1, "Failed to pA2DIcon->Init(), CDlgCustomizeFace::InitResource()");
						
						delete pA2DIcon;
#ifdef _DEBUG
						_asm int 3
#endif
							
							
							
							return false;
					}
					
					//��A2DSprite*�������Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					
					m_ListBoxEar.Append(id, NULL, pszHintName, pA2DIcon, true);
					
				}// if ����

			}//if��ǰ����
			
		}//if ������״

		else if( dtFaceData == DT_FACE_TEXTURE_ESSENCE)//��������
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

			//��ǰ����id���Ա�id
			int uCurCharacterComboID = pFaceTextureEssence->character_combo_id ;
			int uCurGenderID = pFaceTextureEssence->gender_id ;

			
			//���õ�ǰ�������������(���Բ��з���)
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID) && (GetPlayer()->GetGender() == 0))
			{
						
				//����������id
				unsigned int uCurTexPartID = pFaceTextureEssence->tex_part_id;
				
				//��������
				unsigned int uUserDataID = pFaceTextureEssence->user_data ;

				//**��������**//
				if( uCurTexPartID == 10)
				{
					unsigned int id = pFaceTextureEssence->id;
					//ͼ���ļ�
					char* pszIconFile = pFaceTextureEssence->file_icon;

					//���������
					ACHAR* pszHintName = pFaceTextureEssence->name;

					//���ڲ���listbox �ַ���
					ACHAR *pszName = _AL("");

					A2DSprite* pA2DIcon;
					
					if( !pszIconFile)
					{
						a_LogOutput(1, "Faling's texture icon file is NULL, CDlgCustomizeFace::InitResource()");
						assert(0 && "Faling's texture icon file is NULL, CDlgCustomizeFace::InitResource()");

						return false;
					}
					else
					{
					
						pszIconFile += 9;//����"surfaces\"	
						
						//��ͼ���ļ�ΪA2DSprite�ļ�
						pA2DIcon = new A2DSprite; //��Ҫ�ͷ�
						A3DDevice* pDevice = GetGame()->GetA3DDevice();
						if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
						{
							 a_LogOutput(1, "Failed to  pA2DIcon->Init(), CDlgCustomizeFace::InitResource()");
							delete pA2DIcon;

							#ifdef _DEBUG
							_asm int 3
							#endif

							return false;
						}

						//��A2DSprite*�������Vector
						m_vecA2DSprite.push_back(pA2DIcon);
					}

					m_ListBoxFalingTex.Append(id, NULL, pszHintName, pA2DIcon, true);
					
				}//if ��������

			}//if ��ǰ����
		
		}//��������

	}//�����������ݿ�

	return true;
}

void CDlgCustomizeFace::ClearListCtrlAndA2DSprite()
{
	m_ListBoxFaceShape.ResetContent();
	m_ListBoxFalingTex.ResetContent();
	m_ListBoxEar.ResetContent();

	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;

		(*iter) = NULL;
	}
	m_vecA2DSprite.clear();
}

//**********************************
//      CDlgCustomizeFace2
//**********************************


AUI_BEGIN_COMMAND_MAP(CDlgCustomizeFace2, CDlgCustomizeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("Reset", OnCommandReset)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgCustomizeFace2, CDlgCustomizeBase)

AUI_END_EVENT_MAP()

//--------------------------------------------------------------
CDlgCustomizeFace2::CDlgCustomizeFace2()
{
	m_pSliderOffsetForeheadH = NULL;
	m_pSliderOffsetForeheadV = NULL;
	m_pSliderOffsetForeheadZ = NULL;
	m_pSliderRotateForehead = NULL;
	m_pSliderScaleForehead = NULL;

	m_pSliderOffsetYokeBoneH = NULL;
	m_pSliderOffsetYokeBoneV = NULL;
	m_pSliderOffsetYokeBoneZ = NULL;
	m_pSliderRotateYokeBone = NULL;
	m_pSliderScaleYokeBone = NULL;
	
	m_pSliderOffsetCheekH = NULL;
	m_pSliderOffsetCheekV = NULL;
	m_pSliderOffsetCheekZ = NULL;
	m_pSliderScaleCheek = NULL;

	m_pSliderOffsetChainV = NULL;
	m_pSliderOffsetChainZ = NULL;
	m_pSliderRotateChain = NULL;
	m_pSliderScaleChainH = NULL;

	m_pSliderOffsetJawH = NULL;
	m_pSliderOffsetJawV = NULL;
	m_pSliderOffsetJawZ = NULL;
	m_pSliderScaleJawSpecial = NULL;
	m_pSliderScaleJawH = NULL;
	m_pSliderScaleJawV = NULL;

	m_pSliderScaleEar = NULL;
	m_pSliderOffsetEarV = NULL;

	m_pSliderPartUp = NULL;
	m_pSliderPartMiddle = NULL;
	m_pSliderPartDown = NULL;
	
	//���̱�������
	m_pSliderScaleFaceH = NULL;
	m_pSliderScaleFaceV = NULL;
	
}

//--------------------------------------------------------------
CDlgCustomizeFace2::~CDlgCustomizeFace2()
{

}

//----------------------------------------------------------
AUIStillImageButton * CDlgCustomizeFace2::GetSwithButton()
{
	return NULL;
}

//----------------------------------------------------------
bool CDlgCustomizeFace2::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
	return false;


	if (!InitResource())
		return false;

	//���ù�������Χ
	SetSliderBound();

	
	LoadDefault();

	

	return true;
}

//----------------------------------------------------------
void CDlgCustomizeFace2::DoDataExchange(bool bSave)
{
	//ddx control
	DDX_Control("Slider_OffsetForeheadH", m_pSliderOffsetForeheadH);	
	DDX_Control("Slider_OffsetForeheadV", m_pSliderOffsetForeheadV);	
	DDX_Control("Slider_OffsetForeheadZ", m_pSliderOffsetForeheadZ);	
	DDX_Control("Slider_RotateForehead", m_pSliderRotateForehead);		
	DDX_Control("Slider_ScaleForehead", m_pSliderScaleForehead);		

	DDX_Control("Slider_OffsetYokeBoneH", m_pSliderOffsetYokeBoneH);
	DDX_Control("Slider_OffsetYokeBoneV", m_pSliderOffsetYokeBoneV);	
	DDX_Control("Slider_OffsetYokeBoneZ", m_pSliderOffsetYokeBoneZ);	
	DDX_Control("Slider_RotateYokeBone", m_pSliderRotateYokeBone);		
	DDX_Control("Slider_ScaleYokeBone", m_pSliderScaleYokeBone);		
	
	DDX_Control("Slider_OffsetCheekH", m_pSliderOffsetCheekH);		
	DDX_Control("Slider_OffsetCheekV", m_pSliderOffsetCheekV);		
	DDX_Control("Slider_OffsetCheekZ", m_pSliderOffsetCheekZ);		
	DDX_Control("Slider_ScaleCheek", m_pSliderScaleCheek);			

	DDX_Control("Slider_OffsetChainV", m_pSliderOffsetChainV);		
	DDX_Control("Slider_OffsetChainZ", m_pSliderOffsetChainZ);		
	DDX_Control("Slider_RotateChain", m_pSliderRotateChain);		
	DDX_Control("Slider_ScaleChainH", m_pSliderScaleChainH);		

	DDX_Control("Slider_OffsetJawH", m_pSliderOffsetJawH);			
	DDX_Control("Slider_OffsetJawV", m_pSliderOffsetJawV);			
	DDX_Control("Slider_OffsetJawZ", m_pSliderOffsetJawZ);			
	DDX_Control("Slider_ScaleJawSpecial", m_pSliderScaleJawSpecial);	
	DDX_Control("Slider_ScaleJawH", m_pSliderScaleJawH);			
	DDX_Control("Slider_ScaleJawV", m_pSliderScaleJawV);

	DDX_Control("Slider_ScaleEar", m_pSliderScaleEar);
	DDX_Control("Slider_OffsetEarV", m_pSliderOffsetEarV);
	
	//����3ͥ�Ĺ������ؼ�
	DDX_Control("Slider_PartUp", m_pSliderPartUp);
	DDX_Control("Slider_PartMiddle", m_pSliderPartMiddle);
	DDX_Control("Slider_PartDown", m_pSliderPartDown);
	
	//��ʼ�����̱���
	DDX_Control("Slider_ScaleFaceH", m_pSliderScaleFaceH);
	DDX_Control("Slider_ScaleFaceV", m_pSliderScaleFaceV);
		
	if(!IsContextReady())
	return;

	//ddx data
	DDX_Slider(bSave, "Slider_OffsetForeheadH", GetFaceData()->offsetForeheadH);
	DDX_Slider(bSave, "Slider_OffsetForeheadV", GetFaceData()->offsetForeheadV);	
	DDX_Slider(bSave, "Slider_OffsetForeheadZ", GetFaceData()->offsetForeheadZ);	
	DDX_Slider(bSave, "Slider_RotateForehead", GetFaceData()->rotateForehead);		
	DDX_Slider(bSave, "Slider_ScaleForehead", GetFaceData()->scaleForehead);		

	DDX_Slider(bSave, "Slider_OffsetYokeBoneH", GetFaceData()->offsetYokeBoneH);
	DDX_Slider(bSave, "Slider_OffsetYokeBoneV", GetFaceData()->offsetYokeBoneV);	
	DDX_Slider(bSave, "Slider_OffsetYokeBoneZ", GetFaceData()->offsetYokeBoneZ);	
	DDX_Slider(bSave, "Slider_RotateYokeBone", GetFaceData()->rotateYokeBone);		
	DDX_Slider(bSave, "Slider_ScaleYokeBone", GetFaceData()->scaleYokeBone);		
	
	DDX_Slider(bSave, "Slider_OffsetCheekH", GetFaceData()->offsetCheekH);		
	DDX_Slider(bSave, "Slider_OffsetCheekV", GetFaceData()->offsetCheekV);		
	DDX_Slider(bSave, "Slider_OffsetCheekZ", GetFaceData()->offsetCheekZ);		
	DDX_Slider(bSave, "Slider_ScaleCheek", GetFaceData()->scaleCheek);			

	DDX_Slider(bSave, "Slider_OffsetChainV", GetFaceData()->offsetChainV);		
	DDX_Slider(bSave, "Slider_OffsetChainZ", GetFaceData()->offsetChainZ);		
	DDX_Slider(bSave, "Slider_RotateChain", GetFaceData()->rotateChain);		
	DDX_Slider(bSave, "Slider_ScaleChainH", GetFaceData()->scaleChainH);		

	DDX_Slider(bSave, "Slider_OffsetJawH", GetFaceData()->offsetJawH);			
	DDX_Slider(bSave, "Slider_OffsetJawV", GetFaceData()->offsetJawV);			
	DDX_Slider(bSave, "Slider_OffsetJawZ", GetFaceData()->offsetJawZ);			
	DDX_Slider(bSave, "Slider_ScaleJawSpecial", GetFaceData()->scaleJawSpecial);	
	DDX_Slider(bSave, "Slider_ScaleJawH", GetFaceData()->scaleJawH);			
	DDX_Slider(bSave, "Slider_ScaleJawV", GetFaceData()->scaleJawV);			
	
	DDX_Slider(bSave, "Slider_ScaleEar", GetFaceData()->scaleEar);
	DDX_Slider(bSave, "Slider_OffsetEarV", GetFaceData()->offsetEarV);

	DDX_Slider(bSave, "Slider_ScaleFaceH", GetFaceData()->scaleFaceH);
	DDX_Slider(bSave, "Slider_ScaleFaceV", GetFaceData()->scaleFaceV);
	DDX_Slider(bSave, "Slider_PartUp", GetFaceData()->scaleUp);
	DDX_Slider(bSave, "Slider_PartMiddle", GetFaceData()->scaleMiddle);
	DDX_Slider(bSave, "Slider_PartDown", GetFaceData()->scaleDown);

}

//----------------------------------------------------------
void CDlgCustomizeFace2::DoCalculate()
{
	//����������
	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	pBoneCtrler->RestoreBoneTM ();//�ָ���ʼ��λ��������
	
	GetCustomizeMan()->CalculateAdvaceOption();//����߼�ѡ��
	GetFace()->CalculateEar();//�������
	GetFace()->CalculateFace();
	GetFace()->Calculate3Parts();//����3ͥ

	//�ϲ�backup��������
	pBoneCtrler->ApplyBonesOnBakFace(true);
}

//----------------------------------------------------------
bool CDlgCustomizeFace2::InitResource()
{
	return true;
}

//----------------------------------------------------------
void CDlgCustomizeFace2::SetSliderBound()
{
	CECCustomizeBound * pSliderBound = GetCustomizeMan()->GetSliderBound();

	m_pSliderOffsetForeheadH->SetTotal(pSliderBound->m_nOffsetForeheadHMax, pSliderBound->m_nOffsetForeheadHMin);
	m_pSliderOffsetForeheadV->SetTotal(pSliderBound->m_nOffsetForeheadVMax, pSliderBound->m_nOffsetForeheadVMin);
	m_pSliderOffsetForeheadZ->SetTotal(pSliderBound->m_nOffsetForeheadZMax, pSliderBound->m_nOffsetForeheadZMin);
	m_pSliderRotateForehead->SetTotal(pSliderBound->m_nRotateForeheadMax, pSliderBound->m_nRotateForeheadMin);
	m_pSliderScaleForehead->SetTotal(pSliderBound->m_nScaleForeheadMax, pSliderBound->m_nScaleForeheadMin);

	m_pSliderOffsetYokeBoneH->SetTotal(pSliderBound->m_nOffsetYokeBoneHMax, pSliderBound->m_nOffsetYokeBoneHMin);
	m_pSliderOffsetYokeBoneV->SetTotal(pSliderBound->m_nOffsetYokeBoneVMax, pSliderBound->m_nOffsetYokeBoneVMin);
	m_pSliderOffsetYokeBoneZ->SetTotal(pSliderBound->m_nOffsetYokeBoneZMax, pSliderBound->m_nOffsetYokeBoneZMin);
	m_pSliderRotateYokeBone->SetTotal(pSliderBound->m_nRotateYokeBoneMax, pSliderBound->m_nRotateYokeBoneMin);
	m_pSliderScaleYokeBone->SetTotal(pSliderBound->m_nScaleYokeBoneMax, pSliderBound->m_nScaleYokeBoneMin);

	m_pSliderOffsetCheekH->SetTotal(pSliderBound->m_nOffsetCheekHMax, pSliderBound->m_nOffsetCheekHMin);
	m_pSliderOffsetCheekV->SetTotal(pSliderBound->m_nOffsetCheekVMax, pSliderBound->m_nOffsetCheekVMin);
	m_pSliderOffsetCheekZ->SetTotal(pSliderBound->m_nOffsetCheekZMax, pSliderBound->m_nOffsetCheekZMin);
	m_pSliderScaleCheek->SetTotal(pSliderBound->m_nScaleCheekMax, pSliderBound->m_nScaleCheekMin);

	m_pSliderOffsetChainZ->SetTotal(pSliderBound->m_nOffsetChainZMax, pSliderBound->m_nOffsetChainZMin);
	m_pSliderOffsetChainV->SetTotal(pSliderBound->m_nOffsetChainVMax, pSliderBound->m_nOffsetChainVMin);
	m_pSliderRotateChain->SetTotal(pSliderBound->m_nRotateChainMax, pSliderBound->m_nRotateChainMin);
	m_pSliderScaleChainH->SetTotal(pSliderBound->m_nScaleChainHMax, pSliderBound->m_nScaleChainHMin);
	
	m_pSliderOffsetJawH->SetTotal(pSliderBound->m_nOffsetJawHMax, pSliderBound->m_nOffsetJawHMin);
	m_pSliderOffsetJawV->SetTotal(pSliderBound->m_nOffsetJawVMax, pSliderBound->m_nOffsetJawVMin);
	m_pSliderOffsetJawZ->SetTotal(pSliderBound->m_nOffsetJawZMax, pSliderBound->m_nOffsetJawZMin);
	m_pSliderScaleJawSpecial->SetTotal(pSliderBound->m_nScaleJawSpecialMax, pSliderBound->m_nScaleJawSpecialMin);
	m_pSliderScaleJawH->SetTotal(pSliderBound->m_nScaleJawHMax, pSliderBound->m_nScaleJawHMin);
	m_pSliderScaleJawV->SetTotal(pSliderBound->m_nScaleJawVMax, pSliderBound->m_nScaleJawVMin);
	
	m_pSliderScaleEar->SetTotal(pSliderBound->m_nScaleEarMax, pSliderBound->m_nScaleEarMin);
	m_pSliderOffsetEarV->SetTotal(pSliderBound->m_nOffsetEarVMax, pSliderBound->m_nOffsetEarVMin);
	
	m_pSliderScaleFaceH->SetTotal( pSliderBound->m_nScaleFaceHMax, pSliderBound->m_nScaleFaceHMin);
	m_pSliderScaleFaceV->SetTotal(pSliderBound->m_nScaleFaceVMax, pSliderBound->m_nScaleFaceVMin);
	
	m_pSliderPartUp->SetTotal(pSliderBound->m_nScaleUpPartMax, pSliderBound->m_nScaleUpPartMin);
	m_pSliderPartMiddle->SetTotal(pSliderBound->m_nScaleMiddlePartMax, pSliderBound->m_nScaleMiddlePartMin);
	m_pSliderPartDown->SetTotal(pSliderBound->m_nScaleDownPartMax, pSliderBound->m_nScaleDownPartMin);
}

//-----------------------------------------------------------
void CDlgCustomizeFace2::DefaultForehead()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;

	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();

	GetFaceData()->offsetForeheadH = pMan->m_nDefaultOffsetForeheadH;
	GetFaceData()->offsetForeheadV = pMan->m_nDefaultOffsetForeheadV;
	GetFaceData()->offsetForeheadZ = pMan->m_nDefaultOffsetForeheadZ;
	GetFaceData()->rotateForehead= pMan->m_nDefaultRotateForehead;
	GetFaceData()->scaleForehead= pMan->m_nDefaultScaleForehead;
}

//-----------------------------------------------------------
void CDlgCustomizeFace2::DefaultYokeBone()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;
	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();

	GetFaceData()->offsetYokeBoneH = pMan->m_nDefaultOffsetYokeBoneH;
	GetFaceData()->offsetYokeBoneV = pMan->m_nDefaultOffsetYokeBoneV;
	GetFaceData()->offsetYokeBoneZ = pMan->m_nDefaultOffsetYokeBoneZ;
	GetFaceData()->rotateYokeBone = pMan->m_nDefaultRotateYokeBone;
	GetFaceData()->scaleYokeBone = pMan->m_nDefaultScaleYokeBone;
}

//-----------------------------------------------------------
void CDlgCustomizeFace2::DefaultCheek()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;
	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();

	GetFaceData()->offsetCheekH = pMan->m_nDefaultOffsetCheekH;
	GetFaceData()->offsetCheekV = pMan->m_nDefaultOffsetCheekV;
	GetFaceData()->offsetCheekZ = pMan->m_nDefaultOffsetCheekZ;
	GetFaceData()->scaleCheek = pMan->m_nDefaultScaleCheek;

}

//-----------------------------------------------------------
void CDlgCustomizeFace2::DefaultChain()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;
	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();

	GetFaceData()->offsetChainV = pMan->m_nDefaultOffsetChainV;
	GetFaceData()->offsetChainZ = pMan->m_nDefaultOffsetChainZ;
	GetFaceData()->rotateChain = pMan->m_nDefaultRotateChain;
	GetFaceData()->scaleChainH = pMan->m_nDefaultScaleChainH;
}

//-----------------------------------------------------------
void CDlgCustomizeFace2::DefaultJaw()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;
	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();	

	GetFaceData()->offsetJawH = pMan->m_nDefaultOffsetJawH;
	GetFaceData()->offsetJawV = pMan->m_nDefaultOffsetJawV;
	GetFaceData()->offsetJawZ = pMan->m_nDefaultOffsetJawZ;
	GetFaceData()->scaleJawSpecial = pMan->m_nDefaultScaleJawSpecial;
	GetFaceData()->scaleJawH = pMan->m_nDefaultScaleJawH;
	GetFaceData()->scaleJawV = pMan->m_nDefaultScaleJawV;
}

void CDlgCustomizeFace2::DefaultEar()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;
	GetFaceData()->scaleEar = pMan->m_nDefaultScaleEar;
	GetFaceData()->offsetEarV = pMan->m_nDefaultOffsetEarV;
}

//----------------------------------------------------------
//��Ҫ���ڸ�λ��ȱʡ���ݴ�������
void CDlgCustomizeFace2::LoadDefault()
{
	DefaultForehead();
	DefaultYokeBone();
	DefaultCheek();
	DefaultChain();
	DefaultJaw();
	DefaultEar();
	DefaultFace();

	UpdateData(false);
}

//----------------------------------------------------------
void CDlgCustomizeFace2::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	SetCanMove(false);

	//------------------------------------
	//Ԥ�ȼ�����˸߼���3ͥ���������
	//------------------------------------

	CFaceBoneController* pBoneCtrler;
	pBoneCtrler = GetFace()->GetBoneController();
	//��ʼ������
	pBoneCtrler->RestoreBoneTM();
	
	GetFace()->CalculateEye();//��������
	GetFace()->CalculateBrow();//����ü��
	GetFace()->CalculateNose();//�������
	GetFace()->CalculateMouth();//��������	
	GetFace()->CalculateBlendFace ();//����������
	
	pBoneCtrler->BackupBoneTM ();//����֮ǰ������
	
	GetCustomizeMan()->CalculateAdvaceOption();//����߼�ѡ��
	GetFace()->CalculateEar();//�������
	GetFace()->CalculateFace();
	GetFace()->Calculate3Parts();//����3ͥ
	
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
//-------------------------------------------------------
void CDlgCustomizeFace2::OnCommandMoveSlider(const char* szCommand)
{
	BEGIN_SLIDER_TIME_CHECKED;

	UpdateData(true);

	DoCalculate();

	END_SLIDER_TIME_CHECKED;
}
//-------------------------------------------------------
void CDlgCustomizeFace2::OnCommandReset(const char* szCommand)
{
	DefaultForehead();
	DefaultYokeBone();
	DefaultCheek();
	DefaultChain();
	DefaultJaw();
	DefaultEar();
	DefaultFace();
	UpdateData(false);
	DoCalculate();
}

//
void CDlgCustomizeFace2::DefaultFace()
{
	CECCustomizeMgr *pMan = GetCustomizeMan();
	if( !pMan)
		return;
	
	CECFace :: FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();
	
	pFaceData->scaleFaceH = pMan->m_nDefaultScaleFaceH;
	pFaceData->scaleFaceV = pMan->m_nDefaultScaleFaceV;
	pFaceData->scaleUp = pMan->m_nDefaultScaleUp;
	pFaceData->scaleMiddle = pMan->m_nDefaultScaleMiddle;
	pFaceData->scaleDown = pMan->m_nDefaultScaleDown;
}