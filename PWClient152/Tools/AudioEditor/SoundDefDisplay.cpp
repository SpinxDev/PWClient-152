#include "stdafx.h"
#include "SoundDefDisplay.h"
#include "FSoundDefManager.h"
#include "Engine.h"
#include "Project.h"
#include "FSoundDef.h"
#include "FAudio.h"
#include "FSoundDefArchive.h"
#include "resource.h"
#include "FSoundDefManager.h"
#include "Engine.h"
#include "Project.h"
#include "TreeCtrlEx.h"
#include "FAudioGroup.h"
#include "FAudioTemplate.h"

using AudioEngine::SoundDefArchive;
using AudioEngine::Audio;
using AudioEngine::AudioContainer;
using AudioEngine::SoundDef;
using AudioEngine::AudioGroup;

std::string _calcBankPath(Audio* pAudio)
{
	std::string strPath = pAudio->GetName();
	AudioContainer* pParent = pAudio->GetParentContainer();
	while(pParent)
	{
		std::string strTemp = pParent->GetName();
		strTemp += "\\";
		strPath = strTemp + strPath;
		pParent = pParent->GetParentAudioContainer();
	}
	return strPath;
}

CSoundDefDisplay::CSoundDefDisplay(void)
: m_pWndTree(0)
{
}

CSoundDefDisplay::~CSoundDefDisplay(void)
{
}

bool CSoundDefDisplay::BuildImageList(CImageList& imageList)
{
	imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_SOUNDDEF));
	imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_SOUNDDEF_ARCHIVE));
	imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_AUDIO));
	return true;
}

bool CSoundDefDisplay::UpdateTree(CTreeCtrlEx* pWndTree)
{
	if(!pWndTree)
		return false;
	m_pWndTree = pWndTree;
	m_pWndTree->DeleteAllItems();
	if(!Engine::GetInstance().GetCurProject())
		return false;
	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		return false;
	int iArchiveNum = pSoundDefManager->GetSoundDefArchiveNum();
	for (int i=0; i<iArchiveNum; ++i)
	{
		SoundDefArchive* pSoundDefArchive = pSoundDefManager->GetSoundDefArchiveByIndex(i);
		if(!pSoundDefArchive)
			return false;
		HTREEITEM hItem = m_pWndTree->InsertItem(pSoundDefArchive->GetName(), IMAGE_SOUNDDEF_ARCHIVE, IMAGE_SOUNDDEF_ARCHIVE, TVI_ROOT, TVI_SORT);
		m_pWndTree->SetItemData(hItem, (DWORD_PTR)pSoundDefArchive);
		m_pWndTree->SetExtraData(hItem, SOUNDDEF_ITEM_ARCHIVE);
		if(!BuildTree(hItem))
			return false;
	}
	return true;
}

bool CSoundDefDisplay::BuildTree(HTREEITEM hItem)
{
	m_pWndTree->DeleteChildren(hItem);
	if(m_pWndTree->GetExtraData(hItem) == SOUNDDEF_ITEM_ARCHIVE)
	{
		SoundDefArchive* pSoundDefArchive = (SoundDefArchive*)m_pWndTree->GetItemData(hItem);
		int iArchiveNum = pSoundDefArchive->GetSoundDefArchiveNum();
		for (int i=0; i<iArchiveNum; ++i)
		{
			SoundDefArchive* pArchive = pSoundDefArchive->GetSoundDefArchiveByIndex(i);
			if(!pArchive)
				return false;
			HTREEITEM hNewItem = m_pWndTree->InsertItem(pArchive->GetName(), IMAGE_SOUNDDEF_ARCHIVE, IMAGE_SOUNDDEF_ARCHIVE, hItem, TVI_SORT);
			m_pWndTree->SetItemData(hNewItem, (DWORD_PTR)pArchive);
			m_pWndTree->SetExtraData(hNewItem, SOUNDDEF_ITEM_ARCHIVE);
			if(!BuildTree(hNewItem))
				return false;
		}

		int iSoundDefNum = pSoundDefArchive->GetSoundDefNum();
		for (int i=0; i<iSoundDefNum; ++i)
		{
			SoundDef* pSoundDef = pSoundDefArchive->GetSoundDefByIndex(i);
			if(!pSoundDef)
				return false;
			HTREEITEM hNewItem = m_pWndTree->InsertItem(pSoundDef->GetName(), IMAGE_SOUNDDEF, IMAGE_SOUNDDEF, hItem, TVI_SORT);
			m_pWndTree->SetItemData(hNewItem, (DWORD_PTR)pSoundDef);
			m_pWndTree->SetExtraData(hNewItem, SOUNDDEF_ITEM_SOUNDDEF);

			int iAudioGroupNum = pSoundDef->GetAudioGroupNum();
			for (int j=0; j<iAudioGroupNum; ++j)
			{
				AudioGroup* pAudioGroup = pSoundDef->GetAudioGroupByIndex(j);
				if(!pAudioGroup)
					return false;
				int iAudioTemplateNum = pAudioGroup->GetAudioTemplateNum();
				if(pAudioGroup->IsSingle())
				{
					HTREEITEM hAudioItem = m_pWndTree->InsertItem(_calcBankPath(pAudioGroup->GetAudioTemplateByIndex(0)->GetAudio()).c_str(), IMAGE_SOUNDDEF_AUDIO, IMAGE_SOUNDDEF_AUDIO, hNewItem);
					m_pWndTree->SetItemData(hAudioItem, (DWORD_PTR)pAudioGroup->GetAudioTemplateByIndex(0));
					m_pWndTree->SetExtraData(hAudioItem, SOUNDDEF_ITEM_AUDIO);
				}
				else
				{
					HTREEITEM hAudioGroupItem = m_pWndTree->InsertItem("group", IMAGE_SOUNDDEF_AUDIO_GROUP, IMAGE_SOUNDDEF_AUDIO_GROUP, hNewItem);
					m_pWndTree->SetItemData(hAudioGroupItem, (DWORD_PTR)pAudioGroup);
					m_pWndTree->SetExtraData(hAudioGroupItem, SOUNDDEF_ITEM_AUDIO_GROUP);
					for (int k=0; k<iAudioTemplateNum; ++k)
					{
						HTREEITEM hAudioItem = m_pWndTree->InsertItem(_calcBankPath(pAudioGroup->GetAudioTemplateByIndex(k)->GetAudio()).c_str(), IMAGE_SOUNDDEF_AUDIO, IMAGE_SOUNDDEF_AUDIO, hAudioGroupItem);
						m_pWndTree->SetItemData(hAudioItem, (DWORD_PTR)pAudioGroup->GetAudioTemplateByIndex(k));
						m_pWndTree->SetExtraData(hAudioItem, SOUNDDEF_ITEM_AUDIO);
					}
				}
			}
		}
	}
	return true;
}

bool CSoundDefDisplay::RebuildSoundDefItems(HTREEITEM hItem)
{
	m_pWndTree->DeleteChildren(hItem);
	if(m_pWndTree->GetExtraData(hItem) == SOUNDDEF_ITEM_SOUNDDEF)
	{
		SoundDef* pSoundDef = (SoundDef*)m_pWndTree->GetItemData(hItem);
		if(!pSoundDef)
			return false;

		int iAudioGroupNum = pSoundDef->GetAudioGroupNum();
		for (int j=0; j<iAudioGroupNum; ++j)
		{
			AudioGroup* pAudioGroup = pSoundDef->GetAudioGroupByIndex(j);
			if(!pAudioGroup)
				return false;
			int iAudioTemplateNum = pAudioGroup->GetAudioTemplateNum();
			if(pAudioGroup->IsSingle())
			{
				HTREEITEM hAudioItem = m_pWndTree->InsertItem(pAudioGroup->GetAudioTemplateByIndex(0)->GetAudio()->GetPath(), IMAGE_SOUNDDEF_AUDIO, IMAGE_SOUNDDEF_AUDIO, hItem);
				m_pWndTree->SetItemData(hAudioItem, (DWORD_PTR)pAudioGroup->GetAudioTemplateByIndex(0));
				m_pWndTree->SetExtraData(hAudioItem, SOUNDDEF_ITEM_AUDIO);
			}
			else
			{
				HTREEITEM hAudioGroupItem = m_pWndTree->InsertItem("group", IMAGE_SOUNDDEF_AUDIO_GROUP, IMAGE_SOUNDDEF_AUDIO_GROUP, hItem);
				m_pWndTree->SetItemData(hAudioGroupItem, (DWORD_PTR)pAudioGroup);
				m_pWndTree->SetExtraData(hAudioGroupItem, SOUNDDEF_ITEM_AUDIO_GROUP);
				for (int k=0; k<iAudioTemplateNum; ++k)
				{
					HTREEITEM hAudioItem = m_pWndTree->InsertItem(pAudioGroup->GetAudioTemplateByIndex(k)->GetAudio()->GetPath(), IMAGE_SOUNDDEF_AUDIO, IMAGE_SOUNDDEF_AUDIO, hAudioGroupItem);
					m_pWndTree->SetItemData(hAudioItem, (DWORD_PTR)pAudioGroup->GetAudioTemplateByIndex(k));
					m_pWndTree->SetExtraData(hAudioItem, SOUNDDEF_ITEM_AUDIO);
				}
			}
		}
	}
	return true;
}

bool CSoundDefDisplay::RebuildAudioGroupItems(HTREEITEM hItem)
{
	m_pWndTree->DeleteChildren(hItem);
	if(m_pWndTree->GetExtraData(hItem) == SOUNDDEF_ITEM_AUDIO_GROUP)
	{
		AudioGroup* pAudioGroup = (AudioGroup*)m_pWndTree->GetItemData(hItem);
		if(!pAudioGroup)
			return false;

		int iAudioTemplateNum = pAudioGroup->GetAudioTemplateNum();
		for (int k=0; k<iAudioTemplateNum; ++k)
		{
			HTREEITEM hAudioItem = m_pWndTree->InsertItem(pAudioGroup->GetAudioTemplateByIndex(k)->GetAudio()->GetPath(), IMAGE_SOUNDDEF_AUDIO, IMAGE_SOUNDDEF_AUDIO, hItem);
			m_pWndTree->SetItemData(hAudioItem, (DWORD_PTR)pAudioGroup->GetAudioTemplateByIndex(k));
			m_pWndTree->SetExtraData(hAudioItem, SOUNDDEF_ITEM_AUDIO);
		}
	}
	return true;
}