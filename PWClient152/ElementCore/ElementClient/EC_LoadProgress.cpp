/*
 * FILE: EC_LoadProgress.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/5/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_LoadProgress.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_Viewport.h"
#include "EC_ContinueLoad.h"

#include "AAssist.h"
#include "A2DSprite.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DMacros.h"
#include "A3DViewport.h"
#include "EC_UIManager.h"
#include "AUIDialog.h"
#include "AWScriptFile.h"
#include <AFI.h>

#include <AWIniFile.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static abase::vector<AWString> vecTip;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

void CECLoadingUIMan::CalcWindowScale()
{
	CECConfigs *pConfig = g_pGame->GetConfigs();
	if (!pConfig || pConfig->GetSystemSettings().bScaleUI)
	{
		// 如果选项中允许缩放，则调用基类默认处理缩放的方法
		AUILuaManager::CalcWindowScale();
		a_Clamp(m_fWindowScale, 0.78125f, 1.0f);
	}
	else
	{
		// 否则，保持不缩放
		m_fWindowScale = 1.0f;
	}
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECLoadProgress
//	
///////////////////////////////////////////////////////////////////////////

CECLoadProgress::CECLoadProgress(bool bSkipRender)
{
	m_pBackPic	= NULL;
	m_pBarPic	= NULL;
	m_Pos		= 0.0f;
	m_Min		= 0;
	m_Max		= 100;
	m_Step		= 10;
	m_pUIManager = NULL;
	m_bSkipRender = bSkipRender;
}

CECLoadProgress::~CECLoadProgress()
{
}

static int GetImageCount(const char *szFormat)
{
	int count = 0; // the number of background image in loading process
	AString strFile;
	while (true)
	{
		strFile.Format(szFormat, count+1);
		if (!af_IsFileExist(strFile)) break;
		count ++;
	}
	return count;
}

static void GetProcessBarPosition(const char *szKey, int &left, int &top){
	left = top = 0;
	AWIniFile ItemFile;	
	const char *szProcBarIni = "surfaces\\procbar.ini";
	if (ItemFile.Open(szProcBarIni)){
		ACString strKey = AS2AC(szKey);
		left = ItemFile.GetValueAsInt(strKey, _AL("left"), 0);
		top = ItemFile.GetValueAsInt(strKey, _AL("top"), 0);
		ItemFile.Close();
	}else{
		a_LogOutput(1, "Failed to open file %s", szProcBarIni);
	}
}

//	Initialize object
bool CECLoadProgress::Init(float Min, float Max)
{
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	A3DEngine* pA3DEngine = g_pGame->GetA3DEngine();

	int BACKGROUND_IMAGE_NUM_1024 = GetImageCount("surfaces\\载入1024-%d.jpg");
	int BACKGROUND_IMAGE_NUM_1280 = GetImageCount("surfaces\\载入1280-%d.jpg");
	int BACKGROUND_IMAGE_NUM_16_9 = GetImageCount("surfaces\\载入16-9-%d.jpg");
	
	if (BACKGROUND_IMAGE_NUM_1024 < 1)
	{
		ASSERT(false);
		a_LogOutput(1, "missing loading image 载入1024-1.jpg.");
		return false;
	}
	if (BACKGROUND_IMAGE_NUM_1280 < 1)
	{
		ASSERT(false);
		a_LogOutput(1, "missing loading image 载入1280-1.jpg.");
		return false;
	}
	if (BACKGROUND_IMAGE_NUM_16_9 < 1)
	{
		ASSERT(false);
		a_LogOutput(1, "missing loading image 载入16-9-1.jpg.");
		return false;
	}

	int iWid = g_pGame->GetConfigs()->GetSystemSettings().iRndWidth;
	int iHei = g_pGame->GetConfigs()->GetSystemSettings().iRndHeight;
	float fAspect = (float)iHei / iWid;
	AWScriptFile s;
	AWString strTip;
	if (vecTip.empty())
	{
		if( s.Open("Interfaces\\Tips.txt") )
		{
			while( s.PeekNextToken(true) )
			{
				int n = s.GetNextTokenAsInt(true);
				s.GetNextToken(true);
				strTip = s.m_szToken;
				vecTip.push_back(strTip);
			}
			s.Close();
		}
		else
			AUI_ReportError(__LINE__, "CECLoadProgress::Init, Tips.txt error!");
	}
	int nDefaultWid = 0;
	int nDefaultHei = 0;

	int BACKGROUND_IMAGE_NUM = 0;
	const char *szBackFormat = NULL;

	const char* szProcBar = NULL;
	if (fabs(fAspect-0.75f) < 0.001f)
	{
		nDefaultWid = 1024;
		nDefaultHei = 768;
		
		BACKGROUND_IMAGE_NUM = BACKGROUND_IMAGE_NUM_1024;
		szBackFormat = "载入1024-%d.jpg";
		szProcBar = "ProcBar-1024.tga";
	}
	else if (fabs(fAspect-0.8f) < 0.001f)
	{
		nDefaultWid = 1280;
		nDefaultHei = 1024;
		
		BACKGROUND_IMAGE_NUM = BACKGROUND_IMAGE_NUM_1280;
		szBackFormat = "载入1280-%d.jpg";
		szProcBar = "ProcBar-1280.tga";
	}
	else if(fabs(fAspect - 9.0f/16.0f) < 0.001f)
	{
		nDefaultWid = 1280;
		nDefaultHei = 720;
		
		BACKGROUND_IMAGE_NUM = BACKGROUND_IMAGE_NUM_16_9;
		szBackFormat = "载入16-9-%d.jpg";
		szProcBar = "ProcBar-16-9.tga";
	}
	else	//	Arbitrary aspect ratio
	{
		nDefaultWid = 1280;
		nDefaultHei = 1024;
		
		BACKGROUND_IMAGE_NUM = BACKGROUND_IMAGE_NUM_1280;
		szBackFormat = "载入1280-%d.jpg";
		szProcBar = "ProcBar-1280.tga";
	}	
	
	float fScaleX = iWid / (float)nDefaultWid;
	float fScaleY = iHei / (float)nDefaultHei;
	
	//	Load background picture
	char szBackFile[MAX_PATH] = {0};
	int iBackImage = a_Random(100, BACKGROUND_IMAGE_NUM * 100 + 99) / 100;
	bool bContinueLastLoad = CECContinueLoad::Instance().IsContinueLastLoad();
	if (bContinueLastLoad)
	{
		int iLastBackImage = CECContinueLoad::Instance().GetSaveBackImage();
		if (iLastBackImage > 0 && iLastBackImage <= BACKGROUND_IMAGE_NUM)
		{
			//	使用上次资源，以合并进度，用于转服中两次地图资源加载
			iBackImage = iLastBackImage;
		}
	}
	CECContinueLoad::Instance().SetSaveBackImage(iBackImage);

	sprintf(szBackFile, szBackFormat, iBackImage);
	m_pBackPic = new A2DSprite;
	if (!m_pBackPic->Init(pA3DDevice, szBackFile, 0))
	{
		delete m_pBackPic;
		m_pBackPic = NULL;
	}
	else
	{
		m_pBackPic->SetScaleX(fScaleX);
		m_pBackPic->SetScaleY(fScaleY);
		m_pBackPic->SetLinearFilter(true);
	}
	
	//	Load process bar picture
	m_pBarPic = new A2DSprite;
	if (!m_pBarPic->Init(pA3DDevice, szProcBar, 0))
	{
		delete m_pBarPic;
		m_pBarPic = NULL;
	}
	else
	{
		m_pBarPic->SetScaleX(fScaleX);
		m_pBarPic->SetScaleY(fScaleY);
		m_pBarPic->SetLinearFilter(true);
	}
	
	//	Set process bar area
	int BAR_LEFT(0), BAR_TOP(0);
	GetProcessBarPosition(szProcBar, BAR_LEFT, BAR_TOP);
	m_rcBar.SetRect(BAR_LEFT, BAR_TOP, BAR_LEFT + m_pBarPic->GetWidth(), BAR_TOP + m_pBarPic->GetHeight());
	if (iWid != nDefaultWid || iHei != nDefaultHei)
	{
		m_rcBar.left	= (int)(m_rcBar.left * fScaleX + 0.5f);
		m_rcBar.top		= (int)(m_rcBar.top * fScaleY + 0.5f);
		m_rcBar.right	= (int)(m_rcBar.right * fScaleX + 0.5f);
		m_rcBar.bottom	= (int)(m_rcBar.bottom * fScaleY + 0.5f);
	}
	if (m_pBarPic)
		m_pBarPic->SetPosition(m_rcBar.left, m_rcBar.top);
	
	m_NextValid.x = 0;
	m_NextValid.y = 0;

	A3DDEVFMT fmt = pA3DDevice->GetDevFormat();
	m_pUIManager = new CECLoadingUIMan;
	if (!m_pUIManager->Init(pA3DEngine, pA3DDevice, NULL, fmt.nWidth, fmt.nHeight))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECUIManager::ChangeCurUIManager", __LINE__);
		return false;
	}
	if( m_pUIManager )
	{
		m_pUIManager->CreateDlg(g_pGame->GetA3DEngine(), g_pGame->GetA3DDevice(), "Loading.xml");
 		PAUIDIALOG pDlg = m_pUIManager->GetDialog("Win_Loading");
		pDlg->SetCanOutOfWindow(true);
		SIZE size = pDlg->GetSize();
		int posy = (m_rcBar.bottom + fmt.nHeight)/2 - size.cy/2;
		posy += 5;	//	离进度条装饰图片稍远些
		a_ClampRoof(posy, (int)(fmt.nHeight - size.cy));
 		pDlg->SetPosEx(0, posy);

		PAUIOBJECT pTxt_Tip = pDlg->GetDlgItem("Txt_Tip");
		PAUIOBJECT pLab_Tip = pDlg->GetDlgItem("Lab_Tip");
		pLab_Tip->Show(false);

		if (vecTip.empty())
		{
			pTxt_Tip->SetText(_AL(""));
			CECContinueLoad::Instance().SetSaveTipIndex(-1);
		}
		else
		{
			ACString strPrefix = pLab_Tip->GetText();
			size_t iTipIndex = a_Random(0, vecTip.size()-1);
			if (bContinueLastLoad)
			{
				int iLastTipIndex = CECContinueLoad::Instance().GetSaveTipIndex();
				if (iLastTipIndex >= 0 && iLastTipIndex < vecTip.size())
					iTipIndex = iLastTipIndex;
			}
			CECContinueLoad::Instance().SetSaveTipIndex(iTipIndex);
			ACString strContent = vecTip[iTipIndex];
			pTxt_Tip->SetText(strPrefix + strContent);
			pTxt_Tip->SetSize(iWid, pTxt_Tip->GetSize().cy);
		}
 		pDlg->Show(true, true, true);
 	}

	SetProgressRange(Min, Max);
	if (bContinueLastLoad)
		SetProgressPos(CECContinueLoad::Instance().GetSaveLoadPos());

	return true;
}

//	Release object
void CECLoadProgress::Release()
{
	//	保存供转服等连续地图加载时第二次加载使用
	CECContinueLoad::Instance().SetSaveLoadPos(GetProgressPos());

	A3DRELEASE(m_pBackPic);
	A3DRELEASE(m_pBarPic);
	if (m_pUIManager)
	{
		m_pUIManager->Release();
		delete m_pUIManager;
		m_pUIManager = NULL;
	}
}

//	Advances the current position
float CECLoadProgress::OffsetProgressPos(float offset)
{
	if (CECContinueLoad::Instance().IsInMergeLoad())
	{
		//	转服等连续2次加载时，每次加载中进度都只走一半
		m_Pos += offset * 0.5f;
	}
	else m_Pos += offset;
	a_Clamp(m_Pos, m_Min, m_Max);
	return m_Pos;
}

//	Sets the current position 
void CECLoadProgress::SetProgressPos(float Pos)
{
	m_Pos = Pos;
	a_Clamp(m_Pos, m_Min, m_Max);
}

//	Sets the minimum and maximum ranges
void CECLoadProgress::SetProgressRange(float Min, float Max)
{
	ASSERT(Min < Max);
	m_Min = Min;
	m_Max = Max;
	a_Clamp(m_Pos, m_Min, m_Max);
}

//	Advances the current position by the step increment
float CECLoadProgress::StepProgress()
{
	m_Pos += m_Step;
	a_Clamp(m_Pos, m_Min, m_Max);
	return m_Pos;
}

//	Render routine
bool CECLoadProgress::Render(bool forceUpdate)
{
	if (!m_pBackPic || !m_pBarPic)
		return true;

	if(!UpdateBar() && !forceUpdate)
		return true;

	if (m_bSkipRender){
		return true;
	}

	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	A3DVIEWPORTPARAM* pvp = g_pGame->GetViewport()->GetA3DViewport()->GetParam();

	pA3DDevice->BeginRender();

	A3DDEVFMT devFmt = g_pGame->GetA3DDevice()->GetDevFormat();
	A3DVIEWPORTPARAM viewParam;
	viewParam.X = 0;
	viewParam.Y = 0;
	viewParam.Width  = devFmt.nWidth;
	viewParam.Height = devFmt.nHeight;
	viewParam.MinZ = 0.0f;
	viewParam.MaxZ = 1.0f;
	
	g_pGame->GetA3DDevice()->SetViewport(&viewParam);
	g_pGame->GetA3DDevice()->Clear(D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	m_pBackPic->DrawToBack(0, 0);
	
	m_pBarPic->DrawToBack();
	
	PAUIDIALOG pDlg = m_pUIManager->GetDialog("Win_Loading");
	if (pDlg)
	{
		A3DVIEWPORTPARAM oldViewParam = *(g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam());
		g_pGame->GetA3DEngine()->GetActiveViewport()->SetParam(&viewParam);
		pDlg->Render();
		g_pGame->GetA3DEngine()->GetActiveViewport()->SetParam(&oldViewParam);
	}
	
	pA3DDevice->EndRender();
	pA3DDevice->Present();

	return true;
}

bool CECLoadProgress::UpdateBar()
{
	int nWidth = m_pBarPic->GetWidth();
	int nHeight = m_pBarPic->GetHeight();
	float step = nWidth / (m_Max - m_Min); 
	float fLen = step * (m_Pos - m_Min);
	int iLen = (int)(fLen+0.5f);
	a_ClampRoof(iLen, nWidth);

	// only check x because this bar is horizontal
	if(iLen < m_NextValid.x)
	{
		return false;
	}
	m_NextValid.x = iLen + min((int)step, 5);
	a_ClampRoof(m_NextValid.x, (LONG)nWidth);
	
	int barWid = min(nWidth, iLen);
	int leftWid = max(0, nWidth - iLen);

	// select the first rect
	A3DRECT rects[2] = {	A3DRECT(0,0,barWid,nHeight),
							A3DRECT(barWid, 0, leftWid,nHeight)	};
	m_pBarPic->ResetItems(2, rects);
	m_pBarPic->SetCurrentItem(0);

	return true;
}