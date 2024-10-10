#include "StdAfx.h"
#include "A3DGFXSound.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "A3DCameraBase.h"
#include "A3DGFXFuncs.h"

void A3DGFXSound::SetupActiveImp()
{
	if (GFX_IsUseAudioEvent())
		m_pActiveImp = &m_SoundImp22;
	else
		m_pActiveImp = &m_SoundImp;
}

A3DGFXSound::A3DGFXSound(A3DGFXEx* pGfx) 
: A3DGFXElement(pGfx)
{
	m_nEleType = ID_ELE_TYPE_SOUND;
	SetupActiveImp();
}

A3DGFXSound::~A3DGFXSound()
{
}

bool A3DGFXSound::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	m_SoundImp.Load(pFile, dwVersion);
	m_SoundImp22.Load(pFile, dwVersion);
	Init(pDevice);
	return true;
}

bool A3DGFXSound::Save(AFile* pFile)
{
	if (!m_SoundImp.Save(pFile))
		return false;

	if (!m_SoundImp22.Save(pFile))
		return false;

	return true;
}

A3DGFXSound& A3DGFXSound::operator = (const A3DGFXSound& src)
{
	if (&src == this)
		return *this;

	_CloneBase(&src);
	Init(src.m_pDevice);

	m_SoundImp = src.m_SoundImp;
	m_SoundImp22 = src.m_SoundImp22;

	return *this;
}

A3DGFXElement* A3DGFXSound::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXSound* p = new A3DGFXSound(pGfx);
	return &(*p = *this);
}

void A3DGFXSound::Release()
{
	A3DGFXElement::Release();
	
	GetCurImp()->ReleaseSound(this);
	GetCurImp()->ClearSoundFile();
}

bool A3DGFXSound::Render(A3DViewport*)
{
	return true;
}

bool A3DGFXSound::IsSoundValid() const
{
	return GetCurImp()->IsValid();
}

void A3DGFXSound::StopSound()
{
	GetCurImp()->StopSound();
}

void A3DGFXSound::TickSound(DWORD dwTickTime)
{
	GetCurImp()->TickSound(this, dwTickTime);
}

bool A3DGFXSound::TickAnimation(DWORD dwTickTime)
{
	if (!A3DGFXElement::TickAnimation(dwTickTime))
	{
		if (IsFinished() && IsSoundValid())
			StopSound();

		return false;
	}

#ifdef _ANGELICA21

	if (m_pGfx->IsChild())
		return true;

#endif

	TickSound(dwTickTime);
	return true;
}

bool A3DGFXSound::Play()
{
	GetCurImp()->PrePlay();

	if (!IsInit())
	{
		InitBaseData();
		GetCurImp()->InitData();
		SetInit(true);
	}

	GetCurImp()->PostPlay(this);
	return true;
}

bool A3DGFXSound::Stop()
{
	A3DGFXElement::Stop();
	GetCurImp()->Stop(this);
	return true;
}

void A3DGFXSound::ResumeLoop()
{
	GetCurImp()->ResumeLoop();
}

bool A3DGFXSound::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	if (!GetCurImp()->SetProperty(nOp, prop))
		return A3DGFXElement::SetProperty(nOp, prop);

	return true;
}

GFX_PROPERTY A3DGFXSound::GetProperty(int nOp) const
{
	GFX_PROPERTY prop;
	if (GetCurImp()->GetProperty(nOp, prop))
		return prop;

	return A3DGFXElement::GetProperty(nOp);
}