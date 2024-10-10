#include "StdAfx.h"
#ifdef A3D_PHYSX
#include "A3DPhysEmitter.h"
#include "A3DGFXExMan.h"
#include "A3DGFXPhysDataMan.h"
#include "A3DPhysFluidSyncData.h"
#include <APhysX.h>
#include <APhysXFluidObjectDesc.h>

// local const vals
static const A3DCOLOR _cMask = A3DCOLORRGBA(255, 255, 255, 0);
static const A3DCOLOR dwSpecular = A3DCOLORRGBA(0, 0, 0, 255);

// extern vals
extern int _gfx_par_render_count;

//////////////////////////////////////////////////////////////////////////
// Local Functions
//////////////////////////////////////////////////////////////////////////

A3DCOLOR genRandColor(A3DCOLOR dwColorMin, A3DCOLOR dwColorMax)
{
	if (dwColorMin == dwColorMax) return dwColorMin;
	float fR = _UnitRandom();
	float fG = _UnitRandom();
	float fB = _UnitRandom();
	float fA = _UnitRandom();
	int nR = int(A3DCOLOR_GETRED  (dwColorMin) * (1.0f - fR) + A3DCOLOR_GETRED(dwColorMax)	* fR);
	int nG = int(A3DCOLOR_GETGREEN(dwColorMin) * (1.0f - fG) + A3DCOLOR_GETGREEN(dwColorMax)* fG);
	int nB = int(A3DCOLOR_GETBLUE (dwColorMin) * (1.0f - fB) + A3DCOLOR_GETBLUE(dwColorMax)	* fB);
	int nA = int(A3DCOLOR_GETALPHA(dwColorMin) * (1.0f - fA) + A3DCOLOR_GETALPHA(dwColorMax)* fA);
	return A3DCOLORRGBA(nR, nG, nB, nA);
}

float genRandFloat(float fMin, float fMax)
{
	return fMin + (fMax - fMin) * _UnitRandom();
}

void exchangeUV(float& fU0, float& fU1, float& fV0, float& fV1, bool bIsUReverse, bool bIsVReverse, bool bIsUVInterchange)
{
	if (bIsUReverse)
		gfx_swap(fU0, fU1);

	if (bIsVReverse)
		gfx_swap(fV0, fV1);

	if (bIsUVInterchange)
	{
		gfx_swap(fU0, fV0);
		gfx_swap(fU1, fV1);
	}
}

//////////////////////////////////////////////////////////////////////////
// Impl for A3DPhysParticles
//////////////////////////////////////////////////////////////////////////


A3DPhysParticle::A3DPhysParticle() : 
m_fWidth(1.f),
m_fHeight(1.f),
m_f2DRot(0.f), 
m_bIsActived(false), 
m_fScale(1.f),
m_fScaleNoise(0.f),
m_fAge(0.f),
m_dwColor(0xffffffff),
m_pDummy(NULL)
{
	m_qDir.Set(0, 0, 0, 1);
}

A3DPhysParticle::~A3DPhysParticle()
{
	Reset();
}

void A3DPhysParticle::Set2DRot(float f2DRot)
{
	float fTimes = f2DRot / A3D_2PI;
	int nTimes = (int)fTimes;
	if (f2DRot > 0)
		m_f2DRot = f2DRot - A3D_2PI * nTimes;
	else
		m_f2DRot = f2DRot + A3D_2PI * (nTimes + 1);
}

void A3DPhysParticle::SetDummy(const A3DGFXElement* pEle, A3DGFXEx* pGfx)
{
	Reset();
	m_pDummy = pEle->Clone(pGfx);
}

A3DMATRIX4 A3DPhysParticle::GetMatrix() const
{
	A3DMATRIX4 mat;
	m_qDir.ConvertToMatrix(mat);
	mat.Scale(m_fScale, m_fScale, m_fScale);
	mat.SetRow(3, m_vPos);
	return mat;
}

void A3DPhysParticle::Update(DWORD dwTickTime)
{
	if (m_pDummy)
	{
		A3DMATRIX4 matScale = a3d_Scaling(m_fScale, m_fScale, m_fScale);
		A3DMATRIX4 matPar;
		m_qDir.ConvertToMatrix(matPar);
		matPar.SetRow(3, m_vPos);
		m_pDummy->SetDummyMatrix(matScale * matPar);
		m_pDummy->SetDummyColor(m_dwColor);
		m_pDummy->SetDummyScale(m_fScale);
		m_pDummy->DummyTick(dwTickTime);
	}
}


//////////////////////////////////////////////////////////////////////////
//	Impl for A3DPhysEmitter
//////////////////////////////////////////////////////////////////////////


static const char _format_par_height[]	= "ParHeight: %f";
static const char _format_par_width[]	= "ParWidth: %f";
static const char _format_scale_max[]	= "ScaleMax: %f";
static const char _format_scale_min[]	= "ScaleMin: %f";
static const char _format_rot_max[]		= "RotMax: %f";
static const char _format_rot_min[]		= "RotMin: %f";
static const char _format_color_max[]	= "ColorMax: %d";
static const char _format_color_min[]	= "ColorMin: %d";
static const char _format_face_to_view[] = "IsFaceToView: %d";
static const char _format_stopemitwhenfade[] = "StopEmitWhenFade: %d";
static const char _format_aff_num[]		= "AffectorCount: %d";

unsigned int A3DPhysEmitter::s_MaxSupportParticleNum = 1024;

A3DPhysEmitter::A3DPhysEmitter(A3DGFXEx* pGfx)
: A3DPhysFluidEmitter(pGfx)
{
	m_nEleType = ID_ELE_TYPE_PHYS_EMITTER;

	m_fParticleHeight = 1.f;
	m_fParticleWidth = 1.f;
	m_fScaleMax	= 1.f;
	m_fScaleMin	= 1.f;
	m_fRotMax = 0.f;
	m_fRotMin = 0.f;
	m_dwColorMax = A3DCOLORRGBA(255, 255, 255, 255);
	m_dwColorMin = A3DCOLORRGBA(255, 255, 255, 255);
	
	//m_bIsFaceToView	= true;
	m_iPhysParType = PHYSPAR_FACETOVIEWER;
	m_bStopEmitWhenFadeOut = false;

	m_poolPhysParticles = new A3DPhysParticle[s_MaxSupportParticleNum];
}

A3DPhysEmitter::~A3DPhysEmitter()
{
	ReleaseAffectors();
	ReleaseParticlePool();
}

void A3DPhysEmitter::UpdateParticleSize()
{
	for (unsigned int i = 0 ; i < s_MaxSupportParticleNum ; ++i) {
		m_poolPhysParticles[i].SetWidth(m_fParticleWidth);
		m_poolPhysParticles[i].SetHeight(m_fParticleHeight);
	}
}

void A3DPhysEmitter::ReleaseParticlePool()
{
	delete [] m_poolPhysParticles;
	m_poolPhysParticles = NULL;
}

void A3DPhysEmitter::ReleaseAffectors()
{
	for (size_t i = 0; i < m_Affectors.size(); i++)
		delete m_Affectors[i];
	m_Affectors.clear();
}

A3DGFXElement* A3DPhysEmitter::Clone(A3DGFXEx* pGfx) const
{
	A3DPhysEmitter* pEmitter = new A3DPhysEmitter(pGfx);
	return &(*pEmitter = *this);
}

A3DPhysEmitter& A3DPhysEmitter::operator = (const A3DPhysEmitter& rhs)
{
	if (this == &rhs)
		return *this;

	A3DPhysFluidEmitter::operator =(rhs);

	//m_bIsFaceToView = rhs.m_bIsFaceToView;
	m_fScaleMax		= rhs.m_fScaleMax;
	m_fScaleMin		= rhs.m_fScaleMin;
	m_fRotMax		= rhs.m_fRotMax;
	m_fRotMin		= rhs.m_fRotMin;
	m_dwColorMax	= rhs.m_dwColorMax;
	m_dwColorMin	= rhs.m_dwColorMin;
	m_fParticleHeight = rhs.m_fParticleHeight;
	m_fParticleWidth= rhs.m_fParticleWidth;
	m_iPhysParType	= rhs.m_iPhysParType;
	m_bStopEmitWhenFadeOut = rhs.m_bStopEmitWhenFadeOut;

	UpdateParticleSize();

	for (size_t i = 0; i < rhs.m_Affectors.size(); i++)
		m_Affectors.push_back(rhs.m_Affectors[i]->Clone());

	return *this;
}


void A3DPhysEmitter::InitParticle(A3DPhysParticle& pPar)
{
	pPar.SetAge(0);
	pPar.SetScale(genRandFloat(m_fScaleMin, m_fScaleMax));
	pPar.Set2DRot(genRandFloat(m_fRotMin, m_fRotMax));
	pPar.SetDir(A3DQUATERNION(0, 0, 0, 0));
	pPar.SetColor(genRandColor(m_dwColorMin, m_dwColorMax));

	if (m_pDummy)
		pPar.SetDummy(m_pDummy, m_pGfx);

	if (pPar.GetDummy())
	{
		pPar.GetDummy()->Stop();
		pPar.GetDummy()->Play();
	}

	pPar.SetActive(true);
}

void A3DPhysEmitter::TriggerAffectors(float fTimeSpan, const unsigned int* pIDs, int nParNum)
{
	if (m_Affectors.size())
	{
		PROC_DATA ProcData;
		int i;
		for (i = 0 ; i < nParNum ; ++i) {
			A3DPhysParticle* p = &m_poolPhysParticles[pIDs[i]];
			if (!p->IsActived())
				continue;
			
			// No affects
			ProcData.m_KPProcess.m_vPos		= A3DVECTOR3(0);
			ProcData.m_KPProcess.m_vDir		= A3DQUATERNION(1, A3DVECTOR3(0));
			ProcData.m_vAxisOff				= A3DVECTOR3(0);
			// Affects
			ProcData.m_KPProcess.m_fScale	= p->GetScale();
			ProcData.m_KPProcess.m_color	= p->GetColor();
			ProcData.m_KPProcess.m_fRad2D	= p->Get2DRot();
			ProcData.m_fTimeSpan			= p->GetAge();
			ProcData.m_fScaleNoise			= p->GetScaleNoise();
		
			for (size_t j = 0; j < m_Affectors.size(); j++)
				m_Affectors[j]->Process(ProcData, fTimeSpan);

			p->SetScale(ProcData.m_KPProcess.m_fScale);
			p->SetScaleNoise(ProcData.m_fScaleNoise);
			p->SetColor(ProcData.m_KPProcess.m_color);
			p->Set2DRot(ProcData.m_KPProcess.m_fRad2D);
		}
	}
}

bool A3DPhysEmitter::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	if (!A3DPhysFluidEmitter::Load(pDevice, pFile, dwVersion))
		return false;
	
	char szLine[AFILE_LINEMAXLEN];
	DWORD dwReadLen;
	int		nAffectorCount = 0;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_fParticleHeight, sizeof(m_fParticleHeight), &dwReadLen);
		pFile->Read(&m_fParticleWidth, sizeof(m_fParticleWidth), &dwReadLen);
		pFile->Read(&m_fScaleMax, sizeof(m_fScaleMax), &dwReadLen);
		pFile->Read(&m_fScaleMin, sizeof(m_fScaleMin), &dwReadLen);
		pFile->Read(&m_fRotMax, sizeof(m_fRotMax), &dwReadLen);
		pFile->Read(&m_fRotMin, sizeof(m_fRotMin), &dwReadLen);
		pFile->Read(&m_dwColorMax, sizeof(m_dwColorMax), &dwReadLen);
		pFile->Read(&m_dwColorMin, sizeof(m_dwColorMin), &dwReadLen);
		pFile->Read(&m_iPhysParType, sizeof(m_iPhysParType), &dwReadLen);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_par_height, &m_fParticleHeight);
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_par_width, &m_fParticleWidth);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_scale_max, &m_fScaleMax);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_scale_min, &m_fScaleMin);
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_rot_max, &m_fRotMax);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_rot_min, &m_fRotMin);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_color_max, &m_dwColorMax);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_color_min, &m_dwColorMin);

		// Face to view is more than one boolean value now (it means the particle type now)
		// 0 : normal (no face to view, no phys orientation)
		// 1 : face to view (no phys orientation)
		// 2 : phys orientation
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		int nIsFaceToView = 0;
		sscanf(szLine, _format_face_to_view, &nIsFaceToView);
		m_iPhysParType = nIsFaceToView;

		if (dwVersion >= 116)
		{
			int nRead;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_stopemitwhenfade, &nRead);
			m_bStopEmitWhenFadeOut = (nRead != 0);
		}

		if (dwVersion >= 117)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_aff_num, &nAffectorCount);

			for (int i = 0; i < nAffectorCount; i++)
			{
				A3DGFXKeyPointCtrlBase* p = A3DGFXKeyPointCtrlBase::LoadFromFile(pFile, dwVersion);
				if (p) 
				{
					p->Resume();
					m_Affectors.push_back(p);
				}
			}
		}
	}

	UpdateParticleSize();

	return true;
}

bool A3DPhysEmitter::Save(AFile* pFile)
{
	if (!A3DPhysFluidEmitter::Save(pFile))
		return false;

	if (pFile->IsBinary()) {
		DWORD dwWrite;
		
		pFile->Write(&m_fParticleHeight, sizeof(m_fParticleHeight), &dwWrite);
		pFile->Write(&m_fParticleWidth, sizeof(m_fParticleWidth), &dwWrite);
		pFile->Write(&m_fScaleMax, sizeof(m_fScaleMax), &dwWrite);
		pFile->Write(&m_fScaleMin, sizeof(m_fScaleMin), &dwWrite);
		pFile->Write(&m_fRotMax, sizeof(m_fScaleMax), &dwWrite);
		pFile->Write(&m_fRotMin, sizeof(m_fScaleMin), &dwWrite);
		pFile->Write(&m_dwColorMax, sizeof(m_dwColorMax), &dwWrite);
		pFile->Write(&m_dwColorMin, sizeof(m_dwColorMin), &dwWrite);
		pFile->Write(&m_iPhysParType, sizeof(m_iPhysParType), &dwWrite);
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_par_height, m_fParticleHeight);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_par_width, m_fParticleWidth);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_scale_max, m_fScaleMax);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_scale_min, m_fScaleMin);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_rot_max, m_fRotMax);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_rot_min, m_fRotMin);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_color_max, m_dwColorMax);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_color_min, m_dwColorMin);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_face_to_view, m_iPhysParType);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_stopemitwhenfade, m_bStopEmitWhenFadeOut);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_aff_num, m_Affectors.size());
		pFile->WriteLine(szLine);
	}

	for (size_t i = 0; i < m_Affectors.size(); i++)
		m_Affectors[i]->SaveToFile(pFile);

	return true;
}

bool A3DPhysEmitter::TickAnimation(DWORD dwTickTime)
{
	if (!A3DPhysFluidEmitter::TickAnimation(dwTickTime))
		return false;

	size_t nParNum = GetFluidObject().GetParticleNum();
	if (!nParNum || nParNum > s_MaxSupportParticleNum)
		return false;

	const NxVec3* pPositions	= GetFluidObject().GetParticlePositions();
	const NxQuat* pDirections	= GetFluidObject().GetParticleDirections();
	const float* pAges			= GetFluidObject().GetParticleLives();
	const unsigned int* pIDs	= GetFluidObject().GetParticleIDs();
	const float fLifespan		= GetFluidEmitterParam()->mParticleLifetime;

	size_t i;
	size_t nParDead = GetFluidObject().GetDeletedIdNum();
	const unsigned int* pDeadIDs = GetFluidObject().GetDeletedParticleIds();
	for (i = 0 ; i < nParDead ; ++i) 
	{
		if (pDeadIDs[i] >= 0 && pDeadIDs[i] < s_MaxSupportParticleNum)
			m_poolPhysParticles[pDeadIDs[i]].SetActive(false);
	}

	size_t nParCreated = GetFluidObject().GetCreatedIdNum();
	const unsigned int* pCreateIDs = GetFluidObject().GetCreatedParticleIds();
	for (i = 0 ; i < nParCreated ; ++i) 
	{
		if (pCreateIDs[i] >= 0 && pCreateIDs[i] < s_MaxSupportParticleNum)
			InitParticle(m_poolPhysParticles[pCreateIDs[i]]);
	}

	for (i = 0 ; i < nParNum ; ++i) 
	{
		A3DPhysParticle& pPhysPar = m_poolPhysParticles[pIDs[i]];
		if (pPhysPar.IsActived())
		{
			pPhysPar.SetPos(APhysXConverter::N2A_Vector3(pPositions[i]));
			if (pDirections)
				APhysXConverter::N2A_Quat(pDirections[pIDs[i]], pPhysPar.GetDirRef());
			pPhysPar.SetAge(fLifespan - pAges[i]);
			pPhysPar.Update(dwTickTime);
		}
	}



	TriggerAffectors(dwTickTime / 1000.f, pIDs, nParNum);
	return true;
}

bool A3DPhysEmitter::Play()
{
	A3DPhysFluidEmitter::Play();
	
	if (!IsInit())
	{
		InitBaseData();

		if (m_iPhysParType == PHYSPAR_FACETOVIEWER)
		{
			m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
				m_nRenderLayer,
				A3DFVF_A3DTLVERTEX,
				sizeof(A3DTLVERTEX),
				A3DPT_TRIANGLELIST,
				m_strTexture,
				m_Shader,
				m_bTileMode,
				m_bTexNoDownSample,
				m_pGfx->IsZTestEnable() && m_bZEnable,
				HasPixelShader(),
                m_bSoftEdge,
				m_bAbsTexPath);
		}
		else
		{
			m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
				m_nRenderLayer,
				A3DFVF_GFXVERTEX,
				sizeof(A3DGFXVERTEX),
				A3DPT_TRIANGLELIST,
				m_strTexture,
				m_Shader,
				m_bTileMode,
				m_bTexNoDownSample,
				m_pGfx->IsZTestEnable() && m_bZEnable,
				HasPixelShader(),
                m_bSoftEdge,
				m_bAbsTexPath);
		}

		SetInit(true);
	}

	return true;
}

bool A3DPhysEmitter::Stop()
{
	A3DPhysFluidEmitter::Stop();

	int i, nNum = s_MaxSupportParticleNum;
	for (i = 0 ; i < nNum ; ++i)
		m_poolPhysParticles[i].SetActive(false);
	return true;
}

bool A3DPhysEmitter::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	// 物理引擎没有准备好 (如果未准备好物理引擎，本对象不应当被创建出来)
	// (如果是在物理引擎准备好的时候创建的对象，而运行中物理引擎被关闭了则会需要这里的判断)
	if (!gPhysXEngine->IsValid() || !gGetAPhysXScene())
		return true;

	// 尚未创建物理 或 尚不存在需要渲染的粒子
	if (!GetFluidEmitterAttacher() || !GetFluidEmitterAttacher()->GetPhysXObjectComponentNum())
		return true;

	if (m_pDummy)
		Render_Dummy(pView);
	else if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

int A3DPhysEmitter::GetVertsCount()
{
	APhysXFluidObjectWrapper fluidObj = GetFluidObject();
	if (!fluidObj.IsValid())
		return 0;

	// Not support over s_MaxSupportParticleNum Particles
	if (fluidObj.GetParticleNum() > (int)s_MaxSupportParticleNum)
		return 0;

	return fluidObj.GetParticleNum() * 4;
}

#define FILL_GFXVERTEX(v, _x, _y, _z, _matindex, _diffuse, _specular, _u, _v) \
	v.x = _x, v.y = _y, v.z = _z, v.dwMatIndices = _matindex, v.diffuse = _diffuse, v.specular = _specular, v.tu = _u, v.tv = _v

// Update_3D fill buffer when it is not faced to viewer
int A3DPhysEmitter::Update_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex, const unsigned int * pIDs, int nParticleNum)
{
	ASSERT(m_iPhysParType != PHYSPAR_FACETOVIEWER);

	// fAlpha is [0.f, 1.f]
	float fAlpha = m_pGfx->GetActualAlpha() * A3DCOLOR_GETALPHA(kp.m_color) / 255.f;
	float fScale = m_pGfx->GetActualScale() * kp.m_fScale;
	
	bool bIsUsingOuterColor = m_pGfx->IsUsingOuterColor();
	A3DCOLORVALUE dwOuterColor = m_pGfx->GetOuterColor();
	
	float fU0 = m_fTexU;
	float fU1 = m_fTexU + m_fTexWid;

	float fV0 = m_fTexV;
	float fV1 = m_fTexV + m_fTexHei;

	exchangeUV(fU0, fU1, fV0, fV1, m_bUReverse, m_bVReverse, m_bUVInterchange);

	int nIndex, nCount = 0;
	for (nIndex = 0 ; nIndex < nParticleNum ; ++nIndex) {
		int nID = pIDs[nIndex];
		A3DPhysParticle& pPar = m_poolPhysParticles[nID];
		if (!pPar.IsActived())
			continue;

		A3DCOLOR dwCurColor = pPar.GetColor();
		if (bIsUsingOuterColor)
			dwCurColor = (A3DCOLORVALUE(dwCurColor) * dwOuterColor).ToRGBAColor();

		A3DCOLOR dwAlpha = A3DCOLOR(A3DCOLOR_GETALPHA(dwCurColor) * fAlpha);
		if (dwAlpha < 5)
			continue;

		dwCurColor = (dwCurColor & _cMask) | A3DCOLORRGBA(0, 0, 0, dwAlpha);

		float fCurScale = pPar.GetScale() + pPar.GetScaleNoise();
		float fTotalScale = fScale * fCurScale;

		// triangles always face to camera
			
		A3DVECTOR3 vAxisX, vAxisY;
		A3DVECTOR3 vAxisZ;

		switch (m_iPhysParType)
		{
		case PHYSPAR_PHYSORIENT:
			vAxisX = RotateVec(pPar.GetDir(), _unit_x);
			vAxisY = RotateVec(pPar.GetDir(), _unit_z);
			break;
		default:
			if (pPar.Get2DRot())
			{
				A3DQUATERNION q(_unit_z, pPar.Get2DRot());
				q = pPar.GetDir() * q;
				vAxisX = RotateVec(q, _unit_x);
				vAxisY = RotateVec(q, _unit_y);
			}
			else
			{
				vAxisX = RotateVec(pPar.GetDir(), _unit_x);
				vAxisY = RotateVec(pPar.GetDir(), _unit_y);
			}
			break;
		}

		vAxisX *= pPar.GetWidth();
		vAxisY *= pPar.GetHeight();
	
		A3DVECTOR3 v1 = (vAxisX + vAxisY) * fTotalScale;
		A3DVECTOR3 v2 = (vAxisX - vAxisY) * fTotalScale;

		A3DVECTOR3 verts[4];
		verts[0]	= pPar.GetPos() - v2;
		verts[1]	= pPar.GetPos() + v1;
		verts[2]	= pPar.GetPos() - v1;
		verts[3]	= pPar.GetPos() + v2;

		// Render Particle Counter Increased
		// And index the buffer
		int nBufIndex = nCount++ << 2;

		if (pView->GetCamera()->IsMirrored())
		{
			FILL_GFXVERTEX(pVerts[nBufIndex		], verts[0].x, verts[0].y, verts[0].z, nMatrixIndex, dwCurColor, dwSpecular, fU0, fV1);
			FILL_GFXVERTEX(pVerts[nBufIndex + 1 ], verts[1].x, verts[1].y, verts[1].z, nMatrixIndex, dwCurColor, dwSpecular, fU1, fV1);
			FILL_GFXVERTEX(pVerts[nBufIndex + 2 ], verts[2].x, verts[2].y, verts[2].z, nMatrixIndex, dwCurColor, dwSpecular, fU0, fV0);
			FILL_GFXVERTEX(pVerts[nBufIndex + 3 ], verts[3].x, verts[3].y, verts[3].z, nMatrixIndex, dwCurColor, dwSpecular, fU1, fV0);
		}
		else
		{
			FILL_GFXVERTEX(pVerts[nBufIndex		], verts[0].x, verts[0].y, verts[0].z, nMatrixIndex, dwCurColor, dwSpecular, fU0, fV0);
			FILL_GFXVERTEX(pVerts[nBufIndex + 1 ], verts[1].x, verts[1].y, verts[1].z, nMatrixIndex, dwCurColor, dwSpecular, fU1, fV0);
			FILL_GFXVERTEX(pVerts[nBufIndex + 2 ], verts[2].x, verts[2].y, verts[2].z, nMatrixIndex, dwCurColor, dwSpecular, fU0, fV1);
			FILL_GFXVERTEX(pVerts[nBufIndex + 3 ], verts[3].x, verts[3].y, verts[3].z, nMatrixIndex, dwCurColor, dwSpecular, fU1, fV1);
		}

#ifdef GFX_EDITOR

		A3DAABB& aabb = m_AABB; 
		for (int i = 0 ; i < 4 ; ++i)
		{
			A3DVECTOR3 v(
				pVerts[nBufIndex + i].x,
				pVerts[nBufIndex + i].y,
				pVerts[nBufIndex + i].z);
			if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;
			if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;
			if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;
			if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;
			if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;
			if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;
		}
#endif
	}

	A3DMATRIX4 matWorld = _identity_mat4;

	if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
		m_pDevice->SetWorldMatrix(matWorld);
	else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
	{
		matWorld.Transpose();
		m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
	}
	else
		m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);	

	return nCount * 4;
}

#define FILL_TLVERTEX(v, _vert, _diffuse, _specular, _u, _v) \
	v.x = _vert.x, v.y = _vert.y, v.z = _vert.z, v.rhw = _vert.w, v.diffuse = _diffuse, v.specular = _specular, v.tu = _u, v.tv = _v

int A3DPhysEmitter::Update_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts, const unsigned int * pIDs, int nParticleNum)
{
	ASSERT(m_iPhysParType == PHYSPAR_FACETOVIEWER);
	const static A3DCOLOR dwSpecular = A3DCOLORRGBA(0, 0, 0, 255);

	// fAlpha is [0.f, 1.f]
	float fAlpha = m_pGfx->GetActualAlpha() * A3DCOLOR_GETALPHA(kp.m_color) / 255.f;
	float fScale = m_pGfx->GetActualScale() * kp.m_fScale;
	
	bool bIsUsingOuterColor = m_pGfx->IsUsingOuterColor();
	const A3DCOLORVALUE& dwOuterColor = m_pGfx->GetOuterColor();

	float fU0 = m_fTexU;
	float fU1 = m_fTexU + m_fTexWid;

	float fV0 = m_fTexV;
	float fV1 = m_fTexV + m_fTexHei;

	exchangeUV(fU0, fU1, fV0, fV1, m_bUReverse, m_bVReverse, m_bUVInterchange);
	
	int nCount = 0;

	A3DCameraBase* pCamera = pView->GetCamera();
	A3DMATRIX4 matVPS = pCamera->GetVPTM() * pView->GetViewScale();
	A3DVECTOR4 verts[4];

	for (int nIndex = 0 ; nIndex < nParticleNum ; ++nIndex) {
		unsigned int nID = pIDs[nIndex];
		A3DPhysParticle& pPar = m_poolPhysParticles[nID];
		if (!pPar.IsActived())
			continue;

		A3DCOLOR dwCurColor = pPar.GetColor();
		if (bIsUsingOuterColor)
			dwCurColor = (A3DCOLORVALUE(dwCurColor) * dwOuterColor).ToRGBAColor();

		A3DCOLOR dwAlpha = A3DCOLOR(A3DCOLOR_GETALPHA(dwCurColor) * fAlpha);
		if (dwAlpha < 5)
			continue;

		dwCurColor = (dwCurColor & _cMask) | A3DCOLORRGBA(0, 0, 0, dwAlpha);
		A3DVECTOR4 vCenter = TransformToScreen(pPar.GetPos(), matVPS);
	
		if (vCenter.z < 0 || vCenter.z > 1.f)
			continue;

		float fCurScale = pPar.GetScale() + pPar.GetScaleNoise();
		float fTotalScale = fScale * fCurScale;

		float fWidth, fHeight;
		fWidth = fTotalScale * pPar.GetWidth();
		fHeight = fTotalScale * pPar.GetHeight();

		A3DVECTOR3 vExts = pPar.GetPos() + pCamera->GetRight() * fWidth + pCamera->GetUp() * fHeight;
		vExts = matVPS * vExts;
		fWidth	= vExts.x - vCenter.x;
		fHeight	= vCenter.y - vExts.y;

		if (pPar.Get2DRot())
		{
			float fSin = (float)sin(pPar.Get2DRot());
			float fCos = (float)cos(pPar.Get2DRot());

			verts[0].x = vCenter.x - fWidth  * fCos + fHeight * fSin;
			verts[0].y = vCenter.y + fHeight * fCos + fWidth  * fSin;
			verts[0].z = vCenter.z;
			verts[0].w = vCenter.w;

			verts[1].x = vCenter.x - fWidth  * fCos - fHeight * fSin;
			verts[1].y = vCenter.y - fHeight * fCos + fWidth  * fSin;
			verts[1].z = vCenter.z;
			verts[1].w = vCenter.w;

			verts[2].x = vCenter.x + fWidth  * fCos + fHeight * fSin;
			verts[2].y = vCenter.y + fHeight * fCos - fWidth  * fSin;
			verts[2].z = vCenter.z;
			verts[2].w = vCenter.w;

			verts[3].x = vCenter.x + fWidth  * fCos - fHeight * fSin;
			verts[3].y = vCenter.y - fHeight * fCos - fWidth  * fSin;
			verts[3].z = vCenter.z;
			verts[3].w = vCenter.w;
		}
		else
		{
			verts[0].x = vCenter.x - fWidth;
			verts[0].y = vCenter.y + fHeight;
			verts[0].z = vCenter.z;
			verts[0].w = vCenter.w;

			verts[1].x = vCenter.x - fWidth;
			verts[1].y = vCenter.y - fHeight;
			verts[1].z = vCenter.z;
			verts[1].w = vCenter.w;

			verts[2].x = vCenter.x + fWidth;
			verts[2].y = vCenter.y + fHeight;
			verts[2].z = vCenter.z;
			verts[2].w = vCenter.w;

			verts[3].x = vCenter.x + fWidth;
			verts[3].y = vCenter.y - fHeight;
			verts[3].z = vCenter.z;
			verts[3].w = vCenter.w;
		}

		int nBufIndex = nCount++ << 2;
		FILL_TLVERTEX(pVerts[nBufIndex    ], verts[0], dwCurColor, dwSpecular, fU0, fV1);
		FILL_TLVERTEX(pVerts[nBufIndex + 1], verts[1], dwCurColor, dwSpecular, fU0, fV0);
		FILL_TLVERTEX(pVerts[nBufIndex + 2], verts[2], dwCurColor, dwSpecular, fU1, fV1);
		FILL_TLVERTEX(pVerts[nBufIndex + 3], verts[3], dwCurColor, dwSpecular, fU1, fV0);
		

#ifdef GFX_EDITOR
		for (int i = 0 ; i < 4 ; ++i) {
			A3DVECTOR3 v(verts[i].x, verts[i].y, verts[i].z);
			pView->InvTransform(v, v);

			A3DAABB& aabb = m_AABB;
			if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;
			if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;
			if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;
			if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;
			if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;
			if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;
		}
#endif
	}

	return nCount * 4;
}

int A3DPhysEmitter::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	if (!gPhysXEngine->IsValid() || !GetPhysObjDescMan() || !GetPhysObjDescMan()->GetPhysScene())
		return 0;

	KEY_POINT kp;

	PhysGetCurKeyPoint(kp);

	APhysXFluidObjectWrapper fluidObj = GetFluidObject();
	if (!fluidObj.IsValid())
		return 0;

	int nParticleNum = fluidObj.GetParticleNum();
	if (nParticleNum > (int)s_MaxSupportParticleNum)
		return 0;

#ifdef GFX_EDITOR
	m_AABB.Clear(); 
#endif

	int nCount;
	if (m_iPhysParType == PHYSPAR_FACETOVIEWER)
		nCount = Update_2D(kp, pView, (A3DTLVERTEX*)pBuffer, fluidObj.GetParticleIDs(), nParticleNum);
	else
		nCount = Update_3D(kp, pView, (A3DGFXVERTEX*)pBuffer, nMatrixIndex, fluidObj.GetParticleIDs(), nParticleNum);
	
	_gfx_par_render_count += nCount / 4;

	return nCount;
}

void A3DPhysEmitter::Render_Dummy(A3DViewport* pView)
{
	APhysXFluidObjectWrapper fluidObj = GetFluidObject();
	if (!fluidObj.IsValid())
		return;
	
	int nParticleNum = fluidObj.GetParticleNum();
	if (nParticleNum > (int)s_MaxSupportParticleNum)
		return;

	const NxVec3* pPositions = fluidObj.GetParticlePositions();
	const unsigned int* pIDs = fluidObj.GetParticleIDs();

#ifndef GFX_EDITOR

	size_t i, nNum = nParticleNum;
	for (i = 0 ; i < nNum ; ++i) 
	{
		int iParIdx = pIDs[i];
		if (m_poolPhysParticles[iParIdx].IsActived() && m_poolPhysParticles[iParIdx].GetDummy())
		{
			m_poolPhysParticles[iParIdx].GetDummy()->Render(pView);
		}
	}

#else

	size_t i, nNum = nParticleNum;
	m_AABB.Clear();
	for (i = 0 ; i < nNum ; ++i) 
	{
		int iParIdx = pIDs[i];
		if (m_poolPhysParticles[iParIdx].IsActived() && m_poolPhysParticles[iParIdx].GetDummy())
		{	
			A3DGFXElement* pDummy = m_poolPhysParticles[iParIdx].GetDummy();
			pDummy->Render(pView);

			A3DAABB& aabb = m_AABB; 
			const A3DAABB& r = pDummy->GetAABB();
			A3DVECTOR3 mins = r.Mins;
			A3DVECTOR3 maxs = r.Maxs;

			if (mins.x < aabb.Mins.x) aabb.Mins.x = mins.x;
			if (mins.y < aabb.Mins.y) aabb.Mins.y = mins.y;
			if (mins.z < aabb.Mins.z) aabb.Mins.z = mins.z;
			if (maxs.x > aabb.Maxs.x) aabb.Maxs.x = maxs.x;
			if (maxs.y > aabb.Maxs.y) aabb.Maxs.y = maxs.y;
			if (maxs.z > aabb.Maxs.z) aabb.Maxs.z = maxs.z;
		}
	}

#endif
}

// Set / Get property
bool A3DPhysEmitter::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp)
	{
	case ID_GFXOP_PHYSPAR_PE_MAXSCALE:
		m_fScaleMax = (float)prop;
		break;
	case ID_GFXOP_PHYSPAR_PE_MINSCALE:
		m_fScaleMin = (float)prop;
		break;
	case ID_GFXOP_PHYSPAR_PE_MAXROT:
		m_fRotMax = (float)prop;
		break;
	case ID_GFXOP_PHYSPAR_PE_MINROT:
		m_fRotMin = (float)prop;
		break;
	case ID_GFXOP_PHYSPAR_PE_MAXCOLOR:
		m_dwColorMax = (A3DCOLOR)prop;
		break;
	case ID_GFXOP_PHYSPAR_PE_MINCOLOR:
		m_dwColorMin = (A3DCOLOR)prop;
		break;
	case ID_GFXOP_PHYSPAR_PE_PARORIENT:
		m_iPhysParType = (int)prop;
		switch (m_iPhysParType)
		{
		case PHYSPAR_PHYSORIENT:
			m_pFluidSyncData->SetFluidObjectDescCreator(IPhysXFluidObjectDescCreator::CreateFluidObjDescCreatorByType(FLUIDOBJ_ORIENT));
			break;
		case PHYSPAR_NORMAL:
		case PHYSPAR_FACETOVIEWER:
			m_pFluidSyncData->SetFluidObjectDescCreator(IPhysXFluidObjectDescCreator::CreateFluidObjDescCreatorByType(FLUIDOBJ_NORMAL));
			break;
		default:
			return false;
		}
		break;
	case ID_GFXOP_PHYSPAR_PE_PARWIDTH:
		m_fParticleWidth = (float)prop;
		UpdateParticleSize();
		break;
	case ID_GFXOP_PHYSPAR_PE_PARHEIGHT:
		m_fParticleHeight = (float)prop;
		UpdateParticleSize();
		break;
	case ID_GFXOP_PHYSPAR_PE_STOP_EMIT_WHEN_FADE:
		m_bStopEmitWhenFadeOut = prop;
		break;
	default:
		return A3DGFXElement::SetProperty(nOp, prop);
	}
	
	return true;
}

GFX_PROPERTY A3DPhysEmitter::GetProperty(int nOp) const
{
	switch(nOp)
	{
	case ID_GFXOP_PHYSPAR_PE_MAXSCALE:
		return GFX_PROPERTY(m_fScaleMax);
	case ID_GFXOP_PHYSPAR_PE_MINSCALE:
		return GFX_PROPERTY(m_fScaleMin);
	case ID_GFXOP_PHYSPAR_PE_MAXROT:
		return GFX_PROPERTY(m_fRotMax);
	case ID_GFXOP_PHYSPAR_PE_MINROT:
		return GFX_PROPERTY(m_fRotMin);
	case ID_GFXOP_PHYSPAR_PE_MAXCOLOR:
		return GFX_PROPERTY(m_dwColorMax);
	case ID_GFXOP_PHYSPAR_PE_MINCOLOR:
		return GFX_PROPERTY(m_dwColorMin);
	case ID_GFXOP_PHYSPAR_PE_PARORIENT:
		return GFX_PROPERTY(m_iPhysParType).SetType(GFX_VALUE_PHYSPAR_ORIENT);
	case ID_GFXOP_PHYSPAR_PE_PARWIDTH:
		return GFX_PROPERTY(m_fParticleWidth);
	case ID_GFXOP_PHYSPAR_PE_PARHEIGHT:
		return GFX_PROPERTY(m_fParticleHeight);
	case ID_GFXOP_PHYSPAR_PE_STOP_EMIT_WHEN_FADE:
		return GFX_PROPERTY(m_bStopEmitWhenFadeOut);
	}
	return A3DGFXElement::GetProperty(nOp);
}

size_t A3DPhysEmitter::GetSubProtertyCount(int nSubId)
{
	PropIdxArray propByType;
	return GetCtrlListByType(nSubId, propByType).size();
}

bool A3DPhysEmitter::AddSubProterty(int nSubId)
{
	if (m_Affectors.size() >= 5)
		return false;

	A3DGFXKeyPointCtrlBase* p = A3DGFXKeyPointCtrlBase::CreateKPCtrl(nSubId);
	if (p == NULL)
		return false;

	m_Affectors.push_back(p);
	return true;
}

bool A3DPhysEmitter::RemoveSubProterty(int nSubId, size_t nSubIdx)
{
	PropIdxArray propByType;
	GetCtrlListByType(nSubId, propByType);
	if (nSubIdx >= propByType.size())
		return false;

	delete m_Affectors[propByType[nSubIdx]];
	m_Affectors.erase(m_Affectors.begin() + propByType[nSubIdx]);
	return true;
}

bool A3DPhysEmitter::SetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId, const GFX_PROPERTY& prop)
{
	PropIdxArray propByType;
	GetCtrlListByType(nSubId, propByType);
	if (nSubIdx >= propByType.size())
		return false;

	return m_Affectors[propByType[nSubIdx]]->SetProperty(nSubOpId, prop);
}

GFX_PROPERTY A3DPhysEmitter::GetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId) const
{
	PropIdxArray propByType;
	GetCtrlListByType(nSubId, propByType);
	if (nSubIdx >= propByType.size())
		return GFX_PROPERTY();

	return m_Affectors[propByType[nSubIdx]]->GetProperty(nSubOpId);
}

const A3DPhysEmitter::PropIdxArray& A3DPhysEmitter::GetCtrlListByType(int nType, PropIdxArray& lst) const
{
	lst.clear();
	for (size_t i = 0; i < m_Affectors.size(); i++)
		if (m_Affectors[i]->GetType() == nType)
			lst.push_back(i);
	return lst;
}

void A3DPhysEmitter::DoFadeOut()
{
	if (m_bStopEmitWhenFadeOut)
		StopParticleEmit();
}

#endif // end of A3D_PHYSX