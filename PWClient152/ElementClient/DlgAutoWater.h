/*
 * FILE: DlgAutoWater.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_WATER_H_
#define _DLG_AUTO_WATER_H_

#include "DlgHomeBase.h"

class AUISlider;
class AUILabel;

class CDlgAutoWater : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

protected:
	AUISlider* m_pSliderWaterHeight;
	AUISlider* m_pSliderSandWidth;

	AUILabel* m_pLabelWaterHeight;
	AUILabel* m_pLabelSandWidth;

	int m_nWaterHeight;
	int m_nSandWidth;

	
	TextureParamInfo_t m_texParamInfo;
	
public:
	CDlgAutoWater();
	virtual ~CDlgAutoWater();
	
	
	TextureParamInfo_t* GetTextureParamInfo() { return &m_texParamInfo;}

	
protected:
	
	virtual void DoDataExchange(bool bSave);

	void SetSliderBound();
	void LoadDefault();
	void SetLabelsText();

	virtual bool OnInitContext();
	virtual void OnShowDialog();

	void UpdateTerrainHeight();
	void OnCommandMoveSlider(const char * szCommand);
	void OnCommandCreateWater(const char* szCommand);
	void OnCommandTurnToBase(const char* szCommand);
};

#endif