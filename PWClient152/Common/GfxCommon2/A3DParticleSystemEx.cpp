#include "StdAfx.h"
#include "A3DParticleSystemEx.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DConfig.h"
#include "A3DGFXExMan.h"
#include "A3DGFXRenderSlot.h"
#include "A3DGFXStreamMan.h"

static const char _format_quota[]		= "Quota: %d";
static const char _format_width[]		= "ParticleWidth: %f";
static const char _format_height[]		= "ParticleHeight: %f";
static const char _format_aff_num[]		= "AffectorCount: %d";
static const char _format_3d[]			= "3DParticle: %d";
static const char _format_facing[]		= "Facing: %d";
static const char _format_scl_no_off[]	= "ScaleNoOff: %d";
static const char* _format_no_scale		= "NoScale: %d";
static const char* _format_org_pt		= "OrgPt: %f";
static const char* _format_isuseparuv	= "IsUseParUV: %d";
static const char* _format_isstartonground = "IsStartOnGrnd: %d";
static const char* _format_stopemitwhenfade = "StopEmitWhenFade: %d";
static const char* _format_init_random_texture = "InitRandomTexture: %d";
static const char* _format_zoffset		= "ZOffset: %f";

static const A3DCOLOR _cMask = A3DCOLORRGBA(255, 255, 255, 0);
static const int _max_quota = 750;
static const float _lod_dist = 10.0f;
static const float _lod_dist2 = 40.0f;
static const float _lod_value = _lod_dist / 1.2f;
static const int _lod_par_min_count = 3;

extern int _gfx_par_total_count;
extern int _gfx_par_render_count;
extern int _gfx_par_active_count;

volatile int g_nCurParticlePoolSize = 0;

A3DParticleSystemEx::A3DParticleSystemEx(A3DGFXEx* pGfx, int nType)
	: A3DGFXElement(pGfx), m_matSelfScale(_identity_mat4)
{
	m_nEleType			= nType;
	m_pEmitter			= NULL;
	m_nQuota			= -1;
	m_nEmissionCount	= 0;
	m_fParticleWidth	= 1.0f;
	m_fParticleHeight	= 1.0f;
	m_b3DParticle		= false;
	m_bFacing			= false;
	m_bFirstTick		= true;
	m_bScaleNoOffset	= false;
	m_bNoWidthScale		= false;
	m_bNoHeightScale	= false;
	m_fOrgPtWidth		= 0.5f;
	m_fOrgPtHeight		= 0.5f;
	m_bApplyMotion		= false;
	m_bIsUseParUV		= false;
	m_bInitRandom		= false;
	m_bIsStartWithGrndHeight = false;
	m_bTriggerEmitter	= false;
	m_bStopEmitWhenFadeOut = false;
	m_vParAxisOff.Clear();
	m_fZOffset			= 0.0f;
}

bool A3DParticleSystemEx::Init(A3DDevice* pDevice)
{
	A3DGFXElement::Init(pDevice);

	switch (m_nEleType)
	{
	case ID_ELE_TYPE_PARTICLE_POINT:
		m_pEmitter = A3DParticleEmitter::CreateEmitter(EMITTER_TYPE_POINT);
		break;
	case ID_ELE_TYPE_PARTICLE_BOX:
		m_pEmitter = A3DParticleEmitter::CreateEmitter(EMITTER_TYPE_BOX);
		break;
	case ID_ELE_TYPE_PARTICLE_MULTIPLANE:
		m_pEmitter = A3DParticleEmitter::CreateEmitter(EMITTER_TYPE_MULTIPLANE);
		break;
	case ID_ELE_TYPE_PARTICLE_ELLIPSOID:
		m_pEmitter = A3DParticleEmitter::CreateEmitter(EMITTER_TYPE_ELLIPSOID);
		break;
	case ID_ELE_TYPE_PARTICLE_CYLINDER:
		m_pEmitter = A3DParticleEmitter::CreateEmitter(EMITTER_TYPE_CYLINDER);
		break;
	case ID_ELE_TYPE_PARTICLE_CURVE:
		m_pEmitter = A3DParticleEmitter::CreateEmitter(EMITTER_TYPE_CURVE);
		break;
	case ID_ELE_TYPE_PARTICLE_SKELETON:
		m_pEmitter = A3DParticleEmitter::CreateEmitter(EMITTER_TYPE_SKELETON);
		break;
	default:
		return false;
	}

	return true;
}

A3DGFXElement* A3DParticleSystemEx::Clone(A3DGFXEx* pGfx) const
{
	A3DParticleSystemEx* p = new A3DParticleSystemEx(pGfx, m_nEleType);
	return &(*p = *this);
}

A3DParticleSystemEx& A3DParticleSystemEx::operator = (const A3DParticleSystemEx& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);
	m_nQuota			= src.m_nQuota;
	m_nEmissionCount	= src.m_nEmissionCount;
	m_fParticleWidth	= src.m_fParticleWidth;
	m_fParticleHeight	= src.m_fParticleHeight;
	m_b3DParticle		= src.m_b3DParticle;
	m_bFacing			= src.m_bFacing;
	m_bScaleNoOffset	= src.m_bScaleNoOffset;
	m_vParAxisOff		= src.m_vParAxisOff;
	m_bNoWidthScale		= src.m_bNoWidthScale;
	m_bNoHeightScale	= src.m_bNoHeightScale;
	m_fOrgPtWidth		= src.m_fOrgPtWidth;
	m_fOrgPtHeight		= src.m_fOrgPtHeight;
	m_bApplyMotion		= src.m_bApplyMotion;
	m_bIsUseParUV		= src.m_bIsUseParUV;
	m_bIsStartWithGrndHeight = src.m_bIsStartWithGrndHeight;
	m_bStopEmitWhenFadeOut = src.m_bStopEmitWhenFadeOut;
	m_bInitRandom = src.m_bInitRandom;
	m_fZOffset			= src.m_fZOffset;

	m_pEmitter->CloneFrom(src.m_pEmitter);

	ReleaseAffectors();
	for (size_t i = 0; i < src.m_Affectors.size(); i++)
		m_Affectors.push_back(src.m_Affectors[i]->Clone());

	return *this;
}

void A3DParticleSystemEx::ResumeLoop() 
{ 
	A3DGFXElement::ResumeLoop(); 
	Resume();
}

void A3DParticleSystemEx::Resume()
{
	m_bFirstTick = true;
	m_bTriggerEmitter = true;
	m_nEmissionCount = 0;
	m_LstAct.RemoveAll();
	m_LstFree.RemoveAll();
	size_t i;
	
	for (i = 0; i < m_Particles.size(); i++)
		m_LstFree.AddTail(m_Particles[i]);
	
	m_pEmitter->Resume();
	
	for (i = 0; i < m_Affectors.size(); i++)
		m_Affectors[i]->Resume();
}

void A3DParticleSystemEx::ReleasePool()
{
	if (m_Particles.size())
	{
		A3DGFXExMan* pMan = AfxGetGFXExMan();
		ParticlePoolList temp;

		for (size_t i = 0; i < m_Particles.size(); i++)
		{
			A3DParticle* p = m_Particles[i];
			p->Reset();
			temp.AddTail(p);
		}

		pMan->LockParticlePoolList();
		ParticlePoolList& pool = pMan->GetParticlePool();
		ALISTPOSITION pos = temp.GetHeadPosition();

		while (pos)
			pool.AddTail(temp.GetNext(pos));

		g_nCurParticlePoolSize = pool.GetCount();
		pMan->UnlockParticlePoolList();

		_gfx_par_total_count -= m_Particles.size();
		m_Particles.clear();
	}

	m_LstAct.RemoveAll();
	m_LstFree.RemoveAll();
}

bool A3DParticleSystemEx::Play()
{
	if (!IsInit())
	{
		InitBaseData();
		
		int nPoolSize = m_pEmitter->CalcPoolSize();
		
		if (m_nQuota == -1 || nPoolSize <= m_nQuota)
			SetPoolSize(nPoolSize);
		else
			SetPoolSize(m_nQuota);
		
		m_fOrgPtWidCo = 1.0f - m_fOrgPtWidth * 2.0f;
		m_fOrgPtHeiCo = 1.0f - m_fOrgPtHeight * 2.0f;
		
		SetInit(true);
	}

	//	emitter may do some initialize work on start to play gfx
	if (m_pGfx->IsResourceReady())
	{
		m_pEmitter->OnStart(this);
		m_bTriggerEmitter = true;
	}

	return true;
}

bool A3DParticleSystemEx::StopParticleEmit()
{
	m_bTriggerEmitter = false;
	return true;
}

void A3DParticleSystemEx::DoFadeOut()
{
	if (m_bStopEmitWhenFadeOut)
		StopParticleEmit();
}

void A3DParticleSystemEx::SetPoolSize(int nSize)
{
	ReleasePool();

	if (nSize > _max_quota)
		nSize = _max_quota;

	if (nSize == 0) return;

	A3DGFXExMan* pMan = AfxGetGFXExMan();
#ifdef _ANGELICA21

	if (nSize >= 20)
	{
		int nPriority = pMan->GetPriority();

		if (nPriority < 4)
		{
			float fScale;

			if (nPriority == 3) 
				fScale = .83f;
			else if (nPriority == 2)
				fScale = .67f;
			else
				fScale = .5f;

			nSize = int(nSize * fScale);
		}
	}

#endif

	pMan->LockParticlePoolList();
	ParticlePoolList& pool = pMan->GetParticlePool();
	int i;

	for (i = 0; i < nSize; i++)
	{
		A3DParticle* p;

		if (pool.GetCount())
			p = pool.RemoveHead();
		else
		{
#ifdef _ANGELICA21
			break;
#else
			p = new A3DParticle;
#endif
		}

		m_Particles.push_back(p);
		m_LstFree.AddTail(p);
	}

	_gfx_par_total_count += i;
	g_nCurParticlePoolSize = pool.GetCount();
	pMan->UnlockParticlePoolList();

	if (m_b3DParticle)
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
	else
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
}

int A3DParticleSystemEx::GetVertsCount()
{
	return m_LstAct.GetCount() * 4;
}

inline void A3DParticleSystemEx::Expire(float fTimeSpan)
{
	ALISTPOSITION posCur;
	ALISTPOSITION pos = m_LstAct.GetHeadPosition();
	A3DParticle* p;

	while (pos)
	{
		posCur = pos;
		p = m_LstAct.GetNext(pos);

		if (p->m_fTTL + fTimeSpan >= p->m_fTotalTTL)
		{
			m_LstAct.RemoveAt(posCur);
			m_LstFree.AddTail(p);
		}
		else
			break;
	}
}

inline void A3DParticleSystemEx::InitParticle(A3DParticle* p)
{
	m_pEmitter->InitParticle(p);
	
	if (m_bInitRandom)
	{
		p->InitParticleUV( m_nTexRow == 1 ? 0 : a_Random(0, m_nTexRow-1), m_nTexCol == 1 ? 0 : a_Random(0, m_nTexCol-1), m_fTexWid, m_fTexHei);
	}
	else
	{
		p->InitParticleUV();
	}
	
	if (!m_pEmitter->IsBind() && m_bIsStartWithGrndHeight)
	{
#ifdef _ANGELICA21
		p->m_vPos.y = AfxGetGrndNorm(p->m_vPos, NULL, m_pGfx->IsUseRaytrace());
#else
		p->m_vPos.y = AfxGetGrndNorm(p->m_vPos, NULL);
#endif
	}

	if (p->m_pDummy)
	{
		p->m_pDummy->Stop();
		p->m_pDummy->Play();
	}
	else if (m_pDummy)
	{
		p->m_pDummy = m_pDummy->Clone(m_pGfx);
		p->m_pDummy->Play();
	}
}

inline A3DParticle* A3DParticleSystemEx::AddParticle()
{
	if (m_LstFree.GetCount() == 0) return NULL;
	A3DParticle* p = m_LstFree.RemoveHead();
	m_LstAct.AddTail(p);
	return p;
}

inline int A3DParticleSystemEx::GetEmissionCount(float fTimeSpan)
{
	int nCount = m_pEmitter->GetEmissionCount(fTimeSpan);

	// GFX LOD only take effect when it is rendered as a 3d GFX
	if (m_pGfx->GetUseLOD() && !m_pGfx->Is2DRender())
	{
		float fDist = m_pGfx->GetDistToCam();

		if (fDist > _lod_dist2 || (fDist > _lod_dist && nCount > _lod_par_min_count))
		{
			float lod = m_pGfx->GetExtent() * _lod_value / fDist;
			return lod >= 1.0f ? nCount : (int)ceilf(lod * nCount);
		}
	}

	return nCount;
}

void A3DParticleSystemEx::TriggerEmitter(float fTimeSpan)
{
	if (m_nQuota > 0 && m_nEmissionCount >= m_nQuota) return;
	int nRequested = GetEmissionCount(fTimeSpan);
	if (nRequested == 0) return;
	if (m_nQuota > 0 && m_nEmissionCount + nRequested > m_nQuota)
	{
		nRequested = m_nQuota - m_nEmissionCount;
		m_nEmissionCount = m_nQuota;
	}
	else
		m_nEmissionCount += nRequested;

	for (int i = 0; i < nRequested; i++)
	{
		A3DParticle* p = AddParticle();
		if (p == NULL) return;
		p->m_fWidth = m_fParticleWidth;
		p->m_fHeight = m_fParticleHeight;
		InitParticle(p);
	}
}

void A3DParticleSystemEx::TriggerEmitter(
	float fTimeSpan,
	const A3DVECTOR3& vEmit,
	const A3DQUATERNION& qEmit)
{
	if (m_nQuota > 0 && m_nEmissionCount >= m_nQuota) return;
	int nRequested = GetEmissionCount(fTimeSpan);
	if (nRequested == 0) return;
	if (m_nQuota > 0 && m_nEmissionCount + nRequested > m_nQuota)
	{
		nRequested = m_nQuota - m_nEmissionCount;
		m_nEmissionCount = m_nQuota;
	}
	else
		m_nEmissionCount += nRequested;

	A3DMATRIX4 matWorld;
	qEmit.ConvertToMatrix(matWorld);
	matWorld.SetRow(3, vEmit);
	A3DVECTOR3 vAxisOff = matWorld * m_vParAxisOff - m_vParAxisOff;

	if (m_bFirstTick || nRequested == 1)
	{
		for (int i = 0; i < nRequested; i++)
		{
			A3DParticle* p = AddParticle();
			if (p == NULL) return;
			p->m_fWidth = m_fParticleWidth;
			p->m_fHeight = m_fParticleHeight;
			InitParticle(p);
			p->m_vAxisOff = vAxisOff;
		}
	}
	else
	{
		float fSegs = 1.0f / nRequested;
		float fPortion, fR;
		A3DVECTOR3 vInt;
		A3DQUATERNION qInt;

		for (int i = 1; i <= nRequested; i++)
		{
			A3DParticle* p = AddParticle();
			if (p == NULL) return;
			p->m_fWidth = m_fParticleWidth;
			p->m_fHeight = m_fParticleHeight;

			fPortion = i * fSegs;
			fR = 1.0f - fPortion;

			vInt = m_vOldPos * fR + vEmit * fPortion;
			qInt = SLERPQuad(m_qOldDir, qEmit, fPortion);

			m_pEmitter->SetParentDirAndPos(vInt, qEmit);
			InitParticle(p);
			p->m_vAxisOff = vAxisOff;
		}
	}
}

inline void A3DParticleSystemEx::ApplyMotion(A3DParticle* p, float fTimeSpan)
{
	float fDist, fHalfTm = .5f * fTimeSpan;
	float fSelfVelEnd = p->m_fSelfVel + m_pEmitter->GetParAcc() * fTimeSpan;
	fDist = (p->m_fSelfVel + fSelfVelEnd) * fHalfTm;
	p->m_fSelfVel = fSelfVelEnd;
	A3DVECTOR3 vOff = p->m_vMoveDir * fDist;
	p->m_vPos += vOff;
	p->m_vAxisOff += vOff;
	float fVelEnd = p->m_fVelAlongAcc + m_pEmitter->GetAcc() * fTimeSpan;
	float fDist2 = (fVelEnd + p->m_fVelAlongAcc) * fHalfTm;
	p->m_fVelAlongAcc = fVelEnd;
	vOff = m_pEmitter->GetAccDir() * fDist2;
	p->m_vPos += vOff;
	p->m_vAxisOff += vOff;
}

void A3DParticleSystemEx::TriggerAffectors(float fTimeSpan, DWORD dwTick)
{
	const ParticleList& lst = GetActParticleList();
	ALISTPOSITION pos = lst.GetHeadPosition();

	if (m_Affectors.size())
	{
		PROC_DATA ProcData;
		while (pos)
		{
			A3DParticle* const p = lst.GetNext(pos);
			p->m_fTTL += fTimeSpan;
			A3DVECTOR3 v = p->m_vOldPos - p->m_vPos;
			if (v.Normalize() > 1e-6)
				p->m_vOldMoveDir = v;
			p->m_vOldPos					= p->m_vPos;
			ProcData.m_KPProcess.m_vPos		= p->m_vPos;
			ProcData.m_KPProcess.m_vDir		= p->m_vDir;
			ProcData.m_KPProcess.m_fScale	= p->m_fScale;
			ProcData.m_KPProcess.m_color	= p->m_color;
			ProcData.m_KPProcess.m_fRad2D	= p->m_fRotAngle;
			ProcData.m_vAxisOff				= p->m_vAxisOff;
			ProcData.m_fTimeSpan			= p->m_fTTL;
			ProcData.m_fScaleNoise			= p->m_fScaleNoise;

			for (size_t i = 0; i < m_Affectors.size(); i++)
				m_Affectors[i]->Process(ProcData, fTimeSpan);

			p->m_vPos		= ProcData.m_KPProcess.m_vPos;
			p->m_vDir		= ProcData.m_KPProcess.m_vDir;
			p->m_fScale		= ProcData.m_KPProcess.m_fScale;
			p->m_fScaleNoise= ProcData.m_fScaleNoise;
			p->m_color		= ProcData.m_KPProcess.m_color;
			p->m_fRotAngle	= ProcData.m_KPProcess.m_fRad2D;
			p->m_vAxisOff	= ProcData.m_vAxisOff;

			if (m_bApplyMotion)
				ApplyMotion(p, fTimeSpan);

			if (m_bIsUseParUV)
				ApplyUV(p, dwTick);
		}
	}
	else
	{
		while (pos)
		{
			A3DParticle * const p = lst.GetNext(pos);
			p->m_fTTL += fTimeSpan;
			A3DVECTOR3 v = p->m_vOldPos - p->m_vPos;
			if (v.Normalize() > 1e-6)
				p->m_vOldMoveDir = v;
			p->m_vOldPos = p->m_vPos;

			if (m_bApplyMotion)
				ApplyMotion(p, fTimeSpan);

			if (m_bIsUseParUV)
				ApplyUV(p, dwTick);
		}
	}
}

inline void A3DParticleSystemEx::TickDummy(DWORD dwTick)
{
	if (!m_pDummy)
		return;

	assert(m_pDummy->IsDummyEle());

	KEY_POINT kp;
	if (!GetCurKeyPoint(&kp))
		return;

	A3DMATRIX4 matTran, matPar, matScale;

	if (m_pEmitter->IsBind())
	{
		if (m_bScaleNoOffset)
			matTran = m_matSelfScale * GetTranMatrix(kp) * GetParentTM();
		else
			matTran = GetTranMatrix(kp) * m_matSelfScale * GetParentTM();
	}

	A3DQUATERNION qParent = GetParentDir() * kp.m_vDir;
	float fAlpha = A3DCOLOR_GETALPHA(kp.m_color) * m_pGfx->GetActualAlpha() / 255.f;

	const ParticleList& lst = GetActParticleList();
	ALISTPOSITION pos = lst.GetHeadPosition();

	while (pos)
	{
		A3DParticle* const p = lst.GetNext(pos);
		float fScale = p->m_fScale + p->m_fScaleNoise;

		if (m_bFacing)
		{
			float fAbsScale = fScale * kp.m_fScale * m_pGfx->GetActualScale();
			matScale.Scale(fAbsScale, fAbsScale, fAbsScale);
			A3DVECTOR3 vPosAbs = IsParticlePosAbs() ? p->m_vPos : matTran * p->m_vPos;
			A3DVECTOR3 vMoveDir = p->m_vPos - p->m_vOldPos;

			if (vMoveDir.Normalize() < 1e-6)
				vMoveDir = p->m_vMoveDir;

			if (!IsParticlePosAbs())
				vMoveDir = RotateVec(qParent, vMoveDir);

			matPar = _build_matrix(vMoveDir, vPosAbs);
			matPar = matScale * matPar;
		}
		else
		{
			matScale.Scale(fScale, fScale, fScale);

			if (p->m_fRotAngle)
			{
				A3DQUATERNION q(_unit_z, p->m_fRotAngle);
				q = p->m_vDir * q;
				q.ConvertToMatrix(matPar);
			}
			else
				p->m_vDir.ConvertToMatrix(matPar);

			matPar.SetRow(3, p->m_vPos);
			matPar = matScale * matPar;

			if (m_pEmitter->IsBind())
				matPar = matPar * matTran;
		}

		A3DCOLOR cl = (p->m_color & _cMask) + A3DCOLORRGBA(0, 0, 0, int(A3DCOLOR_GETALPHA(p->m_color) * fAlpha));

		assert(p && p->m_pDummy && p->m_pDummy->IsDummyEle() && p->m_pDummy->GetDummyMatrix());
		// 说明:这里传入的DummyMatrix是已经乘以缩放系数的矩阵
		// 同时又传入了fScale参数
		// 对于模型类型的傀儡，更新模型矩阵时，不考虑dummyScale系数，因此DummyMatrix起了完全的作用
		// 对于A3DGFXContainer类型的傀儡，由于SetParentTM函数会将矩阵中的Scale分量忽略，因此DummyScale系数起作用
		// 按照目前的方式，所有的DummyMatrix都需要乘以缩放矩阵
		p->m_pDummy->SetDummyMatrix(matPar);
		p->m_pDummy->SetDummyColor(cl);
		p->m_pDummy->SetDummyScale(fScale);
		p->m_pDummy->DummyTick(dwTick);
	}
}

// interfaces of A3DGFXElement
bool A3DParticleSystemEx::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	int		nAffectorCount = 0;
	int		nPoolSize = 0;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_nQuota, sizeof(m_nQuota), &dwReadLen);
		pFile->Read(&m_fParticleWidth, sizeof(m_fParticleWidth), &dwReadLen);
		pFile->Read(&m_fParticleHeight, sizeof(m_fParticleHeight), &dwReadLen);
		pFile->Read(&m_b3DParticle, sizeof(m_b3DParticle), &dwReadLen);
		pFile->Read(&m_bFacing, sizeof(m_bFacing), &dwReadLen);
		pFile->Read(&m_bScaleNoOffset, sizeof(m_bScaleNoOffset), &dwReadLen);
		pFile->Read(&m_bNoWidthScale, sizeof(m_bNoWidthScale), &dwReadLen);
		pFile->Read(&m_bNoHeightScale, sizeof(m_bNoHeightScale), &dwReadLen);
		pFile->Read(&m_fOrgPtWidth, sizeof(m_fOrgPtWidth), &dwReadLen);
		pFile->Read(&m_fOrgPtHeight, sizeof(m_fOrgPtHeight), &dwReadLen);
		pFile->Read(&m_bIsUseParUV, sizeof(m_bIsUseParUV), &dwReadLen);
		pFile->Read(&m_bIsStartWithGrndHeight, sizeof(m_bIsStartWithGrndHeight), &dwReadLen);
		pFile->Read(&m_bStopEmitWhenFadeOut, sizeof(m_bStopEmitWhenFadeOut), &dwReadLen);
		pFile->Read(&m_bInitRandom, sizeof(m_bInitRandom), &dwReadLen);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_quota, &m_nQuota);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_width, &m_fParticleWidth);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_height, &m_fParticleHeight);

		int nRead;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_3d, &nRead);
		m_b3DParticle = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_facing, &nRead);
		m_bFacing = (nRead != 0);

		if (dwVersion >= 30)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_scl_no_off, &nRead);
			m_bScaleNoOffset = (nRead != 0);
		}

		if (dwVersion >= 37)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_no_scale, &nRead);
			m_bNoWidthScale = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_no_scale, &nRead);
			m_bNoHeightScale = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_org_pt, &m_fOrgPtWidth);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_org_pt, &m_fOrgPtHeight);
		}

		if (dwVersion >= 72)
		{
			int nRead = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_isuseparuv, &nRead);
			m_bIsUseParUV = nRead != 0;
		}

		if (dwVersion >= 79)
		{
			int nRead = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_isstartonground, &nRead);
			m_bIsStartWithGrndHeight = nRead != 0;
		}

		if (dwVersion >= 92)
		{
			int nRead;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_stopemitwhenfade, &nRead);
			m_bStopEmitWhenFadeOut = (nRead != 0);
		}

		if (dwVersion >= 99)
		{
			int nRead = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_init_random_texture, &nRead);
			m_bInitRandom = (nRead != 0);
		}

		if (dwVersion >= 108)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_zoffset, &m_fZOffset);
		}
	}

	if (m_b3DParticle) m_bTLVert = false;

	if (!m_pEmitter->Load(pFile, dwVersion))
	{
		Release();
		return false;
	}

	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&nAffectorCount, sizeof(nAffectorCount), &dwRead);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_aff_num, &nAffectorCount);
	}

	for (int i = 0; i < nAffectorCount; i++)
	{
		A3DGFXKeyPointCtrlBase* p = A3DGFXKeyPointCtrlBase::LoadFromFile(pFile, dwVersion);
		if (p) 
		{
			if (p->HasAxis()) m_vParAxisOff = p->GetAxisCenter();
			p->Resume();
			m_Affectors.push_back(p);
		}
	}

	m_bApplyMotion = (m_pEmitter->GetSpeed() != 0 || m_pEmitter->GetAcc() != 0 || m_pEmitter->GetParAcc() != 0);

	return true;
}

bool A3DParticleSystemEx::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_nQuota, sizeof(m_nQuota), &dwWrite);
		pFile->Write(&m_fParticleWidth, sizeof(m_fParticleWidth), &dwWrite);
		pFile->Write(&m_fParticleHeight, sizeof(m_fParticleHeight), &dwWrite);
		pFile->Write(&m_b3DParticle, sizeof(m_b3DParticle), &dwWrite);
		pFile->Write(&m_bFacing, sizeof(m_bFacing), &dwWrite);
		pFile->Write(&m_bNoWidthScale, sizeof(m_bNoWidthScale), &dwWrite);
		pFile->Write(&m_bNoHeightScale, sizeof(m_bNoHeightScale), &dwWrite);
		pFile->Write(&m_fOrgPtWidth, sizeof(m_fOrgPtWidth), &dwWrite);
		pFile->Write(&m_fOrgPtHeight, sizeof(m_fOrgPtHeight), &dwWrite);
		pFile->Write(&m_bIsUseParUV, sizeof(m_bIsUseParUV), &dwWrite);
		pFile->Write(&m_bInitRandom, sizeof(m_bInitRandom), & dwWrite);
		m_pEmitter->Save(pFile);
		size_t sz = m_Affectors.size();
		pFile->Write(&sz, sizeof(sz), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_quota, m_nQuota);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_width, m_fParticleWidth);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_height, m_fParticleHeight);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_3d, m_b3DParticle);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_facing, (int)m_bFacing);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_scl_no_off, (int)m_bScaleNoOffset);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_no_scale, (int)m_bNoWidthScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_no_scale, (int)m_bNoHeightScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_org_pt, m_fOrgPtWidth);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_org_pt, m_fOrgPtHeight);
		pFile->WriteLine(szLine);

		// add in version 72
		sprintf(szLine, _format_isuseparuv, m_bIsUseParUV);
		pFile->WriteLine(szLine);

		// add in version 79
		sprintf(szLine, _format_isstartonground, m_bIsStartWithGrndHeight);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_stopemitwhenfade, m_bStopEmitWhenFadeOut);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_init_random_texture, m_bInitRandom);
		pFile->WriteLine(szLine);

		// add in version 108
		sprintf_s(szLine, AFILE_LINEMAXLEN, _format_zoffset, m_fZOffset);
		pFile->WriteLine(szLine);

		m_pEmitter->Save(pFile);
		
		sprintf(szLine, _format_aff_num, m_Affectors.size());
		pFile->WriteLine(szLine);
	}

	for (size_t i = 0; i < m_Affectors.size(); i++)
		m_Affectors[i]->SaveToFile(pFile);

	return true;
}

void A3DParticleSystemEx::Release()
{
	A3DGFXElement::Release();
	ReleasePool();

	ReleaseAffectors();

	delete m_pEmitter;
	m_pEmitter = NULL;
}

void A3DParticleSystemEx::ReleaseAffectors()
{
	for (size_t i = 0; i < m_Affectors.size(); i++)
		delete m_Affectors[i];
	m_Affectors.clear();
}

inline float A3DParticleSystemEx::GetCurAlpha(const KEY_POINT& kp) const
{
	return A3DCOLOR_GETALPHA(kp.m_color) * m_pGfx->GetActualAlpha() / 255.f;
}

int A3DParticleSystemEx::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return 0;

	int nCount;

	if (m_b3DParticle)
		nCount = Update_3D(kp, pView, (A3DGFXVERTEX*)pBuffer, nMatrixIndex);
	else
		nCount = Update_2D(kp, pView, (A3DTLVERTEX*)pBuffer);

	_gfx_par_render_count += nCount / 4;
	return nCount;
}

inline void A3DParticleSystemEx::Fill_Vert_3D(
	const A3DParticle* p,
	const KEY_POINT& kp,
	const A3DVECTOR3& vCam,
	A3DVECTOR3* verts,
	const A3DMATRIX4& matTran,
	const A3DQUATERNION& qParent,
	float fParentScale) const
{
	float fScale = p->m_fScale + p->m_fScaleNoise;
	float fWidth = m_bNoWidthScale ? p->m_fWidth : p->m_fWidth * fScale;
	float fHeight = m_bNoHeightScale ? p->m_fHeight : p->m_fHeight * fScale;

	if (m_bFacing)
	{
		fScale = kp.m_fScale * fParentScale;
		fWidth	*= fScale;
		fHeight	*= fScale;
		A3DVECTOR3 vPosAbs;

		if (!IsParticlePosAbs())
			vPosAbs = matTran * p->m_vPos;
		else if (m_pEmitter->IsDrag())
		{
			float fPortion = p->m_fTTL / p->m_fTotalTTL;
			fPortion = powf(fPortion, m_pEmitter->GetDragIntensity());
			vPosAbs = p->m_vPos + (1.0f - fPortion) * (m_vOldPos - p->m_vOrgOffset);
		}
		else
			vPosAbs = p->m_vPos;

		A3DVECTOR3 vView = vPosAbs - vCam;
		A3DVECTOR3 vMoveDir = p->m_vOldPos - p->m_vPos;

		if (vMoveDir.Normalize() < 1e-6)
			vMoveDir = p->m_vOldMoveDir;		

		if (!IsParticlePosAbs())
			vMoveDir = RotateVec(qParent, vMoveDir);

		vMoveDir *= fWidth;

		A3DVECTOR3 vUp = CrossProduct(vView, vMoveDir);
		vUp = Normalize(vUp) * fHeight;

		vPosAbs += vMoveDir * m_fOrgPtWidCo + vUp * m_fOrgPtHeiCo;
		A3DVECTOR3 v1 = vMoveDir + vUp;
		A3DVECTOR3 v2 = vMoveDir - vUp;

		verts[0]	= vPosAbs - v2;
		verts[1]	= vPosAbs + v1;
		verts[2]	= vPosAbs - v1;
		verts[3]	= vPosAbs + v2;
	}
	else
	{
		A3DVECTOR3 vAxisX, vAxisY;

		if (p->m_fRotAngle)
		{
			A3DQUATERNION q(_unit_z, p->m_fRotAngle);
			q = p->m_vDir * q;
			vAxisX = RotateVec(q, _unit_x);
			vAxisY = RotateVec(q, _unit_y);
		}
		else
		{
			vAxisX = RotateVec(p->m_vDir, _unit_x);
			vAxisY = RotateVec(p->m_vDir, _unit_y);
		}

		if (!m_pEmitter->IsBind())
		{
			fWidth *= fParentScale;
			fHeight *= fParentScale;
		}

		vAxisX *= fWidth;
		vAxisY *= fHeight;

		A3DVECTOR3 v1 = vAxisX + vAxisY;
		A3DVECTOR3 v2 = vAxisX - vAxisY;
		A3DVECTOR3 vPos;

		if (m_pEmitter->IsDrag() && !m_pEmitter->IsBind())
		{
			float fPortion = p->m_fTTL / p->m_fTotalTTL;
			fPortion = powf(fPortion, m_pEmitter->GetDragIntensity());
			vPos = p->m_vPos + (1.0f - fPortion) * (m_vOldPos - p->m_vOrgOffset);
		}
		else
			vPos = p->m_vPos;

		A3DVECTOR3 vCenter = vPos + vAxisX * m_fOrgPtWidCo + vAxisY * m_fOrgPtHeiCo;

		verts[0]	= vCenter - v2;
		verts[1]	= vCenter + v1;
		verts[2]	= vCenter - v1;
		verts[3]	= vCenter + v2;
	}
}

inline A3DCOLOR bind_color(A3DCOLOR cl, float fAlpha)
{
	return (cl & _cMask) + A3DCOLORRGBA(0, 0, 0, int(A3DCOLOR_GETALPHA(cl) * fAlpha));
}

#define FILL_LVERT(i) \
	pVert->x			= verts[i].x;\
	pVert->y			= verts[i].y;\
	pVert->z			= verts[i].z;\
	pVert->dwMatIndices	= nMatrixIndex;\
	pVert->tu			= fU[i];\
	pVert->tv			= fV[i];\
	pVert->diffuse		= cl;\
	pVert->specular		= spec;

#define CALC_AABB_3D(w) \
{\
	A3DAABB& aabb = m_AABB; m_pGfx->GetAABBOrgRef();\
	\
	A3DVECTOR3 v(pVert->x, pVert->y, pVert->z);\
	v = w * v;\
	if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;\
	if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;\
	if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;\
	if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;\
	if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;\
	if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;\
}

int A3DParticleSystemEx::Update_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex)
{
	static const A3DCOLOR spec = A3DCOLORRGBA(0, 0, 0, 255);
	DWORD dwVerts = m_LstAct.GetCount() * 4;

	float fAlpha = GetCurAlpha(kp);
	A3DMATRIX4 matTran;

	if (m_pEmitter->IsBind())
		matTran = GetBindTranMat(kp);

	A3DQUATERNION qParent = GetParentDir() * kp.m_vDir;
	float fParentScale = m_pGfx->GetActualScale();
	const A3DVECTOR3& vCam = pView->GetCamera()->GetPos();
	A3DVECTOR3 verts[4];
	A3DMATRIX4 matWorld = IsTranInLocal() ? matTran : _identity_mat4;

	if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
		m_pDevice->SetWorldMatrix(matWorld);
	else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
	{
		matWorld.Transpose();
		m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
	}
	else
		m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);

	int nCount = 0;
	ALISTPOSITION pos = m_LstAct.GetHeadPosition();
	bool bOuterColor = m_pGfx->IsUsingOuterColor();
	const A3DCOLORVALUE& val = m_pGfx->GetOuterColor();

#ifdef GFX_EDITOR
	m_AABB.Clear(); 
#endif

	float fU[] =
	{
		m_fTexU,
		m_fTexU,
		m_fTexU + m_fTexWid,
		m_fTexU + m_fTexWid
	};

	float fV[] =
	{
		m_fTexV + m_fTexHei,
		m_fTexV,
		m_fTexV + m_fTexHei,
		m_fTexV
	};

	if (m_bUReverse)
		gfx_u_reverse(fU);

	if (m_bVReverse)
		gfx_v_reverse(fV);

	if (m_bUVInterchange)
		gfx_uv_interchange(fU, fV);

	while(pos)
	{
		const A3DParticle* const p = m_LstAct.GetNext(pos);

		// if use particle's UV, reassign the UV coords
		if (m_bIsUseParUV || m_bInitRandom)
		{
			fU[0] = p->m_fTexU;
			fU[1] = p->m_fTexU;
			fU[2] = p->m_fTexU + m_fTexWid;
			fU[3] = p->m_fTexU + m_fTexWid;

			fV[0] = p->m_fTexV + m_fTexHei;
			fV[1] = p->m_fTexV;
			fV[2] = p->m_fTexV + m_fTexHei;
			fV[3] = p->m_fTexV;
			
			if (m_bUReverse)
				gfx_u_reverse(fU);

			if (m_bVReverse)
				gfx_v_reverse(fV);

			if (m_bUVInterchange)
				gfx_uv_interchange(fU, fV);
		}

		A3DCOLOR clPar;

		if (bOuterColor)
			clPar = (A3DCOLORVALUE(p->m_color) * val).ToRGBAColor();
		else
			clPar = p->m_color;

		A3DCOLOR cl = A3DCOLOR(A3DCOLOR_GETALPHA(clPar) * fAlpha);

		if (cl < 5)
			continue;

		Fill_Vert_3D(p, kp, vCam, verts, matTran, qParent, fParentScale);

		cl = (clPar & _cMask) | A3DCOLORRGBA(0, 0, 0, cl);
		A3DGFXVERTEX* pVert = &pVerts[nCount++ * 4];

#ifdef GFX_EDITOR
		A3DMATRIX4 matWorld = IsTranInLocal() ? matTran : _identity_mat4;
#endif

		FILL_LVERT(0);
#ifdef GFX_EDITOR
		CALC_AABB_3D(matWorld)
#endif
			pVert++;

		FILL_LVERT(1);
#ifdef GFX_EDITOR
		CALC_AABB_3D(matWorld)
#endif
			pVert++;

		FILL_LVERT(2);
#ifdef GFX_EDITOR
		CALC_AABB_3D(matWorld)
#endif
			pVert++;

		FILL_LVERT(3);
#ifdef GFX_EDITOR
		CALC_AABB_3D(matWorld)
#endif
	}

	return nCount * 4;
}

bool A3DParticleSystemEx::Fill_Vert_2D(
	const A3DParticle* p,
	const KEY_POINT& kp,
	A3DViewport* pView,
	A3DCameraBase* pCamera,
	A3DVECTOR4* verts,
	const A3DMATRIX4& matTran,
	const A3DMATRIX4& matVPS,
	float fParentScale,
	float& fWidth,
	float& fHeight) const
{
	A3DVECTOR3 vCenterWorld;

	if (m_pEmitter->IsBind())
		vCenterWorld = matTran * p->m_vPos;
	else if (m_pEmitter->IsDrag())
	{
		float fPortion = p->m_fTTL / p->m_fTotalTTL;
		fPortion = powf(fPortion, m_pEmitter->GetDragIntensity());
		vCenterWorld = p->m_vPos + (1.0f - fPortion) * (m_vOldPos - p->m_vOrgOffset);
	}
	else
		vCenterWorld = p->m_vPos;

	A3DVECTOR4 vCenter = TransformToScreen(vCenterWorld, matVPS);

	if (vCenter.z <= 0 || vCenter.z >= 1.0f)
		return false;

	if (m_fZOffset != 0.0f)
	{
		A3DVECTOR3 vZCenter = vCenterWorld - pView->GetCamera()->GetDir() * (m_fZOffset * m_pGfx->GetActualScale());
		vZCenter = matVPS * vZCenter;
		vCenter.z = vZCenter.z;
		a_Clamp(vCenter.z, 0.0f, 1.0f);
	}

	float fScale = (p->m_fScale + p->m_fScaleNoise) * fParentScale * kp.m_fScale;
	fWidth = m_bNoWidthScale ? p->m_fWidth : p->m_fWidth * fScale;
	fHeight = m_bNoHeightScale ? p->m_fHeight : p->m_fHeight * fScale;

	A3DVECTOR3 vExts = vCenterWorld + pCamera->GetRight() * fWidth + pCamera->GetUp() * fHeight;
	vExts = matVPS * vExts;
	fWidth	= vExts.x - vCenter.x;
	fHeight	= vCenter.y - vExts.y;

	if (p->m_fRotAngle)
	{
		float fAngle = pView->GetCamera()->IsMirrored() ? -p->m_fRotAngle : p->m_fRotAngle;
		float fSin = (float)sin(fAngle);
		float fCos = (float)cos(fAngle);

		float fDeltaX = fWidth * m_fOrgPtWidCo;
		float fDeltaY = fHeight * m_fOrgPtHeiCo;

		vCenter.x += fDeltaX * fCos + fDeltaY * fSin;
		vCenter.y += fDeltaY * fCos - fDeltaX * fSin;

		verts->x = vCenter.x - fWidth  * fCos + fHeight * fSin;
		verts->y = vCenter.y + fHeight * fCos + fWidth  * fSin;
		verts->z = vCenter.z;
		verts->w = vCenter.w;
		verts++;

		verts->x = vCenter.x - fWidth  * fCos - fHeight * fSin;
		verts->y = vCenter.y - fHeight * fCos + fWidth  * fSin;
		verts->z = vCenter.z;
		verts->w = vCenter.w;
		verts++;

		verts->x = vCenter.x + fWidth  * fCos + fHeight * fSin;
		verts->y = vCenter.y + fHeight * fCos - fWidth  * fSin;
		verts->z = vCenter.z;
		verts->w = vCenter.w;
		verts++;

		verts->x = vCenter.x + fWidth  * fCos - fHeight * fSin;
		verts->y = vCenter.y - fHeight * fCos - fWidth  * fSin;
		verts->z = vCenter.z;
		verts->w = vCenter.w;
	}
	else
	{
		vCenter.x += fWidth * m_fOrgPtWidCo;
		vCenter.y += fHeight * m_fOrgPtHeiCo;

		verts->x = vCenter.x - fWidth;
		verts->y = vCenter.y + fHeight;
		verts->z = vCenter.z;
		verts->w = vCenter.w;
		verts++;

		verts->x = vCenter.x - fWidth;
		verts->y = vCenter.y - fHeight;
		verts->z = vCenter.z;
		verts->w = vCenter.w;
		verts++;

		verts->x = vCenter.x + fWidth;
		verts->y = vCenter.y + fHeight;
		verts->z = vCenter.z;
		verts->w = vCenter.w;
		verts++;

		verts->x = vCenter.x + fWidth;
		verts->y = vCenter.y - fHeight;
		verts->z = vCenter.z;
		verts->w = vCenter.w;
	}

	return true;
}

#define FILL_TLVERT(i) \
	pVert->x		= verts[i].x;\
	pVert->y		= verts[i].y;\
	pVert->z		= verts[i].z;\
	pVert->rhw		= verts[i].w;\
	pVert->tu		= fU[mirror_id[i]];\
	pVert->tv		= fV[mirror_id[i]];\
	pVert->diffuse	= cl;\
	pVert->specular	= spec;

#define CALC_AABB_2D \
	{\
		A3DVECTOR3 v(pVert->x, pVert->y, pVert->z);\
		pView->InvTransform(v, v);\
\
		A3DAABB& aabb = m_AABB; m_pGfx->GetAABBOrgRef();\
		if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;\
		if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;\
		if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;\
		if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;\
		if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;\
		if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;\
	}

// 如果是mirror的camera,则把v交换
inline void _map_id(int index[4], bool bMirror)
{
	if (bMirror)
	{
		index[0] = 1;
		index[1] = 0;
		index[2] = 3;
		index[3] = 2;
	}
	else
	{
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 3;
	}
}

int A3DParticleSystemEx::Update_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts)
{
	static const A3DCOLOR spec = A3DCOLORRGBA(0, 0, 0, 255);
	DWORD dwVerts = m_LstAct.GetCount() * 4;

	float fAlpha = GetCurAlpha(kp);
	A3DMATRIX4 matTran;

	if (m_pEmitter->IsBind())
		matTran = GetBindTranMat(kp);

	float fParentScale = m_pGfx->GetActualScale();
	A3DCameraBase* pCamera = pView->GetCamera();
	A3DVECTOR4 verts[4];
	float fWidth, fHeight;

	int nCount = 0;
	ALISTPOSITION pos = m_LstAct.GetHeadPosition();

	bool bOuterColor = m_pGfx->IsUsingOuterColor();
	const A3DCOLORVALUE& val = m_pGfx->GetOuterColor();

#ifdef GFX_EDITOR
	m_AABB.Clear(); 
#endif

	float fU[] =
	{
		m_fTexU,
		m_fTexU,
		m_fTexU + m_fTexWid,
		m_fTexU + m_fTexWid
	};

	float fV[] =
	{
		m_fTexV + m_fTexHei,
		m_fTexV,
		m_fTexV + m_fTexHei,
		m_fTexV
	};

	if (m_bUReverse)
		gfx_u_reverse(fU);

	if (m_bVReverse)
		gfx_v_reverse(fV);

	if (m_bUVInterchange)
		gfx_uv_interchange(fU, fV);

	int mirror_id[4];
	_map_id(mirror_id, pView->GetCamera()->IsMirrored());
	A3DMATRIX4 matVPS = pView->GetCamera()->GetVPTM() * pView->GetViewScale();

	while (pos)
	{
		const A3DParticle* const p = m_LstAct.GetNext(pos);

		// if use particle's UV, reassign the UV coords
		if (m_bIsUseParUV || m_bInitRandom)
		{
			fU[0] = p->m_fTexU;
			fU[1] = p->m_fTexU;
			fU[2] = p->m_fTexU + m_fTexWid;
			fU[3] = p->m_fTexU + m_fTexWid;

			fV[0] = p->m_fTexV + m_fTexHei;
			fV[1] = p->m_fTexV;
			fV[2] = p->m_fTexV + m_fTexHei;
			fV[3] = p->m_fTexV;
			
			if (m_bUReverse)
				gfx_u_reverse(fU);

			if (m_bVReverse)
				gfx_v_reverse(fV);

			if (m_bUVInterchange)
				gfx_uv_interchange(fU, fV);
		}

		A3DCOLOR clPar;

		if (bOuterColor)
			clPar = (A3DCOLORVALUE(p->m_color) * val).ToRGBAColor();
		else
			clPar = p->m_color;

		A3DCOLOR cl = A3DCOLOR(A3DCOLOR_GETALPHA(clPar) * fAlpha);

		if (cl < 5)
			continue;

		if (!Fill_Vert_2D(p, kp, pView, pCamera, verts, matTran, matVPS, fParentScale, fWidth, fHeight))
			continue;

		cl = (clPar & _cMask) | A3DCOLORRGBA(0, 0, 0, cl);
		A3DTLVERTEX* pVert = &pVerts[nCount++ * 4];

		FILL_TLVERT(0);
#ifdef GFX_EDITOR
		CALC_AABB_2D
#endif
		pVert++;

		FILL_TLVERT(1);
#ifdef GFX_EDITOR
		CALC_AABB_2D
#endif
		pVert++;

		FILL_TLVERT(2);
#ifdef GFX_EDITOR
		CALC_AABB_2D
#endif
		pVert++;

		FILL_TLVERT(3);
#ifdef GFX_EDITOR
		CALC_AABB_2D
#endif
	}

	return nCount * 4;
}

#define FILL_WARP_VERT(i) \
	pVert->pos		= verts[i];\
	pVert->u1		= fU[i];\
	pVert->v1		= fV[i];\
	pVert->u3		= fParAlpha;\
	pVert->v3		= fParAlpha;\
	pVert++;

int A3DParticleSystemEx::DrawToBuffer(
	A3DViewport* pView,
	A3DWARPVERTEX* pVerts,
	int nMaxVerts)
{
	if (GetVertsCount() > nMaxVerts)
		return 0;

	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return 0;

	float fU[] =
	{
		m_fTexU,
		m_fTexU,
		m_fTexU + m_fTexWid,
		m_fTexU + m_fTexWid
	};

	float fV[] =
	{
		m_fTexV + m_fTexHei,
		m_fTexV,
		m_fTexV + m_fTexHei,
		m_fTexV
	};

	if (m_bUReverse)
		gfx_u_reverse(fU);

	if (m_bVReverse)
		gfx_v_reverse(fV);

	if (m_bUVInterchange)
		gfx_uv_interchange(fU, fV);

	float fAlpha = GetCurAlpha(kp);
	A3DMATRIX4 matTran;

	if (m_pEmitter->IsBind())
		matTran = GetBindTranMat(kp);

	A3DQUATERNION qParent = GetParentDir() * kp.m_vDir;
	float fParentScale = m_pGfx->GetActualScale();
	const A3DVECTOR3& vCam = pView->GetCamera()->GetPos();
	A3DVECTOR3 verts[4];
	int nCount = 0, nTotalVerts = 0;
	ALISTPOSITION pos = m_LstAct.GetHeadPosition();

	while(pos)
	{
		const A3DParticle* const p = m_LstAct.GetNext(pos);
		Fill_Vert_3D(p, kp, vCam, verts, matTran, qParent, fParentScale);

		if (IsTranInLocal())
		{
			verts[0] = matTran * verts[0];
			verts[1] = matTran * verts[1];
			verts[2] = matTran * verts[2];
			verts[3] = matTran * verts[3];
		}

		A3DWARPVERTEX* pVert = &pVerts[nCount++ * 4];
		float fParAlpha = A3DCOLOR_GETALPHA(p->m_color) * fAlpha / 255.f;

		FILL_WARP_VERT(0)
		FILL_WARP_VERT(1)
		FILL_WARP_VERT(2)
		FILL_WARP_VERT(3)

		nTotalVerts = nCount * 4;

		if (nTotalVerts >= nMaxVerts)
			break;
	}

	return nTotalVerts;
}

#define FILL_TLWARP_VERT(i) \
	pVert->pos		= verts[i];\
	pVert->pos.w	= 1.0f;\
	pVert->u1		= fU[i];\
	pVert->v1		= fV[i];\
	pVert->u2		= pVert->pos.x * rw;\
	pVert->v2		= pVert->pos.y * rh;\
	pVert->u3		= fWidth * rw * fParAlpha * fInvAlpha;\
	a_Clamp(pVert->u3, 0.0f, 0.5f);\
	pVert->v3		= fHeight * rh * fParAlpha * fInvAlpha;\
	a_Clamp(pVert->v3, 0.0f, 0.5f);\
	pVert++;

int A3DParticleSystemEx::DrawToBuffer(
	A3DViewport* pView,
	A3DTLWARPVERTEX* pVerts,
	int nMaxVerts,
	float rw,
	float rh)
{
	if (GetVertsCount() > nMaxVerts)
		return 0;

	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return 0;

	float fU[] =
	{
		m_fTexU,
		m_fTexU,
		m_fTexU + m_fTexWid,
		m_fTexU + m_fTexWid
	};

	float fV[] =
	{
		m_fTexV + m_fTexHei,
		m_fTexV,
		m_fTexV + m_fTexHei,
		m_fTexV
	};

	if (m_bUReverse)
		gfx_u_reverse(fU);

	if (m_bVReverse)
		gfx_v_reverse(fV);

	if (m_bUVInterchange)
		gfx_uv_interchange(fU, fV);

	float fAlpha = GetCurAlpha(kp);
	A3DMATRIX4 matTran;

	if (m_pEmitter->IsBind())
		matTran = GetBindTranMat(kp);

	float fParentScale = m_pGfx->GetActualScale();
	A3DCameraBase* pCamera = pView->GetCamera();
	A3DMATRIX4 matVPS = pCamera->GetVPTM() * pView->GetViewScale();

	A3DVECTOR4 verts[4];
	float fWidth, fHeight;
	int nCount = 0, nTotalVerts = 0;
	ALISTPOSITION pos = m_LstAct.GetHeadPosition();

	while(pos)
	{
		const A3DParticle* const p = m_LstAct.GetNext(pos);

		if (!Fill_Vert_2D(p, kp, pView, pCamera, verts, matTran, matVPS, fParentScale, fWidth, fHeight))
			continue;

		float fInvAlpha = min(1.0f - fWidth * fWidth * rw * rw * 4.0f, 1.0f - fHeight * fHeight * rh * rh * 4.0f);
		a_Clamp(fInvAlpha, 0.0f, 1.0f);

		if (fInvAlpha < 0.1f)
			continue;

		A3DTLWARPVERTEX* pVert = &pVerts[nCount++ * 4];
		float fParAlpha = A3DCOLOR_GETALPHA(p->m_color) * fAlpha / 255.f;

		FILL_TLWARP_VERT(0)
		FILL_TLWARP_VERT(1)
		FILL_TLWARP_VERT(2)
		FILL_TLWARP_VERT(3)

		nTotalVerts = nCount * 4;
		if (nTotalVerts >= nMaxVerts) break;
	}

	return nTotalVerts;
}

void A3DParticleSystemEx::Render_Dummy(A3DViewport* pView)
{
#ifndef GFX_EDITOR

	ALISTPOSITION pos = m_LstAct.GetHeadPosition();
	while (pos) m_LstAct.GetNext(pos)->m_pDummy->Render(pView);

#else

	ALISTPOSITION pos = m_LstAct.GetHeadPosition();

	m_AABB.Clear();
	while (pos)
	{
		A3DGFXElement* pDummy = m_LstAct.GetNext(pos)->m_pDummy;
		pDummy->Render(pView);

		{
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

bool A3DParticleSystemEx::Render(A3DViewport* pView)
{
	if (!IsInit()) return true;

	int nParticleNum = m_LstAct.GetCount();
	if (nParticleNum == 0) return true;

	if (m_bWarp)
	{
		AfxGetGFXExMan()->AddWarpEle(this);
		return true;
	}

	if (m_pDummy)
		Render_Dummy(pView);
	else if (IsActive() && m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

bool A3DParticleSystemEx::TickAnimation(DWORD dwTickTime)
{
#ifdef GFX_EDITOR
	_gfx_par_active_count += GetActParticleList().GetCount();
#endif

	const A3DMATRIX4& matParent = GetParentTM();
	float fParentScale = m_pGfx->GetActualScale();
	float fTimeSpan = dwTickTime * 0.001f;
	Expire(fTimeSpan);

	if (!A3DGFXElement::TickAnimation(dwTickTime))
		return false;

	//	Do something on tick
	m_pEmitter->OnTick(this);

	KEY_POINT kp;
	GetCurKeyPoint(&kp);

	m_pEmitter->UpdateEmitterAlpha(A3DCOLOR_GETALPHA(kp.m_color) / 255.f);
	m_matSelfScale.Scale(kp.m_fScale, kp.m_fScale, kp.m_fScale);

	if (m_pEmitter->IsBind())
	{
		m_pEmitter->UpdateEmitterScale(kp.m_fScale * fParentScale);

		if (m_bTriggerEmitter)
			TriggerEmitter(fTimeSpan);

		TriggerAffectors(fTimeSpan, dwTickTime);
		TickDummy(dwTickTime);
	}
	else
	{
		A3DVECTOR3 vEmitPos;
		A3DQUATERNION qEmitDir;

		if(m_bScaleNoOffset)
			vEmitPos = matParent * kp.m_vPos;
		else
			vEmitPos = matParent * (m_matSelfScale * kp.m_vPos);

		qEmitDir = GetParentDir() * kp.m_vDir;
		m_pEmitter->SetParentDirAndPos(vEmitPos, qEmitDir);
		m_pEmitter->UpdateEmitterScale(kp.m_fScale * fParentScale);
		
		if (m_bTriggerEmitter)
			TriggerEmitter(fTimeSpan, vEmitPos, qEmitDir);

		TriggerAffectors(fTimeSpan, dwTickTime);
		TickDummy(dwTickTime);
		m_vOldPos		= vEmitPos;
		m_qOldDir		= qEmitDir;
		m_fOldScl		= fParentScale;
		m_bFirstTick	= false;
	}

	return true;
}

bool A3DParticleSystemEx::Stop()
{
	A3DGFXElement::Stop();
	
	// stop the particles' dummies...
	for (size_t i = 0; i < m_Particles.size(); i++)
	{
#ifdef _ANGELICA21

		A3DParticle* pParticle = m_Particles[i];

		if (pParticle && pParticle->m_pDummy)
		{
			pParticle->m_pDummy->Release();
			delete pParticle->m_pDummy;
			pParticle->m_pDummy = NULL;
		}

#else
		if(m_Particles[i] && m_Particles[i]->m_pDummy)
			m_Particles[i]->m_pDummy->Stop();
#endif
	}

	Resume();

	return true;
}


bool A3DParticleSystemEx::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch (nOp)
	{
	case ID_GFXOP_PARSYS_PAR_QUOTA:
		m_nQuota = prop;
		break;
	case ID_GFXOP_PARSYS_PAR_WIDTH:
		m_fParticleWidth = prop;
		break;
	case ID_GFXOP_PARSYS_PAR_HEIGHT:
		m_fParticleHeight = prop;
		break;
	case ID_GFXOP_PARSYS_PAR_3D:
		m_b3DParticle = prop;
		break;
	case ID_GFXOP_PARSYS_PAR_FACING:
		m_bFacing = prop;
		break;
	case ID_GFXOP_PARSYS_SCALE_NO_OFFSET:
		m_bScaleNoOffset = prop;
		break;
	case ID_GFXOP_PARSYS_NO_WID_SCALE:
		m_bNoWidthScale = prop;
		break;
	case ID_GFXOP_PARSYS_NO_HEI_SCALE:
		m_bNoHeightScale = prop;
		break;
	case ID_GFXOP_PARSYS_ORG_PT_WID:
		m_fOrgPtWidth = prop;
		break;
	case ID_GFXOP_PARSYS_ORG_PT_HEI:
		m_fOrgPtHeight = prop;
		break;
	case ID_GFXOP_PARSYS_USE_PAR_UV:
		m_bIsUseParUV = prop;
		break;
	case ID_GFXOP_PARSYS_USE_GRND_HEIGHT:
		m_bIsStartWithGrndHeight = prop;
		break;
	case ID_GFXOP_PARSYS_STOP_EMIT_WHEN_FADE:
		m_bStopEmitWhenFadeOut = prop;
		break;
	case ID_GFXOP_PARSYS_INIT_RANDOM:
		m_bInitRandom = prop;
		break;
	case ID_GFXOP_PARSYS_Z_OFFSET:
		m_fZOffset = prop;
		break;
	default:
		if (A3DGFXElement::SetProperty(nOp, prop))
			return true;
		if (!m_pEmitter->SetProperty(nOp, prop))
			return false;
	}
	
	m_bApplyMotion = (m_pEmitter->GetSpeed() != 0 || m_pEmitter->GetAcc() != 0 || m_pEmitter->GetParAcc() != 0);
	return true;
}

GFX_PROPERTY A3DParticleSystemEx::GetProperty(int nOp) const
{
	switch (nOp)
	{
	case ID_GFXOP_PARSYS_PAR_QUOTA:
		return GFX_PROPERTY(m_nQuota);
	case ID_GFXOP_PARSYS_PAR_WIDTH:
		return GFX_PROPERTY(m_fParticleWidth);
	case ID_GFXOP_PARSYS_PAR_HEIGHT:
		return GFX_PROPERTY(m_fParticleHeight);
	case ID_GFXOP_PARSYS_PAR_3D:
		return GFX_PROPERTY(m_b3DParticle);
	case ID_GFXOP_PARSYS_PAR_FACING:
		return GFX_PROPERTY(m_bFacing);
	case ID_GFXOP_PARSYS_SCALE_NO_OFFSET:
		return GFX_PROPERTY(m_bScaleNoOffset);
	case ID_GFXOP_PARSYS_NO_WID_SCALE:
		return GFX_PROPERTY(m_bNoWidthScale);
	case ID_GFXOP_PARSYS_NO_HEI_SCALE:
		return GFX_PROPERTY(m_bNoHeightScale);
	case ID_GFXOP_PARSYS_ORG_PT_WID:
		return GFX_PROPERTY(m_fOrgPtWidth);
	case ID_GFXOP_PARSYS_ORG_PT_HEI:
		return GFX_PROPERTY(m_fOrgPtHeight);
	case ID_GFXOP_PARSYS_USE_PAR_UV:
		return GFX_PROPERTY(m_bIsUseParUV);
	case ID_GFXOP_PARSYS_USE_GRND_HEIGHT:
		return GFX_PROPERTY(m_bIsStartWithGrndHeight);
	case ID_GFXOP_PARSYS_STOP_EMIT_WHEN_FADE:
		return GFX_PROPERTY(m_bStopEmitWhenFadeOut);
	case ID_GFXOP_PARSYS_INIT_RANDOM:
		return GFX_PROPERTY(m_bInitRandom);
	case ID_GFXOP_PARSYS_Z_OFFSET:
		return GFX_PROPERTY(m_fZOffset);
	}

	GFX_PROPERTY gp = A3DGFXElement::GetProperty(nOp);
	if (gp.GetType() == GFX_VALUE_UNKNOWN)
		return m_pEmitter->GetProperty(nOp);
	return gp;
}



size_t A3DParticleSystemEx::GetSubProtertyCount(int nSubId)
{
	PropIdxArray propByType;
	return GetCtrlListByType(nSubId, propByType).size();
}

bool A3DParticleSystemEx::AddSubProterty(int nSubId)
{
	if (m_Affectors.size() >= 10)
		return false;
	
	A3DGFXKeyPointCtrlBase* p = A3DGFXKeyPointCtrlBase::CreateKPCtrl(nSubId);
	if (p == NULL)
		return false;

	m_Affectors.push_back(p);
	return true;
}

bool A3DParticleSystemEx::RemoveSubProterty(int nSubId, size_t nSubIdx)
{
	PropIdxArray propByType;
	GetCtrlListByType(nSubId, propByType);
	if (nSubIdx >= propByType.size())
		return false;

	delete m_Affectors[propByType[nSubIdx]];
	m_Affectors.erase(m_Affectors.begin() + propByType[nSubIdx]);
	return true;
}

bool A3DParticleSystemEx::SetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId, const GFX_PROPERTY& prop)
{
	PropIdxArray propByType;
	GetCtrlListByType(nSubId, propByType);
	if (nSubIdx >= propByType.size())
		return false;

	return m_Affectors[propByType[nSubIdx]]->SetProperty(nSubOpId, prop);
}

GFX_PROPERTY A3DParticleSystemEx::GetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId) const
{
	PropIdxArray propByType;
	GetCtrlListByType(nSubId, propByType);
	if (nSubIdx >= propByType.size())
		return GFX_PROPERTY();
	
	return m_Affectors[propByType[nSubIdx]]->GetProperty(nSubOpId);
}

const A3DParticleSystemEx::PropIdxArray& A3DParticleSystemEx::GetCtrlListByType(int nType, PropIdxArray& lst) const
{
	lst.clear();
	for (size_t i = 0; i < m_Affectors.size(); i++)
		if (m_Affectors[i]->GetType() == nType)
			lst.push_back(i);
	return lst;
}