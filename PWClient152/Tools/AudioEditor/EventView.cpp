#include "stdafx.h"
#include "EventView.h"
#include "MainFrm.h"
#include "Global.h"
#include "WndDefine.h"
#include "WndEvent.h"
#include "FEvent.h"
#include "FEventLayer.h"
#include "FEffect.h"

using AudioEngine::VECTOR;
using AudioEngine::EventLayer;
using AudioEngine::Effect;

const int nBorderSize = 0;
const int iPlayWndHeight = 70;

CEventView::CEventView(void)
{
	m_pWndPlay = new CWndPlay;
	m_pEvent = 0;
	m_pLastPlayEvent = 0;
	m_pEventInstance = 0;
	m_bPlaying = false;
	m_pWndEvent = new CWndEvent;
	m_bNeedRecreateEventInstance = false;
}

CEventView::~CEventView(void)
{
	delete m_pWndEvent;
	m_pWndEvent = 0;
	delete m_pWndPlay;
	m_pWndPlay = 0;
	if(m_pEventInstance)
	{
		m_pEventInstance->Stop(true);
	}	
}

BEGIN_MESSAGE_MAP(CEventView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_PLAY_PLAY, OnClickPlayBtn)
	ON_MESSAGE(WM_PLAY_PAUSE, OnClickPauseBtn)
	ON_MESSAGE(WM_PLAY_STOP, OnClickStopBtn)
	ON_MESSAGE(WM_PLAY_PLAYING, OnClickPlayingBtn)
END_MESSAGE_MAP()

void CEventView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
}

void CEventView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_pWndPlay->SetWindowPos(NULL, 0, 0, 
		cx, iPlayWndHeight,
		SWP_NOACTIVATE | SWP_NOZORDER);

	m_pWndEvent->SetWindowPos(NULL, 0, iPlayWndHeight, 
		cx, cy,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

int CEventView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_pWndPlay->Create(this, CRect(0,0,0,0)))
		return -1;

	if(!m_pWndEvent->Create(this, CRect(0,0,0,0)))
		return -1;

	m_pWndPlay->ShowPlayingButton(true);

	if(!m_dlgCurPlaying.Create(IDD_CUR_PLAYING, this))
		return -1;
	m_dlgCurPlaying.ShowWindow(SW_HIDE);

	return 0;
}

LRESULT CEventView::OnClickPlayBtn(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hItem = GF_GetMainFrame()->GetEventBar()->GetTree()->GetSelectedItem();
	if(!hItem)
		return 0;
	EVENT_ITEM_TYPE type = (EVENT_ITEM_TYPE)GF_GetMainFrame()->GetEventBar()->GetTree()->GetExtraData(hItem);
	if(type != EVENT_ITEM_EVENT)
		return 0;
	if(!m_pEvent)
		return 0;
	if(m_bPlaying)
		return 0;

	GF_Log(LOG_NORMAL, "点击播放按钮");
	if(!m_bPlaying)
		m_bPlaying = true;
	
	VECTOR pos(g_Configs.v3DEventPos.x, g_Configs.v3DEventPos.x, g_Configs.v3DEventPos.z);
	VECTOR vel(g_Configs.v3DEventVel.x, g_Configs.v3DEventVel.x, g_Configs.v3DEventVel.z);
	VECTOR ori(g_Configs.v3DEventOrientation.x, g_Configs.v3DEventOrientation.x, g_Configs.v3DEventOrientation.z);

	m_pWndPlay->SetPlayButtonState(true);
	m_pWndPlay->SetPauseButtonState(false);
	if(!m_pEvent->Reload())
		goto _failed;
	if(!m_pEventInstance || m_pLastPlayEvent!=m_pEvent || m_bNeedRecreateEventInstance)
	{
		if(m_pLastPlayEvent && m_pEventInstance)
		{
			m_pLastPlayEvent->DestroyInstance(m_pEventInstance);
		}
		m_pEventInstance = m_pEvent->CreateInstance(false);
		if(!m_pEventInstance)
			goto _failed;		
		m_bNeedRecreateEventInstance = false;
	}
	m_pLastPlayEvent = m_pEvent;
	m_pWndEvent->SetEventInstance(m_pEventInstance);
	m_dlgCurPlaying.SetEventInstance(m_pEventInstance);

	EVENT_PROPERTY prop;
	m_pEventInstance->GetEvent()->GetProperty(prop);
	if(prop.mode == AudioEngine::MODE_3D)
	{
		m_pEventInstance->Set3DAttributes(pos, vel);
		m_pEventInstance->Set3DConeOrientation(ori);
	}

	m_pEventInstance->AddListener((EventInstance::EventInstanceListener*)this);

	if(!m_pEventInstance->Start())
	{
		GF_Log(LOG_ERROR, "播放事件“%s”失败。", m_pEvent->GetName());
		goto _failed;
	}
	setMuteLayers();
	setEffectEnable();
	GF_Log(LOG_NORMAL, "播放事件“%s”", m_pEvent->GetName());	
	return 0;

_failed:
	m_pEvent = 0;
	m_pEventInstance = 0;
	m_pWndPlay->SetPlayButtonState(false);
	m_bPlaying = false;
	return 0;
}

LRESULT CEventView::OnClickPauseBtn(WPARAM wParam, LPARAM lParam)
{
	if(!m_pEventInstance || !m_pEvent)
		return 0;
	GF_Log(LOG_NORMAL, "点击暂停按钮");	
	bool bPaused = m_pEventInstance->IsPaused();
	m_pWndPlay->SetPauseButtonState(!bPaused);
	if(!m_pEventInstance->Pause(!bPaused))
	{
		GF_Log(LOG_ERROR, "暂停播放事件“%s”失败", m_pEvent->GetName());
		m_pWndPlay->SetPauseButtonState(bPaused);
		return 0;
	}
	GF_Log(LOG_NORMAL, "暂停播放事件“%s”", m_pEvent->GetName());
	return 0;
}

void CEventView::setMuteLayers()
{
	if(!m_pEventInstance || !m_pEvent)
		return;
	int iLayerNum = m_pEvent->GetLayerNum();
	for (int i=0; i<iLayerNum; ++i)
	{
		bool bCheck = m_pWndEvent->GetLayerCheckState(i);
		m_pEventInstance->GetEventLayerInstanceByIndex(i)->SetMute(!bCheck);
	}
}

void CEventView::setEffectEnable()
{
	if(!m_pEventInstance || !m_pEvent)
		return;
	int iLayerNum = m_pEvent->GetLayerNum();
	for (int i=0; i<iLayerNum; ++i)
	{
		EventLayer* pEventLayer = m_pEvent->GetLayerByIndex(i);
		if(!pEventLayer)
			continue;
		int iEffectNum = pEventLayer->GetEffectNum();
		for (int j=0; j<iEffectNum; ++j)
		{
			Effect* pEffect = pEventLayer->GetEffectByIndex(j);
			if(!pEffect)
				continue;
			bool bCheck = m_pWndEvent->GetEffectCheckState(i, j);
			if(!bCheck)
			{
				m_pEventInstance->GetEventLayerInstanceByIndex(i)->RemoveEffect(pEffect);
			}
			else
			{
				m_pEventInstance->GetEventLayerInstanceByIndex(i)->ApplyEffect(pEffect);
			}
		}		
	}
}

LRESULT CEventView::OnClickStopBtn(WPARAM wParam, LPARAM lParam)
{
	if(!m_pEventInstance || !m_pEvent)
		return 0;
	m_pWndPlay->SetPauseButtonState(false);
	GF_Log(LOG_NORMAL, "点击停止按钮");
	if(!m_pEventInstance->Stop())
	{
		GF_Log(LOG_ERROR, "停止播放事件“%s”失败", m_pEvent->GetName());
		return 0;
	}
	GF_Log(LOG_NORMAL, "停止播放事件“%s”", m_pEvent->GetName());
	return 0;
}

LRESULT CEventView::OnClickPlayingBtn(WPARAM wParam, LPARAM lParam)
{
	m_dlgCurPlaying.ShowWindow(SW_SHOWNORMAL);
	return 0;
}

bool CEventView::OnStop(EventInstance* pEventInstance)
{
	GF_Log(LOG_NORMAL, "事件播放停止");
	m_pEventInstance->RemoveListener((EventInstance::EventInstanceListener*)this);
	m_bPlaying = false;
	if(m_pWndPlay)
		m_pWndPlay->SetPlayButtonState(false);
	if(m_pWndEvent)
		m_pWndEvent->Reset();
	return true;
}

void CEventView::SetEvent(Event* pEvent)
{
	if(m_pEvent == pEvent)
		return;	
	if(m_pEventInstance)
	{
		if(!m_pEventInstance->Stop(true))
		{
			GF_Log(LOG_NORMAL, "CEventView::SetEvent 停止播放事件“%s”失败", m_pEvent->GetName());
			return;
		}
	}
	m_pEvent = pEvent;
	m_pWndEvent->SetEvent(pEvent);
}

void CEventView::RecreateEventInstance()
{
	m_bNeedRecreateEventInstance = true;
}