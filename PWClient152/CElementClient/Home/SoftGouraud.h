//-----------------------------------------
//FileName: SofGouraud.h
//
//Project:SofGouraud.h
//
//Desc:
//
//Created by:Liyi 2005/5/11
//-----------------------------------------

#ifndef _SOFT_GOURAUD_H_
#define _SOFT_GOURAUD_H_

#include <ABaseDef.h>
#include <AString.h>
#include <A3DTypes.h>
#include <vector.h>
#include <A3DVertex.h>
#include <A3DVector.h>
#include <A3DGeometry.h>

class CAutoHome;
class CELBuildingWithBrush;

class CSoftGouraud
{

public:
	CSoftGouraud();
	virtual ~CSoftGouraud();

	void Clear();
	bool Create(CAutoHome* i_pAutoHome, const A3DVECTOR3 &i_vSunDir,
		 DWORD i_dwSunColor, DWORD i_dwAmbient, DWORD i_dwSunColorNight, DWORD i_dwAmbientNight);
	void Destroy();

	struct Vertex_t
	{
		//A3DVECTOR3 vPos;
		A3DVECTOR3 vNormal;
		A3DVECTOR3 vClrDay;
		A3DVECTOR3 vClrNight;
	};

	void CalculateSceneVertsColor(DWORD** pColorDay, DWORD** pColorNight);

	bool CalculateTerrainVertsColor();
//	bool CalculateBuildingsVertsColor();
	
	bool CalculateBuildingVertsColor(CELBuildingWithBrush* pELBuilding);
protected:
	//地形
	bool CreateTerrainStream();
	void ReleaseTerrainStream();
/*	
	//全部建筑
	void CreateBuildingStream();
	void ReleaseBuildingStream();
	void FillBuildingsDiffColor();
*/
	//单个建筑
	void CreateBuildingVB(CELBuildingWithBrush* pELBuilding);
	void ReleaseBuildingVB();
	void FillBuildingDiffColor(CELBuildingWithBrush* pELBuilding);

protected:

	CAutoHome*			m_pAutoHome;

	//Light
	A3DVECTOR3			m_vSunDir;
	A3DVECTOR3			m_vSunColorDay;
	A3DVECTOR3			m_vSunColorNight;
	A3DVECTOR3			m_vAmbientColorDay;
	A3DVECTOR3			m_vAmbientColorNight;

	//terrain
	A3DVERTEX *			m_pTerrainVerts;			// terrain verts
	int					m_nGridX;
	int					m_nGridZ;
	int					m_nTerrainVertsNum;
	
	DWORD*				m_pColorBufDay;
	DWORD*				m_pColorBufNight;
	
	//buildings
	typedef abase::vector<Vertex_t*> VertsVector_t;
	typedef abase::vector<Vertex_t*>::iterator VertsVectorIter_t;
	VertsVector_t m_vecBuildingVertexBuffer;
	
	typedef abase::vector<int> IntVector_t;
	IntVector_t m_vecBuildingVertexNum;

	//building
	VertsVector_t m_vecVerts;
	IntVector_t m_vecVertsNum;
};


#endif