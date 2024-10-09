#include "DlgCustomizePaint.h"
#include "EC_CustomizeMgr.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "AUIListBox.h"
#include "AUIImagePicture.h"
#include <A3DSurface.h>
#include <A3DSurfaceMan.h>
#include "EC_Face.h"
#include "EC_Player.h"
#include "EC_Game.h"
#include "elementdataman.h"
#include <A3DMacros.h>
#include "EC_LoginUIMan.h"
#include "elementdataman.h"
#include "EC_Viewport.h"
#include "A3DViewport.h"
#include "WindowsX.h"

#define DEFAULT_COLOR 0xffffffff

//////////////////////////////////////////////////////////////////////////
// CPaletteControl
//////////////////////////////////////////////////////////////////////////


CPaletteControl::CPaletteControl(AUIImagePicture *pPane, AUIImagePicture * pIndicator, AUIObject *pDisplay, AUIDialog* pDlg) :
	m_pPane(pPane), 
	m_pIndicator(pIndicator),
	m_pDisplay(pDisplay),
	m_pDlg(pDlg),
	m_pSurface(NULL),
	m_color(DEFAULT_COLOR)
{

}

CPaletteControl::~CPaletteControl()
{
	if (m_pSurface)
	{
		A3DSurfaceMan* pA3DSurfaceMan = g_pGame->GetA3DEngine()->GetA3DSurfaceMan();
		pA3DSurfaceMan->ReleaseSurface(m_pSurface);
		m_pSurface = NULL;
	}
}

void CPaletteControl::ResetContext(A3DSurface *pSurface, A2DSprite *pSprite)
{
	m_pPane->SetCover(pSprite, 0);

	if (m_pSurface)
	{
		A3DSurfaceMan* pA3DSurfaceMan = g_pGame->GetA3DEngine()->GetA3DSurfaceMan();
		pA3DSurfaceMan->ReleaseSurface(m_pSurface);
	}
	m_pSurface = pSurface;

	SetColor(DEFAULT_COLOR);
}

A3DCOLOR CPaletteControl::GetColor()
{
	return m_color;
}

void CPaletteControl::SetColor(A3DCOLOR color)
{
	m_color = color;

	POINT pt = GetPtFromColor();
	SetIndicatorPosition(pt);
	UpdateDisplay();
}

namespace {
	int ColorDiff(A3DCOLOR& c1, A3DCOLOR& c2) {
		unsigned char* rgb1 = (unsigned char*)(&c1);
		unsigned char* rgb2 = (unsigned char*)(&c2);
		return ((unsigned short)(short)rgb1[0] - (unsigned short)(short)rgb2[0]) * ((unsigned short)(short)rgb1[0] - (unsigned short)(short)rgb2[0]) +
			((unsigned short)(short)rgb1[1] - (unsigned short)(short)rgb2[1]) * ((unsigned short)(short)rgb1[1] - (unsigned short)(short)rgb2[1]) +
			((unsigned short)(short)rgb1[2] - (unsigned short)(short)rgb2[2]) * ((unsigned short)(short)rgb1[2] - (unsigned short)(short)rgb2[2]);
	}
}

POINT CPaletteControl::SurfacePos2PanelPos(POINT &pt) {
	POINT ret;
	ret.x = pt.x / (float)m_pSurface->GetWidth() * m_pPane->GetSize().cx + 0.5f;
	ret.y = pt.y / (float)m_pSurface->GetHeight() * m_pPane->GetSize().cy + 0.5f;
	return ret;
}

POINT CPaletteControl::GetPtFromColor()
{
	POINT ptFailed = {0, 0};
	POINT ptDst = ptFailed;
	int minDiff = 255 * 255 * 3;

	if (m_pSurface->GetFormat() != A3DFMT_A8R8G8B8) return ptFailed;
	
	A3DCOLOR *pData = NULL;
	int nPitch = 0;
	if (!m_pSurface->LockRect(NULL, (void **)&pData, &nPitch, 0))
		return ptFailed;
	for (int j = 0; j < m_pSurface->GetHeight(); j++)
	{
		for (int i = 0; i < m_pSurface->GetWidth(); i++)
		{
			if (pData[j * (nPitch / 4) + i] == m_color)
			{
				m_pSurface->UnlockRect();
				POINT pt = {i, j};
				return SurfacePos2PanelPos(pt);
			}
			int diff = ColorDiff(pData[j * (nPitch / 4) + i], m_color);
			if (diff < minDiff) {
				minDiff = diff;
				ptDst.x = i;
				ptDst.y = j;
			}
		}
	}

	m_pSurface->UnlockRect();

	//if (minDiff < 300) {
		return SurfacePos2PanelPos(ptDst);
	//}

	return ptFailed;
}

void CPaletteControl::SetIndicatorPosition(POINT center)
{
	A3DRECT rect = m_pIndicator->GetRect();

	POINT pt = 
	{
		center.x - rect.Width() / 2, 
		center.y - rect.Width() / 2  
	};
	PaneToDialog(pt);
	m_pIndicator->SetPos(pt.x, pt.y);
	m_pDlg->ChangeFocus(m_pIndicator);
}


void CPaletteControl::MoveIndicator(POINT center)
{
	ClampPointToPane(center);

	m_color = GetColorFromPt(center);
	SetIndicatorPosition(center);
	UpdateDisplay();
}


A3DCOLOR CPaletteControl::GetColorFromPt(POINT pt)
{
	a_Clamp<long>(pt.x, 0, m_pPane->GetSize().cx - 1);
	a_Clamp<long>(pt.y, 0, m_pPane->GetSize().cy - 1);

	// 处理surface和imagepicture大小不一致（如缩放）的情况
	pt.x = pt.x * (float)m_pSurface->GetWidth() / (float)m_pPane->GetRect().Width() + 0.5f;
	pt.y = pt.y * (float)m_pSurface->GetHeight() / (float)m_pPane->GetRect().Height() + 0.5f;

	RECT rect = {pt.x, pt.y, pt.x + 1, pt.y + 1};

	int nPicPitch = 0;
	A3DCOLOR *pData = NULL;
	if (!m_pSurface->LockRect(&rect, (void**)&pData, &nPicPitch, 0))
		return DEFAULT_COLOR;
	m_pSurface->UnlockRect();
	
	return *pData;
}

POINT CPaletteControl::ClampPointToPane(POINT &pt)
{
	A3DRECT rect = m_pPane->GetRect();
	a_Clamp<long>(pt.x, 0, rect.Width() - 1);
	a_Clamp<long>(pt.y, 0, rect.Height() - 1);
	
	return pt;
}

void CPaletteControl::DialogToPane(POINT &pt)
{
	POINT posPane = m_pPane->GetPos(true);
	pt.x -= posPane.x;
	pt.y -= posPane.y;
}

void CPaletteControl::PaneToDialog(POINT &pt)
{
	POINT posPane = m_pPane->GetPos(true);
	pt.x += posPane.x;
	pt.y += posPane.y;
}

void CPaletteControl::UpdateDisplay()
{
	if (m_pDisplay)
		m_pDisplay->SetColor(m_color);
}

//////////////////////////////////////////////////////////////////////////
// CDlgCustomizePaint 
//////////////////////////////////////////////////////////////////////////

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

AUI_BEGIN_EVENT_MAP(CDlgCustomizePaint, CDlgCustomizeBase)

AUI_ON_EVENT("ImagePicture_ColorPic", WM_MOUSEMOVE, OnMouseMovePicPallete)
AUI_ON_EVENT("ImagePicture_ColorPic", WM_LBUTTONDOWN, OnLButtonDownPicPallete)

AUI_ON_EVENT("Img_*", WM_LBUTTONUP, OnLButtonUpListboxEyeKohl)
AUI_ON_EVENT("ListBox_FaceParts", WM_LBUTTONUP, OnLButtonUpListboxFacepart)

AUI_ON_EVENT("Scroll_*", WM_MOUSEWHEEL, OnMouseWheelListboxEyeKohl)
AUI_ON_EVENT("Edit_*", WM_MOUSEWHEEL, OnMouseWheelListboxEyeKohl)
AUI_ON_EVENT("Img_*", WM_MOUSEWHEEL, OnMouseWheelListboxEyeKohl)
AUI_ON_EVENT("ListBox_FaceParts", WM_KEYDOWN, OnKeyDownListboxFacepart)


AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgCustomizePaint, CDlgCustomizeBase)

AUI_ON_COMMAND("Reset", OnCommandReset)

AUI_END_COMMAND_MAP()

CDlgCustomizePaint::CDlgCustomizePaint()
{
	m_pPicPane = NULL;
	m_pPicDisplay = NULL;
	m_pPicIndicator = NULL;
	m_pLbxFacePart = NULL;
	m_nSelPartIndex = 0;
	m_pPallete = NULL;
}

CDlgCustomizePaint::~CDlgCustomizePaint()
{
	if( m_pPallete)
	{
		delete m_pPallete;
		m_pPallete = NULL;
	}
	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}

AUIStillImageButton * CDlgCustomizePaint::GetSwithButton()
{
	return GetCustomizeMan()->m_pBtnChooseFacePainting;
}


void CDlgCustomizePaint::LoadDefault()
{
	GetFaceData()->colorFace = DEFAULT_COLOR;
	GetFaceData()->colorEye = DEFAULT_COLOR;
	GetFaceData()->colorBrow = DEFAULT_COLOR;
	GetFaceData()->colorMouth = DEFAULT_COLOR;
	GetFaceData()->colorHair = DEFAULT_COLOR;
	GetFaceData()->colorEyeBall = DEFAULT_COLOR;
	GetFaceData()->colorMoustache = DEFAULT_COLOR;

	GetFace()->UpdateFaceColor();
	GetFace()->UpdateEyeColor();
	GetFace()->UpdateBrowColor();
	GetFace()->UpdateMouthColor();
	GetFace()->UpdateHairColor();
	GetFace()->UpdateEyeBallColor();
	GetFace()->UpdateGoateeColor();

	GetPlayer()->SetBodyColor(DEFAULT_COLOR);

	UpdateData(false);
	OnLButtonUpListboxFacepart(NULL, NULL, NULL);
}

bool CDlgCustomizePaint::OnInitDialog()
{
	if (!CDlgCustomizeBase::OnInitDialog())
		return false;

	m_pPallete = new CPaletteControl(m_pPicPane, m_pPicIndicator, m_pPicDisplay, this);

	return true;
}

#define COLOR_BASE_NUM 2800

bool CDlgCustomizePaint::InitResource()
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

	unsigned int uEyeHighTexCount = 0;
	unsigned int uColorPickerItemCount = 0;

	//遍历数据库
	for( int i = 0; i < nNumFaceData; i++)
	{
		//获取脸型数据id
		unsigned int uFaceDataID = pElementDataMan->get_data_id(eIDSpace, i, dtFaceData);
		
		//纹理数据
		if( dtFaceData == DT_FACE_TEXTURE_ESSENCE)
		{
			FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData); 
			if(!pFaceTextureEssence)
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

			//当前人物的脸部纹理
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{			
				//脸型纹理部分id
				unsigned int uCurTexPartID = pFaceTextureEssence->tex_part_id;

				//**眼影纹理**//
				if( uCurTexPartID == 2)
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
						a_LogOutput(1, "EyeKonl's texture icon file name is NULL! CDlgCustomizePaint::InitResource()");

						assert(0 && "EyeKonl's texture icon file name is NULL! CDlgCustomizePaint::InitResource()");
						return false;
					}

					//变图标文件为A2DSprite文件
					pA2DIcon = new A2DSprite; //需要释放
					A3DDevice* pDevice = GetGame()->GetA3DDevice();
					if( !pA2DIcon->Init ( GetGame()->GetA3DDevice(), pszIconFile, 0))
					{
						a_LogOutput(1, "Failed to pA2DIcon->Init()! CDlgCustomizePaint::InitResource()");
						delete pA2DIcon;

						#ifdef _DEBUG
						_asm int 3
						#endif

						return false;
					}

					//把A2DSprite*对象插入Vector
					m_vecA2DSprite.push_back(pA2DIcon);
					m_ListBoxEyeKohl.Append(id, NULL, pszHintName, pA2DIcon, true);
					uEyeHighTexCount++;
				}//if 眼影纹理

			}//if 当前人物

		}//if 纹理数据
		
		//-------------------------------------------
		//如果是与颜色贴图相关的数据
		//-------------------------------------------
		else if( dtFaceData == DT_COLORPICKER_ESSENCE)
		{
			COLORPICKER_ESSENCE* pColorPickerEssence = (COLORPICKER_ESSENCE*)pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData);
			if( !pColorPickerEssence)
				continue;

			//当前人物id和性别id
			int uCurCharacterComboID = pColorPickerEssence->character_combo_id ;
			int uCurGenderID = pColorPickerEssence->gender_id ;
			unsigned int uCurColorPartID = pColorPickerEssence->color_part_id;

			//当前人物的脸部颜色图片
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{
				//颜色图片文件名
				char* pszPicFileName = pColorPickerEssence->file_colorpicker;

				if( !pszPicFileName)
				{
					a_LogOutput(1, "Customize color file name is NULL! CDlgCustomizePaint::InitResource()");

					assert(0 && "Customize color file name is NULL! CDlgCustomizePaint::InitResource()");
	
					return false;
				}
				else
				{
					pszPicFileName += 9;//去掉"surface/"
				}

				//获取数据id
				int id = pColorPickerEssence->id;
				
				//A2DSprite文件
				A2DSprite* pA2DSpriteColorPic = new A2DSprite; //需要释放
	
				A3DDevice*  pDevice = GetGame()->GetA3DDevice();
				if( !pA2DSpriteColorPic->Init ( pDevice, pszPicFileName, 0))
				{
					a_LogOutput(1, "Failed to pA2DSpriteColorPic->Init()! CDlgCustomizePaint::InitResource()");
					
					delete pA2DSpriteColorPic;

					#ifdef _DEBUG
					_asm int 3
					#endif
						
					return false;
				}
				//把A2DSprite*对象插入Vector
 				m_vecA2DSprite.push_back(pA2DSpriteColorPic);

				A3DSurfaceMan* pA3DSurfaceMan = GetGame()->GetA3DEngine()->GetA3DSurfaceMan ();

				switch(uCurColorPartID)
				{
				case 0://脸			
					m_pLbxFacePart->AddString(GetStringFromTable(COLOR_BASE_NUM + uCurColorPartID));
					m_pLbxFacePart->SetItemDataPtr (uColorPickerItemCount, pA2DSpriteColorPic);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, id);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, 0, 1);

					uColorPickerItemCount ++;	
					break;
				case 1://眼影
					m_pLbxFacePart->AddString(GetStringFromTable(COLOR_BASE_NUM + uCurColorPartID));
					
					m_pLbxFacePart->SetItemDataPtr (uColorPickerItemCount, pA2DSpriteColorPic);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, id);

					m_pLbxFacePart->SetItemData(uColorPickerItemCount, 1, 1);

					uColorPickerItemCount ++;
					break;
				case 2://眉毛
					m_pLbxFacePart->AddString(GetStringFromTable(COLOR_BASE_NUM + uCurColorPartID));
					
					m_pLbxFacePart->SetItemDataPtr (uColorPickerItemCount, pA2DSpriteColorPic);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, id);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, 2, 1);

					uColorPickerItemCount ++;
					break;
				case 3://嘴唇
					m_pLbxFacePart->AddString(GetStringFromTable(COLOR_BASE_NUM + uCurColorPartID));
					
					m_pLbxFacePart->SetItemDataPtr (uColorPickerItemCount, pA2DSpriteColorPic);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, id);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, 3, 1);

					uColorPickerItemCount ++;
					break;
				case 4://头发
					m_pLbxFacePart->AddString(GetStringFromTable(COLOR_BASE_NUM + uCurColorPartID));
					m_pLbxFacePart->SetItemDataPtr (uColorPickerItemCount, pA2DSpriteColorPic);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, id);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, 4, 1);

					uColorPickerItemCount ++;
					break;
				case 5://眼珠
					m_pLbxFacePart->AddString(GetStringFromTable(COLOR_BASE_NUM + uCurColorPartID));
					m_pLbxFacePart->SetItemDataPtr (uColorPickerItemCount, pA2DSpriteColorPic);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, id);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, 5, 1);

					uColorPickerItemCount ++;
					break;
				case 6://胡子
					m_pLbxFacePart->AddString(GetStringFromTable(COLOR_BASE_NUM + uCurColorPartID));
					m_pLbxFacePart->SetItemDataPtr (uColorPickerItemCount, pA2DSpriteColorPic);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, id);
					m_pLbxFacePart->SetItemData(uColorPickerItemCount, 6, 1);

					uColorPickerItemCount ++;
					break;
				}


			}//if为当前人物数据

		}//if 颜色贴图数据

	}//遍历整个数据库

	return true;
}

bool CDlgCustomizePaint::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;

	ClearListCtrlAndA2DSprite();
	
	A2DSprite *pSpriteSelect = GetCustomizeMan()->GetImageSelected();
	A2DSprite *pSpriteMouseOn = GetCustomizeMan()->GetImageMouseOn();
	if (!m_ListBoxEyeKohl.Init(this, "Img_01", 1, 3, "Scroll_01", "Edit_01", pSpriteSelect, pSpriteMouseOn))
		return false;
	
	if (!InitResource())
		return false;
	
	return true;
}

void CDlgCustomizePaint::DoDataExchange(bool bSave)
{
	CDlgCustomizeBase::DoDataExchange(bSave);

	// ddx control
	DDX_Control("ImagePicture_ColorPic", m_pPicPane);
	DDX_Control("ImagePicture_CurColor", m_pPicDisplay);
	DDX_Control("Pic_ColorPos", m_pPicIndicator);
	DDX_Control("ListBox_FaceParts", m_pLbxFacePart);
	
	// ddx data
	if (!IsContextReady()) return;

	DDX_ListBox(bSave, "ListBox_FaceParts", m_nSelPartIndex);
}

void CDlgCustomizePaint::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	SetCanMove(false);
	
	m_nSelPartIndex = 0;
	UpdateData(false);

	OnLButtonUpListboxFacepart(NULL, NULL, NULL);

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

//-----------------------------------------------------------------------------------
void CDlgCustomizePaint::OnLButtonUpListboxFacepart(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if(pObj)
	{

		//判断是否点中列表框中的选项
		POINT ptPos = pObj->GetPos();

		A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
		int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
		int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

		if(m_pLbxFacePart->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
			return;
	}
	
	UpdateData(true);

	A3DCOLOR color;
	switch(m_pLbxFacePart->GetItemData(m_nSelPartIndex, 1))
	{
	case 0:
		color = GetFaceData()->colorFace;
		break;
	case 1:
		color = GetFaceData()->colorEye;
		break;
	case 2:
		color = GetFaceData()->colorBrow;
		break;
	case 3:
		color = GetFaceData()->colorMouth;
		break;
	case 4:
		color = GetFaceData()->colorHair;
		break;
	case 5:
		color = GetFaceData()->colorEyeBall;
		break;
	case 6:
		color = GetFaceData()->colorMoustache;
		break;
	default:
		return;
	}

	ResetPaletteControl();
	m_pPallete->SetColor(color);
}

//--------------------------------------------------------------------------------------
void CDlgCustomizePaint::OnLButtonUpListboxEyeKohl(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (!IsContextReady()){
		return ;
	}
	{
		PAUIOBJECT pImage = m_ListBoxEyeKohl.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{
			GetFaceData()->idEyeHighTex = (int)pImage->GetData();			
			UpdateData(true);			
			GetFace()->UpdateEyeTexture();
			return;
		}
	}
}

//--------------------------------------------------------------------------------------
void CDlgCustomizePaint::OnKeyDownListboxFacepart(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( wParam != VK_UP && wParam != VK_DOWN)
	{
		return;
	}

	UpdateData(true);

	A3DCOLOR color;
	switch(m_pLbxFacePart->GetItemData(m_nSelPartIndex, 1))
	{
	case 0:
		color = GetFaceData()->colorFace;
		break;
	case 1:
		color = GetFaceData()->colorEye;
		break;
	case 2:
		color = GetFaceData()->colorBrow;
		break;
	case 3:
		color = GetFaceData()->colorMouth;
		break;
	case 4:
		color = GetFaceData()->colorHair;
		break;
	case 5:
		color = GetFaceData()->colorEyeBall;
		break;
	case 6:
		color = GetFaceData()->colorMoustache;
		break;
	default:
		ASSERT(false);
	}

	ResetPaletteControl();
	m_pPallete->SetColor(color);
}

//--------------------------------------------------------------------------------------
void CDlgCustomizePaint::OnMouseWheelListboxEyeKohl(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	m_ListBoxEyeKohl.OnMouse_Wheel(wParam, lParam, pObj);
}

void CDlgCustomizePaint::OnTick()
{
	m_ListBoxEyeKohl.OnTick();
}



void CDlgCustomizePaint::OnLButtonDownPicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	OnMouseMovePicPallete(MK_LBUTTON, lParam, pObj);
}

void CDlgCustomizePaint::OnMouseMovePicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (!(wParam & MK_LBUTTON)) return;

	A3DVIEWPORTPARAM* param = GetGame()->GetViewport()->GetA3DViewport()->GetParam();

	POINT pt = 
	{
		GET_X_LPARAM(lParam) - m_pPicPane->GetPos().x - param->X,
		GET_Y_LPARAM(lParam) - m_pPicPane->GetPos().y - param->Y
	};

	m_pPallete->MoveIndicator(pt);

	UpdatePlayer(m_pPallete->GetColor());
}

void CDlgCustomizePaint::UpdatePlayer(A3DCOLOR color)
{
	UpdateData(true);

	switch(m_pLbxFacePart->GetItemData(m_nSelPartIndex, 1))
	{
	case 0:
		GetFaceData()->colorFace = color;
		GetFace()->UpdateFaceColor();
		GetPlayer()->SetBodyColor(color);
		break;
	case 1:
		GetFaceData()->colorEye = color;
		GetFace()->UpdateEyeColor();
		break;
	case 2:
		GetFaceData()->colorBrow = color;
		GetFace()->UpdateBrowColor();
		break;
	case 3:
		GetFaceData()->colorMouth = color;
		GetFace()->UpdateMouthColor();
		break;
	case 4:
		GetFaceData()->colorHair = color;
		GetFace()->UpdateHairColor();
		break;
	case 5:
		GetFaceData()->colorEyeBall = color;
		GetFace()->UpdateEyeBallColor();
		break;
	case 6:
		GetFaceData()->colorMoustache = color;
		GetFace()->UpdateGoateeColor();
		break;
	default:
		ASSERT(false);
	}
}




void CDlgCustomizePaint::ResetPaletteControl()
{
	UpdateData(true);

	A3DSurfaceMan* pA3DSurfaceMan = GetGame()->GetA3DEngine ()->GetA3DSurfaceMan ();				

	elementdataman* pElementDataMan;
	pElementDataMan= GetGame()->GetElementDataMan ();

	int id = m_pLbxFacePart->GetItemData(m_nSelPartIndex);
	DATA_TYPE dtFaceData = DT_COLORPICKER_ESSENCE;
	COLORPICKER_ESSENCE* pColorPickerEssence = (COLORPICKER_ESSENCE*)pElementDataMan->get_data_ptr(id, ID_SPACE_FACE,dtFaceData);
	if( !pColorPickerEssence)
		return;

	char* pszPicFileName = pColorPickerEssence->file_colorpicker;
	
	if( !pszPicFileName)
		return;
	
	pszPicFileName += strlen("surface\\");	//去掉"surface\\"
	
	A3DSurface* pSurface = NULL;
	if (!pA3DSurfaceMan->LoadCursorSurfaceFromFile(pszPicFileName, 0, &pSurface))
		return;

	A2DSprite* pSprite = (A2DSprite*)m_pLbxFacePart->GetItemDataPtr(m_nSelPartIndex);

	m_pPallete->ResetContext(pSurface, pSprite);
}

bool CDlgCustomizePaint::Render(void)
{
	SetNoFlush(true);

	return CDlgCustomizeBase::Render();
}

void CDlgCustomizePaint::ClearListCtrlAndA2DSprite()
{
	if(m_pLbxFacePart)
		m_pLbxFacePart->ResetContent();

	m_ListBoxEyeKohl.ResetContent();

	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();

}

void CDlgCustomizePaint::OnCommandReset(const char* szCommand)
{
	ResetFaceAllColor();
}

//复位脸部颜色
void CDlgCustomizePaint::ResetFaceAllColor()
{
	//**获取FaceData指针
	CECFace::FACE_CUSTOMIZEDATA* pFaceData = GetFaceData();

	pFaceData->colorBrow = 0xffffffff;
	pFaceData->colorEye = 0xffffffff;
	pFaceData->colorEyeBall = 0xffffffff;
	pFaceData->colorFace = 0xffffffff;
	pFaceData->colorHair = 0xffffffff;
	pFaceData->colorMoustache = 0xffffffff;
	pFaceData->colorMouth = 0xffffffff;

	GetFace()->UpdateAllFaceColor();
	m_pPicDisplay->SetColor(0xffffffff);

	GetPlayer()->SetBodyColor(0xffffffff);
}