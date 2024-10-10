#include "DlgCustomizeThirdEye.h"
#include "DlgCustomizeEye.h"
#include "EC_LoginUIMan.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "elementdataman.h"

AUI_BEGIN_COMMAND_MAP(CDlgCustomizeThirdEye, CDlgCustomizeBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Close",		OnCommand_Close)
AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgCustomizeThirdEye, CDlgCustomizeBase)
AUI_ON_EVENT("Img_*", WM_LBUTTONUP, OnLButtonUpListBoxThirdEye)
AUI_ON_EVENT("Scroll_*", WM_MOUSEWHEEL, OnMouseWheelListboxThirdEye)
AUI_ON_EVENT("Edit_*", WM_MOUSEWHEEL, OnMouseWheelListboxThirdEye)
AUI_ON_EVENT("Img_*", WM_MOUSEWHEEL, OnMouseWheelListboxThirdEye)
AUI_END_EVENT_MAP()


CDlgCustomizeThirdEye::CDlgCustomizeThirdEye()
{
	CDlgCustomizeBase::CDlgCustomizeBase();
}

CDlgCustomizeThirdEye::~CDlgCustomizeThirdEye()
{
	m_LbxThirdEye.ResetImages(true);
	m_LbxThirdEye.ResetContent();
}

void CDlgCustomizeThirdEye::OnShowDialog()
{
	//
	CDlgCustomizeBase::OnShowDialog();
}

void CDlgCustomizeThirdEye::OnTick()
{
	m_LbxThirdEye.OnTick();
}

bool CDlgCustomizeThirdEye::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;
	
	// must release the image stored in this grid first
	m_LbxThirdEye.ResetImages(true);
	if (!m_LbxThirdEye.Init(this, "Img_01", THIRDEYE_MAX_ROW, THIRDEYE_MAX_COL, "Scroll_01", "Edit_01") )
		return false;

	if (!CDlgCustomizeThirdEye::InitResource(GetGame(), GetPlayer(), GetCustomizeMan(), m_LbxThirdEye))
		return false;
	
	return true;
}

bool CDlgCustomizeThirdEye::InitResource(CECGame* pGame, CECPlayer* pPlayer, CECCustomizeMgr* pMgr, AUI_ImageGrid& imgGrid)
{
	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= pGame->GetElementDataMan ();

	//数据空间ID
	ID_SPACE eIDSpace = ID_SPACE_FACE;

	//获取脸型相关数据数量
	int nNumFaceData;
	nNumFaceData = pElementDataMan->get_data_num(eIDSpace);

	//脸型相关的数据类型
	DATA_TYPE dtFaceData;
	
	int nCharacterMask = 1 << pPlayer->GetProfession();
	
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

			if( !pMgr->m_bLoadFacePillData)
			{
				if( uFacePillOnly != 0)
					continue;
			}

			//当前人物id和性别id
			int uCurCharacterComboID = pFaceThirdEye->character_combo_id ;
			int uCurGenderID = pFaceThirdEye->gender_id ;
			
			//当前人物的脸部形状
			if( (nCharacterMask & uCurCharacterComboID) && (pPlayer->GetGender() == uCurGenderID))
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
				A2DSprite* pA2DIcon = new A2DSprite; //在imgGrid中释放
				
				A3DDevice* pDevice = pGame->GetA3DDevice();
				if( !pA2DIcon->Init ( pGame->GetA3DDevice(), pszIconFile, 0))
				{					
					a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizeThirdEye::InitResource()");
					
					delete pA2DIcon;
					#ifdef _DEBUG
					_asm int 3
					#endif
					return false;
				}
				
				imgGrid.Append(id, NULL, pszHintName, pA2DIcon, true);
			}
		}
	}	
	
	return true;
}


void CDlgCustomizeThirdEye::OnLButtonUpListBoxThirdEye(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
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

void CDlgCustomizeThirdEye::OnMouseWheelListboxThirdEye(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_LbxThirdEye.OnMouse_Wheel(wParam, lParam, pObj))
		return;
}

void CDlgCustomizeThirdEye::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgCustomizeThirdEye::OnCommand_Close(const char * szCommand)
{
	OnCommand("IDCANCEL");
}