/*
 * FILE: DlgAutoBaseRoad.cpp
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
#include "DlgAutoBaseRoad.h"
#include "DlgAutoRoad.h"
#include "DlgAutoAllRes.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoChooseTexture.h"

#include "aui\\AUIListBox.h"
#include "aui\\AUIManager.h"
#include "Aui\\AUIStillImageButton.h"

#include "AutoTerrain.h"
#include "AutoSelectedArea.h"



#include "AutoTerrainCommon.h"

#include "A3DEngine.h"
#include "A3DViewport.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoHillPoly.h"
#include "AutoRoad.h"
#include "AutoParameters.h"
#include "PolygonMountain.h"
#include "AutoBuildingOperation.h"
#include "AutoTerrainAction.h"

#include "windowsx.h"

#include "A3DMacros.h"
#include "io.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoBaseRoad, CDlgHomeBase)

AUI_ON_COMMAND("CreateRoad", OnCommandCreateRoad)
AUI_ON_COMMAND("Advance", OnCommandAdvance)
AUI_ON_COMMAND("DelRoad", OnCommandDelPre)

AUI_ON_COMMAND("ApplyTex", OnCommandApplyTex)
AUI_ON_COMMAND("AdvTex", OnCommandAdvTex)
AUI_ON_COMMAND("DelTex", OnCommandDelTex)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoBaseRoad, CDlgHomeBase)

AUI_ON_EVENT("ListBox_RoadInfo", WM_LBUTTONUP, OnLButtonUpListBoxRoad)
AUI_ON_EVENT("ListBox_Texture", WM_LBUTTONUP, OnLButtonUpListBoxTex)

AUI_END_EVENT_MAP()

CDlgAutoBaseRoad::CDlgAutoBaseRoad()
{
	m_pListBoxTexture = NULL;
	m_nTextureIdx = 0;

	m_pListBoxRoadInfo = NULL;
	m_nRoadInfoIdx = 0;
	
	
	m_iPreRoadNum = 0;
	m_iPreTexNum = 0;
	for(int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
}

CDlgAutoBaseRoad::~CDlgAutoBaseRoad()
{
}
	
AUIStillImageButton * CDlgAutoBaseRoad::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnBaseRoad;
}

void CDlgAutoBaseRoad::OnShowDialog()
{
	SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoBaseRoad::OnInitContext()
{
	if( !InitResource())
		return false;

	LoadDefault();
	UpdateData(false);
	return true;
}

#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);
void CDlgAutoBaseRoad::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_Texture", m_pListBoxTexture);
	DDX_ListBox(bSave, "ListBox_Texture", m_nTextureIdx);

	DDX_Control("ListBox_RoadInfo", m_pListBoxRoadInfo);
	DDX_ListBox(bSave, "ListBox_RoadInfo", m_nRoadInfoIdx);


	GET_BTN_POINTER(BTN_CREATE_ROAD);
	GET_BTN_POINTER(BTN_ADV_ROAD);
	GET_BTN_POINTER(BTN_DEL_ROAD);

	GET_BTN_POINTER(BTN_APPLY_TEX);
	GET_BTN_POINTER(BTN_ADV_TEX);
	GET_BTN_POINTER(BTN_DEL_TEX);

}	

void CDlgAutoBaseRoad::LoadDefault()
{
	m_nTextureIdx = 0;
	m_nRoadInfoIdx = 0;

	m_pBtn[BTN_CREATE_ROAD]->Enable(true);
	m_pBtn[BTN_ADV_ROAD]->Enable(true);
	m_pBtn[BTN_DEL_ROAD]->Enable(false);

	m_pBtn[BTN_APPLY_TEX]->Enable(false);
	m_pBtn[BTN_ADV_TEX]->Enable(false);
	m_pBtn[BTN_DEL_TEX]->Enable(false);
}

bool CDlgAutoBaseRoad::InitResource()
{
	FillTextureListBox();
	FillRoadInfoListBox();

	return true;
}

void CDlgAutoBaseRoad::FillTextureListBox()
{
	m_pListBoxTexture->ResetContent();
	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetTextureSet();

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
	long lFileHandle = _findfirst( "userdata\\home\\roadtex\\*.*", &fdIni);//获得第一个文件的句柄
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

void CDlgAutoBaseRoad::FillRoadInfoListBox()
{
	m_pListBoxRoadInfo->ResetContent();
	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetRoadSet();

	//预设数据
	if( pInfo)
	{
		m_iPreRoadNum = GetAutoParam()->GetNumRoadSet();
		for( int i = 0; i < m_iPreRoadNum; i++)
		{
			m_pListBoxRoadInfo->InsertString(i, AS2AC(pInfo[i].strName));
		}
	}

	//自定义数据
	_finddata_t fdIni;//文件属性
	long lFileHandle = _findfirst( "userdata\\home\\road\\*.*", &fdIni);//获得第一个文件的句柄
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
		m_pListBoxRoadInfo->InsertString(m_pListBoxRoadInfo->GetCount(), AS2AC(fdIni.name));
		
		//获取下一个用户自定义数据文件的句柄
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//关闭句柄
	_findclose( lFileHandle);
}

bool CDlgAutoBaseRoad::GetTextureParameter(int a_nIdx, CAutoParameters::TEXTUREINFO* a_pTexInfo)
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
		strcpy(szFileName, "userdata\\home\\roadtex\\");
		strcat(szFileName, AC2AS(m_pListBoxTexture->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadTextureInfo(szFileName, a_pTexInfo))
			return false;
	}

	return true;
}

bool CDlgAutoBaseRoad::GetRoadInfoParameter(int a_nIdx, CAutoParameters::ROADINFO* a_pRoadInfo)
{
	if( !IsValidIdx(a_nIdx, m_pListBoxRoadInfo))
		return false;

	if( a_nIdx < m_iPreRoadNum) //预设数据
	{
		CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetRoadSet();
		if( !GetAutoParam()->LoadRoadInfo(pInfo[a_nIdx].strPath, a_pRoadInfo))
			return false;
	}	
	else //用户数据
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName, "userdata\\home\\road\\");
		strcat(szFileName, AC2AS(m_pListBoxRoadInfo->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadRoadInfo(szFileName, a_pRoadInfo))
			return false;
	}
	
	return true;
	
}



void CDlgAutoBaseRoad::OnCommandCreateRoad(const char* szCommand)
{
	UpdateData(true);

	PROFILE_BEGIN();

	if (GetOperationType() != OT_ROAD)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoRoad* pRoad = pScene->GetAutoRoad();
	if (NULL == pRoad)
		return;
	
	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumLineEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetLineEditPoints();
	
	pRoad->SetPoints(nNum, pPoints);

	if (!pRoad->IsPointsValid())
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();


	CAutoParameters::ROADINFO roadInfo;
	if(!GetRoadInfoParameter(m_nRoadInfoIdx, &roadInfo))
	{
		AutoHomeReport("CDlgAutoBaseRoad::OnCommandCreateRoad, Failed to Get RoadInfo Parameter!");
		return;
	}

	
	if (!pRoad->CreateRoad(
		roadInfo.fRoadWidth,
		roadInfo.fMaxRoadHeight,
		roadInfo.fMainSmoothRoad,
		roadInfo.fPartSmoothRoad,
		roadInfo.dwSeedRoad,
		roadInfo.bFlatRoad))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	// Store action
	if( GetHomeDlgsMgr()->GetModifying())
	{
		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		if (!pAction->ChangeActionRoad(
					roadInfo.fRoadWidth,
					roadInfo.fMaxRoadHeight,
					roadInfo.fMainSmoothRoad,
					roadInfo.fPartSmoothRoad,
					roadInfo.dwSeedRoad,
					pRoad->GetNumPoints(),
					pRoad->GetPoints(),
				roadInfo.bFlatRoad ? 1 : 0, nIdx))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}

		GetAutoScene()->RefreshAllRoadActions();

	}
	else
	{
		if (!pAction->AddActionRoad(
					roadInfo.fRoadWidth,
					roadInfo.fMaxRoadHeight,
					roadInfo.fMainSmoothRoad,
					roadInfo.fPartSmoothRoad,
					roadInfo.dwSeedRoad,
					pRoad->GetNumPoints(),
					pRoad->GetPoints(),
				roadInfo.bFlatRoad ? 1 : 0))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
	}

	PROFILE_END();

	GetAutoScene()->RecreateTerrainRender(true);
	
	pSelArea->CopyPointsFromLineEditToView();
	pSelArea->SetViewAreaClosed(false);
	pSelArea->DeleteAllLineEditPoints();

	if( !GetHomeDlgsMgr()->GetModifying())
	{
		//Add Item to TerrainRes Tree View
		ACHAR szName[100];
		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1005),  m_nActionCount);
		m_nActionCount++;
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_ROAD, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnModifyToCreate();
		
	}
	
	//设置纹理操作可用
	SetTexOpEnable(true);
	
	return;

}

void CDlgAutoBaseRoad::OnCommandAdvance(const char* szCommand)
{
	UpdateData(true);
	
	//this->Show(false);
	
	GetHomeDlgsMgr()->m_pDlgAutoRoad->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();


	CAutoParameters::ROADINFO roadInfo;
	if( !GetRoadInfoParameter(m_nRoadInfoIdx, &roadInfo))
	{
		AutoHomeReport("CDlgAutoBaseRoad::OnCommandAdvance, Failed to GetRoadInfoParameter!");
		return;
	}

	//预设数据
	if( m_nRoadInfoIdx < m_iPreRoadNum)
	{
		GetHomeDlgsMgr()->m_pDlgAutoRoad->InitSettings(roadInfo,
													false);
	}
	else //自定义数据
	{
		GetHomeDlgsMgr()->m_pDlgAutoRoad->InitSettings(roadInfo,
													true, 
													AC2AS(m_pListBoxRoadInfo->GetText(m_nRoadInfoIdx)));
	}
}

void CDlgAutoBaseRoad::SetBtnCreateToModify()
{
	m_pBtn[BTN_CREATE_ROAD]->SetText( GetStringFromTable(1006) );
	return;
}

void CDlgAutoBaseRoad::SetBtnModifyToCreate()
{
	m_pBtn[BTN_CREATE_ROAD]->SetText( GetStringFromTable(1007) );
	return;
}

void CDlgAutoBaseRoad::InsertRoadSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxRoadInfo->GetCount();
	m_pListBoxRoadInfo->InsertString(iIdx, szFileName);
	
}

void CDlgAutoBaseRoad::OnCommandDelPre(const char* szCommand)
{
	UpdateData(true);

	if(m_nRoadInfoIdx < m_iPreRoadNum || m_nRoadInfoIdx >= m_pListBoxRoadInfo->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\Road\\"));

	a_strcat(szFileName, m_pListBoxRoadInfo->GetText(m_nRoadInfoIdx));

	//删除当前文件
	DeleteFile(szFileName);
	

	//删除当前listbox选项
	m_pListBoxRoadInfo->DeleteString(m_nRoadInfoIdx);
	
	//设置当前listbox选中
	if(m_nRoadInfoIdx < m_pListBoxRoadInfo->GetCount())
		m_pListBoxRoadInfo->SetCurSel(m_nRoadInfoIdx);
	else
	{
		m_nRoadInfoIdx = m_pListBoxRoadInfo->GetCount() -1;

		m_pListBoxRoadInfo->SetCurSel(m_nRoadInfoIdx);
		if( m_nRoadInfoIdx == m_iPreRoadNum -1)
		{
			m_pBtn[BTN_DEL_ROAD]->Enable(false);
		}
	}
	UpdateData(false);

}

void CDlgAutoBaseRoad::OnLButtonUpListBoxRoad(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//判断是否点中列表框中的选项
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxRoadInfo->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxRoadInfo->GetCurSel();

	if( nCurSel >= 0 && nCurSel < m_iPreRoadNum) // 预设
	{
		m_pBtn[BTN_CREATE_ROAD]->Enable(true);
		m_pBtn[BTN_ADV_ROAD]->Enable(true);
		m_pBtn[BTN_DEL_ROAD]->Enable(false);
		
	}
	else if( nCurSel < m_pListBoxRoadInfo->GetCount()) //自定义
	{
		m_pBtn[BTN_CREATE_ROAD]->Enable(true);
		m_pBtn[BTN_ADV_ROAD]->Enable(true);
		m_pBtn[BTN_DEL_ROAD]->Enable(true);
	}
	else //点在空位置											
	{
		m_pBtn[BTN_CREATE_ROAD]->Enable(false);
		m_pBtn[BTN_ADV_ROAD]->Enable(false);
		m_pBtn[BTN_DEL_ROAD]->Enable(false);
	}

	return;
}

void CDlgAutoBaseRoad::OnCommandApplyTex(const char* szCommand)
{
	UpdateData(true);

	CAutoScene* pScene = GetAutoScene();
	
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	
	CAutoParameters::TEXTUREINFO texInfo;

	if(!GetTextureParameter(m_nTextureIdx, &texInfo))
		AutoHomeReport("CDlgAutoBaseTerrain::OnCommandCreateTexture, Can not Get Texture Parameter!");

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	
	pTexture->ChangeTextureParam(
		TT_ROAD,
		TT_ROADBED,
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);


	pAction->AddActionRoadTextureInfo(
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

void CDlgAutoBaseRoad::OnCommandDelTex(const char* szCommand)
{
	UpdateData(true);

	if(m_nTextureIdx < m_iPreTexNum || m_nTextureIdx >= m_pListBoxTexture->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\roadtex\\"));
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

void CDlgAutoBaseRoad::InsertTexSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxTexture->GetCount();
	m_pListBoxTexture->InsertString(iIdx, szFileName);
	
}
void CDlgAutoBaseRoad::OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
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

void CDlgAutoBaseRoad::OnCommandAdvTex(const char* szCommand)
{
	UpdateData(true);
	
	
	CAutoParameters::TEXTUREINFO texInfo;
	if( !GetTextureParameter(m_nTextureIdx, &texInfo))
			AutoHomeReport("CDlgAutoBaseRoad::OnCommandAdvTex, Can not Get TextureParameter!");

	TextureParamInfo_t texParam;
	GetHomeDlgsMgr()->TransformTexInfoToTexParam(texInfo, texParam);

		//预设数据
	if( m_nTextureIdx < m_iPreTexNum)
	{
	
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_ROAD);
	}
	else //自定义数据
	{
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_ROAD,
													true, 
													AC2AS(m_pListBoxTexture->GetText(m_nTextureIdx)));
	}

	GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

}

void CDlgAutoBaseRoad::SetTexOpEnable(bool i_bEnable)
{
	m_pBtn[BTN_ADV_TEX]->Enable(i_bEnable);
	m_pBtn[BTN_APPLY_TEX]->Enable(i_bEnable);
}
*/