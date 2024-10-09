/*
 * FILE: DlgAutoChooseLight.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoChooseLight.h"
#include "DlgAutoChooseColor.h"
#include "DlgAutoLight.h"

#include "aui/AUILabel.h"
#include "aui/AUISlider.h"
#include "aui/AUIEditBox.h"
#include "aui/AUIImagePicture.h"

#include "AutoTerrainCommon.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoChooseLight, CDlgHomeBase)

AUI_ON_COMMAND("OK", OnCommandOK)
AUI_ON_COMMAND("Cancel", OnCommandCancel)
AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("OpenChooseLightColor", OnCommandOpenChooseLightColor)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoChooseLight, CDlgHomeBase)

AUI_END_EVENT_MAP()


CDlgAutoChooseLight::CDlgAutoChooseLight()
{
	m_pEditBoxLightName = NULL;
	m_pImagePicLightColor = NULL;
	m_pLabelLightRadius = NULL;
	m_pSliderLightRadius = NULL;
	m_nLightRadius = 0;
	
	m_bCheckDayEnable = true;
	m_bCheckNightEnable = true;

	m_clrLightColor = 0xffffffff;

	m_LightData.m_dwStatus = 0;

	m_cstrLightName.Empty();
}

CDlgAutoChooseLight::~CDlgAutoChooseLight()
{
}

bool CDlgAutoChooseLight::OnInitContext()
{
	SetSliderBound();
	LoadDefault();
	UpdateData(false);
	SetLabelsText();

	return true;
}

void CDlgAutoChooseLight::OnShowDialog()
{
	this->SetCanMove(false);
}

void CDlgAutoChooseLight::DoDataExchange(bool bSave)
{
	DDX_Control("EditBox_LightName", m_pEditBoxLightName);
	DDX_EditBox(bSave, "EditBox_LightName", m_cstrLightName);
	DDX_Control("ImagePic_LightColor", m_pImagePicLightColor);
	DDX_Control("Label_LightRadius", m_pLabelLightRadius);
	DDX_Control("Slider_LightRadius", m_pSliderLightRadius);
	
	DDX_Slider(bSave, "Slider_LightRadius", m_nLightRadius);

	DDX_CheckBox(bSave, "Check_DayEnable", m_bCheckDayEnable);
	DDX_CheckBox(bSave, "Check_NightEnable", m_bCheckNightEnable);

	if( bSave)
	{
		m_clrLightColor = m_pImagePicLightColor->GetColor();
	}
	else
	{
		m_pImagePicLightColor->SetColor(m_clrLightColor);
	}
}

void CDlgAutoChooseLight::SetSliderBound()
{
	m_pSliderLightRadius->SetTotal(100, 20);
}

void CDlgAutoChooseLight::LoadDefault()
{
	m_nLightRadius = 50;
	m_clrLightColor = 0xffffffff;
	
	m_bCheckDayEnable = true;
	m_bCheckNightEnable = true;

	m_cstrLightName.Empty();

}

void CDlgAutoChooseLight::SetLabelsText()
{
	ACHAR szName[100];
	wsprintf(szName, _AL("%d"), m_nLightRadius);
	m_pLabelLightRadius->SetText(szName);
	
}

void CDlgAutoChooseLight::OnCommandOK(const char* szCommand)
{
	UpdateData(true);
	m_LightData.m_strName = AC2AS(m_cstrLightName);
	//m_LightData.m_vPos = A3DVECTOR3(0.0f);

	m_LightData.m_dwColor = m_clrLightColor;
	m_LightData.m_dwAbient = 0xff000000;

	m_LightData.m_fRange = (float)m_nLightRadius;
	m_LightData.m_fFalloff = 1.0f;
	m_LightData.m_fAttenuation0 = 0.0f;
	m_LightData.m_fAttenuation1 = 0.0f;
	m_LightData.m_fAttenuation2 = 0.0f;
	if (m_bCheckDayEnable)
		m_LightData.m_dwStatus |= LS_DAY_ENABLE;
	if (m_bCheckNightEnable)
		m_LightData.m_dwStatus |= LS_NIGHT_ENABLE;

	CDlgAutoLight* pDlg = GetHomeDlgsMgr()->GetAutoLightDlg();
	CAutoLight* pLight = GetAutoScene()->GetAutoLight();
	switch(m_ChooseLightType)
	{
	case CLT_NEW:
		{	
			pDlg->SetLightData(m_LightData);
			pDlg->SetIsAddingLight(true);
		}
		break;
	case CLT_EDIT:
		{
// 			pLight->Edit(pDlg->GetLightIndex(),m_LightData);
// 			pDlg->UpdateLightInfos();
		}
		break;
	default:
		break;
	}
	
	this->Show(false);
}

void CDlgAutoChooseLight::OnCommandCancel(const char* szCommand)
{
	this->Show(false);
}

void CDlgAutoChooseLight::OnCommandMoveSlider(const char* szCommand)
{
	UpdateData(true);
	SetLabelsText();
}

void CDlgAutoChooseLight::SetImagePicColor(A3DCOLOR clr)
{
	m_pImagePicLightColor->SetColor(clr);
	m_clrLightColor = clr;
}

void CDlgAutoChooseLight::OnCommandOpenChooseLightColor(const char* szCommand)
{
	CDlgAutoChooseColor* pDlg = GetHomeDlgsMgr()->GetAutoChooseColorDlg();

	pDlg->InitColorDlg(CDlgAutoChooseColor::CT_LIGHT,m_clrLightColor);
	pDlg->Show(true);

}
*/
//void CDlgAutoChooseLight::InitChooseLight(ChooseLightType_e chooseLightType, const LIGHTDATA* pLightData /* = NULL*/)
/*{
	m_ChooseLightType = chooseLightType;
	switch(m_ChooseLightType)
	{
	case CLT_NEW:
		{
			LoadDefault();
			UpdateData(false);
			SetLabelsText();
		}
		break;
	case CLT_EDIT:
		{
			SetLightInfo(pLightData);
			UpdateData(false);
			SetLabelsText();
		}
		break;
	default:
		break;
	}

}

void CDlgAutoChooseLight::SetLightInfo(const LIGHTDATA* pLightData)
{
	m_LightData = *pLightData;
	
	m_cstrLightName = AS2AC(pLightData->m_strName);
	m_clrLightColor = pLightData->m_dwColor;
	m_nLightRadius = (int)pLightData->m_fRange;

	
	if(m_LightData.m_dwStatus |= LS_DAY_ENABLE)
	{
		m_bCheckDayEnable = true;
	}
	else
	{
		m_bCheckDayEnable = false;
	}

	
	if(m_LightData.m_dwStatus |= LS_NIGHT_ENABLE)
	{
		m_bCheckNightEnable = true;
	}
	else
	{
		m_bCheckNightEnable = false;
	}
}
*/