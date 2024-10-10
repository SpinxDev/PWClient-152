// Filename	: EC_UIAnimation.cpp
// Creator	: Feng Ning
// Date		: 2011/11/17

#include "EC_UIAnimation.h"
#include "WikiDataReader.h"
#include <AFI.h>

#define new A_DEBUG_NEW

CECUIAnimation::CECUIAnimation()
:m_CurFrame(0)
,m_Tick(-1)
,m_pDlg(0)
{

}

void CECUIAnimation::Reset()
{
	if(!m_pDlg)
	{
		ASSERT(false);
		return;
	}

	m_pDlg->SetScale(1.f);
	m_pDlg->SetPosEx(m_OriginPos.x, m_OriginPos.y);
	m_pDlg->SetWholeAlpha(m_OriginAlpha);

	m_CurFrame = 0;
	m_Tick = 0;
}

void CECUIAnimation::Start(PAUIDIALOG pDlg)
{
	if(!pDlg)
	{
		ASSERT(false);
		return;
	}

	m_pDlg = pDlg;
	m_OriginPos = pDlg->GetPos();
	m_OriginSize = pDlg->GetDefaultSize();
	m_OriginAlpha = pDlg->GetWholeAlpha();
	m_CurFrame = 0;
	m_Tick = 0;
}

bool CECUIAnimation::Update()
{
	// only tick after start and shown
	if(!m_pDlg || !m_pDlg->IsShow())
	{
		return true;
	}

	while(m_CurFrame < (int)m_Frames.size() )
	{
		KeyFrame& frame = m_Frames[m_CurFrame];
		if(frame.tick > m_Tick)
		{
			break;
		}
		m_CurFrame++;
	}
	m_Tick++;

	KeyFrame offset;
	if(!IsStop())
	{
		// get last key frame information
		KeyFrame lastFrame;
		if(m_CurFrame > 0) lastFrame = m_Frames[m_CurFrame-1];

		// get current key frame information
		const KeyFrame& curFrame = m_Frames[m_CurFrame];

		// calculate the delta
		KeyFrame deltaFrame = curFrame - lastFrame;

		// time passed
		float deltaTick = (float)m_Tick - lastFrame.tick;

		// calculate the offset
		if(deltaFrame.tick <= 0 || deltaTick <= 0)
		{
			// warning about invalid data
			ASSERT(deltaFrame.tick >= 0 && deltaTick >= 0);
			offset = lastFrame;
		}
		else if(m_Tick == curFrame.tick)
		{
			offset = curFrame;
		}
		else
		{
			offset.scale = deltaFrame.scale * deltaTick / deltaFrame.tick;
			offset.ox = int(deltaFrame.ox * deltaTick / deltaFrame.tick);
			offset.oy = int(deltaFrame.oy * deltaTick / deltaFrame.tick);
			offset.alpha = int(deltaFrame.alpha * deltaTick / deltaFrame.tick);
			offset = offset + lastFrame;
		}
	}

	if(m_pDlg)
	{
		m_pDlg->SetScale(offset.scale);
		int x = int(m_OriginPos.x + m_OriginSize.cx * (0.5f - offset.scale / 2) );
		int y = int(m_OriginPos.y + m_OriginSize.cy * (0.5f - offset.scale / 2) );

		m_pDlg->SetPosEx(x + offset.ox, y + offset.oy);

		a_Clamp(offset.alpha, 0, 255);
		m_pDlg->SetWholeAlpha(offset.alpha);
	}

	return !IsStop();
}

bool CECUIAnimation::IsStop()
{
	return m_CurFrame >= (int)m_Frames.size();
}

void CECUIAnimation::AddFrame(const KeyFrame& frame)
{
	m_Frames.push_back(frame);
}

CECUIAnimation* CECUIAnimation::Load(const char* path)
{
	if(!af_IsFileExist(path))
	{
		return NULL;
	}

	WikiDataReader reader(false);

	// for KeyFrame
	reader.Init(_AL("KeyFrame"), sizeof(CECUIAnimation::KeyFrame));

	CECUIAnimation::KeyFrame temp;
	reader.Register(_AL("Tick"), &temp, &temp.tick, WikiDataReader::TYPE_INTEGER);
	reader.Register(_AL("Scale"), &temp, &temp.scale, WikiDataReader::TYPE_FLOAT);
	reader.Register(_AL("OffsetX"), &temp, &temp.ox, WikiDataReader::TYPE_INTEGER);
	reader.Register(_AL("OffsetY"), &temp, &temp.oy, WikiDataReader::TYPE_INTEGER);
	reader.Register(_AL("Alpha"), &temp, &temp.alpha, WikiDataReader::TYPE_INTEGER);

	if(!reader.Open(path, true))
	{
		return NULL;
	}

	CECUIAnimation* pAni = new CECUIAnimation();
	while(!reader.IsEnd())
	{
		memset(&temp, 0, sizeof(temp));
		if(!reader.ReadObject(&temp))
		{
			ASSERT(false);
			break;
		}
		pAni->AddFrame(temp);
	}
	reader.Release();

	return pAni;
}
