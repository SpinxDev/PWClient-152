#include "stdafx.h"
#include "BankView.h"
#include "Global.h"
#include "MainFrm.h"
#include "FAudioBank.h"
#include "FAudio.h"
#include "BankBar.h"
#include "FAudioBankArchive.h"
#include "FAudioInstance.h"
#include "Engine.h"
#include "WndPlay.h"
#include "WndDefine.h"
#include "FSoundInstance.h"
#include "FSoundInstanceTemplate.h"
#include "Project.h"
#include "FSoundDef.h"
#include "FAudioTemplate.h"
#include "FAudioGroup.h"

using AudioEngine::AudioBank;
using AudioEngine::AudioGroup;

#define ID_LIST_CTRL	1

const int nBorderSize = 0;
const int iPlayWndHeight = 70;

CBankView::CBankView(void)
{
	m_iRClickItem = -1;
	m_pWndPlay = new CWndPlay;
	m_pSoundInstance = 0;
	m_pSoundInstanceTemplate = 0;
	m_pSoundDef = 0;
	m_bPlaying = false;
}

CBankView::~CBankView(void)
{
	delete m_pSoundInstance;
	delete m_pSoundInstanceTemplate;
	delete m_pSoundDef;
	delete m_pWndPlay;
}
BEGIN_MESSAGE_MAP(CBankView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
//	ON_NOTIFY(NM_RCLICK, ID_LIST_CTRL, &CBankView::OnListCtrlRClick)
	ON_WM_CREATE()
	ON_MESSAGE(WM_PLAY_PLAY, OnClickPlayBtn)
	ON_MESSAGE(WM_PLAY_STOP, OnClickStopBtn)
END_MESSAGE_MAP()

void CBankView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()


	CRect rectList;
	m_wndListCtrl.GetWindowRect (rectList);
	ScreenToClient (rectList);

	rectList.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectList,	::GetSysColor (COLOR_3DSHADOW), 
		::GetSysColor (COLOR_3DSHADOW));
}

void CBankView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	m_pWndPlay->SetWindowPos(NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, iPlayWndHeight - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndListCtrl.SetWindowPos (NULL, nBorderSize, nBorderSize+iPlayWndHeight, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize - iPlayWndHeight,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

int CBankView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_pWndPlay->Create(this, CRect(0,0,0,0)))
		return -1;


	DWORD dwListStyle = LVS_REPORT | LVS_SHOWSELALWAYS;
	if(!m_wndListCtrl.Create(WS_CHILD | WS_VISIBLE | dwListStyle, CRect(0,0,0,0), this, ID_LIST_CTRL))
		return -1;
	m_wndListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_wndListCtrl.InsertColumn(0, "文件路径", LVCFMT_LEFT, 200);
	m_wndListCtrl.InsertColumn(1, "文件大小", LVCFMT_RIGHT, 70);
	m_wndListCtrl.InsertColumn(2, "修改时间", LVCFMT_LEFT, 120);

	return 0;
}

void CBankView::SetTreeItem(HTREEITEM hItem)
{
	if(!hItem)
	{
		m_wndListCtrl.DeleteAllItems();
		return;
	}
	BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)GF_GetMainFrame()->GetBankBar()->GetTree()->GetExtraData(hItem);
	DWORD_PTR pData = GF_GetMainFrame()->GetBankBar()->GetTree()->GetItemData(hItem);
	if(!pData || type == BANK_ITEM_NONE)
		return;
	m_wndListCtrl.DeleteAllItems();
	m_mapItemAudio.clear();
	if(type == BANK_ITEM_BANK)
	{
		AudioBank* pAudioBank = (AudioBank*)pData;
		int iAudioNum = pAudioBank->GetAudioNum();
		for (int i=0; i<iAudioNum; ++i)
		{
			Audio* pAudio = pAudioBank->GetAudioByIndex(i);
			if(!pAudio)
				continue;
			insertItem(pAudio);
		}
		int iArchiveNum = pAudioBank->GetAudioBankArchiveNum();
		for (int i=0; i<iArchiveNum; ++i)
		{
			AudioBankArchive* pChildArchive = pAudioBank->GetAudioBankArchiveByIndex(i);
			if(!pChildArchive)
				continue;
			listItems(pChildArchive);
		}
	}
	else if(type == BANK_ITEM_ARCHIVE)
	{
		AudioBankArchive* pArchive = (AudioBankArchive*)pData;
		listItems(pArchive);
	}
	else if(type == BANK_ITEM_AUDIO)
	{
		Audio* pAudio = (Audio*)pData;
		insertItem(pAudio);
	}
}

void CBankView::listItems(AudioBankArchive* pArchive)
{
	int iAudioNum = pArchive->GetAudioNum();
	for (int i=0; i<iAudioNum; ++i)
	{
		Audio* pAudio = pArchive->GetAudioByIndex(i);
		if(!pAudio)
			continue;
		insertItem(pAudio);
	}

	int iArchiveNum = pArchive->GetAudioBankArchiveNum();
	for (int i=0; i<iArchiveNum; ++i)
	{
		AudioBankArchive* pChildArchive = pArchive->GetAudioBankArchiveByIndex(i);
		if(!pChildArchive)
			continue;
		listItems(pChildArchive);
	}
}

void CBankView::insertItem(Audio* pAudio)
{
	int iItem = m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(), pAudio->GetPath());
	CFileStatus status;
	if(!CFile::GetStatus(pAudio->GetPath(), status))
		return;
	CString csSize;
	float fSize = status.m_size/1024.0f/1024.0f;
	csSize.Format("%.2fMB", fSize);
	if(fSize < 1.0f)
		csSize.Format("%.0fKB", fSize*1024.0f);
	m_wndListCtrl.SetItemText(iItem, 1, csSize);
	CString csModifyTime;
	csModifyTime.Format("%4d-%02d-%02d %02d:%02d", status.m_mtime.GetYear(), status.m_mtime.GetMonth(), status.m_mtime.GetDay(),
		status.m_mtime.GetHour(), status.m_mtime.GetMinute());
	m_wndListCtrl.SetItemText(iItem, 2, csModifyTime);

	m_mapItemAudio[iItem] = pAudio;
}

void CBankView::OnListCtrlRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	m_iRClickItem = m_wndListCtrl.GetNextSelectedItem(pos);

	CPoint pt;
	GetCursorPos(&pt);
	m_wndListCtrl.ScreenToClient(&pt);
	m_wndListCtrl.ClientToScreen(&pt);
	
	CMenu menu;
	menu.CreatePopupMenu();
	menu.TrackPopupMenu(TPM_RIGHTALIGN, pt.x, pt.y, this);
}

LRESULT CBankView::OnClickPlayBtn(WPARAM wParam, LPARAM lParam)
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	int iItem = m_wndListCtrl.GetNextSelectedItem(pos);
	ItemAudioMap::const_iterator it = m_mapItemAudio.find(iItem);
	if(it == m_mapItemAudio.end())
		return 0;
	Audio* pAudio = it->second;
	if(!pAudio)
		return 0;

	if(m_bPlaying)
		return 0;

	GF_Log(LOG_NORMAL, "点击播放按钮");
	if(!m_bPlaying)
		m_bPlaying = true;
 	m_pWndPlay->SetPlayButtonState(true);
	
	EventProject* pEventProject = Engine::GetInstance().GetCurProject()->GetEventProject();

	delete m_pSoundDef;
	m_pSoundDef = new SoundDef;
	if(!m_pSoundDef->Init(pEventProject))
	{
		delete m_pSoundDef;
		m_pSoundDef = 0;
		goto _failed;
	}

	AudioGroup* pAudioGroup = m_pSoundDef->AddAudioGroup();
	if(!pAudioGroup)
	{
		delete m_pSoundDef;
		m_pSoundDef = 0;
		goto _failed;
	}	
	pAudioGroup->AddAudioTemplate(pAudio);

	delete m_pSoundInstanceTemplate;
	m_pSoundInstanceTemplate = new SoundInstanceTemplate;
	if(!m_pSoundInstanceTemplate->Init(pEventProject))
	{
		delete m_pSoundInstanceTemplate;
		m_pSoundInstanceTemplate = 0;
		goto _failed;
	}
	if(!m_pSoundInstanceTemplate->SetSoundDef(m_pSoundDef))
	{
		delete m_pSoundInstanceTemplate;
		m_pSoundInstanceTemplate = 0;
		delete m_pSoundDef;
		m_pSoundDef = 0;
		goto _failed;
	}
	delete m_pSoundInstance;
	m_pSoundInstance = new SoundInstance;
	if(!m_pSoundInstance->Init(m_pSoundInstanceTemplate, pEventProject))
	{
		delete m_pSoundInstanceTemplate;
		m_pSoundInstanceTemplate = 0;
		delete m_pSoundInstance;
		m_pSoundInstance = 0;
		delete m_pSoundDef;
		m_pSoundDef = 0;
		goto _failed;
	}
	if(!m_pSoundInstance->PlayForSoundDef())
	{
		GF_Log(LOG_ERROR, "播放音频文件“%s”失败。", pAudio->GetName());
		goto _failed;
	}
	GF_Log(LOG_NORMAL, "播放音频文件“%s”。", pAudio->GetName());
	m_pSoundInstance->AddListener(this);
	return 0;
_failed:
	m_pWndPlay->SetPlayButtonState(false);
	m_bPlaying = false;
	return 0;
}

LRESULT CBankView::OnClickStopBtn(WPARAM wParam, LPARAM lParam)
{
	if(!m_pSoundInstance)
		return 0;
	GF_Log(LOG_NORMAL, "点击停止按钮");
	if(!m_pSoundInstance->Stop(true))
	{
		GF_Log(LOG_ERROR, "停止播放出错");
		return 0;
	}
	return 0;
}

bool CBankView::OnStop(SoundInstance* pSoundInstance)
{
	GF_Log(LOG_NORMAL, "音频文件播放停止");
	m_pSoundInstance->RemoveListener(this);
	m_bPlaying = false;
	m_pWndPlay->SetPlayButtonState(false);
	return true;
}