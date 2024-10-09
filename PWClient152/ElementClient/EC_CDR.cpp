 /*
 * FILE: EC_CDR.cpp
 *
 * DESCRIPTION:  A set of interfaces define the moving strategy of the avatar.
 *							Which focus on collision detection and give an appropriate response.
 *
 * CREATED BY: He wenfeng, 2005/3/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */


//#define  CDR_DEBUG


#include "EC_Global.h"
#include "EC_CDR.h"
#include "EC_ManOrnament.h"
#include "EC_ManNPC.h"
#include "EC_ManMatter.h"
#include "EC_World.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Instance.h"
#include "A3DTerrain2.h"
#include <A3DCollision.h>


#ifdef CDR_DEBUG
#include <A3DEngine.h>
#include <A3DFont.h>

char    msg[200];
#define OUTPUT_DEBUG_INFO(s)   OutputDebugStringA(s)
#endif


#include "aabbcd.h"

#define SLOPE_Y_THRESH_30DEGREE 0.866f
#define SLOPE_THRESH	0.5f		// 60 degree

#define FLY_MAX_HEIGHT	800.0f		// 飞行的最大高度！

//#define  CDR_ACCUM_DIST

// change this array when some new submap can go!
const unsigned int available_maps[11][8] = 
{
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 0, 0, 0, 0, 1, 1, 0},
	{1, 1, 0, 0, 0, 0, 0, 0},
};

const unsigned int available_maps4x4[4][4] = 
{
	{0, 0, 0, 0},
	{0, 1, 1, 0},
	{0, 1, 1, 0},
	{0, 0, 0, 0},
};

const unsigned int available_maps3x3[3][3] = 
{
	{0, 0, 0},
	{0, 1, 0},
	{0, 0, 0},
};

const unsigned int available_maps2x2[2][2] = 
{
	{1, 1},
	{0, 0},
};

const unsigned int available_maps_137[3][6] = 
{
	{0, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 0, 0},
	{0, 0, 0, 0, 0, 0},
};

const unsigned int available_maps_161[3][4] = 
{
	{0, 0, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 0, 0},
};

const unsigned int available_maps_162[3][4] = 
{
	{0, 0, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 0, 0},
};

const unsigned int available_maps_163[4][4] = 
{
	{0, 0, 0, 0},
	{0, 1, 1, 0},
	{0, 1, 0, 0},
	{0, 0, 0, 0},
};

//////////////////////////////////////////////////////////////////////////
// Note by wenfeng, 05-09-16
//		This function is only for the Big world but not applicable for the 
//		Instance world!
//
//////////////////////////////////////////////////////////////////////////
bool IsPosInAvailableMap(const A3DVECTOR3& vPos)
{
	float	x, z;
	int		su, sv;
	
	bool bFlag = true;
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	if( pWorld )
	{
		int idInst = pWorld->GetInstanceID();
		CECInstance* pInst = g_pGame->GetGameRun()->GetInstance(idInst);
		if (!pInst)
			return false;

		x = vPos.x + pInst->GetColNum() * 512.0f;
		z = pInst->GetRowNum() * 512.0f - vPos.z;
		su = (int)(x / 1024.0f);
		sv = (int)(z / 1024.0f);

		if (su >= pInst->GetColNum() || su < 0 || 
			sv >= pInst->GetRowNum() || sv < 0)
			return false;

		switch (idInst)
		{
		case 1:
			return available_maps[sv][su] != 0 && vPos.x <= 3877.0f;	//	禁止胧族临近看到地图边缘

		case 121:
		case 122:

			return available_maps4x4[sv][su] ? true : false;

		case 118:
		case 119:
		case 120:
		case 123:
		case 125:

			return available_maps3x3[sv][su] ? true : false;

		case 134:
			
			return available_maps2x2[sv][su] ? true : false;

		case 137:
			return available_maps_137[sv][su] ? true : false;

		case 161:
			return available_maps_161[sv][su] != 0;

		case 162:
			return available_maps_162[sv][su] ? true : false;

		case 163:
			return available_maps_163[sv][su] ? true : false;

		default:
			return true;
		}
	}
	else 
		return true;
}


bool RayTraceEnv(
						  const A3DVECTOR3& vStart, 
						  const A3DVECTOR3& vDelta,
						  A3DVECTOR3& vHitPos,
						  A3DVECTOR3& vNormal,
						  float& fraction
						  )
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	CECOrnamentMan* pOrnMan=pWorld->GetOrnamentMan();

	return pOrnMan->RayTrace(vStart,vDelta,vHitPos,&fraction,vNormal);
}

inline void GetTerrainInfo(const A3DVECTOR3& vPos, A3DVECTOR3& vPosOnSurface, A3DVECTOR3& vNormal)
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	A3DTerrain2* pTerrain=pWorld->GetTerrain();
	vPosOnSurface=vPos;
	vPosOnSurface.y=pTerrain->GetPosHeight(vPos,&vNormal);
}

inline void GetWaterAreaInfo(const A3DVECTOR3& vPos, A3DVECTOR3& vPosOnSurface, A3DVECTOR3& vNormal)
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	vPosOnSurface = vPos;
	vPosOnSurface.y = pWorld->GetWaterHeight(vPos);
	vNormal = g_vAxisY;
}

// 向下 Trace 地形和建筑，并返回第一个碰撞点的情况
bool VertRayTrace(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY)
{
	A3DVECTOR3 vTerrainPos, vTerrainNormal, vBuildingPos, vBuildingNormal;
	float fraction;

	GetTerrainInfo(vPos,vTerrainPos,vTerrainNormal);
	if(RayTraceEnv(vPos,-DeltaY*g_vAxisY,vBuildingPos,vBuildingNormal,fraction))
	{
		if(vBuildingPos.y>vTerrainPos.y)
		{
			// 与建筑发生了碰撞
			vHitPos=vBuildingPos;
			vHitNormal=vBuildingNormal;
			return true;
		}
	}
	
	//@note: kuiwu  fix the bug 2005/8/10

	//if(vTerrainPos.y<=vPos.y && vTerrainPos.y>=vPos.y-DeltaY)
	
	if (vTerrainPos.y > vPos.y || (vTerrainPos.y<=vPos.y && vTerrainPos.y>=vPos.y-DeltaY) ) 
	{
	
		vHitPos=vTerrainPos;
		vHitNormal=vTerrainNormal;
		return true;
	}
	return false;
}

// 仅仅向下 Trace 建筑
bool VertRayTraceEnv(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY)
{
	float fraction;
	return RayTraceEnv(vPos,-DeltaY*g_vAxisY,vHitPos,vHitNormal,fraction);
}



bool AABBCollideWithBrush(CHBasedCD::BrushTraceInfo * pInfo)
{
	CECWorld * pWorld = g_pGame->GetGameRun()->GetWorld();

	CECOrnamentMan* pOrnMan=pWorld->GetOrnamentMan();
	bool bBrush = pOrnMan->TraceWithBrush(pInfo);

	CHBasedCD::BrushTraceInfo info = *pInfo;
	CECMatterMan* pMatterMan = pWorld->GetMatterMan();
	if( pMatterMan->TraceWithBrush(&info) && info.fFraction < pInfo->fFraction )
	{
		*pInfo = info;
		bBrush = true;
	}

	info = *pInfo;
	CECNPCMan* pNPCMan = pWorld->GetNPCMan();
	if( pNPCMan->TraceWithBrush(&info) && info.fFraction < pInfo->fFraction )
	{
		*pInfo = info;
		bBrush = true;
	}

	return bBrush;
}



//@desc : used to retrieve support plane (ground or brush),  By Kuiwu[12/9/2005]
struct ground_trace_t 
{
	A3DVECTOR3 vStart;
	A3DVECTOR3 vExt;
	float      fDeltaY;   //down (-y)

	A3DVECTOR3 vEnd;
	A3DVECTOR3 vHitNormal;
	bool       bSupport;  //false if ground missed
};

/*
 * ray: origin + t* dir,  triangle:  p(u,v) = (1-u -v)vert[0] + u*vert[1] + v*vert[2]
 * @desc :
 * @param vDir: normalized direction     
 * @param bCull: cull back face if true
 * @return :
 * @note:
 * @todo:  
 * @author: kuiwu [8/10/2005]
 * @ref:  Tomas Moller's JGT code
 */
bool   RayTriangleIntersect(const  A3DVECTOR3& vOrigin, const A3DVECTOR3& vDir,   A3DVECTOR3 *  vert[3], float& t, float& u, float& v, bool bCull)
{
	// find vectors for two edges sharing vert0 
	A3DVECTOR3 edge1( (*vert[1]) - (*vert[0]));
	A3DVECTOR3 edge2( (*vert[2]) - (*vert[0]));
	// begin calculating determinant - also used to calculate U parameter 
	A3DVECTOR3 pvec = CrossProduct(vDir, edge2);
	// if determinant is near zero, ray lies in plane of triangle 
	float det = DotProduct(edge1, pvec);
#define   LOCAL_EPSILON   1e-5f
	if (bCull)
	{
		if(det<LOCAL_EPSILON)
			return false;
		// From here, det is > 0. 
		// Calculate distance from vert0 to ray origin
		A3DVECTOR3 tvec(vOrigin - (*vert[0]));
		// Calculate U parameter and test bounds
		u = DotProduct(tvec, pvec);
		if ((u  < 0.0f) || (u > det))
			return false;
		
		// prepare to test V parameter 
		A3DVECTOR3 qvec = CrossProduct(tvec, edge1);
		// calculate V parameter and test bounds 
		v = DotProduct(vDir, qvec);
		if ((v < 0.0f) || (u + v > det))
			return false;
		
	     // calculate t, ray intersects triangle 
		t = DotProduct(edge2, qvec) ;
		// Det > 0 so we can early exit here
		// Intersection point is valid if distance is positive 
		// (else it can just be a face behind the orig point)
		if (t < 0.0f)
		{
			return false;
		}
		
		float OneOverDet = 1.0f / det;
		 
		t *= OneOverDet;
		u *= OneOverDet;
		v *= OneOverDet;
	}
	else
	{
		if(det>-LOCAL_EPSILON && det<LOCAL_EPSILON)
			return false;
		float OneOverDet = 1.0f / det;
		// Calculate distance from vert0 to ray origin
		A3DVECTOR3 tvec(vOrigin - (*vert[0]));
		// calculate U parameter and test bounds 
		u = DotProduct(tvec, pvec)* OneOverDet;
		if ((u < 0.0f) || (u > 1.0f))
			return false;
		// prepare to test V parameter 
		A3DVECTOR3 qvec = CrossProduct(tvec, edge1);
		// calculate V parameter and test bounds 
		v = DotProduct(vDir, qvec) * OneOverDet;
	    if ((v < 0.0f) || (u + v > 1.0f))
			return false;
		// calculate t, ray intersects triangle 
		t = DotProduct(edge2, qvec) * OneOverDet;
	}

#undef    LOCAL_EPSILON
	
	return true;
}



/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [9/10/2005]
 * @ref:
 */
bool SegmentTriangleIntersect( const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta,  A3DVECTOR3 * vert[3], float& fFraction, bool bCull)
{
	float dist;
	A3DVECTOR3 vDir(vDelta);

	dist = vDir.Normalize();
	if (dist < 1E-5f)
	{
		assert(0 && "too small dist!");
		fFraction = 0.0f;
		return true;
	}

	float t, u, v;

	if (RayTriangleIntersect(vStart, vDir, vert, t, u, v, bCull) && (t>=0.0f) && (t <= dist))
	{
		//fFraction = t/dist;
		//fFraction = a_Max( 0.0f, fFraction -1E-4f); //put back
		fFraction  = (t- 5E-4f)/dist;
		a_ClampFloor(fFraction, 0.0f);
		return true;
	}

	return false;
}



/*
 *
 * @desc :
 * @param  bWaterSolid:   true, treat water and water plane as a solid; 
						  otherwise treat air and water plane as a solid.
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/10/2005]
 * @ref:
 */
bool CollideWithWater(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, bool bWaterSolid, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart )
{


#define LOCAL_EPSILON  1E-4f
	if (bWaterSolid && (vDelta.y > LOCAL_EPSILON ))
	{
		return false;
	}

	if ( !bWaterSolid && (vDelta.y < -LOCAL_EPSILON))
	{
		return false;
	}

	if (vDelta.y < LOCAL_EPSILON && vDelta.y > -LOCAL_EPSILON)
	{ //parallel  the water plane
		return false;
	}
	

	CECWorld * pWorld = g_pGame->GetGameRun()->GetWorld();
	float h0 = pWorld->GetWaterHeight(vStart);
	float h1 = pWorld->GetWaterHeight(vStart+vDelta);
	float hWater = a_Max(h0, h1);
	fFraction = 100.0f;

	vHitNormal = g_vAxisY;
	bStart = false;
	
	float t=  (hWater - vStart.y)/vDelta.y;
	if(t >= 0.0f && t<= 1.0f)
	{
		fFraction = a_Max(0.0f , t - 1E-2f);
		if (bWaterSolid && h0 > vStart.y)
		{
			fFraction = 0.0f;
			bStart = true;
		}

		if (!bWaterSolid && h0 < vStart.y)
		{
			fFraction = 0.0f;
			bStart = true;
		}

#ifdef CDR_DEBUG
	sprintf(msg, "water test startH %f deltaH %f fFraction %f h0 %f h1 %f\n", vStart.y, vDelta.y, fFraction, h0, h1);
	OUTPUT_DEBUG_INFO(msg);
#endif
		return true;
	}

#undef  LOCAL_EPSILON
	return false;
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [9/10/2005]
 * @ref:
 */
bool CollideWithTerrain(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart)
{

	CECWorld * pWorld = g_pGame->GetGameRun()->GetWorld();
	A3DTerrain2* pTerrain=pWorld->GetTerrain();
	bStart = false;
	float h1 = pTerrain->GetPosHeight(vStart, &vHitNormal);
	if (h1 > vStart.y + 1E-4f )
	{//start under terrain
		bStart = true;
		fFraction = 0.0f;
		return true;
	}


	int nWid, nHei; // in grid, 2 meters
	float fMag = vDelta.Magnitude();
	nWid = (int)ceil(fMag / 2.0f);
	nWid = a_Max(3, nWid);
	nHei = nWid;
	int nTriangles = nWid * nHei*2;
	A3DVECTOR3 * pVerts = (A3DVECTOR3*)a_malloctemp(sizeof(A3DVECTOR3) * ((nWid +1) * (nHei +1)));
	assert(pVerts != NULL);
	memset(pVerts, 0, sizeof(A3DVECTOR3) * (nWid + 1) * (nHei + 1));
	WORD * pIndices = (WORD*)a_malloctemp(sizeof(WORD)*(nTriangles*3));
	assert(pIndices != NULL);
	memset(pIndices, 0, sizeof(WORD) * nTriangles * 3);
	if (!pTerrain->GetFacesOfArea(vStart, nWid, nHei, pVerts, pIndices))
	{
		a_freetemp(pVerts);
		a_freetemp(pIndices);
		return false;
	}
	int i;
	A3DVECTOR3 * vert[3];
	//@note : Here init the fraction.  By Kuiwu[9/10/2005]
	fFraction = 100.0f;
	float tmpFraction = fFraction;

	for (i = 0; i < nTriangles; i++)
	{
		vert[0] = pVerts +pIndices[i*3];
		vert[1] = pVerts +pIndices[i*3+1];
		vert[2] = pVerts +pIndices[i*3+2];
		A3DVECTOR3 vPt;

		//@note: Tomas Moller's JGT code  : By Kuiwu[9/10/2005]
		//@note: discard the engine version because it put back the hit point too much. By Kuiwu[13/10/2005]
//		if(CLS_RayToTriangle(vStart, vDelta, *vert[0], *vert[1], *vert[2], vPt, true, &tmpFraction)
//			&& (tmpFraction <= 1.0f)  && (tmpFraction < fFraction))
		if (SegmentTriangleIntersect(vStart, vDelta, vert, tmpFraction, true) && (tmpFraction < fFraction))
		{
			//get the triangle normal
			A3DVECTOR3 vEdge1((*vert[1]) - (*vert[0]) );
			A3DVECTOR3 vEdge2((*vert[2]) - (*vert[0]) );
			vHitNormal = CrossProduct(vEdge1, vEdge2);
			vHitNormal.Normalize();

			//@note : may be redundant, but to assure.  By Kuiwu[17/10/2005]
			A3DVECTOR3 vDir;
			Normalize(vDelta, vDir);
			if (DotProduct(vHitNormal, vDir) > 0.01f)
			{//leave the hit plane
				assert(0 && "hit a plane with same direction!");
				continue;
			}
			
			fFraction = a_Max(0.0f, tmpFraction);
		}
		
	}
	a_freetemp(pVerts);
	a_freetemp(pIndices);

#ifdef CDR_DEBUG
//	if (fFraction <= 1.0f)
//	{
//		A3DVECTOR3 vHitPos(vStart + vDelta * fFraction);
//		A3DVECTOR3 vTerNormal;
//		float fH2 = pTerrain->GetPosHeight(vHitPos, &vTerNormal);
//		sprintf(msg, "terrain hit %f %f %f normal %f %f %f trcNormal %f %f %f fFraction %f\n", vHitPos.x, vHitPos.y, vHitPos.z, 
//				vHitNormal.x, vHitNormal.y, vHitNormal.z, vTerNormal.x, vTerNormal.y, vTerNormal.z, fFraction);
//		OUTPUT_DEBUG_INFO(msg);
//
//		//vHitNormal = vTerNormal;
//	}
//	else
//	{
//		A3DVECTOR3 vEnd(vStart + vDelta);
//		float fH2 = pTerrain->GetPosHeight(vEnd);
//		sprintf(msg, "terrain test clear %f %f %f height %f\n", vEnd.x, vEnd.y, vEnd.z,fH2);
//		OUTPUT_DEBUG_INFO(msg);
//	}
#endif
	return (fFraction <= 1.0f);

}
/*
 * collision detect with environment, including brush, terrain and water
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [8/10/2005]
 * @ref:
 */
bool CollideWithEnv(env_trace_t * pEnvTrc)
{
	pEnvTrc->fFraction = 100.0f;
	pEnvTrc->bStartSolid = false;
	pEnvTrc->dwClsFlag = 0;
	if ((pEnvTrc->dwCheckFlag & CDR_BRUSH) == CDR_BRUSH)
	{
		BrushTraceInfo  bruInfo;
		bruInfo.Init(pEnvTrc->vStart, pEnvTrc->vDelta, pEnvTrc->vExt);
		if (AABBCollideWithBrush(&bruInfo))
		{
			pEnvTrc->fFraction = bruInfo.fFraction;
			pEnvTrc->vHitNormal = bruInfo.ClipPlane.GetNormal();
			pEnvTrc->bStartSolid = bruInfo.bStartSolid;
			pEnvTrc->dwClsFlag = CDR_BRUSH;
		}
	}
	if ((pEnvTrc->dwCheckFlag & CDR_TERRAIN) == CDR_TERRAIN )
	{
		float fFraction;
		A3DVECTOR3 vTerNormal;
		bool bStart;
		if(CollideWithTerrain(pEnvTrc->vTerStart, pEnvTrc->vDelta, fFraction,  vTerNormal, bStart) 
			&& (fFraction < pEnvTrc->fFraction ))
		{
			assert(fFraction >= 0.0f);
			//pEnvTrc->fFraction = a_Max(0.0f, fFraction - 1E-4f);
			pEnvTrc->fFraction = fFraction ;
			pEnvTrc->vHitNormal = vTerNormal;
			pEnvTrc->bStartSolid = bStart;
			pEnvTrc->dwClsFlag = CDR_TERRAIN;
#ifdef CDR_DEBUG
//			A3DVECTOR3 vHitPos(pEnvTrc->vTerStart + pEnvTrc->vDelta * fFraction);
//			sprintf(msg, "collide terrain, fraction %f pos %f %f %f normal %f %f %f\n", fFraction, vHitPos.x, vHitPos.y, vHitPos.z, vTerNormal.x, vTerNormal.y, vTerNormal.z);
//			OUTPUT_DEBUG_INFO(msg);
#endif
		}

	}
	
	if ((pEnvTrc->dwCheckFlag & CDR_WATER) == CDR_WATER)
	{
		float fFraction;
		A3DVECTOR3 vWatNormal;
		bool bStart;
		//@todo : TBD: use center or foot? By Kuiwu[10/10/2005]
		if (CollideWithWater(pEnvTrc->vWatStart, pEnvTrc->vDelta, pEnvTrc->bWaterSolid, fFraction, vWatNormal, bStart)
			&& fFraction < pEnvTrc->fFraction)
		{
			pEnvTrc->fFraction = fFraction;
			pEnvTrc->vHitNormal = vWatNormal;
			pEnvTrc->bStartSolid = bStart;
			pEnvTrc->dwClsFlag = CDR_WATER;
#ifdef CDR_DEBUG
			A3DVECTOR3 vHitPos(pEnvTrc->vWatStart + pEnvTrc->vDelta * fFraction);
			sprintf(msg, "collide water, fraction %f pos %f %f %f \n", fFraction, vHitPos.x, vHitPos.y, vHitPos.z);
			OUTPUT_DEBUG_INFO(msg);
#endif

		}
	}

	return (pEnvTrc->fFraction < 1.0f+ 1E-4f);
}
/*
 * rescue from solid, currently do NOT handle all in solid. 
 * @desc : It mostly occurred during the ground trace, so here a simple strategy is 
		   applied: only rescue in the y direction (up, more exactly).
 * @param :     
 * @return :
 * @note:  
 * @todo: try to handle all in solid and rescue from different directions.  
 * @author: kuiwu [12/9/2005]
 * @ref: method RetrieveSupportPlane
 */
bool RescueFromSolid(BrushTraceInfo * pTrcInfo)
{
#define MAX_TRY		10
#define STEP_SIZE   0.1f
	int nTry = 0;
	A3DVECTOR3 vStart(pTrcInfo->vStart);
	A3DVECTOR3 vExt(pTrcInfo->vExtents);

	while (nTry < MAX_TRY)
	{
		vStart.y += STEP_SIZE;
		pTrcInfo->Init(vStart, A3DVECTOR3(0.0f), vExt);
		if (!AABBCollideWithBrush(pTrcInfo) ||  !pTrcInfo->bStartSolid)
		{
			break;	
		}
		nTry++;
	}

	if (nTry < MAX_TRY)
	{
		pTrcInfo->Init(vStart, A3DVECTOR3(0.0f, -(MAX_TRY*STEP_SIZE), 0.0f), vExt);
		AABBCollideWithBrush(pTrcInfo);
		return true;
	}

#undef  STEP_SIZE	
#undef  MAX_TRY
	return false;
}


/*
 *
 * @desc :
 * @param :     
 * @return : false if some error occurs during the trace, e.g. start in solid...
 * @note:  ASSUME that ext is positive!!!
 * @todo:   
 * @author: kuiwu [12/9/2005]
 * @ref:
 */
bool  RetrieveSupportPlane(ground_trace_t * pTrc)
{

	//@note : pre-condition. By Kuiwu[19/9/2005]
	assert(pTrc && pTrc->vExt.x > 0.0f 
			&& pTrc->vExt.y >0.0f && pTrc->vExt.z > 0.0f
			&& "invalid input");
	
	A3DVECTOR3 vTerrainPos, vTerrainNormal;

	GetTerrainInfo(pTrc->vStart, vTerrainPos,vTerrainNormal);
	pTrc->bSupport = false;
//	pTrc->vEnd = pTrc->vStart;
//	pTrc->vEnd.y -= pTrc->fDeltaY;
	BrushTraceInfo trcInfo;
	trcInfo.Init(pTrc->vStart, A3DVECTOR3(0.0f, -pTrc->fDeltaY, 0.0f), pTrc->vExt);
	if (AABBCollideWithBrush(&trcInfo))
	{
		
// 		if (trcInfo.bStartSolid && (!RescueFromSolid(&trcInfo)))
// 		{
// 			pTrc->bSupport = true;
// 			pTrc->vHitNormal = vTerrainNormal;
// 			pTrc->vEnd = pTrc->vStart;
// 			return false;
// 		}
		if (trcInfo.bStartSolid)
		{
			return false;
		}
		pTrc->vEnd = trcInfo.vStart + trcInfo.vDelta * trcInfo.fFraction;
		pTrc->vHitNormal = trcInfo.ClipPlane.GetNormal();
		assert(pTrc->vHitNormal.y >= 0.0f && "need an upword face");
		
		float fUp =  pTrc->vExt.y ;
		if (pTrc->vEnd.y > vTerrainPos.y + fUp)
		{
			pTrc->vStart = trcInfo.vStart;
			pTrc->bSupport = true;
			return true;
		}
	}
	//if (vTerrainPos.y > pTrc->vStart.y- pTrc->vExt.y ) 
	if (vTerrainPos.y > pTrc->vStart.y- pTrc->vExt.y || (vTerrainPos.y<= pTrc->vStart.y-pTrc->vExt.y && vTerrainPos.y>=pTrc->vStart.y-pTrc->vExt.y - pTrc->fDeltaY) ) 
	{
	
		pTrc->vEnd=vTerrainPos;
		//@note : maybe sink in the sleep ground, but this is what I need.  By Kuiwu[14/9/2005]
		pTrc->vEnd.y += (pTrc->vExt.y + 0.01f);
		A3DVECTOR3 vDelta(pTrc->vEnd - pTrc->vStart);
		trcInfo.Init(pTrc->vStart, vDelta, pTrc->vExt);
		pTrc->vHitNormal=vTerrainNormal;
		pTrc->bSupport = true;
		return !AABBCollideWithBrush(&trcInfo);
	}
	
	return true;

}

/*
 * an interface for RetrieveSupportPlane, just make the caller happy.
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [12/10/2005]
 * @ref: RetrieveSupportPlane
 */
bool VertAABBTrace(const A3DVECTOR3& vCenter, const A3DVECTOR3& vExt, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY/* =100.0f */)
{
	ground_trace_t groundTrc;
	groundTrc.vStart = vCenter;
	groundTrc.vExt = vExt;
	groundTrc.fDeltaY = DeltaY;
	vHitPos = vCenter;
	vHitPos.y -= DeltaY;
	//RetrieveSupportPlane(&groundTrc);

	if (!RetrieveSupportPlane(&groundTrc))
	{
		vHitPos = vCenter;
		vHitNormal = A3DVECTOR3(0.0f, 1.0f, 0.0f);
		return true;
	}
	if (groundTrc.bSupport)
	{
		vHitPos = groundTrc.vEnd;
		vHitNormal = groundTrc.vHitNormal;
		return true;
	}
	

	return false;
}


/*
 * add terrain-collision detection 
 * @desc :
 * @param :     
 * @return :
 * @note: 
 * @todo:  
 * @author: kuiwu [29/9/2005]
 * @history : Add moving dist. By Kuiwu[13/12/2005]
 * @ref: 
 */
void OnGroundMove(CDR_INFO& CDRInfo)
{
	//assert(0);


#define	 VEL_EPSILON     1E-4f	
#define  DIST_EPSILON    1e-4f
#define  NORMAL_EPSILON  1e-2f
#define  MAX_TRY         4
#define  VEL_MAX_SPEED   200.0f
#define  VEL_REFLECT     0.3f	

#ifdef CDR_DEBUG
	sprintf(msg, "=====================ground move2======================\n");
	OUTPUT_DEBUG_INFO(msg);
	sprintf(msg, "center %f %f %f xozspeed %f %f  yspeed %f t %f tpnormal %f %f %f\n",
		  CDRInfo.vCenter.x, CDRInfo.vCenter.y, CDRInfo.vCenter.z, CDRInfo.vXOZVelDir.x * CDRInfo.fSpeed, CDRInfo.vXOZVelDir.z * CDRInfo.fSpeed,
		  CDRInfo.fYVel, CDRInfo.t, CDRInfo.vTPNormal.x, CDRInfo.vTPNormal.y,
		  CDRInfo.vTPNormal.z);
	OUTPUT_DEBUG_INFO(msg);

//	DWORD dwTStart;
//	static DWORD dwTSum = 0;
//	static DWORD dwFrame = 0;
//	dwFrame++;	
//	dwTStart = (DWORD)(ACounter::GetMicroSecondNow());

#endif	

	CDRInfo.fMoveDist = 0.0f;  //clear the moving dist

	bool bFreeFall = (CDRInfo.vTPNormal.y < CDRInfo.fSlopeThresh);
	if (CDRInfo.fYVel < VEL_EPSILON && CDRInfo.fYVel > -VEL_EPSILON && CDRInfo.fSpeed < VEL_EPSILON && CDRInfo.fSpeed > -VEL_EPSILON && !bFreeFall ) 
	{
		return;
	}

	float fYVel = CDRInfo.fYVel;  //save the y velocity;

	//@todo : refine the speed to determine the jumping state. By Kuiwu[14/9/2005]
	bool bJump = (fYVel > 0.5f);
	//bool bJump = (fYVel > CDRInfo.fGravityAccel * 0.1f);

	A3DVECTOR3 vVelocity(CDRInfo.fSpeed*CDRInfo.vXOZVelDir + fYVel*g_vAxisY );
	
	
	if (bFreeFall) 
	{
		vVelocity -= (CDRInfo.fGravityAccel* CDRInfo.t *g_vAxisY);
		fYVel -= CDRInfo.fGravityAccel* CDRInfo.t;
	}
	A3DVECTOR3 vVelDir(vVelocity);
	float fVelSpeed = vVelDir.Normalize();
	if (!bFreeFall)
	{
		a_ClampRoof(fVelSpeed, VEL_MAX_SPEED);
	}
	vVelocity = vVelDir * fVelSpeed;
	//@note : clip the velocity or dir? By Kuiwu[8/9/2005]
	float dtp = DotProduct(vVelDir, CDRInfo.vTPNormal);
	if (dtp < 0.0f || !bJump)
	{	
		vVelocity = (vVelDir - CDRInfo.vTPNormal * dtp - CDRInfo.vTPNormal*dtp * 0.01f) * fVelSpeed;
		
	}
	


	CDRInfo.vAbsVelocity=vVelocity;	

	A3DVECTOR3 vStart(CDRInfo.vCenter);
	A3DVECTOR3 vExt(CDRInfo.vExtent);
	float  fTime = CDRInfo.t;
	//A3DVECTOR3 vDelta(vVelocity * fTime);
	A3DVECTOR3 vDelta;
	bool bClear = true; 
	int nTry = 0;
	env_trace_t  trcInfo;
	trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH;
	trcInfo.vExt = vExt;
	//A3DVECTOR3 vNormal, vFinalPos(vStart);
	A3DVECTOR3 vNormal, vFinalPos;
	bool bPull = false;
	bool bTryPull = false;
#ifdef CDR_DEBUG
	float fExpMove = fVelSpeed * fTime;
#endif	

	while (nTry < MAX_TRY) 
	{
		vDelta = vVelocity * fTime;
		vFinalPos = vStart;
		float fDeltaDist = vDelta.Magnitude();
		//if (vDelta.SquaredMagnitude() < DIST_EPSILON ) 
		if (fDeltaDist < DIST_EPSILON ) 
		{
			break;
		}
		trcInfo.vStart = vStart;
		trcInfo.vDelta = vDelta;
		trcInfo.vTerStart = vStart;
		trcInfo.vTerStart.y -= vExt.y; //foot
		bClear = !CollideWithEnv(&trcInfo);	
#ifdef CDR_DEBUG
		sprintf(msg, "start %f %f %f delta %f %f %f  frac %f bstart %d\n", 
				vStart.x,  vStart.y, vStart.z,
				vDelta.x, vDelta.y, vDelta.z,
				trcInfo.fFraction, trcInfo.bStartSolid);
		OUTPUT_DEBUG_INFO(msg);
#endif
		++nTry;
		if (trcInfo.bStartSolid)
		{
			CDRInfo.fMoveDist = 0.0f;
			if (CDRInfo.vTPNormal.y < CDRInfo.fSlopeThresh)
			{
				CDRInfo.vTPNormal = A3DVECTOR3(0.0f, 1.0f, 0.0f);
			}
			return;
		}
		if (bClear ) 
		{
			vFinalPos = vStart + vDelta;
#ifdef CDR_ACCUM_DIST
			CDRInfo.fMoveDist += (fDeltaDist);
#endif					
			break;
		}
		
		vStart += vDelta*trcInfo.fFraction;

#ifdef  CDR_ACCUM_DIST
		CDRInfo.fMoveDist +=  (fDeltaDist * trcInfo.fFraction);
#endif

		fTime -= fTime*trcInfo.fFraction;
		vNormal = trcInfo.vHitNormal;
		if (!bFreeFall && !bTryPull && !bJump )
		{
			env_trace_t tmpInfo;
			tmpInfo.vStart = vStart;
			tmpInfo.vDelta = A3DVECTOR3(0.0f, CDRInfo.fStepHeight, 0.0f);
			tmpInfo.vExt = vExt;
			//@note : need check terrain?? By Kuiwu[8/10/2005]
			tmpInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;  
			tmpInfo.vTerStart = vStart;
			tmpInfo.vTerStart.y -= vExt.y;

			bPull = !CollideWithEnv(&tmpInfo);
			if (bPull) 
			{
				vStart.y += CDRInfo.fStepHeight;
				//vDelta = vVelocity * fTime;
				vDelta = vVelocity ;
				tmpInfo.vStart =vStart;
				tmpInfo.vDelta = vDelta;
				tmpInfo.vTerStart = vStart;
				tmpInfo.vTerStart.y -= vExt.y;
				bool bMove = !CollideWithEnv(&tmpInfo);
				if (!bMove)
				{
					vDelta *= tmpInfo.fFraction;
				}
				
				if (vDelta.SquaredMagnitude() < vExt.x* vExt.x *4 )
				{
					vStart.y -= CDRInfo.fStepHeight;
					bPull = false;
				}
			}
			bTryPull = true; 
	
		}

		if (!bPull) 
		{

			fVelSpeed = Normalize(vVelocity, vVelDir);
			fVelSpeed *= (1- nTry * 0.1f);
			dtp = DotProduct(vNormal, vVelDir);
			float fRelSpeed = a_Min(fVelSpeed, 5.0f);

			if(( dtp < 1E-4f) && (dtp >= 0.0f) )
			{//@note : special parallel tangent plane case, rarely happen.  By Kuiwu[20/10/2005]
#ifdef CDR_DEBUG
			   sprintf(msg, "parallel dtp %f fraction %f velocity %f %f %f normal %f %f %f\n", dtp, trcInfo.fFraction, 
				   vVelocity.x, vVelocity.y, vVelocity.z, vNormal.x, vNormal.y, vNormal.z);
			   OUTPUT_DEBUG_INFO(msg);
#endif				
				vVelocity += vNormal* VEL_REFLECT * fRelSpeed;
			}
			else
			{
				vVelocity = (vVelDir - vNormal * dtp ) * fVelSpeed  - vNormal *dtp * VEL_REFLECT * fRelSpeed; 
				//vVelocity = (vVelDir - vNormal * dtp - vNormal*dtp * VEL_REFLECT) * fVelSpeed; 
			}
			

			//CDRInfo.fYVel = vVelocity.y;
			if (fYVel > VEL_EPSILON) 
			{
				if ((vNormal.y >= CDRInfo.fSlopeThresh || vNormal.y < -NORMAL_EPSILON))
				{
					fYVel = 0.0f;
				}
			} 
			else if(fYVel < -VEL_EPSILON)
			{
				if( (vNormal.y >= CDRInfo.fSlopeThresh))
				{
					fYVel = 0.0f;
				}
			}
			else
			{
				//@note : additional handle something???  By Kuiwu[13/9/2005]
			}
		}

	}

	//@note : prevent moving to the invalid area. By Kuiwu[20/9/2005]
	if (!IsPosInAvailableMap(vFinalPos))
	{
		CDRInfo.fMoveDist = 0.0f;
		return;
	}


	A3DVECTOR3 vTPNormal;
	vTPNormal.Clear();
	
	ground_trace_t groundTrc;
	groundTrc.vStart = vFinalPos;
	groundTrc.vExt = vExt;
	groundTrc.fDeltaY = 0.3f;
	if (bPull)
	{
		groundTrc.fDeltaY = CDRInfo.fStepHeight+0.1f;
	}
	if (bJump)
	{
		groundTrc.fDeltaY = 0.0f;
	}

#ifdef CDR_DEBUG
	sprintf(msg, "before down trace %f %f %f down %f fYVel %f\n", vFinalPos.x ,vFinalPos.y, vFinalPos.z, groundTrc.fDeltaY, fYVel);
	OUTPUT_DEBUG_INFO(msg);
#endif	
	if (!RetrieveSupportPlane(&groundTrc))
	{//@note : do NOT change position. By Kuiwu[14/9/2005]
		CDRInfo.fMoveDist = 0.0f;
// 		if (groundTrc.bSupport)
// 		{
// 			CDRInfo.vTPNormal = groundTrc.vHitNormal;
// 		}
		CDRInfo.vTPNormal = A3DVECTOR3(0.0f, 1.0f, 0.0f);
		return; 
	}
	if (groundTrc.bSupport)
	{
		vFinalPos = groundTrc.vEnd;
		if (!bJump)
		{
			vTPNormal = groundTrc.vHitNormal;
		}
	}

	
	if ((vTPNormal.y >= CDRInfo.fSlopeThresh && fYVel < 0.0f) 
			||(!bJump && fYVel > 0.0f) )
	{
		fYVel = 0.0f;
	}
	

#ifndef CDR_ACCUM_DIST
	vDelta = vFinalPos - CDRInfo.vCenter;
	CDRInfo.fMoveDist = vDelta.Magnitude();
#endif

#ifdef CDR_DEBUG
	sprintf(msg, "expect move %f, real move %f\n", fExpMove, CDRInfo.fMoveDist );
	OUTPUT_DEBUG_INFO(msg);
#endif
	CDRInfo.vCenter = vFinalPos;
	CDRInfo.fYVel = fYVel;  //set back the y velocity
	CDRInfo.vTPNormal = vTPNormal;

#ifdef CDR_DEBUG
	sprintf(msg, "out center %f %f %f tpnormal %f %f %f fyvel %f\n", CDRInfo.vCenter.x, CDRInfo.vCenter.y, CDRInfo.vCenter.z,  CDRInfo.vTPNormal.x, CDRInfo.vTPNormal.y, CDRInfo.vTPNormal.z, CDRInfo.fYVel);
	OUTPUT_DEBUG_INFO(msg);
#endif
	
#undef  VEL_EPSILON
#undef  DIST_EPSILON
#undef  NORMAL_EPSILON
#undef  MAX_TRY
#undef  VEL_MAX_SPEED
#undef  VEL_REFLECT

}


//void OnGroundMove(CDR_INFO& CDRInfo)
//{
//
//	
//	//OnGroundMoveBigAABB(CDRInfo);
//	OnGroundMoveBigAABB2(CDRInfo);
//}



/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/10/2005]
 * @ref:
 */
void AirMove(ON_AIR_CDR_INFO& awmInfo)
{

#define  DIST_EPSILON    1e-4f
#define  MAX_TRY         4
#define  VEL_REFLECT     0.0f

#ifdef CDR_DEBUG
	sprintf(msg, "=====================air move======================\n");
	OUTPUT_DEBUG_INFO(msg);
	sprintf(msg, "input center %f %f %f veldir %f %f %f speed %f t %f \n", 
			awmInfo.vCenter.x, awmInfo.vCenter.y, awmInfo.vCenter.z,
			awmInfo.vVelDir.x, awmInfo.vVelDir.y, awmInfo.vVelDir.z,
			awmInfo.fSpeed, awmInfo.t);
	OUTPUT_DEBUG_INFO(msg);
#endif
	//@note : pre-condition. By Kuiwu[20/9/2005]
	assert(awmInfo.fSpeed >= 0.0f && awmInfo.t >= 0.0f && "invalid input");

	float fTime = awmInfo.t;
	//@todo : is it necessary to clamp the speed? By Kuiwu[20/9/2005]
	float fSpeed = awmInfo.fSpeed;
	if (fSpeed * fTime < DIST_EPSILON)
	{
		//@todo : set the output param. By Kuiwu[20/9/2005]
		return;
	}

	A3DVECTOR3 vStart(awmInfo.vCenter);
	A3DVECTOR3 vExt(awmInfo.vExtent);
	A3DVECTOR3 vVelDir(awmInfo.vVelDir);
	float dtp;
	A3DVECTOR3 vVelocity(vVelDir * fSpeed);

	if ((dtp = DotProduct(vVelDir, awmInfo.vTPNormal)) < 0.0f)
	{
		
		//vVelocity = (vVelDir - awmInfo.vTPNormal * dtp - awmInfo.vTPNormal*dtp * 0.01f) * fSpeed;
		vVelocity = (vVelDir - awmInfo.vTPNormal * dtp ) * fSpeed;

#ifdef CDR_DEBUG
		sprintf(msg, "dtp %f clipvel %f %f %f supportnormal %f %f %f\n", 
			dtp, vVelocity.x, vVelocity.y, vVelocity.z,
			awmInfo.vTPNormal.x, awmInfo.vTPNormal.y, awmInfo.vTPNormal.z);
		OUTPUT_DEBUG_INFO(msg);
#endif

	}

	A3DVECTOR3 vDelta(vVelocity*fTime), vNormal, vFinalPos(vStart);
	int nTry = 0;
	bool bClear = true;
	env_trace_t  trcInfo;
	trcInfo.bWaterSolid = true;
	trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH | CDR_WATER;
	trcInfo.vExt = vExt;

	while (nTry < MAX_TRY) 
	{
		if (vDelta.SquaredMagnitude() < DIST_EPSILON ) 
		{
			break;
		}
		trcInfo.vStart = vStart;
		trcInfo.vDelta = vDelta;
		trcInfo.vTerStart = vStart;
		trcInfo.vTerStart.y -= vExt.y;
		trcInfo.vWatStart = vStart;
		trcInfo.vWatStart.y -= vExt.y;
		bClear = !CollideWithEnv(&trcInfo);	

		++nTry;
		if (bClear || trcInfo.bStartSolid) 
		{
			vFinalPos = vStart + vDelta;
			break;
		}
		vStart += vDelta*trcInfo.fFraction;
		vFinalPos = vStart;
		fTime -= fTime*trcInfo.fFraction;
		vNormal = trcInfo.vHitNormal;
		fSpeed = Normalize(vVelocity, vVelDir);
		fSpeed *= (1- nTry * 0.1f);
		dtp = DotProduct(vNormal, vVelDir);
		vVelocity = (vVelDir - vNormal * dtp - vNormal*dtp * VEL_REFLECT) * fSpeed; 
		vDelta = vVelocity * fTime;

	}

#ifdef CDR_DEBUG
	sprintf(msg, "after collision detect %f %f %f bClear %d nTry %d\n", vFinalPos.x, vFinalPos.y, vFinalPos.z, bClear, nTry);
	OUTPUT_DEBUG_INFO(msg);
#endif
	//@note : prevent moving to the invalid area. By Kuiwu[20/9/2005]
	if (!IsPosInAvailableMap(vFinalPos))
	{
		//@todo : set some flag to notify the caller? By Kuiwu[20/9/2005]
		return;
	}
	
	//too high
	if (vFinalPos.y > FLY_MAX_HEIGHT  - 2.0f)
	{
		return;
	}

	//see if meet height thresh
	ground_trace_t groundTrc;
	groundTrc.vStart = vFinalPos;
	groundTrc.vExt = vExt;
	groundTrc.fDeltaY = awmInfo.fHeightThresh + 0.1f;
	
	if (!RetrieveSupportPlane(&groundTrc))
	{
#ifdef CDR_DEBUG
		OUTPUT_DEBUG_INFO("retrive support plane fail\n");
		//assert(0 && "retrive support plane fail");
#endif
		return;
	}

	A3DVECTOR3 vTpNormal(0.0f);
	A3DVECTOR3 vOverTp(vFinalPos);
	bool bAdjust = false;
	awmInfo.bMeetHeightThresh = true;

	float fHWater = g_pGame->GetGameRun()->GetWorld()->GetWaterHeight(vFinalPos);
	if (groundTrc.bSupport)
	{
		bAdjust = true;
		vOverTp = groundTrc.vEnd;
		vTpNormal = groundTrc.vHitNormal;
		if (fHWater > groundTrc.vEnd.y)
		{
			vOverTp.y = fHWater;
			vTpNormal = g_vAxisY;
		}
	}
	else if (vFinalPos.y < fHWater + awmInfo.fHeightThresh )
	{
		bAdjust = true;
		vOverTp = vFinalPos;
		vOverTp.y = fHWater;
		vTpNormal = g_vAxisY;
	}
	
	if (bAdjust &&  (vOverTp.y + awmInfo.fHeightThresh > vFinalPos.y) )
	{
		BrushTraceInfo  brushTrc;
		vDelta.Clear();
		vDelta.y = vOverTp.y + awmInfo.fHeightThresh - vFinalPos.y;
		float fAllow = (float)fabs(awmInfo.vCenter.y - vFinalPos.y)+ 0.001f;
		fAllow = a_Max(fAllow, 0.15f);
		a_ClampRoof(vDelta.y, fAllow);
#ifdef CDR_DEBUG
		sprintf(msg, "adjust %f\n", vDelta.y);
		OUTPUT_DEBUG_INFO(msg);
#endif

		brushTrc.Init(vFinalPos, vDelta, vExt);
		if (AABBCollideWithBrush(&brushTrc))
		{
			vFinalPos += (vDelta * brushTrc.fFraction);
		}
		else
		{
			vFinalPos += vDelta;
		}
		awmInfo.bMeetHeightThresh = (vFinalPos.y - vOverTp.y > awmInfo.fHeightThresh);
	}
	
	awmInfo.vCenter = vFinalPos;
	awmInfo.vTPNormal = vTpNormal;

#ifdef CDR_DEBUG
	sprintf(msg, "out center %f %f %f tpnormal %f %f %f meetthresh %d\n", awmInfo.vCenter.x, awmInfo.vCenter.y, awmInfo.vCenter.z, awmInfo.vTPNormal.x, awmInfo.vTPNormal.y, awmInfo.vTPNormal.z, awmInfo.bMeetHeightThresh);
	OUTPUT_DEBUG_INFO(msg);
#endif	

	
#undef  DIST_EPSILON
#undef  MAX_TRY
#undef  VEL_REFLECT
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/10/2005]
 * @ref:
 */
void WaterMove(ON_AIR_CDR_INFO& awmInfo)
{
#define  DIST_EPSILON    1e-4f
#define  MAX_TRY         4
#define  VEL_REFLECT     0.1f

#ifdef CDR_DEBUG
	sprintf(msg, "=====================water move======================\n");
	OUTPUT_DEBUG_INFO(msg);
	sprintf(msg, "input center %f %f %f veldir %f %f %f speed %f t %f \n", 
			awmInfo.vCenter.x, awmInfo.vCenter.y, awmInfo.vCenter.z,
			awmInfo.vVelDir.x, awmInfo.vVelDir.y, awmInfo.vVelDir.z,
			awmInfo.fSpeed, awmInfo.t);
	OUTPUT_DEBUG_INFO(msg);
#endif
	//@note : pre-condition. By Kuiwu[20/9/2005]
	assert(awmInfo.fSpeed >= 0.0f && awmInfo.t >= 0.0f && "invalid input");

	float fTime = awmInfo.t;
	//@todo : is it necessary to clamp the speed? By Kuiwu[20/9/2005]
	float fSpeed = awmInfo.fSpeed;
	if (fSpeed * fTime < DIST_EPSILON)
	{
		//@todo : set the output param. By Kuiwu[20/9/2005]
		return;
	}

	A3DVECTOR3 vStart(awmInfo.vCenter);
	A3DVECTOR3 vExt(awmInfo.vExtent);
	A3DVECTOR3 vVelDir(awmInfo.vVelDir);
	float dtp;
	A3DVECTOR3 vVelocity(vVelDir * fSpeed);

	if ((dtp = DotProduct(vVelDir, awmInfo.vTPNormal)) < 0.0f)
	{
		
		vVelocity = (vVelDir - awmInfo.vTPNormal * dtp - awmInfo.vTPNormal*dtp * 0.01f) * fSpeed;
		//vVelocity = (vVelDir - awmInfo.vTPNormal * dtp ) * fSpeed;

#ifdef CDR_DEBUG
		sprintf(msg, "dtp %f clipvel %f %f %f supportnormal %f %f %f\n", 
			dtp, vVelocity.x, vVelocity.y, vVelocity.z,
			awmInfo.vTPNormal.x, awmInfo.vTPNormal.y, awmInfo.vTPNormal.z);
		OUTPUT_DEBUG_INFO(msg);
#endif

	}

	A3DVECTOR3 vDelta(vVelocity*fTime), vNormal, vFinalPos(vStart);
	int nTry = 0;
	bool bClear = true;
	env_trace_t  trcInfo;
	trcInfo.bWaterSolid = false;
	trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH | CDR_WATER ;
	trcInfo.vExt = vExt;

	while (nTry < MAX_TRY) 
	{
		if (vDelta.SquaredMagnitude() < DIST_EPSILON ) 
		{
			break;
		}
		trcInfo.vStart = vStart;
		trcInfo.vDelta = vDelta;
		trcInfo.vTerStart = vStart;
		trcInfo.vTerStart.y -= vExt.y;
		trcInfo.vWatStart = vStart;
		trcInfo.vWatStart.y += awmInfo.fUnderWaterDistThresh; //shoulder


		bClear = !CollideWithEnv(&trcInfo);	

		++nTry;
		if (bClear || (trcInfo.bStartSolid && ((trcInfo.dwClsFlag & CDR_WATER) != CDR_WATER)) )
		{
			vFinalPos = vStart + vDelta;


			break;
		}
		vStart += vDelta*trcInfo.fFraction;
		fTime -= fTime*trcInfo.fFraction;
		fSpeed = Normalize(vVelocity, vVelDir);
		fSpeed *= (1- nTry * 0.1f);
		if ((trcInfo.dwClsFlag & CDR_WATER) == CDR_WATER)
		{
			if (trcInfo.bStartSolid)
			{//rescue from solid
			 //@note : it may cause some problems. By Kuiwu[11/10/2005]
	  		 float fHWater = g_pGame->GetGameRun()->GetWorld()->GetWaterHeight(vStart);
			 vStart.y = fHWater;
			}
			vVelDir.y = 0.0f;
			vVelocity = vVelDir * fSpeed;
		}
		else
		{
			vNormal = trcInfo.vHitNormal;
			dtp = DotProduct(vNormal, vVelDir);
			vVelocity = (vVelDir - vNormal * dtp - vNormal*dtp * VEL_REFLECT) * fSpeed; 
		}
		vDelta = vVelocity * fTime;
		vFinalPos = vStart;

	}

#ifdef CDR_DEBUG
	sprintf(msg, "after collision detect %f %f %f bClear %d nTry %d\n", vFinalPos.x, vFinalPos.y, vFinalPos.z, bClear, nTry);
	OUTPUT_DEBUG_INFO(msg);
#endif
	//@note : prevent moving to the invalid area. By Kuiwu[20/9/2005]
	if (!IsPosInAvailableMap(vFinalPos))
	{
		//@todo : set some flag to notify the caller? By Kuiwu[20/9/2005]
		return;
	}
	

	//see if meet height thresh
	ground_trace_t groundTrc;
	groundTrc.vStart = vFinalPos;
	groundTrc.vExt = vExt;
	groundTrc.fDeltaY = awmInfo.fHeightThresh + 0.1f;
	
	if (!RetrieveSupportPlane(&groundTrc))
	{
#ifdef CDR_DEBUG
		OUTPUT_DEBUG_INFO("retrive support plane fail\n");
		assert(0 && "retrive support plane fail");
#endif
		return;
	}

	A3DVECTOR3 vTpNormal(0.0f);
	A3DVECTOR3 vOverTp(vFinalPos);
	bool bAdjust = false;
	awmInfo.bMeetHeightThresh = true;

	if (groundTrc.bSupport)
	{
		bAdjust = true;
		vOverTp = groundTrc.vEnd;
		vTpNormal = groundTrc.vHitNormal;
	}

	if (bAdjust &&  (vOverTp.y + awmInfo.fHeightThresh > vFinalPos.y) )
	{
		float fHWater = g_pGame->GetGameRun()->GetWorld()->GetWaterHeight(vFinalPos);
		float fAllow = fHWater - awmInfo.fUnderWaterDistThresh - vFinalPos.y;
		if (fAllow > 1E-4f)
		{
			vDelta.Clear();
			vDelta.y = vOverTp.y + awmInfo.fHeightThresh - vFinalPos.y;
			fAllow = a_Min(fAllow, 0.15f);
			a_ClampRoof(vDelta.y, fAllow);
			BrushTraceInfo  brushTrc;
#ifdef CDR_DEBUG
		sprintf(msg, "adjust %f\n", vDelta.y);
		OUTPUT_DEBUG_INFO(msg);
#endif
			brushTrc.Init(vFinalPos, vDelta, vExt);
			if (AABBCollideWithBrush(&brushTrc))
			{
				vFinalPos += (vDelta * brushTrc.fFraction);
			}
			else
			{
				vFinalPos += vDelta;
			}
			awmInfo.bMeetHeightThresh = (vFinalPos.y - vOverTp.y > awmInfo.fHeightThresh);
		}
	}
	
	awmInfo.vCenter = vFinalPos;
	awmInfo.vTPNormal = vTpNormal;

#ifdef CDR_DEBUG
	sprintf(msg, "out center %f %f %f tpnormal %f %f %f meetthresh %d\n", awmInfo.vCenter.x, awmInfo.vCenter.y, awmInfo.vCenter.z, awmInfo.vTPNormal.x, awmInfo.vTPNormal.y, awmInfo.vTPNormal.z, awmInfo.bMeetHeightThresh);
	OUTPUT_DEBUG_INFO(msg);
#endif	

	
#undef  DIST_EPSILON
#undef  MAX_TRY
#undef  VEL_REFLECT
}
/*
 * 
 * @desc : refactor version of OnAirMoveBigAABB, add collision detect with water & terrain
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [30/9/2005]
 * @ref:
 */
void OnAirMove(ON_AIR_CDR_INFO& awmInfo)
{
	//assert(0 && "Not ready yet");

	if (awmInfo.bOnAir)
	{
		AirMove(awmInfo);
	}
	else
	{
		WaterMove(awmInfo);
	}
}

//void OnAirMove(ON_AIR_CDR_INFO& OnAirCDRInfo)
//{
//	
//	//OnAirMoveBigAABB(OnAirCDRInfo);
//	OnAirMoveBigAABB2(OnAirCDRInfo);
//}

void OtherPlayerMove(OtherPlayer_Move_Info& OPMoveInfo)
{
	A3DVECTOR3 vDelta = OPMoveInfo.t * OPMoveInfo.vVelocity;
	OPMoveInfo.vCenter += vDelta;

	OPMoveInfo.vecGroundNormal = g_vAxisY;
	A3DVECTOR3 vGroundPos, vNormal;

	//	Now, we directly interpolate the pos, and we don't use bTraceGround
	if (OPMoveInfo.bTestTrnOnly)
	{
		GetTerrainInfo(OPMoveInfo.vCenter, vGroundPos, vNormal);
		vGroundPos.y += OPMoveInfo.vExts.y;
	
		if (OPMoveInfo.vCenter.y < vGroundPos.y + 0.1f)
			OPMoveInfo.vecGroundNormal = vNormal;

		//	verify not below the terrain
		if (OPMoveInfo.vCenter.y < vGroundPos.y)
			OPMoveInfo.vCenter.y = vGroundPos.y;
	}
	else
	{
		if (VertRayTrace(OPMoveInfo.vCenter, vGroundPos, vNormal, 3.0f))
		{
			OPMoveInfo.vecGroundNormal = vNormal;

			vGroundPos.y += OPMoveInfo.vExts.y;
	
			//	verify not below the ground
			if (OPMoveInfo.vCenter.y < vGroundPos.y)
				OPMoveInfo.vCenter.y = vGroundPos.y;
		}
	}
}

bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction)
{
	RAYTRACERT rt;
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	A3DTerrain2* pTerrain=pWorld->GetTerrain();
	fraction=1.0f;
	rt.fFraction=1.0f;
	bool bTraceEnv=RayTraceEnv(vStart,vDelta,vHitPos,vNormal,fraction);
	bool bTraceTerrain=pTerrain->RayTrace(vStart, vDelta, 1.0f, &rt);

	if(bTraceEnv)
	{
		if(bTraceTerrain && rt.fFraction<fraction)
		{
			vHitPos=rt.vHitPos;
			vNormal=rt.vNormal;
			fraction=rt.fFraction;
		}
		return true;
	}
	else
		if(bTraceTerrain)
		{
			vHitPos=rt.vHitPos;
			vNormal=rt.vNormal;
			fraction=rt.fFraction;
			return true;	
		}
		else
			return false;
	
}

#undef  CDR_BRUSH
#undef  CDR_TERRAIN
#undef  CDR_WATER

