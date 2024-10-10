/*
 * FILE: A3DMeshSorter.cpp
 *
 * DESCRIPTION: Routines for sorting meshes
 *
 * CREATED BY: duyuxin, 2001/12/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DMeshSorter.h"
#include "A3DPI.h"
#include "A3DConfig.h"
#include "A3DIBLScene.h"
#include "A3DLight.h"
#include "A3DMesh.h"
#include "A3DDevice.h"
#include "A3DViewport.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DMeshSorter::A3DMeshSorter() : m_aMeshNodes(0, 128)
{
	m_bIncrease	= false;
	m_pDevice	= NULL;
	m_iCurNode	= 0;

	m_pIBLStaticLight = NULL;
	m_pIBLDynamicLight = NULL;
}

A3DMeshSorter::~A3DMeshSorter()
{
}

//	Initialize object
bool A3DMeshSorter::Init(A3DDevice* pDevice)
{
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
		return true;

	m_vCameraPos	= A3DVECTOR3(0.0f);
	m_pDevice		= pDevice;
	m_iCurNode		= 0;

	return true;
}

//	Release objbect
void A3DMeshSorter::Release()
{
	if (!m_pDevice)
		return;

	for (int i=0; i < m_aMeshNodes.GetSize(); i++)
		delete m_aMeshNodes[i];

	m_aMeshNodes.RemoveAll();
	m_MeshList.RemoveAll();

	m_iCurNode	= 0;
	m_pDevice	= NULL;
}

/*	Insert a mesh to list.

	Return true for success, otherwise return false

	matTrans: mesh's translate matrix
	iCurFrame: mesh's current frame
	pMesh: new mesh's address.
*/
bool A3DMeshSorter::InsertMesh(A3DMATRIX4& matTrans, A3DIBLLIGHTPARAM& iblLightParam, int iCurFrame, A3DMesh* pMesh)
{
	if (!m_pDevice)
		return true;

	if (!pMesh)
	{
		g_A3DErrLog.Log("A3DMeshSorter::InsertMesh, NULL pointer is ignored");
		return false;
	}

	MESHNODE* pMeshNode = NULL;

	if (m_iCurNode < m_aMeshNodes.GetSize())
		pMeshNode = m_aMeshNodes[m_iCurNode++];
	else
	{
		if (!(pMeshNode = new MESHNODE))
		{
			g_A3DErrLog.Log("A3DMeshSorter::InsertMesh, Not enough memory !");
			return false;
		}

		m_aMeshNodes.Add(pMeshNode);
		
		m_iCurNode++;
	}

	pMeshNode->pMesh			= pMesh;
	pMeshNode->iCurFrame		= iCurFrame;
	pMeshNode->matTrans			= matTrans;
	pMeshNode->iblLightParam	= iblLightParam;

	//	Calculate mesh's weight
	A3DOBB obb = pMesh->GetMeshAutoOBB(iCurFrame);
	A3DVECTOR3 vPos	= matTrans * obb.Center - m_vCameraPos;

	pMeshNode->fWeight = DotProduct(vPos, vPos);

	//	Insert to a proper position in list
	if (m_bIncrease)	//	Increasing order
	{
		ALISTPOSITION pos = m_MeshList.GetHeadPosition();
		while (pos)
		{
			MESHNODE* pComp = m_MeshList.GetAt(pos);

			if (pMeshNode->fWeight < pComp->fWeight)
			{
				m_MeshList.InsertBefore(pos, pMeshNode);
				return true;
			}

			m_MeshList.GetNext(pos);
		}
	}
	else	//	Decreasing order
	{
		ALISTPOSITION pos = m_MeshList.GetHeadPosition();
		while (pos)
		{
			MESHNODE* pComp = m_MeshList.GetAt(pos);

			if (pMeshNode->fWeight > pComp->fWeight)
			{
				m_MeshList.InsertBefore(pos, pMeshNode);
				return true;
			}

			m_MeshList.GetNext(pos);
		}
	}

	//	Add this element to tail
	m_MeshList.AddTail(pMeshNode);

	return true;
}

//	Remove all meshes from list. Note: this function only reset mesh list but not 
//	mesh buffer. To release mesh buffers call Release().
void A3DMeshSorter::RemoveAllMeshes()
{
	if (!m_pDevice) 
		return;

	m_MeshList.RemoveAll();

	m_iCurNode	= 0;
}

/*	Render sorted meshes.

	pCurViewport: current viewport's address
*/
bool A3DMeshSorter::Render(A3DViewport* pCurViewport)
{
	if( !m_pDevice ) return true;

	MESHNODE* pMeshNode;
	A3DMesh* pMesh;

	ALISTPOSITION pos = m_MeshList.GetHeadPosition();
	while (pos)
	{
		pMeshNode	= m_MeshList.GetNext(pos);
		pMesh		= pMeshNode->pMesh;

		m_pDevice->SetWorldMatrix(pMeshNode->matTrans);
		pMesh->UpdateToFrame(pMeshNode->iCurFrame);

		// Update static and dynamic light information if we use IBL scene
		if( A3DIBLScene::GetGobalLightGrid() )
		{
			if( m_pIBLStaticLight )
			{
				A3DLIGHTPARAM param = m_pIBLStaticLight->GetLightparam();
				param.Diffuse = pMeshNode->iblLightParam.clLightDirect;
				param.Direction = pMeshNode->iblLightParam.vecLightDir;
				m_pIBLStaticLight->SetLightParam(param);
				m_pDevice->SetAmbient(pMeshNode->iblLightParam.clLightAmbient);
				m_pDevice->SetTextureColorOP(0, A3DIBLScene::GetGobalLightGrid()->GetColorOP());
			}
			if( m_pIBLDynamicLight )
			{
				if( pMeshNode->iblLightParam.dynamicLightParam.Type != A3DLIGHT_FORCE_DWORD )
				{
					m_pIBLDynamicLight->SetLightParam(pMeshNode->iblLightParam.dynamicLightParam);
					m_pIBLDynamicLight->TurnOn();
				}
				else
					m_pIBLDynamicLight->TurnOff();
			}
		}

		if (!pMesh->Render(pCurViewport))
			return false;
	}

	if( A3DIBLScene::GetGobalLightGrid() )
	{
		if( m_pIBLStaticLight )
			m_pDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
		if( m_pIBLDynamicLight )
			m_pIBLDynamicLight->TurnOff();
	}

	return true;
}


