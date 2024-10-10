/*
 * FILE: A3DRigidMesh.cpp
 *
 * DESCRIPTION: A3D rigid mesh class
 *
 * CREATED BY: duyuxin, 2003/10/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DRigidMesh.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DStream.h"
#include "A3DSkin.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DViewport.h"
#include "A3DSkinModel.h"
#include "AFile.h"
#include "AMemory.h"
#include "A3DFuncs.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DRigidMesh
//
///////////////////////////////////////////////////////////////////////////

A3DRigidMesh::A3DRigidMesh()
{
	m_dwClassID		= A3D_CID_RIGIDMESH;
	m_aVerts		= NULL;
	m_aIndices		= NULL;
	m_pA3DStream	= NULL;
	m_bStaticBuf	= true;
	m_iBoneIdx		= -1;
	m_aTangentVerts = NULL;
	m_pVSStreamTan	= NULL;
}

A3DRigidMesh::~A3DRigidMesh()
{
}

//	Initialize object
bool A3DRigidMesh::Init(A3DEngine* pA3DEngine)
{
	if (!A3DMeshBase::Init(pA3DEngine))
		return false;

	return true;
}

//	Release object
void A3DRigidMesh::Release()
{
	A3DRELEASE(m_pVSStreamTan);
	A3DRELEASE(m_pA3DStream);

	//	Release original data buffers
	ReleaseOriginalBuffers();

	A3DMeshBase::Release();
}

//	Release original data buffers
void A3DRigidMesh::ReleaseOriginalBuffers()
{
	if (m_aVerts)
	{
		delete [] m_aVerts;
		m_aVerts = NULL;
	}

	if (m_aIndices)
	{
		delete [] m_aIndices;
		m_aIndices = NULL;
	}

	if(m_aTangentVerts)
	{
		delete [] m_aTangentVerts;
		m_aTangentVerts = NULL;
	}

	m_iNumVert	= 0;
	m_iNumIdx	= 0;
}

//	Load skin mesh data form file
bool A3DRigidMesh::Load(AFile* pFile, A3DSkin* pSkin, int nSkinVersion)
{
	ASSERT(m_pA3DEngine);

	//	Load mesh name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Load(), Failed to read mesh name!");
		return false;
	}

	RIGIDMESHDATA Data;
	DWORD dwRead;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Load, Failed to load skin mesh data!");
		return false;
	}

	m_iBoneIdx	= Data.iBoneIdx;
	m_iNumVert	= Data.iNumVert;
	m_iNumIdx	= Data.iNumIdx;
	m_iTexture	= Data.iTexture;
	m_iMaterial	= Data.iMaterial;

	//	Create original buffers
	if (!CreateOriginalBuffers(m_iNumVert, m_iNumIdx, nSkinVersion))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Load, Failed to create original buffers!");
		return false;
	}
	
	if(m_aTangentVerts == NULL && g_pA3DConfig->GetFlagHLSLEffectEnable())
	{
        //ReleaseOriginalBuffers();
		g_A3DErrLog.Log("A3DRigidMesh::Load, Mesh need tangent data in A3DEffect-rendering.[%s]", pSkin->GetFileName());
		//return false;
	}

	//	Load vertex data
	if (m_iNumVert > 0)
	{
		DWORD dwSize = sizeof (A3DVERTEX) * m_iNumVert;
		if (!pFile->Read(m_aVerts, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::Load, Failed to load vertex data!");
			return false;
		}
	}

	//	Load index data
	if (m_iNumIdx > 0)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;
		if (!pFile->Read(m_aIndices, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to load index data!");
			return false;
		}
	}


	//Load tangent data
	if(nSkinVersion == SKINFILE_TANGENT_VERSION)
	{
		DWORD dwSize = sizeof(A3DVECTOR4)* m_iNumVert;
		A3DVECTOR4* pTangents = new A3DVECTOR4[m_iNumVert];
		if (!pFile->Read(pTangents, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::LoadData, Failed to load Tangent data!");
			Release();
			delete[] pTangents;
			return false;
		}

		//copy tangent verts
		for( int i = 0; i < m_iNumVert; i++)
		{
			memcpy(&m_aTangentVerts[i], &m_aVerts[i], sizeof(A3DVERTEX));
			m_aTangentVerts[i].tangents = pTangents[i];
		}
		delete[] pTangents;
	}
	else if(nSkinVersion == 100)
	{
		DWORD dwSize = sizeof(A3DVECTOR4)* m_iNumVert;
		A3DVECTOR4* pTangents = new A3DVECTOR4[m_iNumVert];
		if (!pFile->Read(pTangents, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DSkinMesh::LoadData, Failed to load Tangent data!");
			ReleaseOriginalBuffers();
			delete[] pTangents;
			return false;
		}

		//copy tangent verts
		for( int i = 0; i < m_iNumVert; i++)
		{
			memcpy(&m_aTangentVerts[i], &m_aVerts[i], sizeof(A3DVERTEX));
            
			A3DVECTOR3 vTangent(pTangents[i].x, pTangents[i].y, pTangents[i].z);
			A3DVECTOR3 vNormal(m_aVerts[i].nx, m_aVerts[i].ny, m_aVerts[i].nz);
			A3DVECTOR3 vBinormal = CrossProduct(vNormal, vTangent) *pTangents[i].w;
			m_aTangentVerts[i].tangents =A3DVECTOR4(vBinormal.x, vBinormal.y, vBinormal.z, pTangents[i].w);
		}
		delete[] pTangents;

		g_A3DErrLog.Log("A3DRigidMesh::Load, Old rigid mesh version, must use new max exporter!");
	}
    else   // very old format
    {
        //copy tangent verts
        for( int i = 0; i < m_iNumVert; i++)
        {
			
			memcpy(&m_aTangentVerts[i], &m_aVerts[i], sizeof(A3DVERTEX));

            // fake tangent space to get correct (Normal * Light)
            A3DVECTOR3 vNormal(m_aVerts[i].nx, m_aVerts[i].ny, m_aVerts[i].nz);
            A3DVECTOR3 vBinormal = CrossProduct(vNormal, A3DVECTOR3(1, 0, 0));
            // in case of normal == (1, 0, 0)
            if (vBinormal.Normalize() < 0.01f)
            {
                vBinormal = CrossProduct(vNormal, A3DVECTOR3(0, 1, 0));
                vBinormal.Normalize();
            }
            m_aTangentVerts[i].tangents = A3DVECTOR4(vBinormal.x, vBinormal.y, vBinormal.z, 1.0f);
        }
    }

	if (m_iNumVert > 0 && m_iNumIdx > 0)
	{
		if (!CreateStream())
		{
			g_A3DErrLog.Log("A3DRigidMesh::Load, Failed to create stream!");
			return false;
		}
	}

	//	Calculate initial mesh aabb
	CalcInitMeshAABB();

	return true;
}

//	Save skin mesh data from file
bool A3DRigidMesh::Save(AFile* pFile, int nSkinVersion,int& nReturnValue)
{
	nReturnValue = RV_UNKNOWN_ERR;
	//	Write mesh name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Save(), Failed to write mesh name!");
		return false;
	}

	RIGIDMESHDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.iBoneIdx	= m_iBoneIdx;
	Data.iNumVert	= m_iNumVert;
	Data.iNumIdx	= m_iNumIdx;
	Data.iTexture	= m_iTexture;
	Data.iMaterial	= m_iMaterial;

	DWORD dwWrite;

	//	Write data
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to write skin mesh data!");
		return false;
	}

	//check tangent 
	// if uv mirror, deal with normals
	A3DVECTOR4* pTangents = NULL;
	if( nSkinVersion == SKINFILE_TANGENT_VERSION)
	{
		if(m_aTangentVerts)
		{	
			pTangents = new A3DVECTOR4[m_iNumVert];
			GenerateTangentBinormal(pTangents);
			if( !CheckTangentsUVMirrorError(pTangents))
			{
				nReturnValue = RV_UV_MIRROR_ERR;
				g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to Check Tangents !");			
			}
		}
	}

	//	Write vertex data
	if (m_aVerts && m_iNumVert)
	{
		DWORD dwSize = sizeof (A3DVERTEX) * m_iNumVert;
		if (!pFile->Write(m_aVerts, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to write vertex data!");
			return false;
		}
	}

	//	Write index data
	if (m_aIndices && m_iNumIdx)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;
		if (!pFile->Write(m_aIndices, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to write index data!");
			return false;
		}
	}

	//write tangent
	if( nSkinVersion == SKINFILE_TANGENT_VERSION)
	{
		if(m_aTangentVerts)
		{
			DWORD dwSize = sizeof(A3DVECTOR4)* m_iNumVert;

			if(!pFile->Write(pTangents, dwSize, &dwWrite) || dwWrite != dwSize)
			{
				g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to write Tangent data!");
				delete[] pTangents;
				return false;
			}

			if( pTangents )
			{
				delete[] pTangents;
				pTangents = NULL;
			}
		}
	}
	
	if( nReturnValue == RV_UNKNOWN_ERR)
	{
		nReturnValue = RV_OK;
	}
	return true;
}

//	Create original data buffers
bool A3DRigidMesh::CreateOriginalBuffers(int iNumVert, int iNumIdx, int nSkinVersion)
{
	//	Release old buffers
	ReleaseOriginalBuffers();

	if (iNumVert > 0)
	{
		m_aVerts = new A3DVERTEX[iNumVert];

		if (!m_aVerts)
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}

//无论何时都创建Tangent
// 		if( nSkinVersion == SKINFILE_TANGENT_VERSION || nSkinVersion == 100
//             || !g_pA3DConfig->GetFlagSkinCheckTangent())
		{

			struct A3DVERTEX_TANGENT__
			{
				struct {
					A3DVECTOR3 pos;
					A3DVECTOR3 normal;
				};
				FLOAT tu, tv;
				A3DVECTOR4 tangents;	//	tangent axis
			};

			m_aTangentVerts = (A3DVERTEX_TANGENT*) new A3DVERTEX_TANGENT__[iNumVert];//(A3DVERTEX_TANGENT *)malloc(sizeof(A3DVERTEX_TANGENT) * iNumVert);
			if (!m_aTangentVerts)
			{
				g_A3DErrLog.Log("A3DRigidMesh::CreateOriginalBuffers, Not enough memory !");
				return false;
			}
		}

	}

	if (iNumIdx > 0)
	{
		m_aIndices = new WORD[iNumIdx];

		if (!m_aIndices)
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	m_iNumVert	= iNumVert;
	m_iNumIdx	= iNumIdx;

	return true;
}

//	Create A3D stream
bool A3DRigidMesh::CreateStream()
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
		g_A3DErrLog.Log("A3DRigidMesh::CreateStream, Not Enough Memory!");
		return false;
	}

	ASSERT(!(m_iNumIdx % 3));

	DWORD dwFlags = A3DSTRM_REFERENCEPTR;
	if (m_bStaticBuf)
		dwFlags |= A3DSTRM_STATIC;
	
	if (!m_pA3DStream->Init(pA3DDevice, A3DVT_VERTEX, m_iNumVert, m_iNumIdx, dwFlags, dwFlags))
	{
		g_A3DErrLog.Log("A3DRigidMesh::CreateStream, Failed to initialize A3DStream !");
		return false;
	}

	//	Dynamic buffer will be filled every frame, so needn't initializing
	if (m_bStaticBuf)
	{
		if (!m_pA3DStream->SetVerts((BYTE*)m_aVerts, m_iNumVert))
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateStream, Failed to set vertex stream data !");
			return false;
		}

		m_pA3DStream->SetVertexRef((BYTE*)m_aVerts);
		m_pA3DStream->SetIndexRef(m_aIndices);
	}
	
	//	Fill index data
	if (!m_pA3DStream->SetIndices(m_aIndices, m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DRigidMesh::CreateStream, Failed to set index stream data !");
		return false;
	}

	// 带切线数据的部分
	if(m_aTangentVerts != NULL)
	{
		if (!(m_pVSStreamTan = new A3DStream))
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateStream(), Not Enough Memory!");
			return false;
		}

		if (!m_pVSStreamTan->Init(pA3DDevice, sizeof(A3DVERTEX_TANGENT), A3DVT_VERTEXTAN, m_iNumVert, m_iNumIdx, 
			A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR))
		{
			delete m_pVSStreamTan;
			g_A3DErrLog.Log("A3DRigidMesh::CreateStream(), Failed to initialize A3DStream !");
			return false;
		}

		m_pVSStreamTan->SetVertexRef((BYTE*)m_aTangentVerts);
		m_pVSStreamTan->SetIndexRef(m_aIndices);

		//	Fill vertex buffer
		if (!m_pVSStreamTan->SetVerts((BYTE*)m_aTangentVerts, m_iNumVert))
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateStream(), Failed to fill vertex buffer!");
			return false;
		}

		//	Fill index buffer
		if (!m_pVSStreamTan->SetIndices(m_aIndices, m_iNumIdx))
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateStream(), Failed to fill index buffer!");
			return false;
		}

	}
	return true;
}

//	Calculate initial mesh aabb
void A3DRigidMesh::CalcInitMeshAABB()
{
	if (!m_iNumVert || !m_aVerts)
	{
		memset(&m_aabbInitMesh, 0, sizeof (m_aabbInitMesh));
		return;
	}

	m_aabbInitMesh.Clear();
	A3DVECTOR3 v;

	for (int i=0; i < m_iNumVert; i++)
	{
		A3DVERTEX* pv = &m_aVerts[i];
		v.Set(pv->x, pv->y, pv->z);
		m_aabbInitMesh.AddVertex(v);
	}

	m_aabbInitMesh.CompleteCenterExts();
}

//	Render mesh
bool A3DRigidMesh::Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	if (!pA3DDevice || !m_pA3DStream || !m_iNumVert || !m_iNumIdx)
		return true;

	if (pA3DDevice->GetSkinModelRenderMethod() == A3DDevice::SMRD_VERTEXSHADER)
	{
		if(IsEnableExtMaterial())
		{
			m_pVSStreamTan->Appear(0, false);
		}
		else
		{
			m_pA3DStream->Appear(0, false);
		}
		if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
			return false;
	}
	else if (m_bStaticBuf)
	{
		//	Set transform matrix
		const A3DMATRIX4& matWorld = pSkinModel->GetBlendMatrix(m_iBoneIdx);
		pA3DDevice->SetWorldMatrix(matWorld);

		m_pA3DStream->Appear();
		if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
			return false;
	}
	else
	{
		//	TODO: Software vertex process, for example, do LOD for mesh.
		//		fill stream data and render it.
		if (!RenderSoftware(pViewport, pSkinModel))
			return false;
	}


	return true;
}

//	Software rendering
bool A3DRigidMesh::RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel)
{
	return true;
}

//	Get approximate mesh data size
int A3DRigidMesh::GetDataSize()
{
	int iSize = 0;
	int iVertSize = m_iNumVert * sizeof (A3DVERTEX);
	int iIdxSize = m_iNumIdx * sizeof (WORD);

	if (m_aVerts)
		iSize += iVertSize;

	if (m_aIndices)
		iSize += iIdxSize;

	//	There are 2 copies of vertex data: original data, software render data
	//	and hardware sub mesh render data
	iSize += iSize;

	return iSize;
}


void A3DRigidMesh::GenerateTangentBinormal(A3DVECTOR4* pTangents)
{
	abase::vector<A3DVECTOR3> tangents(m_iNumVert, A3DVECTOR3(0.0f));
	abase::vector<A3DVECTOR3> binormals(m_iNumVert, A3DVECTOR3(0.0f));

	int i;
	int nNumFace = m_iNumIdx / 3;
	for(i=0; i < nNumFace; i++)
	{
		int v0 = m_aIndices[i * 3];
		int v1 = m_aIndices[i * 3 + 1];
		int v2 = m_aIndices[i * 3 + 2];

		A3DVECTOR3 p1;
		p1.x = m_aVerts[v1].x - m_aVerts[v0].x;
		p1.y = m_aVerts[v1].y - m_aVerts[v0].y;
		p1.z = m_aVerts[v1].z - m_aVerts[v0].z;

		A3DVECTOR3 p2;
		p2.x = m_aVerts[v2].x - m_aVerts[v0].x;
		p2.y = m_aVerts[v2].y - m_aVerts[v0].y;
		p2.z = m_aVerts[v2].z - m_aVerts[v0].z;

		float du1 = m_aVerts[v1].tu - m_aVerts[v0].tu;
		float dv1 = m_aVerts[v1].tv - m_aVerts[v0].tv;
		float du2 = m_aVerts[v2].tu - m_aVerts[v0].tu;
		float dv2 = m_aVerts[v2].tv - m_aVerts[v0].tv;

		float fValue0 = (du1 * dv2 - du2 * dv1);
		float fValue1 = (du1 * dv2 - du2 * dv1);
		if( fabs(fValue0) < FLT_EPSILON || fabs(fValue1) < FLT_EPSILON)
			continue;

		A3DVECTOR3 T = (p1 * dv2 - p2 * dv1) / fValue0;
		A3DVECTOR3 B = (p2 * du1 - p1 * du2) / fValue1;

		tangents[v0] = tangents[v0] + T;
		tangents[v1] = tangents[v1] + T;
		tangents[v2] = tangents[v2] + T;

		binormals[v0] = binormals[v0] + B;
		binormals[v1] = binormals[v1] + B;
		binormals[v2] = binormals[v2] + B;
	}

	for(i=0; i < m_iNumVert; i++)
	{
		A3DVECTOR3 vNormal = A3DVECTOR3(m_aVerts[i].nx,m_aVerts[i].ny, m_aVerts[i].nz);

		//	We have set invalid normal to ZERO in AddSubMeshToModel()
		//if (vNormal.IsZero())
		//	vNormal = aOurNormals[i];

		A3DVECTOR3 vTangent = a3d_Normalize(tangents[i]);
		A3DVECTOR3 vBinormal = a3d_Normalize(binormals[i]);

		//A3DVECTOR3 vFinalTangent = vTangent - vNormal * DotProduct(vNormal, vTangent);
		//vFinalTangent.Normalize();
		//pTangents[i].Set(vFinalTangent.x, vFinalTangent.y, vFinalTangent.z, 1.0f);

		A3DVECTOR3 vFinalBinormal = vBinormal - vNormal * DotProduct(vNormal, vBinormal);
		vFinalBinormal.Normalize();
		pTangents[i].Set(vFinalBinormal.x, vFinalBinormal.y, vFinalBinormal.z, 1.0f);



		//A3DVECTOR3 vFinalBinormal = CrossProduct(vNormal, vTangent);
		//float fSign = (DotProduct(vFinalBinormal, vBinormal) < 0.0f) ? -1.0f: 1.0f;
		//	vertices[i].binormal = a3d_Normalize(vFinalBinormal * fSign);
		pTangents[i].w = 1.0f;
	}

}
bool A3DRigidMesh::CheckTangentsUVMirrorError(A3DVECTOR4* pTangents)
{
	/*
	AArray<int> aryAllErrIdx;
	for( int nVert = 0; nVert < m_iNumVert; nVert++)
	{
		int nNumFace = m_iNumIdx / 3;

		AArray<int> aryWinds;

		for(int nFace = 0; nFace < nNumFace; nFace++)
		{
			int v0 = m_aIndices[nFace * 3];
			int v1 = m_aIndices[nFace * 3 + 1];
			int v2 = m_aIndices[nFace * 3 + 2];

			if( v0 == nVert || v1 == nVert || v2 == nVert)
			{
				// 如果uv 退化三角形,则跳过
				float du1 = m_aVerts[v1].tu - m_aVerts[v0].tu;
				float dv1 = m_aVerts[v1].tv - m_aVerts[v0].tv;
				float du2 = m_aVerts[v2].tu - m_aVerts[v0].tu;
				float dv2 = m_aVerts[v2].tv - m_aVerts[v0].tv;

				float fValue0 = (du1 * dv2 - du2 * dv1);
				float fValue1 = (du1 * dv2 - du2 * dv1);
				if( fabs(fValue0) < FLT_EPSILON || fabs(fValue1) < FLT_EPSILON)
					continue;

				A3DVECTOR3 T(du1, dv1, 0);
				A3DVECTOR3 B(du2, dv2, 0);
				A3DVECTOR3 N = CrossProduct(T, B);
				int nWind = N.z > 0? 1: -1;
				aryWinds.Add(nWind);

			}
		}

		if(aryWinds.GetSize())
		{
			int nCurWid = aryWinds[0];
			for( int i = 1; i < aryWinds.GetSize(); i++)
			{
				if(nCurWid != aryWinds[i])
				{
					aryAllErrIdx.Add(nVert);
					break;
				}
			}
		}

	}

	//deal with verts normals
	if( aryAllErrIdx.GetSize())
	{
		SetTangentW(pTangents);
		for( int nIdx = 0; nIdx < aryAllErrIdx.GetSize(); nIdx++)
		{
			int nVert = aryAllErrIdx[nIdx];

			m_aVerts[nVert].nx = 0.0f;
			m_aVerts[nVert].ny = 0.0f;
			m_aVerts[nVert].nz = 0.0f;
			
			pTangents[nVert].w = 0.0f;
		}
		
		return false;
	}
*/
	SetTangentW(pTangents);
	return true;
}

void A3DRigidMesh::SetTangentW(A3DVECTOR4* pTangents)
{
	for( int nVert = 0; nVert < m_iNumVert; nVert++)
	{
		int nNumFace = m_iNumIdx / 3;


		for(int nFace = 0; nFace < nNumFace; nFace++)
		{
			int v0 = m_aIndices[nFace * 3];
			int v1 = m_aIndices[nFace * 3 + 1];
			int v2 = m_aIndices[nFace * 3 + 2];

			if( v0 == nVert || v1 == nVert || v2 == nVert)
			{
				// 如果uv 退化三角形,则跳过
				float du1 = m_aVerts[v1].tu - m_aVerts[v0].tu;
				float dv1 = m_aVerts[v1].tv - m_aVerts[v0].tv;
				float du2 = m_aVerts[v2].tu - m_aVerts[v0].tu;
				float dv2 = m_aVerts[v2].tv - m_aVerts[v0].tv;

				float fValue0 = (du1 * dv2 - du2 * dv1);
				float fValue1 = (du1 * dv2 - du2 * dv1);
				if( fabs(fValue0) < FLT_EPSILON || fabs(fValue1) < FLT_EPSILON)
					continue;

				A3DVECTOR3 T(du1, dv1, 0);
				A3DVECTOR3 B(du2, dv2, 0);
				A3DVECTOR3 N = CrossProduct(T, B);
				pTangents[nVert].w = N.z> 0 ? 1.0f:-1.0f;
				break;

			}
		}		
	}


	return;
}