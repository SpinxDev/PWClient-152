// Filename	: EC_BaseUIMan.cpp
// Creator	: Feng Ning
// Date		: 2010/10/18

#pragma warning (disable: 4284)

#include "EC_BaseUIMan.h"
#include "DlgTheme.h"
#include "DlgExplorer.h"
#include "AF.h"
#include "A3DEngine.h"
#include "A3DDevice.h"

#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "CSplit.h"
#include "EC_UIAnimation.h"
#include "EC_UIConfigs.h"
#include "EC_FixedMsg.h"
#include "EC_Utility.h"
#include "EC_Global.h"
#include "Arc/arc_overlay.h"
#include "EC_CustomizeMgr.h"
#include "EC_Player.h"
#include "EC_Profile.h"
#include "EC_Configs.h"
#include "EC_GameSession.h"

#include <AUIStillImageButton.h>

#define new A_DEBUG_NEW

CECBaseUIMan::CECBaseUIMan()
:m_bChanging(false)
,m_pAUIFrameDebug(NULL)
,m_IsDebug(false)
{
}

CECBaseUIMan::~CECBaseUIMan()
{
	for(size_t i=0; i<m_Animations.size(); i++)
	{
		CECUIAnimation* pAni = m_Animations[i];
		delete pAni;
	}
	m_Animations.clear();
}

PAUIDIALOG CECBaseUIMan::CreateDlgInstance(const AString strTemplName)
{
	return new CDlgTheme;
}

bool CECBaseUIMan::ChangeLayout(const char *pszFilename)
{
	ScopedElapsedTime _dummy(_AL("changelayout time"));

	ASSERT(m_pA3DEngine && m_pA3DDevice);

	if( !pszFilename )
	{
		return false;
	}

	bool bval;
	AScriptFile s;
	char szFilename[MAX_PATH];

	sprintf(szFilename, "Interfaces\\%s", pszFilename);
	bval = s.Open(szFilename);
	if( !bval )
	{
		return AUI_ReportError(__LINE__, 1, "CECBaseUIMan::ChangeLayout(), failed to open script file %s", pszFilename);
	}

	// store the filename for duplicated check
	abase::hash_map<AString, PAUIDIALOG> filemap;
	for( int id = 0; id < (int)m_aDialog.size(); id++ )
	{
		AString filename(m_aDialog[id]->GetFilename());
		filename.MakeLower();
		filemap[filename] = m_aDialog[id];
	}

	CECBaseUIMan* pTmpMgr = new CECBaseUIMan();

	// sync the default setting
	pTmpMgr->m_nDefaultFontSize = this->m_nDefaultFontSize;
	pTmpMgr->m_strDefaultFontName = this->m_strDefaultFontName;

	pTmpMgr->Init(m_pA3DEngine, m_pA3DEngine->GetA3DDevice(), NULL);
	
	// start to check the new theme file
	m_pObjMouseOn = NULL;
	m_bChanging = true;

	//abase::vector<CDlgTheme*> newLayout;
	while( !s.IsEnd() )
	{
		bval = s.GetNextToken(true);
		if( !bval ) break;		// End of file.

		BEGIN_FAKE_WHILE2;

		if( 0 == strcmpi(s.m_szToken, "#") ||
			0 == strcmpi(s.m_szToken, "//") )
		{
			s.SkipLine();
		}
		else
		{
			// read info from dcf file
			AString filename(s.m_szToken);
			filename.MakeLower();
			
			// check duplicated dialog
			bool bDuplicate = (filemap.find(filename) != filemap.end());

			CDlgTheme* pDlg = NULL;

			// skip the duplicated dialog
			if(!bDuplicate)
			{
				// create new dialog template
				int idDialog = pTmpMgr->CreateDlg(m_pA3DEngine, m_pA3DDevice, filename);
				if( idDialog < 0 )
				{
					AUI_ReportError(__LINE__, 1, "CECBaseUIMan::ChangeLayout(), failed to create dialog from %s", filename);
				}
				else
				{
					pDlg = (CDlgTheme*)pTmpMgr->GetDialog(idDialog);
				}
			}

			CHECK_BREAK2(s.GetNextToken(true));
			int x = atoi(s.m_szToken);
			
			CHECK_BREAK2(s.GetNextToken(true));
			int y = atoi(s.m_szToken);
			
			CHECK_BREAK2(s.GetNextToken(true));
			bool bShow = atoi(s.m_szToken) ? true : false;

			if(pDlg)
			{
				pDlg->TranslatePosForAlign(x, y);
				pDlg->SetPosEx(x, y);
				pDlg->SetIsDefaultShow(bShow);

				// change layout on old dialog
				CDlgTheme* pOld = dynamic_cast<CDlgTheme*>(GetDialog(pDlg->GetName()));
				if(!pOld)
				{
					AUI_ReportError(__LINE__, 1, "CECBaseUIMan::ChangeLayout(), failed to find dialog %s", pDlg->GetName());
				}
				else if(!pOld->ChangeLayout(pDlg))
				{
					AUI_ReportError(__LINE__, 1, "CECBaseUIMan::ChangeLayout(), failed to change dialog %s", pDlg->GetName());
				}
				pDlg->SetDestroyFlag(true);
			}

			// for deleting unnecessary dialog
			pTmpMgr->Tick(0);
		}
		END_FAKE_WHILE2;

		BEGIN_ON_FAIL_FAKE_WHILE2;

		AUI_ReportError(DEFAULT_2_PARAM, "CECBaseUIMan::ChangeLayout(), failed to read from file");

		END_ON_FAIL_FAKE_WHILE2;
	}
	s.Close();

	A3DRELEASE(pTmpMgr);

	LoadTemplateFrame();

	m_bChanging = false;
	return true;
}

bool CECBaseUIMan::Release()
{
	A3DRELEASE(m_pAUIFrameDebug);
	return AUILuaManager::Release();
}

bool CECBaseUIMan::Init(A3DEngine* pA3DEngine, A3DDevice* pA3DDevice, const char* szDCFile)
{
	m_webBrowserDialogs.clear();

	// set NULL because we may use different filename for StringTable
	bool ret = AUILuaManager::Init(pA3DEngine, pA3DDevice, NULL);
	if(ret)
	{
		LoadDialogLayout(szDCFile);
		
		A3DRECT rcNew;
		A3DDEVFMT fmt = m_pA3DEngine->GetA3DDevice()->GetDevFormat();
		rcNew.SetRect(0, 0, fmt.nWidth, fmt.nHeight);
		ResizeWindows(m_rcWindow, rcNew);
		RepositionAnimation();

		LoadTemplateFrame();

		OptimizeSpriteResource(true);
	}

	return ret;
}

void CECBaseUIMan::OptimizeSpriteResource(bool bOptimize)
{
	for( int i = 0; i < (int)m_aDialog.size(); i++ )
	{
		PAUIDIALOG pDlg = m_aDialog[i];
		pDlg->SetOptimizeResourceLoad(bOptimize);
		if (!bOptimize){
			pDlg->OptimizeSpriteResource(true);			//	加载所有资源
		}else if (!pDlg->IsShow()){
			pDlg->OptimizeSpriteResource(false);		//	对没显示资源卸载资源
		}
	}
}

bool CECBaseUIMan::LoadTemplateFrame()
{
	AUIOBJECT_SETPROPERTY prop;

	// initialize debug frame
	A3DRELEASE(m_pAUIFrameDebug);
	m_pAUIFrameDebug = new AUIFrame;
	if( !m_pAUIFrameDebug ) return AUI_ReportError(__LINE__, 1, "CECBaseUIMan::LoadTemplateFrame(), no enough memory");

	
	PAUIDIALOG pHint = GetDialog("Tmpl_Hint");
	if(pHint)
	{
		A3DRELEASE(m_pAUIFrameHint);

		m_pAUIFrameHint = new AUIFrame;
		if( !m_pAUIFrameHint ) return AUI_ReportError(__LINE__, 1, "CECBaseUIMan::LoadTemplateFrame(), no enough memory");
		
		pHint->GetProperty("Frame Image", &prop);
		bool bval = m_pAUIFrameHint->Init(m_pA3DEngine, m_pA3DDevice, prop.fn);
		if( !bval ) A3DRELEASE(m_pAUIFrameHint);

		bval = m_pAUIFrameDebug->Init(m_pA3DEngine, m_pA3DDevice, prop.fn);
		if( !bval ) A3DRELEASE(m_pAUIFrameDebug);
	}
	else
	{
		// default debug frame
		bool bval = m_pAUIFrameDebug->Init(m_pA3DEngine, m_pA3DDevice, "Frame\\Frame.tga");
		if( !bval ) A3DRELEASE(m_pAUIFrameDebug);
	}

	PAUIDIALOG pMsgbox = GetDialog("Tmpl_Msgbox");
	if(pMsgbox)
	{
		pMsgbox->GetProperty("Frame Image", &prop);
		m_szMsgBoxFrame = prop.fn;

		PAUIOBJECT pBtn = pMsgbox->GetDlgItem("Btn_Tmpl");
		if(pBtn)
		{
			pBtn->GetProperty("Up Frame File", &prop);
			m_szMsgBoxButton = prop.fn;
			
			pBtn->GetProperty("Down Frame File", &prop);
			m_szMsgBoxButtonDown = prop.fn;

//			pBtn->GetProperty("OnHover Frame File", &prop);
//			m_szMsgBoxButtonHover = prop.fn;
		}
	}

	return true;
}

bool CECBaseUIMan::IsCustomizeCharacter()
{
	CECCustomizeMgr *pCustomizeMgr = GetCustomizeMgr();
	return pCustomizeMgr && pCustomizeMgr->IsWorking();
}

bool CECBaseUIMan::AddRestoreDlgToCC(AUIDialog *pDlg)
{
	if (IsCustomizeCharacter()){
		return GetCustomizeMgr()->AddRestoreDialog(pDlg);
	}
	return false;
}

bool CECBaseUIMan::RemoveRestoreDlgToCC(AUIDialog *pDlg)
{
	if (IsCustomizeCharacter()){
		return GetCustomizeMgr()->RemoveRestoreDialog(pDlg);
	}
	return false;
}

ACString CECBaseUIMan::GetRealmName(int realmLevel)
{
	ACString strRealm;
	if (realmLevel > 0){
		int layer = CECPlayer::GetRealmLayer(realmLevel);
		int subLevel = CECPlayer::GetRealmSubLevel(realmLevel);
		strRealm.Format(GetStringFromTable(11100), GetStringFromTable(11100+layer), GetStringFromTable(11120+subLevel));
	}
	return strRealm;
}

bool CECBaseUIMan::Render(void)
{
	bool bval = AUILuaManager::Render();
	if( !bval ) return false;

	bval = RenderUIDebug();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "CECBaseUIMan::Render(), failed to call RenderUIDebug()");

	return true;
}

ACString CECBaseUIMan::GetDebugString()
{
	// set debug info
	AString strDebug;
	if( m_pObjMouseOn )
	{
		strDebug.Format("FILE: %s\rDLG: %s\rOBJ: %s\rTYPE: %s\rCMD: %s",
			m_pObjMouseOn->GetParent()->GetFilename(), 
			m_pObjMouseOn->GetParent()->GetName(),
			m_pObjMouseOn->GetName(),
			AUI_RetrieveControlName(m_pObjMouseOn->GetType()),
			m_pObjMouseOn->GetCommand()
			);
	}
	else if(m_pDlgMouseOn)
	{
		strDebug.Format("FILE: %s\rDLG: %s",
			m_pDlgMouseOn->GetFilename(), 
			m_pDlgMouseOn->GetName());
	}
	else
	{
		return _AL("");
	}

	return AS2AC(strDebug);
}

bool CECBaseUIMan::RenderUIDebug()
{
	if( !m_pAUIFrameDebug || !m_IsDebug )
		return true;

	// set debug info
	ACString strDebug = GetDebugString();
	if(strDebug.GetLength() == 0)
	{
		return true;
	}

	// get cursor pos
	POINT ptPos;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	GetCursorPos(&ptPos);
	ScreenToClient(m_pA3DDevice->GetDeviceWnd(), &ptPos);
	ptPos.x -= m_rcWindow.left;
	ptPos.y -= m_rcWindow.top;

	bool bval;
	int nWidth[10], nHeight[10], W, H, nLines, x, y;
	A3DFTFont *pFont = GetDefaultFont();
	int nSize = a_atoi(GetStringFromTable(2));
	if( nSize > 0 )
		pFont->Resize(nSize * m_fWindowScale);
	
	SIZE s = m_pAUIFrameDebug->GetSizeLimit();
	CSplit sp(strDebug);
	CSPLIT_STRING_VECTOR vec = sp.Split(_AL("\t"));
	W = H = 0;

	size_t i;
	for(i = 0; i < vec.size(); i++)
	{
		while (vec[i].Right(1) == _AL("\r")) 
			vec[i].CutRight(1);
		
		bval = AUI_GetTextRect(pFont, vec[i], nWidth[i], nHeight[i], nLines, 3);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "CECBaseUIMan::RenderUIDebug(), failed to call AUI_GetTextRect()");
		W += nWidth[i] + s.cx;
		if( nHeight[i] + s.cy - 2 > H )
			H = nHeight[i] + s.cy - 2;
	}

	// judge the size
	x = max(0, ptPos.x);
	y = ptPos.y + 32;
	
	if( x + W > m_rcWindow.Width() )
		x = m_rcWindow.Width() - W;
	if( y + H > m_rcWindow.Height() )
	{
		if( ptPos.y - s.cy - H >= 0 )
			y = ptPos.y - s.cy - H;
		else
			y = m_rcWindow.Height() - H;
	}

	// render debug info
	for(i = 0; i < vec.size(); i++)
	{
		bval = m_pAUIFrameDebug->Render(x + m_rcWindow.left, y + m_rcWindow.top, nWidth[i] + s.cx, nHeight[i] + s.cy,
			pFont, 0, NULL, AUIFRAME_ALIGN_LEFT, 0, 0, 255);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "CECBaseUIMan::RenderUIDebug(), failed to call m_pAUIFrameDebug->Render()");

		x += s.cx / 2;
		y += s.cy / 2;
		bval = AUI_TextOutFormat(pFont, x + m_rcWindow.left, y + m_rcWindow.top, vec[i], NULL, 0, 0, 3);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "CECBaseUIMan::RenderUIDebug(), failed to call AUI_TextOutFormat()");
		x += s.cx / 2;
		y -= s.cy / 2;
		x += nWidth[i];
	}

	bval = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "CECBaseUIMan::Render(), failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");
	
	m_pFontMan->Flush();
	
	return true;
}

bool CECBaseUIMan::Tick(DWORD dwDeltaTime)
{
	// update animation
	for(size_t i=0; i>=0 && i<m_Animations.size(); i++)
	{
		CECUIAnimation* pAni = m_Animations[i];
		if(!pAni->Update())
		{
			// stopped
			m_Animations.erase(m_Animations.begin()+i);
			delete pAni;
			i--;
		}
	}

	return AUILuaManager::Tick(dwDeltaTime);
}

void CECBaseUIMan::StartAnimation(PAUIDIALOG pDlg, CECUIAnimation* pAni)
{
	// remove the duplicated animation
	for(size_t i=0; i>=0 && i<m_Animations.size(); i++)
	{
		CECUIAnimation* pAni = m_Animations[i];
		if(pAni->BelongsTo(pDlg))
		{
			pAni->Reset();
			m_Animations.erase(m_Animations.begin()+i);
			delete pAni;
			break;
		}
	}

	pAni->Start(pDlg);
	m_Animations.push_back(pAni);
}

void CECBaseUIMan::RepositionAnimation()
{
	for (unsigned int i = 0;i < m_aDialog.size();i++)
	{
		for (unsigned int j = 0;j < m_Animations.size();++j)
		{
			if (m_Animations[j]->BelongsTo(m_aDialog[i]))
			{
				m_Animations[j]->SetPos(m_aDialog[i]->GetPos());
			}
		}
	}
}

void CECBaseUIMan::RemoveAnimation(PAUIDIALOG pDlg)
{
	for (unsigned int i(0); i < m_Animations.size(); ++i)
	{
		CECUIAnimation* pAni = m_Animations[i];
		if (pAni->BelongsTo(pDlg))
		{
			pAni->Reset();
			m_Animations.erase(m_Animations.begin()+i);
			delete pAni;
			break;
		}
	}
}

void CECBaseUIMan::NavigateURL(const ACString &strURL, const char *szWinExplorer, bool bByCoreClient, CECURLOSNavigator *pOSNavigatorInWinExplorer)
{
	//	szWinExplorer:	内置 IE 浏览窗口的名称，为 NULL 则使用外部浏览器打开
	//	bByCoreClient:	为 true 时优先通过 Core Overlay 进行网页浏览。
	//					为 true 时浏览函数返回失败或为 false 时通过 szWinExplorer 中指定内置IE浏览
	//	pOSNavigatorInWinExplorer : 当确定使用 szWinExplorer 指定的对话框进行浏览时，szWinExplorer 窗口上在外部打开按钮关联的参数
	if (strURL.IsEmpty())
		return;

	glb_LogURL(AC2AS(strURL));

	if (g_bIgnoreURLNavigate)
	{
		//	忽略导航操作，主要用于获取访问链接在外部进行测试（如 sso ticket）
		return;
	}

	if (bByCoreClient){
		using namespace CC_SDK;
		if (ArcOverlay::Instance().IsLoaded() &&
			ArcOverlay::Instance().GotoURL(strURL)){
			return;
		}
	}
	
	while (true)
	{
		if (!CECUIConfig::Instance().GetGameUI().bEnableIE)
			break;

		if (!szWinExplorer)
			break;
		
		CDlgExplorer *pDlgExplorer = dynamic_cast<CDlgExplorer *>(GetDialog(szWinExplorer));
		if (!pDlgExplorer)
			break;
		
		if (!pDlgExplorer->IsShow())
			pDlgExplorer->Show(true);
		
		pDlgExplorer->NavigateUrl(strURL, pOSNavigatorInWinExplorer);
		return;
	}
	
	ShellExecute(NULL,_AL("open"), strURL, NULL, NULL, SW_SHOWNORMAL);
}

bool CECBaseUIMan::PreTranslateMessage(MSG &msg)
{
	bool bProcess(false);
	for (int i = 0; i < GetWebBrowserDlgCount(); ++ i)
	{
		CDlgExplorer *pDlgExplorer = GetWebBrowserDlg(i);
		if (pDlgExplorer &&
			pDlgExplorer->IsShow() &&
			pDlgExplorer->GetExplorerMan() &&
			::PreTranslateMessage(msg, pDlgExplorer->GetExplorerMan()->GetBrowser()))
		{
			bProcess = true;
			break;
		}
	}
	return bProcess;
}

int	   CECBaseUIMan::GetWebBrowserDlgCount()
{
	return (int)m_webBrowserDialogs.size();
}

CDlgExplorer * CECBaseUIMan::GetWebBrowserDlg(int index)
{
	CDlgExplorer *pDlgExplorer = NULL;
	if (index >= 0 && index < GetWebBrowserDlgCount())
		pDlgExplorer = m_webBrowserDialogs[index];
	return pDlgExplorer;
}

bool CECBaseUIMan::IsWebBrowser(HWND hWnd)
{
	bool bRet(false);
	
	if (hWnd)
	{
		for (int i = 0; i < GetWebBrowserDlgCount(); ++ i)
		{
			CDlgExplorer *pDlgExplorer = GetWebBrowserDlg(i);
			if (pDlgExplorer &&
				pDlgExplorer->GetExplorerMan() &&
				pDlgExplorer->GetExplorerMan()->GetBrowser() &&
				pDlgExplorer->GetExplorerMan()->GetBrowser()->GetCtrl() == hWnd)
			{
				bRet = true;
				break;
			}
		}
	}

	return bRet;
}

bool CECBaseUIMan::IsDialogBefore(PAUIDIALOG pDialogBefore, PAUIDIALOG pDialogAfter)
{
	//	对话框 pDialogBefore 与 pDialogAfter 在同一队列中且 pDialogBefore 在 pDialogAfter 前时返回 true

	while (true)
	{
		if (!pDialogBefore || !pDialogAfter || pDialogBefore == pDialogAfter ||
			pDialogBefore->IsBackgroundDialog() != pDialogAfter->IsBackgroundDialog())
		{
			ASSERT(false);
			break;
		}

		const APtrList<PAUIDIALOG> &zOrderList = pDialogBefore->IsBackgroundDialog() ? m_DlgZOrderBack : m_DlgZOrder;

		if (!zOrderList.Find(pDialogBefore) || !zOrderList.Find(pDialogAfter))
		{
			ASSERT(false);
			break;
		}

		ALISTPOSITION pos = zOrderList.GetHeadPosition();
		while (pos)
		{
			PAUIDIALOG pDlgCur = zOrderList.GetNext(pos);
			if (pDlgCur == pDialogBefore)
			{
				//	先找到 pDialogBefore，返回 true
				return true;
			}
			if (pDlgCur == pDialogAfter)
			{
				//	先找到 pDialogAfter，返回 false
				return false;
			}
		}
	}
	return false;
}

void CECBaseUIMan::ScaleDialogToAllVisibleByHeight(const char *szDialogName){
	if (PAUIDIALOG pDlg = GetDialog(szDialogName)){
		int defaultHeight = pDlg->GetDefaultSize().cy;
		float fScale = 1.0f;
		if (defaultHeight > m_rcWindow.Height()){
			fScale = m_rcWindow.Height()/(float)defaultHeight;
			fScale /= GetWindowScale();
		}
		pDlg->SetScale(fScale);
	}else{
		ASSERT(false);
	}
}

void CECBaseUIMan::SetLinearFilter(const char *szDialogName){
	PAUIDIALOG pDlg = GetDialog(szDialogName);
	if (!pDlg){
		ASSERT(false);
		return;
	}
	PAUIOBJECTLISTELEMENT pElement = pDlg->GetFirstControl();
	while (pElement)
	{
		PAUIOBJECTLISTELEMENT pNext = pDlg->GetNextControl(pElement);
		if (pElement->pThis->GetType() == AUIOBJECT_TYPE_STILLIMAGEBUTTON){
			PAUISTILLIMAGEBUTTON pButton = static_cast<PAUISTILLIMAGEBUTTON>(pElement->pThis);
			int frames[] = {AUISTILLIMAGEBUTTON_FRAME_UP,
				AUISTILLIMAGEBUTTON_FRAME_DOWN,
				AUISTILLIMAGEBUTTON_FRAME_ONHOVER,
				AUISTILLIMAGEBUTTON_FRAME_DISABLED,
			};
			for (int i(0); i < ARRAY_SIZE(frames); ++ i){
				if (PAUIFRAME pFrame = pButton->GetFrame(frames[i])){
					if (pFrame->GetA2DSprite()){
						pFrame->GetA2DSprite()->SetLinearFilter(true);
					}
				}
			}
		}
		pElement = pNext;
	}
}

AString CECBaseUIMan::GetURL(const char *szSec, const char *szKey)
{
	AString strURL;
	
	AIniFile iniFile;
	if(iniFile.Open("configs\\loginpage.ini"))
	{
		strURL = iniFile.GetValueAsString(szSec, szKey, "");
		
		iniFile.Close();
		
		strURL.TrimLeft();
		strURL.TrimRight();
	}

	return strURL;
}

ACString CECBaseUIMan::GetFormatNumber(__int64 i) const
{
	ACString szRes = g_pGame->GetFormattedPrice(i);
	return szRes;
}

ACString CECBaseUIMan::GetFormatTime(int time)
{
	const int TIME_DAY = 3600 * 24;
	const int TIME_HOUR = 3600;
	const int TIME_MINUTE = 60;
	
	ACString szTime;
	
	if( time > TIME_DAY + TIME_HOUR )
	{
		szTime.Format(GetStringFromTable(5535), time / TIME_DAY, (time % TIME_DAY) / TIME_HOUR);
	}
	else if( time > TIME_HOUR + TIME_MINUTE )
	{
		szTime.Format(GetStringFromTable(5536), time / TIME_HOUR, (time % TIME_HOUR) / TIME_MINUTE);
	}
	else if( time > TIME_MINUTE )
	{
		szTime.Format(GetStringFromTable(5537), time / TIME_MINUTE, time % TIME_MINUTE);
	}
	else
	{
		szTime.Format(GetStringFromTable(5538), time);
	}
	
	return szTime;
}

ACString CECBaseUIMan::GetFormatTimeSimple(int time)
{
	const int TIME_DAY = 3600 * 24;
	const int TIME_HOUR = 3600;
	const int TIME_MINUTE = 60;
	
	ACString szTime;
	if( time < 0 )
		szTime = GetStringFromTable(1600);
	else if( time > TIME_DAY )
		szTime.Format(GetStringFromTable(1604), time / TIME_DAY);
	else if( time > TIME_HOUR )
		szTime.Format(GetStringFromTable(1603), time / TIME_HOUR);
	else if( time > TIME_MINUTE )
		szTime.Format(GetStringFromTable(1602), time / TIME_MINUTE);
	else
		szTime.Format(GetStringFromTable(1601), time);
	
	return szTime;
}

ACString CECBaseUIMan::GetShopItemFormatTime(int nTime)
{
	ACString szTime;
	if( nTime == 0 )
		szTime = GetStringFromTable(1600);
	else if( nTime % (24 * 3600) == 0 )
		szTime.Format(GetStringFromTable(1604), nTime / 24 / 3600);
	else if( nTime % 3600 == 0 )
		szTime.Format(GetStringFromTable(1603), nTime / 3600);
	else if( nTime % 60 == 0 )
		szTime.Format(GetStringFromTable(1602), nTime / 60);
	else
		szTime.Format(GetStringFromTable(1601), nTime);
	
	return szTime;
}

A3DCOLOR CECBaseUIMan::GetPriceColor(__int64 i) const
{
	ACString strColor;
	
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	if( i >= 100000000) // 100 million
		strColor = pDescTab->GetWideString(ITEMDESC_COL_GREEN);
	else if ( i >= 10000000) // 10 million
		strColor = pDescTab->GetWideString(ITEMDESC_COL_DARKGOLD);
	else if ( i >= 1000000) // 1 million
		strColor = pDescTab->GetWideString(ITEMDESC_COL_YELLOW);
	
	A3DCOLOR col;
	return STRING_TO_A3DCOLOR(strColor, col) ? col : A3DCOLORRGB(255, 255, 255);
}

A3DCOLOR CECBaseUIMan::GetMsgBoxColor()
{
	return A3DCOLORRGBA(255, 255, 255, 160);
}

void CECBaseUIMan::ShowErrorMsg(const ACHAR *pszMsg, const char *pszName)
{
	MessageBox(pszName, pszMsg, MB_OK, GetMsgBoxColor());
}

void CECBaseUIMan::ShowErrorMsg(int idString)
{
	ShowErrorMsg(GetStringFromTable(idString));
}

void CECBaseUIMan::ShowReconnectMsg(const ACHAR *pszMsg, const char *pszName)
{
	MessageBox(pszName, pszMsg, MB_YESNO, GetMsgBoxColor());
}

bool CECBaseUIMan::NavigateTWRechargeURL(){
	bool result(false);
	const CECUIConfig::GameUI &gameUI = CECUIConfig::Instance().GetGameUI();
	if (gameUI.bEnableTWRecharge){
		AString strFormat = GetURL("SHOP", "TWRECHARGE");
		if (!strFormat.IsEmpty()){
			AString strUser = g_pGame->GetGameSession()->GetTicketAccount();
			time_t ts = g_pGame->GetServerAbsTime();
			int zoneid = g_pGame->GetGameSession()->GetZoneID();
			
			AString strToken;
			AString strUserLower = strUser;
			strUserLower.MakeLower();
			AString strGame = gameUI.strTWRechargeGame;
			strGame.MakeLower();
			strToken.Format("%s|%u|%s|%s|%d|%s", gameUI.strTWRechargeAppID, ts, strGame, strUserLower, zoneid, gameUI.strTWRechargeKey);
			GNET::MD5Hash md5;
			md5.Update(Octets(strToken, strToken.GetLength()));
			GNET::Octets tokenMD5;
			md5.Final(tokenMD5);
			strToken = AString((const char *)tokenMD5.begin(), tokenMD5.size());
			strToken = glb_ConverToHex(strToken);
			
			AString strURL;
			strURL.Format(strFormat, gameUI.strTWRechargeAppID, strUser, gameUI.strTWRechargeGame, zoneid, ts, strToken);
			NavigateURL(strURL, NULL);
		}
		result = true;
	}
	return result;
}

void CECBaseUIMan::NavigateDefaultRechargeURL(){	
	AString strUrl = GetURL("SHOP", "RECHARGE");
	NavigateURL(strUrl, NULL);
}

void CECBaseUIMan::NavigateRechargeURL(){
	if (NavigateTWRechargeURL()){
		return;
	}
	NavigateDefaultRechargeURL();
}