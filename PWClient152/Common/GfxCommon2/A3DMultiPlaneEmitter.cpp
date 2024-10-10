#include "StdAfx.h"
#include "A3DMultiPlaneEmitter.h"
#include "A3DGFXExMan.h"
#include "A3DFrame.h"
#include "A3DMesh.h"
#include "A3DMoxMan.h"

static const char* _format_ref_model = "RefModel: %s";
static const char* _format_use_normal = "UseNormal: %d";
static const char* _format_perp_to_normal = "PerpToNormal: %d";
static const char* _format_skin_index = "SkinIndex: %d";
static const char* _format_skin_mesh_index = "SkinMeshIndex: %d";

int A3DMultiPlaneEmitter::GetEmissionCount(float fTimeSpan)
{
	return GenEmissionCount(fTimeSpan);
}

void A3DMultiPlaneEmitter::InitParticle(A3DParticle* pParticle)
{
	if (!m_bInit)
		return;

    A3DParticleEmitter::InitParticle(pParticle);

	A3DVECTOR3 vParticlePos, vParticleNormal;

	GetPosAndNormal(vParticlePos, vParticleNormal);

	pParticle->m_vPos = vParticlePos;

	if (m_bUseNormal)
	{
		if (m_bPerpToNormal)
		{
			pParticle->m_vDir = QuatFromAxes(vParticleNormal, CalcVertVec(vParticleNormal));
			GenDirection(pParticle->m_vMoveDir);
		}
		else
		{
			//			pParticle->m_vDir = m_qParentDir;
			pParticle->m_vDir = m_qParticleDir;

			if (m_fAngle)
				pParticle->m_vMoveDir = RandomDeviation(vParticleNormal, m_fAngle);
			else
				pParticle->m_vMoveDir = vParticleNormal;
		}
	}
	else
	{
		//		pParticle->m_vDir = m_qParentDir;
		pParticle->m_vDir = m_qParticleDir;
		GenDirection(pParticle->m_vMoveDir);
	}

	pParticle->m_vOldPos = pParticle->m_vPos;
	pParticle->m_vOldMoveDir = pParticle->m_vMoveDir;
	pParticle->m_fTotalTTL = GenTTL();
	pParticle->m_fTTL = 0;
}

void A3DMultiPlaneEmitter::SetVertAndPlaneData(
	const A3DVECTOR3* pVerts,
	int nVertsCount,
	const int* pIndices,
	int nIndicesCount )
{
	if (nVertsCount == 0)
		return;

	ClearOldVertAndPlaneData();

	m_nVertCount = nVertsCount;
	m_aExternVert = new A3DVECTOR3[m_nVertCount];
	memcpy(m_aExternVert, pVerts, sizeof(A3DVECTOR3) * m_nVertCount);

	m_aExternIndex = new int[nIndicesCount];
	memcpy(m_aExternIndex, pIndices, sizeof(int) * nIndicesCount);

	m_nPlaneCount = nIndicesCount / 3;
	m_fPlanePortion = new float[m_nPlaneCount];
	memset(m_fPlanePortion, 0, sizeof(float) * m_nPlaneCount);

	if (m_bUseNormal)
		m_pPlaneNormal = new A3DVECTOR3[m_nPlaneCount];

	CalcPlanePortion();
	m_bInit = true;
}


void A3DMultiPlaneEmitter::CloneFrom(const A3DParticleEmitter* p)
{
	A3DParticleEmitter::CloneFrom(p);

	A3DVECTOR3 *pVerts = NULL;
	int nVertsCount = 0;
	int *pIndices = NULL;
	int nIndicesCount = 0;
	const A3DMultiPlaneEmitter *p1 = static_cast<const A3DMultiPlaneEmitter *>(p);
	p1->GetVertAndPlaneData(&pVerts, &nVertsCount, &pIndices, &nIndicesCount);
	SetVertAndPlaneData(pVerts, nVertsCount, pIndices, nIndicesCount);
	m_strRefModel = p1->m_strRefModel;
	m_nSkinIndex = p1->m_nSkinIndex;
	m_nSkinMeshIndex = p1->m_nSkinMeshIndex;
}

void A3DMultiPlaneEmitter::ClearOldVertAndPlaneData()
{
	delete[] m_aExternVert;
	m_aExternVert = 0;
	delete[] m_aExternIndex;
	m_aExternIndex = 0;
	delete[] m_fPlanePortion;
	m_fPlanePortion = 0;
	delete[] m_pPlaneNormal;
	m_pPlaneNormal = 0;
}

void A3DMultiPlaneEmitter::GetVertAndPlaneData(A3DVECTOR3 **ppVerts, int * pVertsCount, int **ppIndices, int *pIndicesCount) const
{
	*ppVerts = m_aExternVert;
	*pVertsCount = m_nVertCount;
	*ppIndices = m_aExternIndex;
	*pIndicesCount = m_nPlaneCount * 3;
}

bool A3DMultiPlaneEmitter::IsInited() const
{
	return m_bInit;
}

bool A3DMultiPlaneEmitter::Load(AFile* pFile, DWORD dwVersion)
{
	if (!A3DParticleEmitter::Load(pFile, dwVersion))
		return false;

	char	szLine[AFILE_LINEMAXLEN];
	char	szPath[MAX_PATH];
	DWORD	dwReadLen;
	int		nRead;

	if (dwVersion >= 51)
	{
		szPath[0] = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_ref_model, szPath);
		m_strRefModel = szPath;
	}

	if (dwVersion >= 52)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_use_normal, &nRead);
		m_bUseNormal = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_perp_to_normal, &nRead);
		m_bPerpToNormal = (nRead != 0);
	}

	if (dwVersion >= 99)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_skin_index, &m_nSkinIndex);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_skin_mesh_index, &m_nSkinMeshIndex);
	}

	LoadRefModelData();
	return true;
}

bool A3DMultiPlaneEmitter::Save(AFile* pFile)
{
	if (!A3DParticleEmitter::Save(pFile))
		return false;

	char	szLine[AFILE_LINEMAXLEN];

	sprintf(szLine, _format_ref_model, m_strRefModel);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_use_normal, m_bUseNormal);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_perp_to_normal, m_bPerpToNormal);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_skin_index, m_nSkinIndex);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_skin_mesh_index, m_nSkinMeshIndex);
	pFile->WriteLine(szLine);

	return true;
}

void A3DMultiPlaneEmitter::LoadRefModelData()
{
	if (m_strRefModel.IsEmpty())
		return;

	if(af_CheckFileExt(m_strRefModel, ".mox"))
	{
		LoadMox();		
	}
	else if (af_CheckFileExt(m_strRefModel, ".smd"))
	{
		LoadSkinModel();		
	}
}

void A3DMultiPlaneEmitter::LoadMox()
{
	A3DFrame* pFrame;

	if (!AfxGetA3DDevice()->GetA3DEngine()->GetA3DMoxMan()->LoadMoxFile(m_strRefModel, &pFrame))
		return;
	
	if (pFrame->GetNumMeshes() == 1)
	{
		A3DMesh* pMesh = pFrame->GetFirstMesh();

		if (pMesh->GetFrameCount() == 1)
		{
			const A3DVERTEX* pVerts = pMesh->GetVerts(0);
			const int nVertCount = pMesh->GetVertCount();
			const WORD* pIndices = pMesh->GetIndices();
			const int nIndexCount = pMesh->GetIndexCount();

			if (nVertCount && nIndexCount)
			{
				A3DVECTOR3* _pv = new A3DVECTOR3[nVertCount];
				int* _pi = new int[nIndexCount];
				int i;

				for (i = 0; i < nVertCount; i++)
				{
					_pv[i].x = pVerts[i].x;
					_pv[i].y = pVerts[i].y;
					_pv[i].z = pVerts[i].z;
				}

				for (i = 0; i < nIndexCount; i++)
					_pi[i] = pIndices[i];

				SetVertAndPlaneData(_pv, nVertCount, _pi, nIndexCount);

				delete[] _pv;
				delete[] _pi;
			}
		}
	}

	A3DRELEASE(pFrame);	
}

void A3DMultiPlaneEmitter::LoadSkinModel()
{
	A3DGFXModelMan& ModelMan = AfxGetGFXExMan()->GetModelMan();
	A3DGFXModelRef* pModelRef = ModelMan.LoadModel("Gfx\\Models\\" + m_strRefModel, true);

	if (m_pModelRef)
		ModelMan.ReleaseModel(m_pModelRef);

	m_pModelRef = pModelRef;

	if (m_pModelRef)
	{
		//	Build up blend matrices
		A3DSkinModel* pSkinModel = m_pModelRef->GetSkinModel();
		A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();

		if (!pSkeleton)
			return;

		AArray<A3DMATRIX4> aMats;
		pSkeleton->GetOriginBoneState(aMats, false);

		//	Add bone init matrix
		int i;
		for (i=0; i < pSkeleton->GetBoneNum(); ++i)
		{
			A3DBone* pBone = pSkeleton->GetBone(i);
			aMats[i] = pBone->GetBoneInitTM() * aMats[i];
		}

		//caculate mesh
		if (pSkinModel->GetSkinNum() == 0)
			return;

		int nSkinIndex = min(pSkinModel->GetSkinNum()-1, abs(m_nSkinIndex));
		A3DSkin* pSkin = pSkinModel->GetA3DSkin(nSkinIndex);
		if (!pSkin)
			return;

		if (pSkin->GetSkinMeshNum() == 0)
			return;

		int nSkinMeshIndex = min(pSkin->GetSkinMeshNum()-1, abs(m_nSkinMeshIndex));
		A3DSkinMesh* pMesh = pSkin->GetSkinMesh(nSkinMeshIndex);
		if (!pMesh)
			return;
		
		int nVertCount = pMesh->GetVertexNum();
		int nIndexCount = pMesh->GetIndexNum();
		SKIN_VERTEX* pSkinVertex = pMesh->GetOriginVertexBuf();
		const WORD* pIndices = pMesh->GetOriginIndexBuf();

		if (nVertCount && nIndexCount)
		{
			A3DVECTOR3* _pv = new A3DVECTOR3[nVertCount];
			int* _pi = new int[nIndexCount];
			int i;

			for (i=0; i<nVertCount; ++i)
			{
				const SKIN_VERTEX& curSkinVertex = pSkinVertex[i];

				A3DVECTOR3 initPos(curSkinVertex.vPos[0], curSkinVertex.vPos[1], curSkinVertex.vPos[2]);
				A3DVECTOR3 initNormal(curSkinVertex.vNormal[0], curSkinVertex.vNormal[1], curSkinVertex.vNormal[2]);

				const BYTE* boneIdx = reinterpret_cast<const BYTE*>(&curSkinVertex.dwMatIndices);
				float fWeight = 0;
				A3DVECTOR3 vFinalPos = 0;
				for (int iBoneAffIdx = 0; iBoneAffIdx < 4; ++iBoneAffIdx)
				{
					float fBoneWeight = (iBoneAffIdx == 3 ? 1 - fWeight : curSkinVertex.aWeights[iBoneAffIdx]);
					int idx = pSkin->MapBoneIndex(boneIdx[iBoneAffIdx]);
					vFinalPos += initPos * aMats[idx];

					fWeight += fBoneWeight;
					if (fWeight >= 1.f)
						break;
				}

				_pv[i].x = vFinalPos.x;
				_pv[i].y = vFinalPos.y;
				_pv[i].z = vFinalPos.z;
			}

			for (i = 0; i < nIndexCount; i++)
				_pi[i] = pIndices[i];

			SetVertAndPlaneData(_pv, nVertCount, _pi, nIndexCount);

			delete[] _pv;
			delete[] _pi;
		}	
	}
}

void A3DMultiPlaneEmitter::ReleaseModel()
{
	if (m_pModelRef)
	{
		AfxGetGFXExMan()->GetModelMan().ReleaseModel(m_pModelRef);
		m_pModelRef = NULL;
	}
}

void A3DMultiPlaneEmitter::GetPosAndNormal( A3DVECTOR3& vPos, A3DVECTOR3& vNormal )
{
	int nPlane = RandDecidePlane();
	vPos = RandGenVert(nPlane);
	if (m_bUseNormal)
		vNormal = m_pPlaneNormal[nPlane];
}
