/*
 * FILE: AutoBuilding.h
 *
 * DESCRIPTION: Class for automaticlly generating building
 *
 * CREATED BY: Jiang Dalong, 2005/05/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOBUILDING_H_
#define _AUTOBUILDING_H_

#include <A3DTypes.h>
#include <AArray.h>
#include <A3DGeometry.h>

struct MODELSTATUS
{
	DWORD		dwTypeID;		// ID of building type, may be same as other models
	DWORD		dwAttribute;	// Model attribute
	DWORD		dwUniqueID;		// Unique ID. Different for each model
	A3DVECTOR3	vPos;			// Building pos
	float		fRotateY;		// Rotate angle by Y axis
	A3DOBB		obbModel;		// OBB for model
	bool		bInit;			// Is initialized? (lighting and obb)
};

class CAutoHome;
class CECOrnamentMan;
class CELBuildingWithBrush;
class A3DViewport;

class CAutoBuilding   
{
public:
	enum MODELATTRIBUTE
	{
		MA_HOUSE		= 0,		// 房屋
		MA_OUTDOOR		= 1,		// 室外摆设
		MA_FURNITURE	= 2,		// 家具
		MA_PLANT		= 3,		// 植物
		MA_TOY			= 4,		// 玩具
		MA_VIRTU		= 5,		// 古董
		MA_OTHER		= 0xff,		// 其他或错误
	};

public:
	CAutoBuilding(CAutoHome* pAutoHome);
	virtual ~CAutoBuilding();

public:
	void Release();
	bool Tick(DWORD dwTickTime);
	bool Render(A3DViewport* pA3DViewport);

	void DeleteAllBuildings();
	// Load model by ID
	bool LoadBuildingByID(DWORD dwTypeID, const A3DVECTOR3& vPos, float fRotateY);
	// Set model pos
	bool SetBuildingPosByID(DWORD dwUniqueID, const A3DVECTOR3& vPos);
	// Delete a building
	bool DeleteBuildingByID(DWORD dwUniqueID);
	// Translate building
	bool TranslateBuildingByID(DWORD dwUniqueID, const A3DVECTOR3& vTrans);
	// Rotate building
	bool RotateBuildingByID(DWORD dwUniqueID, float fRotateY);
	// Set model pos
	bool SetBuildingPosByIndex(int nIndex, const A3DVECTOR3& vPos);
	// Delete a building
	bool DeleteBuildingByIndex(int nIndex);
	// Translate building
	bool TranslateBuildingByIndex(int nIndex, const A3DVECTOR3& vTrans);
	// Rotate building
	bool RotateBuildingByIndex(int nIndex, float fRotateY);
	// Translate building obb
	bool TranslateOBB(int nIndex, const A3DVECTOR3& vTrans);
	// Rotate building obb
	bool RotateOBB(int nIndex, A3DMATRIX4& matRoate);
	// Set obb dir with building
	bool SetOBBDirWithBuilding(int nIndex);

	// Get building position
	bool GetBuildingPos(int nIndex, A3DVECTOR3& vPos);

	// Ray trace model
	int RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vTracePos);

	// Calculate building light
	bool CalculateBuildingColor(int nIndex);

	inline int GetNumBuildings() { return m_aModelStatus.GetSize(); }
	inline MODELSTATUS& GetModelStatus(int nIndex) { return m_aModelStatus[nIndex]; }
	inline CECOrnamentMan* GetOrnamentMan() { return m_pOrnamentMan; }


protected:
	CAutoHome*			m_pAutoHome;
	CECOrnamentMan*		m_pOrnamentMan;
	
	AArray<MODELSTATUS, MODELSTATUS&>	m_aModelStatus;

protected:
	// Get ELBuilding by ID
	CELBuildingWithBrush* GetELBuilding(DWORD dwUniqueID);
	// Get index by ID
	int GetIndexByID(DWORD dwUniqueID);
	// Calculate building light
	bool CalculateBuildingColor(CELBuildingWithBrush* pELBuilding);
	// Check building status
	bool CheckBuildingStatus();
};

#endif // #ifndef _AUTOBUILDING_H_
