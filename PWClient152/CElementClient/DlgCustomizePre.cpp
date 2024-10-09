/*
 * FILE: DlgCustomizePre.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "DlgCustomizePre.h"
#include "EC_CustomizeMgr.h"

#include "EC_Game.h"
#include "EC_Face.h"
#include "AUIImagePicture.h"
#include "AUIListBox.h"
#include "FaceBoneController.h"
#include "EC_Game.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "EC_LoginUIMan.h"
#include "EC_GameUIMan.h"
#include "EC_Viewport.h"

#include "DlgCustomizeEar.h"
#include "DlgCustomizeEye.h"
#include "DlgCustomizeEyebrow.h"
#include "DlgCustomizeFace.h"
#include "DlgCustomizeMouth.h"
#include "DlgCustomizeNose.h"
#include "DlgCustomizeStature.h"
#include "DlgCustomizeAdv.h"
#include "DlgCustomizeChsPre.h"
#include "DlgFaceName.h"
#include "EC_Global.h"

#include "elementdataman.h"

#include "A3DViewport.h"
#include "WindowsX.h"
#include <AFI.h>
#include <direct.h>

AUI_BEGIN_COMMAND_MAP(CDlgCustomizePre, CDlgCustomizeBase)

AUI_ON_COMMAND("Btn_SaveCustomizeData", OnCommandSaveCusData)
AUI_ON_COMMAND("Btn_DeleteCustomizeData", OnCommandDeleteCusData)
AUI_ON_COMMAND("Btn_Random", OnCommandRandomCusData)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgCustomizePre, CDlgCustomizeBase)

AUI_ON_EVENT("ListBox_PreCustomize", WM_LBUTTONUP, OnLButtonUpListBoxPreCus)
AUI_ON_EVENT("ListBox_PreCustomize", WM_KEYDOWN, OnKeyDownListBoxPreCus)

AUI_ON_EVENT("Img_*",WM_LBUTTONUP, OnLButtonUpListboxPredefined)
AUI_ON_EVENT("Scroll_*",WM_MOUSEWHEEL, OnMouseWheelListboxPredefined)
AUI_ON_EVENT("Edit_*", WM_MOUSEWHEEL, OnMouseWheelListboxPredefined)
AUI_ON_EVENT("Img_*", WM_MOUSEWHEEL, OnMouseWheelListboxPredefined)

AUI_END_EVENT_MAP()

//----------------------------------------------------------
CDlgCustomizePre::CDlgCustomizePre()
{
	m_pBtnDeleteCustomizeData = NULL;	
	m_pListBoxPreCustomize = NULL;
	m_pBtnDeleteCustomizeData = NULL;

}

//----------------------------------------------------------
CDlgCustomizePre::~CDlgCustomizePre()
{
	//释放自定义数据文件名向量
	abase::vector<char*>::iterator iterFileName;
	abase::vector<char*>::iterator iterFileNameEnd;
	iterFileName = m_vecCustomizeFileName.begin();
	iterFileNameEnd = m_vecCustomizeFileName.end();
	for( ; iterFileName != iterFileNameEnd; ++iterFileName)
	{
		if( *iterFileName != NULL)
		{
			delete[] (*iterFileName);
		}
	}
	m_vecCustomizeFileName.clear();
	
	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();

	m_vecStockFileName.clear();
	m_iLastStockFile = -1;
}

bool CDlgCustomizePre::LoadUserCustomizeData(void)
{
	
	char szUserCustomizeDataDir[MAX_PATH];
	sprintf(szUserCustomizeDataDir, "userdata\\character\\Customize\\%d%d\\",GetPlayer()->GetProfession(),GetPlayer()->GetGender());
	
	char szUserCustomizeDataFilePath[MAX_PATH];//当前人物自定义数据
	if( szUserCustomizeDataDir[strlen(szUserCustomizeDataDir)-1] == '\\')
		sprintf(szUserCustomizeDataFilePath, "%s*.ini", szUserCustomizeDataDir);
	
	_finddata_t fdIni;//文件属性
	long lFileHandle = _findfirst( szUserCustomizeDataFilePath, &fdIni);//获得第一个文件的句柄
	if( lFileHandle == -1)
	{
		return true;
	}
	
	int nResult = 0;//保存findnext的返回值
	while( nResult != -1)
	{
		//得到完整的用户自定义数据文件名
		if( szUserCustomizeDataDir[strlen(szUserCustomizeDataDir) -1] =='\\')
			sprintf( szUserCustomizeDataFilePath, "%s%s", szUserCustomizeDataDir, fdIni.name );
		char szTempName[MAX_PATH];
		sprintf(szTempName, "%s", fdIni.name);
		
		char*pEnd = strstr(szTempName, ".ini");
		if (!pEnd)
		{
			a_LogOutput(1, "CECCustomize::LoadUserCustomizeData, Strange file name %s", szTempName);
		}
		else
		{
			(*pEnd) = '\0';
			
			ACHAR szUserCustomizeDataName[MAX_PATH];
			
			int nLength = strlen(szTempName);
			int nWLength = MultiByteToWideChar(CP_ACP, 0, szTempName, nLength, szUserCustomizeDataName,nLength);
			szUserCustomizeDataName[nWLength]= _AL('\0');
			
			char* szCustiomizeFileName = new char[MAX_PATH];
			strcpy(szCustiomizeFileName,szUserCustomizeDataFilePath);
			m_vecCustomizeFileName.push_back(szCustiomizeFileName);
			
			//在listbox 开头插入
			m_pListBoxPreCustomize->InsertString(0, szUserCustomizeDataName);
			m_pListBoxPreCustomize->SetItemDataPtr(0, szCustiomizeFileName);//设置listbox数据文件指针
		}
		
		//获取下一个用户自定义数据文件的句柄
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//关闭句柄
	_findclose( lFileHandle);
	
	return true;
}

//----------------------------------------------------------
AUIStillImageButton * CDlgCustomizePre::GetSwithButton()
{
	return GetCustomizeMan()->m_pBtnChoosePreCustomize;
}

//----------------------------------------------------------
bool CDlgCustomizePre::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;
	ClearListCtrlAndA2DSprite();
	
	A2DSprite *pSpriteSelect = GetCustomizeMan()->GetImageSelected();
	A2DSprite *pSpriteMouseOn = GetCustomizeMan()->GetImageMouseOn();
	if (!m_ListBoxPredefined.Init(this, "Img_01", 3, 3, "Scroll_01", "Edit_01", pSpriteSelect, pSpriteMouseOn))
		return false;
	
	m_pListBoxPreCustomize = (AUIListBox*)GetDlgItem("ListBox_PreCustomize");
	m_pBtnDeleteCustomizeData = (AUIStillImageButton*)GetDlgItem("Btn_DeleteCustomizeData");
	if (NULL == m_pListBoxPreCustomize || NULL == m_pBtnDeleteCustomizeData)
	{
		return false;
	}
	if (!LoadUserCustomizeData())
		return false;
	
	if (!InitResource())
		return false;

	UpdateData(false);

	return true;
}

//----------------------------------------------------------
void CDlgCustomizePre::DoDataExchange(bool bSave)
{
	//ddx control
	DDX_Control("ListBox_PreCustomize", m_pListBoxPreCustomize);
	DDX_Control("Btn_DeleteCustomizeData", m_pBtnDeleteCustomizeData);

	if (!IsContextReady()) return;
	//ddx data

}

//----------------------------------------------------------
void CDlgCustomizePre::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	SetCanMove(false);

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


//----------------------------------------------------------
void CDlgCustomizePre::DoCalculate()
{

}

//----------------------------------------------------------
bool CDlgCustomizePre::InitResource()
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

			//当前人物的预设数据
			if( (nCharacterMask & uCurCharacterComboID) && (GetPlayer()->GetGender() == uCurGenderID))
			{
				int id = pCustomizedDataEssence->id;
				
				ACHAR* pszName = pCustomizedDataEssence->name;
				char* pszFile = pCustomizedDataEssence->file_data;
				if (!af_IsFileExist(pszFile)){
					continue;
				}
				m_vecStockFileName.push_back(pszFile);
				ACHAR* pszHintName = pCustomizedDataEssence->name;
				char* pszIconFile = pCustomizedDataEssence->file_icon;
				A2DSprite* pA2DIcon = NULL;
				if (*pszIconFile){
					pszIconFile += 9;//跳过"surfaces\"
					A2DSprite* pA2DIcon = new A2DSprite;
					if (!pA2DIcon->Init(GetGame()->GetA3DDevice(), pszIconFile, 0)){
						delete pA2DIcon;
						pA2DIcon = NULL;
					}else{
						m_vecA2DSprite.push_back(pA2DIcon);
						m_ListBoxPredefined.Append(id, NULL, pszHintName, pA2DIcon, true);
					}
				}
				//	没有图标的，表明策划不想显示在这里
			}

		}//if与预定义相关的数据
		
	}//遍历数据库
	
	return true;
}

//--------------------------------------------------------------------
void CDlgCustomizePre::OnCommandSaveCusData(const char* szCommand)
{
	static DWORD nLastTime = 0;
	
	DWORD nThisTime = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	if(  nThisTime - nLastTime > 1000)
	{	
		//弹出保存对话框
		CDlgFaceName *pDlg = dynamic_cast<CDlgFaceName *>(GetAUIManager()->GetDialog("Win_Facename"));
		pDlg->Show(true, true);
		nLastTime = nThisTime;
	}
}

void CDlgCustomizePre::OnCommandDeleteCusData(const char* szCommand)
{
	int nCurSel = m_pListBoxPreCustomize->GetCurSel();
	
	//判断是否为用户自定义文件(缺省文件不允许删除)
	if( nCurSel >= 0 && nCurSel < m_pListBoxPreCustomize->GetCount())
	{
		char* pFileName = (char*)m_pListBoxPreCustomize->GetItemDataPtr(nCurSel);
		
		// use af_GetBaseDir() to get the root folder		
		char szFile[MAX_PATH];
		sprintf(szFile, "%s\\%s", af_GetBaseDir(), pFileName);
		DeleteFileA(szFile);
		
		//删除当前listbox选项
		m_pListBoxPreCustomize->DeleteString(nCurSel);
		DeleteFileName(pFileName);
		
		//设置当前listbox选中
		if(nCurSel < m_pListBoxPreCustomize->GetCount())
			m_pListBoxPreCustomize->SetCurSel(nCurSel);
		else
			m_pListBoxPreCustomize->SetCurSel(m_pListBoxPreCustomize->GetCount() -1);
	}
}

void CDlgCustomizePre::OnCommandRandomCusData(const char* szCommand)
{
	int count = (int)m_vecStockFileName.size();
	if (count <= 0){
		return;
	}
	int i = glb_Random(0, count-1);
	if (i == m_iLastStockFile && count > 1){
		i = (i+1)%count;	//	确保每次随机形象不同
	}
	ApplyCustomizeData(m_vecStockFileName[i]);
	m_iLastStockFile = i;
}

void CDlgCustomizePre::ApplyCustomizeData(const char *szFile)
{
	if(!LoadCustomizeData(szFile))
	{
		int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();
		
		if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
		{
			((CECLoginUIMan*)m_pAUIManager)->MessageBox("", GetStringFromTable(2901), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			
			return;
		}
		else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
		{
			((CECGameUIMan*)m_pAUIManager)->MessageBox("", GetStringFromTable(7860), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
	}
	
	//初始化默认值
	GetCustomizeMan()->InitDefaultData();
	
	//调用各窗口的LoadDefault
	GetCustomizeMan()->m_pDlgCustomizeEye2->LoadDefault();
	GetCustomizeMan()->m_pDlgCustomizeFace2->LoadDefault();
	GetCustomizeMan()->m_pDlgCustomizeNoseMouth2->LoadDefault();
	GetCustomizeMan()->m_pDlgCustomizeStature->LoadDefault();
}


void CDlgCustomizePre::OnLButtonUpListboxPredefined(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!IsContextReady()){
		return;
	}
	{
		PAUIOBJECT pImage = m_ListBoxPredefined.OnMouse_Clicked(wParam, lParam, pObj);
		if (pImage)
		{			
			elementdataman* pElementDataMan = GetGame()->GetElementDataMan();
			ID_SPACE eIDSpace = ID_SPACE_FACE;
			DATA_TYPE dt;
			unsigned int uFaceDataID = (unsigned int)pImage->GetData();
			CUSTOMIZEDATA_ESSENCE* pCustomizedDataEssence = (CUSTOMIZEDATA_ESSENCE*)pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dt);
			char* pszFile = pCustomizedDataEssence->file_data;
			ApplyCustomizeData(pszFile);
			return;
		}
		
	}
}

void CDlgCustomizePre::OnMouseWheelListboxPredefined(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_ListBoxPredefined.OnMouse_Wheel(wParam, lParam, pObj))
		return;
}

void CDlgCustomizePre::OnLButtonUpListBoxPreCus(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//判断是否点中列表框中的选项
	POINT ptPos = pObj->GetPos();
	
	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;
	
	if(m_pListBoxPreCustomize->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
	{
		return;
	}
	
	int nCurSel = m_pListBoxPreCustomize->GetCurSel();
	if( nCurSel >= 0 && nCurSel < m_pListBoxPreCustomize->GetCount())
	{
		if( 0 == m_pListBoxPreCustomize->GetCount())
		{
			m_pBtnDeleteCustomizeData->Enable(false);
		}
		else
		{
			m_pBtnDeleteCustomizeData->Enable(true);
		}
		
		char* pszFile = (char*)m_pListBoxPreCustomize->GetItemDataPtr(nCurSel);
		ApplyCustomizeData(pszFile);
	}
	else
	{
		if (0 != m_pListBoxPreCustomize->GetCount())
		{
			m_pListBoxPreCustomize->SetCurSel(m_pListBoxPreCustomize->GetCount() - 1);
		}
	}
}

void CDlgCustomizePre::OnKeyDownListBoxPreCus(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( wParam != VK_UP && wParam != VK_DOWN)
	{
		return;
	}
	
	int nCurSel = m_pListBoxPreCustomize->GetCurSel();
	if( nCurSel >= 0 && nCurSel < m_pListBoxPreCustomize->GetCount())
	{
		if( 0 == m_pListBoxPreCustomize->GetCount())
		{
			m_pBtnDeleteCustomizeData->Enable(false);
		}
		else
		{
			m_pBtnDeleteCustomizeData->Enable(true);
		}
		
		char* pszFile = (char*)m_pListBoxPreCustomize->GetItemDataPtr(nCurSel);
		ApplyCustomizeData(pszFile);		
	}
}

//---------------------------------------------------------------------------
// 载入形象的个性化数据

bool CDlgCustomizePre::LoadCustomizeData(const char* pszFile)
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
	if(!MakeSureCustomizeData(DT_FACE_THIRDEYE_ESSENCE, data.faceData.idThirdEye))
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
bool CDlgCustomizePre::SaveUserCustomizeData(const wchar_t *szInputName)
{
	CECPlayer::PLAYER_CUSTOMIZEDATA playerCustomizeData = GetPlayer()->GetCustomizeData();
	playerCustomizeData.faceData = *(GetFace()->GetFaceData());

	GetPlayer()->ChangeCustomizeData(playerCustomizeData);
	const CECFace::FACE_CUSTOMIZEDATA& faceData = playerCustomizeData.faceData;

	char szTempName[MAX_PATH];
	int iLen = WideCharToMultiByte(CP_ACP, 0, szInputName, wcslen(szInputName), szTempName, MAX_PATH, NULL, NULL);
	szTempName[iLen] = '\0';

	// use af_GetBaseDir() to get the root folder
	char szPath[MAX_PATH];
	sprintf(szPath, "%s\\userdata\\character\\Customize", af_GetBaseDir());
	_mkdir(szPath);
	
	char szPath2[MAX_PATH];
	sprintf(szPath2, "%s\\%d%d", szPath, GetPlayer()->GetProfession(),GetPlayer()->GetGender());
	_mkdir(szPath2);

	AIniFile IniFile;
	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\%s.ini", szPath2, szTempName);

	char* szCustiomizeFileName = new char[MAX_PATH];
	sprintf(szCustiomizeFileName, "userdata\\character\\Customize\\%d%d\\%s.ini",GetPlayer()->GetProfession(),GetPlayer()->GetGender(), szTempName);

	//压入向量
	if (!CheckFileName(szCustiomizeFileName))
	{
		((CECLoginUIMan*)m_pAUIManager)->MessageBox("", GetStringFromTable(2900), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return false;	
	}
	else
		m_vecCustomizeFileName.push_back(szCustiomizeFileName);
	
	IniFile.Open(szFile);

	//3庭3
	AString strSection = "3Parts";
	IniFile.WriteIntValue(strSection, "scaleUp", faceData.scaleUp);
	IniFile.WriteIntValue(strSection, "scaleMiddle", faceData.scaleMiddle);
	IniFile.WriteIntValue(strSection, "scaleDown", faceData.scaleDown);

	//脸型融合3
	strSection = "BlendFace";
	IniFile.WriteIntValue(strSection, "idFaceShape1", faceData.idFaceShape1);
	IniFile.WriteIntValue(strSection, "idFaceShape2", faceData.idFaceShape2);
	IniFile.WriteIntValue(strSection, "blendFaceShape", faceData.blendFaceShape);

	//脸盘调整3
	strSection = "Face";
	IniFile.WriteIntValue(strSection, "scaleFaceH", faceData.scaleFaceH);
	IniFile.WriteIntValue(strSection, "scaleFaceV", faceData.scaleFaceV);
	IniFile.WriteIntValue(strSection, "idFaceTex", faceData.idFaceTex);

	//法令
	strSection = "Faling";
	IniFile.WriteIntValue(strSection, "idFalingSkin", faceData.idFalingSkin);
	IniFile.WriteIntValue(strSection, "idFalingTex", faceData.idFalingTex);

	//额头5
	strSection = "Forehead";
	IniFile.WriteIntValue(strSection, "offsetForeheadH", faceData.offsetForeheadH);
	IniFile.WriteIntValue(strSection, "offsetForeheadV", faceData.offsetForeheadV);
	IniFile.WriteIntValue(strSection, "offsetForeheadZ", faceData.offsetForeheadZ);
	IniFile.WriteIntValue(strSection, "rotateForehead", faceData.rotateForehead);
	IniFile.WriteIntValue(strSection, "scaleForehead", faceData.scaleForehead);

	//颧骨5
	strSection = "YokeBone";
	IniFile.WriteIntValue(strSection, "offsetYokeBoneH", faceData.offsetYokeBoneH);
	IniFile.WriteIntValue(strSection, "offsetYokeBoneV", faceData.offsetYokeBoneV);
	IniFile.WriteIntValue(strSection, "offsetYokeBoneZ", faceData.offsetYokeBoneZ);
	IniFile.WriteIntValue(strSection, "rotateYokeBone", faceData.rotateYokeBone);
	IniFile.WriteIntValue(strSection, "scaleYokeBone", faceData.scaleYokeBone);
	
	//脸颊4
	strSection = "Cheek";
	IniFile.WriteIntValue(strSection, "offsetCheekH", faceData.offsetCheekH);
	IniFile.WriteIntValue(strSection, "offsetCheekV", faceData.offsetCheekV);
	IniFile.WriteIntValue(strSection, "offsetCheekZ", faceData.offsetCheekZ);
	IniFile.WriteIntValue(strSection, "scaleCheek", faceData.scaleCheek);

	//下巴4
	strSection = "Chain";
	IniFile.WriteIntValue(strSection, "offsetChainV", faceData.offsetChainV);
	IniFile.WriteIntValue(strSection, "offsetChainZ", faceData.offsetChainZ);
	IniFile.WriteIntValue(strSection, "rotateChain", faceData.rotateChain);
	IniFile.WriteIntValue(strSection, "scaleChainH", faceData.scaleChainH);

	//颌骨6
	strSection = "Jaw";
	IniFile.WriteIntValue(strSection, "offsetJawH", faceData.offsetJawH);
	IniFile.WriteIntValue(strSection, "offsetJawV", faceData.offsetJawV);
	IniFile.WriteIntValue(strSection, "offsetJawZ", faceData.offsetJawZ);
	IniFile.WriteIntValue(strSection, "scaleJawSpecial", faceData.scaleJawSpecial);
	IniFile.WriteIntValue(strSection, "scaleJawH", faceData.scaleJawH);
	IniFile.WriteIntValue(strSection, "scaleJawV", faceData.scaleJawV);

	//眼睛18
	strSection = "Eye";
	IniFile.WriteIntValue(strSection, "idThirdEye", faceData.idThirdEye);
	IniFile.WriteIntValue(strSection, "idEyeBaseTex", faceData.idEyeBaseTex);
	IniFile.WriteIntValue(strSection, "idEyeHighTex", faceData.idEyeHighTex);
	IniFile.WriteIntValue(strSection, "idEyeBallTex", faceData.idEyeBallTex);
	IniFile.WriteIntValue(strSection, "idEyeShape", faceData.idEyeShape);
	IniFile.WriteIntValue(strSection, "scaleEyeH", faceData.scaleEyeH);
	IniFile.WriteIntValue(strSection, "scaleEyeV", faceData.scaleEyeV);
	IniFile.WriteIntValue(strSection, "rotateEye", faceData.rotateEye);
	IniFile.WriteIntValue(strSection, "offsetEyeH", faceData.offsetEyeH);
	IniFile.WriteIntValue(strSection, "offsetEyeV", faceData.offsetEyeV);
	IniFile.WriteIntValue(strSection, "offseteyeZ", faceData.offsetEyeZ);
	IniFile.WriteIntValue(strSection, "scaleEyeBall", faceData.scaleEyeBall);

	IniFile.WriteIntValue(strSection, "scaleEyeH2", faceData.scaleEyeH2);
	IniFile.WriteIntValue(strSection, "scaleEyeV2", faceData.scaleEyeV2);
	IniFile.WriteIntValue(strSection, "rotateEye2", faceData.rotateEye2);
	IniFile.WriteIntValue(strSection, "offsetEyeH2", faceData.offsetEyeH2);
	IniFile.WriteIntValue(strSection, "offsetEyeV2", faceData.offsetEyeV2);
	IniFile.WriteIntValue(strSection, "offseteyeZ2", faceData.offsetEyeZ2);
	IniFile.WriteIntValue(strSection, "scaleEyeBall2", faceData.scaleEyeBall2);	
	
	//眉毛8
	strSection = "Brow";
	IniFile.WriteIntValue(strSection, "idBrowTex", faceData.idBrowTex);
	IniFile.WriteIntValue(strSection, "idBrowShape", faceData.idBrowShape);
	IniFile.WriteIntValue(strSection, "scaleBrowH", faceData.scaleBrowH);
	IniFile.WriteIntValue(strSection, "scaleBrowV", faceData.scaleBrowV);
	IniFile.WriteIntValue(strSection, "rotateBrow", faceData.rotateBrow);
	IniFile.WriteIntValue(strSection, "offsetBrowH", faceData.offsetBrowH);
	IniFile.WriteIntValue(strSection, "offsetBrowV", faceData.offsetBrowV);
	IniFile.WriteIntValue(strSection, "offsetBrowZ", faceData.offsetBrowZ);

	IniFile.WriteIntValue(strSection, "scaleBrowH2", faceData.scaleBrowH2);
	IniFile.WriteIntValue(strSection, "scaleBrowV2", faceData.scaleBrowV2);
	IniFile.WriteIntValue(strSection, "rotateBrow2", faceData.rotateBrow2);
	IniFile.WriteIntValue(strSection, "offsetBrowH2", faceData.offsetBrowH2);
	IniFile.WriteIntValue(strSection, "offsetBrowV2", faceData.offsetBrowV2);
	IniFile.WriteIntValue(strSection, "offsetBrowZ2", faceData.offsetBrowZ2);

	//鼻子8
	strSection = "Nose";
	IniFile.WriteIntValue(strSection, "idNoseTex", faceData.idNoseTex);
	IniFile.WriteIntValue(strSection, "idNoseTipShape", faceData.idNoseTipShape);
	IniFile.WriteIntValue(strSection, "scaleNoseTipH", faceData.scaleNoseTipH);
	IniFile.WriteIntValue(strSection, "scaleNoseTipV", faceData.scaleNoseTipV);
	IniFile.WriteIntValue(strSection, "scaleNoseTipZ", faceData.scaleNoseTipZ);
	IniFile.WriteIntValue(strSection, "offsetNoseTipV", faceData.offsetNoseTipV);
	IniFile.WriteIntValue(strSection, "idNoseBridgeShape", faceData.idNoseBridgeShape);
	IniFile.WriteIntValue(strSection, "scaleBridgeTipH", faceData.scaleBridgeTipH);
	IniFile.WriteIntValue(strSection, "offsetBridgeTipZ", faceData.offsetBridgeTipZ);

	//嘴9
	strSection = "Mouth";
	IniFile.WriteIntValue (strSection, "idMouthUpLipLine", faceData.idMouthUpLipLine);
	IniFile.WriteIntValue(strSection, "idMouthMidLipLine", faceData.idMouthMidLipLine);
	IniFile.WriteIntValue(strSection, "idMouthDownLipLine", faceData.idMouthDownLipLine);
	IniFile.WriteIntValue(strSection, "thickUpLip", faceData.thickUpLip);
	IniFile.WriteIntValue(strSection, "thickDownLip", faceData.thickDownLip);
	IniFile.WriteIntValue(strSection, "scaleMouthH", faceData.scaleMouthH);
	IniFile.WriteIntValue(strSection, "offsetMouthV", faceData.offsetMouthV);
	IniFile.WriteIntValue(strSection, "offsetMOuthZ", faceData.offsetMouthZ);
	IniFile.WriteIntValue(strSection, "idMouthTex", faceData.idMouthTex);
	IniFile.WriteIntValue(strSection, "offsetCornerOfMouthSpecial", faceData.offsetCornerOfMouthSpecial);
	IniFile.WriteIntValue(strSection, "scaleMouthH2", faceData.scaleMouthH2);
	IniFile.WriteIntValue(strSection, "offsetCornerOfMouthSpecial2", faceData.offsetCornerOfMouthSpecial2);

	//耳朵3
	strSection = "Ear";
	IniFile.WriteIntValue(strSection, "idEarShape", faceData.idEarShape);
	IniFile.WriteIntValue(strSection, "scaleEar", faceData.scaleEar);
	IniFile.WriteIntValue(strSection, "offsetEarV", faceData.offsetEarV);
	
	//头发1
	strSection = "Hair";
	IniFile.WriteIntValue(strSection, "idHairModel", faceData.idHairModel);
	IniFile.WriteIntValue(strSection, "idHairTex", faceData.idHairTex);

	//胡子2
	strSection = "Moustache";
	IniFile.WriteIntValue(strSection, "idMoustacheTex", faceData.idMoustacheTex);
	IniFile.WriteIntValue(strSection, "idMoustacheSkin", faceData.idMoustacheSkin);
	IniFile.WriteIntValue(strSection, "idGoateeTex", faceData.idGoateeTex);

	//各部分颜色7
	strSection = "Color";
	IniFile.WriteIntValue(strSection, "colorHair", faceData.colorHair);
	IniFile.WriteIntValue(strSection, "colorFace", faceData.colorFace);
	IniFile.WriteIntValue(strSection, "colorEye", faceData.colorEye);
	IniFile.WriteIntValue(strSection, "colorBrow", faceData.colorBrow);
	IniFile.WriteIntValue(strSection, "colorMouth", faceData.colorMouth);
	IniFile.WriteIntValue(strSection, "colorEyeBall", faceData.colorEyeBall);
	IniFile.WriteIntValue(strSection, "colorMoustache", faceData.colorMoustache);

	//身体参数
	strSection = "Body";
	IniFile.WriteIntValue(strSection, "bodyID", playerCustomizeData.bodyID);
	IniFile.WriteIntValue(strSection, "colorBody", playerCustomizeData.colorBody);
	IniFile.WriteIntValue(strSection, "headScale", playerCustomizeData.headScale);
	IniFile.WriteIntValue(strSection, "upScale", playerCustomizeData.upScale);
	IniFile.WriteIntValue(strSection, "waistScale", playerCustomizeData.waistScale);
	IniFile.WriteIntValue(strSection, "armWidth", playerCustomizeData.armWidth);
	IniFile.WriteIntValue(strSection, "legWidth", playerCustomizeData.legWidth);
	IniFile.WriteIntValue(strSection, "breastScale", playerCustomizeData.breastScale);

	IniFile.Save(szFile);
	IniFile.Close();

	//把文件名插入预设数组
	ACHAR szCustomizeName[MAX_PATH];
	wcscpy(szCustomizeName, szInputName);

	//在列表开头加入
	m_pListBoxPreCustomize->InsertString(0, szCustomizeName);
	m_pListBoxPreCustomize->SetItemDataPtr(0,szCustiomizeFileName);
	m_pListBoxPreCustomize->SetCurSel(0);

	return true;
}

bool CDlgCustomizePre::CheckFileName(char* szFileName)
{
	//释放自定义数据文件名向量
	abase::vector<char*>::iterator iterFileName;
	abase::vector<char*>::iterator iterFileNameEnd;
	iterFileName = m_vecCustomizeFileName.begin();
	iterFileNameEnd = m_vecCustomizeFileName.end();
	for( ; iterFileName != iterFileNameEnd; ++iterFileName)
	{
		if( 0 == stricmp(szFileName, *iterFileName))
		{
			return false;
		}
	}
	return true;
}

void CDlgCustomizePre::DeleteFileName(char* szFileName)
{
	abase::vector<char*>::iterator iterFileName;
	abase::vector<char*>::iterator iterFileNameEnd;
	iterFileName = m_vecCustomizeFileName.begin();
	iterFileNameEnd = m_vecCustomizeFileName.end();
	for( ; iterFileName != iterFileNameEnd; ++iterFileName)
	{
		if( 0 == stricmp(szFileName, *iterFileName))
		{
			delete [] (*iterFileName);
			m_vecCustomizeFileName.erase(iterFileName);
			return;
		}
	}
}

//确认个性化数据是否合法
//形状part:0-脸型、1-眼型、2-眉型、3-鼻头、4-鼻梁、5-上唇线、6-唇沟、7-下唇线、8-耳型
//纹理part: 0-脸、1-眼皮、2-眼影、3-眉、4-唇、5-鼻、6-眼珠、7-小胡子、8-头发、9-大胡子、10-法令
bool CDlgCustomizePre::MakeSureCustomizeData( DATA_TYPE dt, unsigned int id, unsigned int part)
{
	//妖兽、巫师、刺客不判断
	if( GetPlayer()->GetProfession() == PROF_ORC ||
		GetPlayer()->GetProfession() == PROF_MONK ||
		GetPlayer()->GetProfession() == PROF_GHOST)
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
	//	附眼
	if( dt == DT_FACE_THIRDEYE_ESSENCE)
	{
		// only allowed the third eye in these professions
		if( GetPlayer()->GetProfession() != PROF_MEILING &&
			GetPlayer()->GetProfession() != PROF_JIANLING )
		{
			return id == 0;
		}
		
		FACE_THIRDEYE_ESSENCE* pEssence = (FACE_THIRDEYE_ESSENCE*)pElementDataMan->get_data_ptr(id, ID_SPACE_FACE,dtFaceData);
		
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


	return true;
}

void CDlgCustomizePre::ClearListCtrlAndA2DSprite()
{
	if(m_pListBoxPreCustomize)
		m_pListBoxPreCustomize->ResetContent();

	m_ListBoxPredefined.ResetContent();
	
	//释放自定义数据文件名向量
	abase::vector<char*>::iterator iterFileName;
	abase::vector<char*>::iterator iterFileNameEnd;
	iterFileName = m_vecCustomizeFileName.begin();
	iterFileNameEnd = m_vecCustomizeFileName.end();
	for( ; iterFileName != iterFileNameEnd; ++iterFileName)
	{
		if( *iterFileName != NULL)
		{
			delete[] (*iterFileName);
		}
	}
	m_vecCustomizeFileName.clear();
	
	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();

	m_vecStockFileName.clear();
	m_iLastStockFile = -1;
}