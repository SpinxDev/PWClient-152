#include "StdAfx.h"
#ifdef A3D_PHYSX
#include "A3DPhysPointEmitter.h"
#include "A3DGFXExMan.h"
#include "A3DGFXPhysDataMan.h"
#include <APhysX.h>


static const char _format_is_fade[] = "IsFade: %d";
static const char _format_fadetime[] = "FadeTime: %f";
static const char _format_pointsize[] = "PointSize: %f";
static const char _format_scaleA[] = "ScaleA: %f";
static const char _format_scaleB[] = "ScaleB: %f";
static const char _format_scaleC[] = "ScaleC: %f";

A3DPhysPointEmitter::A3DPhysPointEmitter(A3DGFXEx* pGfx)
: A3DPhysFluidEmitter(pGfx)
{
	m_nEleType = ID_ELE_TYPE_PHYS_POINTEMITTER;

	m_bIsFade = false;
	m_fFadeTime = 1.f;

	m_fPointSize = 1.f;
	m_fScaleA = m_fScaleB = 1.f;
	m_fScaleC = 10.f;
}

A3DPhysPointEmitter::~A3DPhysPointEmitter()
{
}

bool A3DPhysPointEmitter::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	if (!A3DPhysFluidEmitter::Load(pDevice, pFile, dwVersion))
		return false;

	DWORD dwReadLen;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_bIsFade, sizeof(m_bIsFade), &dwReadLen);
		pFile->Read(&m_fFadeTime, sizeof(m_fFadeTime), &dwReadLen);
		pFile->Read(&m_fPointSize, sizeof(m_fPointSize), &dwReadLen);
		pFile->Read(&m_fScaleA, sizeof(m_fScaleA), &dwReadLen);
		pFile->Read(&m_fScaleB, sizeof(m_fScaleB), &dwReadLen);
		pFile->Read(&m_fScaleC, sizeof(m_fScaleC), &dwReadLen);
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		int nIsFade = 0;
		sscanf(szLine, _format_is_fade, &nIsFade);
		m_bIsFade = nIsFade != 0;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_fadetime, &m_fFadeTime);
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_pointsize, &m_fPointSize);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_scaleA, &m_fScaleA);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_scaleB, &m_fScaleB);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_scaleC, &m_fScaleC);
	}

	return true;
}


bool A3DPhysPointEmitter::Save(AFile* pFile)
{
	if (!A3DPhysFluidEmitter::Save(pFile))
		return false;

	DWORD dwWrite;
	if (pFile->IsBinary()) {
		pFile->Write(&m_bIsFade, sizeof(m_bIsFade), &dwWrite);
		pFile->Write(&m_fFadeTime, sizeof(m_fFadeTime), &dwWrite);
		pFile->Write(&m_fPointSize, sizeof(m_fPointSize), &dwWrite);
		pFile->Write(&m_fScaleA, sizeof(m_fScaleA), &dwWrite);
		pFile->Write(&m_fScaleB, sizeof(m_fScaleB), &dwWrite);
		pFile->Write(&m_fScaleC, sizeof(m_fScaleC), &dwWrite);
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_is_fade, m_bIsFade);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_fadetime, m_fFadeTime);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_pointsize, m_fPointSize);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_scaleA, m_fScaleA);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_scaleB, m_fScaleB);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_scaleC, m_fScaleC);
		pFile->WriteLine(szLine);
	}

	return true;
}

A3DGFXElement* A3DPhysPointEmitter::Clone(A3DGFXEx* pGfx) const
{
	A3DPhysPointEmitter* p = new A3DPhysPointEmitter(pGfx);
	return &(*p = *this);
}

A3DPhysPointEmitter& A3DPhysPointEmitter::operator = (const A3DPhysPointEmitter& src)
{
	if (&src == this)
		return *this;

	A3DPhysFluidEmitter::operator =(src);

	m_bIsFade	= src.m_bIsFade;
	m_fFadeTime = src.m_fFadeTime;
	return *this;
}


	// GFX element tick state
bool A3DPhysPointEmitter::TickAnimation(DWORD dwTickTime)
{
	if (!A3DPhysFluidEmitter::TickAnimation(dwTickTime))
		return false;
	
	return true;
}


bool A3DPhysPointEmitter::Play()
{
	A3DPhysFluidEmitter::Play();
	
	if (!IsInit())
	{
		InitBaseData();

		m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
			m_nRenderLayer,
			A3DFVF_POINTVERTEX,
			sizeof(A3DPOINTVERTEX),
			A3DPT_POINTLIST,
			m_strTexture,
			m_Shader,
			m_bTileMode,
			m_bTexNoDownSample,
			m_pGfx->IsZTestEnable() && m_bZEnable,
			HasPixelShader(),
            m_bSoftEdge,
			m_bAbsTexPath);

		SetInit(true);
	}

	return true;
}


bool A3DPhysPointEmitter::Stop()
{
	return A3DPhysFluidEmitter::Stop();
}


bool A3DPhysPointEmitter::Render(A3DViewport*)
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

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}


int A3DPhysPointEmitter::GetVertsCount()
{
	APhysXFluidObjectWrapper fluidObj = GetFluidObject();
	if (!fluidObj.IsValid())
		return 0;

	return fluidObj.GetParticleNum();
}

int A3DPhysPointEmitter::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	if (!gPhysXEngine->IsValid() || !GetPhysObjDescMan() || !GetPhysObjDescMan()->GetPhysScene())
		return 0;

	KEY_POINT kp;
	PhysGetCurKeyPoint(kp);

	if (m_pGfx->IsUsingOuterColor())
		kp.m_color = (A3DCOLORVALUE(kp.m_color) * m_pGfx->GetOuterColor()).ToRGBAColor();

	A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());

	if (A3DCOLOR_GETALPHA(diffuse) < 5)
		return 0;

	APhysXFluidObjectWrapper fluidObj = GetFluidObject();
	if (!fluidObj.IsValid())
		return 0;

	float fLifetime = GetFluidEmitterParam()->mParticleLifetime;
	const int nParticleNum		= fluidObj.GetParticleNum();
	const NxVec3* pPositions	= fluidObj.GetParticlePositions();
	const float* pLifetimes		= fluidObj.GetParticleLives();

	A3DPOINTVERTEX* pVerts = (A3DPOINTVERTEX*)pBuffer;
	int i;
	for (i = 0 ; i < nParticleNum ; ++i) {
		pVerts->x = pPositions[i].x;
		pVerts->y = pPositions[i].y;
		pVerts->z = pPositions[i].z;

		if (m_bIsFade)
		{
			unsigned char ucCurAlpha;
			ucCurAlpha = calcAlphaByTime(A3DCOLOR_GETALPHA(diffuse), pLifetimes[i], fLifetime, m_fFadeTime);
			pVerts->diffuse = A3DCOLORRGBA(0, 0, 0, ucCurAlpha);
			pVerts->diffuse |= (diffuse & 0x00ffffff);
		}
		else
			pVerts->diffuse = diffuse;

		pVerts++;


#ifdef GFX_EDITOR
		A3DVECTOR3 v = APhysXConverter::N2A_Vector3(pPositions[i]);

		A3DAABB& aabb = m_AABB;
		if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;
		if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;
		if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;
		if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;
		if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;
		if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;
#endif
	}

	return nParticleNum;
}

void A3DPhysPointEmitter::GetPointSpriteInfo(PointSpriteInfo* pInfo) const
{
	pInfo->fPointSize = m_fPointSize;
	pInfo->fScaleA = m_fScaleA;
	pInfo->fScaleB = m_fScaleB;
	pInfo->fScaleC = m_fScaleC;
}

bool A3DPhysPointEmitter::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp)
	{
	case ID_GFXOP_PHYSPAR_PPE_ISFADE:
		m_bIsFade = (bool)prop;
		break;
	case ID_GFXOP_PHYSPAR_PPE_FADETIME:
		m_fFadeTime = (float)prop;
		break;
	case ID_GFXOP_PHYSPAR_PPE_POINTSIZE:
		m_fPointSize = (float)prop;
		break;
	case ID_GFXOP_PHYSPAR_PPE_SCALEA:
		m_fScaleA = (float)prop;
		break;
	case ID_GFXOP_PHYSPAR_PPE_SCALEB:
		m_fScaleB = (float)prop;
		break;
	case ID_GFXOP_PHYSPAR_PPE_SCALEC:
		m_fScaleC = (float)prop;
		break;
	default:
		return A3DGFXElement::SetProperty(nOp, prop);
	}
	
	return true;
}

GFX_PROPERTY A3DPhysPointEmitter::GetProperty(int nOp) const
{
	switch(nOp)
	{
	case ID_GFXOP_PHYSPAR_PPE_ISFADE:
		return GFX_PROPERTY(m_bIsFade);
	case ID_GFXOP_PHYSPAR_PPE_FADETIME:
		return GFX_PROPERTY(m_fFadeTime);
	case ID_GFXOP_PHYSPAR_PPE_POINTSIZE:
		return GFX_PROPERTY(m_fPointSize);
	case ID_GFXOP_PHYSPAR_PPE_SCALEA:
		return GFX_PROPERTY(m_fScaleA);
	case ID_GFXOP_PHYSPAR_PPE_SCALEB:
		return GFX_PROPERTY(m_fScaleB);
	case ID_GFXOP_PHYSPAR_PPE_SCALEC:
		return GFX_PROPERTY(m_fScaleC);
	}
	return A3DGFXElement::GetProperty(nOp);
}

#endif