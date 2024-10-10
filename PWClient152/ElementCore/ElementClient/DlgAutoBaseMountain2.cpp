/*
 * FILE: DlgAutoBaseMountain2.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoBaseMountain2.h"
#include "DlgAutoAllRes.h"
#include "DlgAutoMountain2.h"
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoChooseTexture.h"

#include "EC_HomeDlgsMgr.h"
#include "aui\\AUIListBox.h"
#include "aui\\AUIManager.h"
#include "AUI\\AUIStillImageButton.h"


#include "AutoTerrainCommon.h"

#include "A3DEngine.h"
#include "A3DViewport.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoBuildingOperation.h"
#include "AutoSelectedArea.h"
#include "AutoTerrainAction.h"


#include "windowsx.h"

#include "A3DMacros.h"
#include "io.h"


AUI_BEGIN_COMMAND_MAP(CDlgAutoBaseMountain2, CDlgHomeBase)

AUI_ON_COMMAND("CreateMountainLine", OnCommandCreateMountainLine)
AUI_ON_COMMAND("Advance", OnCommandAdvance)
AUI_ON_COMMAND("DelHillLine", OnCommandDelPre)

AUI_ON_COMMAND("ApplyTex", OnCommandApplyTex)
AUI_ON_COMMAND("AdvTex", OnCommandAdvTex)
AUI_ON_COMMAND("DelTex", OnCommandDelTex)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoBaseMountain2, CDlgHomeBase)

AUI_ON_EVENT("ListBox_Mountain2", WM_LBUTTONUP, OnLButtonUpListBoxHillLine)
AUI_ON_EVENT("ListBox_Texture", WM_LBUTTONUP, OnLButtonUpListBoxTex)

AUI_END_EVENT_MAP()

CDlgAutoBaseMountain2::CDlgAutoBaseMountain2()
{
	m_pListBoxTexture = NULL;
	m_pListBoxMountain2 = NULL;
	m_nMountain2Idx = 0;
	m_nTextureIdx = 0;

	

	m_iPreHillLineNum = 0;
	m_iPreTexNum = 0;

	for(int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
}

CDlgAutoBaseMountain2::~CDlgAutoBaseMountain2()
{
}
	
AUIStillImageButton * CDlgAutoBaseMountain2::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnBaseMountain2;
}

void CDlgAutoBaseMountain2::OnShowDialog()
{
	SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoBaseMountain2::OnInitContext()
{
	if(!InitResource())
		return false;

	
	LoadDefault();
	UpdateData(false);
	
	return true;
}

#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);
void CDlgAutoBaseMountain2::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_Mountain2", m_pListBoxMountain2);
	DDX_Control("ListBox_Texture", m_pListBoxTexture);
	DDX_ListBox(bSave, "ListBox_Mountain2", m_nMountain2Idx);
	DDX_ListBox(bSave, "ListBox_Texture", m_nTextureIdx);


	GET_BTN_POINTER(BTN_CREATE_HILL_LINE);
	GET_BTN_POINTER(BTN_ADV_HILL_LINE);
	GET_BTN_POINTER(BTN_DEL_HILL_LINE);

	GET_BTN_POINTER(BTN_APPLY_TEX);
	GET_BTN_POINTER(BTN_ADV_TEX);
	GET_BTN_POINTER(BTN_DEL_TEX);
	
	
}
void CDlgAutoBaseMountain2::LoadDefault()
{
	m_nMountain2Idx = 0;
	m_nTextureIdx = 0;

	m_pBtn[BTN_CREATE_HILL_LINE]->Enable(true);
	m_pBtn[BTN_ADV_HILL_LINE]->Enable(true);
	m_pBtn[BTN_DEL_HILL_LINE]->Enable(false);

	m_pBtn[BTN_APPLY_TEX]->Enable(false);
	m_pBtn[BTN_ADV_TEX]->Enable(false);
	m_pBtn[BTN_DEL_TEX]->Enable(false);
}
bool CDlgAutoBaseMountain2::InitResource()
{

	FillMountain2ListBox();
	FillTextureListBox();
	return true;
}
void CDlgAutoBaseMountain2::FillMountain2ListBox()
{
	m_pListBoxMountain2->ResetContent();
	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetHillLineSet();

	//预设数据
	if( pInfo)
	{
		 m_iPreHillLineNum = GetAutoParam()->GetNumHillLineSet();
		for( int i = 0; i < m_iPreHillLineNum ; i++)
		{
			m_pListBoxMountain2->InsertString(i, AS2AC(pInfo[i].strName));
		}
	}

	//自定义数据
	_finddata_t fdIni;//文件属性
	long lFileHandle = _findfirst( "userdata\\home\\hillline\\*.*", &fdIni);//获得第一个文件的句柄
	if( lFileHandle == -1)
	{
		return ;
	}

	int nResult = 0;//保存findnext的返回值
	while( nResult != -1)
	{
		//如果是当前是目录文件,则跳过
		if( fdIni.attrib & 0x10)//16表示目录(0x10)
		{
			nResult = _findnext( lFileHandle, &fdIni);
			continue;
		}
	
		//在listbox 
		m_pListBoxMountain2->InsertString(m_pListBoxMountain2->GetCount(), AS2AC(fdIni.name));
		
		//获取下一个用户自定义数据文件的句柄
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//关闭句柄
	_findclose( lFileHandle);

}
void CDlgAutoBaseMountain2::FillTextureListBox()
{
	m_pListBoxTexture->ResetContent();
	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetTextureSet();

	//预设数据
	if( pInfo)
	{
		m_iPreTexNum = GetAutoParam()->GetNumTextureSet();
		for( int i = 0; i < m_iPreTexNum ; i++)
		{
			m_pListBoxTexture->InsertString(i, AS2AC(pInfo[i].strName));
		}
	}

	//自定义数据
	_finddata_t fdIni;//文件属性
	long lFileHandle = _findfirst( "userdata\\home\\hilltex\\*.*", &fdIni);//获得第一个文件的句柄
	if( lFileHandle == -1)
	{
		return ;
	}

	int nResult = 0;//保存findnext的返回值
	while( nResult != -1)
	{
		//如果是当前是目录文件,则跳过
		if( fdIni.attrib & 0x10)//16表示目录(0x10)
		{
			nResult = _findnext( lFileHandle, &fdIni);
			continue;
		}
	
		//在listbox 
		m_pListBoxTexture->InsertString(m_pListBoxTexture->GetCount(), AS2AC(fdIni.name));
		
		//获取下一个用户自定义数据文件的句柄
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//关闭句柄
	_findclose( lFileHandle);
}


bool CDlgAutoBaseMountain2::GetTextureParameter(int a_nIdx, CAutoParameters::TEXTUREINFO* a_pTexInfo)
{
	UpdateData(true);
	
	if( !IsValidIdx(a_nIdx, m_pListBoxTexture))
		return false;

	if( a_nIdx < m_iPreTexNum)
	{
		CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetTextureSet();
		if( !GetAutoParam()->LoadTextureInfo(pInfo[a_nIdx].strPath, a_pTexInfo))
			return false;

	}	
	else //用户数据
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName, "userdata\\home\\hilltex\\");
		strcat(szFileName, AC2AS(m_pListBoxTexture->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadTextureInfo(szFileName, a_pTexInfo))
			return false;
	}

	return true;
}

bool CDlgAutoBaseMountain2::GetMountainLineParameter(int a_nIdx, CAutoParameters::HILLINFO* a_pMounLineInfo)
{
	if( !IsValidIdx(a_nIdx, m_pListBoxMountain2))
		return false;

	//预设数据
	if( a_nIdx < m_iPreHillLineNum)
	{
		CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetHillLineSet();
		if( !GetAutoParam()->LoadHillInfo(pInfo[a_nIdx].strPath, a_pMounLineInfo))
			return false;

	}	
	else //用户数据
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName, "userdata\\home\\hillline\\");
		strcat(szFileName, AC2AS(m_pListBoxMountain2->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadHillInfo(szFileName, a_pMounLineInfo))
			return false;
	}

	return true;
}

void CDlgAutoBaseMountain2::OnCommandCreateMountainLine(const char* szCommand)
{
	UpdateData(true);

	if (GetOperationType() != OT_HILL_LINE)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

		return;
	}

	//test begine
	PROFILE_BEGIN();

	CAutoScene* pScene = GetAutoScene();
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	CAutoHillLine* pAutoHillLine = pScene->GetAutoHillLine();
	if (NULL == pAutoHillLine)
		return;

	//Add Points
	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumLineEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetLineEditPoints();
	
	
	pAutoHillLine->SetPoints(nNum, pPoints);

	if (!pAutoHillLine->IsPointsValid())
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

		return;
	}
	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();


	CAutoParameters::HILLINFO mounLineInfo;
	if( !GetMountainLineParameter(m_nMountain2Idx, &mounLineInfo))
	{
		AutoHomeReport("CDlgAutoBaseMountain2::OnCommandCreateMountainLine, Failed to Get MountainLineParameter!");
		return;
	}
	
	// Store action
	if( GetHomeDlgsMgr()->GetModifying())
	{
		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		if (!pAction->ChangeActionHillLine(
				mounLineInfo.fMaxHeightHill,
				mounLineInfo.fLineHillWidth,
				mounLineInfo.fMainSmoothHill,
				mounLineInfo.fPartSmoothHill,
				mounLineInfo.dwHillSeed,
				mounLineInfo.nNumHeightCurve,
				mounLineInfo.pHeightCurve,
				mounLineInfo.nNumLineCurve,
				mounLineInfo.pLineCurve,
				mounLineInfo.bHillUp,
				pAutoHillLine->GetNumPoints(), 
				pAutoHillLine->GetPoints(),
				mounLineInfo.fTextureRatio,
				mounLineInfo.fMainTexture,
				mounLineInfo.fPatchTexture,
				mounLineInfo.fPatchDivision,
				mounLineInfo.fBridgeScope,
				mounLineInfo.fBorderErode, nIdx))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}

		GetAutoScene()->RefreshAllHillActions();
	}
	else 
	{
		if (!pAutoHillLine->CreateHillLine(
			mounLineInfo.fMaxHeightHill,
			mounLineInfo.fLineHillWidth,
			mounLineInfo.fMainSmoothHill,
			mounLineInfo.fPartSmoothHill,
			mounLineInfo.dwHillSeed,
			mounLineInfo.nNumHeightCurve,
			mounLineInfo.pHeightCurve,
			mounLineInfo.nNumLineCurve,
			mounLineInfo.pLineCurve,
			mounLineInfo.bHillUp,
			mounLineInfo.fBorderErode))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}	

		// Render top
		if (mounLineInfo.bHillUp)
		{
			if (!pAutoHillLine->RenderHillTop(
					mounLineInfo.fTextureRatio, 
					mounLineInfo.fMainTexture,
					mounLineInfo.fPatchTexture,
					mounLineInfo.fPatchDivision,
					mounLineInfo.fBridgeScope))
			{
				GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
		}


		if (!pAction->AddActionHillLine(
				mounLineInfo.fMaxHeightHill,
				mounLineInfo.fLineHillWidth,
				mounLineInfo.fMainSmoothHill,
				mounLineInfo.fPartSmoothHill,
				mounLineInfo.dwHillSeed,
				mounLineInfo.nNumHeightCurve,
				mounLineInfo.pHeightCurve,
				mounLineInfo.nNumLineCurve,
				mounLineInfo.pLineCurve,
				mounLineInfo.bHillUp,
				pAutoHillLine->GetNumPoints(), 
				pAutoHillLine->GetPoints(),
				mounLineInfo.fTextureRatio,
				mounLineInfo.fMainTexture,
				mounLineInfo.fPatchTexture,
				mounLineInfo.fPatchDivision,
				mounLineInfo.fBridgeScope,
				mounLineInfo.fBorderErode))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
	
	}

	GetAutoScene()->RecreateTerrainRender(true);

	// Update buildings
	pScene->GetAutoTerrain()->Tick(0);// Update terrain height
	CAutoBuildingOperation* pBuildingOperation = pScene->GetAutoBuildingOperation();
	pBuildingOperation->AdjustModelsAfterTerrainChange();

	//test end
	PROFILE_END();
	

	pSelArea->CopyPointsFromLineEditToView();
	pSelArea->SetViewAreaClosed(false);
	pSelArea->DeleteAllLineEditPoints();

	if( !GetHomeDlgsMgr()->GetModifying())
	{
		//Add Item to TerrainRes Tree View
		ACHAR szName[100];
		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1008), m_nActionCount );
		m_nActionCount++;
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_MOUNTAIN_LINE, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnModifyToCreate();
		
	}

	//设置纹理操作可用
	SetTexOpEnable(true);

	return ;
}
	


void CDlgAutoBaseMountain2::SetBtnCreateToModify()
{
	m_pBtn[BTN_CREATE_HILL_LINE]->SetText(GetStringFromTable(1006) );
	return;
}

void CDlgAutoBaseMountain2::SetBtnModifyToCreate()
{
	m_pBtn[BTN_CREATE_HILL_LINE]->SetText( GetStringFromTable(1007) );
	return;
}

void CDlgAutoBaseMountain2::InsertHillLineSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxMountain2->GetCount();
	m_pListBoxMountain2->InsertString(iIdx, szFileName);
	
}

void CDlgAutoBaseMountain2::OnCommandAdvance(const char* szCommand)
{
	//this->Show(false);
	
	UpdateData(true);
	GetHomeDlgsMgr()->m_pDlgAutoMountain2->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

	CAutoParameters::HILLINFO mounLineInfo;
	if( !GetMountainLineParameter(m_nMountain2Idx, &mounLineInfo))
	{
		AutoHomeReport("CDlgAutoBaseMountain2::OnCommandAdvance, Failed to Get MountainLineParameter!");
		return;
	}

	//预设数据
	if( m_nMountain2Idx < m_iPreHillLineNum)
	{
		GetHomeDlgsMgr()->m_pDlgAutoMountain2->InitSettings(mounLineInfo,
													false);
	}
	else //自定义数据
	{
		GetHomeDlgsMgr()->m_pDlgAutoMountain2->InitSettings(mounLineInfo,
													true, 
													AC2AS(m_pListBoxMountain2->GetText(m_nMountain2Idx)));
	}
}


void CDlgAutoBaseMountain2::OnCommandDelPre(const char* szCommand)
{
	UpdateData(true);

	if(m_nMountain2Idx < m_iPreHillLineNum || m_nMountain2Idx >= m_pListBoxMountain2->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\hillline\\"));
	a_strcat(szFileName, m_pListBoxMountain2->GetText(m_nMountain2Idx));

	//删除当前文件
	DeleteFile(szFileName);
	

	//删除当前listbox选项
	m_pListBoxMountain2->DeleteString(m_nMountain2Idx);
	
	//设置当前listbox选中
	if(m_nMountain2Idx < m_pListBoxMountain2->GetCount())
		m_pListBoxMountain2->SetCurSel(m_nMountain2Idx);
	else
	{
		m_nMountain2Idx = m_pListBoxMountain2->GetCount() -1;

		m_pListBoxMountain2->SetCurSel(m_nMountain2Idx);
		if( m_nMountain2Idx == m_iPreHillLineNum -1)
		{
			m_pBtn[BTN_DEL_HILL_LINE]->Enable(false);
		}
	}
	UpdateData(false);

}

void CDlgAutoBaseMountain2::OnLButtonUpListBoxHillLine(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//判断是否点中列表框中的选项
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxMountain2->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxMountain2->GetCurSel();

	if( nCurSel >= 0 && nCurSel < m_iPreHillLineNum) // 预设
	{
		m_pBtn[BTN_CREATE_HILL_LINE]->Enable(true);
		m_pBtn[BTN_ADV_HILL_LINE]->Enable(true);
		m_pBtn[BTN_DEL_HILL_LINE]->Enable(false);
		
	}
	else if( nCurSel < m_pListBoxMountain2->GetCount()) //自定义
	{
		m_pBtn[BTN_CREATE_HILL_LINE]->Enable(true);
		m_pBtn[BTN_ADV_HILL_LINE]->Enable(true);
		m_pBtn[BTN_DEL_HILL_LINE]->Enable(true);
	}
	else //点在空位置											
	{
		m_pBtn[BTN_CREATE_HILL_LINE]->Enable(false);
		m_pBtn[BTN_ADV_HILL_LINE]->Enable(false);
		m_pBtn[BTN_DEL_HILL_LINE]->Enable(false);
	}

	return;
}

void CDlgAutoBaseMountain2::OnCommandApplyTex(const char* szCommand)
{
	UpdateData(true);

	CAutoScene* pScene = GetAutoScene();
	
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	
	CAutoParameters::TEXTUREINFO texInfo;

	if(!GetTextureParameter(m_nTextureIdx, &texInfo))
		AutoHomeReport("CDlgAutoBaseTerrain::OnCommandCreateTexture, Can not Get Texture Parameter!");

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	
	pTexture->ChangeTextureParam(
		TT_BRIDGE,
		TT_BRIDGE_PATCH,
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);


	pAction->AddActionHillTextureInfo(
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);

	if (!pScene->RecreateTerrainRender(true))
	{
		AutoHomeReport( "DlgAutoTerrain::OnCommandCreateTexture, Failed to recreate terrain render!");
		return;
	}
}

void CDlgAutoBaseMountain2::OnCommandDelTex(const char* szCommand)
{
	UpdateData(true);

	if(m_nTextureIdx < m_iPreTexNum || m_nTextureIdx >= m_pListBoxTexture->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\hilltex\\"));
	a_strcat(szFileName, m_pListBoxTexture->GetText(m_nTextureIdx));

	//删除当前文件
	DeleteFile(szFileName);
	

	//删除当前listbox选项
	m_pListBoxTexture->DeleteString(m_nTextureIdx);
	
	//设置当前listbox选中
	if(m_nTextureIdx < m_pListBoxTexture->GetCount())
		m_pListBoxTexture->SetCurSel(m_nTextureIdx);
	else
	{
		m_nTextureIdx = m_pListBoxTexture->GetCount() -1;

		m_pListBoxTexture->SetCurSel(m_nTextureIdx);
		if( m_nTextureIdx == m_iPreTexNum -1)
		{
			m_pBtn[BTN_DEL_TEX]->Enable(false);
		}
	}
	UpdateData(false);
}

void CDlgAutoBaseMountain2::InsertTexSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxTexture->GetCount();
	m_pListBoxTexture->InsertString(iIdx, szFileName);
	
}
void CDlgAutoBaseMountain2::OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//判断是否点中列表框中的选项
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxTexture->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxTexture->GetCurSel();

	if( nCurSel >= 0 && nCurSel < m_iPreTexNum) // 预设
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(true);
		m_pBtn[BTN_ADV_TEX]->Enable(true);
		m_pBtn[BTN_DEL_TEX]->Enable(false);
		
	}
	else if( nCurSel < m_pListBoxTexture->GetCount()) //自定义
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(true);
		m_pBtn[BTN_ADV_TEX]->Enable(true);
		m_pBtn[BTN_DEL_TEX]->Enable(true);
	}
	else //点在空位置											
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(false);
		m_pBtn[BTN_ADV_TEX]->Enable(false);
		m_pBtn[BTN_DEL_TEX]->Enable(false);
	}

	return;
}

void CDlgAutoBaseMountain2::OnCommandAdvTex(const char* szCommand)
{
	UpdateData(true);
	
	
	CAutoParameters::TEXTUREINFO texInfo;
	if( !GetTextureParameter(m_nTextureIdx, &texInfo))
			AutoHomeReport("CDlgAutoBaseMountain2::OnCommandAdvTex, Can not Get TextureParameter!");

	TextureParamInfo_t texParam;
	GetHomeDlgsMgr()->TransformTexInfoToTexParam(texInfo, texParam);

		//预设数据
	if( m_nTextureIdx < m_iPreTexNum)
	{
	
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_HILL_LINE);
	}
	else //自定义数据
	{
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_HILL_LINE,
													true, 
													AC2AS(m_pListBoxTexture->GetText(m_nTextureIdx)));
	}

	GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

}

void CDlgAutoBaseMountain2::SetTexOpEnable(bool i_bEnable)
{
	m_pBtn[BTN_ADV_TEX]->Enable(i_bEnable);
	m_pBtn[BTN_APPLY_TEX]->Enable(i_bEnable);
}
*/