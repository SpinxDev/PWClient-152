#include "stdafx.h"
#include "BankBar.h"
#include "Engine.h"
#include "Project.h"
#include "DlgEditName.h"
#include "Global.h"
#include "FAudio.h"
#include "FAudioBank.h"
#include "FAudioBankArchive.h"
#include "PropertyBar.h"
#include "MainFrm.h"
#include "AudioEditorView.h"
#include "FSoundDef.h"
#include "FSoundDefManager.h"
#include "FAudioGroup.h"
#include "FAudioTemplate.h"
#include "Render.h"
#include <AFileDialogWrapper.h>
#include "DlgSelectAudio.h"

using AudioEngine::AudioBank;
using AudioEngine::SoundDefMap;
using AudioEngine::AudioGroup;

/////////////////////////////////////////////////////////////////////////////
// CBankBar

#define MENU_NEW_BANK			5001
#define MENU_NEW_ARCHIVE		5002
#define MENU_DELETE_BANK		5003
#define MENU_DELETE_ARCHIVE		5004
#define MENU_ADD_AUDIO			5005
#define MENU_DELETE_AUDIO		5006
#define MENU_REFRESH			5007
#define MENU_BATCH_DELETE_AUDIO	5008

BEGIN_MESSAGE_MAP(CBankBar, CTreeBar)
	//{{AFX_MSG_MAP(CBankBar)

	ON_COMMAND(MENU_NEW_BANK, &CBankBar::OnNewBank)
	ON_COMMAND(MENU_NEW_ARCHIVE, &CBankBar::OnNewArchive)
	ON_COMMAND(MENU_DELETE_BANK, &CBankBar::OnDeleteBank)
	ON_COMMAND(MENU_DELETE_ARCHIVE, &CBankBar::OnDeleteArchive)	
	ON_COMMAND(MENU_ADD_AUDIO, &CBankBar::OnAddAudio)
	ON_COMMAND(MENU_DELETE_AUDIO, &CBankBar::OnDeleteAudio)
	ON_COMMAND(MENU_BATCH_DELETE_AUDIO, &CBankBar::OnBatchDeleteAudio)
	ON_COMMAND(MENU_REFRESH, &CBankBar::OnRefresh)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_CTRL, OnTreeKeyDown)
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEventBar construction/destruction

CBankBar::CBankBar()
: m_hRClickItem(0)
{
}

CBankBar::~CBankBar()
{
}

void CBankBar::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(!Engine::GetInstance().GetCurProject())
		return;
	CPoint pos;
	GetCursorPos(&pos);
	m_wndTree.ScreenToClient(&pos);
	m_hRClickItem = m_wndTree.HitTest(pos);
	m_wndTree.ClientToScreen(&pos);
	CMenu menu;
	menu.CreatePopupMenu();
	if(!m_hRClickItem)
	{
		menu.AppendMenu(MF_STRING, MENU_NEW_BANK, "��Ӳֿ�");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_REFRESH, "ˢ��");
		menu.TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
		return;
	}
	if(!m_wndTree.SelectItem(m_hRClickItem))
		return;	
	//////////////////////////////////////////////////////////////////////////
	// �Ҽ��˵�����
	BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
	if(type == BANK_ITEM_NONE)
		return;
	if(type == BANK_ITEM_BANK)
	{
		menu.AppendMenu(MF_STRING, MENU_NEW_ARCHIVE, "���Ŀ¼");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_DELETE_BANK, "ɾ���ֿ�");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_ADD_AUDIO, "��������ļ�");
	}
	else if(type == BANK_ITEM_ARCHIVE)
	{
		menu.AppendMenu(MF_STRING, MENU_NEW_ARCHIVE, "���Ŀ¼");
		menu.AppendMenu(MF_STRING, MENU_DELETE_ARCHIVE, "ɾ��Ŀ¼");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_ADD_AUDIO, "��������ļ�");
	}
	else if(type == BANK_ITEM_AUDIO)
	{
		menu.AppendMenu(MF_STRING, MENU_DELETE_AUDIO, "ɾ�������ļ�");
		menu.AppendMenu(MF_STRING, MENU_BATCH_DELETE_AUDIO, "����ɾ�������ļ�...");
	}
	menu.TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
	//////////////////////////////////////////////////////////////////////////
	*pResult = 0;
}

void CBankBar::OnNewBank()
{
	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		return;

	GF_Log(LOG_NORMAL, "�½��ֿ�...");

	CDlgEditName dlg("�ֿ�����");
_EditName:
	if(IDOK != dlg.DoModal())
		return;	
	if(pAudioManager->IsBankExist(dlg.GetName()))
	{
		MessageBox("�ֿ����Ѵ���", "����", MB_ICONWARNING);
		goto _EditName;
	}
	AudioBank* pAudioBank = pAudioManager->CreateAudioBank(dlg.GetName());
	if(!pAudioBank)
		return;

	GF_Log(LOG_NORMAL, "�����ֿ⡰%s��", dlg.GetName());

	HTREEITEM hItem = m_wndTree.InsertItem(dlg.GetName(), IMAGE_BANK, IMAGE_BANK, TVI_ROOT, TVI_SORT);
	m_wndTree.SetItemData(hItem, (DWORD_PTR)pAudioBank);
	m_wndTree.SetExtraData(hItem, BANK_ITEM_BANK);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CBankBar::OnNewArchive()
{
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(hSelItem);
	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		return;

	GF_Log(LOG_NORMAL, "�½�Ŀ¼...");

	AudioContainer* pAudioContainer = (AudioContainer*)m_wndTree.GetItemData(hSelItem);
	CDlgEditName dlg("Ŀ¼����");
_EditName:
	if(IDOK != dlg.DoModal())
		return;
	if(pAudioManager->IsArchiveExist(pAudioContainer, dlg.GetName()))
	{
		GF_Log(LOG_WARNING, "�Ѵ���Ŀ¼��%s��", dlg.GetName());
		goto _EditName;
	}
	AudioBankArchive* pNewAudioBankArchive = pAudioManager->CreateAudioBankArchive(pAudioContainer, dlg.GetName());	
	if(!pNewAudioBankArchive)
		return;

	GF_Log(LOG_NORMAL, "����Ŀ¼��%s��", dlg.GetName());

	HTREEITEM hItem = m_wndTree.InsertItem(dlg.GetName(), IMAGE_BANK_ARCHIVE, IMAGE_BANK_ARCHIVE, hSelItem, TVI_SORT);
	m_wndTree.SetItemData(hItem, (DWORD_PTR)pNewAudioBankArchive);
	m_wndTree.SetExtraData(hItem, BANK_ITEM_ARCHIVE);
	m_wndTree.Expand(hSelItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CBankBar::OnAddAudio()
{
	if(!m_hRClickItem)
		return;

	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		return;

	GF_Log(LOG_NORMAL, "����ļ�...");

 	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),TRUE, "", "ѡ����Ƶ�ļ�","mp3 wav ogg",this);
	if(IDOK != dlg.DoModal())
	{
		SetCurrentDirectory(g_Configs.szRootPath);
		return;
	}
	SetCurrentDirectory(g_Configs.szRootPath);
	CString csWorkDir(g_Configs.szRootPath);
	csWorkDir.MakeLower();

	bool bAccurateTime = (IDYES == MessageBox("�Ƿ����þ�ȷʱ�������ѡ���ǡ����ã����ú󲥷�ʱ�������ӳ١�\n�����������ѡ���񡱡�", "���þ�ȷʱ�����", MB_YESNO|MB_ICONWARNING));

	int iSelNum = dlg.GetSelectedCount();
	for (int i=0; i<iSelNum; ++i)
	{
		CString csPath(dlg.GetSelectedFileByIdx(i));
		csPath.MakeLower();
		csPath.Replace(csWorkDir, "");
		CString csName = csPath.Right(csPath.GetLength() - csPath.ReverseFind('\\') - 1);

		BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);
		AudioContainer* pAudioContainer = (AudioContainer*)m_wndTree.GetItemData(m_hRClickItem);
		if(pAudioManager->IsAudioExist(pAudioContainer, csName))
		{
			GF_Log(LOG_WARNING, "����ͬ�������ļ���%s��", csName);
			continue;
		}

		GF_Log(LOG_NORMAL, "�ɹ�����ļ���%s��", csName);

		Audio* pAudio = pAudioManager->CreateAudio(pAudioContainer, csName, csPath);
		pAudio->EnableAccurateTime(bAccurateTime);
		HTREEITEM hNewItem = m_wndTree.InsertItem(csName, IMAGE_BANK_AUDIO, IMAGE_BANK_AUDIO, m_hRClickItem, TVI_SORT);
		m_wndTree.SetExtraData(hNewItem, BANK_ITEM_AUDIO);
		m_wndTree.SetItemData(hNewItem, (DWORD_PTR)pAudio);
	}
	m_wndTree.Expand(m_hRClickItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CBankBar::OnDeleteAudio()
{
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	Audio* pAudio = (Audio*)m_wndTree.GetItemData(hSelItem);
	if(!pAudio)
		return;

	GF_Log(LOG_NORMAL, "ɾ���ļ�...");

	char szInfo[1024] = {0};
	sprintf_s(szInfo, "ȷ��ɾ�������ļ�\"%s\"��", pAudio->GetName());
	if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
		return;
	if(!deleteAudio(pAudio))
		return;

	m_wndTree.DeleteItem(hSelItem);	
}

void CBankBar::OnBatchDeleteAudio()
{
	CDlgSelectAudio dlg(true, "ѡ������ɾ������Ƶ�ļ�");
	if(IDOK != dlg.DoModal())
		return;
	AudioList listAudio = dlg.GetSelectAudioList();
	AudioList::const_iterator it = listAudio.begin();
	for (; it!=listAudio.end(); ++it)
	{
		Audio* pAudio = *it;
		if(!pAudio)
			continue;
		if(!deleteAudio(pAudio))
			continue;
	}
	UpdateTree();
}

bool CBankBar::deleteAudio(Audio* pAudio)
{
	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		return false;
	if(!ifCanDelete(pAudio))
		return false;
	char szPath[MAX_PATH] = {0};
	sprintf_s(szPath, MAX_PATH, "%s", pAudio->GetPath());
	if(!GF_DeleteFolder(szPath))
	{
		GF_Log(LOG_ERROR, "ɾ���ļ���%s��ʧ��", szPath);
		return false;
	}
	CString csName = pAudio->GetName();
	if(!pAudioManager->DeleteAudio(pAudio))
	{
		GF_Log(LOG_ERROR, "ɾ���ļ���%s��ʧ��", csName);
		return false;
	}

	GF_Log(LOG_NORMAL, "�ɹ�ɾ���ļ���%s��", szPath);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	return true;
}

void CBankBar::OnDeleteBank()
{
	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		return;
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	AudioBank* pAudioBank = (AudioBank*)m_wndTree.GetItemData(hSelItem);
	if(!pAudioBank)
		return;

	GF_Log(LOG_NORMAL, "ɾ���ֿ�...");

	char szInfo[1024] = {0};
	sprintf_s(szInfo, "ȷ��ɾ���ֿ�\"%s\"��", pAudioBank->GetName());
	if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
		return;
	if(pAudioBank->GetAudioBankArchiveNum() || pAudioBank->GetAudioNum())
	{
		sprintf_s(szInfo, "�ֿ�\"%s\"��Ϊ�գ�ȷ��ɾ����", pAudioBank->GetName());
		if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
			return;
	}

	AudioList listAudio;
	getSubAudio(pAudioBank, listAudio);
	AudioList::const_iterator it = listAudio.begin();
	for (; it!=listAudio.end(); ++it)
	{
		if(!ifCanDelete(*it))
			return;
	}

	CString csName = pAudioBank->GetName();
	if(!pAudioManager->DeleteAudioBank(pAudioBank->GetName()))
	{
		GF_Log(LOG_ERROR, "ɾ���ֿ⡰%s��ʧ��", csName);
		return;
	}

	GF_Log(LOG_NORMAL, "�ɹ�ɾ���ֿ⡰%s��", csName);

	m_wndTree.DeleteItem(hSelItem);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CBankBar::OnDeleteArchive()
{
	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		return;
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	AudioBankArchive* pAudioBankArchive = (AudioBankArchive*)m_wndTree.GetItemData(hSelItem);
	if(!pAudioBankArchive)
		return;

	GF_Log(LOG_NORMAL, "ɾ��Ŀ¼...");

	char szInfo[1024] = {0};
	sprintf_s(szInfo, "ȷ��ɾ��Ŀ¼\"%s\"��", pAudioBankArchive->GetName());
	if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
		return;
	if(pAudioBankArchive->GetAudioBankArchiveNum() || pAudioBankArchive->GetAudioNum())
	{
		sprintf_s(szInfo, "Ŀ¼\"%s\"��Ϊ�գ�ȷ��ɾ����", pAudioBankArchive->GetName());
		if(IDYES != MessageBox(szInfo, "����", MB_ICONWARNING|MB_YESNO))
			return;
	}
	AudioList listAudio;
	getSubAudio(pAudioBankArchive, listAudio);
	AudioList::const_iterator it = listAudio.begin();
	for (; it!=listAudio.end(); ++it)
	{
		if(!ifCanDelete(*it))
			return;
	}

	CString csName = pAudioBankArchive->GetName();
	
	if(!pAudioManager->DeleteAudioBankArchive(pAudioBankArchive))
	{
		GF_Log(LOG_ERROR, "ɾ��Ŀ¼��%s��ʧ�ܡ�", csName);
		return;
	}

	GF_Log(LOG_NORMAL, "�ɹ�ɾ��Ŀ¼��%s��", csName);

	m_wndTree.DeleteItem(hSelItem);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

bool CBankBar::UpdateTree()
{
	if(!m_bankDisplay.UpdateTree(&m_wndTree))
		return false;
	return true;
}

void CBankBar::OnRefresh()
{
	UpdateTree();
}

void CBankBar::OnTreeLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bDragging)
		return;	
	m_bDragging = false;
	CImageList::DragLeave(&m_wndTree);
	CImageList::EndDrag();
	ReleaseCapture();
	delete m_pDragImage;
	m_pDragImage = 0;

	CString csSrc;
	CString csDest;

	m_wndTree.SelectDropTarget(NULL);
	if(m_hItemDragSrc == m_hItemDragDest)
		goto _end;

	if(!m_hItemDragDest)
		goto _end;
	HTREEITEM hItemDest = m_hItemDragDest;

	m_wndTree.Expand(hItemDest, TVE_EXPAND);
	if(m_wndTree.IsChildItem(hItemDest, m_hItemDragSrc))
		goto _end;

	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		goto _end;
	//////////////////////////////////////////////////////////////////////////
	// �������ݴ������	
	BANK_ITEM_TYPE src_type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(m_hItemDragSrc);
	if(src_type == BANK_ITEM_BANK)
		goto _end;
	BANK_ITEM_TYPE dest_type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(hItemDest);
	if(dest_type == BANK_ITEM_ARCHIVE)
	{
		AudioBankArchive* pDestArchive = (AudioBankArchive*)m_wndTree.GetItemData(hItemDest);
		csDest = pDestArchive->GetName();
		if(src_type == BANK_ITEM_AUDIO)
		{			
			Audio* pAudio = (Audio*)m_wndTree.GetItemData(m_hItemDragSrc);
			csSrc = pAudio->GetName();			
			if(!pAudioManager->MoveAudio(pAudio, pDestArchive))
				goto _end;
			m_wndTree.DeleteItem(m_hItemDragSrc);
			m_wndTree.DeleteChildren(hItemDest);
			m_bankDisplay.BuildTree(hItemDest);
			GF_Log(LOG_NORMAL, "�ɹ��ƶ��ļ���%s������%s��", csSrc, csDest);
		}
		else if(src_type == BANK_ITEM_ARCHIVE)
		{
			AudioBankArchive* pArchive = (AudioBankArchive*)m_wndTree.GetItemData(m_hItemDragSrc);
			csSrc = pArchive->GetName();
			if(!pAudioManager->MoveAudioBankArchive(pArchive, pDestArchive))
				goto _end;
			m_wndTree.DeleteItem(m_hItemDragSrc);
			m_wndTree.DeleteChildren(hItemDest);
			m_bankDisplay.BuildTree(hItemDest);
			GF_Log(LOG_NORMAL, "�ɹ��ƶ�Ŀ¼��%s������%s��", csSrc, csDest);
		}
	}
	else if(dest_type == BANK_ITEM_BANK)
	{
		AudioBank* pDestAudioBank = (AudioBank*)m_wndTree.GetItemData(hItemDest);
		csDest = pDestAudioBank->GetName();
		if(src_type == BANK_ITEM_AUDIO)
		{
			Audio* pAudio = (Audio*)m_wndTree.GetItemData(m_hItemDragSrc);
			csSrc = pAudio->GetName();
			if(!pAudioManager->MoveAudio(pAudio, pDestAudioBank))
				goto _end;
			m_wndTree.DeleteItem(m_hItemDragSrc);
			m_wndTree.DeleteChildren(hItemDest);
			m_bankDisplay.BuildTree(hItemDest);
			GF_Log(LOG_NORMAL, "�ɹ��ƶ��ļ���%s������%s��", csSrc, csDest);
		}
		else if(src_type == BANK_ITEM_ARCHIVE)
		{
			AudioBankArchive* pArchive = (AudioBankArchive*)m_wndTree.GetItemData(m_hItemDragSrc);
			csSrc = pArchive->GetName();
			if(!pAudioManager->MoveAudioBankArchive(pArchive, pDestAudioBank))
				goto _end;
			m_wndTree.DeleteItem(m_hItemDragSrc);
			m_wndTree.DeleteChildren(hItemDest);
			m_bankDisplay.BuildTree(hItemDest);
			GF_Log(LOG_NORMAL, "�ɹ��ƶ�Ŀ¼��%s������%s��", csSrc, csDest);
		}
	}
	else
	{
		goto _end;
	}
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_GetMainFrame()->GetSoundDefBar()->UpdateTree();
	//////////////////////////////////////////////////////////////////////////	
_end:
	KillTimer(m_nScrollTimerID);
}

void CBankBar::OnTreeSelectChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPropertyBar* pWndPropBar = GF_GetMainFrame()->GetPropertyBar();
	if(!pWndPropBar)
		return;
	PROP_TYPE prop_type = PROP_NONE;
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		return;
	GF_GetView()->SetCurView(VIEW_BANK);
	GF_GetView()->GetBankView()->SetTreeItem(hItem);
	BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type == BANK_ITEM_BANK)
	{
		AudioBank* pAudioBank = (AudioBank*)m_wndTree.GetItemData(hItem);
		if(!pAudioBank)
			return;
		prop_type = PROP_BANK;
		GF_Log(LOG_NORMAL, "ѡ�вֿ⡰%s��", pAudioBank->GetName());
	}
	else if(type == BANK_ITEM_ARCHIVE)
	{
		AudioBankArchive* pAudioBankArchive = (AudioBankArchive*)m_wndTree.GetItemData(hItem);
		if(!pAudioBankArchive)
			return;
		prop_type = PROP_BANK_ARCHIVE;
		GF_Log(LOG_NORMAL, "ѡ��Ŀ¼��%s��", pAudioBankArchive->GetName());
	}
	else if(type == BANK_ITEM_AUDIO)
	{
		Audio* pAudio = (Audio*)m_wndTree.GetItemData(hItem);
		if(!pAudio)
			return;
		prop_type = PROP_BANK_AUDIO;
		GF_Log(LOG_NORMAL, "ѡ���ļ���%s��", pAudio->GetName());
	}

	pWndPropBar->BuildProperty(prop_type, hItem);

	*pResult = 0;
}

BOOL CBankBar::SetItemText(HTREEITEM hItem, LPCTSTR lpszText)
{
	if(!hItem || !lpszText || !lpszText[0])
		return FALSE;
	AudioManager* pAudioManager = Engine::GetInstance().GetCurProject()->GetAudioManager();
	if(!pAudioManager)
		return FALSE;
	DWORD_PTR pData = m_wndTree.GetItemData(hItem);
	if(!pData)
		return FALSE;
	BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type == BANK_ITEM_BANK)
	{
		AudioBank* pAudioBank = (AudioBank*)pData;
		if(pAudioManager->IsBankExist(lpszText))
		{			
			GF_Log(LOG_WARNING, "����ͬ���ֿ⡰%s��", lpszText);
			return FALSE;
		}
		if(!CTreeBar::SetItemText(hItem, lpszText))
			return FALSE;
		pAudioBank->SetName(lpszText);
	}
	else if(type == BANK_ITEM_ARCHIVE)
	{
		bool bExist = false;
		AudioBankArchive* pAudioBankArchive = (AudioBankArchive*)pData;
		if(pAudioManager->IsArchiveExist(pAudioBankArchive->GetParentContainer(), lpszText))
			bExist = true;
		if(bExist)
		{
			GF_Log(LOG_WARNING, "����ͬ��Ŀ¼��%s��", lpszText);
			return FALSE;
		}
		if(!CTreeBar::SetItemText(hItem, lpszText))
			return FALSE;
		pAudioBankArchive->SetName(lpszText);
	}
	else if(type == BANK_ITEM_AUDIO)
	{
		bool bExist = false;
		Audio* pAudio = (Audio*)pData;
		if(pAudioManager->IsAudioExist(pAudio->GetParentContainer(), lpszText))
			bExist = true;
		if(bExist)
		{
			GF_Log(LOG_WARNING, "����ͬ�������ļ���%s��", lpszText);
			return FALSE;
		}
		if(!CTreeBar::SetItemText(hItem, lpszText))
			return FALSE;
		pAudio->SetName(lpszText);
	}
	Engine::GetInstance().GetCurProject()->SetModified(true);
	return TRUE;
}

void CBankBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CTreeBar::OnPaint()
	CTreeBar::OnPaint();
	if(!GF_GetView()->GetBankView()->IsWindowVisible())
	{
		GF_GetView()->SetCurView(VIEW_BANK);
		HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
		if(!hSelItem)
			return;
		m_wndTree.SelectItem(m_wndTree.GetRootItem());
		m_wndTree.SelectItem(hSelItem);
	}
}

int CBankBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_bankDisplay.BuildImageList(m_ImageList))
		return -1;

	return 0;
}

bool CBankBar::ifCanDelete(Audio* pAudio)
{
	if(!pAudio)
		return false;
	SoundDefManager* pSoundDefManager = Engine::GetInstance().GetCurProject()->GetSoundDefManager();
	if(!pSoundDefManager)
		return false;
	const SoundDefMap& soundDefMap = pSoundDefManager->GetSoundDefMap();
	SoundDefMap::const_iterator it = soundDefMap.begin();
	for (; it!=soundDefMap.end(); ++it)
	{
		SoundDef* pSoundDef = it->second;
		if(!pSoundDef)
			continue;
		int iAudioGroupNum = pSoundDef->GetAudioGroupNum();
		for (int i=0; i<iAudioGroupNum; ++i)
		{
			AudioGroup* pAudioGroup = pSoundDef->GetAudioGroupByIndex(i);
			int iAudioTemplateNum = pAudioGroup->GetAudioTemplateNum();
			for (int j=0; j<iAudioTemplateNum; ++j)
			{
				if(pAudio == pAudioGroup->GetAudioTemplateByIndex(j)->GetAudio())
				{
					GF_Log(LOG_WARNING, "�������塰%s����������Ƶ�ļ���%s��������ɾ����", pSoundDef->GetName(), pAudio->GetName());
					return false;
				}
			}			
		}
	}
	return true;
}

void CBankBar::getSubAudio(AudioContainer* pAudioContainer, AudioList& listAudio)
{
	if(!pAudioContainer)
		return;
	int iAudioNum = pAudioContainer->GetAudioNum();
	for (int i=0; i<iAudioNum; ++i)
	{
		listAudio.push_back(pAudioContainer->GetAudioByIndex(i));
	}
	int iArchiveNum = pAudioContainer->GetAudioBankArchiveNum();
	for (int i=0; i<iArchiveNum; ++i)
	{
		getSubAudio(pAudioContainer->GetAudioBankArchiveByIndex(i), listAudio);
	}
}

void CBankBar::OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVKEYDOWN* pKeyDown = reinterpret_cast<NMTVKEYDOWN*>(pNMHDR);
	if (!pKeyDown)
		return;

	WORD nChar = pKeyDown->wVKey;
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(hSelItem);
	if(nChar == VK_DELETE)
	{
		switch(type)
		{
		case BANK_ITEM_AUDIO:
			OnDeleteAudio();
			break;
		case BANK_ITEM_ARCHIVE:
			OnDeleteArchive();
			break;
		case BANK_ITEM_BANK:
			OnDeleteBank();
			break;
		}
	}
	else if((nChar == 'a' || nChar == 'A') && GetKeyState(VK_MENU) < 0)
	{
		if(type == BANK_ITEM_BANK || type == BANK_ITEM_ARCHIVE)
			OnNewArchive();
	}
}
