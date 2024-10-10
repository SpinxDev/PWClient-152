#include "StdAfx.h"
#include "A3DAdhereMesh.h"
#include "A3DClothMesh.h"

//////////////////////////////////////////////////////////////////////////

A3DAdhereMesh::A3DAdhereMesh()
{
	m_pAdereParam = NULL;
	m_pSyncVert = NULL;
	m_pSyncNormal = NULL;
	m_pTriNormal = NULL;
	m_pSyncIndex = NULL;

	m_pSkin = NULL;
	m_pSkinModel = NULL;
	m_pSrcMesh = NULL;
	m_pDstMesh = NULL;

}
A3DAdhereMesh::~A3DAdhereMesh()
{
	Release();
}
void A3DAdhereMesh::Release()
{
	if (m_pAdereParam)
	{
		delete[] m_pAdereParam;
		m_pAdereParam = NULL;
	}
	if (m_pSyncVert)
	{
		delete[] m_pSyncVert;
		m_pSyncVert = NULL;
	}
	if (m_pSyncNormal)
	{
		delete[] m_pSyncNormal;
		m_pSyncNormal = NULL;
	}
	if (m_pTriNormal)
	{
		delete[] m_pTriNormal;
		m_pTriNormal = NULL;
	}
	if (m_pSyncIndex)
	{
		delete[] m_pSyncIndex;
		m_pSyncIndex = NULL;
	}
	m_pSkin = NULL;
	m_pSkinModel = NULL;
	m_pSrcMesh = NULL;
	m_pDstMesh = NULL;
}

static bool IsValidTriangle(const A3DVECTOR3& v0, const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	float d1 = (v0 - v1).Magnitude();
	float d2 = (v0 - v2).Magnitude();
	float d3 = (v1 - v2).Magnitude();

	return (fabsf(d1 + d2) > d3 && fabsf(d1 + d3) > d2 && fabsf(d2 + d3) > d1) 
		&& ( fabsf(d1 - d2) < d3 && fabsf(d1 - d3) < d2 && fabsf(d2 - d3) < d1);
}

//解方程组
//a11*var1 + a12*var2 = c1
//a21*var1 + a22*var2 = c2;
static bool SolveEquations(float a11, float a12, float a21, float a22, float c1, float c2, float& var1, float& var2)
{
	float det = a11 * a22 - a12 * a21;
	if (fabsf(det) < 1e-6f)
	{
		var1 = var2 = 0;
		return false;
	}

	var1 = (c1*a22 - c2*a12) / det;
	var2 = (c2*a11 - c1*a21) / det;
	return true;
}

//计算点p到三角形abc的最近点
static A3DVECTOR3 ClostestPtPointTriangle(const A3DVECTOR3& p, const A3DVECTOR3& a, const A3DVECTOR3& b, const A3DVECTOR3& c)
{
	//check if p in vertex region outside a
	A3DVECTOR3 ab = b - a;
	A3DVECTOR3 ac = c - a;
	A3DVECTOR3 ap = p - a;
	float d1 = DotProduct(ab, ap);
	float d2 = DotProduct(ac, ap);
	if (d1 <= 0 && d2 <= 0) return a;

	//check if p in vertex region outside b
	A3DVECTOR3 bp = p - b;
	float d3 = DotProduct(ab, bp);
	float d4 = DotProduct(ac, bp);
	if (d3 >= 0 && d4 <= d3) return b;

	//check if p in edge region of ab, if so return projection of p onto ab
	float vc = d1*d4 - d3*d2;
	if (vc <= 0 && d1 >= 0 && d3 <= 0)
	{
		float v = d1/(d1 - d3);
		return a + v*ab;//barycentric coordinates(1-v, v, 0)
	}

	//check if p in vertex region outside c
	A3DVECTOR3 cp = p - c;
	float d5 = DotProduct(ab, cp);
	float d6 = DotProduct(ac, cp);
	if (d6 >= 0 && d5 <= d6) return c;

	//check if p in edge region of ac, if so return projection of p onto ac
	float vb = d5*d2 - d1*d6;
	if (vb <= 0 && d2 >= 0 && d6 <= 0)
	{
		float w = d2 / (d2 - d6);
		return a + w * ac;
	}

	//check if p in edge region of bc, if so return projection of p onto bc
	float va = d3*d6 - d5*d4;
	if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0)
	{
		float w = (d4 - d3)/((d4 - d3) +(d5 - d6));
		return b + w * (c - b);
	}

	// p inside face region. compute Q through its barycentric coordinates
	float denom = 1.f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab*v + ac * w;
}

// adhere pSrcMesh to pDstMesh
bool A3DAdhereMesh::Init(A3DSkin* pSkin, A3DSkinModel* pModel, A3DClothMeshImp* pMeshSrc, A3DClothMeshImp* pMeshDst, AdhereParam* pAdherePara )
{
	Release();

	if (pMeshDst == NULL || pMeshSrc == NULL)
		return false;

	int i;
	abase::vector<A3DVECTOR3> vertSrc;
	abase::vector<DWORD> IdxsSrc;
	GetClothMeshOriVertsAndIdxs(pMeshSrc, vertSrc, IdxsSrc);
	int nSrcVert = (int)vertSrc.size();
	int nSrcIdxNum = IdxsSrc.size();
	
	m_pSkin = pSkin;
	m_pSkinModel = pModel;
	m_pSrcMesh = pMeshSrc;
	m_pDstMesh = pMeshDst;
	m_pAdereParam = new A3DAdhereMesh::AdhereParam[nSrcVert];
	m_pSyncVert = new A3DVECTOR3[nSrcVert];
	m_pSyncNormal = new A3DVECTOR3[nSrcVert];
	m_pTriNormal = new A3DVECTOR3[nSrcIdxNum / 3];
	m_pSyncIndex = new int[nSrcIdxNum];
	for (i = 0; i < nSrcIdxNum; i++)
		m_pSyncIndex[i] = IdxsSrc[i];

	if (pAdherePara)
	{
		for (i = 0; i < nSrcVert; i++)
		{
			m_pAdereParam[i] = pAdherePara[i];
		}

		return true;
	}

	abase::vector<A3DVECTOR3> vertDst;
	abase::vector<DWORD> IdxsDst;
	GetClothMeshOriVertsAndIdxs(pMeshDst, vertDst, IdxsDst);
	int nTriangle = IdxsDst.size() / 3;//dest mesh triangle

	A3DVECTOR3 v0, v1, v2;
	A3DVECTOR3 nv0, nv1, nv2;

	for (i = 0; i < nSrcVert; i++)
	{
		//find nearest triangle in dest mesh
		float fNearSquredDist = 10000.0f;
		int iNearTri = -1;

		for (int j = 0; j < nTriangle; j++)
		{			
			v0 = vertDst[IdxsDst[3 * j] ];
			v1 = vertDst[IdxsDst[3 * j + 1] ];
			v2 = vertDst[IdxsDst[3 * j + 2] ];

			if (!IsValidTriangle(v0, v1, v2))
				continue;

			A3DVECTOR3 vTriPos = ClostestPtPointTriangle(vertSrc[i], v0, v1, v2); // nearest position on triangle

			float fDist = (vTriPos - vertSrc[i]).SquaredMagnitude();
			if (fNearSquredDist > fDist)
			{
				fNearSquredDist = fDist;
				iNearTri = j; // keep the nearest triangle id
				nv0 = v0, nv1 = v1, nv2 = v2; //keep the nearest triangle position
			}
		}
		if (iNearTri != -1)
		{

			//compute weight1, weight2
			A3DVECTOR3 p1, p2;
			p1 = nv1 - nv0;
			p2 = nv2 - nv0;
			float weight1, weight2;
			A3DVECTOR3 vHSNormal = CrossProduct(p1, p2);
			vHSNormal.Normalize();

			float dist = DotProduct(vertSrc[i], vHSNormal) - DotProduct(nv0, vHSNormal);
			A3DVECTOR3 vPosOnPlane = vertSrc[i] - vHSNormal * dist;// position on halfspace
			A3DVECTOR3 vBakPosOnPlane = vPosOnPlane;
			vPosOnPlane -= nv0;

			//setup projection matrix
			A3DMATRIX3 mat;
			A3DVECTOR3 vp1 = p1;
			vp1.Normalize();
			mat._11 = vp1.x, mat._21 = vp1.y, mat._31 = vp1.z;
			mat._12 = vHSNormal.x, mat._22 = vHSNormal.y, mat._32 = vHSNormal.z;
			A3DVECTOR3 vTmp = CrossProduct(p1, vHSNormal);
			vTmp.Normalize();
			mat._13 = vTmp.x, mat._23 = vTmp.y, mat._33 = vTmp.z;

			p1 = p1 * mat; p2 = p2 * mat;
			vPosOnPlane = vPosOnPlane * mat;
			SolveEquations(p1.x, p2.x, p1.z, p2.z, vPosOnPlane.x, vPosOnPlane.z, weight1, weight2);

			m_pAdereParam[i].iTriangleIdx = iNearTri;
			m_pAdereParam[i].m_fDist = dist;
			m_pAdereParam[i].m_fWeight1 = weight1;
			m_pAdereParam[i].m_fWeight2 = weight2;

		}
		else
		{
			Release();
			assert(0);
			return false;
		}
	}
	return true;
}

int A3DAdhereMesh::GetClothMeshVertNum(A3DClothMeshImp* pClothMesh)
{
	if (m_pSrcMesh->GetClassID() == A3D_CID_SKINMESH)
	{
		return ((A3DSkinMesh*)(pClothMesh))->GetVertexNum();
	}
	else if (m_pSrcMesh->GetClassID() == A3D_CID_RIGIDMESH)
	{
		return ((A3DRigidMesh*)(pClothMesh))->GetVertexNum();
	}
	
	return 0; 
}
int A3DAdhereMesh::GetClothMeshIdxNum(A3DClothMeshImp* pClothMesh)
{
	if (pClothMesh->GetClassID() == A3D_CID_SKINMESH)
	{
		return ((A3DSkinMesh*)(m_pSrcMesh))->GetIndexNum();
	}
	else if (pClothMesh->GetClassID() == A3D_CID_RIGIDMESH)
	{
		return ((A3DRigidMesh*)(m_pSrcMesh))->GetIndexNum();
	}

	return 0;
}

void A3DAdhereMesh::GetClothMeshOriVertsAndIdxs(A3DClothMeshImp* pClothMesh, abase::vector<A3DVECTOR3>& vs, abase::vector<DWORD>& Idxs)
{
	vs.clear();
	Idxs.clear();
	if (pClothMesh->GetClassID() == A3D_CID_SKINMESH)
	{
		A3DSkinMesh* pSkinMesh = (A3DSkinMesh*)(pClothMesh);
		int nNum = pSkinMesh->GetVertexNum();
		vs.reserve(nNum);
		int nIdx = pSkinMesh->GetIndexNum();
		Idxs.reserve(nIdx);

		SKIN_VERTEX* pVert = pSkinMesh->GetOriginVertexBuf();
		int i;
		for (i = 0; i < nNum; i++)
		{
			vs.push_back(A3DVECTOR3(pVert[i].vPos[0], pVert[i].vPos[1], pVert[i].vPos[2])); 
		}
		
		WORD* pIdx = pSkinMesh->GetOriginIndexBuf();
		for (i = 0; i < nIdx; i++)
		{
			Idxs.push_back(pIdx[i]);
		}
	}
	else if (pClothMesh->GetClassID() == A3D_CID_RIGIDMESH)
	{
		A3DRigidMesh* pRigidMesh = (A3DRigidMesh*)(pClothMesh);
		int nNum = pRigidMesh->GetIndexNum();
		vs.reserve(nNum);
		int nIdx = pRigidMesh->GetIndexNum();
		Idxs.reserve(nIdx);

		A3DVERTEX* pVert = pRigidMesh->GetOriginVertexBuf();
		int i;
		for (i = 0; i < nNum; i++)
		{
			vs.push_back(A3DVECTOR3(pVert[i].x, pVert[i].y, pVert[i].z)); 
		}

		WORD* pIdx = pRigidMesh->GetOriginIndexBuf();
		for (i = 0; i < nIdx; i++)
		{
			Idxs.push_back(pIdx[i]);
		}
	}
}

void A3DAdhereMesh::GetBlendedVertices(A3DVECTOR3* pVert)
{
	A3DSkinMesh*  pSkinMesh = (A3DSkinMesh*)m_pSrcMesh;
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	int iBoneNum = pSkeleton->GetBoneNum();
	A3DMATRIX4* aMats = new A3DMATRIX4[iBoneNum];
	for(int i=0; i<iBoneNum; i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(i);
		aMats[i] = pBone->GetBoneInitTM() * pBone->GetAbsoluteTM();
	}
	int iVertNum = pSkinMesh->GetVertexNum();
	
	A3DVECTOR3* Normals = new A3DVECTOR3[iVertNum];

	pSkinMesh->GetBlendedVertices(m_pSkin, aMats, pVert, Normals);

	delete[] Normals;
	delete[] aMats;
}

// sync source mesh from dest mesh
void A3DAdhereMesh::SyncSrcMesh()
{
	if (m_pAdereParam == NULL || m_pDstMesh == NULL || m_pSrcMesh == NULL)
		return;
	
	int i;
	int nSrcVertNum = GetClothMeshVertNum(m_pSrcMesh); 

	A3DVERTEX* pVert = m_pDstMesh->GetVertexBuf();
	WORD* index = m_pDstMesh->GetIndexBuf();

	if (index == NULL)
	{
		assert(false);
		return;
	}
	
	A3DVECTOR3* pSrcVerts = new A3DVECTOR3[nSrcVertNum];
	GetBlendedVertices(pSrcVerts);
	
	A3DVECTOR3 p1, p2;
	A3DVECTOR3 v0, v1, v2;
	//build vertex position
	for (i = 0; i < nSrcVertNum; i++)
	{
		if (m_pAdereParam[i].m_fAdhereWeight <= 1e-5f)
		{
			m_pSyncVert[i] = pSrcVerts[i]; 
			continue;
		}
		int iTri = m_pAdereParam[i].iTriangleIdx;

		A3DVERTEX* vertex = &(pVert[index[3*iTri] ]);
		v0.Set(vertex->x, vertex->y, vertex->z);
		vertex = &(pVert[index[3*iTri+1] ]);
		v1.Set(vertex->x, vertex->y, vertex->z);
		vertex = &(pVert[index[3*iTri+2] ]);
		v2.Set(vertex->x, vertex->y, vertex->z);
		p1 = v1 - v0;
		p2 = v2 - v0;
		A3DVECTOR3 vNormal = CrossProduct(p1, p2);
		vNormal.Normalize();

		m_pSyncVert[i] = p1 * m_pAdereParam[i].m_fWeight1 + p2 * m_pAdereParam[i].m_fWeight2 + vNormal * m_pAdereParam[i].m_fDist + v0;
		if (m_pAdereParam[i].m_fAdhereWeight < 1.0f)
		{
			m_pSyncVert[i] = m_pSyncVert[i] * m_pAdereParam[i].m_fAdhereWeight + pSrcVerts[i] * (1 - m_pAdereParam[i].m_fAdhereWeight);
		}
	}

	delete[] pSrcVerts;

	//build normal
	for (i = 0; i < nSrcVertNum; i++)
	{
		m_pSyncNormal[i].Clear();
	}
	int nIndex = 0;
	index = ((A3DSkinMesh*)m_pSrcMesh)->GetOriginIndexBuf();//m_pSrcMesh->GetIndexBuf();	
	nIndex = ((A3DSkinMesh*)m_pSrcMesh)->GetIndexNum();//GetClothMeshIdxNum(m_pSrcMesh);	
	int nDstTriangle = nIndex / 3;
	
	for (i = 0; i < nDstTriangle; i++)
	{
		int idx = i * 3;
		p1 = m_pSyncVert[index[idx + 1] ] - m_pSyncVert[index[idx] ];
		p2 = m_pSyncVert[index[idx + 2] ] - m_pSyncVert[index[idx] ];
		m_pTriNormal[i] = CrossProduct(p1, p2);
		m_pTriNormal[i].Normalize();
	}
	
	for (i = 0; i < nDstTriangle; i++)
	{
		int idx = i * 3;
		m_pSyncNormal[index[idx] ] += m_pTriNormal[i];
		m_pSyncNormal[index[idx + 1] ] += m_pTriNormal[i];
		m_pSyncNormal[index[idx + 2] ] += m_pTriNormal[i];
	}
	for (i = 0; i < nSrcVertNum; i++)
		m_pSyncNormal[i].Normalize();

	//sync
	m_pSrcMesh->UpdateMeshData(nSrcVertNum, nIndex, m_pSyncVert, m_pSyncNormal, m_pSyncIndex, 0, NULL);
}