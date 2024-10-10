/*
 * FILE: A3DMuscleOrgan.cpp
 *
 * DESCRIPTION: Muscle as an organ, this is the control interface and data defination
 *
 * CREATED BY: Hedi, 2004/7/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPI.h"
#include "AMemory.h"
#include "A3DMuscleOrgan.h"
#include "A3DFuncs.h"
#include "A3DVertex.h"
#include "AFile.h"
#include "AFileImage.h"
#include "A3DMuscleMesh.h"

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

A3DMuscleOrgan::A3DMuscleOrgan()
: m_muscleData()
{
	m_pInitMatrices			= NULL;
	m_pAffectedRegions		= NULL;
	m_pHostMesh				= NULL;
	m_bChanged				= false;
}

A3DMuscleOrgan::~A3DMuscleOrgan()
{
}

bool A3DMuscleOrgan::Init(int nNumMatrices, const A3DMATRIX4* pInitMatrices, A3DMuscleMeshImp* pHostMesh, const A3DMuscleData& muscleData)
{
	m_pHostMesh		= pHostMesh;
	m_muscleData	= muscleData;

	// Create an empty affected region buffer
	if( !CreateAffectedRegion() )
	{
		g_A3DErrLog.Log("A3DMuscleOrgan::Init, failed to create affected region!");
		return false;
	}

	if( !CreateInitMatrices(nNumMatrices, pInitMatrices) )
	{
		g_A3DErrLog.Log("A3DMuscleOrgan::Init, failed to create init matrices!");
		return false;
	}

	// now update the affected region informations
	if( !UpdateAffectedRegion() )
	{
		g_A3DErrLog.Log("A3DMuscleOrgan::Init, failed to update all affected region!");
		return false;
	}

	return true;
}

bool A3DMuscleOrgan::CreateInitMatrices(int nNumMatrices, const A3DMATRIX4* pInitMatrices)
{
	// release old first.
	ReleaseInitMatrices();

	m_nNumInitMatrices = nNumMatrices;
	m_pInitMatrices = new A3DMATRIX4[nNumMatrices];
	if( NULL == m_pInitMatrices )
	{
		g_A3DErrLog.Log("A3DMuscleOrgan::Init, not enough memory!");
		return false;
	}

	memcpy(m_pInitMatrices, pInitMatrices, sizeof(A3DMATRIX4) * nNumMatrices);
	return true;
}

bool A3DMuscleOrgan::ReleaseInitMatrices()
{
	if( m_pInitMatrices )
	{
		delete [] m_pInitMatrices;
		m_pInitMatrices = NULL;
	}

	return true;
}

bool A3DMuscleOrgan::CreateAffectedRegion()
{
	// release old first.
	ReleaseAffectedRegion();

	m_pAffectedRegions = new MUSCLEAFR[m_muscleData.GetNumMuscle()];
	if( NULL == m_pAffectedRegions )
	{
		g_A3DErrLog.Log("A3DMuscleOrgan::CreateAffectedRegions, Not enough memory!");
		return false;
	}

	// set flag to indicate needs update
	m_bChanged = true;
	return true;
}

void A3DMuscleOrgan::ReleaseAffectedRegion()
{
	if( m_pAffectedRegions )
	{
		for(int i=0; i<m_muscleData.GetNumMuscle(); i++)
		{
			MUSCLEAFR& theAFR = m_pAffectedRegions[i];
			theAFR.idVert.clear();
			theAFR.weightVert.clear();
			theAFR.vecDelta.clear();
			theAFR.nNumVert = 0;
		}

		delete [] m_pAffectedRegions;
		m_pAffectedRegions = NULL;
	}
}

bool A3DMuscleOrgan::UpdateAffectedRegion()
{
	int nNumMuscle = m_muscleData.GetNumMuscle();

	for(int i=0; i<nNumMuscle; i++)
	{
		if( !UpdateMuscleAFR(i) )
		{
			g_A3DErrLog.Log("A3DMuscleOrgan::UpdateAffectedRegion, Failed to update muscle [%d]!", i);
			return false;
		}
	}

	return true;
}

static float GetCurvedWeight(float w, float vPitch)
{
	//return (float)pow(w, vPitch);

	if( vPitch >= 1.0f )
	{
		return (float)pow(w, vPitch + 1.0f);
	}
	else
	{
		float t=10.0f, k=(1.0f - vPitch) * 10.0f + 3.0f;
		float k2 = k * k;
		float k12 = (k-1) * (k-1);
		float t2 = t*t;
		float M = k12*k*t2 - k12*(k-2)*t;
		float N = k2 * (k - 1) * t2 - 2*(k-1)*(k-2)*t + (k-2)*(k-3);
		float C = M/N;
		float B = -2 * C + k;
		float A = C - (k - 1);

		w = 1.0f - w;
		float wk2 = pow(w, k - 2);
		return 1.0f - (A * w * w * wk2 + B * w * wk2 + C * wk2);
	}
	
	/*
	float a = vPitch;
	float b = 10;//log(a) * 2.0f;
	float eb = pow(2.73f, -b);
	
	float k = a * (1.0f - eb) / (1 + a) / (1 + a * eb);
	float y = k / (1.0f + a * pow(2.73f, -b*w)) - k / (1.0f + a);
	return y;
	*/

	/*
	float t=10.0f, k=vPitch + 2.5f;
	float k2 = k * k;
	float k12 = (k-1) * (k-1);
	float t2 = t*t;
	float M = k12*k*t2 - k12*(k-2)*t;
	float N = k2 * (k - 1) * t2 - 2*(k-1)*(k-2)*t + (k-2)*(k-3);
	float C = M/N;
	float B = -2 * C + k;
	float A = C - (k - 1);

	w = 1.0f - w;
	float wk2 = pow(w, k - 2);
	return 1.0f - (A * w * w * wk2 + B * w * wk2 + C * wk2);
	*/
}

bool A3DMuscleOrgan::UpdateMuscleAFR(int nIndex)
{
	if( !m_pHostMesh )
		return true;

	const MUSCLEDATA&	data = m_muscleData.GetMuscleData(nIndex);
	MUSCLEAFR&			theAFR = m_pAffectedRegions[nIndex];
	
	// first, clear old record
	theAFR.idVert.clear();
	theAFR.weightVert.clear();
	theAFR.vecDelta.clear();
	theAFR.nNumVert = 0;

	A3DBVERTEX3 *	pVerts = m_pHostMesh->GetOriginalVertexBuf();
	int				nNumVert = m_pHostMesh->GetVertexNum();
	int				i, j;

	// prepare for new record
	int nAffectedVerts = 0;
	A3DVECTOR3 vecCenter = data.vecCenter;
	
	// convert delta vector in object space to muscle space
	for(i=0; i<m_nNumInitMatrices; i++)
	{
		A3DMATRIX4 matInvBone = InverseTM(m_pInitMatrices[i]);
		matInvBone._41 = matInvBone._42 = matInvBone._43 = 0.0f;
		theAFR.vecDelta.push_back(data.vecMuscleDir * matInvBone * data.vLength);
	}
	
	A3DMATRIX4 tmToMuscle = InverseTM(TransformMatrix(data.vecYAxis, data.vecZAxis, A3D::g_vOrigin));
	// now calculate the weight in world space
	float		w;
	A3DVECTOR3	vSrcPos;
	for(i=0; i<nNumVert; i++)
	{
		A3DBVERTEX3 src = pVerts[i];
		A3DVECTOR3	vecPos = A3D::g_vOrigin;
		vSrcPos.Set(src.vPos[0], src.vPos[1], src.vPos[2]);

		float fTotalWeight = 0.0f;
		// first get the real deformed (by the skeleton) vertex position
		for (j=0; j < 3; j++)
		{
			if (!src.aWeights[j])
				break;

			int iMat = (src.dwMatIndices >> (j << 3)) & 0x000000ff;
			vecPos	+= (m_pInitMatrices[iMat] * vSrcPos) * src.aWeights[j];

			fTotalWeight += src.aWeights[j];
		}

		if (j >= 3)
		{
			fTotalWeight = 1.0f - fTotalWeight;
			if (fTotalWeight > 0.0f)
			{
				int iMat = (src.dwMatIndices & 0xff000000) >> 24;
				vecPos	+= (m_pInitMatrices[iMat] * vSrcPos) * fTotalWeight;
			}
		}

		A3DVECTOR3 vecDelta = vecPos - data.vecCenter;
		A3DVECTOR3 vecDeltaOrth = vecDelta * tmToMuscle / data.vecRadius;
		float vDisOrth = Magnitude(vecDeltaOrth);
		if( vDisOrth < 1.0f )
		{
			// add this vert into the affected list
			theAFR.idVert.push_back(i);

			// now determine the weight.
			w = GetCurvedWeight(1.0f - vDisOrth, data.vPitch);
			if( data.type == MUSCLE_TYPE_SINGLEDIR )
			{
			}
			else if( data.type == MUSCLE_TYPE_DOUBLEDIR )
			{
				if( DotProduct(vecDelta, data.vecMuscleDir) < 0.0f )
					w *= -1.0f;
			}
			theAFR.weightVert.push_back(w);

			nAffectedVerts ++;
		}
	}

	theAFR.nNumVert = nAffectedVerts;	
	return true;
}

void A3DMuscleOrgan::Release()
{
	ReleaseInitMatrices();
	ReleaseAffectedRegion();
}

// update organ's data
// do track interpolation if needed
bool A3DMuscleOrgan::Update(int nDeltaTime)
{
	return true;
}

bool A3DMuscleOrgan::DeformMesh()
{
	if( !m_pHostMesh || !m_bChanged )
		return true;

	// first restore the deformed mesh to be original one
	A3DBVERTEX3 * pDeformedVerts = m_pHostMesh->GetDeformedBuffer();
	A3DBVERTEX3 * pOriginalVerts = m_pHostMesh->GetOriginalVertexBuf();
	memcpy(pDeformedVerts, pOriginalVerts, sizeof(A3DBVERTEX3) * m_pHostMesh->GetVertexNum());

	int i, j, k;
	for(i=0; i<m_muscleData.GetNumMuscle(); i++)
	{
		const MUSCLEAFR&	theAFR = m_pAffectedRegions[i];
		const MUSCLEDATA&	muscledata = m_muscleData.GetMuscleData(i);

		float		vScale = muscledata.vTension;
		A3DVECTOR3	vecDelta;
		for(j=0; j<theAFR.nNumVert; j++)
		{
			int index = theAFR.idVert[j];
			float weight = theAFR.weightVert[j] * vScale;
			
			A3DBVERTEX3 src = pOriginalVerts[index];
			A3DVECTOR3	vecPos = A3D::g_vOrigin;
			
			vecDelta = A3D::g_vOrigin;
			float fTotalWeight = 0.0f;
			for (k=0; k<3; k++)
			{
				if (!src.aWeights[k])
					break;

				int iMat = (src.dwMatIndices >> (k << 3)) & 0x000000ff;
				//vecDelta += theAFR.vecDelta[iMat] * (src.aWeights[k] * weight);
				vecDelta += theAFR.vecDelta[iMat] * weight;

				fTotalWeight += src.aWeights[k];
			}

			if (k >= 3)
			{
				fTotalWeight = 1.0f - fTotalWeight;
				if (fTotalWeight > 0.0f)
				{
					int iMat = (src.dwMatIndices & 0xff000000) >> 24;
					//vecDelta += theAFR.vecDelta[iMat] * (fTotalWeight * weight);
					vecDelta += theAFR.vecDelta[iMat] * weight;
				}
			}

			pDeformedVerts[index].vPos[0] += vecDelta.x;
			pDeformedVerts[index].vPos[1] += vecDelta.y;
			pDeformedVerts[index].vPos[2] += vecDelta.z;
		}
	}

	m_bChanged = false;
	return true;
}

//	get number of muscle
int A3DMuscleOrgan::GetMuscleCount()
{
	return m_muscleData.GetNumMuscle();
}

//	get one muscle's name by its index
const char * A3DMuscleOrgan::GetMuscleName(int nIndex)
{
	const MUSCLEDATA& data = m_muscleData.GetMuscleData(nIndex);

	return data.szName;
}

//	get muscle's tension
float A3DMuscleOrgan::GetMuscleTension(int nIndex)
{
	MUSCLEDATA data = m_muscleData.GetMuscleData(nIndex);

	return data.vTension;
}

float A3DMuscleOrgan::GetMuscleTensionByName(const char * szName)
{
	int nNumMuscle = m_muscleData.GetNumMuscle();
	MUSCLEDATA data;

	for(int i=0; i<nNumMuscle; i++)
	{
		data = m_muscleData.GetMuscleData(i);
		if( 0 == _stricmp(szName, data.szName) )
			return data.vTension;
	}

	return 0.0f;
}

//	set muscle's tension
void A3DMuscleOrgan::SetMuscleTension(int nIndex, float vTension)
{
	m_muscleData.SetMuscleTension(nIndex, vTension);

	// trigger the change flag, next A3DMuscleMeshImp::Update 
	// will automatically call to deform the mesh
	m_bChanged = true;
}

void A3DMuscleOrgan::SetMuscleTensionByName(const char * szName, float vTension)
{
	int nNumMuscle = m_muscleData.GetNumMuscle();
	MUSCLEDATA data;

	for(int i=0; i<nNumMuscle; i++)
	{
		data = m_muscleData.GetMuscleData(i);
		if( 0 == _stricmp(szName, data.szName) )
		{
			SetMuscleTension(i, vTension);
			break;
		}
	}
}

//	save current muscle tensions
bool A3DMuscleOrgan::SaveTensions(const char * szFile)
{
	AFile File;

	if (!File.Open("", szFile, AFILE_CREATENEW | AFILE_BINARY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DMuscleData::Save, Cannot open file %s!", szFile);
		return false;
	}

	if (!SaveTensions(&File))
	{
		File.Close();
		return false;
	}

	File.Close();
	return true;
}

bool A3DMuscleOrgan::SaveTensions(AFile* pFile)
{
	DWORD dwWritten;
	DWORD dwSize;

	int nNumMuscle = m_muscleData.GetNumMuscle();
	dwSize = sizeof(nNumMuscle);
	if( !pFile->Write(&nNumMuscle, dwSize, &dwWritten) || dwWritten != dwSize )
	{
		g_A3DErrLog.Log("A3DMuscleOrgan::Save, Failed to save number of muscle!");
		return false;
	}

	MUSCLEDATA	data;
	float		tension;
	for(int i=0; i<nNumMuscle; i++)
	{
		data = m_muscleData.GetMuscleData(i);		
		if( !pFile->WriteString(data.szName) )
		{
			g_A3DErrLog.Log("A3DMuscleOrgan::Save, Failed to save one muscle's name!");
			return false;
		}

		tension = data.vTension;
		dwSize = sizeof(tension);
		if( !pFile->Write(&tension, dwSize, &dwWritten) || dwWritten != dwSize )
		{
			g_A3DErrLog.Log("A3DMuscleOrgan::Save, Failed to save one muscle's tension!");
			return false;
		}
	}

	return true;
}

//	load a set of muscle tensions from file
bool A3DMuscleOrgan::LoadTensions(const char * szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DMuscleData::Load, Cannot open file %s!", szFile);
		return false;
	}

	if (!LoadTensions(&File))
	{
		File.Close();
		return false;
	}

	File.Close();
	return true;
}

bool A3DMuscleOrgan::LoadTensions(AFile* pFile)
{
	DWORD dwRead;
	DWORD dwSize;

	int nNumMuscle = m_muscleData.GetNumMuscle();
	dwSize = sizeof(nNumMuscle);

	if( !pFile->Read(&nNumMuscle, dwSize, &dwRead) || dwRead != dwSize )
	{
		g_A3DErrLog.Log("A3DMuscleOrgan::Load, Failed to load number of muscle!");
		return false;
	}

	for(int i=0; i<nNumMuscle; i++)
	{
		AString strName;

		if (!pFile->ReadString(strName))
		{
			g_A3DErrLog.Log("A3DMuscleOrgan::Load, Failed to read one muscle's name!");
			return false;
		}

		float tension;
		dwSize = sizeof(tension);

		if( !pFile->Read(&tension, dwSize, &dwRead) || dwRead != dwSize )
		{
			g_A3DErrLog.Log("A3DMuscleOrgan::Load, Failed to load one muscle's tension!");
			return false;
		}

		SetMuscleTensionByName(strName, tension);
	}

	return true;
}		   

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DMuscleData
//	
///////////////////////////////////////////////////////////////////////////

A3DMuscleData::A3DMuscleData()
{
	m_nNumMuscle	= 0;
	m_pMuscles		= NULL;
}

A3DMuscleData::~A3DMuscleData()
{
}

bool A3DMuscleData::Init()
{
	return true;
}

void A3DMuscleData::Release()
{
	ReleaseMuscles();
}

const A3DMuscleData& A3DMuscleData::operator =(const A3DMuscleData& data2)
{
	// release old muscles at first;
	ReleaseMuscles();

	// then copy from data2
	m_strName = data2.m_strName;
	m_nNumMuscle = data2.m_nNumMuscle;

	// create muscle buffer
	if( !CreateMuscles() )
	{
		g_A3DErrLog.Log("A3DMuscleData::operator =, Failed to create muscles!");
		return *this;
	}

	// now copy the muscles
	if( m_nNumMuscle > 0 )
	{
		DWORD dwSize;
		dwSize = sizeof(MUSCLEDATA) * m_nNumMuscle;
		memcpy(m_pMuscles, data2.m_pMuscles, dwSize);
	}

	return *this;
}

void A3DMuscleData::SetMuscleTension(int nIndex, float vTension)
{ 
	MUSCLEDATA& data = m_pMuscles[nIndex];

	data.vTension = vTension; 
	for(unsigned int i=0; i<data.idLink.size(); i++)
	{
		m_pMuscles[data.idLink[i]].vTension = vTension;
	}
	
}
//	Load from disc or memory
bool A3DMuscleData::Load(AFile* pFile)
{
	//	Load muscle organ's name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle organ's name!");
		return false;
	}

	DWORD dwRead;
	DWORD dwSize;

	dwSize = sizeof(int);
	if( !pFile->Read(&m_nNumMuscle, dwSize, &dwRead) || dwRead != dwSize )
	{
		g_A3DErrLog.Log("A3DMuscleData::Load, Failed to load number of muscle!");
		return false;
	}

	// create muscle buffer
	if( !CreateMuscles() )
	{
		g_A3DErrLog.Log("A3DMuscleData::Load, Failed to create muscles!");
		return false;
	}

	// now load the muscles
	if( m_nNumMuscle > 0 )
	{
		int			i;
		A3DVECTOR3	vecEnd;
		for(i=0; i<m_nNumMuscle; i++)
		{
			MUSCLEDATA& muscle = m_pMuscles[i];

			dwSize = 32;
			if( !pFile->Read(muscle.szName, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's name!");
				return false;
			}

			dwSize = sizeof(A3DVECTOR3);
			if( !pFile->Read(&muscle.vecCenter, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's center!");
				return false;
			}
			if( !pFile->Read(&muscle.vecMuscleDir, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's dir!");
				return false;
			}
			if( !pFile->Read(&vecEnd, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's end!");
				return false;
			}
			muscle.vLength = Magnitude(vecEnd - muscle.vecCenter);

			if( !pFile->Read(&muscle.vecXAxis, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's x axis!");
				return false;
			}
			if( !pFile->Read(&muscle.vecYAxis, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's y axis!");
				return false;
			}
			if( !pFile->Read(&muscle.vecZAxis, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's z axis!");
				return false;
			}
			if( !pFile->Read(&muscle.vecRadius, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's radius!");
				return false;
			}
			
			dwSize = sizeof(float);
			if( !pFile->Read(&muscle.vPitch, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's pitch!");
				return false;
			}
			dwSize = sizeof(muscle.type);
			if( !pFile->Read(&muscle.type, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's type!");
				return false;
			}
			dwSize = sizeof(float);
			if( !pFile->Read(&muscle.vTension, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read muscle's tension!");
				return false;
			}
		}
		// now load linked muscle id
		for(i=0; i<m_nNumMuscle; i++)
		{
			MUSCLEDATA& muscle = m_pMuscles[i];
			dwSize = sizeof(int);
			int nNumLinked;
			if( !pFile->Read(&nNumLinked, dwSize, &dwRead) || dwRead != dwSize )
			{
				g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read linked number!");
				return false;
			}

			for(int j=0; j<nNumLinked; j++)
			{
				dwSize = sizeof(int);
				int index;
				if( !pFile->Read(&index, dwSize, &dwRead) || dwRead != dwSize )
				{
					g_A3DErrLog.Log("A3DMuscleData::Load, Failed to read linked muscle id!");
					return false;
				}

				muscle.idLink.push_back(index);
			}
		}
	}

	return true;
}

bool A3DMuscleData::Load(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DMuscleData::Load, Cannot open file %s!", szFile);
		return false;
	}

	if (!Load(&File))
	{
		File.Close();
		return false;
	}

	File.Close();
	return true;
}

//	Save to disc or memory
bool A3DMuscleData::Save(AFile* pFile)
{
	//	save muscle organ's name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DMuscleData::Save, Failed to save muscle organ's name!");
		return false;
	}

	DWORD dwWritten;
	DWORD dwSize;

	dwSize = sizeof(int);
	if( !pFile->Write(&m_nNumMuscle, dwSize, &dwWritten) || dwWritten != dwSize )
	{
		g_A3DErrLog.Log("A3DMuscleData::Save, Failed to save number of muscle!");
		return false;
	}

	// now save the muscles
	if( m_nNumMuscle > 0 )
	{
		dwSize = sizeof(MUSCLEDATA) * m_nNumMuscle;
		if( !pFile->Write(m_pMuscles, dwSize, &dwWritten) || dwWritten != dwSize )
		{
			g_A3DErrLog.Log("A3DMuscleData::Save, Failed to save muscles!");
			return false;
		}
	}

	return true;
}

bool A3DMuscleData::Save(const char* szFile)
{
	AFile File;

	if (!File.Open("", szFile, AFILE_CREATENEW | AFILE_BINARY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DMuscleData::Save, Cannot open file %s!", szFile);
		return false;
	}

	if (!Save(&File))
	{
		File.Close();
		return false;
	}

	File.Close();
	return true;
}

bool A3DMuscleData::CreateMuscles()
{
	// First release old data
	ReleaseMuscles();

	m_pMuscles = new MUSCLEDATA[m_nNumMuscle];
	if( NULL == m_pMuscles )
	{
		g_A3DErrLog.Log("A3DMuscleData::CreateMuscles, Not enough memory!");
		return false;
	}

	return true;
}

bool A3DMuscleData::ReleaseMuscles()
{
	if( m_pMuscles )
	{
		for(int i=0; i<m_nNumMuscle; i++)
		{
			m_pMuscles[i].idLink.clear();
		}
		delete [] m_pMuscles;
		m_pMuscles = NULL;
	}

	return true;
}


