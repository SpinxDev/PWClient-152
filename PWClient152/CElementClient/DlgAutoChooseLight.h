/*
 * FILE: DlgAutoChooseLight.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_CHOOSE_LIGHT_H_
#define _DLG_AUTO_CHOOSE_LIGHT_H_

#include "DlgHomeBase.h"
#include "AutoLight.h"

class AUIEditBox;
class AUIImagePicture;
class AUILabel;
class AUISlider;

class CDlgAutoChooseLight : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoChooseLight();
	~CDlgAutoChooseLight();

	enum ChooseLightType_e
	{
		CLT_NEW,
		CLT_EDIT
	};


	virtual bool OnInitContext();
	virtual void OnShowDialog();
	virtual void DoDataExchange(bool bSave);
	
	void SetSliderBound();
	void LoadDefault();
	void SetLabelsText();

	void SetImagePicColor(A3DCOLOR clr);

	void InitChooseLight(ChooseLightType_e chooseLightType, const LIGHTDATA* pLightData = NULL);

protected:
	AUIEditBox* m_pEditBoxLightName;
	ACString m_cstrLightName;
	AUIImagePicture* m_pImagePicLightColor;
	AUILabel* m_pLabelLightRadius;
	AUISlider* m_pSliderLightRadius;
	int m_nLightRadius;
	bool m_bCheckDayEnable;
	bool m_bCheckNightEnable;
	
	A3DCOLOR m_clrLightColor;
	LIGHTDATA m_LightData;
	ChooseLightType_e m_ChooseLightType;
protected:
	void OnCommandOK(const char* szCommand);
	void OnCommandCancel(const char* szCommand);
	void OnCommandMoveSlider(const char* szCommand);

	void OnCommandOpenChooseLightColor(const char* szCommand);

	void SetLightInfo(const LIGHTDATA* pLightData);
};

#endif