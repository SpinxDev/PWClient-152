#include "DlgCustomizeHair.h"
#include "EC_CustomizeMgr.h"
#include "AUIListBox.h"
#include "EC_Face.h"
#include "EC_Game.h"
#include "EC_LoginUIMan.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "elementdataman.h"

#include "A3DViewport.h"
#include "WindowsX.h"

AUI_BEGIN_EVENT_MAP(CDlgCustomizeHair, CDlgCustomizeBase)

AUI_ON_EVENT("Img_*", WM_LBUTTONUP, OnLButtonUpListboxHair)
AUI_ON_EVENT("Scroll_*", WM_MOUSEWHEEL, OnMouseWheelListboxHair)
AUI_ON_EVENT("Edit_*", WM_MOUSEWHEEL, OnMouseWheelListboxHair)
AUI_ON_EVENT("Img_*", WM_MOUSEWHEEL, OnMouseWheelListboxHair)

AUI_END_EVENT_MAP()

CDlgCustomizeHair::CDlgCustomizeHair()
{
}

CDlgCustomizeHair::~CDlgCustomizeHair()
{
	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}

AUIStillImageButton * CDlgCustomizeHair::GetSwithButton()
{
	return GetCustomizeMan()->m_pBtnChooseHair;
}

void CDlgCustomizeHair::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	SetCanMove(false);

	int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();
	if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
	{
		((CECLoginUIMan*)m_pAUIManager)->ChangeScene(
			(CECLoginUIMan::LOGIN_SCENE)(CECLoginUIMan::LOGIN_SCENE_FACECUSTOMIZE_BEGIN +  GetPlayer()->GetProfession()));
	}
	else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
	{
		GetCustomizeMan()->ChangeScene(FACE_SCENE_HEAD);
	}
}

bool CDlgCustomizeHair::ChangeHairTexOnHairModelChange()
{
	//	ͷ��ģ�͸ı�ʱ��ͷ�������Ƿ���Ӧ�޸�
	//	����ʦ���̿͡����顢���飬����ͷ��ģ��ֱ��ѡ���Ӧ����
	
	if(!CanCustomizeHairTex())
	{
		DATA_TYPE DataType;
		elementdataman* pDB = GetGame()->GetElementDataMan();
		const FACE_HAIR_TEXTURE_MAP* pConfig = (const FACE_HAIR_TEXTURE_MAP*)pDB->get_data_ptr(953, ID_SPACE_FACE, DataType);
		if (pConfig && DataType == DT_FACE_HAIR_TEXTURE_MAP)
		{
			int mapSize = sizeof(pConfig->hair_texture_map)/sizeof(pConfig->hair_texture_map[0]);
			for (int i(0); i<mapSize; ++i)
			{
				if (pConfig->hair_texture_map[i].model_id == GetFaceData()->idHairModel)
				{
					GetFaceData()->idHairTex = pConfig->hair_texture_map[i].texture_id;
					GetFace()->UpdateHairTexture();
					break;
				}
			}
		}

		return true;
	}
	
	return false;
}

bool CDlgCustomizeHair::CanCustomizeHair()
{
	return true;
}

bool CDlgCustomizeHair::CanCustomizeHairTex()
{
	//	ͷ���������ͷ��ģ�ͱ仯ʱ����������ͷ��
	return false;
}

bool CDlgCustomizeHair::CanCustomizeGoatee()
{
	//	������Ƿ�ɶ���
	bool bRet(true);
	int iProfession = GetPlayer()->GetProfession();
	if (iProfession == PROF_MONK ||
		iProfession == PROF_GHOST)
	{
		bRet = false;
	}
	return bRet;
}

bool CDlgCustomizeHair::CanCustomizeGoateeTex()
{
	//	����������Ƿ�ɶ���
	bool bRet(true);
	int iProfession = GetPlayer()->GetProfession();
	if (iProfession == PROF_MONK ||
		iProfession == PROF_GHOST)
	{
		bRet = false;
	}
	return bRet;
}

bool CDlgCustomizeHair::CanCustomizeMoustache()
{
	//	С���������Ƿ�ɶ���
	bool bRet(true);
	int iProfession = GetPlayer()->GetProfession();
	if (iProfession == PROF_MONK ||
		iProfession == PROF_GHOST ||
		iProfession == PROF_JIANLING ||
		iProfession == PROF_MEILING ||
		iProfession == PROF_YEYING ||
		iProfession == PROF_YUEXIAN)
	{
		bRet = false;
	}
	return bRet;
}

bool CDlgCustomizeHair::InitResource()
{
	// ����ְҵ���ڽ����������������
	int iProfession = GetPlayer()->GetProfession();
	bool bCustomizeHair = CanCustomizeHair();
	bool bCustomizeHairTex = CanCustomizeHairTex();
	bool bCustomizeGoatee = CanCustomizeGoatee();
	bool bCustomizeGoateeTex = CanCustomizeGoateeTex();
	bool bCustomizeMoustache = CanCustomizeMoustache();

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

	unsigned int uHairModelCount = 0;
	unsigned int uGoateeModelCount = 0;
	unsigned int uMoustacheTexCount = 0;
	unsigned int uHairTexCount = 0;
	unsigned int uGoateeTexCount = 0;
	
	std::vector<AppearanceDataNode> hairDataList;


	//�������ݿ�
	for( int i = 0; i < nNumFaceData; i++)
	{
		//��ȡ��������id
		unsigned int uFaceDataID = pElementDataMan->get_data_id(eIDSpace, i, dtFaceData);
		
		//��������
		if( dtFaceData == DT_FACE_TEXTURE_ESSENCE)
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

			//���õ�ǰ�������������
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{			
				//����������id
				unsigned int uCurTexPartID = pFaceTextureEssence->tex_part_id;

				//**С��������**//
				if( uCurTexPartID == 7)
				{
					if (!bCustomizeMoustache)
						continue;

					unsigned int id = pFaceTextureEssence->id;
					//ͼ���ļ�
					char* pszIconFile = pFaceTextureEssence->file_icon;

					//���������
					ACHAR* pszHintName = pFaceTextureEssence->name;

					//���ڲ���listbox �ַ���
					ACHAR *pszName = _AL("");

					A2DSprite* pA2DIcon;

					if(*pszIconFile)
					{			
						pszIconFile += 9;//����"surfaces\"			
					}
					else
					{
						a_LogOutput(1, "Moustache's texture icon file is NULL! CDlgCustomizeHair::InitResource()");
						assert(0 && "Moustache's texture icon file is NULL! CDlgCustomizeHair::InitResource()");
						return false;
					}

					//��ͼ���ļ�ΪA2DSprite�ļ�
					pA2DIcon = new A2DSprite; //��Ҫ�ͷ�
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{

						a_LogOutput(1, "Failed to pA2DSprite->Init()! CDlgCustomizeHair::InitResource()");

						delete pA2DIcon;

						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}

					//��A2DSprite*�������Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxMoustache.Append(id, NULL, pszHintName, pA2DIcon, true);
					uMoustacheTexCount++;
				}
				//ͷ������
				else if( uCurTexPartID == 8)
				{
					if (!bCustomizeHairTex)
						continue;

					unsigned int id = pFaceTextureEssence->id;
					//ͼ���ļ�
					char* pszIconFile = pFaceTextureEssence->file_icon;

					//���������
					ACHAR* pszHintName = pFaceTextureEssence->name;

					//���ڲ���listbox �ַ���
					ACHAR *pszName = _AL("");

					A2DSprite* pA2DIcon;

					if(*pszIconFile)
					{			
						pszIconFile += 9;//����"surfaces\"			
					}
					else
					{
						a_LogOutput(1,"Hair's texture icon file is NULL! CDlgCustomizeHair::InitResource()"); 
						assert(0 && "Hair's texture icon file is NULL! CDlgCustomizeHair::InitResource()");

						return false;
					}

					//��ͼ���ļ�ΪA2DSprite�ļ�
					pA2DIcon = new A2DSprite; //��Ҫ�ͷ�
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeHair::InitResource()");
						delete pA2DIcon;

						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}

					//��A2DSprite*�������Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxHairTex.Append(id, NULL, pszHintName, pA2DIcon, true);
					uHairTexCount++;
				}//ifͷ������

				//���������
				else if( uCurTexPartID == 9)
				{
					if (!bCustomizeGoateeTex)
						continue;

					unsigned int id = pFaceTextureEssence->id;
					//ͼ���ļ�
					char* pszIconFile = pFaceTextureEssence->file_icon;

					//���������
					ACHAR* pszHintName = pFaceTextureEssence->name;

					//���ڲ���listbox �ַ���
					ACHAR *pszName = _AL("");

					A2DSprite* pA2DIcon;

					if(*pszIconFile)
					{			
						pszIconFile += 9;//����"surfaces\"			
					}
					else
					{
						a_LogOutput(1, "Goatee's texture icon file is NULL! CDlgCustomizeHair::InitResource()");
						assert(0 && "Goatee's texture icon file is NULL! CDlgCustomizeHair::InitResource()");
						return false;
					}

					//��ͼ���ļ�ΪA2DSprite�ļ�
					pA2DIcon = new A2DSprite; //��Ҫ�ͷ�
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeHair::InitResource()");
						delete pA2DIcon;
						
						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}

					//��A2DSprite*�������Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxGoateeTex.Append(id, NULL, pszHintName, pA2DIcon, true);
					uGoateeTexCount++;

				}//if���������

			}//if ��ǰ����
		}//if ��������
					
		//------------------------------------------------
		//�������ͷ��ģ����ص�����
		//------------------------------------------------
		else if( dtFaceData == DT_FACE_HAIR_ESSENCE)
		{
			if (!bCustomizeHair)
				continue;

			FACE_HAIR_ESSENCE* pFaceHairEssence = (FACE_HAIR_ESSENCE*)pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData);

			if( !pFaceHairEssence)
				continue;

			unsigned int uFacePillOnly = pFaceHairEssence->facepill_only;

			if( !GetCustomizeMan()->m_bLoadFacePillData)
			{
				if( uFacePillOnly != 0)
					continue;
			}

			//��ǰ����id���Ա�id
			int uCurCharacterComboID = pFaceHairEssence->character_combo_id ;
			int uCurGenderID = pFaceHairEssence->gender_id ;

			//��ǰ�����ͷ��ģ��
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{				
				int id = pFaceHairEssence->id;

				//ͼ���ļ�
				char* pszIconFile = pFaceHairEssence->file_icon;

				char* pszHairModelFile = pFaceHairEssence->file_hair_model;
				char* pszHairSkinFile = pFaceHairEssence->file_hair_skin;
				
				ACHAR* pszHintName = pFaceHairEssence->name;

				A2DSprite* pA2DIcon;
				
				if( !pszIconFile)
				{
					a_LogOutput(1, "Hair's model icon file is NULL! CDlgCustomizeHair::InitResource()");
					assert( 0 && "Hair's model icon file is NULL! CDlgCustomizeHair::InitResource()");

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
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeHair::InitResource()");
						delete pA2DIcon;

						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}

					//��A2DSprite*�������Vector
					m_vecA2DSprite.push_back(pA2DIcon);
				}
				
			
				//�����б��
				ACHAR *pszName = _AL("");
				AppearanceDataNode datanode;
				datanode.tid = id;
				datanode.hintName = pszHintName;
				datanode.p2DIcon = pA2DIcon;
				hairDataList.push_back(datanode);
				uHairModelCount ++;
				
			}//if��ǰ�����ͷ��ģ��

		}//ifͷ��ģ����ص�����

		//������������
		else if( dtFaceData == DT_FACE_MOUSTACHE_ESSENCE)
		{
			if (!bCustomizeGoatee)
				continue;
			
			FACE_MOUSTACHE_ESSENCE* pFaceMoustancheEssence = (FACE_MOUSTACHE_ESSENCE*) pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData);

			if(!pFaceMoustancheEssence)
				continue;

			unsigned int uFacePillOnly = pFaceMoustancheEssence->facepill_only;

			if( !GetCustomizeMan()->m_bLoadFacePillData)
			{
				if( uFacePillOnly != 0)
					continue;
			}

			//��ǰ����id���Ա�id
			int uCurCharacterComboID = pFaceMoustancheEssence->character_combo_id ;
			int uCurGenderID = pFaceMoustancheEssence->gender_id ;

			//��ǰ����ĺ���ģ��
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{	
				int id = pFaceMoustancheEssence->id;
			
				ACHAR* pszGoateeName = pFaceMoustancheEssence->name;

				//ͼ���ļ�
				char* pszIconFile = pFaceMoustancheEssence->file_icon;

				char* pszGoateeSkinFile = pFaceMoustancheEssence->file_moustache_skin;
			
				ACHAR* pszHintName = pFaceMoustancheEssence->name;

				A2DSprite* pA2DIcon;

				if( !pszIconFile)
				{
					a_LogOutput(1, "Goatee's model icon file is NULL! CDlgCustomizeHair::InitResource()");
					assert( 0 && "Goatee's model icon file is NULL! CDlgCustomizeHair::InitResource()");
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
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeHair::InitResource()");

						delete pA2DIcon;

						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}

					//��A2DSprite*�������Vector
					m_vecA2DSprite.push_back(pA2DIcon);
				}
				
				//�����б��
				ACHAR *pszName = _AL("");
				m_ListBoxGoatee.Append(id, NULL, pszHintName, pA2DIcon, true);				
				uGoateeModelCount ++;
				
			}//if��ǰ����ĺ���ģ��

		}//if����ģ���������

	}//�������ݿ�

	//��ְҵ���з��������µķ��ͷŵ�ǰ��
	if (iProfession == PROF_YEYING || iProfession == PROF_YUEXIAN){
		std::sort(hairDataList.begin(),hairDataList.end(), std::greater<AppearanceDataNode>());
	}

	std::vector<AppearanceDataNode>::iterator it = hairDataList.begin();
	for (; it != hairDataList.end(); ++it){
		m_ListBoxHair.Append(it->tid, NULL, it->hintName, it->p2DIcon, true);
	}

	return true;
}

bool CDlgCustomizeHair::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;

	ClearListCtrlAndA2DSprite();

	A2DSprite *pSpriteSelect = GetCustomizeMan()->GetImageSelected();
	A2DSprite *pSpriteMouseOn = GetCustomizeMan()->GetImageMouseOn();
	if (!m_ListBoxHair.Init(this, "Img_01", 3, 3, "Scroll_01", "Edit_01", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxHairTex.Init(this, "Img_19", 1, 3, "Scroll_02", "Edit_02", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxMoustache.Init(this, "Img_10", 1, 3, "Scroll_03", "Edit_03", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxGoatee.Init(this, "Img_13", 1, 3, "Scroll_04", "Edit_04", pSpriteSelect, pSpriteMouseOn) ||
		!m_ListBoxGoateeTex.Init(this, "Img_16", 1, 3, "Scroll_05", "Edit_05", pSpriteSelect, pSpriteMouseOn))
		return false;
	
	if (!InitResource())
		return false;

	UpdateData(false);

	return true;
}

void CDlgCustomizeHair::DoDataExchange(bool bSave)
{
	CDlgCustomizeBase::DoDataExchange(bSave);

	// ddx control	

	if (!IsContextReady()) return;
	// ddx data
}


void CDlgCustomizeHair::OnLButtonUpListboxHair(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!IsContextReady()){
		return;
	}	
	{
		PAUIOBJECT pImage = m_ListBoxHair.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idHairModel = (int)pImage->GetData();
			UpdateData(true);
			ChangeHairTexOnHairModelChange();			
			GetFace()->UpdateHairModel();
			return;
		}
	}
	{
		PAUIOBJECT pImage = m_ListBoxHairTex.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idHairTex = (int)pImage->GetData();
			UpdateData(true);
			GetFace()->UpdateHairTexture();
			return;
		}
	}
	{
		PAUIOBJECT pImage = m_ListBoxMoustache.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idMoustacheTex = (int)pImage->GetData();
			UpdateData(true);
			GetFace()->UpdateMoustacheTexture();
			return;
		}
	}
	{
		PAUIOBJECT pImage = m_ListBoxGoatee.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idMoustacheSkin = (int)pImage->GetData();
			UpdateData(true);
			GetFace()->UpdateGoateeModel();
			return ;
		}
	}
	{
		PAUIOBJECT pImage = m_ListBoxGoateeTex.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idGoateeTex = (int)pImage->GetData();
			UpdateData(true);
			GetFace()->UpdateGoateeTexture();
			return;
		}
	}
}

void CDlgCustomizeHair::OnMouseWheelListboxHair(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_ListBoxHair.OnMouse_Wheel(wParam, lParam, pObj))	
		return;
	if (m_ListBoxHairTex.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_ListBoxMoustache.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_ListBoxGoatee.OnMouse_Wheel(wParam, lParam, pObj))
		return;
	if (m_ListBoxGoateeTex.OnMouse_Wheel(wParam, lParam, pObj))
		return;
}

void CDlgCustomizeHair::OnTick()
{
	m_ListBoxHair.OnTick();
	m_ListBoxHairTex.OnTick();
	m_ListBoxMoustache.OnTick();
	m_ListBoxGoatee.OnTick();
	m_ListBoxGoateeTex.OnTick();
}

//--------------------------------------------------------------------
void CDlgCustomizeHair::ClearListCtrlAndA2DSprite()
{
	m_ListBoxHair.ResetContent();
	m_ListBoxMoustache.ResetContent();
	m_ListBoxGoatee.ResetContent();
	m_ListBoxHairTex.ResetContent();
	m_ListBoxGoateeTex.ResetContent();

	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}