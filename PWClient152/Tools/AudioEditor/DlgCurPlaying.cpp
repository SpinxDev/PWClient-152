// DlgCurPlaying.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgCurPlaying.h"
#include "FEvent.h"
#include "FEventInstance.h"
#include "FSoundInstanceTemplate.h"
#include "FAudioTemplate.h"
#include "FEventLayer.h"

using AudioEngine::Event;
using AudioEngine::EventLayer;
using AudioEngine::SoundInstanceTemplate;

// CDlgCurPlaying dialog

IMPLEMENT_DYNAMIC(CDlgCurPlaying, CBCGPDialog)

CDlgCurPlaying::CDlgCurPlaying(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgCurPlaying::IDD, pParent)
{
	m_pEventInstance = 0;
}

CDlgCurPlaying::~CDlgCurPlaying()
{
}

void CDlgCurPlaying::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PLAYING, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgCurPlaying, CBCGPDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgCurPlaying message handlers

BOOL CDlgCurPlaying::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();
	m_wndListCtrl.InsertColumn(0, "²ã", LVCFMT_CENTER, 80);
	m_wndListCtrl.InsertColumn(1, "ÕýÔÚ²¥·Å", LVCFMT_CENTER, 390);

	m_wndListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_dscMan.Add(DynSizeCtrl(&m_wndListCtrl, this));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgCurPlaying::OnPlay(AudioInstance* pAudioInstance)
{
	//Need Fix
// 	if(!m_pEventInstance)
// 		return false;
// 	Event* pEvent = m_pEventInstance->GetEvent();
// 	if(!pEvent)
// 		return false;
// 	int iLayerNum = pEvent->GetLayerNum();
// 	for (int i=0; i<iLayerNum; ++i)
// 	{
// 		EventLayer* pEventLayer = pEvent->GetLayerByIndex(i);
// 		if(!pEventLayer)
// 			continue;
// 		if(pEventLayer == pAudioInstance->GetSoundInstanceTemplate()->GetEventLayer())
// 		{
// 			m_wndListCtrl.SetItemText(i, 1, pAudioInstance->GetAudioTemplate()->GetAudio()->GetPath());
// 			return true;
// 		}
// 	}
	return true;
}

bool CDlgCurPlaying::OnStop(AudioInstance* pAudioInstance)
{
	//Need Fix
// 	if(!m_pEventInstance)
// 		return false;
// 	Event* pEvent = m_pEventInstance->GetEvent();
// 	if(!pEvent)
// 		return false;
// 	pAudioInstance->RemoveListener(this);
// 	int iLayerNum = pEvent->GetLayerNum();
// 	for (int i=0; i<iLayerNum; ++i)
// 	{
// 		EventLayer* pEventLayer = pEvent->GetLayerByIndex(i);
// 		if(!pEventLayer)
// 			continue;
// 		if(pEventLayer == pAudioInstance->GetSoundInstanceTemplate()->GetEventLayer())
// 		{
// 			m_wndListCtrl.SetItemText(i, 1, "");
// 			return true;
// 		}
// 	}	
	return true;
}

void CDlgCurPlaying::SetEventInstance(EventInstance* pEventInstance)
{
	if(!pEventInstance)
		return;
	m_wndListCtrl.DeleteAllItems();
	m_pEventInstance = pEventInstance;
	Event* pEvent = m_pEventInstance->GetEvent();
	if(!pEvent)
		return;
	int iLayerNum = pEvent->GetLayerNum();
	for (int i=0; i<iLayerNum; ++i)
	{
		EventLayer* pEventLayer = pEvent->GetLayerByIndex(i);
		if(!pEventLayer)
			continue;
		m_wndListCtrl.InsertItem(i, pEventLayer->GetName());
		int iSoundInsTemNum = pEventLayer->GetSoundInstanceTemplateNum();
		for (int j=0; j<iSoundInsTemNum; ++j)
		{
			SoundInstanceTemplate* pSoundInsTem = pEventLayer->GetSoundInstanceTemplateByIndex(j);
			if(!pSoundInsTem)
				continue;
			int iAudioInstanceNum = pSoundInsTem->GetAudioInstanceNum();
			for (int k=0; k<iAudioInstanceNum; ++k)
			{
				AudioInstance* pAudioInstance = pSoundInsTem->GetAudioInstanceByIndex(k, true, true);
				if(!pAudioInstance)
					continue;
		//		pAudioInstance->AddListener((AudioInstance::AudioInstanceListener*)this);
			}
		}
	}
}

void CDlgCurPlaying::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	m_dscMan.Resize(cx, cy);
}
