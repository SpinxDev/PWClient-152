// Filename	: DlgCountryMapSub.cpp
// Creator	: Han Guanghui
// Date		: 2013/4/3

#include "DlgCountryMapSub.h"
#include "DlgCountryMap.h"
#include "EC_DomainCountry.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Configs.h"
#include "EC_GameSession.h"
#include "EC_CountryConfig.h"
#include "A3DDevice.h"
#include "A3DGdi.h"
#include "AUIDef.h"
#include "AUIManager.h"
#include <A2DSprite.h>

AUI_BEGIN_COMMAND_MAP(CDlgCountryMapSub, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgCountryMapSub, CDlgBase)

AUI_ON_EVENT("CountryMap",	WM_MOUSEMOVE,	OnEventMouseMove)
AUI_ON_EVENT("CountryMap",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("CountryMap",	WM_LBUTTONUP,	OnEventLButtonUp)
AUI_ON_EVENT("CountryMap",	WM_MOUSEWHEEL,  OnEventMouseWheel)
AUI_ON_EVENT("CountryMap",	WM_RBUTTONDOWN,	OnEventRButtonDown)

AUI_END_EVENT_MAP()

CDlgCountryMapSub::CDlgCountryMapSub():
m_pImg_CountryMap(NULL),
m_pSubDlg(NULL),
m_pDlgCountryMap(NULL),
m_nHoverAreaIndex(CECDomain::OUT_OF_DOMAIN),
m_nLastHoverIndex(CECDomain::OUT_OF_DOMAIN),
m_DragHBarPosStart(0),
m_DragVBarPosStart(0),
m_bIsDragging(false),
m_bLButtonDown(false)
{
	m_DraggingStartPoint.x = 0;
	m_DraggingStartPoint.y = 0;
}

CDlgCountryMapSub::~CDlgCountryMapSub()
{

}

bool CDlgCountryMapSub::OnInitDialog()
{
	DDX_Control("CountryMap",m_pImg_CountryMap);

	m_DomainCountry.SetImage(m_pImg_CountryMap);
	return true;
}

void CDlgCountryMapSub::UpdateImage(bool dataChanged)
{
	//	数据有改变，检查贴图是否需要更新
	int iDataType = CECDomainCountryInfo::GetInstance()->GetDataType();
	const char *szImages[DOMAIN2_TYPE_NUM] = {"ingame\\战略地图.dds", "ingame\\战略地图_跨服内嵌.dds"};
	if (iDataType >= 0 && iDataType < DOMAIN2_TYPE_NUM){
		bool bImageChanged = false;
		AUIOBJECT_SETPROPERTY p;
		m_pImg_CountryMap->GetProperty("Image File", &p);
		if (stricmp(p.fn, szImages[iDataType])){
			bImageChanged = true;
		}
		if (!bImageChanged && !dataChanged){
			return;
		}

		DropCapture();
		
		if (bImageChanged){
			strcpy(p.fn, szImages[iDataType]);
			ScopedAUIControlSpriteModify _dummy(m_pImg_CountryMap);
			m_pImg_CountryMap->SetProperty("Image File", &p);
		}
		m_DomainCountry.SetImage(m_pImg_CountryMap);
		OnResize();
		m_DomainCountry.ResizeImages();
	}
}

void CDlgCountryMapSub::DomainRenderCallback(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3)
{
	CDlgCountryMapSub* pDlgSub = (CDlgCountryMapSub*)param1;
	if (pDlgSub)
	{
		pDlgSub->RenderDomain();
	}
}

bool CDlgCountryMapSub::Render()
{
	m_pImg_CountryMap->SetRenderCallback(DomainRenderCallback, (DWORD)this);
	return CDlgBase::Render();
}
void CDlgCountryMapSub::RenderDomain()
{
	if (m_pDlgCountryMap->IsMapReady() && m_pDlgCountryMap->IsLocationReady())
	{
		CECDomainCountry::RenderParam param(
			m_nHoverAreaIndex, m_pDlgCountryMap->m_domainLoc, m_pDlgCountryMap->m_domainNext, 
			m_pDlgCountryMap->m_cntMoveStart, m_pDlgCountryMap->m_cntMoveEnd, m_pDlgCountryMap->GetCurrentTime(),	
			m_pDlgCountryMap->m_route);
		
		m_DomainCountry.Render(param);
	}
}
void CDlgCountryMapSub::OnCommandCancel(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgCountryMapSub::OnTick()
{
	bool bDoResize = m_pDlgCountryMap->GetData("DoSomethingWhenResized") == 1;
	if (bDoResize)
	{
		m_DomainCountry.ResizeImages();
		m_pDlgCountryMap->SetData(0,"DoSomethingWhenResized");
	}
}

void CDlgCountryMapSub::OnShowDialog()
{
	m_nHoverAreaIndex = CECDomain::OUT_OF_DOMAIN;
	m_nLastHoverIndex = CECDomain::OUT_OF_DOMAIN;
	if (m_pDlgCountryMap == NULL) {
		m_pSubDlg = GetParentDlgControl();
		if (m_pSubDlg) 
			m_pDlgCountryMap = dynamic_cast<CDlgCountryMap*>(GetParentDlgControl()->GetParent());
		if (m_pDlgCountryMap == NULL) {
			ASSERT(false);
			Show(false);
		} else {
			SIZE szImgMin = GetMinImageSize();
			m_pImg_CountryMap->SetSize(szImgMin.cx, szImgMin.cy);
			m_pSubDlg->SetSubDialogOrgSize(szImgMin.cx, szImgMin.cy);
		}
	}
}

void CDlgCountryMapSub::OnHideDialog()
{
	DropCapture();
}
void CDlgCountryMapSub::OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	A3DPOINT2 pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	
	if( !(wParam & MK_LBUTTON) ){
		m_nHoverAreaIndex = m_DomainCountry.GetDomain().GetDomainIndexByCoordinate(pt);
		if( m_DomainCountry.IsHoverDomainStateImage(pt) )
		{
			m_nLastHoverIndex = -2;
			UpdateHint(false);
		}
		else if (m_nLastHoverIndex != m_nHoverAreaIndex)
		{
			m_nLastHoverIndex = m_nHoverAreaIndex;
			UpdateHint(true);
		}
	} else if (m_bLButtonDown){
		m_bIsDragging = true;
		int ox = m_DraggingStartPoint.x - pt.x;
		int oy = m_DraggingStartPoint.y - pt.y;
		
		m_pSubDlg->SetHBarPos(m_DragHBarPosStart + ox);
		m_pSubDlg->SetVBarPos(m_DragVBarPosStart + oy);
	}
	
}

void CDlgCountryMapSub::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	A3DPOINT2 pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	m_DraggingStartPoint = pt;
	m_DragHBarPosStart = m_pSubDlg->GetHBarPos();
	m_DragVBarPosStart = m_pSubDlg->GetVBarPos();
	SetCaptureObject(m_pImg_CountryMap);
	SetCapture(true, pt.x, pt.y);
	m_bLButtonDown = true;
}

void CDlgCountryMapSub::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (!m_bLButtonDown){
		return;
	}
	m_bLButtonDown = false;
	if (IsCapture()) SetCapture(false);
	SetCaptureObject(NULL);
	if (m_bIsDragging) {
		m_bIsDragging = false;
		return;
	}

	A3DPOINT2 pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	m_nHoverAreaIndex = m_DomainCountry.GetDomain().GetDomainIndexByCoordinate(pt);
	m_pDlgCountryMap->OnClickDomain(m_nHoverAreaIndex, lParam);
}

void CDlgCountryMapSub::DropCapture()
{
	m_bIsDragging = false;
	m_bLButtonDown = false;
	if (IsCapture()) SetCapture(false);
	SetCaptureObject(NULL);
}

SIZE CDlgCountryMapSub::GetMinImageSize()
{
	//	计算当前对话框控件 m_pImg_CountryMap 在保持缩放比时的最小大小（不小于父 SUBDIALOG 的大小）
	SIZE szImgMin;
	while (true)
	{
		if (!m_pSubDlg ||
			!m_pImg_CountryMap || !m_pImg_CountryMap->GetImage() ||
			m_pImg_CountryMap->GetImage()->GetWidth() <= 0 ||
			m_pImg_CountryMap->GetImage()->GetHeight() <= 0){
			ASSERT(false);
			szImgMin = GetSize();
			break;
		}
		SIZE szParentControl = m_pSubDlg->GetSize();
		int nImageW = m_pImg_CountryMap->GetImage()->GetWidth();
		int nImageH = m_pImg_CountryMap->GetImage()->GetHeight();		
		float fWHRatio = nImageW/(float)nImageH;
		int nScaledW = static_cast<int>(fWHRatio * szParentControl.cy);
		if (nScaledW >= szParentControl.cx){
			szImgMin.cx = nScaledW;
			szImgMin.cy = szParentControl.cy;
		}else{
			float fHWRatio = nImageH/(float)nImageW;
			int nScaledH = static_cast<int>(fHWRatio * szParentControl.cx);
			szImgMin.cx = szParentControl.cx;
			szImgMin.cy = nScaledH;
		}
		break;
	}
	return szImgMin;
}

void CDlgCountryMapSub::OnResize()
{
	if (!m_pDlgCountryMap) return;
	SIZE szImgMin = GetMinImageSize();
	m_pSubDlg->SetHBarPos(0);
	m_pSubDlg->SetVBarPos(0);
	m_pImg_CountryMap->SetSize(szImgMin.cx, szImgMin.cy);
	m_pSubDlg->SetSubDialogOrgSize(szImgMin.cx, szImgMin.cy);
}

void CDlgCountryMapSub::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if(wParam & MK_LBUTTON) return;
	int zDelta = (short)HIWORD(wParam);

	SIZE szImg = m_pImg_CountryMap->GetSize();
	if (szImg.cx == 0 || szImg.cy == 0) return;

	SIZE szImgMin = GetMinImageSize();
	float fScale = 1.f;
	if (zDelta > 0) {
		float sx = 2048.f / szImg.cx;
		float sy = 2048.f / szImg.cy;
		fScale = a_Min(sx, sy, 1.25f);
	} else {
		float sx = (float)szImgMin.cx / szImg.cx;
		float sy = (float)szImgMin.cy / szImg.cy;
		fScale = a_Max(sx, sy, 0.8f);
	}
	szImg.cx *= fScale;
	szImg.cy *= fScale;

	a_Clamp(szImg.cx, szImgMin.cx, 2048l);
	a_Clamp(szImg.cy, szImgMin.cy, 2048l);	

	//	修改前、先计算鼠标所在位置对应图片原始分辨率象素空间中的坐标
	A3DPOINT2 ptMouse;
	ptMouse.x = GET_X_LPARAM(lParam);
	ptMouse.y = GET_Y_LPARAM(lParam);	
	HWND hMainWnd = GetGame()->GetGameInit().hWnd;
	RECT rc;
	POINT pt ={0};
	GetClientRect(hMainWnd,&rc);
	ClientToScreen(hMainWnd,&pt);
	ptMouse.x -= pt.x;
	ptMouse.y -= pt.y;

	A3DPOINT2 ptPixel = m_DomainCountry.GetDomain().ScreenToPixel(ptMouse);

	//	执行缩放
	m_pSubDlg->SetHBarPos(0);
	m_pSubDlg->SetVBarPos(0);
	m_pImg_CountryMap->SetSize(szImg.cx, szImg.cy);
	m_pSubDlg->SetSubDialogOrgSize(szImg.cx, szImg.cy);

	//	缩放后，计算新位置、以尽量维持鼠标所在内容不变
	A3DPOINT2 ptOffset = m_DomainCountry.GetDomain().CalcImgOffset(ptPixel, ptMouse);
	m_pSubDlg->SetHBarPos(m_pSubDlg->GetHBarPos() - ptOffset.x);
	m_pSubDlg->SetVBarPos(m_pSubDlg->GetVBarPos() - ptOffset.y);

	m_pDlgCountryMap->SetData(1,"DoSomethingWhenResized");
}

void CDlgCountryMapSub::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	m_pDlgCountryMap->ShowFalse();
}

void CDlgCountryMapSub::UpdateHint(bool bDomainDesc/* =true */)
{
	ACString strHint;
	if (CECDomain::OUT_OF_DOMAIN == m_nHoverAreaIndex)
	{
		strHint = _AL("");
	}
	else if (m_pDlgCountryMap->IsMapReady())
	{
		const DOMAIN2_INFO& info = CECDomainCountryInfo::GetInstance()->Get(m_nHoverAreaIndex);
		ACString strTemp;
		ACString strNextLine = _AL("\r");

		if( !bDomainDesc )
		{
			// 领土名称
			strHint += info.name;
			strHint += strNextLine;

			if( info.byStateMask & DOMAIN2_INFO::STATE_RESTRICT )
			{
				ASSERT(info.iPlayer.size() == info.iGhost.size());

				if( info.iPlayer.size() == 1 )
				{
					int iProfession = GetHostPlayer()->GetProfession();
					strTemp.Format(GetStringFromTable(10300), GetGameRun()->GetProfName(iProfession), info.iPlayer[0], info.iGhost[0]);
					strHint += strTemp;
					strHint += strNextLine;
				}
				else
				{
					for( size_t i=0;i<info.iPlayer.size();i++ )
					{
						strTemp.Format(GetStringFromTable(10300), GetGameRun()->GetProfName(i), info.iPlayer[i], info.iGhost[i]);
						strHint += strTemp;
						strHint += strNextLine;
					}
				}
			}

			if( info.byStateMask & DOMAIN2_INFO::STATE_SMALLTOKEN )
			{
				strHint += GetStringFromTable(10301);
				strHint += strNextLine;
			}
			else if( info.byStateMask & DOMAIN2_INFO::STATE_BIGTOKEN )
			{
				strHint += GetStringFromTable(10302);
				strHint += strNextLine;
			}
		}
		else
		{
			if (g_pGame->GetConfigs()->GetShowIDFlag())
			{
				strTemp.Format(_AL("%d"), info.id);
				strHint += strTemp;
				strHint += strNextLine;
			}

			strHint += info.name;
			strHint += strNextLine;

			if (info.IsAtWar())
			{
				strTemp.Format(GetStringFromTable(9904), *CECCountryConfig::Instance().GetName(info.challenger));
				strHint += strTemp;
				strHint += strNextLine;
				strTemp.Format(GetStringFromTable(9905), *CECCountryConfig::Instance().GetName(info.owner));
				strHint += strTemp;
				strHint += strNextLine;
				strTemp.Format(GetStringFromTable(9906), info.player_count[info.challenger - 1]);
				strHint += strTemp;
				strHint += strNextLine;
				strTemp.Format(GetStringFromTable(9907), info.player_count[info.owner - 1]);
				strHint += strTemp;
				strHint += strNextLine;
				strTemp.Format(GetStringFromTable(9901), info.point);
				strHint += strTemp;
			}
			else
			{
				strTemp.Format(GetStringFromTable(9900), *CECCountryConfig::Instance().GetName(info.owner));
				strHint += strTemp;
				strHint += strNextLine;
				strTemp.Format(GetStringFromTable(9901), info.point);
				strHint += strTemp;
				strHint += strNextLine;
				if (info.HasEndTime())
				{
					int sec = m_pDlgCountryMap->ConverToSeconds(m_pDlgCountryMap->GetTimeLeft(info.end_time));
					strTemp.Format(GetStringFromTable(9902), GetGameUIMan()->GetFormatTime(sec));
					strHint += strTemp;
					strHint += strNextLine;
				}
				strTemp.Format(GetStringFromTable(9903), info.player_count[info.owner - 1]);
				strHint += strTemp;
			}

			strTemp.Format(GetStringFromTable(10390), GetStringFromTable(10391+info.wartype));
			strHint += strNextLine;
			strHint += strTemp;
		}
	}
	m_pImg_CountryMap->SetHint(strHint);
	m_pImg_CountryMap->SetHintFollow(true);
}
