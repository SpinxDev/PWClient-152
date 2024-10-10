/*
 * FILE: A3DSkinModelAux.cpp
 *
 * DESCRIPTION: A3D skin model aux class
 *
 * CREATED BY: duyuxin, 2003/11/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AFile.h"
#include "AAssist.h"
#include "A3DPI.h"
#include "A3DVSDef.h"
#include "A3DFuncs.h"
#include "A3DSkinModel.h"
#include "A3DSkinModelAux.h"
#include "A3DSkeleton.h"
#include "A3DSkinMan.h"
#include "A3DSkin.h"
#include "A3DBone.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DGraphicsFX.h"
#include "A3DGFXMan.h"
#include "AMemory.h"
#include "AAssist.h"

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
//	Implement A3DHanger
//
///////////////////////////////////////////////////////////////////////////

A3DHanger::A3DHanger(A3DSkinModel* pSkinModel) : 
m_matOffset(A3DMATRIX4::IDENTITY)
{ 
	m_iType			= TYPE_UNKNOWN;
	m_dwClassID		= A3D_CID_HANGER;
	m_pA3DSkinModel	= pSkinModel;
	m_iBindIndex	= -1;
	m_bInheritScale = false;
	m_fInheritScale	= 1.0f;
}

//	Load hanger from file
bool A3DHanger::Load(AFile* pFile)
{
	//	Load hanger name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DHanger::Load, Failed to Load hanger name");
		return false;
	}

	//	Load bind target name
	if (!pFile->ReadString(m_strBindTo))
	{
		g_A3DErrLog.Log("A3DHanger::Load, Failed to Load bind target name");
		return false;
	}

	DWORD dwRead;
	HANGERDATA Data;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DHanger::Load(), Failed to read hanger data!");
		return false;
	}

	m_iType		= Data.iType;
	m_bOnBone	= Data.bOnBone;

	return true;
}

//	Save hanger from file
bool A3DHanger::Save(AFile* pFile)
{
	//	Write hanger name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DHanger::Save, Failed to save hanger name");
		return false;
	}

	//	Write bind target name
	if (!pFile->WriteString(m_strBindTo))
	{
		g_A3DErrLog.Log("A3DHanger::Save, Failed to save bind target name");
		return false;
	}

	DWORD dwWrite;
	HANGERDATA Data;

	Data.bOnBone	= m_bOnBone;
	Data.iType		= m_iType;

	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DHanger::Save(), Failed to write hanger data!");
		return false;
	}

	return true;
}

//	Update hanger
bool A3DHanger::Update(int iDeltaTime)
{
	ASSERT(m_pA3DSkinModel);

	A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();
	if (!pSkeleton)
		return false;

	A3DMATRIX4 mat;

	if (m_bOnBone)
	{
		A3DBone* pBone = NULL;
		if (m_iBindIndex < 0)
			pBone = pSkeleton->GetBone(m_strBindTo, &m_iBindIndex);
		else
			pBone = pSkeleton->GetBone(m_iBindIndex);

		if (!pBone)
			return false;

		m_matTM = pBone->GetNoScaleAbsTM();

		if (m_bInheritScale)
			m_fInheritScale = pBone->GetAccuWholeScale();
		else
			m_fInheritScale = 1.0f;
	}
	else
	{
		A3DSkeletonHook* pHook = NULL;
		if (m_iBindIndex < 0)
			pHook = pSkeleton->GetHook(m_strBindTo, &m_iBindIndex);
		else
			pHook = pSkeleton->GetHook(m_iBindIndex);

		if (!pHook)
			return false;

		m_matTM = pHook->GetNoScaleAbsTM();

		if (pHook->GetBone() >= 0)
		{
			if (m_bInheritScale)
				m_fInheritScale = pSkeleton->GetBone(pHook->GetBone())->GetAccuWholeScale();
			else
				m_fInheritScale = 1.0f;
		}
		else
			m_fInheritScale = 1.0f;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DGFXHanger
//
///////////////////////////////////////////////////////////////////////////

A3DGFXHanger::~A3DGFXHanger()
{
	A3DGFXHanger::ReleaseHangerObject();
}

//	Load hanger from file
bool A3DGFXHanger::Load(AFile* pFile)
{
	if (!A3DHanger::Load(pFile))
		return false;

	//	Load GFX object file name
	if (!pFile->ReadString(m_strGFXFile))
	{
		g_A3DErrLog.Log("A3DGFXHanger::Load, Failed to load GFX file string");
		return false;
	}

	return true;
}

//	Save hanger from file
bool A3DGFXHanger::Save(AFile* pFile)
{
	if (!A3DHanger::Save(pFile))
		return false;

	//	Write GFX object file name
	if (!pFile->WriteString(m_strGFXFile))
	{
		g_A3DErrLog.Log("A3DGFXHanger::Save, Failed to write GFX file string");
		return false;
	}

	return true;
}

//	Load hanger object
bool A3DGFXHanger::LoadHangerObject()
{
	A3DDevice* pA3DDevice = m_pA3DSkinModel->GetA3DDevice();
	if (!pA3DDevice)
		return true;

	if (!pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->LoadGFXFromFile((char*)(const char*)m_strGFXFile, NULL, NULL, false, &m_pGFX))
	{
		g_A3DErrLog.Log("A3DGFXHanger::LoadHangerObject, Failed to Load GFX %s", m_strGFXFile);
		return false;
	}
		
	return true;
}

//	Release hanger object
void A3DGFXHanger::ReleaseHangerObject()
{
	if (!m_pGFX)
		return;

	A3DDevice* pA3DDevice = m_pA3DSkinModel->GetA3DDevice();
	if (!pA3DDevice)
		return;

	pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->ReleaseGFX(m_pGFX);
	m_pGFX = NULL;
}

//	Update routine
bool A3DGFXHanger::Update(int iDeltaTime)
{
	if (!m_pGFX)
		return true;

	if (!A3DHanger::Update(iDeltaTime))
		return false;

	m_pGFX->SetParentTM(m_matOffset * m_matTM);

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSkinModelHanger
//
///////////////////////////////////////////////////////////////////////////

A3DSkinModelHanger::A3DSkinModelHanger(A3DSkinModel* pSkinModel) : A3DHanger(pSkinModel)
{
	m_iType			= TYPE_SKINMODEL;
	m_dwClassID		= A3D_CID_SKINMODELHANGER;
	m_pModel		= NULL;
	m_iCCIndex		= -1;
}

A3DSkinModelHanger::~A3DSkinModelHanger()
{
	A3DSkinModelHanger::ReleaseHangerObject();
}

//	Load hanger from file
bool A3DSkinModelHanger::Load(AFile* pFile)
{
	if (!A3DHanger::Load(pFile))
		return false;

	//	Load model object file name
	if (!pFile->ReadString(m_strModelFile))
	{
		g_A3DErrLog.Log("A3DSkinModelHanger::Load, Failed to load model file string");
		return false;
	}

	//	Load CC name
	if (!pFile->ReadString(m_strCCName))
	{
		g_A3DErrLog.Log("A3DSkinModelHanger::Load, Failed to load CC name");
		return false;
	}

	return true;
}

//	Save hanger from file
bool A3DSkinModelHanger::Save(AFile* pFile)
{
	if (!A3DHanger::Save(pFile))
		return false;

	//	Write model object file name
	if (!pFile->WriteString(m_strModelFile))
	{
		g_A3DErrLog.Log("A3DSkinModelHanger::Save, Failed to write model file string");
		return false;
	}

	//	Write GFX object file name
	if (!pFile->WriteString(m_strCCName))
	{
		g_A3DErrLog.Log("A3DSkinModelHanger::Save, Failed to write CC name");
		return false;
	}

	return true;
}

//	Load hanger object
bool A3DSkinModelHanger::LoadHangerObject()
{
	A3DSkinModel* pModel = new A3DSkinModel;
	if (!pModel)
		return false;

	pModel->Init(m_pA3DSkinModel->GetA3DEngine());

	if (!pModel->Load(m_strModelFile))
	{
		pModel->Release();
		delete pModel;
		g_A3DErrLog.Log("A3DSkinModelHanger::LoadHangerObject, Failed to Load skin model %s", m_strModelFile);
		return false;
	}

	return SetHangerObject(pModel);
}

//	Set hanger object
bool A3DSkinModelHanger::SetHangerObject(A3DSkinModel* pModel)
{
	if (!pModel)
	{
		ASSERT(pModel);
		return false;
	}

	A3DSkeletonHook* pHook = pModel->GetSkeleton()->GetHook(m_strCCName, &m_iCCIndex);
	if (!pHook)
		m_iCCIndex = -1;

	//	Set child model's light information
	pModel->SetLightInfo(m_pA3DSkinModel->GetLightInfo());
	pModel->EnableSpecular(m_pA3DSkinModel->IsSpecularEnable());
	pModel->SetTransparent(m_pA3DSkinModel->GetTransparent());

	m_pModel = pModel;

	return true;
}

//	Unbind hanger object and this hanger. This function doesn't release hanger object
A3DSkinModel* A3DSkinModelHanger::UnbindHangerObject()
{
	A3DSkinModel* pModel = m_pModel;
	m_pModel = NULL;
	return pModel;
}

//	Release hanger object
void A3DSkinModelHanger::ReleaseHangerObject()
{
	if (!m_pModel)
		return;

	A3DDevice* pA3DDevice = m_pA3DSkinModel->GetA3DDevice();
	if (!pA3DDevice)
		return;

	m_pModel->Release();
	delete m_pModel;
	m_pModel = NULL;
}

//	Update routine
bool A3DSkinModelHanger::Update(int iDeltaTime)
{
	if (!m_pModel || m_iCCIndex < 0)
		return true;

	if (!A3DHanger::Update(iDeltaTime))
		return false;

	//	Get CC hook on child model
	A3DSkeletonHook* pCCHook = m_pModel->GetSkeleton()->GetHook(m_iCCIndex);
	
	//	Check if CC hook's parent bone is animation driven or not
	bool bAnimDriven = true;
	if (pCCHook->GetBone() >= 0)
	{
		A3DBone* pBone = m_pModel->GetSkeleton()->GetBone(pCCHook->GetBone());
		bAnimDriven = pBone->IsAnimDriven();
	}

	A3DSkeleton* pSkeleton = m_pModel->GetSkeleton();
	if (pSkeleton->GetInheritScale() != m_fInheritScale)
		pSkeleton->SetInheritScale(m_fInheritScale);

	if (bAnimDriven)
	{
		//	Update model
		m_pModel->SetAbsoluteTM(A3D::g_matIdentity);
		m_pModel->UpdateSkeletonAndAnim(iDeltaTime, m_pA3DSkinModel->GetNoAnimUpdateFlag());

		//	Get hook matrix
		A3DMATRIX4 matInvCCTM;
		a3d_InverseTM(pCCHook->GetNoScaleAbsTM(), &matInvCCTM);

		//	Re-update model's absolute TM
		if (m_bInheritScale)
		{
			A3DMATRIX4 matOff = m_matOffset;
			matOff._41 *= m_fInheritScale;
			matOff._42 *= m_fInheritScale;
			matOff._43 *= m_fInheritScale;

			m_pModel->SetAbsoluteTM(matInvCCTM * matOff * m_matTM);
		}
		else
		{
			m_pModel->SetAbsoluteTM(matInvCCTM * m_matOffset * m_matTM);
		}

		m_pModel->UpdateInternal(iDeltaTime, true);
	}
	else	//	Isn't animation driven
	{
		m_pModel->UpdateInternal(iDeltaTime, m_pA3DSkinModel->GetNoAnimUpdateFlag());
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DSkinHitBox
//
///////////////////////////////////////////////////////////////////////////

//	Load hit box from file
bool A3DSkinHitBox::Load(AFile* pFile)
{
	//	Read hit box name
	pFile->ReadString(m_strName);

	//	Read hit box data
	BOXDATA BoxData;
	DWORD dwRead;

	if (!pFile->Read(&BoxData, sizeof (BoxData), &dwRead) || dwRead != sizeof (BoxData))
	{
		g_A3DErrLog.Log("A3DSkinHitBox::Load, Failed to read box data");
		return false;
	}

	m_iBone = BoxData.iBone;
	m_obb	= BoxData.obb;

	m_obb.ExtX	= m_obb.XAxis * m_obb.Extents.x;
	m_obb.ExtY	= m_obb.YAxis * m_obb.Extents.y;
	m_obb.ExtZ	= m_obb.ZAxis * m_obb.Extents.z;

	return true;
}

//	Save hit box from file
bool A3DSkinHitBox::Save(AFile* pFile)
{
	//	Write hit box name
	pFile->WriteString(m_strName);

	BOXDATA BoxData;
	memset(&BoxData, 0, sizeof (BoxData));

	BoxData.iBone	= m_iBone;
	BoxData.obb		= m_obb;

	DWORD dwWrite;
	if (!pFile->Write(&BoxData, sizeof (BoxData), &dwWrite) || dwWrite != sizeof (BoxData))
	{
		g_A3DErrLog.Log("A3DSkinHitBox::Save, Failed to save box data");
		return false;
	}

	return true;
}

//	Update routine
bool A3DSkinHitBox::Update(int iDeltaTime)
{
	ASSERT(m_pA3DSkinModel);

	A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();
	if (!pSkeleton)
		return false;

	//	m_obb is represented in bone space, so we couldn't use blend matrix
	//	which contains bone's init-matrix. Instead we should use 
	//	bone-to-world matrix directly
	A3DBone* pBone = pSkeleton->GetBone(m_iBone);
	const A3DMATRIX4& mat = pBone->GetNoScaleAbsTM();

	m_obbWorld.Center	= m_obb.Center * pBone->GetAbsoluteTM();
	m_obbWorld.Extents	= m_obb.Extents * pBone->GetScaleMatrix();
	m_obbWorld.XAxis	= a3d_VectorMatrix3x3(m_obb.XAxis, mat);
	m_obbWorld.YAxis	= a3d_VectorMatrix3x3(m_obb.YAxis, mat);
	m_obbWorld.ZAxis	= a3d_VectorMatrix3x3(m_obb.ZAxis, mat);

	m_obbWorld.XAxis.Snap();
	m_obbWorld.YAxis.Snap();
	m_obbWorld.ZAxis.Snap();

	m_obbWorld.CompleteExtAxis();

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinPhyShape
//
///////////////////////////////////////////////////////////////////////////

//	Load physique shape from file
bool A3DSkinPhyShape::Load(AFile* pFile)
{
	//	Read physique shape name
	pFile->ReadString(m_strName);

	//	Read physique shape data
	PHYSHAPDEDATA PhyShapeData;
	DWORD dwRead;

	if (!pFile->Read(&PhyShapeData, sizeof (PhyShapeData), &dwRead) || dwRead != sizeof (PhyShapeData))
	{
		g_A3DErrLog.Log("A3DSkinPhyShape::Load, Failed to read physique shape data");
		return false;
	}

	m_iBone = PhyShapeData.iBone;
	m_cc	= PhyShapeData.cc;

	return true;
}

//	Save physique shape from file
bool A3DSkinPhyShape::Save(AFile* pFile)
{
	//	Write physique shape name
	pFile->WriteString(m_strName);

	PHYSHAPDEDATA PhyShapeData;
	memset(&PhyShapeData, 0, sizeof (PhyShapeData));

	PhyShapeData.iBone	= m_iBone;
	PhyShapeData.cc		= m_cc;

	DWORD dwWrite;
	if (!pFile->Write(&PhyShapeData, sizeof (PhyShapeData), &dwWrite) || dwWrite != sizeof (PhyShapeData))
	{
		g_A3DErrLog.Log("A3DSkinPhyShape::Save, Failed to save physique shape data");
		return false;
	}

	return true;
}

//	Update routine
bool A3DSkinPhyShape::Update(int iDeltaTime)
{
	ASSERT(m_pA3DSkinModel);

	A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();
	if (!pSkeleton)
		return false;

	//	m_cc is represented in bone space, so we couldn't use blend matrix
	//	which contains bone's init-matrix. Instead we should use 
	//	bone-to-world matrix directly
	A3DBone* pBone = pSkeleton->GetBone(m_iBone);
	const A3DMATRIX4& mat = pBone->GetNoScaleAbsTM();

	m_ccWorld.vCenter	= m_cc.vCenter * pBone->GetAbsoluteTM();
	m_ccWorld.vAxisX	= a3d_VectorMatrix3x3(m_cc.vAxisX, mat);
	m_ccWorld.vAxisY	= a3d_VectorMatrix3x3(m_cc.vAxisY, mat);
	m_ccWorld.vAxisZ	= a3d_VectorMatrix3x3(m_cc.vAxisZ, mat);

	//	In biped system, bone's length direction is along it's x axis,
	//	so we scale cylinder's length by vFactor.x
//	const A3DVECTOR3& vFactor = pBone->GetAbsoluteScale();
//	m_ccWorld.fHalfLen	= m_cc.fHalfLen * vFactor.x;
//	m_ccWorld.fRadius	= m_cc.fRadius * vFactor.z;

	return true;
}



