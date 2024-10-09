// Filename	: DlgPopMsg.cpp
// Creator	: XuWenbin
// Date		: 2012/8/3

#include "DlgPopMsg.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_TimeSafeChecker.h"

#include <AMSoundEngine.h>
#include <AMSoundBufferMan.h>

#define new A_DEBUG_NEW

CDlgPopMsg::CDlgPopMsg()
: m_pTxt_Message(NULL)
, m_state(STATE_EMPTY)
, m_dwNPCSpeakTime(0)
{
	m_Center.x = 0;
	m_Center.y = 0;
	m_Size.cx = 0;
	m_Size.cy = 0;
}

bool CDlgPopMsg::OnInitDialog()
{
	m_pTxt_Message = GetDlgItem("Txt_Message");
	return true;
}

void CDlgPopMsg::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	Align();
}

void CDlgPopMsg::OnChangeLayoutEnd(bool bAllDone)
{
	Align();
}

void CDlgPopMsg::Add(const ACHAR* pszMsg)
{
	if( !a_strcmp(pszMsg,_AL("")) )
	{
		Show(false);
		m_state = STATE_EMPTY;
		m_dwNPCSpeakTime = 0;
	}
	else
	{
		m_pTxt_Message->SetText(pszMsg);
		if( !IsShow() ) Show(true, false, false);
		m_pTxt_Message->SetAlpha(255);
		m_state = STATE_ENTER;
		m_dwNPCSpeakTime = GetTickCount();
		Align();
		
		// 这里播放音效
		GetGame()->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->
			Play2DAutoSound("SFX\\Effect\\Scene\\AreaShock6.wav");
	}
}

void CDlgPopMsg::OnTick()
{
	DWORD dwTime = GetTickCount();
    if( m_state == STATE_ENTER )
    {
        if(CECTimeSafeChecker::ElapsedTime(dwTime, m_dwNPCSpeakTime) >= 200 )
        {
            SetScale(1.0f);
            m_state = STATE_KEEPING;
            m_dwNPCSpeakTime = dwTime;
        }
        else
        {
            // 心跳效果
            float f = sinf((float)CECTimeSafeChecker::ElapsedTime(dwTime, m_dwNPCSpeakTime)/200.0f*A3D_PI);
            float fScale = 1.0f + f * 0.4f;
			
            POINT pos;
            pos.x = (int)(m_Center.x - m_Size.cx*fScale*0.5f);
            pos.y = (int)(m_Center.y - m_Size.cy*fScale*0.5f);
            SetPosEx( pos.x,pos.y );
            SetScale(fScale);
        }
    }
    else if( m_state == STATE_KEEPING )
    {
        if( CECTimeSafeChecker::ElapsedTime(dwTime, m_dwNPCSpeakTime) >= 10000 )
        {
            m_state = STATE_LEAVE;
            m_dwNPCSpeakTime = dwTime;
        }
    }
    else if( m_state == STATE_LEAVE )
    {
        if( CECTimeSafeChecker::ElapsedTime(dwTime, m_dwNPCSpeakTime) >= 1000 )
        {
            Show(false);
            m_state = STATE_EMPTY;
        }
        else
        {
            float t = 1.0f-(float)CECTimeSafeChecker::ElapsedTime(dwTime, m_dwNPCSpeakTime)/1000.0f;
            m_pTxt_Message->SetAlpha((int)(t*255));
        }
    }
}

void CDlgPopMsg::Align()
{
	if (!IsShow()) return;

    int nWidth, nHeight, nLines;
    AUI_GetTextRect(m_pTxt_Message->GetFont(), m_pTxt_Message->GetText(), nWidth, nHeight, nLines);
    m_Size.cx = nWidth;
    m_Size.cy = nHeight;
    int nWidth2, nHeight2, nLines2;
    PAUIOBJECT pZoneLbl = GetGameUIMan()->GetDialog("Win_Zone")->GetDlgItem("Txt_Faction");
    AUI_GetTextRect(pZoneLbl->GetFont(), pZoneLbl->GetText(), nWidth2, nHeight2, nLines2);
    m_Center.x = GetGameUIMan()->GetRect().CenterPoint().x;
    int y = pZoneLbl->GetPos().y + nHeight2;
    m_Center.y = y + m_Size.cy / 2;
    SetPosEx(m_Center.x-m_Size.cx/2, y);
}