// TerrainPlantPaintOperation.h: interface for the CTerrainPlantPaintOperation class.
//
//////////////////////////////////////////////////////////////////////
//----------------------------------------------------------
// Filename	: TerrainPlantPaintOperation.h
// Creator	: QingFeng Xin
// Date		: 2004.9.16
// Desc		: 用于地表植被的种植操作
//-----------------------------------------------------------
#if !defined(AFX_TERRAINPLANTPAINTOPERATION_H__A2BF07FA_D14E_4C94_9803_C3944832BC0B__INCLUDED_)
#define AFX_TERRAINPLANTPAINTOPERATION_H__A2BF07FA_D14E_4C94_9803_C3944832BC0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Operation.h"
#include "TerrainStretchBrush.h"
#include "TerrainPlants.h"


class CTerrainPlantPaintOperation :public COperation
{
public:
	CTerrainPlantPaintOperation();
	virtual ~CTerrainPlantPaintOperation();
	
	enum
	{
		PLANT_TREE_ADD = 0,
		PLANT_TREE_DELETE,
		PLANT_TREE_MOVE_Y,
		PLANT_TREE_MOVE_XZ,
		PLANT_GRASS_ADD,
		PLANT_GRASS_DELETE,
		PLANT_PLANT_NO,
	};

	virtual void Render(A3DViewport* pA3DViewport);
	
	//	Mouse action handler,
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);

	void SetPaintType(int type){ m_nPlantOperationType = type; };
	
	void SetPlants(PPLANT pPlant);
private:
	//这儿计算要在刷子的周围要产生的植物
	void CalculatePlants();
	void PlantsGrass();
	void ScreenTraceAxis(A3DVECTOR3 &pos, CPoint pt, A3DVECTOR3 *pXY, A3DVECTOR3 *pXZ, A3DVECTOR3 *pYZ);
	//重新选择植物到植物选择表
	void SelectedPlants();

	void MoveTree(A3DVECTOR3 vDelta);
	
	void RenderPlantGrid();
	
	void DrawRect(float x,float z,DWORD dwColor);
	
	CTerrainStretchBrush *m_pBrush;
	bool m_bLMouseDown;
	PPLANT m_pPlant;

	APtrList<PPLANTPOS> m_listSelected;
	
	// 对植物的操作类型
	int  m_nPlantOperationType;

	DWORD m_dwLastTime;
	int m_nOldX;
	int m_nOldY;
	bool m_bRenderBrush;

	A3DVECTOR3 m_vXY;
	A3DVECTOR3 m_vXZ;
	A3DVECTOR3 m_vYZ;

};

#endif // !defined(AFX_TERRAINPLANTPAINTOPERATION_H__A2BF07FA_D14E_4C94_9803_C3944832BC0B__INCLUDED_)
