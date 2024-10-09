/*
 * FILE: EC_HomeDlgsMgr.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_HOME_DLGS_MGR_H_
#define _EC_HOME_DLGS_MGR_H_

#include "vector.h"
#include "AutoScene.h"

#include "AutoTexture.h"
#include "AutoParameters.h"

#include "ALog.h"

inline void AutoHomeAssert() 
{
#ifdef _DEBUG
	_asm int 3
#endif
}

inline void AutoHomeReport(char* szReport)
{
	if(!szReport)
		return;
	a_LogOutput(1, szReport);
	AutoHomeAssert();
}
					

class AUIManager;
class AUIDialog;
class AUIStillImageButton;
class CAutoScene;
class CAutoParameters;


struct TextureParamInfo_t
{
	float	fMainTextureU;
	float	fMainTextureV;
	float	fPatchTextureU;
	float	fPatchTextureV;
	AString	strMainTextureFile;
	AString	strPatchTextureFile;
	unsigned int nMainTexID;
	unsigned int nPatchTexID;
	int nMainTexIdx;
	int nPatchTexIdx;
};

enum ChooseTexInitParam_e
{
	CTIP_NULL,		//NULL
	CTIP_TERRAIN,
	CTIP_HILL,
	CTIP_HILL_LINE,
	CTIP_LOCALTEX,
	CTIP_ROAD,
	CTIP_WATER,
};


class CECHomeDlgsMgr
{
	
	friend class CDlgAutoLight;
	friend class CDlgAutoLocalTexture;
	friend class CDlgAutoMountain;
	friend class CDlgAutoMountain2;
	friend class CDlgAutoRoad;
	friend class CDlgAutoTerrain;
	friend class CDlgAutoWater;
	friend class CDlgAutoChooseTexture;
	friend class CDlgHomeBase;
	friend class CDlgCurve;
	
	friend class CDlgAutoChooseSky;
	friend class CDlgAutoChooseColor;
	friend class CDlgAutoChooseLight;

	friend class CDlgAutoTerrainRes;
	friend class CDlgAutoIndoorRes;
	friend class CDlgAutoOutdoorRes;
	friend class CDlgAutoAllRes;
	
	friend class CDlgAutoTools;
	friend class CDlgAutoNote;
	friend class CDlgAutoNote2;
	friend class CDlgAutoCtrl;

	friend class CDlgAutoBaseTerrain;
	friend class CDlgAutoBaseMountain;
	friend class CDlgAutoBaseMountain2;
	friend class CDlgAutoBaseRoad;
	friend class CDlgAutoBaseWater;
	friend class CDlgAutoBaseLocalTex;
	friend class CDlgAutoLevelOff;

	friend class CDlgAutoOutdoorTree;
	friend class CDlgAutoOutdoorGrass;
	friend class CDlgAutoOutdoorOtherObject;
	friend class CDlgAutoOutdoorBuilding;

	friend class CDlgAutoIndoorVirtu;
	friend class CDlgAutoIndoorPlant;
	friend class CDlgAutoIndoorToy;
	friend class CDlgAutoIndoorFurniture;

	friend class CDlgAutoSwitchDlg;
	friend class CDlgAutoCtrlEditArea;
	friend class CDlgAutoGenLightMapProgress;
	friend class CDlgAutoCamera;
	

public:
	struct HomeDlgsMgrInitParams_t
	{
		AUIManager* pAUIMgr;

		HomeDlgsMgrInitParams_t(): pAUIMgr(0) {};

	};
	enum MODEL_STATE
	{
		MSE_CREATE = 0,
		MSE_MODIFY,
		MSE_HIT
	};


public:
	CECHomeDlgsMgr();
	virtual ~CECHomeDlgsMgr();
	
	bool Init(HomeDlgsMgrInitParams_t params);

	void Show(bool bShow = true);

	bool LoadAutoSceneAction(const char* szActionFile);

	bool SetPersCamera();
	bool SetOrthCamera();

	inline CAutoScene* GetAutoScene() { return m_pAutoScene;}
	inline CAutoParameters* GetAutoParam() { return m_pAutoParam;}

	//void DeleteEditPoint(void);
	void OnLButtonDownScreen(WPARAM wParam, LPARAM lParam);


	inline CDlgAutoLight* GetAutoLightDlg() { return m_pDlgAutoLight;}
	inline CDlgAutoLocalTexture* GetAutoLocalTextureDlg() { return m_pDlgAutoLocalTexture;}
	inline CDlgAutoMountain* GetAutoMountainDlg() { return m_pDlgAutoMountain;}
	inline CDlgAutoRoad* GetAutoRoadDlg(){ return m_pDlgAutoRoad;}
	inline CDlgAutoTerrain* GetAutoTerrainDlg() { return m_pDlgAutoTerrain;}
	inline CDlgAutoWater* GetAutoWaterDlg(){ return m_pDlgAutoWater;}

	inline CDlgAutoChooseTexture* GetAutoChooseTextureDlg() { return m_pDlgAutoChooseTexture;}
	inline CDlgCurve* GetCurveDlg() { return m_pDlgCurve;}
	
	inline CDlgAutoChooseSky* GetAutoChooseSkyDlg() { return m_pDlgAutoChooseSky;}
	inline CDlgAutoChooseColor* GetAutoChooseColorDlg() { return m_pDlgAutoChooseColor;}
	inline CDlgAutoChooseLight* GetAutoChooseLightDlg() { return m_pDlgAutoChooseLight;}


	inline CDlgAutoCtrl* GetAutoCtrlDlg() { return m_pDlgAutoCtrl;}
	inline CDlgAutoNote* GetAutoNoteDlg() { return m_pDlgAutoNote;}
	inline CDlgAutoTools* GetAutoToolsDlg() { return m_pDlgAutoTools;}

	inline CDlgAutoBaseLocalTex* GetAutoBaseLocalTexDlg() { return m_pDlgAutoBaseLocalTex;}
	inline CDlgAutoBaseMountain* GetAutoBaseMountainDlg() { return m_pDlgAutoBaseMountain;}
	inline CDlgAutoBaseMountain2* GetAutoBaseMountain2Dlg() { return m_pDlgAutoBaseMountain2;}
	inline CDlgAutoBaseWater* GetAutoBaseWaterDlg() { return m_pDlgAutoBaseWater;}
	inline CDlgAutoBaseRoad* GetAutoBaseRoadDlg() { return m_pDlgAutoBaseRoad;}
	inline CDlgAutoLevelOff* GetAutoLevelOffDlg() { return m_pDlgAutoLevelOff;}
	inline CDlgAutoBaseTerrain* GetAutoBaseTerrainDlg() { return m_pDlgAutoBaseTerrain;}
	inline CDlgAutoGenLightMapProgress* GetAutoGenLightMapProgress() { return m_pDlgAutoGenLightMapProgress;}
	inline CDlgAutoCamera* GetAutoCameraDlg() { return m_pDlgAutoCamera;}
	
	inline AUIManager* GetAUIManager() { return m_pAUIMgr;}
	void SetModifying(bool i_bModifying);
	void SetModelState( int i_nModelState) { m_nModelState = i_nModelState;}
	

	inline bool GetModifying() const { return m_bModifying;}
	inline const int GetModelState() const { return m_nModelState;}

	//设置非高级窗口不可用
	void SetUnAdvDlgsDisable();
	void SetUnAdvDlgsEnable();
	
	//Utility Function
	bool LoadTextureParamInfo(TextureParamInfo_t& param, TEXTURETYPE mainTexture, TEXTURETYPE patchTexture);
	bool SaveTextureParamInfo(TextureParamInfo_t& param, TEXTURETYPE mainTexture, TEXTURETYPE patchTexture);
	bool GetTextureIDAndIdx(const AString& strTexName, unsigned int& uTexID, int& uTexIdx);
	void HideAllAdvanceDlg();
	void TransformTexParamToTexInfo(const TextureParamInfo_t &texParam, TEXTUREINFO &texInfo);
	void TransformTexInfoToTexParam(const TEXTUREINFO &texInfo, TextureParamInfo_t &texParam);

	
	bool InitDialogs();

	void Clear();

	void ClearDlgsVars();
	bool Reset();

protected:
	
	void HideAllOldDlg();
	void ShowAllOldDlg();

	//
	void ShowHomeDlgs();
	void HideHomeDlgs();

	bool InitSwitchBtn();

    void SwitchDialog(const char * szCommand);
	

protected:

	CAutoScene* m_pAutoScene;
	CAutoParameters* m_pAutoParam;

	abase::vector<AUIDialog*> m_vecOldDlg;
	typedef abase::vector<AUIDialog*>::iterator OldDlgVecIter_t;

	AUIManager * m_pAUIMgr;

	//-------------------------------------------------
	
	CDlgAutoLight* m_pDlgAutoLight;
	CDlgAutoLocalTexture* m_pDlgAutoLocalTexture;
	CDlgAutoMountain* m_pDlgAutoMountain;
	CDlgAutoMountain2* m_pDlgAutoMountain2;
	CDlgAutoRoad* m_pDlgAutoRoad;
	CDlgAutoTerrain* m_pDlgAutoTerrain;
	CDlgAutoWater* m_pDlgAutoWater;
	
	AUIDialog* m_pCurDlg;
	

	CDlgAutoChooseTexture* m_pDlgAutoChooseTexture;
	CDlgCurve* m_pDlgCurve;

	CDlgAutoChooseSky* m_pDlgAutoChooseSky;
	CDlgAutoChooseColor* m_pDlgAutoChooseColor;
	CDlgAutoChooseLight* m_pDlgAutoChooseLight;


	CDlgAutoAllRes*  m_pDlgAutoAllRes;

	CDlgAutoBaseTerrain* m_pDlgAutoBaseTerrain;
	CDlgAutoBaseMountain* m_pDlgAutoBaseMountain;
	CDlgAutoBaseMountain2* m_pDlgAutoBaseMountain2;
	CDlgAutoBaseRoad* m_pDlgAutoBaseRoad;
	CDlgAutoBaseWater* m_pDlgAutoBaseWater;
	CDlgAutoBaseLocalTex* m_pDlgAutoBaseLocalTex;
	CDlgAutoLevelOff* m_pDlgAutoLevelOff;
	AUIDialog* m_pDlgCurBase;

	CDlgAutoTools* m_pDlgAutoTools;
	CDlgAutoNote* m_pDlgAutoNote;
	CDlgAutoNote2* m_pDlgAutoNote2;
	CDlgAutoCtrl* m_pDlgAutoCtrl;

	CDlgAutoIndoorToy* m_pDlgAutoIndoorToy;
	CDlgAutoIndoorPlant* m_pDlgAutoIndoorPlant;
	CDlgAutoIndoorFurniture* m_pDlgAutoIndoorFurniture;
	CDlgAutoIndoorVirtu* m_pDlgAutoIndoorVirtu;
	AUIDialog* m_pDlgCurIndoor;

	CDlgAutoOutdoorBuilding* m_pDlgAutoOutdoorBuilding;
	CDlgAutoOutdoorGrass* m_pDlgAutoOutdoorGrass;
	CDlgAutoOutdoorTree* m_pDlgAutoOutdoorTree;
	CDlgAutoOutdoorOtherObject* m_pDlgAutoOutdoorOtherObject;
	AUIDialog* m_pDlgCurOutdoor;
	
	CDlgAutoSwitchDlg* m_pDlgAutoSwitchDlg;
	CDlgAutoCtrlEditArea* m_pDlgAutoCtrlEditArea;

	CDlgAutoGenLightMapProgress* m_pDlgAutoGenLightMapProgress;
	CDlgAutoCamera* m_pDlgAutoCamera;


	//----------------------------------------------
	
	AUIStillImageButton* m_pBtnBaseTerrain;
	AUIStillImageButton* m_pBtnBaseMountain;
	AUIStillImageButton* m_pBtnBaseMountain2;
	AUIStillImageButton* m_pBtnBaseRoad;
	AUIStillImageButton* m_pBtnBaseWater;
	AUIStillImageButton* m_pBtnBaseLocalTex;
	AUIStillImageButton* m_pBtnLevelOff;
	AUIStillImageButton* m_pBtnCurBase;

	//---------------------------------------------
	AUIStillImageButton* m_pBtnIndoorToy;
	AUIStillImageButton* m_pBtnIndoorFurniture;
	AUIStillImageButton* m_pBtnIndoorVirtu;
	AUIStillImageButton* m_pBtnIndoorPlant;
	AUIStillImageButton* m_pBtnCurIndoor;

	AUIStillImageButton* m_pBtnOutdoorTree;
	AUIStillImageButton* m_pBtnOutdoorGrass;
	AUIStillImageButton* m_pBtnOutdoorBuilding;
	AUIStillImageButton* m_pBtnOutdoorOtherObject;
	AUIStillImageButton* m_pBtnCurOutdoor;
	
	bool m_bInitReady; 
	bool m_bModifying;
	int m_nModelState;

};

#endif