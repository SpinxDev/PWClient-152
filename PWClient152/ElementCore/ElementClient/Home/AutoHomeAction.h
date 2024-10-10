/*
 * FILE: AutoHomeAction.h
 *
 * DESCRIPTION: Class for home actions
 *
 * CREATED BY: Jiang Dalong, 2005/05/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOHOMEACTION_H_
#define _AUTOHOMEACTION_H_

#include "AutoSceneFunc.h"
#include <AArray.h>

class CAutoHome;
class CAutoHillPoly;
class CAutoHillLine;
class CAutoPartTexture;
class CAutoRoad;
class CAutoForestPoly;
class CAutoForestLine;
class CAutoGrassPoly;
class CAutoTerrainFlat;
class CAutoBuilding;
/*
class CAutoLight;
class CAutoBuildingOperation;
class CAutoFog;
*/
enum TERRAINACTIONTYPE
{
	TAT_BASE_TERRAIN,
	TAT_HILL_RECT,
	TAT_HILL_POLY,
	TAT_HILL_LINE,
	TAT_TEXTURE_POLY,
	TAT_WATER,
	TAT_ROAD,
	TAT_FOREST_POLY,
	TAT_FOREST_LINE,
	TAT_GRASS_POLY,
	TAT_BUILDING,
	TAT_BASE_TEXTURE_INFO,
	TAT_HILL_TEXTURE_INFO,
	TAT_PART_TEXTURE_INFO,
	TAT_WATER_TEXTURE_INFO,
	TAT_ROAD_TEXTURE_INFO,
	TAT_TERRAIN_FLAT,
	TAT_SKY,
	TAT_LIGHT,
	TAT_FOG,
}; 

class CAutoHomeAction   
{
public:
	struct BASETERRAIN// Base terrain
	{
		float	fMaxHeight;
		int		nBaseFrequency;
		float	fPersistence;
		float	fTextureRatio;
		float	fMainTexture;
		float	fPatchTexture;
		float	fPatchDivision;
		DWORD	dwRandomSeed;
	};
	struct TEXTUREINFO// Texture info
	{
		float	fMainTextureU;
		float	fMainTextureV;
		float	fPatchTextureU;
		float	fPatchTextureV;
		DWORD	dwMainID;
		DWORD	dwPatchID;
	};
	struct HILLPOLY// Hill poly
	{
		float fMaxHeight;
		float fMainSmooth;
		float fPartSmooth;
		DWORD dwSeed;
		float fTextureRatio;
		float fMainTexture;
		float fPatchTexture;
		float fPatchDivision;
		float fBridgeScope;
		float fBridgeDepth;
		int nNumPoints;
		int nHillUp;
		int nNumCurvePoint;
		float fBorderErode;
		POINT_FLOAT* pPoints;
		float* pCurvePoints;
	};
	struct HILLLINE// Hill line
	{
		float fMaxHeight;
		float fHillWidth;
		float fMainSmooth;
		float fPartSmooth;
		DWORD dwSeed;
		float fTextureRatio;
		float fMainTexture;
		float fPatchTexture;
		float fPatchDivision;
		float fBridgeScope;
		int nNumPoints;
		int nHillUp;
		int nNumCurvePoint;
		int nNumCurveLinePoint;
		float fBorderErode;
		POINT_FLOAT* pPoints;
		float* pCurvePoints;
		float* pCurveLinePoints;
	};
	struct PARTTEXTURE// Part texture
	{
		float fTextureRatio;
		float fMainTexture;
		float fPatchTexture;
		float fPatchDivision;
		DWORD dwSeed;
		float fBorderScope;
		int nTextureType;
		int nNumPoints;
		POINT_FLOAT* pPoints;
	};
	struct WATER// Water
	{
		float fWaterHeight;
		float fSandWidth;
		float fWaveSize;
	};
	struct ROAD// Road
	{
		float fRoadWidth;
		float fRoadHeight;
		float fMainSmooth;
		float fPartSmooth;
		DWORD dwSeed;
		int nFlatRoad;
		int nNumPoints;
		POINT_FLOAT* pPoints;
	};
	struct FORESTPOLY// Forest poly
	{
		int nPlantType;
		float fDensityX;
		float fDensityZ;
		float fMaxSlope;
		int nNumTreeType;
		DWORD dwSeed;
		int nNumPoints;
		DWORD* pIDs;
		float* pWeights;
		POINT_FLOAT* pPoints;
	};
	struct FORESTLINE// Forest line
	{
		float fSpace;
		float fSpaceNoise;
		float fMaxSlope;
		int nNumTreeType;
		DWORD dwSeed;
		int nNumPoints;
		DWORD* pIDs;
		float* pWeights;
		POINT_FLOAT* pPoints;
	};
	struct GRASSPOLY// Grass poly
	{
		int nGrassKind;
		float fDensity;
		DWORD dwSeed;
		int nNumGrassType;
		int nNumPoints;
		DWORD* pIDs;
		float* pWeights;
		POINT_FLOAT* pPoints;
	};
	struct BUILDING// Building
	{
		DWORD dwID;
		float fPosX;
		float fPosY;
		float fPosZ;
		float fRotateY;
	};
	struct TERRAINFLAT// Terrain flat
	{
		int nNumPoints;
		POINT_FLOAT* pPoints;
	};
	struct SKY// Sky
	{
		DWORD dwDaySkyID;
		DWORD dwNightSkyID;
	};
	struct LIGHT// Light
	{
		int nNameLength;
		float fPosX;
		float fPosY;
		float fPosZ;
		DWORD dwColor;
		DWORD dwAmbient;
		float fRange;
		float fFallOff;
		float fAttenuation0;
		float fAttenuation1;
		float fAttenuation2;
		DWORD dwStatus;
		char* szName;
	};
	struct FOG// fog
	{
		bool bFogEnableDay;
		float fFogStartDay;
		float fFogEndDay;
		DWORD dwFogColorDay;
		bool bFogEnableNight;
		float fFogStartNight;
		float fFogEndNight;
		DWORD dwFogColorNight;
		DWORD dwFogColorWater;
	};

public:
	CAutoHomeAction(CAutoHome* pAutoHome);
	virtual ~CAutoHomeAction();

public:
	// Add action of generating base terrain
	bool AddActionBaseTerrain(float fMaxHeight, int nBaseFrequency, float fPersistence, float fMaskWeights, float fMaskSelfWeights, float fTransition, float fPartitionNumber, DWORD dwRandomSeed);
	// Add action of generating base texture info
	bool AddActionBaseTextureInfo(float fMainTextureU, float fMainTextureV, 
					float fPatchTextureU, float fPatchTextureV, 
					DWORD dwMainID, DWORD dwPatchID);
	// Add action of generating hill poly
	bool AddActionHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth,
					DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, bool bHillUp, float fTextureRatio, float fMainTexture, float fPatchTexture, 
					float fPatchDivision, float fBridgeScope, float fBridgeDepth, int nNumPoints, POINT_FLOAT* pPoints, float fBorderErode);
	// Add action of generating hill line
	bool AddActionHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, int nNumCurveLinePoint, float* pCurveLinePoints, bool bHillUp, int nNumPoints, POINT_FLOAT* pPoints,
				float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, float fBridgeScope, float fBorderErode);
	// Add action of generating base texture info
	bool AddActionHillTextureInfo(float fMainTextureU, float fMainTextureV, 
					float fPatchTextureU, float fPatchTextureV, 
					DWORD dwMainID, DWORD dwPatchID);
	// Add action of generating part texture
	bool AddActionPartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, 
							  DWORD dwSeed, float fBorderScope, int nNumPoints, POINT_FLOAT* pPoints, int nTextureType);
	// Add action of generating part texture info
	bool AddActionPartTextureInfo(float fMainTextureU, float fMainTextureV, 
					float fPatchTextureU, float fPatchTextureV, 
					DWORD dwMainID, DWORD dwPatchID);
	// Add action of generating water
	bool AddActionWater(float fWaterHeight, float fSandWidth, float fWaveSize);
	// Add action of generating water texture info
	bool AddActionWaterTextureInfo(float fMainTextureU, float fMainTextureV, 
					float fPatchTextureU, float fPatchTextureV, 
					DWORD dwMainID, DWORD dwPatchID);
	// Add action of generating road
	bool AddActionRoad(float fRoadWidth, float fRoadHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nFlatRoad);
	// Add action of generating road texture info
	bool AddActionRoadTextureInfo(float fMainTextureU, float fMainTextureV, 
					float fPatchTextureU, float fPatchTextureV, 
					DWORD dwMainID, DWORD dwPatchID);
	// Add action of generating polygon forest
	bool AddActionForestPoly(int nPlantType, float fDensityX, float fDensityZ, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints);
	// Add action of generating linear forest
	bool AddActionForestLine(float fSpace, float fSpaceNoise, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints);
	// Add action of grass poly
	bool AddActionGrassPoly(int nGrassKind, float fDensity,
				int nGrassTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints);
	// Add action of terrain flat
	bool AddActionTerrainFlat(int nNumPoints, POINT_FLOAT* pPoints);
	// Add action of building
	bool AddActionBuilding(DWORD dwID, float fPosX, float fPosY, float fPosZ, float fRotateY);
	// Add action of sky
	bool AddActionSky(DWORD dwDayID, DWORD dwNightID);
	// Add action of light
	bool AddActionLight(int nNameLength, const char* szName, float fPosX, float fPosY, float fPosZ, 
				DWORD dwColor, DWORD dwAmbient, float fRange, float fFallOff,
				float fAttenuation0, float fAttenuation1, float fAttenuation2, DWORD dwStatus);
	// Add action of fog
	bool AddActionFog(bool bFogEnableDay, float fFogStartDay, float fFogEndDay, DWORD dwFogColorDay,
				bool bFogEnableNight, float fFogStartNight, float fFogEndNight, DWORD dwFogColorNight, DWORD dwFogColorWater);

	// Change action of generating hill poly
	bool ChangeActionHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth,
					DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, bool bHillUp, float fTextureRatio, float fMainTexture, float fPatchTexture, 
					float fPatchDivision, float fBridgeScope, float fBridgeDepth, int nNumPoints, POINT_FLOAT* pPoints, float fBorderErode, int nIndex);
	// Change action of generating hill line
	bool ChangeActionHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, int nNumCurveLinePoint, float* pCurveLinePoints, bool bHillUp, int nNumPoints, POINT_FLOAT* pPoints,
				float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, float fBridgeScope, float fBorderErode, int nIndex);
	// Change action of generating part texture
	bool ChangeActionPartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, 
							  DWORD dwSeed, float fBorderScope, int nNumPoints, POINT_FLOAT* pPoints, int nTextureType, int nIndex);
	// Change action of generating road
	bool ChangeActionRoad(float fRoadWidth, float fRoadHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nFlatRoad, int nIndex);
	// Change action of generating polygon forest
	bool ChangeActionForestPoly(int nPlantType, float fDensityX, float fDensityZ, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex);
	// Change action of generating linear forest
	bool ChangeActionForestLine(float fSpace, float fSpaceNoise, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex);
	// Change action of grass poly
	bool ChangeActionGrassPoly(int nGrassKind, float fDensity,
				int nGrassTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex);
	// Change action of terrain flat
	bool ChangeActionTerrainFlat(int nNumPoints, POINT_FLOAT* pPoints, int nIndex);

	// Save all actions
	bool SaveActions(const char* szFile);
	// Load actions
	bool LoadActions(const char* szFile);
	// Save actions to a memory
	bool SaveActionsToMemory(char** pMemAction, DWORD* pMemSize);
	// Load actions from a memory
	bool LoadActionsFromMemory(const char* pMemAction, DWORD dwMemSize);
	// Do actions
	bool DoActions();
	// Get all action count
	int GetActionCount();
	// Clear all actions
	void ClearAllActions();
	// Clear all hill actions
	void ClearAllHillActions();
	// Clear all part texture actions
	void ClearAllPartTextureActions();
	// Clear all road actions
	void ClearAllRoadActions();
	// Clear all forest actions
	void ClearAllForestActions();
	// Clear all grass actions
	void ClearAllGrassActions();
	// Clear all terrain flat actions
	void ClearAllTerrainFlatActions();
	// Clear all building actions
	void ClearAllBuildingActions();
	// Clear all light actions
	void ClearAllLightActions();
	// Release
	void Release();
	// Delete a action
	bool DeleteAction(int type, int nIndex);

	// Do hill actions
	bool DoHillActions();
	// Do part texture actions
	bool DoPartTextureActions();
	// Do road actions
	bool DoRoadActions();
	// Redo hill poly action
	bool RedoHillPolyAction();
	// Redo hill line action
	bool RedoHillLineAction();
	// Redo part texture action
	bool RedoPartTextureAction();
	// Redo road action
	bool RedoRoadAction();
	// Do forest poly actions
	bool DoForestPolyActions();
	// Redo forest poly action
	bool RedoForestPolyAction();
	// Do forest line actions
	bool DoForestLineActions();
	// Redo forest line action
	bool RedoForestLineAction();
	// Do grass poly actions
	bool DoGrassPolyActions();
	// Redo grass poly action
	bool RedoGrassPolyAction();
	// Do terrain flat actions
	bool DoTerrainFlatActions();
	// Redo terrain flat action
	bool RedoTerrainFlatAction();
	// Do building actions
	bool DoBuildingActions();
	// Redo building action
	bool RedoBuildingAction();
/*	// Do light actions
	bool DoLightActions(CAutoLight* pLight);
	// Redo light action
	bool RedoLightAction(CAutoLight* pLight);
*/
	// Action undo
	bool ActionUndo(TERRAINACTIONTYPE type);
	// Action redo
	bool ActionRedo(TERRAINACTIONTYPE type);
	// Is undo posible?
	bool IsUndoPosible(TERRAINACTIONTYPE type);
	// Is redo posible?
	bool IsRedoPosible(TERRAINACTIONTYPE type);
	// Clear nodes after current position
	bool ClearRedoNodes(TERRAINACTIONTYPE type);

	// Create Building actions
	bool CreateBuildingActions();
/*	// Create light actions
	bool CreateLightActions(CAutoLight* pLight);
	// Create fog action
	bool CreateFogAction();
*/	
	// Get the area points in an action
	bool GetAreaPointsInAction(int nActionType, int nIndex, int* pNumPoints, POINT_FLOAT** pPoints);

	inline void SetUpDownPri(int nPri) { m_nUpDownPri = nPri; }
	inline int GetUpDownPri() { return m_nUpDownPri; }

	inline AArray<HILLPOLY, HILLPOLY&>* GetActionHillPoly() { return &m_aHillPoly; }
	inline AArray<HILLLINE, HILLLINE&>* GetActionHillLine() { return &m_aHillLine; }
	inline AArray<PARTTEXTURE, PARTTEXTURE&>* GetActionPartTexture() { return &m_aPartTexture; }
	inline AArray<ROAD, ROAD&>* GetActionRoad() { return &m_aRoad; }
	inline AArray<FORESTPOLY, FORESTPOLY&>* GetActionForestPoly() { return &m_aForestPoly; }
	inline AArray<FORESTLINE, FORESTLINE&>* GetActionForestLine() { return &m_aForestLine; }
	inline AArray<GRASSPOLY, GRASSPOLY&>* GetActionGrassPoly() { return &m_aGrassPoly; }
	inline AArray<TERRAINFLAT, TERRAINFLAT&>* GetActionTerrainFlat() { return &m_aTerrainFlat; }
	inline AArray<BUILDING, BUILDING&>* GetActionBuilding() { return &m_aBuilding; }
	inline AArray<LIGHT, LIGHT&>* GetActionLight() { return &m_aLight; }
	inline int GetNumActionHillPoly() { return m_aHillPoly.GetSize(); }
	inline int GetNumActionHillLine() { return m_aHillLine.GetSize(); }
	inline int GetNumActionPartTexture() { return m_aPartTexture.GetSize(); }
	inline int GetNumActionRoad() { return m_aRoad.GetSize(); }
	inline int GetNumActionForestPoly() { return m_aForestPoly.GetSize(); }
	inline int GetNumActionForestLine() { return m_aForestLine.GetSize(); }
	inline int GetNumActionGrassPoly() { return m_aGrassPoly.GetSize(); }
	inline int GetNumActionTerrainFlat() { return m_aTerrainFlat.GetSize(); }
	inline int GetNumActionBuilding() { return m_aBuilding.GetSize(); }
	inline int GetNumActionLight() { return m_aLight.GetSize(); }

protected:
	CAutoHome*				m_pAutoHome;			// Auto home point
	BASETERRAIN				m_anBaseTerrain;		// Base terrain
	TEXTUREINFO				m_anBaseTextureInfo;	// Base texture info action
	AArray<HILLPOLY, HILLPOLY&>			m_aHillPoly;	// Hill poly
	AArray<HILLLINE, HILLLINE&>			m_aHillLine;	// Hill line
	TEXTUREINFO				m_anHillTextureInfo;	// Hill texture info action
	AArray<PARTTEXTURE, PARTTEXTURE&>	m_aPartTexture;	// Part texture
	TEXTUREINFO				m_anPartTextureInfo;	// Part texture info action
	WATER					m_anWater;				// Water action
	TEXTUREINFO				m_anWaterTextureInfo;	// Water texture info action
	AArray<ROAD, ROAD&>					m_aRoad;		// Road action
	TEXTUREINFO				m_anRoadTextureInfo;	// Road texture info action
	AArray<FORESTPOLY, FORESTPOLY&>		m_aForestPoly;	// Forest poly action
	AArray<FORESTLINE, FORESTLINE&>		m_aForestLine;	// Forest line action
	AArray<GRASSPOLY, GRASSPOLY&>		m_aGrassPoly;	// Grass poly action
	AArray<BUILDING, BUILDING&>			m_aBuilding;	// Building action
	AArray<TERRAINFLAT, TERRAINFLAT&>	m_aTerrainFlat;	// Terrain flat action
	SKY						m_anSky;				// Sky action
	AArray<LIGHT, LIGHT&>				m_aLight;		// Light action
	FOG						m_anFog;				// Fog action

	bool					m_bAddBaseTerrain;		// Is add base terrain?
	bool					m_bAddBaseTextureInfo;	// Is add base texture info?
	bool					m_bAddHillTextureInfo;	// Is add hill texture info?
	bool					m_bAddPartTextureInfo;	// Is add part texture info?
	bool					m_bAddWater;			// Is add water?
	bool					m_bAddWaterTextureInfo;	// Is add water texture info?
	bool					m_bAddRoadTextureInfo;	// Is add road texture info?
	bool					m_bAddSky;				// Is add sky?
	bool					m_bAddFog;				// Is add fog?

	int						m_nUpDownPri;			// Terrain up and down PRI

	int						m_nHillPolyNodePos;		// Current hill poly node position
	int						m_nHillLineNodePos;		// Current hill line node position
	int						m_nPartTextureNodePos;	// Current part texture node position
	int						m_nRoadNodePos;			// Current road node position
	int						m_nForestPolyNodePos;	// Current forest poly node position
	int						m_nForestLineNodePos;	// Current forest line node position
	int						m_nGrassPolyNodePos;	// Current grass poly node position
	int						m_nBuildingNodePos;		// Current building node position
	int						m_nTerrainFlatNodePos;	// Current terrain flat node position
	int						m_nLightNodePos;		// Current light node position

protected:
	// Do one hill poly action
	bool DoHillPolyAction(HILLPOLY& action);
	// Do one hill line action
	bool DoHillLineAction(HILLLINE& action);
	// Do one part texture action
	bool DoPartTextureAction(PARTTEXTURE& action);
	// Do one road action
	bool DoRoadAction(ROAD& action);
	// Do forest poly action
	bool DoForestPolyAction(FORESTPOLY& action);
	// Do forest line action
	bool DoForestLineAction(FORESTLINE& action);
	// Do grass poly action
	bool DoGrassPolyAction(GRASSPOLY& action);
	// Do terrain flat action
	bool DoTerrainFlatAction(TERRAINFLAT& action);
	// Do building action
	bool DoBuildingAction(BUILDING& action);
	// Do one light action
//	bool DoLightAction(CAutoLight* pLight, LIGHT& action);
};

#endif // #ifndef _AUTOHOMEACTION_H_