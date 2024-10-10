/*
 * FILE: A3DMorphMesh.cpp
 *
 * DESCRIPTION: A3D morph mesh class
 *
 * CREATED BY: duyuxin, 2003/11/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMorphMesh.h"
#include "A3DPI.h"
#include "A3DVSDef.h"
#include "A3DSkin.h"
#include "A3DSkeleton.h"
#include "A3DDevice.h"
#include "A3DBone.h"
#include "A3DSkinModel.h"
#include "A3DMorphRigidMesh.h"
#include "A3DMorphSkinMesh.h"
#include "A3DJoint.h"
#include "A3DConfig.h"
#include "A3DStream.h"
#include "A3DTrackMan.h"
#include "A3DEngine.h"
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

using namespace A3D;

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
//	Implement of A3DMorphMesh
//
///////////////////////////////////////////////////////////////////////////

A3DMorphMesh::A3DMorphMesh()
{
	m_pA3DMesh		= NULL;
	m_iMaxMCNum		= 0;
	m_iActiveMCNum	= 0;
	m_iBoundBone	= -1;
	m_aCurWeights	= NULL;
	m_aChannelSorts	= NULL;
	m_iDataSize		= 0;
}

A3DMorphMesh::~A3DMorphMesh()
{
}

//	Initialize object
bool A3DMorphMesh::Init(A3DMeshBase* pMesh)
{
	m_pA3DMesh = pMesh;
	return true;
}

//	Release object
void A3DMorphMesh::Release()
{
	int i;

	//	Release all channels
	for (i=0; i < m_aChannels.GetSize(); i++)
	{
		CHANNEL* pChannel = m_aChannels[i];

		if (pChannel->pStream)
			pChannel->pStream->Release();

		delete pChannel->pStream;
	}

	m_aChannels.RemoveAll();

	//	Release all channel sort results
	if (m_aChannelSorts)
	{
		delete [] m_aChannelSorts;
		m_aChannelSorts = NULL;
	}

	if (m_aCurWeights)
	{
		delete [] m_aCurWeights;
		m_aCurWeights = NULL;
	}

	m_iDataSize = 0;
}

//	Load skin mesh data form file
bool A3DMorphMesh::Load(AFile* pFile, A3DSkin* pSkin)
{
	MORPHDATA Data;
	DWORD dwRead;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DMorphMesh::Load, Failed to load morph skin mesh data!");
		return false;
	}

	m_iAnimStart	= Data.iAnimStart;
	m_iAnimEnd		= Data.iAnimEnd;
	m_iAnimFPS		= Data.iAnimFPS;

	int i;

	//	Load channels ...
	for (i=0; i < Data.iNumChannel; i++)
	{
		if (!LoadChannel(pFile, pSkin))
			return false;
	}

	int iNumFrame = m_iAnimEnd - m_iAnimStart + 1;
	int iNumChannel = m_aChannels.GetSize();

	//	Load channel sort result of every frame
	if (!(m_aChannelSorts = new BYTE[iNumFrame * iNumChannel]))
	{
		g_A3DErrLog.Log("A3DMorphMesh::Load, Not enough memory !");
		return false;
	}

	DWORD dwSize = iNumFrame * iNumChannel * sizeof (BYTE);

	if (!pFile->Read(m_aChannelSorts, dwSize, &dwRead) || dwRead != dwSize)
	{
		g_A3DErrLog.Log("A3DMorphMesh::Load, Failed to read channel sort data !");
		return false;
	}

	//	Build morph data
	if (!BuildMorphData())
	{
		g_A3DErrLog.Log("A3DMorphMesh::Load, Failed to build morph data !");
		return false;
	}

	//	Calculate approximate mesh data size
	m_iDataSize = (int)dwSize;
	int iNumVert = m_pA3DMesh->GetVertexNum();

	for (i=0; i < m_aChannels.GetSize(); i++)
	{
		m_iDataSize += iNumVert * sizeof (A3DMCVERTEX);
		m_iDataSize += m_aChannels[i]->pWeightTrack->GetDataSize();
	}

	return true;
}

//	Load a channel's data from file
bool A3DMorphMesh::LoadChannel(AFile* pFile, A3DSkin* pSkin)
{
	DWORD dwRead;

	CHANNEL* pChannel = new CHANNEL;
	if (!pChannel)
	{
		g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Not enough memory!");
		return false;
	}

	pChannel->pStream		= NULL;
	pChannel->pWeightTrack	= NULL;
	pChannel->aOriginVerts	= NULL;

	//	Load channel name
	if (!pFile->ReadString(pChannel->strName))
	{
		g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Failed to read channel name!");
		return false;
	}
	
	int iNumVert = m_pA3DMesh->GetVertexNum();

	//	Allocate original data buffer
	if (!(pChannel->aOriginVerts = new A3DMCVERTEX[iNumVert]))
	{
		g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Not enough memory!");
		return false;
	}

	if (!pFile->Read(pChannel->aOriginVerts, iNumVert * sizeof (A3DMCVERTEX), &dwRead) ||
		dwRead != iNumVert * sizeof (A3DMCVERTEX))
	{
		g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Failed to load channel data !");
		return false;
	}

	A3DDevice* pA3DDevice = m_pA3DMesh->GetA3DEngine()->GetA3DDevice();

	if (pA3DDevice && pA3DDevice->GetSkinModelRenderMethod() == A3DDevice::SMRD_VERTEXSHADER)
	{
		//	Create stream
		if (!(pChannel->pStream = new A3DStream))
		{
			g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Not enough memory!");
			return false;
		}

		if (!pChannel->pStream->Init(pA3DDevice, A3DVT_MCVERTEX, iNumVert, 0, 
				A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR, 0))
		{
			g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Failed to initialize stream!");
			return false;
		}

		if (!pChannel->pStream->SetVerts((BYTE*)pChannel->aOriginVerts, iNumVert))
		{
			g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Failed to set vertex data!");
			return false;
		}

		pChannel->pStream->SetVertexRef((BYTE*)pChannel->aOriginVerts);
	}

	A3DTRACKSAVEDATA TrackData;

	//	Load channel weight track
	if (!a3d_LoadTrackInfo(pFile, &TrackData))
	{
		g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Failed to load channel weight track information!");
		return false;
	}

	//	Create channel weight track buffer
	pChannel->pWeightTrack = CreateWeightTrack(pChannel->strName, TrackData.iNumKey, TrackData.iFrameRate, TrackData.iNumSegment, pSkin);
	if (!pChannel->pWeightTrack)
		return false;

	//	Load weight track data
	if (!pChannel->pWeightTrack->Load(pFile, 2))
	{
		g_A3DErrLog.Log("A3DMorphMesh::LoadChannel, Failed to load weight track data!");
		return false;
	}

	pChannel->fCurWeight = 0.0f;

	m_aChannels.Add(pChannel);

	return true;
}

//	Save skin mesh data from file
bool A3DMorphMesh::Save(AFile* pFile)
{
	MORPHDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.iAnimStart		= m_iAnimStart;
	Data.iAnimEnd		= m_iAnimEnd;
	Data.iAnimFPS		= m_iAnimFPS;
	Data.iNumChannel	= m_aChannels.GetSize();

	DWORD dwWrite;

	//	Write data
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DMorphMesh::Save, Failed to write morph skin mesh data!");
		return false;
	}

	int i, iNumVert = m_pA3DMesh->GetVertexNum();

	//	Save channels
	for (i=0; i < m_aChannels.GetSize(); i++)
	{
		CHANNEL* pChannel = m_aChannels[i];

		//	Write channel name
		if (!pFile->WriteString(pChannel->strName))
		{
			g_A3DErrLog.Log("A3DMorphMesh::Save(), Failed to write channel name!");
			return false;
		}

		//	Write position delta data
		if (!pFile->Write(pChannel->aOriginVerts, iNumVert * sizeof (A3DMCVERTEX), &dwWrite) ||
			dwWrite != iNumVert * sizeof (A3DMCVERTEX))
		{
			g_A3DErrLog.Log("A3DMorphMesh::Save, Failed to save position deltas!");
			return false;
		}

		//	Save channel weight track
		if (!pChannel->pWeightTrack->Save(pFile))
		{
			g_A3DErrLog.Log("A3DMorphMesh::Save, Failed to save channel weight track data!");
			return false;
		}
	}
	
	//	Save channel sort result of every frame
	if (m_aChannelSorts)
	{
		DWORD dwSize = (m_iAnimEnd - m_iAnimStart + 1) * Data.iNumChannel * sizeof (BYTE);

		if (!pFile->Write(m_aChannelSorts, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DMorphMesh::Save, Failed to save channel sort data !");
			return false;
		}
	}

	return true;
}

void A3DMorphMesh::SetActiveMorphChannelNum(int iNum)
{
	m_iActiveMCNum = iNum;
	a_Clamp(m_iActiveMCNum, 0, m_iMaxMCNum);
}

//	Build morph mesh data
bool A3DMorphMesh::BuildMorphData()
{
	if (m_pA3DMesh->GetClassID() == A3D_CID_MORPHSKINMESH)
	{
		m_iBoundBone = ((A3DMorphSkinMesh*)m_pA3DMesh)->GetBoundBone();
	}
	else	//	A3D_CID_MORPHRIGIDMESH
	{
		m_iBoundBone = ((A3DMorphRigidMesh*)m_pA3DMesh)->GetBoneIndex();
	}

	A3DDevice* pA3DDevice = m_pA3DMesh->GetA3DEngine()->GetA3DDevice();
	if (!pA3DDevice)
	{
		m_iMaxMCNum		= 0;
		m_iActiveMCNum	= 0;
	}
	else
	{
		//	Calcualte valid morph channel number, which is limited by vertex shader
		//	vertex input register numbers
		if (pA3DDevice->GetSkinModelRenderMethod() == A3DDevice::SMRD_VERTEXSHADER)
			m_iMaxMCNum	= a_Min(GetChannelNum(), MAXNUM_HWMORPHCHANNEL);
		else
			m_iMaxMCNum = a_Min(GetChannelNum(), MAXNUM_SWMORPHCHANNEL);

		//	For the sake of efficiency, limit default m_iActiveMCNum by 6
		m_iActiveMCNum = a_Min(m_iMaxMCNum, 6);
	}

	int iSize = ((m_aChannels.GetSize() + 3) / 4) * 4;
	if (!(m_aCurWeights = new float [iSize]))
	{
		g_A3DErrLog.Log("A3DMorphMesh::BuildMorphData, Not enough memory !");
		return false;
	}

	return true;
}

//	Create channel weight track
//	pSkin: skin object used as to identify this weight track, this value can
//		be NULL but in that 	case, weight track collision may occur easily.
A3DFloatTrack* A3DMorphMesh::CreateWeightTrack(const char* szName, int nNumKeys, int nFrameRate, 
							int iNumSegment, A3DSkin* pSkin)
{
	ASSERT(m_pA3DMesh);
	
	AString str;
	if (pSkin)
		str = pSkin->GetFileName() + AString(szName);
	else
		str = szName;
	
	DWORD dwFlag1 = pSkin ? pSkin->GetSkinID() : 0;
	DWORD dwFlag2 = a_MakeIDFromLowString(str);

	A3DFloatTrack* pTrack = g_A3DTrackMan.CreateMorphChannelWeightTrack(dwFlag1, dwFlag2, nNumKeys, nFrameRate, iNumSegment);
	if (!pTrack)
	{
		g_A3DErrLog.Log("A3DMorphMesh::CreateWeightTrack(), Create track data fail!");
		return NULL;
	}

	return pTrack;
}

/*	Calculate current channels' weights

	Return current channel sort array for success, otherwise return NULL
*/
BYTE* A3DMorphMesh::CalcCurrentChannelWeights(A3DSkinModel* pSkinModel)
{
	if (m_iBoundBone < 0)
		return NULL;

	A3DDevice* pA3DDevice = m_pA3DMesh->GetA3DEngine()->GetA3DDevice();

	if (!pA3DDevice || !m_aChannels.GetSize())
		return NULL;

	//	Now, we assume that morph mesh can only be bound to ONE bone, so we
	//	use bone's current animation time to update morph mesh's state.
	int i, iTime = pSkinModel->GetMorphActionTime(m_iBoundBone);
	
	//	Get all channel's current weight
	for (i=0; i < m_aChannels.GetSize(); i++)
	{
		CHANNEL* pChannel = m_aChannels[i];
		pChannel->fCurWeight = pChannel->pWeightTrack->GetKeyValue(iTime);
	}

	//	Get current frame
	int iFrame = (int)(m_aChannels[0]->pWeightTrack->GetCurFrame(iTime) + 0.5f);
	a_Clamp(iFrame, m_iAnimStart, m_iAnimEnd);
	iFrame -= m_iAnimStart;

	return &m_aChannelSorts[iFrame * m_aChannels.GetSize()];
}

//	Fill channel data into vertex shader
bool A3DMorphMesh::ApplyVSChannelData(A3DSkinModel* pSkinModel)
{
	if (!m_iActiveMCNum)
		return true;

	//	Calculate current channels' weights and get channel sort array
	BYTE* pSortBuf = CalcCurrentChannelWeights(pSkinModel);
	if (!pSortBuf)
		return false;

	for (int i=0; i < m_iActiveMCNum; i++)
	{
		CHANNEL* pChannel = m_aChannels[pSortBuf[i]];

		m_aCurWeights[i] = pChannel->fCurWeight;

		//	Set vertex stream
		pChannel->pStream->Appear(i+1, false);
	}

	A3DDevice* pA3DDevice = m_pA3DMesh->GetA3DEngine()->GetA3DDevice();
	pA3DDevice->SetVertexShaderConstants(SMVSC_BLENDMAT0, m_aCurWeights, GetCurVSConstNumUsedByMorph());

	return true;
}

//	Clear VS channel data
void A3DMorphMesh::ClearVSChannelData()
{
	A3DDevice* pA3DDevice = m_pA3DMesh->GetA3DEngine()->GetA3DDevice();
	if (!pA3DDevice)
		return;

	//	Set vertex streams
	for (int i=0; i < m_iActiveMCNum; i++)
		pA3DDevice->ClearStreamSource(i+1);
}


