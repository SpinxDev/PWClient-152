// workspace2.cpp : implementation of the CSoundDefBar class
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "SoundDefBar.h"
#include "DlgEditName.h"
#include "Engine.h"
#include "Project.h"
#include "FSoundDefManager.h"
#include "FSoundDef.h"
#include "FAudio.h"
#include "FSoundDefArchive.h"
#include "PropertyBar.h"
#include "MainFrm.h"
#include "Global.h"
#include "FSoundInstance.h"
#include "FEventProject.h"
#include "DlgSelectAudio.h"
#include "WndDefine.h"
#include "AudioEditorView.h"
#include "FSoundInstanceTemplate.h"
#include "FEvent.h"
#include "FAudioTemplate.h"
#include "FAudioGroup.h"
#include "../AudioEngine/xml/xmlcommon.h"
#include "DlgSoundDefPropSet.h"
#include "Render.h"
#include "DlgSelectSoundDef.h"
#include <AFileDialogWrapper.h>

using AudioEngine::EventProject;
using AudioEngine::AudioGroup;
using AudioEngine::AudioTemplate;
using AudioEngine::SOUND_INSTANCE_PROPERTY;
using AudioEngine::PLAY_NUM_ALL;
using AudioEngine::EventMap;
using AudioEngine::EventLayer;

#define MENU_NEW_ARCHIVE			6001
#define MENU_NEW_EMPTY_SOUNDDEF		6002
#define MENU_DELETE_SOUNDDEF		6003
#define MENU_DELETE_ARCHIVE			6004
#define MENU_DELETE_AUDIO			6005
#define MENU_ADD_AUDIO				6006
#define MENU_REFRESH				6007
#define MENU_ADD_AUDIO_GROUP		6008
#define MENU_DELETE_AUDIO_GROUP		6009
#define MENU_SAVE_PRESET			6010
#define MENU_LOAD_PRESET			6011
#define MENU_MOVE_UP_AUDIO			6012
#define MENU_MOVE_DOWN_AUDIO		6013
#define MENU_MOVE_UP_AUDIO_GROUP	6014
#define MENU_MOVE_DOWN_AUDIO_GROUP	6015
#define MENU_BATCH_NEW_SOUNDDEF		6016
#define MENU_BATCH_DELETE_SOUNDDEF	6017

const int nBorderSize = 0;
const int iPlayWndHeight = 70;

BEGIN_MESSAGE_MAP(CSoundDefBar, CTreeBar)
	//{{AFX_MSG_MAP(CSoundDefBar)

	ON_COMMAND(MENU_NEW_ARCHIVE, &CSoundDefBar::OnNewArchive)
	ON_COMMAND(MENU_NEW_EMPTY_SOUNDDEF, &CSoundDefBar::OnNewEmptySoundDef)
	ON_COMMAND(MENU_BATCH_NEW_SOUNDDEF, &CSoundDefBar::OnBatchNewSoundDef)
	ON_COMMAND(MENU_DELETE_ARCHIVE, &CSoundDefBar::OnDeleteArchive)
	ON_COMMAND(MENU_DELETE_SOUNDDEF, &CSoundDefBar::OnDeleteSoundDef)
	ON_COMMAND(MENU_BATCH_DELETE_SOUNDDEF, &CSoundDefBar::OnBatchDeleteSoundDef)
	ON_COMMAND(MENU_DELETE_AUDIO, &CSoundDefBar::OnDeleteAudio)
	ON_COMMAND(MENU_ADD_AUDIO, &CSoundDefBar::OnAddAudio)
	ON_COMMAND(MENU_REFRESH, &CSoundDefBar::OnRefresh)
	ON_COMMAND(MENU_ADD_AUDIO_GROUP, &CSoundDefBar::OnAddAudioGroup)
	ON_COMMAND(MENU_DELETE_AUDIO_GROUP, &CSoundDefBar::OnDeleteAudioGroup)
	ON_COMMAND(MENU_SAVE_PRESET, OnSavePreset)
	ON_COMMAND(MENU_LOAD_PRESET, OnLoadPreset)
	ON_COMMAND(MENU_MOVE_UP_AUDIO, OnMoveUpAudio)
	ON_COMMAND(MENU_MOVE_DOWN_AUDIO, OnMoveDownAudio)
	ON_COMMAND(MENU_MOVE_UP_AUDIO_GROUP, OnMoveUpAudioGroup)
	ON_COMMAND(MENU_MOVE_DOWN_AUDIO_GROUP, OnMoveDownAudioGroup)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_CTRL, OnTreeKeyDown)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_PLAY_PLAY, OnClickPlayBtn)
	ON_MESSAGE(WM_PLAY_STOP, OnClickStopBtn)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundDefBar

/////////////////////////////////////////////////////////////////////////////
// CSoundDefBar construction/destruction

CSoundDefBar::CSoundDefBar()
: m_hRClickItem(0)
{
	m_pWndPlay = new CWndPlay;
	m_bPlaying = false;
	m_pSoundInstance = 0;
	m_pSoundInstanceTemplate = 0;
	m_pLastPlaySoundDef = 0;
	m_bNeedRecreateSoundInstance = false;
}

CSoundDefBar::~CSoundDefBar()
{	
	delete m_pWndPlay;
	m_pWndPlay = 0;
	if(m_pSoundInstance)
	{
		m_pSoundInstance->Stop(true);
	}
	delete m_pSoundInstanceTemplate;
	delete m_pSoundInstance;
}

void CSoundDefBar::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(!Engine::GetInstance().GetCurProject())
		return;
	CPoint pos;
	GetCursorPos(&pos);
	m_wndTree.ScreenToClient(&pos);
	m_hRClickItem = m_wndTree.HitTest(pos);
	CMenu menu;
	menu.CreatePopupMenu();
	if(!m_hRClickItem)
	{
		menu.AppendMenu(MF_STRING, MENU_NEW_ARCHIVE, "���Ŀ¼");
		m_wndTree.ClientToScreen(&pos);
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_REFRESH, "ˢ��");
		menu.TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
		return;
	}
	if(!m_wndTree.SelectItem(m_hRClickItem))
		return;

	//////////////////////////////////////////////////////////////////////////
	// �Ҽ��˵�����
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
	if(type == SOUNDDEF_ITEM_ARCHIVE)
	{
		menu.AppendMenu(MF_STRING, MENU_NEW_EMPTY_SOUNDDEF, "��ӿ���������");
		menu.AppendMenu(MF_STRING, MENU_BATCH_NEW_SOUNDDEF, "����������������");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_NEW_ARCHIVE, "���Ŀ¼");
		menu.AppendMenu(MF_STRING, MENU_DELETE_ARCHIVE, "ɾ��Ŀ¼");
	}
	else if(type == SOUNDDEF_ITEM_SOUNDDEF)
	{
		menu.AppendMenu(MF_STRING, MENU_ADD_AUDIO, "�����Ƶ�ļ�");
		menu.AppendMenu(MF_STRING, MENU_ADD_AUDIO_GROUP, "�����Ƶ�ļ���");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_DELETE_SOUNDDEF, "ɾ����������");
		menu.AppendMenu(MF_STRING, MENU_BATCH_DELETE_SOUNDDEF, "����ɾ����������...");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_SAVE_PRESET, "����Ԥ��...");
		menu.AppendMenu(MF_STRING, MENU_LOAD_PRESET, "����Ԥ��...");
	}
	else if(type == SOUNDDEF_ITEM_AUDIO)
	{
		menu.AppendMenu(MF_STRING, MENU_DELETE_AUDIO, "ɾ����Ƶ�ļ�");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_MOVE_UP_AUDIO, "����");
		menu.AppendMenu(MF_STRING, MENU_MOVE_DOWN_AUDIO, "����");
	}
	else if(type == SOUNDDEF_ITEM_AUDIO_GROUP)
	{
		menu.AppendMenu(MF_STRING, MENU_ADD_AUDIO, "�����Ƶ�ļ�");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_DELETE_AUDIO_GROUP, "ɾ����Ƶ�ļ���");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_MOVE_UP_AUDIO_GROUP, "����");
		menu.AppendMenu(MF_STRING, MENU_MOVE_DOWN_AUDIO_GROUP, "����");
	}
	m_wndTree.ClientToScreen(&pos);
	menu.TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
	//////////////////////////////////////////////////////////////////////////
	*pResult = 0;
}

void CSoundDefBar::newArchive(HTREEITEM hItem)
{
	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		return;
	SoundDefArchive* pSoundDefArchive = 0;
	CDlgEditName dlg("Ŀ¼����");
_EditName:
	if(IDOK != dlg.DoModal())
		return;
	if(!hItem)
	{
		if(pSoundDefManager->IsArchiveExist(dlg.GetName()))
		{
			GF_Log(LOG_WARNING, "�Ѵ���Ŀ¼��%s��", dlg.GetName());
			goto _EditName;
		}
	}
	else
	{
		pSoundDefArchive = (SoundDefArchive*)m_wndTree.GetItemData(hItem);
		if(pSoundDefManager->IsArchiveExist(pSoundDefArchive, dlg.GetName()))
		{
			GF_Log(LOG_WARNING, "�Ѵ���Ŀ¼��%s��", dlg.GetName());
			goto _EditName;
		}
	}

	SoundDefArchive* pNewArchive = 0;
	if(!hItem)
	{
		pNewArchive = pSoundDefManager->CreateSoundDefArchive(dlg.GetName());
	}
	else
	{		
		pNewArchive = pSoundDefManager->CreateSoundDefArchive(pSoundDefArchive, dlg.GetName());
	}

	if(!pNewArchive)
		return;
	HTREEITEM hChildItem = m_wndTree.InsertItem(dlg.GetName(), IMAGE_SOUNDDEF_ARCHIVE, IMAGE_SOUNDDEF_ARCHIVE, hItem, TVI_SORT);
	m_wndTree.SetItemData(hChildItem, (DWORD_PTR)pNewArchive);
	m_wndTree.SetExtraData(hChildItem, SOUNDDEF_ITEM_ARCHIVE);
	m_wndTree.Expand(hItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CSoundDefBar::OnNewArchive()
{
	newArchive(m_hRClickItem);
}

void CSoundDefBar::OnNewEmptySoundDef()
{
	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		return;
	SoundDefArchive* pSoundDefArchive = (SoundDefArchive*)m_wndTree.GetItemData(m_hRClickItem);
	CDlgEditName dlg("������������");
_EditName:
	if(IDOK != dlg.DoModal())
		return;	
	if(pSoundDefManager->IsSoundDefExist(pSoundDefArchive, dlg.GetName()))
	{		
		GF_Log(LOG_WARNING, "�Ѵ����������塰%s��", dlg.GetName());
		goto _EditName;
	}
	SoundDef* pSoundDef = pSoundDefManager->CreateSoundDef(pSoundDefArchive, dlg.GetName());
	if(!pSoundDef)
		return;
	HTREEITEM hItem = m_wndTree.InsertItem(dlg.GetName(), IMAGE_SOUNDDEF, IMAGE_SOUNDDEF, m_hRClickItem, TVI_SORT);
	m_wndTree.SetItemData(hItem, (DWORD_PTR)pSoundDef);
	m_wndTree.SetExtraData(hItem, SOUNDDEF_ITEM_SOUNDDEF);
	m_wndTree.Expand(m_hRClickItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CSoundDefBar::OnBatchNewSoundDef()
{
	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		return;
	SoundDefArchive* pSoundDefArchive = (SoundDefArchive*)m_wndTree.GetItemData(m_hRClickItem);
	CDlgSelectAudio dlgSelAudio(true);	
	if(IDOK != dlgSelAudio.DoModal())
		return;
	AudioList listAudio = dlgSelAudio.GetSelectAudioList();
	if(listAudio.size() == 0)
		return;
	CDlgSoundDefPropSet dlgSoundDefPropSet;
	dlgSoundDefPropSet.DoModal();
	SOUND_DEF_PROPERTY prop = dlgSoundDefPropSet.GetProp();
	AudioList::const_iterator it = listAudio.begin();
	for (; it!=listAudio.end(); ++it)
	{
		Audio* pAudio = *it;
		CString csName(pAudio->GetName());
		csName = csName.Left(csName.ReverseFind('.'));
		if(pSoundDefManager->IsSoundDefExist(pSoundDefArchive, csName))
		{		
			GF_Log(LOG_WARNING, "�Ѵ����������塰%s��", csName);
			continue;
		}
		SoundDef* pSoundDef = pSoundDefManager->CreateSoundDef(pSoundDefArchive, csName);
		if(!pSoundDef)
			return;
		pSoundDef->SetProperty(prop);
		HTREEITEM hSoundDefItem = m_wndTree.InsertItem(csName, IMAGE_SOUNDDEF, IMAGE_SOUNDDEF, m_hRClickItem, TVI_SORT);
		m_wndTree.SetItemData(hSoundDefItem, (DWORD_PTR)pSoundDef);
		m_wndTree.SetExtraData(hSoundDefItem, SOUNDDEF_ITEM_SOUNDDEF);

		AudioGroup* pAudioGroup = pSoundDef->AddAudioGroup();
		AudioTemplate* pAudioTemplate = pAudioGroup->AddAudioTemplate(pAudio);

		HTREEITEM hAudioTemplateItem = m_wndTree.InsertItem(pAudio->GetPath(), IMAGE_SOUNDDEF_AUDIO, IMAGE_SOUNDDEF_AUDIO, hSoundDefItem);
		m_wndTree.SetItemData(hAudioTemplateItem, (DWORD_PTR)pAudioTemplate);
		m_wndTree.SetExtraData(hAudioTemplateItem, SOUNDDEF_ITEM_AUDIO);
	}
	m_wndTree.Expand(m_hRClickItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CSoundDefBar::OnDeleteArchive()
{
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hSelItem);
	if(type != SOUNDDEF_ITEM_ARCHIVE)
		return;
	SoundDefArchive* pArchive = (SoundDefArchive*)m_wndTree.GetItemData(hSelItem);
	if(!pArchive)
		return;
	GF_Log(LOG_NORMAL, "ɾ��Ŀ¼...");
	char szInfo[1024] = {0};
	sprintf_s(szInfo, "ȷ��ɾ��Ŀ¼\"%s\"��", pArchive->GetName());
	if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
		return;
	
	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		return;	
	if(pArchive->GetSoundDefNum() || pArchive->GetSoundDefArchiveNum())
	{
		sprintf_s(szInfo, "Ŀ¼\"%s\"��Ϊ�գ�ȷ��ɾ����", pArchive->GetName());
		if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
			return;
	}

	SoundDefList listSoundDef;
	getSubSoundDef(pArchive, listSoundDef);
	SoundDefList::const_iterator it = listSoundDef.begin();
	for (; it!=listSoundDef.end(); ++it)
	{
		if(!ifCanDelete(*it))
			return;
	}

	CString csName = pArchive->GetName();
	if(!pSoundDefManager->DeleteSoundDefArchive(pArchive))
	{
		GF_Log(LOG_ERROR, "ɾ����������Ŀ¼��%s��ʧ�ܡ�", csName);
		return;
	}
	m_wndTree.DeleteItem(hSelItem);
	Engine::GetInstance().GetCurProject()->SetModified(true);

	GF_Log(LOG_NORMAL, "�ɹ�ɾ��Ŀ¼��%s��", csName);
}

void CSoundDefBar::OnDeleteSoundDef()
{
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hSelItem);
	if(type != SOUNDDEF_ITEM_SOUNDDEF)
		return;
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hSelItem);
	if(!pSoundDef)
		return;

	GF_Log(LOG_NORMAL, "ɾ����������...");

	char szInfo[1024] = {0};
	sprintf_s(szInfo, "ȷ��ɾ����������\"%s\"��", pSoundDef->GetName());
	if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
		return;

	if(!deleteSoundDef(pSoundDef))
		return;
	m_wndTree.DeleteItem(hSelItem);	
}

void CSoundDefBar::OnBatchDeleteSoundDef()
{
	CDlgSelectSoundDef dlg(true, "ѡ������ɾ������������");
	if(IDOK != dlg.DoModal())
		return;
	SoundDefList listSoundDef = dlg.GetSelectSoundDefList();
	SoundDefList::const_iterator it = listSoundDef.begin();
	for (; it!=listSoundDef.end(); ++it)
	{
		SoundDef* pSoundDef = *it;
		if(!pSoundDef)
			continue;
		if(!deleteSoundDef(pSoundDef))
			continue;
	}
	UpdateTree();
}

bool CSoundDefBar::deleteSoundDef(SoundDef* pSoundDef)
{
	if(!ifCanDelete(pSoundDef))
		return false;
	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		return false;

	CString csName = pSoundDef->GetName();
	if(!pSoundDefManager->DeleteSoundDef(pSoundDef))
	{
		GF_Log(LOG_ERROR, "ɾ���������塰%s��ʧ��", csName);
		return false;
	}
	GF_Log(LOG_NORMAL, "�ɹ�ɾ���������塰%s��", csName);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	return true;
}

bool CSoundDefBar::UpdateTree()
{
	if(!m_soundDefDisplay.UpdateTree(&m_wndTree))
		return false;
	return true;
}

void CSoundDefBar::OnRefresh()
{
	UpdateTree();
}

void CSoundDefBar::OnTreeLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bDragging)
		return;
	m_bDragging = false;
	CImageList::DragLeave(&m_wndTree);
	CImageList::EndDrag();
	ReleaseCapture();
	delete m_pDragImage;
	m_pDragImage = 0;

	m_wndTree.SelectDropTarget(NULL);
	if(m_hItemDragSrc == m_hItemDragDest)
		goto _end;

	m_wndTree.Expand(m_hItemDragDest, TVE_EXPAND);
	if(m_wndTree.IsChildItem(m_hItemDragDest, m_hItemDragSrc))
		return;

	//////////////////////////////////////////////////////////////////////////
	// �������ݴ������

	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		goto _end;
	SOUNDDEF_ITEM_TYPE dest_type = SOUNDDEF_ITEM_NONE;
	SOUNDDEF_ITEM_TYPE src_type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hItemDragSrc);
	if(src_type == SOUNDDEF_ITEM_AUDIO)
		goto _end;
	if(!m_hItemDragDest)
	{
		if(src_type != SOUNDDEF_ITEM_ARCHIVE)
			goto _end;
		SoundDefArchive* pArchive = (SoundDefArchive*)m_wndTree.GetItemData(m_hItemDragSrc);
		if(!pSoundDefManager->MoveArchive(pArchive))
		{
			GF_Log(LOG_ERROR, "�ƶ���������Ŀ¼��%s��ʧ�ܡ�", pArchive->GetName());
			goto _end;
		}
		GF_Log(LOG_NORMAL, "�ɹ��ƶ���������Ŀ¼��%s������Ŀ¼", pArchive->GetName());
		UpdateTree();
	}
	else
	{
		dest_type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hItemDragDest);
		if(dest_type != SOUNDDEF_ITEM_ARCHIVE)
			goto _end;
		SoundDefArchive* pDestArchive = (SoundDefArchive*)m_wndTree.GetItemData(m_hItemDragDest);		
		if(src_type == SOUNDDEF_ITEM_SOUNDDEF)
		{
			SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(m_hItemDragSrc);
			if(!pSoundDefManager->MoveSoundDef(pSoundDef, pDestArchive))
			{
				GF_Log(LOG_ERROR, "�ƶ��������塰%s����Ŀ¼��%s��ʧ�ܡ�", pSoundDef->GetName(), pDestArchive->GetName());
				goto _end;
			}
			GF_Log(LOG_NORMAL, "�ɹ��ƶ��������塰%s����Ŀ¼��%s��", pSoundDef->GetName(), pDestArchive->GetName());
		}
		else if(src_type == SOUNDDEF_ITEM_ARCHIVE)
		{
			SoundDefArchive* pArchive = (SoundDefArchive*)m_wndTree.GetItemData(m_hItemDragSrc);
			if(!pSoundDefManager->MoveArchive(pArchive, pDestArchive))
			{
				GF_Log(LOG_ERROR, "�ƶ���������Ŀ¼��%s����Ŀ¼��%s��ʧ�ܡ�", pArchive->GetName(), pDestArchive->GetName());
				goto _end;
			}
			GF_Log(LOG_NORMAL, "�ɹ��ƶ���������Ŀ¼��%s����Ŀ¼��%s��", pArchive->GetName(), pDestArchive->GetName());
		}
		m_wndTree.DeleteItem(m_hItemDragSrc);
		m_wndTree.DeleteChildren(m_hItemDragDest);
		if(!m_soundDefDisplay.BuildTree(m_hItemDragDest))
			goto _end;		
	}
	Engine::GetInstance().GetCurProject()->SetModified(true);
	//////////////////////////////////////////////////////////////////////////
_end:
	KillTimer(m_nScrollTimerID);
}

void CSoundDefBar::OnTreeSelectChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPropertyBar* pWndPropBar = GF_GetMainFrame()->GetPropertyBar();
	if(!pWndPropBar)
		return;
	PROP_TYPE prop_type = PROP_NONE;
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		return;
	GF_GetView()->SetCurView(VIEW_SOUNDDEF);
	GF_GetView()->GetSoundDefView()->SetTreeItem(hItem);
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type == SOUNDDEF_ITEM_SOUNDDEF)
	{
		SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hItem);
		if(!pSoundDef)
			return;
		SOUND_DEF_PROPERTY prop;
		pSoundDef->GetProperty(prop);
		prop_type = PROP_SOUNDDEF;
		GF_Log(LOG_NORMAL, "ѡ���������塰%s��", pSoundDef->GetName());
	}
	else if(type == SOUNDDEF_ITEM_ARCHIVE)
	{
		SoundDefArchive* pSoundDefArchive = (SoundDefArchive*)m_wndTree.GetItemData(hItem);
		if(!pSoundDefArchive)
			return;
		prop_type = PROP_SOUNDDEF_ARCHIVE;
		GF_Log(LOG_NORMAL, "ѡ����������Ŀ¼��%s��", pSoundDefArchive->GetName());
	}
	else if(type == SOUNDDEF_ITEM_AUDIO)
	{
		prop_type = PROP_SOUNDDEF_AUDIO;
		GF_Log(LOG_NORMAL, "ѡ����Ƶ�ļ�");
	}
	else if(type == SOUNDDEF_ITEM_AUDIO_GROUP)
	{
		prop_type = PROP_SOUNDDEF_AUDIO_GROUP;
		GF_Log(LOG_NORMAL, "ѡ����Ƶ�ļ���");
	}

	pWndPropBar->BuildProperty(prop_type, hItem);

	*pResult = 0;
}

BOOL CSoundDefBar::SetItemText(HTREEITEM hItem, LPCTSTR lpszText)
{
	if(!hItem || !lpszText || !lpszText[0])
		return FALSE;
	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		return FALSE;
	DWORD_PTR pData = m_wndTree.GetItemData(hItem);
	if(!pData)
		return FALSE;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type == SOUNDDEF_ITEM_SOUNDDEF)
	{
		SoundDef* pSoundDef = (SoundDef*)pData;
		if(pSoundDefManager->IsSoundDefExist(pSoundDef->GetParentArchive(), lpszText))
		{
			GF_Log(LOG_WARNING, "�Ѵ����������塰%s��", lpszText);
			return FALSE;
		}
		if(!CTreeBar::SetItemText(hItem, lpszText))
			return FALSE;
		pSoundDef->SetName(lpszText);
	}
	else if(type == SOUNDDEF_ITEM_ARCHIVE)
	{
		bool bExist = false;
		SoundDefArchive* pSoundDefArchive = (SoundDefArchive*)pData;
		if(pSoundDefArchive->GetParentArchive())
		{
			if(pSoundDefManager->IsArchiveExist(pSoundDefArchive->GetParentArchive(), lpszText))
				bExist = true;
		}
		else
		{
			if(pSoundDefManager->IsArchiveExist(lpszText))
				bExist = true;
		}
		if(bExist)
		{
			GF_Log(LOG_WARNING, "�Ѵ���Ŀ¼��%s��", lpszText);
			return FALSE;
		}
		if(!CTreeBar::SetItemText(hItem, lpszText))
			return FALSE;
		pSoundDefArchive->SetName(lpszText);
	}
	Engine::GetInstance().GetCurProject()->SetModified(true);
	return TRUE;
}
void CSoundDefBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CTreeBar::OnPaint()
	CTreeBar::OnPaint();
	if(!GF_GetView()->GetSoundDefView()->IsWindowVisible())
	{
		GF_GetView()->SetCurView(VIEW_SOUNDDEF);
		HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
		if(!hSelItem)
			return;
		m_wndTree.SelectItem(m_wndTree.GetRootItem());
		m_wndTree.SelectItem(hSelItem);
	}
}

void CSoundDefBar::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	m_pWndPlay->SetWindowPos(NULL, nBorderSize, cy - iPlayWndHeight + nBorderSize, 
		cx - 2 * nBorderSize, iPlayWndHeight - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndTree.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize - iPlayWndHeight,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

int CSoundDefBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rtClient;
	GetClientRect(&rtClient);
	CRect rtPlay = rtClient;
	int iPlayHight = iPlayWndHeight;
	rtPlay.top = rtPlay.bottom - iPlayHight;

	if(!m_soundDefDisplay.BuildImageList(m_ImageList))
		return -1;
	if(!m_pWndPlay->Create(this, rtPlay))
		return -1;

	return 0;
}

LRESULT CSoundDefBar::OnClickPlayBtn(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		return 0;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type == SOUNDDEF_ITEM_SOUNDDEF)
	{
		SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hItem);
		if(!pSoundDef)
			return 0;

		if(m_bPlaying)
			return 0;

		GF_Log(LOG_NORMAL, "������Ű�ť");
		if(!m_bPlaying)
			m_bPlaying = true;
		m_pWndPlay->SetPlayButtonState(true);
		EventProject* pEventProject = Engine::GetInstance().GetCurProject()->GetEventProject();


		if(!m_pSoundInstance || m_pLastPlaySoundDef != pSoundDef || m_bNeedRecreateSoundInstance)
		{
			SOUND_INSTANCE_PROPERTY prop;
			delete m_pSoundInstanceTemplate;
			m_pSoundInstanceTemplate = new SoundInstanceTemplate;
			if(!m_pSoundInstanceTemplate->Init(pEventProject))
			{
				delete m_pSoundInstanceTemplate;
				m_pSoundInstanceTemplate = 0;
				goto _failed;
			}
			if(!m_pSoundInstanceTemplate->SetSoundDef(pSoundDef))
			{
				delete m_pSoundInstanceTemplate;
				m_pSoundInstanceTemplate = 0;
				goto _failed;
			}	
			m_pSoundInstanceTemplate->GetProperty(prop);
			prop.playNum = PLAY_NUM_ALL;
			m_pSoundInstanceTemplate->SetProperty(prop);
			delete m_pSoundInstance;
			m_pSoundInstance = new SoundInstance;	
			if(!m_pSoundInstance->Init(m_pSoundInstanceTemplate, pEventProject))
			{
				delete m_pSoundInstanceTemplate;
				m_pSoundInstanceTemplate = 0;
				delete m_pSoundInstance;
				m_pSoundInstance = 0;
				goto _failed;
			}
			m_bNeedRecreateSoundInstance = false;
		}

		m_pLastPlaySoundDef = pSoundDef;
		if(!m_pSoundInstance->PlayForSoundDef())
		{
			GF_Log(LOG_ERROR, "�������塰%s������ʧ�ܡ�", pSoundDef->GetName());
			goto _failed;
		}
		m_pSoundInstance->AddListener(this);
		return 0;
	}
	else if (type == SOUNDDEF_ITEM_AUDIO_GROUP)
	{
		AudioGroup* pAudioGroup = (AudioGroup*)m_wndTree.GetItemData(hItem);
		if(!pAudioGroup)
			return 0;

		if(m_bPlaying)
			return 0;

		GF_Log(LOG_NORMAL, "������Ű�ť");
		if(!m_bPlaying)
			m_bPlaying = true;
		m_pWndPlay->SetPlayButtonState(true);
		EventProject* pEventProject = Engine::GetInstance().GetCurProject()->GetEventProject();

		SOUND_INSTANCE_PROPERTY prop;
		delete m_pSoundInstanceTemplate;
		m_pSoundInstanceTemplate = new SoundInstanceTemplate;
		if(!m_pSoundInstanceTemplate->Init(pEventProject))
		{
			delete m_pSoundInstanceTemplate;
			m_pSoundInstanceTemplate = 0;
			goto _failed;
		}
		SoundDef* pSoundDef = new SoundDef();
		if(!pSoundDef->Init(pEventProject))
		{
			delete pSoundDef;
			goto _failed;
		}
		AudioGroup* pNewAudioGroup = pSoundDef->AddAudioGroup();
		for (int i=0; i<pAudioGroup->GetAudioTemplateNum(); ++i)
		{
			pNewAudioGroup->AddAudioTemplate(pAudioGroup->GetAudioTemplateByIndex(i)->GetAudio());
		}
		pNewAudioGroup->SetSingle(pAudioGroup->IsSingle());
		pNewAudioGroup->SetPlayMode(pAudioGroup->GetPlayMode());
		if(!m_pSoundInstanceTemplate->SetSoundDef(pSoundDef))
		{
			delete m_pSoundInstanceTemplate;
			m_pSoundInstanceTemplate = 0;
			goto _failed;
		}	
		m_pSoundInstanceTemplate->GetProperty(prop);
		prop.playNum = PLAY_NUM_ALL;
		m_pSoundInstanceTemplate->SetProperty(prop);
		delete m_pSoundInstance;
		m_pSoundInstance = new SoundInstance;	
		if(!m_pSoundInstance->Init(m_pSoundInstanceTemplate, pEventProject))
		{
			delete m_pSoundInstanceTemplate;
			m_pSoundInstanceTemplate = 0;
			delete m_pSoundInstance;
			m_pSoundInstance = 0;
			goto _failed;
		}
		m_bNeedRecreateSoundInstance = false;

		m_pLastPlaySoundDef = pSoundDef;
		if(!m_pSoundInstance->PlayForSoundDef())
		{
			GF_Log(LOG_ERROR, "�������塰%s������ʧ�ܡ�", pSoundDef->GetName());
			goto _failed;
		}
		m_pSoundInstance->AddListener(this);
		return 0;
	}
_failed:
	m_pWndPlay->SetPlayButtonState(false);
	m_bPlaying = false;
	return 0;
}

LRESULT CSoundDefBar::OnClickStopBtn(WPARAM wParam, LPARAM lParam)
{
	if(!m_pSoundInstance)
		return 0;
	GF_Log(LOG_NORMAL, "���ֹͣ��ť");
	if(!m_pSoundInstance->Stop(true))
	{
		GF_Log(LOG_ERROR, "ֹͣ���ų���");
	}
	return 0;
}

bool CSoundDefBar::OnStop(SoundInstance* pSoundInstance)
{
	GF_Log(LOG_NORMAL, "�������岥��ֹͣ");
	m_pSoundInstance->RemoveListener(this);
	m_bPlaying = false;
	if(m_pWndPlay)
		m_pWndPlay->SetPlayButtonState(false);
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		return false;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if (type == SOUNDDEF_ITEM_AUDIO_GROUP)
	{
		delete m_pLastPlaySoundDef;
		m_pLastPlaySoundDef = 0;
	}
	return true;
}

void CSoundDefBar::OnDeleteAudio()
{
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hSelItem);
	if(type != SOUNDDEF_ITEM_AUDIO)
		return;
	HTREEITEM hParentItem = m_wndTree.GetParentItem(hSelItem);
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hParentItem);
	if(!pSoundDef)
		return;

	AudioTemplate* pAudioTemplate = (AudioTemplate*)m_wndTree.GetItemData(hSelItem);
	if(!pAudioTemplate)
		return;

	std::string strName = pAudioTemplate->GetAudio()->GetName();

	GF_Log(LOG_NORMAL, "ɾ����Ƶ�ļ�����...");

	char szInfo[1024] = {0};
	sprintf_s(szInfo, "ȷ���Ƴ������ļ�\"%s\"��", pAudioTemplate->GetAudio()->GetName());
	if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
		return;
	AudioGroup* pParentGroup = pAudioTemplate->GetParentGroup();
	int iAudioTemplateNum = pParentGroup->GetAudioTemplateNum();
	if(pParentGroup->IsSingle() || iAudioTemplateNum == 0)
	{
		if(!pSoundDef->RemoveAudioGroup(pParentGroup))
		{
			GF_Log(LOG_ERROR, "ɾ����Ƶ�ļ���ʧ�ܡ�");
			return;
		}
		SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hParentItem);
		if(type == SOUNDDEF_ITEM_AUDIO_GROUP)
			m_wndTree.DeleteItem(hParentItem);
		else
			m_wndTree.DeleteItem(hSelItem);
	}
	else
	{
		if(!pParentGroup->DeleteAudioTemplate(pAudioTemplate))
		{
			GF_Log(LOG_ERROR, "ɾ����Ƶ�ļ���%s��������ʧ�ܡ�", strName.c_str());
			return;
		}
		m_wndTree.DeleteItem(hSelItem);
	}	
	
	Engine::GetInstance().GetCurProject()->SetModified(true);
	if(!reloadEventWhenSoundDefChanged(pSoundDef))
	{
		GF_Log(LOG_ERROR, "���¼���Eventʧ��");
		return;
	}

	GF_Log(LOG_NORMAL, "�ɹ�ɾ����Ƶ�ļ����á�%s��", strName.c_str());
	RecreateSoundInstance();
}

void CSoundDefBar::OnAddAudio()
{
	if(!m_hRClickItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
	if(type != SOUNDDEF_ITEM_SOUNDDEF && type != SOUNDDEF_ITEM_AUDIO_GROUP)
		return;	

	GF_Log(LOG_NORMAL, "����ļ�...");

	CDlgSelectAudio dlg(true);
	if(IDOK != dlg.DoModal())
		return;
	SoundDef* pSoundDef = 0;
	AudioList listSelectAudio = dlg.GetSelectAudioList();
	AudioList::const_iterator it = listSelectAudio.begin();
	for (; it!=listSelectAudio.end(); ++it)
	{
		Audio* pAudio = *it;
		AudioTemplate* pAudioTemplate = 0;		
		if(type == SOUNDDEF_ITEM_SOUNDDEF)
		{
			pSoundDef = (SoundDef*)m_wndTree.GetItemData(m_hRClickItem);
			if(!pSoundDef)
				return;
			AudioGroup* pAudioGroup = pSoundDef->AddAudioGroup();
			pAudioTemplate = pAudioGroup->AddAudioTemplate(pAudio);
		}
		else
		{
			AudioGroup* pAudioGroup = (AudioGroup*)m_wndTree.GetItemData(m_hRClickItem);
			pSoundDef = (SoundDef*)m_wndTree.GetItemData(m_wndTree.GetParentItem(m_hRClickItem));
			pAudioTemplate = pAudioGroup->AddAudioTemplate(pAudio);
		}

		HTREEITEM hItem = m_wndTree.InsertItem(pAudio->GetPath(), IMAGE_SOUNDDEF_AUDIO, IMAGE_SOUNDDEF_AUDIO, m_hRClickItem);
		m_wndTree.SetItemData(hItem, (DWORD_PTR)pAudioTemplate);
		m_wndTree.SetExtraData(hItem, SOUNDDEF_ITEM_AUDIO);
		GF_Log(LOG_NORMAL, "�ɹ�����ļ���%s��", pAudio->GetName());
	}
	
	m_wndTree.Expand(m_hRClickItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	if(!reloadEventWhenSoundDefChanged(pSoundDef))
	{
		GF_Log(LOG_ERROR, "���¼���Eventʧ��");
		return;
	}	
	RecreateSoundInstance();
}

bool CSoundDefBar::ifCanDelete(SoundDef* pSoundDef)
{
	if(!pSoundDef)
		return false;
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return false;
	const EventMap& mapEvent = pEventManager->GetEventMap();
	EventMap::const_iterator it = mapEvent.begin();
	for (; it!=mapEvent.end(); ++it)
	{
		Event* pEvent = it->second;
		if(!pEvent)
			continue;
		int iLayerNum = pEvent->GetLayerNum();
		for (int i=0; i<iLayerNum; ++i)
		{
			EventLayer* pEventLayer = pEvent->GetLayerByIndex(i);
			if(!pEventLayer)
				continue;
			int iSoundDefNum = pEventLayer->GetSoundInstanceTemplateNum();
			for (int j=0; j<iSoundDefNum; ++j)
			{
				SoundInstanceTemplate* pSoundInstanceTemplate = pEventLayer->GetSoundInstanceTemplateByIndex(j);
				if(!pSoundInstanceTemplate)
					continue;
				if(pSoundInstanceTemplate->GetSoundDef() == pSoundDef)
				{
					GF_Log(LOG_WARNING, "�¼���%s���������������塰%s��������ɾ����", pEvent->GetName(), pSoundDef->GetName());
					return false;
				}
			}
		}
	}
	return true;
}

bool CSoundDefBar::reloadEventWhenSoundDefChanged(SoundDef* pSoundDef)
{
	if(!pSoundDef)
		return false;
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return false;
	const EventMap& mapEvent = pEventManager->GetEventMap();
	EventMap::const_iterator it = mapEvent.begin();
	for (; it!=mapEvent.end(); ++it)
	{
		Event* pEvent = it->second;
		if(!pEvent)
			continue;
		int iLayerNum = pEvent->GetLayerNum();
		for (int i=0; i<iLayerNum; ++i)
		{
			EventLayer* pEventLayer = pEvent->GetLayerByIndex(i);
			if(!pEventLayer)
				continue;
			int iSoundDefNum = pEventLayer->GetSoundInstanceTemplateNum();
			for (int j=0; j<iSoundDefNum; ++j)
			{
				SoundInstanceTemplate* pSoundInstanceTemplate = pEventLayer->GetSoundInstanceTemplateByIndex(j);
				if(!pSoundInstanceTemplate)
					continue;
				if(pSoundInstanceTemplate->GetSoundDef() == pSoundDef)
				{
					if(!pEvent->Reload())
						return false;
				}
			}
		}
	}
	return true;
}

void CSoundDefBar::getSubSoundDef(SoundDefArchive* pArchive, SoundDefList& listSoundDef)
{
	if(!pArchive)
		return;
	int iSoundDefNum = pArchive->GetSoundDefNum();
	for (int i=0; i<iSoundDefNum; ++i)
	{
		listSoundDef.push_back(pArchive->GetSoundDefByIndex(i));
	}

	int iArchiveNum = pArchive->GetSoundDefArchiveNum();
	for (int i=0; i<iArchiveNum; ++i)
	{
		getSubSoundDef(pArchive->GetSoundDefArchiveByIndex(i), listSoundDef);
	}
}

void CSoundDefBar::RecreateSoundInstance()
{
	m_bNeedRecreateSoundInstance = true;
}

void CSoundDefBar::OnAddAudioGroup()
{
	if(!m_hRClickItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
	if(type != SOUNDDEF_ITEM_SOUNDDEF)
		return;

	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pSoundDef)
		return;

	GF_Log(LOG_NORMAL, "����ļ�...");

	CDlgSelectAudio dlg(true);
	if(IDOK != dlg.DoModal())
		return;
	AudioGroup* pAudioGroup = pSoundDef->AddAudioGroup();
	pAudioGroup->SetSingle(false);
	HTREEITEM hGroupItem = m_wndTree.InsertItem("group", IMAGE_SOUNDDEF_AUDIO_GROUP, IMAGE_SOUNDDEF_AUDIO_GROUP, m_hRClickItem);
	m_wndTree.SetItemData(hGroupItem, (DWORD_PTR)pAudioGroup);
	m_wndTree.SetExtraData(hGroupItem, SOUNDDEF_ITEM_AUDIO_GROUP);

	AudioList listSelectAudio = dlg.GetSelectAudioList();
	AudioList::const_iterator it = listSelectAudio.begin();
	for (; it!=listSelectAudio.end(); ++it)
	{
		Audio* pAudio = *it;
		AudioTemplate* pAudioTemplate = pAudioGroup->AddAudioTemplate(pAudio);	

		HTREEITEM hItem = m_wndTree.InsertItem(pAudio->GetPath(), IMAGE_SOUNDDEF_AUDIO, IMAGE_SOUNDDEF_AUDIO, hGroupItem);
		m_wndTree.SetItemData(hItem, (DWORD_PTR)pAudioTemplate);
		m_wndTree.SetExtraData(hItem, SOUNDDEF_ITEM_AUDIO);
		GF_Log(LOG_NORMAL, "�ɹ�����ļ���%s��", pAudio->GetName());
	}	

	m_wndTree.Expand(m_hRClickItem, TVM_EXPAND);
	m_wndTree.Expand(hGroupItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	if(!reloadEventWhenSoundDefChanged(pSoundDef))
	{
		GF_Log(LOG_ERROR, "���¼���Eventʧ��");
		return;
	}	
	RecreateSoundInstance();
}

void CSoundDefBar::OnDeleteAudioGroup()
{
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hSelItem);
	if(type != SOUNDDEF_ITEM_AUDIO_GROUP)
		return;
	HTREEITEM hParentItem = m_wndTree.GetParentItem(hSelItem);
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hParentItem);
	if(!pSoundDef)
		return;

	AudioGroup* pAudioGroup = (AudioGroup*)m_wndTree.GetItemData(hSelItem);
	if(!pAudioGroup)
		return;

	GF_Log(LOG_NORMAL, "ɾ����Ƶ�ļ���...");

	char szInfo[1024] = {0};
	sprintf_s(szInfo, "ȷ���Ƴ������ļ�����");
	if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
		return;
	if(!pSoundDef->RemoveAudioGroup(pAudioGroup))
	{
		GF_Log(LOG_ERROR, "ɾ����Ƶ�ļ���ʧ�ܡ�");
		return;
	}
	m_wndTree.DeleteItem(hSelItem);	

	Engine::GetInstance().GetCurProject()->SetModified(true);
	if(!reloadEventWhenSoundDefChanged(pSoundDef))
	{
		GF_Log(LOG_ERROR, "���¼���Eventʧ��");
		return;
	}

	GF_Log(LOG_NORMAL, "�ɹ�ɾ����Ƶ�ļ���");
	RecreateSoundInstance();
}

void CSoundDefBar::OnSavePreset()
{
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pSoundDef)
		return;

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),FALSE, g_Configs.szSoundDefPresetPath, "����SoundDefԤ���ļ�","",this);
	if(IDOK != dlg.DoModal())
		return;

	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement("SoundDefPreset");
	doc.LinkEndChild(root);
	pSoundDef->SaveXML(root, true);
	TiXmlPrinter printer;
	doc.Accept(&printer);

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, dlg.GetFullPathName(), "w"))
	{
		return;
	}
	fwrite((LPVOID)printer.CStr(), printer.Size(), 1, pFile);
	fclose(pFile);
	GF_Log(LOG_NORMAL, "�ɹ�����Ԥ��%s", dlg.GetFileName());
}

void CSoundDefBar::OnLoadPreset()
{
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pSoundDef)
		return;

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),TRUE, g_Configs.szSoundDefPresetPath, "ѡ��SoundDefԤ���ļ�","",this);
	if(IDOK != dlg.DoModal())
		return;

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, dlg.GetFullPathName(), "r"))
	{
		return;
	}

	fseek(pFile, 0, SEEK_END);
	long lLength = ftell(pFile);
	char* pBuf = new char[lLength + 1];
	memset(pBuf, 0, lLength + 1);
	fseek(pFile, 0, SEEK_SET);
	fread(pBuf, lLength, 1, pFile);	
	fclose(pFile);
	pBuf[lLength] = 0;

	TiXmlDocument doc;
	doc.Parse(pBuf);
	delete [] pBuf;

	TiXmlNode* pNode = doc.FirstChild("SoundDefPreset");
	if (!pNode)
	{
		GF_Log(LOG_ERROR, "%s����SoundDefԤ���ļ�", dlg.GetFileName());
		return;
	}

	TiXmlElement* root = pNode->ToElement();
	pSoundDef->LoadXML(root->FirstChild("SoundDef")->ToElement(), true);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_Log(LOG_NORMAL, "�ɹ�����Ԥ��%s", dlg.GetFileName());
}

void CSoundDefBar::OnMoveUpAudio()
{
	if(!m_hRClickItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
	if(type != SOUNDDEF_ITEM_AUDIO)
		return;
	HTREEITEM hParentItem = m_wndTree.GetParentItem(m_hRClickItem);
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hParentItem);
	if(!pSoundDef)
		return;

	AudioTemplate* pAudioTemplate = (AudioTemplate*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pAudioTemplate)
		return;

	std::string strName = pAudioTemplate->GetAudio()->GetName();

	GF_Log(LOG_NORMAL, "������Ƶ�ļ�����...");

	AudioGroup* pParentGroup = pAudioTemplate->GetParentGroup();
	if(pParentGroup->IsSingle())
	{
		if(!pSoundDef->MoveUpAudioGroup(pParentGroup))
		{
			GF_Log(LOG_ERROR, "������Ƶ�ļ���ʧ�ܡ�");
			return;
		}
		m_soundDefDisplay.RebuildSoundDefItems(hParentItem);
	}
	else
	{
		if(!pParentGroup->MoveUpAudioTemplate(pAudioTemplate))
		{
			GF_Log(LOG_ERROR, "������Ƶ�ļ���%s��������ʧ�ܡ�", strName.c_str());
			return;
		}
		m_soundDefDisplay.RebuildAudioGroupItems(hParentItem);
	}
	
	m_wndTree.Expand(hParentItem, TVM_EXPAND);

	Engine::GetInstance().GetCurProject()->SetModified(true);
	if(!reloadEventWhenSoundDefChanged(pSoundDef))
	{
		GF_Log(LOG_ERROR, "���¼���Eventʧ��");
		return;
	}

	GF_Log(LOG_NORMAL, "�ɹ�������Ƶ�ļ����á�%s��", strName.c_str());
	RecreateSoundInstance();
}

void CSoundDefBar::OnMoveDownAudio()
{
	if(!m_hRClickItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
	if(type != SOUNDDEF_ITEM_AUDIO)
		return;
	HTREEITEM hParentItem = m_wndTree.GetParentItem(m_hRClickItem);
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hParentItem);
	if(!pSoundDef)
		return;

	AudioTemplate* pAudioTemplate = (AudioTemplate*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pAudioTemplate)
		return;

	std::string strName = pAudioTemplate->GetAudio()->GetName();

	GF_Log(LOG_NORMAL, "������Ƶ�ļ�����...");

	AudioGroup* pParentGroup = pAudioTemplate->GetParentGroup();
	if(pParentGroup->IsSingle())
	{
		if(!pSoundDef->MoveDownAudioGroup(pParentGroup))
		{
			GF_Log(LOG_ERROR, "������Ƶ�ļ���ʧ�ܡ�");
			return;
		}
		m_soundDefDisplay.RebuildSoundDefItems(hParentItem);
	}
	else
	{
		if(!pParentGroup->MoveDownAudioTemplate(pAudioTemplate))
		{
			GF_Log(LOG_ERROR, "������Ƶ�ļ���%s��������ʧ�ܡ�", strName.c_str());
			return;
		}
		m_soundDefDisplay.RebuildAudioGroupItems(hParentItem);
	}

	m_wndTree.Expand(hParentItem, TVM_EXPAND);

	Engine::GetInstance().GetCurProject()->SetModified(true);
	if(!reloadEventWhenSoundDefChanged(pSoundDef))
	{
		GF_Log(LOG_ERROR, "���¼���Eventʧ��");
		return;
	}

	GF_Log(LOG_NORMAL, "�ɹ�������Ƶ�ļ����á�%s��", strName.c_str());
	RecreateSoundInstance();
}

void CSoundDefBar::OnMoveUpAudioGroup()
{
	if(!m_hRClickItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
	if(type != SOUNDDEF_ITEM_AUDIO_GROUP)
		return;
	HTREEITEM hParentItem = m_wndTree.GetParentItem(m_hRClickItem);
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hParentItem);
	if(!pSoundDef)
		return;

	AudioGroup* pAudioGroup = (AudioGroup*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pAudioGroup)
		return;

	GF_Log(LOG_NORMAL, "������Ƶ�ļ���...");

	if(!pSoundDef->MoveUpAudioGroup(pAudioGroup))
	{
		GF_Log(LOG_ERROR, "������Ƶ�ļ���ʧ�ܡ�");
		return;
	}
	m_soundDefDisplay.RebuildSoundDefItems(hParentItem);
	m_wndTree.Expand(hParentItem, TVM_EXPAND);

	Engine::GetInstance().GetCurProject()->SetModified(true);
	if(!reloadEventWhenSoundDefChanged(pSoundDef))
	{
		GF_Log(LOG_ERROR, "���¼���Eventʧ��");
		return;
	}

	GF_Log(LOG_NORMAL, "�ɹ�������Ƶ�ļ���");
	RecreateSoundInstance();
}

void CSoundDefBar::OnMoveDownAudioGroup()
{
	if(!m_hRClickItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
	if(type != SOUNDDEF_ITEM_AUDIO_GROUP)
		return;
	HTREEITEM hParentItem = m_wndTree.GetParentItem(m_hRClickItem);
	SoundDef* pSoundDef = (SoundDef*)m_wndTree.GetItemData(hParentItem);
	if(!pSoundDef)
		return;

	AudioGroup* pAudioGroup = (AudioGroup*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pAudioGroup)
		return;

	GF_Log(LOG_NORMAL, "������Ƶ�ļ���...");

	if(!pSoundDef->MoveDownAudioGroup(pAudioGroup))
	{
		GF_Log(LOG_ERROR, "������Ƶ�ļ���ʧ�ܡ�");
		return;
	}
	m_soundDefDisplay.RebuildSoundDefItems(hParentItem);
	m_wndTree.Expand(hParentItem, TVM_EXPAND);

	Engine::GetInstance().GetCurProject()->SetModified(true);
	if(!reloadEventWhenSoundDefChanged(pSoundDef))
	{
		GF_Log(LOG_ERROR, "���¼���Eventʧ��");
		return;
	}

	GF_Log(LOG_NORMAL, "�ɹ�������Ƶ�ļ���");
	RecreateSoundInstance();
}

void CSoundDefBar::OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVKEYDOWN* pKeyDown = reinterpret_cast<NMTVKEYDOWN*>(pNMHDR);
	if (!pKeyDown)
		return;

	WORD nChar = pKeyDown->wVKey;
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hSelItem);
	if(nChar == VK_DELETE)
	{		
		switch(type)
		{
		case SOUNDDEF_ITEM_AUDIO:
			OnDeleteAudio();
			break;
		case SOUNDDEF_ITEM_AUDIO_GROUP:
			OnDeleteAudioGroup();
			break;
		case SOUNDDEF_ITEM_SOUNDDEF:
			OnDeleteSoundDef();
			break;
		case SOUNDDEF_ITEM_ARCHIVE:
			OnDeleteArchive();
			break;
		}
	}
	else if((nChar == 'a' || nChar == 'A') && GetKeyState(VK_MENU) < 0)
	{
		if(type == SOUNDDEF_ITEM_ARCHIVE)
			newArchive(hSelItem);
	}
}