/*
 * FILE: A3DMultiPlaneEmitter.h
 *
 * DESCRIPTION: MultiPlane Emitter
 *
 * CREATED BY: ZhangYu, 2004/8/13
 *
 * HISTORY:
 *
 */

#ifndef _A3DMULTIPLANEEMITTER
#define _A3DMULTIPLANEEMITTER

#include "A3DParticleEmitter.h"
#include <AFI.h>
class A3DGFXModelRef;

class A3DMultiPlaneEmitter : public A3DParticleEmitter
{
public:

	A3DMultiPlaneEmitter() :
	  m_aExternVert(0),
	  m_aExternIndex(0),
	  m_fPlanePortion(0),
	  m_pPlaneNormal(0),
	  m_nVertCount(0),
	  m_nPlaneCount(0),
	  m_fTotalArea(0),
	  m_bUseNormal(false),
	  m_bPerpToNormal(true),
	  m_bInit(false),
	  m_pModelRef(0),
	  m_nSkinIndex(0),
	  m_nSkinMeshIndex(0)
	{
		m_nType = EMITTER_TYPE_MULTIPLANE;
	}

	~A3DMultiPlaneEmitter()
	{
		ReleaseModel();
		ClearOldVertAndPlaneData();
	}

protected:
	enum MODEL_TYPE
	{
		MODEL_NONE = 0,
		MODEL_MOX,
		MODEL_SMD
	};

protected:

	A3DVECTOR3*	m_aExternVert;
	int*		m_aExternIndex;
	float*		m_fPlanePortion;
	A3DVECTOR3*	m_pPlaneNormal;
	int			m_nVertCount;
	int			m_nPlaneCount;
	float		m_fTotalArea;
	AString		m_strRefModel;
	bool		m_bUseNormal;
	bool		m_bPerpToNormal;
	bool		m_bInit;

	//skin model
	A3DGFXModelRef*	m_pModelRef;
	int			m_nSkinIndex;
	int			m_nSkinMeshIndex;

protected:

	void ClearOldVertAndPlaneData();
	void CalcPlanePortion();
	int RandDecidePlane() const;
	A3DVECTOR3 RandGenVert(int nPlane) const;
	void LoadRefModelData();

	void LoadMox();
	void ReleaseModel();
	void GetPosAndNormal(A3DVECTOR3& vPos, A3DVECTOR3& vNormal);
	void LoadSkinModel();

public:

	int GetEmissionCount(float fTimeSpan);
	void InitParticle(A3DParticle* pParticle);
	void SetVertAndPlaneData(const A3DVECTOR3* pVerts, int nVertsCount, const int* pIndices, int nIndicesCount);
	void GetVertAndPlaneData(A3DVECTOR3 **ppVerts, int * pVertsCount, int **ppIndices, int *pIndicesCount) const;
	bool IsInited() const;
	virtual void CloneFrom(const A3DParticleEmitter* p);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);

	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_REF_MOX:
		case ID_GFXOP_EMITTER_REF_SMD:
			m_strRefModel = prop;
			ClearOldVertAndPlaneData();
			m_bInit = false;
			LoadRefModelData();
			break;
		case ID_GFXOP_EMITTER_SMD_SKIN_INDEX:
			m_nSkinIndex = prop;
			ClearOldVertAndPlaneData();
			m_bInit = false;
			LoadRefModelData();
			break;
		case ID_GFXOP_EMITTER_SMD_SKINMESH_INDEX:
			m_nSkinMeshIndex = prop;
			ClearOldVertAndPlaneData();
			m_bInit = false;
			LoadRefModelData();
			break;

		case ID_GFXOP_EMITTER_USE_NORMAL:

			m_bUseNormal = prop;
			ClearOldVertAndPlaneData();
			m_bInit = false;
			LoadRefModelData();
			break;

		case ID_GFXOP_EMITTER_PERP_TO_NORMAL:

			m_bPerpToNormal = prop;
			ClearOldVertAndPlaneData();
			m_bInit = false;
			LoadRefModelData();
			break;

		default:

			if (!A3DParticleEmitter::SetProperty(nOp, prop))
				return false;
		}

		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_REF_MOX:
			if(af_CheckFileExt(m_strRefModel, ".mox"))
				return GFX_PROPERTY(m_strRefModel, GFX_VALUE_PATH_MOX);
			else
				return GFX_PROPERTY("", GFX_VALUE_PATH_MOX);

		case ID_GFXOP_EMITTER_REF_SMD:

			if(af_CheckFileExt(m_strRefModel, ".smd"))
				return GFX_PROPERTY(m_strRefModel, GFX_VALUE_PATH_MOD);
			else
				return GFX_PROPERTY("", GFX_VALUE_PATH_MOD);

		case ID_GFXOP_EMITTER_SMD_SKIN_INDEX:

			return GFX_PROPERTY(m_nSkinIndex);

		case ID_GFXOP_EMITTER_SMD_SKINMESH_INDEX:

			return GFX_PROPERTY(m_nSkinMeshIndex);
		case ID_GFXOP_EMITTER_USE_NORMAL:

			return GFX_PROPERTY(m_bUseNormal);

		case ID_GFXOP_EMITTER_PERP_TO_NORMAL:

			return GFX_PROPERTY(m_bPerpToNormal);

		default:

			return A3DParticleEmitter::GetProperty(nOp);
		}

		return GFX_PROPERTY();
	}
};

#define PORTION_MIN 1E-3f

inline void A3DMultiPlaneEmitter::CalcPlanePortion()
{
	m_fTotalArea = 0;
	int i;

	for (i = 0; i < m_nPlaneCount; i++)
	{
		int nBase = i * 3;
		int nV1 = m_aExternIndex[nBase];
		int nV2 = m_aExternIndex[nBase+1];
		int nV3 = m_aExternIndex[nBase+2];

		const A3DVECTOR3& v1 = m_aExternVert[nV1];
		const A3DVECTOR3& v2 = m_aExternVert[nV2];
		const A3DVECTOR3& v3 = m_aExternVert[nV3];

		A3DVECTOR3 e1 = v3 - v1;
		A3DVECTOR3 e2 = v2 - v1;
		A3DVECTOR3 norm = CrossProduct(e2, e1);

		m_fPlanePortion[i] = norm.Normalize() * .5f;
		m_fTotalArea += m_fPlanePortion[i];

		if (m_pPlaneNormal)
			m_pPlaneNormal[i] = norm;
	}

	if (m_fTotalArea > PORTION_MIN)
	{
		float r = 1.0f / m_fTotalArea;

		for (i = 0; i < m_nPlaneCount; i++)
			m_fPlanePortion[i] *= r;
	}
}

inline int A3DMultiPlaneEmitter::RandDecidePlane() const
{
	float fRand = _UnitRandom();

	for (int i = 0; i < m_nPlaneCount; i++)
	{
		if (fRand <= m_fPlanePortion[i])
			return i;

		fRand -= m_fPlanePortion[i];
	}

	return 0;
}

inline A3DVECTOR3 A3DMultiPlaneEmitter::RandGenVert(int nPlane) const
{
	//	   /\
	//    /  \
	//   /____\
	//   \    /
	//	  \  /
	//	   \/

	// 把三角形扩展成平行四边形，若随机点落在对称三角形内，则取
	// 原三角形中的对称点
	int nBase = nPlane * 3;
	int nV1 = m_aExternIndex[nBase];
	int nV2 = m_aExternIndex[nBase+1];
	int nV3 = m_aExternIndex[nBase+2];
	float fRand1 = _UnitRandom();
	float fRand2 = _UnitRandom();

	if (fRand1 + fRand2 <= 1.0f) // 落在原三角形内
	{
		A3DVECTOR3 r1 = (m_aExternVert[nV2] - m_aExternVert[nV1]) * fRand1;
		A3DVECTOR3 r2 = (m_aExternVert[nV3] - m_aExternVert[nV1]) * fRand2;
		return r1 + r2 + m_aExternVert[nV1];
	}
	else // 取镜像
	{
		A3DVECTOR3 r1 = (m_aExternVert[nV2] - m_aExternVert[nV1]) * (1.0f - fRand1);
		A3DVECTOR3 r2 = (m_aExternVert[nV3] - m_aExternVert[nV1]) * (1.0f - fRand2);
		return r1 + r2 + m_aExternVert[nV1];
	}
}

#endif
