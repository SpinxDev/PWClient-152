/*
 * FILE: DlgAutoLight.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_LIGHT_H_
#define _DLG_AUTO_LIGHT_H_
/*
#include "DlgHomeBase.h"
#include "AutoLight.h"

class AUIImagePicture;
class AUISlider;
class AUILabel;
class AUIListBox;

class CDlgAutoLight: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
protected:
	A3DCOLOR m_clrDaySun;
	A3DCOLOR m_clrDayEnv;
	A3DCOLOR m_clrDayFog;
	A3DCOLOR m_clrNightSun;
	A3DCOLOR m_clrNightEnv;
	A3DCOLOR m_clrNightFog;
	A3DCOLOR m_clrWaterFog;
	
	AUIImagePicture* m_pImagePicDaySun;
	AUIImagePicture* m_pImagePicDayEnv;
	AUIImagePicture* m_pImagePicDayFog;
	AUIImagePicture* m_pImagePicNightSun;
	AUIImagePicture* m_pImagePicNightEnv;
	AUIImagePicture* m_pImagePicNightFog;
	AUIImagePicture* m_pImagePicWaterFog;

	AUISlider* m_pSliderDayFogLevel;
	AUISlider* m_pSliderNightFogLevel;

	AUILabel* m_pLabelDayFogLevel;
	AUILabel* m_pLabelNightFogLevel;

	int m_nDayFogLevel;
	int m_nNightFogLevel;



	
	
	bool m_bCheckFog;
// 	int m_nShowSky;

// 	AUIListBox* m_pListBoxLightInfos;
// 	int m_nLightIndex;

	bool m_bAddingLight;
	LIGHTDATA m_LightData;

	int m_nLightmapType;

	AUISlider* m_pSliderDayWeight;
	int m_nDayWeight;

public:
	CDlgAutoLight();
	virtual ~CDlgAutoLight();
	
	
	void SetImagePicColor(int nColorType, A3DCOLOR color);

	void SetLightData(LIGHTDATA lightData);
	void SetIsAddingLight(bool bAddingLight) { m_bAddingLight = bAddingLight;}

	//void OnLButtonDownScreen(WPARAM wParam, LPARAM lParam);

	//bool UpdateLightInfos();
	//int GetLightIndex() { return m_nLightIndex;}

protected:
	virtual bool OnInitContext();
	virtual void OnShowDialog();
	virtual void DoDataExchange(bool bSave);

	void SetSliderBound();
	void LoadDefault();
	void SetLabelsText();

	void SetAllImagePicCtrlColor();

	void OnCommandLoadDaySkyBox(const char* szCommand);
	void OnCommandLoadNightSkyBox(const char* szCommand);

	void OnEventOpenChooseDaySunColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventOpenChooseDayEnvironmentColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventOpenChooseDayFogColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventOpenChooseNightSunColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventOpenChooseNightEnvironmentColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventOpenChooseNightFogColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventOpenChooseWaterFogColor(WPARAM wParam, LPARAM lParam, AUIObject* pObj);



	void OnCommandMoveSlider(const char* szCommand);
	void OnCommandUpdateFog(const char* szCommand);

//	void OnCommandShowDaySky(const char* szCommand);
//	void OnCommandShowNightSky(const char* szCommand);

//	void OnCommandOpenChooseLight(const char* szCommand);
	void OnCommandEditLight(const char* szCommand);
	void OnCommandPerspectiveView(const char* szCommand);
	//void OnCommandEditCurLight(const char* szCommand);
	//void OnCommandDelLight(const char* szCommand);
	//void OnCommandClearAllLight(const char* szCommand);

// 	bool IsShowDaySky();

	
	void OnCommandChangeLightMapType(const char* szCommand);
	void OnCommandMoveSliderDayWeight(const char* szCommand);
};
*/
#endif