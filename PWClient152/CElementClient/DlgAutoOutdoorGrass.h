/*
 * FILE: DlgAutoOutdoorGrass.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_OUTDOOR_GRASS_H_
#define _DLG_AUTO_OUTDOOR_GRASS_H_

#include "DlgHomeBase.h"
#include "AutoGrass.h"

class AUILabel;
class AUIListBox;
class AUISlider;


class CDlgAutoOutdoorGrass: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoOutdoorGrass();
	virtual ~CDlgAutoOutdoorGrass();

	virtual AUIStillImageButton * GetSwithButton();

	void SetBtnCreateToModify();
	void SetBtnModifyToCreate();
	
protected:
	virtual void OnShowDialog() ;
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	void SetLabelsText();

	bool FillNormalGrassSetListBox();
	bool FillWaterGrassSetListBox();
	void SetSliderBound();

	void ShowNormalGrassInfo();
	void ShowWaterGrassInfo();

	void OnCommandAddNormalGrassInfo(const char* szCommand);
	void OnCommandDelNormalGrassInfo(const char* szCommand);
	void OnCommandClearAllNormalGrassInfo(const char* szCommand);

	void OnCommandAddWaterGrassInfo(const char* szCommand);
	void OnCommandDelWaterGrassInfo(const char* szCommand);
	void OnCommandClearAllWaterGrassInfo(const char* szCommand);


	void OnCommandCreateNormalGrass(const char* szCommand);
	void OnCommandCreateWaterGrass(const char* szCommand);

	void OnCommandMoveSlider(const char * szCommand);

	void OnEventDblClkNormalGrassSetList(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventDblClkWaterGrassSetList(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventDblClkNormalGrassPlantList(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventDblClkWaterGrassPlantList(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	
protected:
	AUISlider* m_pSliderGrassDensity;
	AUISlider* m_pSliderGrassSeed;

	AUISlider* m_pSliderNormalGrassWeight;
	AUISlider* m_pSliderWaterGrassWeight;
	
	AUILabel* m_pLabelGrassDensity;
	AUILabel* m_pLabelGrassSeed;

	AUILabel* m_pLabelNormalGrassWeight;
	AUILabel* m_pLabelWaterGrassWeight;

	int m_nGrassDensity;
	int m_nGrassSeed;
	int m_nNormalGrassWeight;
	int m_nWaterGrassWeight;

	float m_fGrassDensity;
	float m_fGrassSeed;
	float m_fNormalGrassWeight;
	float m_fWaterGrassWeight;

	AUIListBox* m_pListBoxNormalGrassSet;
	AUIListBox* m_pListBoxNormalGrassPlant;
	AUIListBox* m_pListBoxWaterGrassSet;
	AUIListBox* m_pListBoxWaterGrassPlant;

	CAutoGrass::GRASSINFO* m_pNormalGrassInfo;
	int m_nNumNormalGrassInfos;

	CAutoGrass::GRASSINFO* m_pWaterGrassInfo;
	int m_nNumWaterGrassInfos;

	typedef AArray<PLANTIDWEIGHTINFO, PLANTIDWEIGHTINFO&> InfosArray_t;
	InfosArray_t m_NormalGrassInfoArray;
	InfosArray_t m_WaterGrassInfoArray;
	

	class AUIStillImageButton* m_pBtnCreateNormalGrass;
	class AUIStillImageButton* m_pBtnCreateWaterGrass;

protected:
	bool MakeSureThisInfoNoExiting(const InfosArray_t &i_InfoArray, const PLANTIDWEIGHTINFO &i_Info);

};

#endif