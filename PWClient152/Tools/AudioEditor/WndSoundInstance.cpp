#include "stdafx.h"
#include "WndSoundInstance.h"
#include "FSoundInstanceTemplate.h"
#include "FSoundDef.h"
#include "FEventLayer.h"
#include "DlgSoundInsProp.h"
#include "WndDefine.h"
#include "Engine.h"
#include "Project.h"
#include "Global.h"
#include "AudioEditorView.h"
#include "EventView.h"
#include "CheckSoundInsProp.h"
#include "..\AudioEngine\xml\xmlcommon.h"
#include "WndDefine.h"
#include "FSoundDefArchive.h"
#include "Render.h"
#include <AFileDialogWrapper.h>

using AudioEngine::SoundDefArchive;
using AudioEngine::EventLayer;
using AudioEngine::EventParameter;
using AudioEngine::EVENT_PARAM_PROPERTY;

#define MENU_SOUNDINS_PROP	0x3001
#define MENU_DEL_SOUNDINS	0x3002
#define MENU_SAVE_PRESET	0x3003
#define MENU_LOAD_PRESET	0x3004

BEGIN_MESSAGE_MAP(CWndSoundInstance, CWndBase)
	ON_WM_RBUTTONUP()
	ON_COMMAND(MENU_SOUNDINS_PROP, OnSoundInsProp)
	ON_COMMAND(MENU_DEL_SOUNDINS, OnDeleteSoundIns)
	ON_COMMAND(MENU_SAVE_PRESET, OnSavePreset)
	ON_COMMAND(MENU_LOAD_PRESET, OnLoadPreset)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

std::string _calcSoundDefPath(SoundDef* pSoundDef)
{
	std::string strPath = pSoundDef->GetName();
	SoundDefArchive* pParent = pSoundDef->GetParentArchive();
	while(pParent)
	{
		std::string strTemp = pParent->GetName();
		strTemp += " \\ ";
		strPath = strTemp + strPath;
		pParent = pParent->GetParentArchive();
	}
	return strPath;
}

CWndSoundInstance::CWndSoundInstance(void)
{
	m_clBK = RGB(186,205,243);
	m_pSoundInstanceTemplate = 0;
	m_bLButtonDown = false;
	m_hResizeCursor = AfxGetApp()->LoadCursor(IDC_RESIZE_CURSOR);
	m_hMoveCursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
}

CWndSoundInstance::~CWndSoundInstance(void)
{
}

void CWndSoundInstance::DrawRealize(CDC* pDC)
{
	if(!m_pSoundInstanceTemplate)
		return;
	CRect rc;
	GetClientRect(&rc);
	CRect rcEdge = rc;
	rcEdge.right -= 1;
	rcEdge.bottom -= 1;
	rcEdge.DeflateRect(1, 1);
	DrawRect(pDC, rcEdge, RGB(255,255,255), 1);
	CString strDefName = _calcSoundDefPath(m_pSoundInstanceTemplate->GetSoundDef()).c_str();
	CRect rcText = rc;
	rcText.top = (rc.Height()-20)/2;
	rcText.bottom = rcText.top + 20;
	DrawText(pDC, rcText, strDefName, 0, DT_CENTER);
}

void CWndSoundInstance::SetSoundInstance(SoundInstanceTemplate* pSoundInstanceTemplate)
{
	if(!pSoundInstanceTemplate)
		return;
	m_pSoundInstanceTemplate = pSoundInstanceTemplate;
}

void CWndSoundInstance::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, MENU_SOUNDINS_PROP, "声音定义实例属性");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, MENU_DEL_SOUNDINS, "删除声音定义实例");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, MENU_SAVE_PRESET, "保存预设...");
	menu.AppendMenu(MF_STRING, MENU_LOAD_PRESET, "加载预设...");
	menu.TrackPopupMenu(TPM_RIGHTALIGN, point.x, point.y, this);

	CWndBase::OnRButtonUp(nFlags, point);
}

void CWndSoundInstance::OnSoundInsProp()
{
	if(!m_pSoundInstanceTemplate)
		return;

	EventLayer* pEventLayer = m_pSoundInstanceTemplate->GetEventLayer();
	if(!pEventLayer)
		return;

	EventParameter* pEventParameter = pEventLayer->GetParameter();
	EVENT_PARAM_PROPERTY paramProp;
	pEventParameter->GetProperty(paramProp);
	SOUND_INSTANCE_PROPERTY soundInsOrop;
	m_pSoundInstanceTemplate->GetProperty(soundInsOrop);
	soundInsOrop.fStartPosition = paramProp.fMinValue + (paramProp.fMaxValue-paramProp.fMinValue)*soundInsOrop.fStartPosition;
	soundInsOrop.fLength = soundInsOrop.fLength*(paramProp.fMaxValue - paramProp.fMinValue);
	CDlgSoundInsProp dlgSoundInsProp;
	dlgSoundInsProp.SetProp(soundInsOrop, paramProp.fMinValue, paramProp.fMaxValue, m_pSoundInstanceTemplate->GetNote());
_EditProp:
	if(IDOK != dlgSoundInsProp.DoModal())
		return;
	SOUND_INSTANCE_PROPERTY newSoundInsOrop = dlgSoundInsProp.GetProp();

	newSoundInsOrop.fStartPosition = (newSoundInsOrop.fStartPosition - paramProp.fMinValue)/(paramProp.fMaxValue-paramProp.fMinValue);
	newSoundInsOrop.fLength = newSoundInsOrop.fLength/(paramProp.fMaxValue-paramProp.fMinValue);

	CCheckSoundInsProp check;
	float fConflictStart;
	float fConflictLength;
	if(!check.IsLegal(newSoundInsOrop, m_pSoundInstanceTemplate, fConflictStart, fConflictLength))
		goto _EditProp;

	m_pSoundInstanceTemplate->SetProperty(newSoundInsOrop);
	m_pSoundInstanceTemplate->SetNote(dlgSoundInsProp.GetNote());
	::SendMessage(this->GetParent()->GetParent()->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	GF_GetView()->GetEventView()->RecreateEventInstance();
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndSoundInstance::OnDeleteSoundIns()
{
	if(!m_pSoundInstanceTemplate)
		return;

	EventLayer* pEventLayer = m_pSoundInstanceTemplate->GetEventLayer();
	if(!pEventLayer)
		return;
//	if(m_pSoundInstanceTemplate->IsPlaying())
	{
//		MessageBox("正在播放，不能删除", "警告", MB_ICONWARNING);
//		return;
	}
	CString strTip;
	strTip.Format("确定删除声音定义实例“%s”吗？", m_pSoundInstanceTemplate->GetSoundDef()->GetName());
	if(IDYES!=MessageBox(strTip, "删除声音定义实例", MB_ICONWARNING|MB_YESNO))
		return;
	if(!pEventLayer->DeleteSoundInstanceTemplate(m_pSoundInstanceTemplate))
		return;
	::SendMessage(this->GetParent()->GetParent()->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	GF_GetView()->GetEventView()->RecreateEventInstance();
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndSoundInstance::OnSavePreset()
{
	if(!m_pSoundInstanceTemplate)
		return;

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),FALSE, g_Configs.szSoundInsPresetPath, "保存SoundInstance预设文件","",this);
	if(IDOK != dlg.DoModal())
		return;

	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement("SoundInstancePreset");
	doc.LinkEndChild(root);
	m_pSoundInstanceTemplate->SaveXML(root, true);
	TiXmlPrinter printer;
	doc.Accept(&printer);

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, dlg.GetFullPathName(), "w"))
	{
		return;
	}
	fwrite((LPVOID)printer.CStr(), printer.Size(), 1, pFile);
	fclose(pFile);
	GF_Log(LOG_NORMAL, "成功保存预设%s", dlg.GetFileName());
}

void CWndSoundInstance::OnLoadPreset()
{
	if(!m_pSoundInstanceTemplate)
		return;

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),TRUE, g_Configs.szSoundInsPresetPath, "选择SoundInstance预设文件","",this);
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

	TiXmlNode* pNode = doc.FirstChild("SoundInstancePreset");
	if (!pNode)
	{
		GF_Log(LOG_ERROR, "%s不是SoundInstance预设文件", dlg.GetFileName());
		return;
	}

	SOUND_INSTANCE_PROPERTY oldSoundInsOrop;
	m_pSoundInstanceTemplate->GetProperty(oldSoundInsOrop);
	TiXmlElement* root = pNode->ToElement();
	m_pSoundInstanceTemplate->LoadXML(root->FirstChild("SoundInstanceTemplate")->ToElement(), true);

	EventLayer* pEventLayer = m_pSoundInstanceTemplate->GetEventLayer();
	if(!pEventLayer)
		return;
	EventParameter* pEventParameter = pEventLayer->GetParameter();
	EVENT_PARAM_PROPERTY paramProp;
	pEventParameter->GetProperty(paramProp);

	SOUND_INSTANCE_PROPERTY newSoundInsOrop;
	m_pSoundInstanceTemplate->GetProperty(newSoundInsOrop);
	newSoundInsOrop.fStartPosition = (newSoundInsOrop.fStartPosition - paramProp.fMinValue)/(paramProp.fMaxValue-paramProp.fMinValue);
	newSoundInsOrop.fLength = newSoundInsOrop.fLength/(paramProp.fMaxValue-paramProp.fMinValue);
	CCheckSoundInsProp check;
	float fConflictStart;
	float fConflictLength;
	if(!check.IsLegal(newSoundInsOrop, m_pSoundInstanceTemplate, fConflictStart, fConflictLength))
	{
		m_pSoundInstanceTemplate->SetProperty(oldSoundInsOrop);
		return;
	}
	::SendMessage(this->GetParent()->GetParent()->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_Log(LOG_NORMAL, "成功加载预设%s", dlg.GetFileName());
}

void CWndSoundInstance::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = true;
	SetCapture();	
	::POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CRect rc;
	GetClientRect(&rc);
	if(pt.x<5 || pt.x>rc.right-5)
	{
		if(pt.x<5)
		{
			m_msg = WM_RESIZE_SOUNDINS_LEFT;
		}
		else
		{
			m_msg = WM_RESIZE_SOUNDINS_RIGHT;
		}
		::SetCursor(m_hResizeCursor);
	}
	else
	{
		::SetCursor(m_hMoveCursor);
		m_msg = WM_MOVE_SOUNDINS;
	}
	m_ptLast.x = pt.x;
	m_ptLast.y = pt.y;
	CWndBase::OnLButtonDown(nFlags, point);
}

void CWndSoundInstance::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(GetCapture() == this)
		ReleaseCapture();
	m_bLButtonDown = false;
	CWndBase::OnLButtonUp(nFlags, point);
}

void CWndSoundInstance::OnMouseMove(UINT nFlags, CPoint point)
{
	CWndBase::OnMouseMove(nFlags, point);	
	CPoint ptCur;
	GetCursorPos(&ptCur);
	ScreenToClient(&ptCur);
	CRect rc;
	GetClientRect(&rc);
	if(ptCur.x<5 || ptCur.x>rc.right-5)
	{
		::SetCursor(m_hResizeCursor);
	}
	if(!m_bLButtonDown)
		return;
	if(m_msg == WM_RESIZE_SOUNDINS_LEFT || m_msg == WM_RESIZE_SOUNDINS_RIGHT)
	{
		::SetCursor(m_hResizeCursor);
	}
	else if(m_msg == WM_MOVE_SOUNDINS)
	{
		::SetCursor(m_hMoveCursor);
	}

	if(ptCur.x - m_ptLast.x != 0)
		::SendMessage(GetParent()->GetSafeHwnd(), m_msg, (WPARAM)this, ptCur.x - m_ptLast.x);
	if(m_msg == WM_RESIZE_SOUNDINS_RIGHT)
	{
		m_ptLast = ptCur;
	}
}
