#include "stdafx.h"
#include "SoundDefView.h"
#include "FSoundDef.h"
#include "FAudio.h"
#include "MainFrm.h"
#include "Global.h"
#include "WndPlay.h"
#include "FAudioInstance.h"
#include "Engine.h"
#include "WndDefine.h"
#include "FSoundInstance.h"
#include "FSoundInstanceTemplate.h"
#include "Project.h"
#include "FAudioGroup.h"
#include "FAudioTemplate.h"

using AudioEngine::AudioGroup;

const int nBorderSize = 0;
const int iPlayWndHeight = 70;

CSoundDefView::CSoundDefView(void)
{
	m_iRClickItem = -1;
	m_pWndPlay = new CWndPlay;
	m_pSoundInstance = 0;
	m_pSoundInstanceTemplate = 0;
	m_pSoundDef = 0;
	m_bPlaying = false;
}

CSoundDefView::~CSoundDefView(void)
{
	delete m_pWndPlay;	
	delete m_pSoundInstance;
	delete m_pSoundInstanceTemplate;
	delete m_pSoundDef;
}
BEGIN_MESSAGE_MAP(CSoundDefView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_PLAY_PLAY, OnClickPlayBtn)
	ON_MESSAGE(WM_PLAY_STOP, OnClickStopBtn)
END_MESSAGE_MAP()

void CSoundDefView::OnPaint()
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

void CSoundDefView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_pWndPlay->SetWindowPos(NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, iPlayWndHeight - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndListCtrl.SetWindowPos (NULL, nBorderSize, nBorderSize+iPlayWndHeight, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize - iPlayWndHeight,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

int CSoundDefView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rtClient;
	GetClientRect(&rtClient);
	CRect rtPlay = rtClient;
	int iPlayHight = iPlayWndHeight;
	rtPlay.bottom = rtPlay.top + iPlayHight;

	if(!m_pWndPlay->Create(this, rtPlay))
		return -1;

	CRect rtList = rtClient;
	rtList.top = rtList.top + iPlayHight;

	DWORD dwListStyle = LVS_REPORT | LVS_SHOWSELALWAYS;
	if(!m_wndListCtrl.Create(WS_CHILD | WS_VISIBLE | dwListStyle, rtList, this, 1))
		return -1;

	m_wndListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_wndListCtrl.InsertColumn(0, "文件路径", LVCFMT_LEFT, 200);
	m_wndListCtrl.InsertColumn(1, "文件大小", LVCFMT_RIGHT, 70);
	m_wndListCtrl.InsertColumn(2, "修改时间", LVCFMT_LEFT, 120);
	return 0;
}

void CSoundDefView::insertItem(Audio* pAudio)
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

void CSoundDefView::SetTreeItem(HTREEITEM hItem)
{
	if(!hItem)
	{
		m_wndListCtrl.DeleteAllItems();
		return;
	}
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)GF_GetMainFrame()->GetSoundDefBar()->GetTree()->GetExtraData(hItem);
	DWORD_PTR pData = GF_GetMainFrame()->GetSoundDefBar()->GetTree()->GetItemData(hItem);
	if(!pData || type != SOUNDDEF_ITEM_SOUNDDEF)
		return;
	m_wndListCtrl.DeleteAllItems();
	SoundDef* pSoundDef = (SoundDef*)pData;
	int iAudioGroupNum = pSoundDef->GetAudioGroupNum();
	for (int i=0; i<iAudioGroupNum; ++i)
	{
		AudioGroup* pAudioGroup = pSoundDef->GetAudioGroupByIndex(i);
		if(!pAudioGroup)
			continue;
		int iAudioTemplateNum = pAudioGroup->GetAudioTemplateNum();
		for (int j=0; j<iAudioTemplateNum; ++j)
		{
			Audio* pAudio = pAudioGroup->GetAudioTemplateByIndex(j)->GetAudio();
			insertItem(pAudio);
		}		
	}
}

LRESULT CSoundDefView::OnClickPlayBtn(WPARAM wParam, LPARAM lParam)
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
		delete pAudioGroup;
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
	GF_Log(LOG_NORMAL, "播放音频文件“%s”", pAudio->GetName());
	m_pSoundInstance->AddListener(this);
	return 0;
_failed:
	m_pWndPlay->SetPlayButtonState(false);
	m_bPlaying = false;
	return 0;
}

LRESULT CSoundDefView::OnClickStopBtn(WPARAM wParam, LPARAM lParam)
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

bool CSoundDefView::OnStop(SoundInstance* pSoundInstance)
{
	GF_Log(LOG_NORMAL, "音频文件播放停止");
	m_pSoundInstance->RemoveListener(this);
	m_bPlaying = false;
	m_pWndPlay->SetPlayButtonState(false);
	return true;
}