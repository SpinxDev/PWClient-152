#include "stdafx.h"
#include "PropertyBar.h"
#include "MainFrm.h"
#include "BankBar.h"
#include "SoundDefBar.h"
#include "EventBar.h"
#include "Global.h"
#include "FAudioBank.h"
#include "FAudioBankArchive.h"
#include "FAudio.h"
#include "FSoundDefArchive.h"
#include "FSoundDef.h"
#include "FEvent.h"
#include "FEventGroup.h"
#include "Engine.h"
#include "Project.h"
#include "WndLabel.h"
#include "FCurve.h"
#include "AudioEditorView.h"
#include "FAudioTemplate.h"
#include "PropListCtrl/BCGPPropertyListWrapper.h"
#include "PropListCtrl/BCGPPropertyObject.h"

using AudioEngine::AudioBank;
using AudioEngine::AudioTemplate;
using AudioEngine::AUDIO_PROPERTY;
using AudioEngine::AudioGroup;
using AudioEngine::AUDIO_GROUP_PLAY_MODE;
using AudioEngine::EventGroup;
using AudioEngine::COMPRESSOR_DATA;
using AudioEngine::BANK_TYPE_LOAD_INTO_MEMORY;
using AudioEngine::BANK_TYPE_STREAM_FROM_DISK;
using AudioEngine::BANK_TYPE_DECOMPRESS_INTO_MEMORY;
using AudioEngine::AGPM_SEQUENTIAL;
using AudioEngine::AGPM_RANDOM;
using AudioEngine::AGPM_RANDOM_NOREPEAT;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 0;
const int iLabelHeight = 60;

static prop_set_template<int>	s_setBankType;
static prop_range_template<float> s_rangeVolume;
static prop_range_template<float> s_rangePitch;
static prop_set_template<int>	s_setAudioGroupPlayMode;
static prop_range_template<float> s_rangeAttack;
static prop_range_template<float> s_rangeRelease;
static prop_range_template<float> s_rangeMakeUpGain;
static prop_set_template<int>	s_setGroupClass;

/////////////////////////////////////////////////////////////////////////////
// CEventBar

BEGIN_MESSAGE_MAP(CPropertyBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CPropertyBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventBar construction/destruction

CPropertyBar::CPropertyBar()
{
	m_pWndLabel = new CWndLabel;
	m_pBCGPPropListWrapper = new CBCGPPropertyListWrapper;
	m_pPropertyListener = new CPropertyUpdateListener(this);
	m_pProperties = new CPropertyArray;
	m_pProperties->AddListener(m_pPropertyListener);
}

CPropertyBar::~CPropertyBar()
{
	delete m_pWndLabel;
	delete m_pPropertyListener;
	m_pPropertyListener = 0;
	if (m_pProperties)
		m_pProperties->Clear();
	delete m_pProperties;
	m_pProperties = 0;
	delete m_pBCGPPropListWrapper;
	m_pBCGPPropListWrapper = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CEventBar message handlers

int CPropertyBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	if (!m_pBCGPPropListWrapper->OnCreate(rectDummy, this))
		return FALSE;



	if(!m_pWndLabel->Create(this, CRect(0,0,0,0)))
		return -1;

 	if(!m_EventPropList.Init(m_pProperties, m_pBCGPPropListWrapper))
 		return -1;
 
 	if(!m_SoundDefPropList.Init(m_pProperties, m_pBCGPPropListWrapper))
 		return -1;

	return 0;
}

void CPropertyBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	m_pWndLabel->SetWindowPos(NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, iLabelHeight - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
	
	m_pBCGPPropListWrapper->SetWindowPos (NULL, nBorderSize, iLabelHeight + nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize - iLabelHeight,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertyBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	//m_wndPropList.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
		::GetSysColor (COLOR_3DSHADOW));
}

bool CPropertyBar::BuildProperty(PROP_TYPE type, HTREEITEM hItem)
{
	if(type == PROP_NONE)
		return true;

	if (!m_pProperties)
		return false;
	m_pProperties->Clear();

	m_curPropType = type;
	m_hCurItem = hItem;

	if(!hItem)
		return false;

	CBankBar* pBankBar = GF_GetMainFrame()->GetBankBar();
	CSoundDefBar* pSoundDefBar = GF_GetMainFrame()->GetSoundDefBar();
	CEventBar* pEventBar = GF_GetMainFrame()->GetEventBar();

	if(!pBankBar || !pSoundDefBar || !pEventBar)
		return false;

	s_rangeVolume.SetRange(-60.0f, 0.0f);
	s_rangeVolume.SetStep(0.1f);
	s_rangeVolume.SetUnit("dB");
	s_rangeVolume.SetTipFunc(GF_VolumeTip);


	s_rangePitch.SetRange(0.0f, 24.0f);
	s_rangePitch.SetStep(0.1f);
	s_rangePitch.SetUnit("Tone");
	s_rangePitch.SetTipFunc(GF_PitchTip);

	UINT uiIconID = 0;
	CString csLabel;

	if(type == PROP_BANK)
	{
		AudioBank* pAudioBank = (AudioBank*)pBankBar->GetTree()->GetItemData(hItem);
		if(!pAudioBank)
			return false;
		m_pProperties->AddProperty(std::string(pAudioBank->GetName()), "名称");

		if(0 == s_setBankType.GetCount())
		{
			s_setBankType.AddElement((int)BANK_TYPE_LOAD_INTO_MEMORY, "直接加载");
			s_setBankType.AddElement((int)BANK_TYPE_STREAM_FROM_DISK, "流加载");
			s_setBankType.AddElement((int)BANK_TYPE_DECOMPRESS_INTO_MEMORY, "解压加载");
		}

		{
			PROP_ITEM pi;
			pi.Val = (int)pAudioBank->GetBankType();
			pi.strName = "加载方式";
			pi.pOptions = &s_setBankType;
			pi.iWay = BCGP_PROP_WAY_OPTIONS;
			m_pProperties->AddProperty(pi);
		}
		
		m_pProperties->AddProperty(std::string(pAudioBank->GetNote()), "标签");

		uiIconID = IDI_ICON_BANK;
		csLabel = pAudioBank->GetName();
	}
	else if(type == PROP_BANK_ARCHIVE)
	{
		AudioBankArchive* pAudioBankArchive = (AudioBankArchive*)pBankBar->GetTree()->GetItemData(hItem);
		if(!pAudioBankArchive)
			return false;
		m_pProperties->AddProperty(std::string(pAudioBankArchive->GetName()), "名称");
		m_pProperties->AddProperty(std::string(pAudioBankArchive->GetNote()), "标签");

		uiIconID = IDI_ICON_BANK_ARCHIVE;
		csLabel = pAudioBankArchive->GetName();
	}
	else if(type == PROP_BANK_AUDIO)
	{
		Audio* pAudio = (Audio*)pBankBar->GetTree()->GetItemData(hItem);
		if(!pAudio)
			return false;

		m_pProperties->AddProperty(std::string(pAudio->GetName()), "名称");
		m_pProperties->AddProperty(pAudio->IsAccurateTime(), "精确时间操作", "为TRUE时可以获取、设置精确播放位置");
		m_pProperties->AddProperty(std::string(pAudio->GetNote()), "标签");

		uiIconID = IDI_ICON_AUDIO;
		csLabel = pAudio->GetName();
	}
	else if(type == PROP_SOUNDDEF_ARCHIVE)
	{
		SoundDefArchive* pSoundDefArchive = (SoundDefArchive*)pSoundDefBar->GetTree()->GetItemData(hItem);
		if(!pSoundDefArchive)
			return false;

		m_pProperties->AddProperty(std::string(pSoundDefArchive->GetName()), "名称");
		m_pProperties->AddProperty(std::string(pSoundDefArchive->GetNote()), "标签");

		uiIconID = IDI_ICON_SOUNDDEF_ARCHIVE;
		csLabel = pSoundDefArchive->GetName();
	}
	else if(type == PROP_SOUNDDEF)
	{
		SoundDef* pSoundDef = (SoundDef*)pSoundDefBar->GetTree()->GetItemData(hItem);
		if(!pSoundDef)
			return false;
		m_pProperties->AddProperty(std::string(pSoundDef->GetName()), "名称");

		SOUND_DEF_PROPERTY prop;
		pSoundDef->GetProperty(prop);

 		if(!m_SoundDefPropList.BuildProperty(prop))
 			return false;

		m_pProperties->AddProperty(std::string(pSoundDef->GetNote()), "标签");

		uiIconID = IDI_ICON_SOUNDDEF;
		csLabel = pSoundDef->GetName();
	}
	else if(type == PROP_SOUNDDEF_AUDIO)
	{
		AudioTemplate* pAudioTemplate = (AudioTemplate*)pSoundDefBar->GetTree()->GetItemData(hItem);
		if(!pAudioTemplate)
			return false;
		AUDIO_PROPERTY prop;
		pAudioTemplate->GetProperty(prop);
		
		{
			PROP_ITEM pi;
			pi.Val = prop.fVolume;
			pi.strName = "音量";
			pi.strDesc = "单位dB";
			pi.pRange = &s_rangeVolume;
			pi.iWay = BCGP_PROP_WAY_RANGE;
			m_pProperties->AddProperty(pi);
		}

		{
			PROP_ITEM pi;
			pi.Val = prop.fVolumeRandomization;
			pi.strName = "音量随机";
			pi.strDesc = "单位dB";
			pi.pRange = &s_rangeVolume;
			pi.iWay = BCGP_PROP_WAY_RANGE;
			m_pProperties->AddProperty(pi);
		}

		{
			PROP_ITEM pi;
			pi.Val = prop.fPitch;
			pi.strName = "音高";
			pi.strDesc = "单位Tone";
			pi.pRange = &s_rangePitch;
			pi.iWay = BCGP_PROP_WAY_RANGE;
			m_pProperties->AddProperty(pi);
		}

		{
			PROP_ITEM pi;
			pi.Val = prop.fPitchRandomization;
			pi.strName = "音高随机";
			pi.strDesc = "单位Tone";
			pi.pRange = &s_rangePitch;
			pi.iWay = BCGP_PROP_WAY_RANGE;
			m_pProperties->AddProperty(pi);
		}

		m_pProperties->AddProperty(std::string(pAudioTemplate->GetNote()), "标签");

		uiIconID = IDI_ICON_AUDIO;
		csLabel = pAudioTemplate->GetAudio()->GetName();
	}
	else if(type == PROP_SOUNDDEF_AUDIO_GROUP)
	{
		AudioGroup* pAudioGroup = (AudioGroup*)pSoundDefBar->GetTree()->GetItemData(hItem);
		if(!pAudioGroup)
			return false;
		if(0 == s_setAudioGroupPlayMode.GetCount())
		{
			s_setAudioGroupPlayMode.AddElement((int)AGPM_SEQUENTIAL, "顺序播放");
			s_setAudioGroupPlayMode.AddElement((int)AGPM_RANDOM, "随机播放");
			s_setAudioGroupPlayMode.AddElement((int)AGPM_RANDOM_NOREPEAT, "随机不重复");
		}

		{
			PROP_ITEM pi;
			pi.Val = (int)pAudioGroup->GetPlayMode();
			pi.strName = "播放方式";
			pi.pOptions = &s_setAudioGroupPlayMode;
			pi.iWay = BCGP_PROP_WAY_OPTIONS;
			m_pProperties->AddProperty(pi);
		}	
	}
	else if(type == PROP_EVENT_GROUP)
	{
		EventGroup* pEventGroup = (EventGroup*)pEventBar->GetTree()->GetItemData(hItem);
		if(!pEventGroup)
			return false;
		m_pProperties->AddProperty(std::string(pEventGroup->GetName()), "名称");

		if(s_setGroupClass.GetCount() != 1)
		{
			s_setGroupClass.RemoveAll();
			s_setGroupClass.AddElement(-1, "无(-1)");
			for (int i=0; i<MAX_GROUP_CLASS_NUM; ++i)
			{
				if(strlen(g_Configs.szClassNames[i]) != 0)
				{
					char szName[MAX_PATH] = {0};
					sprintf(szName, "%s(%d)", g_Configs.szClassNames[i], i);
					s_setGroupClass.AddElement(i, szName);
				}
			}
		}

		{
			PROP_ITEM pi;
			pi.Val = GF_ConvertVolumeFromRatio2DB(pEventGroup->GetGroupVolume());
			pi.strName = "Group Volume";
			pi.strDesc = "单位dB";
			pi.pRange = &s_rangeVolume;
			pi.iWay = BCGP_PROP_WAY_RANGE;
			m_pProperties->AddProperty(pi);
		}

		if(!pEventGroup->GetParentGroup())
		{
			PROP_ITEM pi;
			pi.Val = pEventGroup->GetClass();
			pi.strName = "Group类属";
			pi.strDesc = "";
			pi.pOptions = &s_setGroupClass;
			pi.iWay = BCGP_PROP_WAY_OPTIONS;
			m_pProperties->AddProperty(pi);
		}


		m_pProperties->AddProperty(std::string(pEventGroup->GetNote()), "标签");

		uiIconID = IDI_ICON_EVENT_GROUP;
		csLabel = pEventGroup->GetName();
	}
	else if(type == PROP_EVENT)
	{
		Event* pEvent = (Event*)pEventBar->GetTree()->GetItemData(hItem);
		if(!pEvent)
			return false;
		m_pProperties->AddProperty(std::string(pEvent->GetName()), "名称");
		EVENT_PROPERTY prop;
		pEvent->GetProperty(prop);		
 		if(!m_EventPropList.BuildProperty(prop))
 			return false;

		m_pProperties->AddProperty(std::string(pEvent->GetNote()), "标签");

		m_pProperties->AddProperty(std::string(pEvent->GetGuidString()), "GUID");

		uiIconID = IDI_ICON_EVENT;
		csLabel = pEvent->GetName();
	}
	if(uiIconID != 0 && csLabel != "")
		m_pWndLabel->SetLabel(csLabel, uiIconID);

	m_pBCGPPropListWrapper->AttachTo(m_pProperties);

	return true;
}

void CPropertyBar::UpdateProperty(bool bSave, bool bIsCommit /*= false*/)
{
	CBankBar* pBankBar = GF_GetMainFrame()->GetBankBar();
	CSoundDefBar* pSoundDefBar = GF_GetMainFrame()->GetSoundDefBar();
	CEventBar* pEventBar = GF_GetMainFrame()->GetEventBar();

	if(!pBankBar || !pSoundDefBar || !pEventBar)
		return;

	PROP_TYPE type = m_curPropType;
	HTREEITEM hItem = m_hCurItem;

	if(type == PROP_BANK)
	{
		AudioBank* pAudioBank = (AudioBank*)pBankBar->GetTree()->GetItemData(hItem);
		if(!pAudioBank)
			return;
		if(bSave)
		{
			std::string strNewName = any_cast<std::string>(m_pProperties->GetPropVal(idBankName)).c_str();
			if(0 != strNewName.compare(pAudioBank->GetName()))
			{
				if(!GF_CheckName(strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pAudioBank->GetName());
					return;
				}
				if(!pBankBar->SetItemText(hItem, strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pAudioBank->GetName());
					return;
				}
			}
			pAudioBank->SetBankType((BANK_TYPE)any_cast<int>(m_pProperties->GetPropVal(idBankType)));
			pAudioBank->SetNote(any_cast<std::string>(m_pProperties->GetPropVal(idBankNote)).c_str());
		}
		else
		{
			m_pProperties->SetPropVal(idBankName, pAudioBank->GetName());
			m_pProperties->SetPropVal(idBankType, (int)pAudioBank->GetBankType());
			m_pProperties->SetPropVal(idBankNote, pAudioBank->GetNote());
		}
	}
	else if(type == PROP_BANK_ARCHIVE)
	{
		AudioBankArchive* pAudioBankArchive = (AudioBankArchive*)pBankBar->GetTree()->GetItemData(hItem);
		if(!pAudioBankArchive)
			return;
		if(bSave)
		{
			std::string strNewName = any_cast<std::string>(m_pProperties->GetPropVal(idBankArchiveName)).c_str();
			if(0 != strNewName.compare(pAudioBankArchive->GetName()))
			{
				if(!GF_CheckName(strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pAudioBankArchive->GetName());
					return;
				}
				if(!pBankBar->SetItemText(hItem, strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pAudioBankArchive->GetName());
					return;
				}
			}
			pAudioBankArchive->SetNote(any_cast<std::string>(m_pProperties->GetPropVal(idBankArchiveNote)).c_str());
		}
		else
		{
			m_pProperties->SetPropVal(idBankArchiveName, pAudioBankArchive->GetName());
			m_pProperties->SetPropVal(idBankArchiveNote, pAudioBankArchive->GetNote());
		}
	}
	else if(type == PROP_BANK_AUDIO)
	{
		Audio* pAudio = (Audio*)pBankBar->GetTree()->GetItemData(hItem);
		if(!pAudio)
			return;
		if(bSave)
		{
			std::string strNewName = any_cast<std::string>(m_pProperties->GetPropVal(idBankAudioName)).c_str();
			if(0 != strNewName.compare(pAudio->GetName()))
			{
				if(!GF_CheckName(strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pAudio->GetName());
					return;
				}
				if(!pBankBar->SetItemText(hItem, strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pAudio->GetName());
					return;
				}
			}
			pAudio->EnableAccurateTime(any_cast<bool>(m_pProperties->GetPropVal(idBankAudioAccurateTime)));
			pAudio->SetNote(any_cast<std::string>(m_pProperties->GetPropVal(idBankAudioNote)).c_str());
		}
		else
		{
			m_pProperties->SetPropVal(idBankAudioName, pAudio->GetName());
			m_pProperties->SetPropVal(idBankAudioAccurateTime, pAudio->IsAccurateTime());
			m_pProperties->SetPropVal(idBankAudioNote, pAudio->GetNote());
		}		
	}
	else if(type == PROP_SOUNDDEF_ARCHIVE)
	{
		SoundDefArchive* pSoundDefArchive = (SoundDefArchive*)pSoundDefBar->GetTree()->GetItemData(hItem);
		if(!pSoundDefArchive)
			return;
		if(bSave)
		{
			std::string strNewName = any_cast<std::string>(m_pProperties->GetPropVal(idSoundDefArchiveName)).c_str();
			if(0 != strNewName.compare(pSoundDefArchive->GetName()))
			{
				if(!GF_CheckName(strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pSoundDefArchive->GetName());
					return;
				}
				if(!pSoundDefBar->SetItemText(hItem, strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pSoundDefArchive->GetName());
					return;
				}
			}
			pSoundDefArchive->SetNote(any_cast<std::string>(m_pProperties->GetPropVal(idSoundDefArchiveNote)).c_str());
		}
		else
		{
			m_pProperties->SetPropVal(idSoundDefArchiveName, pSoundDefArchive->GetName());
			m_pProperties->SetPropVal(idSoundDefArchiveNote, pSoundDefArchive->GetNote());
		}	
	}
	else if(type == PROP_SOUNDDEF)
	{
		bool bSoundDefPropChanged = false;
		SOUND_DEF_PROPERTY prop;
		SoundDef* pSoundDef = (SoundDef*)pSoundDefBar->GetTree()->GetItemData(hItem);
		if(!pSoundDef)
			return;
		pSoundDef->GetProperty(prop);

		if(bSave)
		{
			std::string strNewName = any_cast<std::string>(m_pProperties->GetPropVal(idSoundDefName)).c_str();
			if(0 != strNewName.compare(pSoundDef->GetName()))
			{
				if(!GF_CheckName(strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pSoundDef->GetName());
					return;
				}
				if(!pSoundDefBar->SetItemText(hItem, strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pSoundDef->GetName());
					return;
				}
			}
			m_SoundDefPropList.PropChanged(prop, bSave, bIsCommit, 1);
			pSoundDef->SetNote(any_cast<std::string>(m_pProperties->GetPropVal(idSoundDefNote)).c_str());
		}
		else
		{
			m_pProperties->SetPropVal(idSoundDefName, pSoundDef->GetName());
			m_SoundDefPropList.PropChanged(prop, bSave, bIsCommit, 1);
			m_pProperties->SetPropVal(idSoundDefNote, pSoundDef->GetNote());
		}
		
		if(bSave)
		{
			pSoundDef->SetProperty(prop);
			GF_GetMainFrame()->GetSoundDefBar()->RecreateSoundInstance();
		}
	}
	else if(type == PROP_SOUNDDEF_AUDIO)
	{
		AudioTemplate* pAudioTemplate = (AudioTemplate*)pSoundDefBar->GetTree()->GetItemData(hItem);
		if(!pAudioTemplate)
			return;
		bool bAudioPropChanged = false;
		AUDIO_PROPERTY prop;
		pAudioTemplate->GetProperty(prop);
		if(bSave)
		{
			prop.fVolume = any_cast<float>(m_pProperties->GetPropVal(idAudioVolume));
			prop.fVolumeRandomization = any_cast<float>(m_pProperties->GetPropVal(idAudioVolumeRandom));
			prop.fPitch = any_cast<float>(m_pProperties->GetPropVal(idAudioPitch));
			prop.fPitchRandomization = any_cast<float>(m_pProperties->GetPropVal(idAudioPitchRandom));
			pAudioTemplate->SetNote(any_cast<std::string>(m_pProperties->GetPropVal(idAudioNote)).c_str());

			pAudioTemplate->SetProperty(prop);
		}
		else
		{
			m_pProperties->SetPropVal(idAudioVolume, prop.fVolume);
			m_pProperties->SetPropVal(idAudioVolumeRandom, prop.fVolumeRandomization);
			m_pProperties->SetPropVal(idAudioPitch, prop.fPitch);
			m_pProperties->SetPropVal(idAudioPitchRandom, prop.fPitchRandomization);
			m_pProperties->SetPropVal(idAudioNote, pAudioTemplate->GetNote());
		}		
	}
	else if(type == PROP_SOUNDDEF_AUDIO_GROUP)
	{
		AudioGroup* pAudioGroup = (AudioGroup*)pSoundDefBar->GetTree()->GetItemData(hItem);
		if(!pAudioGroup)
			return;
		if(bSave)
		{
			pAudioGroup->SetPlayMode(static_cast<AUDIO_GROUP_PLAY_MODE>(any_cast<int>(m_pProperties->GetPropVal(idAudioGroupPlayMode))));
		}
		else
		{
			m_pProperties->SetPropVal(idAudioGroupPlayMode, (int)pAudioGroup->GetPlayMode());
		}
	}
	else if(type == PROP_EVENT)
	{
		EVENT_PROPERTY prop;
		Event* pEvent = (Event*)pEventBar->GetTree()->GetItemData(hItem);
		if(!pEvent)
			return;
		pEvent->GetProperty(prop);
		if(bSave)
		{
			std::string strNewName = any_cast<std::string>(m_pProperties->GetPropVal(idEventName)).c_str();
			if(0 != strNewName.compare(pEvent->GetName()))
			{
				if(!GF_CheckName(strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pEvent->GetName());
					return;
				}
				if(!pEventBar->SetItemText(hItem, strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pEvent->GetName());
					return;
				}
			}
			m_EventPropList.PropChanged(prop, bSave, bIsCommit, 1);
			pEvent->SetNote(any_cast<std::string>(m_pProperties->GetPropVal(idEventNote)).c_str());
		}
		else
		{
			m_pProperties->SetPropVal(idEventName, pEvent->GetName());
			m_EventPropList.PropChanged(prop, bSave, bIsCommit, 1);
			m_pProperties->SetPropVal(idEventNote, pEvent->GetNote());
		}		
		if(bSave)
		{
			pEvent->SetProperty(prop);
			GF_GetView()->GetEventView()->RecreateEventInstance();
		}
	}
	else if(type == PROP_EVENT_GROUP)
	{
		EventGroup* pEventGroup = (EventGroup*)pEventBar->GetTree()->GetItemData(hItem);
		if(!pEventGroup)
			return;
		if(bSave)
		{
			int idx = 0;
			std::string strNewName = any_cast<std::string>(m_pProperties->GetPropVal(idx++)).c_str();
			if(0 != strNewName.compare(pEventGroup->GetName()))
			{
				if(!GF_CheckName(strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pEventGroup->GetName());
					return;
				}
				if(!pEventBar->SetItemText(hItem, strNewName.c_str()))
				{
					m_pProperties->SetPropVal(0, pEventGroup->GetName());
					return;
				}
			}
	//		idx++;			

			pEventGroup->SetGroupVolume(GF_ConvertVolumeFromDB2Ratio(any_cast<float>(m_pProperties->GetPropVal(idx++))));

			if(!pEventGroup->GetParentGroup())
				pEventGroup->SetClass(any_cast<int>(m_pProperties->GetPropVal(idx++)));

			pEventGroup->SetNote(any_cast<std::string>(m_pProperties->GetPropVal(idx++)).c_str());
		}
		else
		{
			int idx = 0;
			m_pProperties->SetPropVal(idx++, pEventGroup->GetName());

		//	idx++;

			m_pProperties->SetPropVal(idx++, GF_ConvertVolumeFromRatio2DB(pEventGroup->GetGroupVolume()));
			
			if(!pEventGroup->GetParentGroup())
				m_pProperties->SetPropVal(idx++, pEventGroup->GetClass());
			m_pProperties->SetPropVal(idx++, pEventGroup->GetNote());
		}
	}

	Engine::GetInstance().GetCurProject()->SetModified(true);
}