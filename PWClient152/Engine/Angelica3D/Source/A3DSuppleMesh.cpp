/*
 * FILE: A3DSuppleMesh.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/3/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DSuppleMesh.h"
#include "A3DPI.h"
#include "A3DStream.h"
#include "A3DFuncs.h"
#include "A3DSkin.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DSkinModel.h"
#include "A3DViewport.h"
#include "AAssist.h"
#include "AFile.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Structure used to save and load supple mesh
struct SUPPLEMESHDATA
{
	int		iTexture;		//	Texture index
	int		iMaterial;		//	Material index
	int		iNumSVert;		//	Number of spring vertex
	int		iNumRVert;		//	Number of render vertex
	int		iNumIdx;		//	Number of index
	int		iNumSpring;		//	Nubmer of spring
	float	fGravityFactor;	//	Vertex gravity factor
	float	fAirResist;		//	Air resistance factor
	float	fVertWeight;	//	Vertex weight
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DSuppleMesh
//	
///////////////////////////////////////////////////////////////////////////

A3DSuppleMesh::A3DSuppleMesh()
{
	m_dwClassID			= A3D_CID_SUPPLEMESH;
	m_aSVerts			= NULL;
	m_aRVerts			= NULL;
	m_aIndices			= NULL;
	m_aSprings			= NULL;
	m_iNumSVert			= 0;
	m_iNumSpring		= 0;
	m_fGravityFactor	= 1.0f;
	m_fAirResist		= 0.3f;
	m_fVertWeight		= 1.0f;
}

A3DSuppleMesh::~A3DSuppleMesh()
{
}

//	Initialize object
bool A3DSuppleMesh::Init(A3DEngine* pA3DEngine)
{
	if (!A3DMeshBase::Init(pA3DEngine))
		return false;

	return true;
}

//	Release object
void A3DSuppleMesh::Release()
{
	//	Release original buffers
	ReleaseOriginalBuffers();

	A3DMeshBase::Release();
}

//	Get approximate mesh data size
int A3DSuppleMesh::GetDataSize()
{
	int iSize = 0;

	if (m_aSVerts)
		iSize += m_iNumSVert * sizeof (SVERTEX);

	if (m_aRVerts)
		iSize += m_iNumVert * sizeof (RVERTEX);

	if (m_aIndices)
		iSize += m_iNumIdx * sizeof (WORD);

	if (m_aSprings)
		iSize += m_iNumSpring * sizeof (SPRING);

	return iSize;
}

/*	Create original data buffers

	iNumSVert: number of spring vertex
	iNumRVert: number of render vertex
	iNumIdx: number of index
	iNumSpring: number of spring
*/
bool A3DSuppleMesh::CreateOriginalBuffers(int iNumSVert, int iNumRVert, int iNumIdx, int iNumSpring)
{
	//	Release old buffers
	ReleaseOriginalBuffers();

	if (iNumSVert > 0)
	{
		if (!(m_aSVerts = new SVERTEX[iNumSVert]))
		{
			g_A3DErrLog.Log("A3DSuppleMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	if (iNumRVert > 0)
	{
		if (!(m_aRVerts = new RVERTEX[iNumRVert]))
		{
			g_A3DErrLog.Log("A3DSuppleMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	if (iNumIdx > 0)
	{
		if (!(m_aIndices = new WORD[iNumIdx]))
		{
			g_A3DErrLog.Log("A3DSuppleMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	if (iNumSpring > 0)
	{
		if (!(m_aSprings = new SPRING[iNumSpring]))
		{
			g_A3DErrLog.Log("A3DSuppleMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	m_iNumSVert		= iNumSVert;
	m_iNumVert		= iNumRVert;
	m_iNumIdx		= iNumIdx;
	m_iNumSpring	= iNumSpring;

	return true;
}

//	Release original data buffers
void A3DSuppleMesh::ReleaseOriginalBuffers()
{
	if (m_aSVerts)
	{
		delete [] m_aSVerts;
		m_aSVerts = NULL;
	}

	if (m_aRVerts)
	{
		delete [] m_aRVerts;
		m_aRVerts = NULL;
	}

	if (m_aIndices)
	{
		delete [] m_aIndices;
		m_aIndices = NULL;
	}

	if (m_aSprings)
	{
		delete [] m_aSprings;
		m_aSprings = NULL;
	}
}

//	Load skin mesh data form file
bool A3DSuppleMesh::Load(AFile* pFile, A3DSkin* pSkin,int nSkinVersion)
{
	//	Load mesh name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DSuppleMesh::Load(), Failed to read mesh name!");
		return false;
	}

	SUPPLEMESHDATA Data;
	DWORD dwRead;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DSuppleMesh::Load, Failed to load skin mesh data!");
		return false;
	}

	m_iNumSVert			= Data.iNumSVert;
	m_iNumVert			= Data.iNumRVert;
	m_iNumIdx			= Data.iNumIdx;
	m_iTexture			= Data.iTexture;
	m_iMaterial			= Data.iMaterial;
	m_iNumSpring		= Data.iNumSpring;
	m_fGravityFactor	= Data.fGravityFactor;
	m_fAirResist		= Data.fAirResist;
	m_fVertWeight		= Data.fVertWeight;

	//	Create original data buffers
	if (!CreateOriginalBuffers(m_iNumSVert, m_iNumVert, m_iNumIdx, m_iNumSpring))
	{
		g_A3DErrLog.Log("A3DSuppleMesh::Load, Failed to create buffers!");
		return false;
	}

	//	Load spring vertex data
	if (m_iNumSVert > 0)
	{
		DWORD dwSize = m_iNumSVert * sizeof (SVERTEX);
		if (!pFile->Read(m_aSVerts, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DSuppleMesh::Load, Failed to load spring vertex data!");
			return false;
		}
	}

	//	Load render vertex data
	if (m_iNumVert > 0)
	{
		DWORD dwSize = m_iNumVert * sizeof (RVERTEX);
		if (!pFile->Read(m_aRVerts, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DSuppleMesh::Load, Failed to load render vertex data!");
			return false;
		}
	}

	//	Load index data
	if (m_iNumIdx > 0)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;
		if (!pFile->Read(m_aIndices, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DSuppleMesh::Load, Failed to load index data!");
			return false;
		}
	}

	//	Load spring data
	if (m_iNumSpring > 0)
	{
		DWORD dwSize = sizeof (SPRING) * m_iNumSpring;
		if (!pFile->Read(m_aSprings, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DSuppleMesh::Load, Failed to load spring data!");
			return false;
		}
	}

	//	Calculate initial mesh aabb
	CalcInitMeshAABB();

	return true;
}

//	Save skin mesh data from file
bool A3DSuppleMesh::Save(AFile* pFile, int nSkinVersion, int& nReturnValue)
{
	nReturnValue = RV_UNKNOWN_ERR;
	//	Write mesh name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DSuppleMesh::Save, Failed to write mesh name!");
		return false;
	}

	SUPPLEMESHDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.iNumSVert		= m_iNumSVert;
	Data.iNumRVert		= m_iNumVert;
	Data.iNumIdx		= m_iNumIdx;
	Data.iTexture		= m_iTexture;
	Data.iMaterial		= m_iMaterial;
	Data.iNumSpring		= m_iNumSpring;
	Data.fGravityFactor	= m_fGravityFactor;
	Data.fAirResist		= m_fAirResist;
	Data.fVertWeight	= m_fVertWeight;

	DWORD dwWrite;

	//	Write data
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DSuppleMesh::Save, Failed to write skin mesh data!");
		return false;
	}

	//	Write spring vertex data
	if (m_aSVerts && m_iNumSVert)
	{
		DWORD dwSize = m_iNumSVert * sizeof (SVERTEX);
		if (!pFile->Write(m_aSVerts, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DSuppleMesh::Save, Failed to write spring vertex data!");
			return false;
		}
	}

	//	Write render vertex data
	if (m_aRVerts && m_iNumVert)
	{
		DWORD dwSize = m_iNumVert * sizeof (RVERTEX);
		if (!pFile->Write(m_aRVerts, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DSuppleMesh::Save, Failed to write render vertex data!");
			return false;
		}
	}

	//	Write index data
	if (m_aIndices && m_iNumIdx)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;

		if (!pFile->Write(m_aIndices, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DSuppleMesh::Save, Failed to write index data!");
			return false;
		}
	}

	//	Write spring data
	if (m_aSprings && m_iNumSpring)
	{
		DWORD dwSize = sizeof (SPRING) * m_iNumSpring;

		if (!pFile->Write(m_aSprings, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DSuppleMesh::Save, Failed to write spring data!");
			return false;
		}
	}
	if( nReturnValue == RV_UNKNOWN_ERR)
	{
		nReturnValue = RV_OK;
	}
	return true;
}

//	Calculate initial mesh aabb
void A3DSuppleMesh::CalcInitMeshAABB()
{
	if (!m_iNumSVert || !m_aSVerts)
	{
		memset(&m_aabbInitMesh, 0, sizeof (m_aabbInitMesh));
		return;
	}

	m_aabbInitMesh.Clear();
	for (int i=0; i < m_iNumSVert; i++)
		m_aabbInitMesh.AddVertex(m_aSVerts[i].vPos);

	m_aabbInitMesh.CompleteCenterExts();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DSuppleMeshImp
//	
///////////////////////////////////////////////////////////////////////////

A3DSuppleMeshImp::A3DSuppleMeshImp(A3DSuppleMesh* pCoreMesh, A3DSkinModel* pSkinModel)
{
	m_dwClassID			= A3D_CID_SUPPLEMESHIMP;
	m_pSkinModel		= pSkinModel;
	m_pA3DStream		= NULL;
	m_pCoreMesh			= pCoreMesh;
	m_aPhysicalProps	= NULL;
	m_iInterateTime		= 2;
	m_bErasePosEffect	= false;
}

A3DSuppleMeshImp::~A3DSuppleMeshImp()
{
}

//	Initialize object
bool A3DSuppleMeshImp::Init(A3DEngine* pA3DEngine)
{
	ASSERT(m_pCoreMesh);

	if (!A3DMeshBase::Init(pA3DEngine))
		return false;

	m_iNumVert		= m_pCoreMesh->GetVertexNum();
	m_iNumIdx		= m_pCoreMesh->GetIndexNum();
	m_iTexture		= m_pCoreMesh->GetTextureIndex();
	m_iMaterial		= m_pCoreMesh->GetMaterialIndex();
	m_aabbInitMesh	= m_pCoreMesh->GetInitMeshAABB();

	//	Create stream
	if (m_iNumVert > 0 && m_iNumIdx > 0)
	{
		if (!CreateStream())
		{
			g_A3DErrLog.Log("A3DSuppleMeshImp::Init, Failed to create stream!");
			return false;
		}
	}

	//	Initialize physical properties
	if (!CreatePhysicalProperties())
		return false;

	return true;
}

//	Release object
void A3DSuppleMeshImp::Release()
{
	A3DRELEASE(m_pA3DStream);

	if (m_aPhysicalProps)
	{
		delete [] m_aPhysicalProps;
		m_aPhysicalProps = NULL;
	}

	A3DMeshBase::Release();
}

//	Create physical properties
bool A3DSuppleMeshImp::CreatePhysicalProperties()
{
	int iNumSVert = m_pCoreMesh->GetSpringVertexNum();

	if (!(m_aPhysicalProps = new PHYSICALPROP[iNumSVert]))
	{
		g_A3DErrLog.Log("A3DSuppleMeshImp::CreatePhysicalProperties, Not enough memory !");
		return false;
	}

	A3DSuppleMesh::SVERTEX* aSrcVerts = m_pCoreMesh->GetOriginSpringVertexBuf();

	//	Fill initial data
	for (int i=0; i < iNumSVert; i++)
	{
		PHYSICALPROP* pProp = &m_aPhysicalProps[i];
		A3DSuppleMesh::SVERTEX* pSrcVert = &aSrcVerts[i];

		pProp->vPos		= pSrcVert->vPos;
		pProp->vOldPos	= pProp->vPos;
		pProp->vForce.Clear();
	}

	return true;
}

//	Create A3D stream
bool A3DSuppleMeshImp::CreateStream()
{
	if (!m_pA3DEngine->GetA3DDevice())
		return true;

	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Release old stream
	if (m_pA3DStream)
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
	}

	if (!(m_pA3DStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DSuppleMeshImp::CreateStream, Not Enough Memory!");
		return false;
	}

	ASSERT(!(m_iNumIdx % 3));

	if (!m_pA3DStream->Init(pA3DDevice, A3DVT_VERTEX, m_iNumVert, m_iNumIdx, 
			A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR))
	{
		g_A3DErrLog.Log("A3DSuppleMeshImp::CreateStream, Failed to initialize A3DStream !");
		return false;
	}

	//	Fill index data
	if (!m_pA3DStream->SetIndices(m_pCoreMesh->GetOriginIndexBuf(), m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DSuppleMeshImp::CreateStream, Failed to set index stream data !");
		return false;
	}

	m_pA3DStream->SetIndexRef(m_pCoreMesh->GetOriginIndexBuf());

	return true;
}

//	Add external force
void A3DSuppleMeshImp::AddForce(const A3DVECTOR3& vForce, float fDisturb)
{
	FORCE Force;
	Force.fForce	= Normalize(vForce, Force.vForceDir);
	Force.fDisturb	= (float)fabs(fDisturb);
	m_aForces.Add(Force);
}

//	Update mesh
bool A3DSuppleMeshImp::Update(int iDeltaTime, A3DSkinModel* pSkinModel)
{
	a_ClampRoof(iDeltaTime, 10);
	CalculateForces(iDeltaTime, pSkinModel);
	CalculateVertices(iDeltaTime, pSkinModel);
	m_bErasePosEffect = false;
	return true;
}

//	Calculate forces which effect vertices
void A3DSuppleMeshImp::CalculateForces(int iDeltaTime, A3DSkinModel* pSkinModel)
{
	int iNumSVert = m_pCoreMesh->GetSpringVertexNum();
	float fGravityFactor = m_pCoreMesh->GetGravityFactor();
	float fVertWeight = m_pCoreMesh->GetVertWeight();

	const A3DMATRIX4* aBlendMats = pSkinModel->GetBlendMatrices();
	A3DVECTOR3 vSrcPos, vSrcNormal, vPos, vNormal;

	//	Get core mesh original vertices
	const A3DSuppleMesh::SVERTEX* aOriginVerts = m_pCoreMesh->GetOriginSpringVertexBuf();

	for (int i=0; i < iNumSVert; i++)
	{
		PHYSICALPROP* pPhysicalProp = &m_aPhysicalProps[i];
		const A3DSuppleMesh::SVERTEX* pSrc = &aOriginVerts[i];

		if (pSrc->fBoneFactor != 1.0f)
	//	if (pSrc->fBoneFactor == 0.0f)
		{
			//	This vertex will be effected by bone
			vSrcPos	= pSrc->vPos;
			vPos.Clear();

			float fTotalWeight = 0;
			int j = 0;
			for (j=0; j < 3; j++)
			{
				if (!pSrc->aWeights[j])
					break;

				int iMat = (pSrc->dwMatIndices >> (j << 3)) & 0x000000ff;
				vPos += (aBlendMats[iMat] * vSrcPos) * pSrc->aWeights[j];

				fTotalWeight += pSrc->aWeights[j];
			}

			if (j >= 3)
			{
				fTotalWeight = 1.0f - fTotalWeight;
				if (fTotalWeight > 0.0f)
				{
					int iMat = (pSrc->dwMatIndices & 0xff000000) >> 24;
					vPos += (aBlendMats[iMat] * vSrcPos) * fTotalWeight;
				}
			}

			vNormal.Normalize();

			pPhysicalProp->vTransPos	= vPos;
			pPhysicalProp->vTransNormal	= vNormal;
		}

		pPhysicalProp->vForce.Clear();

		//	Add external forces, only take vertices with a weight > 0 into account
		if (pSrc->fBoneFactor > 0.0f)
		{
			for (int j=0; j < m_aForces.GetSize(); j++)
			{
				const FORCE& Force = m_aForces[j];
				float f = Force.fForce + a_Random(-Force.fDisturb, Force.fDisturb);
				pPhysicalProp->vForce += Force.vForceDir * f;
			}

			//	Consider gravity
			A3DVECTOR3 vGravity(0.0f, fVertWeight * fGravityFactor * -98.1f, 0.0f);
			pPhysicalProp->vForce += vGravity;
		}
	}

	//	Remove all external force
	m_aForces.RemoveAll(false);
}

//	Calculate vertices positions
void A3DSuppleMeshImp::CalculateVertices(int iDeltaTime, A3DSkinModel* pSkinModel)
{
/*	int i, iNumSVert = m_pCoreMesh->GetSpringVertexNum();
	float fDeltaTime = iDeltaTime * 0.001f;
	float fInvVertWeight = 1.0f / m_pCoreMesh->GetVertWeight();

	//	Get core mesh original spring vertices
	const A3DSuppleMesh::SVERTEX* aOriginVerts = m_pCoreMesh->GetOriginSpringVertexBuf();

	for (i=0; i < iNumSVert; i++)
	{
		PHYSICALPROP* pPhysicalProp = &m_aPhysicalProps[i];
		const A3DSuppleMesh::SVERTEX* pSrc = &aOriginVerts[i];

		//	Store current position for later use
		A3DVECTOR3 vPos = pPhysicalProp->vPos;

		//	Only take vertices with a weight > 0 into account
		if (pSrc->fBoneFactor > 0.0f)
		{
			//	Calculate air resistance = v0 * factor
			A3DVECTOR3 v0 = (vPos - pPhysicalProp->vOldPos) * 0.99f;
			A3DVECTOR3 vF = -v0 * m_pCoreMesh->GetAirResistance();
		//	A3DVECTOR3 vF = -v0 * m_pCoreMesh->GetAirResistance() * 0.0f;

			//	ds = v * t;	v = v0 + a * t;
			//	ds = v0 * t + a * t * t;
			A3DVECTOR3 vds = v0 + (pPhysicalProp->vForce + vF) * fInvVertWeight * fDeltaTime * fDeltaTime;

			pPhysicalProp->vPos += vds;

			if (pSrc->fBoneFactor != 1.0f)
			{
				//	Blend with transformed position
		//		pPhysicalProp->vPos = pPhysicalProp->vPos * pSrc->fBoneFactor + 
		//			pPhysicalProp->vTransPos * (1.0f - pSrc->fBoneFactor);
				float fBoneFactor = (1.0f - pSrc->fBoneFactor) * 0.001f;
				pPhysicalProp->vPos = pPhysicalProp->vPos * (1.0f - fBoneFactor) + 
					pPhysicalProp->vTransPos * fBoneFactor;
			}
		}
		else
			pPhysicalProp->vPos = pPhysicalProp->vTransPos;

		//	Make the current position the old one
		pPhysicalProp->vOldPos = vPos;
	}

	//	Get the spring vector of the core submesh
	const A3DSuppleMesh::SPRING* aSprings = m_pCoreMesh->GetSpringBuf();
	int iNumSpring = m_pCoreMesh->GetSpringNum();

	if (m_bErasePosEffect)
	{
		int iSize = (iNumSVert + 7) >> 3;
		if (m_aFixFlags.GetSize() != iSize)
			m_aFixFlags.SetSize(iSize, 10);

		memset(m_aFixFlags.GetData(), 0, iSize);

		//	Iterate a few times to relax the constraints
		for (i=0; i < m_iInterateTime; i++)
		{
			//	Loop through all the springs
			for (int j=0; j < iNumSpring; j++)
			{
				const A3DSuppleMesh::SPRING& Spring = aSprings[j];
				PHYSICALPROP* pp1 = &m_aPhysicalProps[Spring.v1];
				PHYSICALPROP* pp2 = &m_aPhysicalProps[Spring.v2];

				//	Compute the difference between the two spring vertices
				A3DVECTOR3 vDist = pp2->vPos - pp1->vPos;

				if (!aOriginVerts[Spring.v1].fBoneFactor)
					SetFixedFlag(Spring.v1);

				if (!aOriginVerts[Spring.v2].fBoneFactor)
					SetFixedFlag(Spring.v2);

				if (GetFixedFlag(Spring.v1))
				{
					vDist.Normalize();
					pp2->vPos	 = pp1->vPos + vDist * Spring.fIdleLen;
					pp2->vOldPos = pp2->vPos;
					SetFixedFlag(Spring.v2);
				}
				else if (GetFixedFlag(Spring.v2))
				{
					vDist.Normalize();
					pp1->vPos	 = pp2->vPos - vDist * Spring.fIdleLen; 
					pp1->vOldPos = pp1->vPos;
					SetFixedFlag(Spring.v1);
				}
			}
		}
	}
	else
	{
		//	Iterate a few times to relax the constraints
		for (i=0; i < m_iInterateTime; i++)
		{
			//	Loop through all the springs
			for (int j=0; j < iNumSpring; j++)
			{
				const A3DSuppleMesh::SPRING& Spring = aSprings[j];
				PHYSICALPROP* pp1 = &m_aPhysicalProps[Spring.v1];
				PHYSICALPROP* pp2 = &m_aPhysicalProps[Spring.v2];

				//	Do collision check for spring
				if (aOriginVerts[Spring.v1].fBoneFactor > 0.0f &&
					aOriginVerts[Spring.v2].fBoneFactor > 0.0f)
				{
					A3DVECTOR3 vOff1, vOff2;
					bool r1 = m_pSkinModel->AdjustSuppleMeshVertPos(pp1->vPos, vOff1);
					bool r2 = m_pSkinModel->AdjustSuppleMeshVertPos(pp2->vPos, vOff2);

					if (r1 && r2)
					{
						pp1->vPos += vOff1;
						pp2->vPos += vOff2;
					}
					else if (r1)
					{
						pp1->vPos += vOff1;
						pp2->vPos += vOff1;
					}
				}

				//	Compute the difference between the two spring vertices
				A3DVECTOR3 vDist = pp2->vPos - pp1->vPos;

				//	Get the current length of the spring
				float fLength = vDist.Magnitude();
				if (fLength == 0.0f)
					continue;
			    
				float fTargetLen;
				float fFlexibleLen = Spring.fIdleLen * Spring.fCoefficient;

				if (fLength < Spring.fIdleLen - fFlexibleLen)
					fTargetLen = Spring.fIdleLen - fFlexibleLen;
				else if (fLength > Spring.fIdleLen)
					fTargetLen = Spring.fIdleLen;
				else
					continue;

				float aFactors[2];
			//	aFactors[0] = (fLength - Spring.fIdleLen) / fLength;
				aFactors[0] = (fLength - fTargetLen) / fLength;
				aFactors[1] = aFactors[0];

				if (aOriginVerts[Spring.v1].fBoneFactor > 0.0f)
				{
					aFactors[0] *= 0.5f;
					aFactors[1] *= 0.5f;
				}
				else
				{
					aFactors[0] = 0.0f;
				}

				if (aOriginVerts[Spring.v2].fBoneFactor <= 0.0f)
				{
					aFactors[0] *= 2.0f;
					aFactors[1]  = 0.0f;
				}

				pp1->vPos += vDist * aFactors[0];
				pp2->vPos -= vDist * aFactors[1];
			}
		}
	}
*/
}

//	Render mesh
bool A3DSuppleMeshImp::Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	if (!pA3DDevice || !m_pA3DStream)
		return true;

	pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	//	Get core mesh original render vertices
	const A3DSuppleMesh::RVERTEX* aOriginVerts = m_pCoreMesh->GetOriginRenderVertexBuf();

	A3DVERTEX* aVerts;

	//	Lock vertex buffer
	if (!m_pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &aVerts, 0))
		return false;

	//	Fill vertex stream
	for (int i=0; i < m_iNumVert; i++)
	{
		const A3DSuppleMesh::RVERTEX* pSrc = &aOriginVerts[i];
		ASSERT(pSrc->iSVertex >= 0 && pSrc->iSVertex < m_pCoreMesh->GetSpringVertexNum());

		PHYSICALPROP* pPhysicalProp = &m_aPhysicalProps[pSrc->iSVertex];
		A3DVERTEX* pVert = &aVerts[i];

		pVert->x	= pPhysicalProp->vPos.x;
		pVert->y	= pPhysicalProp->vPos.y;
		pVert->z	= pPhysicalProp->vPos.z;
		pVert->nx	= pSrc->vNormal.x;
		pVert->ny	= pSrc->vNormal.y;
		pVert->nz	= pSrc->vNormal.z;
		pVert->tu	= pSrc->tu;
		pVert->tv	= pSrc->tv;
	}

	//	Unlock vertex buffer
	if (!m_pA3DStream->UnlockVertexBuffer())
		return false;

	m_pA3DStream->Appear();

	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
		return false;

	return true;
}

//	Get approximate mesh data size
int A3DSuppleMeshImp::GetDataSize()
{
	return 0;
}

