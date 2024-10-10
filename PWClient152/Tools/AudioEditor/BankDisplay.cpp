#include "stdafx.h"
#include "BankDisplay.h"
#include "FAudio.h"
#include "FAudioBank.h"
#include "FAudioBankArchive.h"
#include "Global.h"
#include "MainFrm.h"
#include "Engine.h"
#include "FAudioManager.h"
#include "Project.h"
#include "resource.h"

using AudioEngine::AudioBank;
using AudioEngine::AudioBankArchive;

CBankDisplay::CBankDisplay(void)
: m_pWndTree(0)
{
}

CBankDisplay::~CBankDisplay(void)
{
}

bool CBankDisplay::BuildImageList(CImageList& imageList)
{
	imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_BANK));
	imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_BANK_ARCHIVE));
	imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_AUDIO));	
	return true;
}

bool CBankDisplay::UpdateTree(CTreeCtrlEx* pWndTree)
{
	if(!pWndTree)
		return false;
	m_pWndTree = pWndTree;

	m_pWndTree->DeleteAllItems();
	if(!Engine::GetInstance().GetCurProject())
		return false;
	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		return false;
	int iBankNum = pAudioManager->GetAudioBankNum();
	for (int i=0; i<iBankNum; ++i)
	{
		AudioBank* pAudioBank = pAudioManager->GetAudioBankByIndex(i);
		if(!pAudioBank)
			return false;		
		HTREEITEM hItem = m_pWndTree->InsertItem(pAudioBank->GetName(), IMAGE_BANK, IMAGE_BANK, TVI_ROOT, TVI_SORT);
		m_pWndTree->SetItemData(hItem, (DWORD_PTR)pAudioBank);
		m_pWndTree->SetExtraData(hItem, BANK_ITEM_BANK);
		if(!BuildTree(hItem))
			return false;
	}
	return true;
}

bool CBankDisplay::BuildTree(HTREEITEM hItem)
{
	m_pWndTree->DeleteChildren(hItem);
	BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_pWndTree->GetExtraData(hItem);
	if(type == BANK_ITEM_ARCHIVE || type == BANK_ITEM_BANK)
	{
		int iArchiveNum = 0;
		if(type == BANK_ITEM_BANK)
		{
			iArchiveNum = ((AudioBank*)m_pWndTree->GetItemData(hItem))->GetAudioBankArchiveNum();
		}
		else
		{
			iArchiveNum = ((AudioBankArchive*)m_pWndTree->GetItemData(hItem))->GetAudioBankArchiveNum();
		}
		for (int i=0; i<iArchiveNum; ++i)
		{
			AudioBankArchive* pArchive = 0;
			if(type == BANK_ITEM_BANK)
			{
				pArchive = ((AudioBank*)m_pWndTree->GetItemData(hItem))->GetAudioBankArchiveByIndex(i);
			}
			else
			{
				pArchive = ((AudioBankArchive*)m_pWndTree->GetItemData(hItem))->GetAudioBankArchiveByIndex(i);
			}
			if(!pArchive)
				return false;
			HTREEITEM hNewItem = m_pWndTree->InsertItem(pArchive->GetName(), IMAGE_BANK_ARCHIVE, IMAGE_BANK_ARCHIVE, hItem, TVI_SORT);
			m_pWndTree->SetItemData(hNewItem, (DWORD_PTR)pArchive);
			m_pWndTree->SetExtraData(hNewItem, BANK_ITEM_ARCHIVE);
			if(!BuildTree(hNewItem))
				return false;
		}

		int iAudioNum = 0;
		if(type == BANK_ITEM_BANK)
		{
			iAudioNum = ((AudioBank*)m_pWndTree->GetItemData(hItem))->GetAudioNum();
		}
		else
		{
			iAudioNum = ((AudioBankArchive*)m_pWndTree->GetItemData(hItem))->GetAudioNum();
		}

		for (int i=0; i<iAudioNum; ++i)
		{
			Audio* pAudio = 0;
			if(type == BANK_ITEM_BANK)
			{
				pAudio = ((AudioBank*)m_pWndTree->GetItemData(hItem))->GetAudioByIndex(i);
			}
			else
			{
				pAudio = ((AudioBankArchive*)m_pWndTree->GetItemData(hItem))->GetAudioByIndex(i);
			}
			if(!pAudio)
				return false;
			HTREEITEM hNewItem = m_pWndTree->InsertItem(pAudio->GetName(), IMAGE_BANK_AUDIO, IMAGE_BANK_AUDIO, hItem, TVI_SORT);
			m_pWndTree->SetItemData(hNewItem, (DWORD_PTR)pAudio);
			m_pWndTree->SetExtraData(hNewItem, BANK_ITEM_AUDIO);
		}
	}
	return true;
}