#include "StdAfx.h"
#include "A3DGFXContainer.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"

static const char* _format_gfx_path = "GfxPath: %s";
static const char* _format_out_color = "OutColor: %d";
static const char* _format_loop_flag = "LoopFlag: %d";
static const char* _format_gfx_playspeed = "PlaySpeed: %f";
static const char* _format_dummy_use_gfx_scale = "DummyUseGScale: %d";

A3DGFXContainer::A3DGFXContainer(A3DGFXEx* pGfx) : A3DGFXElement(pGfx)
{
	m_nEleType = ID_ELE_TYPE_GFX_CONTAINER;
	m_pBindGfx = 0;
	m_fPlaySpeed = 1.0f;
	m_bOuterColor = false;
	m_bLoopFlag = false;
	m_bStart = false;
	m_bDummyUseGfxScale = true;
}

bool A3DGFXContainer::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch (nOp)
	{
	case ID_GFXOP_CONTAINER_GFX_PATH:

		m_strGfxPath = prop;

		if (m_pBindGfx)
		{
			m_pBindGfx->Release();
			delete m_pBindGfx;
			m_pBindGfx = 0;
		}

		SetInit(false);
		break;

	case ID_GFXOP_CONTAINER_OUT_COLOR:

		m_bOuterColor = prop;
		break;

	case ID_GFXOP_CONTAINER_LOOP_FLAG:

		m_bLoopFlag = prop;
		break;

	case ID_GFXOP_CONTAINER_GFX_PLAYSPEED:

		m_fPlaySpeed = prop;
		
		if (m_pBindGfx)
		{
			m_pBindGfx->SetPlaySpeed(m_fPlaySpeed);
		}
		break;
	
	case ID_GFXOP_CONTAINER_USE_GFX_SCALE_WHENDUMMY:

		m_bDummyUseGfxScale = prop;

		break;

	default:
		return A3DGFXElement::SetProperty(nOp, prop);
	}

	return true;
}

GFX_PROPERTY A3DGFXContainer::GetProperty(int nOp) const
{
	switch (nOp)
	{
	case ID_GFXOP_CONTAINER_GFX_PATH:
		return GFX_PROPERTY(m_strGfxPath, GFX_VALUE_PATH_GFX);
	case ID_GFXOP_CONTAINER_OUT_COLOR:
		return GFX_PROPERTY(m_bOuterColor);
	case ID_GFXOP_CONTAINER_LOOP_FLAG:
		return GFX_PROPERTY(m_bLoopFlag);
	case ID_GFXOP_CONTAINER_GFX_PLAYSPEED:
		return GFX_PROPERTY(m_fPlaySpeed);
	case ID_GFXOP_CONTAINER_USE_GFX_SCALE_WHENDUMMY:
		return GFX_PROPERTY(m_bDummyUseGfxScale);
	}

	return A3DGFXElement::GetProperty(nOp);
}

A3DGFXElement* A3DGFXContainer::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXContainer* p = new A3DGFXContainer(pGfx);
	return &(*p = *this);
}

A3DGFXContainer& A3DGFXContainer::operator = (const A3DGFXContainer& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	m_strGfxPath = src.m_strGfxPath;
	m_bOuterColor = src.m_bOuterColor;
	m_bLoopFlag = src.m_bLoopFlag;
	m_fPlaySpeed = src.m_fPlaySpeed;
	m_bDummyUseGfxScale = src.m_bDummyUseGfxScale;
	return *this;
}

bool A3DGFXContainer::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	char	szBuf[MAX_PATH];
	DWORD	dwReadLen;

	if (pFile->IsBinary())
		pFile->ReadString(m_strGfxPath);
	else
	{
		szBuf[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_gfx_path, szBuf);
		m_strGfxPath = szBuf;

		if (dwVersion >= 47)
		{
			int nBool = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_out_color, &nBool);
			m_bOuterColor = (nBool != 0);
		}

		if (dwVersion >= 56)
		{
			int nBool = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_loop_flag, &nBool);
			m_bLoopFlag = (nBool != 0);
		}


		if (dwVersion >= 78)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_gfx_playspeed, &m_fPlaySpeed);
		}

		if (dwVersion >= 94)
		{
			int nBool = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_dummy_use_gfx_scale, &nBool);
			m_bDummyUseGfxScale = (nBool != 0);
		}
	}

	return true;
}

bool A3DGFXContainer::Save(AFile* pFile)
{
	if (pFile->IsBinary())
		pFile->WriteString(m_strGfxPath);
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_gfx_path, m_strGfxPath);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_out_color, m_bOuterColor);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_loop_flag, m_bLoopFlag);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_gfx_playspeed, m_fPlaySpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_dummy_use_gfx_scale, m_bDummyUseGfxScale);
		pFile->WriteLine(szLine);
	}

	return true;
}

void A3DGFXContainer::Release()
{
	if (m_pBindGfx)
	{
		m_pBindGfx->Release();
		delete m_pBindGfx;
		m_pBindGfx = 0;
	}

	SetInit(false);
}

bool A3DGFXContainer::Play()
{
	if (!IsInit())
	{

#ifdef GFX_EDITOR

		m_pBindGfx = AfxGetGFXExMan()->LoadGfx(m_pGfx->GetA3DDevice(), m_strGfxPath, false);

#else
		m_pBindGfx = AfxGetGFXExMan()->LoadGfx(m_pGfx->GetA3DDevice(), m_strGfxPath);

#endif

		if (m_pBindGfx)
		{
			SetInit(true);
			m_pBindGfx->DeriveParentProperty(m_pGfx);
			m_pBindGfx->SetPlaySpeed(m_fPlaySpeed);
			m_pBindGfx->Start(true);
		}
	}
	else if (m_pGfx->IsResourceReady())
	{
		m_pBindGfx->DeriveParentProperty(m_pGfx);
		m_pBindGfx->SetPlaySpeed(m_fPlaySpeed);
		m_pBindGfx->Start(true);
	}

	return true;
}

bool A3DGFXContainer::Pause()
{
	A3DGFXElement::Pause();

	if (!IsInit())
		return false;

	m_pBindGfx->Pause();
	return true;
}

bool A3DGFXContainer::Stop()
{
	A3DGFXElement::Stop();

	if (!IsInit())
		return false;

	m_pBindGfx->Stop();

#ifdef GFX_EDITOR

	Release();

#endif

	return true;
}

bool A3DGFXContainer::StopParticleEmit()
{
	if (m_pBindGfx)
		m_pBindGfx->StopParticleEmit();
	
	return true;
}

void A3DGFXContainer::DoFadeOut()
{
	if (m_pBindGfx)
		m_pBindGfx->SetFadingOut(true);
}

bool A3DGFXContainer::TickAnimation(DWORD dwTickTime)
{
	bool bTickResult = A3DGFXElement::TickAnimation(dwTickTime);

	if (!IsInit())
		return false;
	
	// If self's KeyPoint has already finished or IsInit() is false
	// Return false directly
	// Only let the (m_pBind && !m_pBind->IsActive()) case pass which means m_pBind exist and m_pBind KeyPoint finished
	if (!bTickResult && !(m_pBind && !m_pBind->IsActive()))
		return false;

	KEY_POINT kp;
	GetCurKeyPoint(&kp);
	m_pBindGfx->SetAlpha(A3DCOLOR_GETALPHA(kp.m_color) * m_pGfx->GetActualAlpha() / 255.f);

	// Only If A3DGfxElement::TickAnimation returns true
	// which means Self's KeyPoint has not finished and m_pBind's KeyPoint has not finished either
	// Normally, it means everything goes right. (self and m_pBind's KeyPoint Currently is Not Finished, Inited is true)
	if (bTickResult)
	{
		m_pBindGfx->SetScale(kp.m_fScale * m_pGfx->GetActualScale());
		m_pBindGfx->SetParentTM(GetTranMatrix(kp) * GetParentTM());
		m_pBindGfx->TickAnimation(dwTickTime);
	}

	return true;
}

void A3DGFXContainer::DummyTick(DWORD dwTick)
{
	if (!IsInit())
		return;

	assert(m_pDummyMat);
	m_pBindGfx->SetAlpha(A3DCOLOR_GETALPHA(m_clDummy) / 255.f);
	float fBindGfxScale = m_fDummyScale;
	if (m_bDummyUseGfxScale)
		fBindGfxScale *= m_pGfx->GetActualScale();

	m_pBindGfx->SetScale(fBindGfxScale);
	m_pBindGfx->SetParentTM(*m_pDummyMat);
	m_pBindGfx->TickAnimation(dwTick);
}

bool A3DGFXContainer::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	if (m_bOuterColor)
	{
		if (m_bDummy)
			m_pBindGfx->SetOuterColor(m_clDummy);
		else
		{
			KEY_POINT kp;
			
			if (GetCurKeyPoint(&kp))
				m_pBindGfx->SetOuterColor(kp.m_color);
		}
	}
	else if (m_pGfx->IsUsingOuterColor())
		m_pBindGfx->SetOuterColor(m_pGfx->GetOuterColor());

	m_pBindGfx->Render(pView);

#ifdef GFX_EDITOR

	{
		m_AABB = m_pBindGfx->GetAABB();
	}

#endif

	return true;
}

void A3DGFXContainer::ResumeLoop()
{
	A3DGFXElement::ResumeLoop();
	
	if (m_pBindGfx && m_bLoopFlag)
	{
		// 无论子效果是否已经播完，强制从头播
		m_pBindGfx->Start();
	}
}
