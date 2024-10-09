/*
 * FILE: DlgAutoLight.cpp
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
#include "DlgAutoLight.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoChooseSky.h"
#include "DlgAutoChooseColor.h"
#include "DlgAutoChooseLight.h"
#include "DlgAutoSwitchDlg.h"

#include "aui/AUIImagePicture.h"
#include "aui/AUIManager.h"
#include "aui/AUILabel.h"
#include "aui/AUIListBox.h"

#include "A3DMacros.h"
#include "A3DEngine.h"
#include "A3DLight.h"
#include "A3DDevice.h"
#include "A3DViewport.h"

#include "AutoTerrainConfig.h"
#include "AutoTerrainCommon.h"
#include "AutoTerrainAction.h"

#include "Render.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"

#include "AutoFog.h"			// Added by jdl, 2006.03.03
#include <A3DTrace.h>			// Added by jdl, 2006.03.03

#include "windowsx.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoLight, CDlgHomeBase)

AUI_ON_COMMAND("LoadDaySkyBox", OnCommandLoadDaySkyBox)
AUI_ON_COMMAND("LoadNightSkyBox", OnCommandLoadNightSkyBox)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("UpdateFog", OnCommandUpdateFog)
//AUI_ON_COMMAND("ShowDaySky", OnCommandShowDaySky)
//AUI_ON_COMMAND("ShowNightSky", OnCommandShowNightSky)

//AUI_ON_COMMAND("OpenChooseLight", OnCommandOpenChooseLight)

//AUI_ON_COMMAND("EditLight", OnCommandEditLight)
//AUI_ON_COMMAND("PerspectiveView", OnCommandPerspectiveView)
//AUI_ON_COMMAND("EditCurLight", OnCommandEditCurLight)
//AUI_ON_COMMAND("DelLight", OnCommandDelLight)
//AUI_ON_COMMAND("ClearAllLight", OnCommandClearAllLight)

AUI_ON_COMMAND("ChangeLightMapType", OnCommandChangeLightMapType)
AUI_ON_COMMAND("MoveSliderDayWeight", OnCommandMoveSliderDayWeight)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoLight, CDlgHomeBase)

AUI_ON_EVENT("ImagePic_DaySun", WM_LBUTTONDOWN, OnEventOpenChooseDaySunColor)
AUI_ON_EVENT("ImagePic_DayEnv", WM_LBUTTONDOWN, OnEventOpenChooseDayEnvironmentColor)
AUI_ON_EVENT("ImagePic_DayFog", WM_LBUTTONDOWN, OnEventOpenChooseDayFogColor)
AUI_ON_EVENT("ImagePic_NightSun", WM_LBUTTONDOWN, OnEventOpenChooseNightSunColor)
AUI_ON_EVENT("ImagePic_NightEnv", WM_LBUTTONDOWN, OnEventOpenChooseNightEnvironmentColor)
AUI_ON_EVENT("ImagePic_NightFog", WM_LBUTTONDOWN, OnEventOpenChooseNightFogColor)
AUI_ON_EVENT("ImagePic_WaterFog", WM_LBUTTONDOWN, OnEventOpenChooseWaterFogColor)

AUI_END_EVENT_MAP()

struct FogRange_t
{
	float fBegin;
	float fEnd;
};

FogRange_t s_FogRange[] = {
	{50.0f, 500.0f},
	{50.0f, 400.0f},
	{50.0f, 300.0f},
	{50.0f, 200.0f},
	{50.0f, 100.0f} };

CDlgAutoLight::CDlgAutoLight()
{
	 m_clrDaySun = 0xffffffff; 
	 m_clrDayEnv = 0xff000000;
	 m_clrDayFog = 0xff404040;
	 m_clrNightSun = 0xff808080;
	 m_clrNightEnv = 0xff000000;
	 m_clrNightFog = 0xff404040;
	 m_clrWaterFog = 0x00000000;

	m_pImagePicDaySun = NULL;
	m_pImagePicDayEnv = NULL;
	m_pImagePicDayFog = NULL;
	m_pImagePicNightSun = NULL;
	m_pImagePicNightEnv = NULL;
	m_pImagePicNightFog = NULL;
	m_pImagePicWaterFog = NULL;

	m_pSliderDayFogLevel = NULL;
	m_pSliderNightFogLevel = NULL;

	m_pLabelDayFogLevel = NULL;
	m_pLabelNightFogLevel = NULL;

	m_nDayFogLevel= 1;
	m_nNightFogLevel = 1;

	m_bCheckFog = false;
	//m_nShowSky = 0;
// 	m_pListBoxLightInfos = NULL;
// 	m_nLightIndex = 0;
	m_bAddingLight = false;

	m_nLightmapType = 0;

	m_pSliderDayWeight = NULL;
	m_nDayWeight = 0;

	
}

CDlgAutoLight::~CDlgAutoLight()
{

}

bool CDlgAutoLight::OnInitContext()
{
	
	SetSliderBound();
	LoadDefault();
	UpdateData(false);
	SetAllImagePicCtrlColor();

	SetLabelsText();

	return true;
}

void CDlgAutoLight::OnShowDialog()
{
	CDlgHomeBase::OnShowDialog();
	SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}



void CDlgAutoLight::DoDataExchange(bool bSave)
{
	DDX_Control("ImagePic_DaySun", m_pImagePicDaySun);
	DDX_Control("ImagePic_DayEnv", m_pImagePicDayEnv);
	DDX_Control("ImagePic_DayFog", m_pImagePicDayFog);
	DDX_Control("ImagePic_NightSun", m_pImagePicNightSun);
	DDX_Control("ImagePic_NightEnv", m_pImagePicNightEnv);
	DDX_Control("ImagePic_NightFog", m_pImagePicNightFog);
	DDX_Control("ImagePic_WaterFog", m_pImagePicWaterFog);

	DDX_Control("Slider_DayFogLevel", m_pSliderDayFogLevel);
	DDX_Control("Slider_NightFogLevel", m_pSliderNightFogLevel);

	DDX_Control("Label_DayFogLevel", m_pLabelDayFogLevel);
	DDX_Control("Label_NightFogLevel", m_pLabelNightFogLevel);

	DDX_Slider(bSave, "Slider_DayFogLevel", m_nDayFogLevel);
	DDX_Slider(bSave, "Slider_NightFogLevel", m_nNightFogLevel);
	
	
	DDX_CheckBox(bSave, "Check_Fog", m_bCheckFog);
//	DDX_RadioButton(bSave, 1, m_nShowSky);

// 	DDX_Control("ListBox_LightInfos", m_pListBoxLightInfos);
// 	DDX_ListBox(bSave, "ListBox_LightInfos", m_nLightIndex);

	DDX_RadioButton(bSave, 1, m_nLightmapType);

	DDX_Control("Slider_DayWeight", m_pSliderDayWeight);
	DDX_Slider(bSave, "Slider_DayWeight", m_nDayWeight);
}	

void CDlgAutoLight::SetImagePicColor(int nColorType, A3DCOLOR color)
{
	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();

	switch(nColorType)
	{
	case CDlgAutoChooseColor::CT_DAYSUN:
		{
			m_clrDaySun = color;
			pConfig->dwSunColor = color;
		}
		 break;
	case CDlgAutoChooseColor::CT_DAYENV:
		{
			m_clrDayEnv = color;
			pConfig->dwAmbient = color;
		}
		break;
	case CDlgAutoChooseColor::CT_DAYFOG:
		{
			m_clrDayFog = color;
			pConfig->dwFogColorDay = color;
		}
		break;
	case CDlgAutoChooseColor::CT_NIGHTSUN:
		{
			m_clrNightSun = color;
			pConfig->dwSunColorNight = color;
		}
		break;
	case CDlgAutoChooseColor::CT_NIGHTENV:
		{
			m_clrNightEnv = color;
			pConfig->dwAmbientNight = color;
		}
		break;
	case CDlgAutoChooseColor::CT_NIGHTFOG:
		{
			m_clrNightFog = color;
			pConfig->dwFogColorNight = color;
		}
		break;
	case CDlgAutoChooseColor::CT_WATERFOG:
		{
			m_clrWaterFog = color;
			pConfig->dwFogColorWater = color;
		}
		break;
	default:
		break;
	};

	SetAllImagePicCtrlColor();
}

void CDlgAutoLight::SetAllImagePicCtrlColor()
{
	m_pImagePicDaySun->SetColor(m_clrDaySun);
	m_pImagePicDayEnv->SetColor(m_clrDayEnv);
	m_pImagePicDayFog->SetColor(m_clrDayFog);
	m_pImagePicNightSun->SetColor(m_clrNightSun);
	m_pImagePicNightEnv->SetColor(m_clrNightEnv);
	m_pImagePicNightFog->SetColor(m_clrNightFog);
	m_pImagePicWaterFog->SetColor(m_clrWaterFog);
}

void CDlgAutoLight::OnCommandLoadDaySkyBox(const char* szCommand)
{
	GetHomeDlgsMgr()->GetAutoChooseSkyDlg()->InitChooseSky(CDlgAutoChooseSky::ST_DAY);
	GetHomeDlgsMgr()->GetAutoChooseSkyDlg()->Show(true);
}

void CDlgAutoLight::OnCommandLoadNightSkyBox(const char* szCommand)
{
	GetHomeDlgsMgr()->GetAutoChooseSkyDlg()->InitChooseSky(CDlgAutoChooseSky::ST_NIGHT);
	GetHomeDlgsMgr()->GetAutoChooseSkyDlg()->Show(true);
}


void CDlgAutoLight::OnEventOpenChooseDaySunColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{	CDlgAutoChooseColor* pDlgColor = GetHomeDlgsMgr()->GetAutoChooseColorDlg();
	pDlgColor->InitColorDlg(CDlgAutoChooseColor::CT_DAYSUN, m_clrDaySun);
	pDlgColor->Show(true, true);
}

void CDlgAutoLight::OnEventOpenChooseDayEnvironmentColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CDlgAutoChooseColor* pDlgColor = GetHomeDlgsMgr()->GetAutoChooseColorDlg();
	pDlgColor->InitColorDlg(CDlgAutoChooseColor::CT_DAYENV, m_clrDayEnv);
	pDlgColor->Show(true, true);
}

void CDlgAutoLight::OnEventOpenChooseDayFogColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CDlgAutoChooseColor* pDlgColor = GetHomeDlgsMgr()->GetAutoChooseColorDlg();
	pDlgColor->InitColorDlg(CDlgAutoChooseColor::CT_DAYFOG, m_clrDayFog);
	pDlgColor->Show(true, true);
}

void CDlgAutoLight::OnEventOpenChooseNightSunColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CDlgAutoChooseColor* pDlgColor = GetHomeDlgsMgr()->GetAutoChooseColorDlg();
	pDlgColor->InitColorDlg(CDlgAutoChooseColor::CT_NIGHTSUN, m_clrNightSun);
	pDlgColor->Show(true, true);
}
void CDlgAutoLight::OnEventOpenChooseWaterFogColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CDlgAutoChooseColor* pDlgColor = GetHomeDlgsMgr()->GetAutoChooseColorDlg();
	pDlgColor->InitColorDlg(CDlgAutoChooseColor::CT_WATERFOG, m_clrWaterFog);
	pDlgColor->Show(true, true);
}

void CDlgAutoLight::OnEventOpenChooseNightEnvironmentColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CDlgAutoChooseColor* pDlgColor = GetHomeDlgsMgr()->GetAutoChooseColorDlg();
	pDlgColor->InitColorDlg(CDlgAutoChooseColor::CT_NIGHTENV, m_clrNightEnv);
	pDlgColor->Show(true, true);
}

void CDlgAutoLight::OnEventOpenChooseNightFogColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CDlgAutoChooseColor* pDlgColor = GetHomeDlgsMgr()->GetAutoChooseColorDlg();
	pDlgColor->InitColorDlg(CDlgAutoChooseColor::CT_NIGHTFOG, m_clrNightFog);
	pDlgColor->Show(true, true);
}

void CDlgAutoLight::SetSliderBound()
{
	m_pSliderDayFogLevel->SetTotal(5, 1);
	m_pSliderNightFogLevel->SetTotal(5, 1);
	m_pSliderDayWeight->SetTotal(100, 0);
}

void CDlgAutoLight::LoadDefault()
{
	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();

	m_clrDaySun = pConfig->dwSunColor;
	m_clrDayEnv = pConfig->dwAmbient;
	m_clrDayFog = pConfig->dwFogColorDay;

	m_clrNightSun = pConfig->dwSunColorNight;
	m_clrNightEnv = pConfig->dwAmbientNight;
	m_clrNightFog = pConfig->dwFogColorNight;

	m_clrWaterFog = pConfig->dwFogColorWater;

	m_bCheckFog = pConfig->bFogEnableDay;

	//m_bCheckFog = false;
//	m_nShowSky = 0;
//	m_nLightIndex = 0;
	
	m_nLightmapType = pConfig->nLightmapType;
	
	m_nDayWeight = 0;

//	m_pListBoxLightInfos->ResetContent();
}

void CDlgAutoLight::SetLabelsText()
{
	ACHAR szName[100];
	swprintf(szName, _AL("%d"), m_nDayFogLevel);
	m_pLabelDayFogLevel->SetText(szName);

	swprintf(szName, _AL("%d"), m_nNightFogLevel);
	m_pLabelNightFogLevel->SetText(szName);


}

void CDlgAutoLight::OnCommandMoveSlider(const char* szCommand)
{
	UpdateData(true);
	SetLabelsText();

	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	pConfig->fFogStartDay = s_FogRange[m_nDayFogLevel-1].fBegin;
	pConfig->fFogEndDay = s_FogRange[m_nDayFogLevel-1].fEnd;
	pConfig->fFogStartNight = s_FogRange[m_nNightFogLevel-1].fBegin;
	pConfig->fFogEndNight = s_FogRange[m_nNightFogLevel-1].fEnd;

	pConfig->dwFogColorDay = m_clrDayFog;
	pConfig->dwFogColorNight = m_clrNightFog;
	pConfig->dwFogColorWater = m_clrWaterFog;

	pConfig->bFogEnableDay = m_bCheckFog;
	pConfig->bFogEnableNight = m_bCheckFog;

	
}

void CDlgAutoLight::OnCommandUpdateFog(const char* szCommand)
{
	UpdateData(true);

	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	pConfig->bFogEnableDay = m_bCheckFog;
	pConfig->bFogEnableNight = m_bCheckFog;
}

//bool CDlgAutoLight::IsShowDaySky()
//{
//	if( m_nShowSky == 0)
//		return true;
//	
//	return false;
//}



//void CDlgAutoLight::OnCommandShowDaySky(const char* szCommand)
//{
//	CAutoSky* pAutoSky = GetAutoScene()->GetAutoSky();
//	pAutoSky->SetCurSkyType(CAutoSky::ST_DAY);
//	pAutoSky->SetBackColorByCurSky();
//	UpdateFog();
//}
//
//void CDlgAutoLight::OnCommandShowNightSky(const char* szCommand)
//{
//	CAutoSky* pAutoSky = GetAutoScene()->GetAutoSky();
//	pAutoSky->SetCurSkyType(CAutoSky::ST_NIGHT);
//	pAutoSky->SetBackColorByCurSky();
//	UpdateFog();
//}

// void CDlgAutoLight::OnCommandOpenChooseLight(const char* szCommand)
// {
// 	
// 	if (GetOperationType() != OT_LIGHT)
// 	{
// 		GetAUIManager()->MessageBox("报告", _AL("当前不处于灯光操作!"), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
// 		return;
// 	}
// 	if (m_bAddingLight)
// 	{
// 		GetAUIManager()->MessageBox("报告", _AL("已经选择了灯光，请在地图上选择灯光位置!"), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
// 		return;
// 	}
// 
// 	GetHomeDlgsMgr()->GetAutoChooseLightDlg()->InitChooseLight(CDlgAutoChooseLight::CLT_NEW);
// 	GetHomeDlgsMgr()->GetAutoChooseLightDlg()->Show(true);
// }

void CDlgAutoLight::OnCommandEditLight(const char* szCommand)
{
	CAutoScene* pAutoScene = GetAutoScene();
	pAutoScene->SetOperationType(OT_LIGHT);
}

void CDlgAutoLight::OnCommandPerspectiveView(const char* szCommand)
{
	CAutoScene* pAutoScene = GetAutoScene();
	pAutoScene->SetOperationType(OT_NORMAL);
}

void CDlgAutoLight::SetLightData(LIGHTDATA lightData)
{
	m_LightData = lightData;
}
*/
/*
void CDlgAutoLight::OnCommandEditCurLight(const char* szCommand)
{
	if( m_pListBoxLightInfos->GetCount() <= 0)
		return;

	CDlgAutoChooseLight* pDlg = GetHomeDlgsMgr()->GetAutoChooseLightDlg();
	CAutoLight* pLight = GetAutoScene()->GetAutoLight();
	LIGHTDATA* pData = &((*pLight->GetLightData())[m_nLightIndex]);
	m_LightData = *pData;
	pDlg->InitChooseLight(CDlgAutoChooseLight::CLT_EDIT, &m_LightData);
	pDlg->Show(true);
}
*/
/*
void CDlgAutoLight::OnCommandDelLight(const char* szCommand)
{
	if (GetOperationType() != OT_LIGHT)
	{
		GetAUIManager()->MessageBox("报告", _AL("当前不处于灯光操作!"), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (m_bAddingLight)
	{
		GetAUIManager()->MessageBox("报告", _AL("已经选择了灯光，请在地图上选择灯光位置!"), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	UpdateData(true);
	if (m_pListBoxLightInfos->GetCount() <= 0)
		return;
	CAutoLight* pLight = GetAutoScene()->GetAutoLight();
	if (!pLight->Delete(m_nLightIndex))
		return;
	UpdateLightInfos();
}
*/

/*
void CDlgAutoLight::OnCommandClearAllLight(const char* szCommand)
{
		// TODO: Add your control notification handler code here
	if (GetOperationType() != OT_LIGHT)
	{
		GetAUIManager()->MessageBox("报告", _AL("当前不处于灯光操作!"), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (m_bAddingLight)
	{
		GetAUIManager()->MessageBox("报告", _AL("已经选择了灯光，请在地图上选择灯光位置!"), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	UpdateData(true);
	CAutoLight* pLight = GetAutoScene()->GetAutoLight();
	if (!pLight->DeleteAll())
		return;
	UpdateLightInfos();
}
*/

/*
bool CDlgAutoLight::UpdateLightInfos()
{
	CAutoLight* pLight = GetAutoScene()->GetAutoLight();

	AArray<LIGHTDATA, LIGHTDATA&>* pLightData = pLight->GetLightData();

	int i;
	m_pListBoxLightInfos->ResetContent();
	for (i=0; i<pLightData->GetSize(); i++)
	{
		m_pListBoxLightInfos->AddString(AS2AC((*pLightData)[i].m_strName));
	}
	m_nLightIndex = pLight->GetSelIndex();
	UpdateData(false);

	return true;
}
*/

/*
void CDlgAutoLight::OnLButtonDownScreen(WPARAM wParam, LPARAM lParam)
{
	APointI point;

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	point.x = GET_X_LPARAM(lParam)  - p->X;
	point.y = GET_Y_LPARAM(lParam)  - p->Y;

	if (!m_bAddingLight)
		return ;
	
	m_bAddingLight = false;

	A3DVECTOR3 vEnd((float)point.x, (float)point.y, 1.0f);
	A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
	A3DVECTOR3 vStart((float)point.x, (float)point.y, 0.0f);

	pViewport->InvTransform(vStart,vStart);
	pViewport->InvTransform(vEnd,vEnd);

	RAYTRACERT TraceRt;	//	Used to store trace result
	A3DVECTOR3 vTracePos;
	if(!GetAutoScene()->GetTracePos(vStart,vEnd,vTracePos)) 
		return ;

	CAutoLight* pLight = GetAutoScene()->GetAutoLight();
	m_LightData.m_vPos = vTracePos;
	pLight->Add(m_LightData);
	pLight->SetSelIndex(pLight->GetLightData()->GetSize()-1);
	UpdateLightInfos();
	
	return ;
}
*/

/*
void CDlgAutoLight::OnCommandChangeLightMapType(const char* szCommand)
{
	UpdateData(true);
	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	pConfig->nLightmapType = m_nLightmapType;
}

void CDlgAutoLight::OnCommandMoveSliderDayWeight(const char* szCommand)
{
	UpdateData(true);

	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	pConfig->fDayWeight = m_nDayWeight / 100.0f;

}
*/