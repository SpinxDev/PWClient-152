/*
 * FILE: EC_LoginUIMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "AFI.h"
#include "AIniFile.h"

#include "EC_CDS.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_IvtrTypes.h"
#include "EC_LoginUIMan.h"
#include "EC_ManPlayer.h"
#include "EC_ShadowRender.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_LoginPlayer.h"
#include "EC_Configs.h"
#include "EL_BackMusic.h"
#include "EC_Resource.h"
#include "EC_CommandLine.h"
#include "CSplit.h"
#include "gameclient.h"
#include "EC_Utility.h"
#include "EC_GFXCaster.h"
#include "EC_UIConfigs.h"
#include "EC_TimeSafeChecker.h"
#include "EC_Reconnect.h"
#include "EC_ContinueLoad.h"
#include "EC_CustomizePolicy.h"
#include "EC_LoginQueue.h"

#include "gnetdef.h"
#include "Network/ssogetticket_re.hpp"

#include "A3DEngine.h"
#include "A2DSprite.h"
#include "A3DCamera.h"
#include "A3DViewport.h"
#include "A3DFont.h"
#include "A3DSkinRender.h"
#include "A3DSkinMan.h"
#include "A3DCollision.h"
#include "A3DFuncs.h"
#include "A3DTerrainWater.h"
#include <AFilePackMan.h>

#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"

#include "DlgCustomizeFace.h"
#include "DlgCustomizeEye.h"
#include "DlgCustomizeThirdEye.h"
#include "DlgCustomizeEyebrow.h"
#include "DlgCustomizeHair.h"
#include "DlgCustomizeEar.h"
#include "DlgCustomizeMouth.h"
#include "DlgCustomizeNose.h"
#include "DlgCustomizePaint.h"
#include "DlgCustomizePre.h"
#include "DlgCustomizePreForFree.h"
#include "DlgCustomizeStature.h"
#include "DlgCustomizeAdv.h"
#include "DlgCustomizeUserPic.h"
#include "DlgCustomizeMoveCamera.h"
#include "DlgCustomize.h"
#include "DlgCustomizeChsPre.h"
#include "DlgExplorer.h"
#include "DlgFaceName.h"

#include "DlgLoginServerList.h"
#include "DlgLoginVersion.h"
#include "DlgFindServer.h"
#include "DlgLoginPage.h"
#include "DlgCWindow.h"
#include "EC_UIManager.h"
#include "DlgPwdHint.h"
#include "DlgCreate.h"
#include "DlgLogin.h"
#include "DlgLoginButton.h"
#include "DlgLoginOther.h"
#include "DlgAgreement.h"
#include "DlgSelect.h"
#include "DlgPwdProtect.h"
#include "DlgWebRoleTrade.h"
#include "DlgWebTradeInfo.h"
#include "DlgLoginQueue.h"

#include "playerpositionresetrqst.hrp"

#include "EC_CrossServer.h"
#include "EC_ServerList.h"
#include "EC_MCDownload.h"
#include "EC_RecordAccount.h"
#include "EC_ProfConfigs.h"
#include "EC_ProfDisplay.h"
#include "EC_LoginSwitch.h"
#include "Arc/Asia/EC_ArcAsia.h"

#include "elementdataman.h"

#define new A_DEBUG_NEW

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

static const A3DVECTOR3 s_camPosDelta[NUM_PROFESSION][2] = 
{
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
	A3DVECTOR3(0.0f, 0.2f, 0.0f), A3DVECTOR3(0.0f),
};

static bool s_bNeedTestRoleAutoLogin = true;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECLoginUIMan
//	
///////////////////////////////////////////////////////////////////////////
char CECLoginUIMan::m_passwdFlag = 0;
char CECLoginUIMan::m_usbBind = 0;
void CECLoginUIMan::SetUsbBind(char usbbind)
{
	if(m_usbBind >= 0 || usbbind > 0)
	{
		m_usbBind = usbbind;
	}
}

CECLoginUIMan::CECLoginUIMan()
{
	m_curScene = LOGIN_SCENE_NULL;

	m_idCurRole = -1;
	m_roleDisplayActionTrigger = new ActionTrigger;

	m_nCurProfession = m_nCurGender = -1;
	memset(m_aModel, 0, sizeof(m_aModel));
	m_bNewCharEquipShow = true;

	m_bReferrerBound = false;

	m_bSelectCharDragRole = false; // 登陆选择角色界面旋转人物标志

	m_roleListReady = false;
	m_bSelectRoleSent = false;
}

CECLoginUIMan::~CECLoginUIMan()
{
}

//	Initialize manager
bool CECLoginUIMan::Init(A3DEngine* pA3DEngine, A3DDevice* pA3DDevice, const char* szDCFile/* NULL */)
{
	int i;
	bool bval;
	float x, y, z;
	AIniFile theIni;
	char szKey[40], szFile[MAX_PATH];

	sprintf(szFile, "%s\\Configs\\SceneCtrl.ini", af_GetBaseDir());
	theIni.Open(szFile);

	for( i = 0; i < LOGIN_SCENE_MAX; i++ )
	{
		sprintf(szKey, "PosX%d", i);
		x = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		sprintf(szKey, "PosY%d", i);
		y = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		sprintf(szKey, "PosZ%d", i);
		z = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		m_aCamPos[i] = A3DVECTOR3(x, y, z);

		sprintf(szKey, "DirX%d", i);
		x = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		sprintf(szKey, "DirY%d", i);
		y = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		sprintf(szKey, "DirZ%d", i);
		z = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		m_aCamDir[i] = A3DVECTOR3(x, y, z);

		sprintf(szKey, "UpX%d", i);
		x = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		sprintf(szKey, "UpY%d", i);
		y = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		sprintf(szKey, "UpZ%d", i);
		z = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		m_aCamUp[i] = A3DVECTOR3(x, y, z);
	}

	for( i = 0; i < LOGIN_SCENE_CREATE_END - LOGIN_SCENE_CREATE_BEGIN + 1; i++ )
	{
		sprintf(szKey, "PosX%d", i);
		x = theIni.GetValueAsFloat("NewChar", szKey, 0.0f);
		sprintf(szKey, "PosY%d", i);
		y = theIni.GetValueAsFloat("NewChar", szKey, 0.0f);
		sprintf(szKey, "PosZ%d", i);
		z = theIni.GetValueAsFloat("NewChar", szKey, 0.0f);
		m_aCreatePos[i] = A3DVECTOR3(x, y, z);
	}
	{
		sprintf(szKey, "PosX%d", 0);
		x = theIni.GetValueAsFloat("NewCharCenter", szKey, 0.0f);
		sprintf(szKey, "PosY%d", 0);
		y = theIni.GetValueAsFloat("NewCharCenter", szKey, 0.0f);
		sprintf(szKey, "PosZ%d", 0);
		z = theIni.GetValueAsFloat("NewCharCenter", szKey, 0.0f);
		m_aCreateDisplayCenter = A3DVECTOR3(x, y, z);
		m_aCreateDisplayRadius = theIni.GetValueAsFloat("NewCharCenter", "Radius", RotateAnimator::RA_DEFAULT_RADIUS);
		m_aCreateDeltaDist	= theIni.GetValueAsFloat("NewCharCenter", "DeltaDist", RotateAnimator::RA_DEFAULT_DELTA_DIST);
		m_aCreateSelectMove = theIni.GetValueAsFloat("NewCharCenter", "SelectMove", RotateAnimator::RA_DEFAULT_SELECT_MOVE);
		RotateAnimator::Instance().SetCenter(m_aCreateDisplayCenter);
		RotateAnimator::Instance().SetRadius(m_aCreateDisplayRadius);
		RotateAnimator::Instance().SetDeltaDist(m_aCreateDeltaDist);
		RotateAnimator::Instance().SetSelectMove(m_aCreateSelectMove);
	}
	for (i = 0; i < NUM_PROFESSION; ++ i)
	{
		sprintf(szKey, "Weapon%d", i);
		m_aCreateEquips[i][NEWCHAR_WEAPON] = theIni.GetValueAsInt("NewCharEquip", szKey, 0);
		sprintf(szKey, "UpperBody%d", i);
		m_aCreateEquips[i][NEWCHAR_UPPERBODY] = theIni.GetValueAsInt("NewCharEquip", szKey, 0);
		sprintf(szKey, "LowerBody%d", i);
		m_aCreateEquips[i][NEWCHAR_LOWERBODY] = theIni.GetValueAsInt("NewCharEquip", szKey, 0);
		sprintf(szKey, "Wrist%d", i);
		m_aCreateEquips[i][NEWCHAR_WRIST] = theIni.GetValueAsInt("NewCharEquip", szKey, 0);
		sprintf(szKey, "Foot%d", i);
		m_aCreateEquips[i][NEWCHAR_FOOT] = theIni.GetValueAsInt("NewCharEquip", szKey, 0);
		sprintf(szKey, "Wing%d", i);
		m_aCreateEquips[i][NEWCHAR_WING] = theIni.GetValueAsInt("NewCharEquip", szKey, 0);
	}
	
	theIni.Close();

	// force to load the default setting
	ImportStringTable("loginui.stf");
	ImportUIParam("loginui.xml");

	CECServerList::Instance().ResetPing();
	CECRecordAccount::Instance().Load();

	bval = CECBaseUIMan::Init(pA3DEngine, pA3DDevice, szDCFile);
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);	

	LoadRaceDisplayModels();
	
	m_pDlgLogin = dynamic_cast<CDlgLogin*>(GetDialog("Win_Login"));
	m_pDlgSelect = dynamic_cast<CDlgSelect*>(GetDialog("Win_Select"));
	m_pDlgTrade = dynamic_cast<CDlgWebRoleTrade*>(GetDialog("Win_Webroletrade"));
	m_pDlgLoginQueueVIP = dynamic_cast<CDlgLoginQueue*>(GetDialog(CDlgLoginQueue::GetDialogNameForVIPQueue()));
	m_pDlgLoginQueueNormal = dynamic_cast<CDlgLoginQueue*>(GetDialog(CDlgLoginQueue::GetDialogNameForNormalQueue()));
	
	m_webBrowserDialogs.clear();
	const char *szWebBrowserDialogs[] = {"Win_Explorer", "Win_LoginOther"};
	for (i = 0; i < sizeof(szWebBrowserDialogs)/sizeof(szWebBrowserDialogs[0]); ++ i)
	{
		PAUIDIALOG pDlg = GetDialog(szWebBrowserDialogs[i]);
		if (!pDlg)	continue;
		CDlgExplorer *pDlgExplorer = dynamic_cast<CDlgExplorer *>(pDlg);
		if (!pDlgExplorer)	continue;
		m_webBrowserDialogs.push_back(pDlgExplorer);
	}

	RepositionLoginDialog();
	
	CDlgTheme *pDlgLoginButton = dynamic_cast<CDlgTheme *>(GetDialog("Win_LoginButton"));
	pDlgLoginButton->RegisterEnableDelay("Btn_ChangeServer");
	pDlgLoginButton->RegisterEnableDelay("Btn_Quit");
	pDlgLoginButton->RegisterEnableDelay("Btn_Link");
	CDlgTheme *pDlgManage = dynamic_cast<CDlgTheme *>(GetDialog("Win_Manage"));
	pDlgManage->RegisterEnableDelay("Btn_Return");
	CDlgTheme *pDlgCreateButton1 = dynamic_cast<CDlgTheme *>(GetDialog("Win_CreateButton1"));
	pDlgCreateButton1->RegisterEnableDelay("Btn_Confirm");
	pDlgCreateButton1->RegisterEnableDelay("Btn_Cancel");
	CDlgTheme *pDlgCreateButton2 = dynamic_cast<CDlgTheme *>(GetDialog("Win_CreateButton2"));
	pDlgCreateButton2->RegisterEnableDelay("Btn_Back");
	pDlgCreateButton2->RegisterEnableDelay("Btn_CancelCreate");
	
	PAUIDIALOG pDlgCreateRace = GetDialog("Win_CreateRace");
	for (i = 0; i < NUM_RACE; ++ i)
	{
		if (PAUISTILLIMAGEBUTTON pBtn_Race = dynamic_cast<PAUISTILLIMAGEBUTTON>(pDlgCreateRace->GetDlgItem(AString().Format("Btn_Race%d", i)))){
			pBtn_Race->SetText(GetStringFromTable(311+i));
			pBtn_Race->SetPushLike(true);
		}
	}
	pDlgCreateRace->GetDlgItem("Img_Gfx1")->SetTransparent(true);	//	遮挡住 Win_CreateProfession 时仍能透过
	pDlgCreateRace->GetDlgItem("Img_Gfx2")->SetTransparent(true);
	SetLinearFilter("Win_CreateRace");

	if (m_pDlgLoginQueueVIP){
		CECLoginQueue::Instance().RegisterObserver(m_pDlgLoginQueueVIP);
	}
	if (m_pDlgLoginQueueNormal){
		CECLoginQueue::Instance().RegisterObserver(m_pDlgLoginQueueNormal);
	}

	return true;
}

//	Release manager
bool CECLoginUIMan::Release(void)
{
	if (m_pDlgLoginQueueVIP){
		CECLoginQueue::Instance().UnregisterObserver(m_pDlgLoginQueueVIP);
	}
	if (m_pDlgLoginQueueNormal){
		CECLoginQueue::Instance().UnregisterObserver(m_pDlgLoginQueueNormal);
	}
	CECRecordAccount::Instance().Save();
	g_pGame->GetConfigs()->SaveSystemSettings();

	int i, j;

	for (i = 0; i < int(m_vecRoleModel.size()); i++)
		A3DRELEASE(m_vecRoleModel[i]);
	m_vecRoleModel.clear();

	delete m_roleDisplayActionTrigger;
	m_roleDisplayActionTrigger = NULL;

	m_vecRoleInfo.clear();
	
	for (i = 0; i < NUM_PROFESSION; i++)
	{
		for (j = 0; j < NUM_GENDER; j++)
			A3DRELEASE(m_aModel[i][j]);
	}
	m_bNewCharEquipShow = true;
	m_CustomizeMgr.Release();

	char szIniFile[MAX_PATH];
	sprintf(szIniFile, "Configs\\SceneCtrl.ini", af_GetBaseDir());
	AFilePackBase *pPack = g_AFilePackMan.GetFilePck(szIniFile);
	if (!pPack || !pPack->IsFileExist(szIniFile)){
		AIniFile theIni;
		sprintf(szIniFile, "%s\\Configs\\SceneCtrl.ini", af_GetBaseDir());
		char szKey[40];
		for( i = 0; i < LOGIN_SCENE_MAX; i++ )
		{
			sprintf(szKey, "PosX%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamPos[i].x);
			sprintf(szKey, "PosY%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamPos[i].y);
			sprintf(szKey, "PosZ%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamPos[i].z);
			
			sprintf(szKey, "DirX%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamDir[i].x);
			sprintf(szKey, "DirY%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamDir[i].y);
			sprintf(szKey, "DirZ%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamDir[i].z);
			
			sprintf(szKey, "UpX%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamUp[i].x);
			sprintf(szKey, "UpY%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamUp[i].y);
			sprintf(szKey, "UpZ%d", i);
			theIni.WriteFloatValue("Camera", szKey, m_aCamUp[i].z);
		}
		
		for( i = 0; i < LOGIN_SCENE_CREATE_END - LOGIN_SCENE_CREATE_BEGIN + 1; i++ )
		{
			sprintf(szKey, "PosX%d", i);
			theIni.WriteFloatValue("NewChar", szKey, m_aCreatePos[i].x);
			sprintf(szKey, "PosY%d", i);
			theIni.WriteFloatValue("NewChar", szKey, m_aCreatePos[i].y);
			sprintf(szKey, "PosZ%d", i);
			theIni.WriteFloatValue("NewChar", szKey, m_aCreatePos[i].z);
		}
		{
			sprintf(szKey, "PosX%d", 0);
			theIni.WriteFloatValue("NewCharCenter", szKey, m_aCreateDisplayCenter.x);
			sprintf(szKey, "PosY%d", 0);
			theIni.WriteFloatValue("NewCharCenter", szKey, m_aCreateDisplayCenter.y);
			sprintf(szKey, "PosZ%d", 0);
			theIni.WriteFloatValue("NewCharCenter", szKey, m_aCreateDisplayCenter.z);
			theIni.WriteFloatValue("NewCharCenter", "Radius", m_aCreateDisplayRadius);
			theIni.WriteFloatValue("NewCharCenter", "DeltaDist", m_aCreateDeltaDist);
			theIni.WriteFloatValue("NewCharCenter", "SelectMove", m_aCreateSelectMove);
		}	
		for (i = 0; i < NUM_PROFESSION; ++ i)
		{
			sprintf(szKey, "Weapon%d", i);
			theIni.WriteIntValue("NewCharEquip", szKey, m_aCreateEquips[i][NEWCHAR_WEAPON]);
			sprintf(szKey, "UpperBody%d", i);
			theIni.WriteIntValue("NewCharEquip", szKey, m_aCreateEquips[i][NEWCHAR_UPPERBODY]);
			sprintf(szKey, "LowerBody%d", i);
			theIni.WriteIntValue("NewCharEquip", szKey, m_aCreateEquips[i][NEWCHAR_LOWERBODY]);
			sprintf(szKey, "Wrist%d", i);
			theIni.WriteIntValue("NewCharEquip", szKey, m_aCreateEquips[i][NEWCHAR_WRIST]);
			sprintf(szKey, "Foot%d", i);
			theIni.WriteIntValue("NewCharEquip", szKey, m_aCreateEquips[i][NEWCHAR_FOOT]);
			sprintf(szKey, "Wing%d", i);
			theIni.WriteIntValue("NewCharEquip", szKey, m_aCreateEquips[i][NEWCHAR_WING]);
		}
		theIni.Save(szIniFile);
		theIni.Close();
	}

	return CECBaseUIMan::Release();
}

//	On command
bool CECLoginUIMan::OnCommand(const char* szCommand, AUIDialog* pDlg)
{
	// HACK for theme test
	if( 0 == stricmp(szCommand, "theme") )
	{
		EC_SYSTEM_SETTING ss = g_pGame->GetConfigs()->GetSystemSettings();
		
		if(ss.iTheme == 0) { ss.iTheme = 1;}
		else { ss.iTheme = 0;}

		g_pGame->GetConfigs()->SetSystemSettings(ss);
		return true;
	}

	if (CECBaseUIMan::OnCommand(szCommand, pDlg))
		return true;

	// NOTICE: message transfer (mainly for "IDCANCEL")
	if( 0 == stricmp(pDlg->GetName(), "Win_Instruction"))
	{
		return CECBaseUIMan::OnCommand(szCommand, GetDialog("Win_Agreement"));
	}

	if( 0 == stricmp(pDlg->GetName(), "Win_LoginWait") 
		|| 0 == stricmp(pDlg->GetName(), "Win_Softkb"))
	{
		return CECBaseUIMan::OnCommand(szCommand, m_pDlgLogin);
	}

	if( 0 == stricmp(pDlg->GetName(), "Win_Manage") ||
		0 == stricmp(pDlg->GetName(), "Win_Manage2") ||
		0 == stricmp(pDlg->GetName(), "Win_TradeInfo") ||
		0 == stricmp(pDlg->GetName(), "Win_LoginPage"))
	{
		return CECBaseUIMan::OnCommand(szCommand, m_pDlgSelect);
	}

	if (0 == stricmp(pDlg->GetName(), "Win_AddID"))
		return OnCommand_Referreral(szCommand, pDlg);

	if (!stricmp(pDlg->GetName(), "Win_CreateRace") ||
		!stricmp(pDlg->GetName(), "Win_CreateButton1") || 
		strstr(pDlg->GetName(), "Win_RaceInfo") == pDlg->GetName() ||
		strstr(pDlg->GetName(), "Win_ProfRadar") == pDlg->GetName()){
		return CECBaseUIMan::OnCommand(szCommand, GetDialog("Win_CreateProfession"));
	}
	if (!stricmp(pDlg->GetName(), "Win_CreateButton2") || 
		!stricmp(pDlg->GetName(), "Win_SwitchNewCharEquip") ||
		strstr(pDlg->GetName(), "Win_CreateInfo") == pDlg->GetName()){
		return CECBaseUIMan::OnCommand(szCommand, GetDialog("Win_CreateGenderName"));
	}

	return false;
}

bool CECLoginUIMan::OnCommand_Referreral(const char* szCommand, AUIDialog* pDlg)
{
	if (!stricmp(szCommand, "IDCANCEL"))
	{
		pDlg->Show(false);
	}
	else if (!stricmp(szCommand, "confirm"))
	{
		PAUIOBJECT pEdtID = pDlg->GetDlgItem("Edt_ID");
		ACString name = m_strReferrerID; // temp stored in this
		m_strReferrerID = pEdtID->GetText();
		pDlg->Show(false);
		NewCharacterImpl(name);
	}
	return true;
}

//	On message box
bool CECLoginUIMan::OnMessageBox(int iRetVal, AUIDialog* pDlg)
{
	if (0 == stricmp(pDlg->GetName(), "MsgBox_LoginFail") ||
		0 == stricmp(pDlg->GetName(), "MsgBox_LinkBroken")){
		if (CECUIConfig::Instance().GetLoginUI().bAvoidLoginUI){
			PostQuitMessage(0);
		}else{
			if (m_pDlgLogin->IsShow()){
				BringWindowToTop(m_pDlgLogin);	// To focus on username editbox.
			}
		}
	}
	else if (0 == stricmp(pDlg->GetName(), "MsgBox_RemindReconnect")){
		if (IDYES != iRetVal && IDOK != iRetVal ||
			!CECReconnect::Instance().CanRequestReconnect() ||
			!CECReconnect::Instance().RequestReconnect()){
			CECReconnect::Instance().ResetState();
			if (CECUIConfig::Instance().GetLoginUI().bAvoidLoginUI){
				PostQuitMessage(0);
			}else{
				SwitchToLogin();
				BringWindowToTop(m_pDlgLogin);	// To focus on username editbox.
			}
		}
	}
	else if( 0 == stricmp(pDlg->GetName(), "SelChar_DelChar") && IDYES == iRetVal )
	{
		g_pGame->GetGameSession()->DeleteRole(m_vecRoleInfo[m_idCurRole].roleid);
	}
	else if( 0 == stricmp(pDlg->GetName(), "SelChar_CancelPrepost") && IDYES == iRetVal )
	{
		// send cancel protocol
		g_pGame->GetGameSession()->webTrade_RolePreCancelPost(m_vecRoleInfo[m_idCurRole].roleid);
	}
	else if( 0 == stricmp(pDlg->GetName(), "PwdProtect_Cancel") && IDYES == iRetVal )
	{
		CDlgPwdProtect* pPwd = dynamic_cast<CDlgPwdProtect*>(GetDialog("Win_PwdProtect"));
		pPwd->HideMatrix();
		
		m_pDlgLogin->Show(true);
		g_pGame->GetGameSession()->Close();
	}
	else if( 0 == stricmp(pDlg->GetName(), "ServerList_Query"))
	{
		PAUIDIALOG pDlgFindServer = GetDialog("Win_FindServer");
		if(pDlgFindServer->IsShow()) pDlgFindServer->Show(true, true);
	}
	else if (strstr(pDlg->GetName(), "Game_WebTrade_") == pDlg->GetName())
	{
		m_pDlgTrade->OnMessageBox(pDlg, iRetVal);
	}
	else if (!stricmp(pDlg->GetName(), "Redirect_LoginPos"))
	{
		using namespace GNET;
		PlayerPositionResetRqst* p = (PlayerPositionResetRqst*)pDlg->GetDataPtr();
		if (p)
		{
			if (g_pGame->GetGameSession()->IsConnected())
			{
				PlayerPositionResetRqstArg* pArg = (PlayerPositionResetRqstArg*)p->GetArgument();
				CECGameRun *pGameRun = g_pGame->GetGameRun();
				RoleInfo role = pGameRun->GetSelectedRoleInfo();
				if (role.roleid != pArg->roleid)
				{
					ASSERT(false);
					a_LogOutput(1, "CECLoginUIMan::OnMessageBox, Invalid roleid(%d!=%d)", role.roleid, pArg->roleid);
				}
				else
				{
					PlayerPositionResetRqstRes* pResult = (PlayerPositionResetRqstRes*)p->GetResult();
					pResult->retcode = (IDOK == iRetVal) ? 1 : 0;
					g_pGame->GetGameSession()->SendNetData(*p);

					//	保留重定向的角色信息，登陆成功时使用
					role.worldtag = pArg->worldtag;
					role.posx = pArg->pos_x;
					role.posy = pArg->pos_y;
					role.posz = pArg->pos_z;
					pGameRun->SetRedirectLoginPosRoleInfo(role);
				}
			}
			p->Destroy();
			pDlg->SetDataPtr(NULL);
		}
	}
	else if( !stricmp(pDlg->GetName(), "Game_DownloadOK") && IDYES == iRetVal )
	{
		CECMCDownload::GetInstance().SendInstall();
	}
	else if( m_pDlgLogin->IsShow() )
	{
		BringWindowToTop(m_pDlgLogin);	// To focus on username editbox.
	}

	return CECBaseUIMan::OnMessageBox(iRetVal, pDlg);
}

//	Handle windows message
bool CECLoginUIMan::DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (CECContinueLoad::Instance().IsInMergeLoad() &&
		!CECLoginQueue::Instance().InQueue())
	{
		//	转服等连续加载过程中，屏蔽玩家交互，防止退出到账号输入界面
		return false;
	}
	
	if (DealMessageBoxQuickKey(uMsg, wParam, lParam))
		return true;
	
	if( uMsg == WM_KEYDOWN && wParam == VK_ESCAPE){
		if (m_pDlgActive == m_pDlgLogin &&
			m_pDlgLogin->IsShow() &&
			m_pDlgLogin->GetFocus() == m_pDlgLogin->GetDlgItem("Lst_Account")){
			//	此处不处理，会导致 Lst_Account 中输入 VK_ESCAPE 时调用 CDlgLogin::OnCommand_Cancel 而退出游戏客户端
			m_pDlgLogin->OnEventKeyDown_LstAccount(wParam, 0, NULL);
			return true;
		}
	}
	
	if( CECBaseUIMan::DealWindowsMessage(uMsg, wParam, lParam) )
		return true;
	
	else if( uMsg == WM_RBUTTONDOWN && AUI_PRESS('R') )
	{
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		AdjustRole(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else if( GetCurScene() == LOGIN_SCENE_SELCHAR 
		     && DealRotateSelCharMessage(uMsg, wParam, lParam) )
	{
		return true;
	}
	//	else if( uMsg == WM_KEYDOWN && wParam == VK_F1 && glb_IsConsoleEnable() )
	//	{
	//		GetDialog("Win_CWindowMain")->Show(true);
	//	}
	else
		return false;
	
	return true;
}

bool CECLoginUIMan::DealRotateSelCharMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg == WM_RBUTTONDOWN ){
		InitDragRole(lParam);
	} 
	else if( uMsg == WM_MOUSEMOVE && m_bSelectCharDragRole ){
		if( !(wParam & MK_RBUTTON) ){
			ResetDragRole(false); 
		}
		else{
			DragRole(lParam);
		}
	}
	else if( uMsg == WM_RBUTTONUP ){
		ResetDragRole(false);
	}
	else
		return false;

	return true;
}

//	Handle event
bool CECLoginUIMan::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, AUIDialog* pDlg, AUIObject* pObj)
{
	if (CECBaseUIMan::OnEvent(uMsg, wParam, lParam, pDlg, pObj))
		return true;

	// event transfer
	if( 0 == stricmp(pDlg->GetName(), "Win_Manage") ||
		0 == stricmp(pDlg->GetName(), "Win_Manage2") ||
		0 == stricmp(pDlg->GetName(), "Win_TradeInfo") ||		
		0 == stricmp(pDlg->GetName(), "Win_LoginPage"))
	{
		return CECBaseUIMan::OnEvent(uMsg, wParam, lParam, m_pDlgSelect, NULL);
	}
	return true;
}

bool CECLoginUIMan::LaunchCharacter()
{
	m_pDlgLogin->Show(false);
	GetDialog("Win_LoginWait")->Show(false);

	ClearRoleList();

	m_pDlgSelect->Switch(true);
	// lock create function before getting role list
	m_pDlgSelect->EnableCreate(false);

	return true;
}

bool CECLoginUIMan::RemoveLocalCharacter(int idRole)
{
	//	删除当前客户端本地角色列表中idRole（应该只有一个，但按多个处理）

	bool bFound(false);
	int newRoleIndex = m_idCurRole;				//	删除角色可能导致当前选中模型的下标修改
	RoleInfoList	vecRoleInfo;
	LoginPlayerList	vecRoleModel;
	for (size_t u(0); u < m_vecRoleInfo.size(); ++ u)
	{
		const GNET::RoleInfo &role = m_vecRoleInfo[u];
		if (idRole == role.roleid)
		{
			bFound = true;
			A3DRELEASE(m_vecRoleModel[u]);
		}
		else
		{
			vecRoleInfo.push_back(m_vecRoleInfo[u]);
			vecRoleModel.push_back(m_vecRoleModel[u]);
		}
		if (m_idCurRole == (int)u)
		{
			//	如果删除选中角色，将选中上一个（有可能为-1）
			//	删除其它角色将保持已选中角色
			newRoleIndex = (int)vecRoleInfo.size() - 1;
		}
	}

	if (bFound)
	{
		m_vecRoleInfo.swap(vecRoleInfo);
		m_vecRoleModel.swap(vecRoleModel);
		RebuildRoleList();
		if (newRoleIndex < 0)
			SelectLatestCharacter();
		else
			RefreshRole(newRoleIndex);
	}

	return bFound;
}

bool CECLoginUIMan::CreateCharacter(GNET::RoleInfo &ri)
{
	//	创建角色时，服务器有机会重用已删除角色id，
	//	由于本地角色列表与服务器可能不同步，即客户端获取时角色未删除，但创建新角色返回时用服务器已经删除，
	//	因此，创建角色时，需要检查并删除在服务器端已删除的角色
	RemoveLocalCharacter(ri.roleid);

	bool bval = AddCharacter(&ri);
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	CDlgCreateGenderName *pDlgGenderName = dynamic_cast<CDlgCreateGenderName *>(GetDialog("Win_CreateGenderName"));
	pDlgGenderName->OnCreateSucccess();

	RebuildRoleList();

	RefreshRole(m_vecRoleInfo.size() - 1);

	return true;
}

bool CECLoginUIMan::AddCharacter(GNET::RoleInfo* pri)
{
	if( pri )
	{
		m_vecRoleInfo.push_back(*pri);
		m_vecRoleModel.push_back(NULL);
	}

	return pri != NULL;
}

bool CECLoginUIMan::SelectLatestCharacter()
{
	// unlock create function after got role list
	m_pDlgSelect->EnableCreate(true);

	// try to select the trade role
	if(LaunchRoleSelling(g_pGame->GetGameRun()->GetSellingRoleID()))
	{
		return true;
	}

	RebuildRoleList();

	int idRole = 0, nLastLoginTime = 0;
	for(int i = 0; i < (int)m_vecRoleInfo.size(); i++ )
	{
		if( m_vecRoleInfo[i].lastlogin_time > nLastLoginTime )
		{
			nLastLoginTime = m_vecRoleInfo[i].lastlogin_time;
			idRole = i;
		}
	}

	RefreshRole(idRole, false);
	
	AutoLogin();

	return true;
}

void CECLoginUIMan::CancelAutoLogin()
{
	s_bNeedTestRoleAutoLogin = false;
}

void CECLoginUIMan::AutoLogin()
{
	//	检查是否有默认角色名，有则直接选取并登录
	while (s_bNeedTestRoleAutoLogin)
	{
		//	仅第一次看到非空玩家角色列表时可以默认登录，以处理从世界返回时又会选择默认角色登录的情况
		//
		s_bNeedTestRoleAutoLogin = false;
		ACString strRoleName = CECCommandLine::GetRole();
		if (strRoleName.IsEmpty()){
			//	默认登录角色为空
			break;
		}
		SelectLoginChar(FindCharByName(strRoleName));
		break;
	}
}

bool CECLoginUIMan::SelectLoginChar(int iChar){
	bool result(false);
	if (iChar >= 0){
		//	模拟选中角色
		m_pDlgSelect->SelectChar(iChar);
		//	角色选取成功时，模拟登录
		if (m_idCurRole == iChar){
			result = LoginSelectedChar();
		}
	}
	return result;
}

int CECLoginUIMan::FindCharByName(const ACString &strCharName)const{
	int result(-1);
	ACHAR szText[80];
	for(int i = 0; i < (int)m_vecRoleInfo.size(); i++ )
	{
		const GNET::RoleInfo &info = m_vecRoleInfo[i];
		ACString strName((const ACHAR *)info.name.begin(), info.name.size() / sizeof(ACHAR));
		AUI_ConvertChatString(strName, szText);
		if (!strCharName.CompareNoCase(szText)){
			result = i;
			break;
		}
	}
	return result;
}

int CECLoginUIMan::FindCharByRoleID(int roleID)const{
	int result(-1);
	for(int i = 0; i < (int)m_vecRoleInfo.size(); i++ )
	{
		const GNET::RoleInfo &info = m_vecRoleInfo[i];
		if (info.roleid == roleID){
			result = i;
			break;
		}
	}
	return result;
}

bool CECLoginUIMan::LoginSelectedChar(){
	if (GetIDCurRole() < 0){
		return false;
	}
	//	隐藏完美神盾提示界面
	PAUIDIALOG pDlgHint = GetDialog("Win_PwdHint");
	if (pDlgHint && pDlgHint->IsShow())
		pDlgHint->Show(false);
	
	//	模拟登录
	return m_pDlgSelect->LoginSelectedChar();
}

bool CECLoginUIMan::DelCharacter(int nResult, int idRole)
{
	if( nResult == ERR_SUCCESS )
	{
		for( int i = 0; i < (int)m_vecRoleInfo.size(); i++ )
		{
			if( idRole != m_vecRoleInfo[i].roleid ) continue;

			m_pDlgSelect->DeleteCharacter(i);

			m_vecRoleInfo[i].status = _ROLE_STATUS_READYDEL;
			m_vecRoleInfo[i].delete_time = time(NULL);
			m_vecRoleModel[i]->SitDown();
			m_roleDisplayActionTrigger->Reset();

			break;
		}
		ShowErrorMsg(209);
		RebuildRoleList();
		RefreshPlayerList();
	}
	else
	{
		ShowErrorMsg(208);
	}

	return true;
}

bool CECLoginUIMan::RestoreCharacter(int nResult, int idRole)
{
	if( nResult == ERR_SUCCESS )
	{
		for( int i = 0; i < (int)m_vecRoleInfo.size(); i++ )
		{
			if( idRole != m_vecRoleInfo[i].roleid ) continue;

			m_pDlgSelect->RestoreCharacter(i);

			m_vecRoleInfo[i].status = _ROLE_STATUS_NORMAL;
			m_vecRoleModel[i]->StandUp();
			m_roleDisplayActionTrigger->Reset();
			
			break;
		}
		ShowErrorMsg(212);
		RebuildRoleList();
		RefreshPlayerList();
	}
	else
		ShowErrorMsg(211);

	return true;
}

bool CECLoginUIMan::ChangeCustomizeData(int idRole, int result)
{
	int i;

	for( i = 0; i < (int)m_vecRoleInfo.size(); i++ )
	{
		if( idRole == m_vecRoleInfo[i].roleid )
		{
			if(result != ERROR_SUCCESS)
			{
				// restore custom data if failed
				m_pCurPlayer->RestoreCustomizeData();
			}
			else
			{
				CECPlayer::PLAYER_CUSTOMIZEDATA data = m_pCurPlayer->GetCustomizeData();
				m_vecRoleInfo[i].custom_data.replace(&data, sizeof(CECPlayer::PLAYER_CUSTOMIZEDATA));
			}
			break;
		}
	}

	//--------------------new-------begin---------------------------------
	//解决服务器延迟导致个性化数据尚未更新便进入游戏的bug
	m_pDlgSelect->Switch(true);
	RefreshRole(GetIDCurRole());
	//--------------------new-------end-----------------------------------
	
	return true;
}

void CECLoginUIMan::ChangeSceneByRole()
{
	//	根据当前选中角色、显示相应场景，可减少寄售、跨服转服中背景切换
	ChangeScene(LOGIN_SCENE_SELCHAR);
	const GNET::RoleInfo& Info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
	ChangeCameraByScene((LOGIN_SCENE)(LOGIN_SCENE_CREATE_BEGIN + Info.occupation));
}

bool CECLoginUIMan::LaunchPreface()
{
	if(g_pGame->GetGameRun()->IsFirstShow())
	{
		ChangeScene(LOGIN_SCENE_LOGIN);
		GetDialog("Win_Agreement")->Show(true);		
		if (CECCommandLine::GetExportServerListZoneIDName())
			CECServerList::Instance().Export();
	}
	else
	{
		if (g_pGame->GetGameRun()->GetLogoutFlag() == 2 &&
			!CECCrossServer::Instance().IsWaitLogin() &&
			!CECReconnect::Instance().IsReconnecting())
		{
			//	退出到登录界面
			ChangeScene(LOGIN_SCENE_LOGIN);
			m_pDlgLogin->Show(true);
		}

		if(!m_pDlgLogin->IsShow())
		{
			if (CECCrossServer::Instance().IsWaitLogin() ||
				CECReconnect::Instance().IsReconnecting())
				return true;

			ChangeSceneByRole();

			// show trade dialog instead of wait dialog
			if(g_pGame->GetGameRun()->GetSellingRoleID() > 0)
			{
				ASSERT(g_pGame->GetGameRun()->GetSelectedRoleInfo().roleid == g_pGame->GetGameRun()->GetSellingRoleID());
				m_pDlgTrade->Show(true);
				return true;
			}
			
			GetDialog("Win_LoginWait")->Show(true);
		}
	}
	
	return true;
}

bool CECLoginUIMan::LaunchLoading()
{
	OutputDebugStringA("Start game 1...\n");

	const GNET::RoleInfo& ri = g_pGame->GetGameRun()->GetSelectedRoleInfo();
	A3DVECTOR3 vecPos(ri.posx, ri.posy, ri.posz);

	if (!g_pGame->GetGameRun()->StartGame(ri.worldtag, vecPos))
	{
		//	Fatal error. Exit game directly
		a_LogOutput(1, "CECLoginUIMan::LaunchLoading, Failed to start game !");
		::PostMessage(g_pGame->GetGameInit().hWnd, WM_QUIT, 0, 0);
		return false;
	}

	g_pGame->GetGameSession()->StartGame();

	OutputDebugStringA("Start game 2...\n");
	return true;
}

bool CECLoginUIMan::LaunchMatrixChallenge(void* pData)
{
	MatrixChallenge* p = (MatrixChallenge*)pData;
	CDlgPwdProtect* pPwd = dynamic_cast<CDlgPwdProtect*>(GetDialog("Win_PwdProtect"));
	pPwd->ShowMatrix(p);

	return true;
}

bool CECLoginUIMan::Tick(DWORD dwDeltaTime)
{
	if( m_pObjMouseOn && 
		( strstr(m_pObjMouseOn->GetName(), "Img_ZoomIn") || 
		  strstr(m_pObjMouseOn->GetName(), "Btn_Link") ) )
	{
		g_pGame->ChangeCursor(RES_CUR_HAND);
	}
	else
	{
		g_pGame->ChangeCursor(RES_CUR_NORMAL);
	}
	
	
	if( GetCurScene() == LOGIN_SCENE_SELCHAR )
	{
		ResetCursor();
	}
	else if( GetDialog("Win_LoginWait")->IsShow() )
	{
		ACString strText;
		DWORD dwTotalTime, dwCnt;
		PAUIDIALOG pDlg = GetDialog("Win_LoginWait");
		PAUILABEL pLabel = (PAUILABEL)pDlg->GetDlgItem("Txt_Message");

		if( g_pGame->GetGameSession()->GetOvertimeCnt(dwTotalTime, dwCnt) )
		{
			dwTotalTime /= 1000;
			dwCnt /= 1000;
			strText.Format(GetStringFromTable(217), dwTotalTime, dwCnt);
		}
		else
			strText.Format(GetStringFromTable(216));

		pLabel->SetText(strText);
	}
	else if( GetDialog("Win_Manage")->IsShow() )
	{
		// keep this focus
		if (!m_pDlgActive || !m_pDlgActive->IsShow())
			BringWindowToTop(GetDialog("Win_Manage"));
	}

	AdjustCamera();

	UpdateRedirectLoginPos();

	if (GetCurScene() == LOGIN_SCENE_CREATE_CHOOSE_PROFESSION){
		if (AUI_PRESS('R')){
			if (AUI_PRESS(VK_UP)){
				m_aCreateDisplayRadius += 0.1f;
				a_ClampRoof(m_aCreateDisplayRadius, 50.0f);
				RotateAnimator::Instance().SetRadius(m_aCreateDisplayRadius);
			}else if (AUI_PRESS(VK_DOWN)){
				m_aCreateDisplayRadius -= 0.1f;
				a_ClampFloor(m_aCreateDisplayRadius, 1.0f);
				RotateAnimator::Instance().SetRadius(m_aCreateDisplayRadius);
			}
		}else if (AUI_PRESS('D')){
			if (AUI_PRESS(VK_UP)){
				m_aCreateDeltaDist += 0.02f;
				a_ClampRoof(m_aCreateDeltaDist, 10.0f);
				RotateAnimator::Instance().SetDeltaDist(m_aCreateDeltaDist);
			}else if (AUI_PRESS(VK_DOWN)){
				m_aCreateDeltaDist -= 0.02f;
				a_ClampFloor(m_aCreateDeltaDist, 0.0f);
				RotateAnimator::Instance().SetDeltaDist(m_aCreateDeltaDist);
			}
		}else if (AUI_PRESS('S')){
			if (AUI_PRESS(VK_UP)){
				m_aCreateSelectMove += 0.05f;
				a_ClampRoof(m_aCreateSelectMove, 10.0f);
				RotateAnimator::Instance().SetSelectMove(m_aCreateSelectMove);
			}else if (AUI_PRESS(VK_DOWN)){
				m_aCreateSelectMove -= 0.05f;
				a_ClampFloor(m_aCreateSelectMove, 0.0f);
				RotateAnimator::Instance().SetSelectMove(m_aCreateSelectMove);
			}
		}
	}	
	TickRaceModels(dwDeltaTime);
	if (GetCurScene() == LOGIN_SCENE_SELCHAR){
		m_roleDisplayActionTrigger->Tick(dwDeltaTime);
		if (m_roleDisplayActionTrigger->GetTriggered()){
			CECLoginPlayer *pPlayer = GetCurRoleModel();
			if (pPlayer){
				pPlayer->ShowDisplayAction();
			}
		}
	}

	return CECBaseUIMan::Tick(dwDeltaTime);
}

void CECLoginUIMan::UpdateRedirectLoginPos(){
	PAUIDIALOG pDlgResetPosition = GetDialog("Redirect_LoginPos");
	if (!pDlgResetPosition || pDlgResetPosition->GetDestroyFlag()){
		return;
	}
	//	超时未做选择、或断开链接时，对话框的自我清除
	bool bCancel(false);
	if (!g_pGame->GetGameSession()->IsConnected())
		bCancel = true;
	else{
		DWORD dwTickStart = pDlgResetPosition->GetData();
		DWORD elapsed = CECTimeSafeChecker::ElapsedTimeFor(dwTickStart);			
		PlayerPositionResetRqst* p = (PlayerPositionResetRqst*)pDlgResetPosition->GetDataPtr();
		if (p && !p->TimePolicy(elapsed/1000)){
			bCancel = true;
		}
	}
	if (bCancel){
		pDlgResetPosition->OnCommand("IDCANCEL");
		return;
	}
}

int CECLoginUIMan::PickRole(int x, int y)
{
	float fFraction;
	A3DVECTOR3 vecHit, vecNormal;
	A3DVECTOR3 vecPos(float(x), float(y), 1.0f);
	A3DVECTOR3 vecCam = g_pGame->GetViewport()->GetA3DCamera()->GetPos();
	g_pGame->GetViewport()->GetA3DViewport()->InvTransform(vecPos, vecPos);

	for( int i = 0; i < int(m_vecRoleModel.size()); i++ )
	{
		const A3DAABB& aabb = m_vecRoleModel[i]->GetPlayerAABB();
		if( CLS_RayToAABB3(vecCam, vecPos - vecCam, aabb.Mins, aabb.Maxs, vecHit, &fFraction, vecNormal) )
			return i;
	}

	return -1;
}

A3DCOLOR CECLoginUIMan::GetMsgBoxColor()
{
	return A3DCOLORRGBA(188, 255, 255, 160);
}

void CECLoginUIMan::ShowErrorMsg2(const ACHAR *pszMsg, const char *pszName)
{
	if( GetDialog("Win_LoginWait")->IsShow() ||
		m_curScene == LOGIN_SCENE_SELCHAR	||
		m_pDlgTrade->IsShow())
	{
		// clear the selling id
		m_pDlgTrade->Show(false);
		g_pGame->GetGameRun()->SetSellingRoleID(0);

		// switch back to login ui
		GetDialog("Win_LoginWait")->Show(false);
		SwitchToLogin();
		pszName = "MsgBox_LoginFail";
	}
	
	if (CECCrossServer::Instance().IsWaitLogin())
		CECCrossServer::Instance().OnLoginFail();

	MessageBox(pszName, pszMsg, MB_OK, GetMsgBoxColor());
}

void CECLoginUIMan::ShowReconnectMsg(const ACHAR *pszMsg, const char *pszName){
	if (m_pDlgTrade->IsShow()){
		m_pDlgTrade->Show(false);
		g_pGame->GetGameRun()->SetSellingRoleID(0);
	}
	if (GetDialog("Win_LoginWait")->IsShow()){
		GetDialog("Win_LoginWait")->Show(false);
	}
	if (CECCrossServer::Instance().IsWaitLogin()){
		CECCrossServer::Instance().OnLoginFail();
	}
	CECBaseUIMan::ShowReconnectMsg(pszMsg, pszName);
}

bool CECLoginUIMan::UIControlCursor()
{
	return false;
}

void CECLoginUIMan::ChangeCameraByScene(LOGIN_SCENE scene)
{
	if (!IsSceneValid(scene)){
		ASSERT(false);
		return;
	}
	A3DCamera * pCamera = g_pGame->GetViewport()->GetA3DCamera();
	
	if( scene >= LOGIN_SCENE_BODYCUSTOMIZE_BEGIN &&
		scene <= LOGIN_SCENE_BODYCUSTOMIZE_END )
	{
		int nIndex = LOGIN_SCENE_CREATE_BEGIN + scene - LOGIN_SCENE_BODYCUSTOMIZE_BEGIN;
		pCamera->SetPos(m_aCamPos[nIndex]);
		pCamera->SetDirAndUp(m_aCamDir[nIndex], m_aCamUp[nIndex]);
	}else{
		pCamera->SetPos(m_aCamPos[scene]);
		pCamera->SetDirAndUp(m_aCamDir[scene], m_aCamUp[scene]);
	}
	
	if( scene >= LOGIN_SCENE_FACECUSTOMIZE_BEGIN && scene <= LOGIN_SCENE_FACECUSTOMIZE_END )
	{
		pCamera->SetPos(s_camPosDelta[m_pCurPlayer->GetProfession()][m_pCurPlayer->GetGender()] + pCamera->GetPos());
		
		pCamera->SetFOVSet(DEG2RAD(10.0f));
		pCamera->SetFOV(DEG2RAD(10.0f));
	}
	else
	{
		pCamera->SetFOVSet(DEG2RAD(DEFCAMERA_FOV));
		pCamera->SetFOV(DEG2RAD(DEFCAMERA_FOV));
	}
}

bool CECLoginUIMan::ChangeScene(LOGIN_SCENE scene)
{
	ChangeCameraByScene(scene);
	m_curScene = scene;
	if (scene == LOGIN_SCENE_CREATE_CHOOSE_PROFESSION){
		SwitchShowModelEquip(true);
		DisplayActionTrigger::Instance().Rest();
		if (!CECProfConfig::Instance().IsProfession(m_nCurProfession)){
			//	第一次使用创建
			A3DVECTOR3 dir = m_aCamPos[LOGIN_SCENE_CREATE_CHOOSE_PROFESSION] - m_aCreateDisplayCenter;
			RotateAnimator::Instance().SetDir(dir);
			//	默认职业将由界面计算、并调用 ChangeShowModel
		}else{
			//	直接指到最近一次选中的职业
			int race = CECProfConfig::Instance().GetRaceByProfession(m_nCurProfession);
			int order = CECProfConfig::Instance().GetRaceShowOrder(race);
			RotateAnimator::Instance().SetFirstIndicator(order);
			RefreshPlayerList();
			TickRaceModels(0);
		}
	}
	if (scene != LOGIN_SCENE_SELCHAR){
		if( m_bSelectCharDragRole )
		{
			m_bSelectCharDragRole = false;
			g_pGame->ShowCursor(true);
		}
	}
	if (scene == LOGIN_SCENE_LOGIN){
		CECReconnect::Instance().ResetState();
	}

	// for update terrain water's current water height data
	g_pGame->GetGameRun()->GetWorld()->GetTerrainWater()->UpdateWaterHeight();

	return RefreshPlayerList();
}

bool CECLoginUIMan::RefreshPlayerList()
{
	CECPlayerMan::LoginPlayerTable& players = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetLoginPlayerTable();

	players.clear();

	while (true)
	{
		if (m_curScene == LOGIN_SCENE_SELCHAR)
		{
			if( m_idCurRole >= 0 )
				players.push_back(m_vecRoleModel[m_idCurRole]);
			break;
		}

		if (m_curScene >= LOGIN_SCENE_CREATE_BEGIN &&
			m_curScene <= LOGIN_SCENE_CREATE_END)
		{
			if( m_aModel[m_nCurProfession][m_nCurGender] )
				players.push_back(m_aModel[m_nCurProfession][m_nCurGender]);
			break;
		}
		
		if (m_curScene == LOGIN_SCENE_CREATE_CHOOSE_PROFESSION)
		{
			for (int i = 0; i < NUM_PROFESSION; ++ i)
			{
				for (int j = 0; j < NUM_GENDER; ++ j)
				{
					if (!CECProfConfig::Instance().CanShowOnCreate(i, j) ||
						m_aModel[i][j] == NULL){
						continue;
					}
					players.push_back(m_aModel[i][j]);
				}
			}
			break;
		}

		if (m_curScene >= LOGIN_SCENE_FACECUSTOMIZE_BEGIN &&
			m_curScene <= LOGIN_SCENE_FACECUSTOMIZE_END	||
			m_curScene >= LOGIN_SCENE_BODYCUSTOMIZE_BEGIN &&
			m_curScene <= LOGIN_SCENE_BODYCUSTOMIZE_END)
		{
			if( m_pCurPlayer )
				players.push_back((CECLoginPlayer *)m_pCurPlayer);
			break;
		}

		break;
	}

	return true;
}

bool CECLoginUIMan::IsSceneValid(LOGIN_SCENE scene)const{
	return scene >= 0 && scene < LOGIN_SCENE_MAX;
}

void CECLoginUIMan::AdjustCamera()
{
	if (!AUI_PRESS('C') || !IsSceneValid(GetCurScene())){
		return;
	}

	static float fDeltaDeg = 0.5f;
	static float fDeltaDist = 0.2f;
	A3DCamera * pCamera = g_pGame->GetViewport()->GetA3DCamera();

	if( AUI_PRESS(VK_MENU) )
		fDeltaDist = 0.02f;
	else
		fDeltaDist = 0.2f;

	if( AUI_PRESS(VK_UP) )
	{
		if( AUI_PRESS(VK_CONTROL) )
			pCamera->PitchDelta(fDeltaDeg);
		else
			pCamera->MoveFront(fDeltaDist);
	}
	else if( AUI_PRESS(VK_DOWN) )
	{
		if( AUI_PRESS(VK_CONTROL) )
			pCamera->PitchDelta(-fDeltaDeg);
		else
			pCamera->MoveBack(fDeltaDist);
	}
	else if( AUI_PRESS(VK_LEFT) )
	{
		if( AUI_PRESS(VK_CONTROL) )
			pCamera->DegDelta(-fDeltaDeg);
		else
			pCamera->MoveLeft(fDeltaDist);
	}
	else if( AUI_PRESS(VK_RIGHT) )
	{
		if( AUI_PRESS(VK_CONTROL) )
			pCamera->DegDelta(fDeltaDeg);
		else
			pCamera->MoveRight(fDeltaDist);
	}
	else if( AUI_PRESS(VK_PRIOR) )
		pCamera->Move(g_vAxisY * fDeltaDist);
	else if( AUI_PRESS(VK_NEXT) )
		pCamera->Move(-g_vAxisY * fDeltaDist);

	A3DVECTOR3 vecOffset(0);
	if (m_curScene >= LOGIN_SCENE_FACECUSTOMIZE_BEGIN && m_curScene <= LOGIN_SCENE_FACECUSTOMIZE_END)
		vecOffset = s_camPosDelta[m_pCurPlayer->GetProfession()][m_pCurPlayer->GetGender()];
	int scene = m_curScene;
	if (scene >= LOGIN_SCENE_BODYCUSTOMIZE_BEGIN &&	scene <= LOGIN_SCENE_BODYCUSTOMIZE_END ){
		//	参照 ChangeCameraByScene , 与 LOGIN_SCENE_CREATE_BEGIN 共用
		scene = LOGIN_SCENE_CREATE_BEGIN + scene - LOGIN_SCENE_BODYCUSTOMIZE_BEGIN;
	}
	// 保存相机位置的时候要减掉ChangeScene里加上的偏移值
	m_aCamPos[scene] = pCamera->GetPos() - vecOffset;
	m_aCamDir[scene] = pCamera->GetDir();
	m_aCamUp[scene] = pCamera->GetUp();

	if (m_curScene == LOGIN_SCENE_CREATE_CHOOSE_PROFESSION){		
		A3DVECTOR3 dir = m_aCamPos[LOGIN_SCENE_CREATE_CHOOSE_PROFESSION] - m_aCreateDisplayCenter;
		RotateAnimator::Instance().SetDir(dir);
	}
}

void CECLoginUIMan::AdjustRole(int x, int y)
{
	RAYTRACERT TraceRt;
	CECCDS *pCDS = g_pGame->GetGameRun()->GetWorld()->GetCDS();
	A3DCamera *pCamera = g_pGame->GetViewport()->GetA3DCamera();
	A3DViewport *pView = g_pGame->GetViewport()->GetA3DViewport();
	A3DVECTOR3 vecStart, vecDest(float(x), float(y), 1.0f), vecDelta, vecDir;

	pView->InvTransform(vecDest, vecDest);
	vecStart = pCamera->GetPos();
	vecDelta = vecDest - vecStart;

	if( !pCDS->RayTrace(vecStart, vecDelta, 1.0f, &TraceRt, TRACEOBJ_SCENE, 0) )
		return;

	vecDir = pCamera->GetPos() - TraceRt.vPoint;
	vecDir.y = 0.0f;

	if(m_curScene >= LOGIN_SCENE_CREATE_BEGIN && m_curScene <= LOGIN_SCENE_CREATE_END )
	{
		if (CECProfConfig::Instance().IsProfession(m_nCurProfession) &&
			CECProfConfig::Instance().IsGender(m_nCurGender) &&
			m_aModel[m_nCurProfession][m_nCurGender]){
			m_aModel[m_nCurProfession][m_nCurGender]->SetPos(TraceRt.vPoint);
			m_aModel[m_nCurProfession][m_nCurGender]->ChangeModelMoveDirAndUp(vecDir, g_vAxisY);
			m_aCreatePos[m_nCurProfession] = TraceRt.vPoint;
		}
	}else if (m_curScene == LOGIN_SCENE_CREATE_CHOOSE_PROFESSION){
		m_aCreateDisplayCenter = TraceRt.vPoint;		
		RotateAnimator::Instance().SetCenter(m_aCreateDisplayCenter);
		A3DVECTOR3 dir = m_aCamPos[LOGIN_SCENE_CREATE_CHOOSE_PROFESSION] - m_aCreateDisplayCenter;
		RotateAnimator::Instance().SetDir(dir);
	}
}

void CECLoginUIMan::RebuildRoleList()
{
	int i;
	bool bSwapped;
	GNET::RoleInfo ri;
	CECLoginPlayer *pModel;

	do
	{
		bSwapped = false;
		for( i = 0; i < (int)m_vecRoleInfo.size() - 1; i++ )
		{
			if( m_vecRoleInfo[i].create_time > m_vecRoleInfo[i + 1].create_time )
			{
				ri = m_vecRoleInfo[i];
				m_vecRoleInfo[i] = m_vecRoleInfo[i + 1];
				m_vecRoleInfo[i + 1] = ri;

				pModel = m_vecRoleModel[i];
				m_vecRoleModel[i] = m_vecRoleModel[i + 1];
				m_vecRoleModel[i + 1] = pModel;
				
				bSwapped = true;
			}
		}
	} while( bSwapped );

	m_pDlgSelect->RefreshCharacterList();
}

//初始化自定义
bool CECLoginUIMan::Init_Customize(bool bCallForCreate)
{	
	CECCustomizeMgr::CustomizeInitParam_t cusInitParam;
	cusInitParam.pAUImanager = this;
	cusInitParam.nFaceTicketID = FACE_TICKET_DEFAULT;
	cusInitParam.vPos.Set(0.0f, 0.0f, 0.0f);
	cusInitParam.vDir.Set(0.0f, 0.0f, 0.0f);
	cusInitParam.pCurPlayer = NULL;
	if( bCallForCreate )//创建模型
	{
		// now get the face model for customizing from the player model
		cusInitParam.pCurPlayer = m_aModel[m_nCurProfession][m_nCurGender];
	}
	else//修改模型
	{
		// move this player to the position for customize
		cusInitParam.pCurPlayer = m_vecRoleModel[m_idCurRole];
	}

	m_pCurPlayer = cusInitParam.pCurPlayer;

	CECCustomizePolicy *pPolicy = NULL;
	if (bCallForCreate){
		pPolicy = &CECCustomizeCreateCharacterPolicy::Instance();
	}else{
		pPolicy = &CECCustomizeLoginCharacterPolicy::Instance();
	}
	if (IsCustomizeCharacter()){
		//	当前正在个性化
		if (!m_CustomizeMgr.Test(cusInitParam)){
			//	个性化参数不同，先取消当前个性化，再初始化新个性化
			CancelCustomize();
			m_CustomizeMgr.Init(cusInitParam, pPolicy);
		}//else 是同一个模型个性化，忽略处理
	}else{
		m_CustomizeMgr.Init(cusInitParam, pPolicy);
	}
	if (!bCallForCreate){
		// move this player to its create position
		m_pCurPlayer->SetPos(m_aCreatePos[m_pCurPlayer->GetProfession()]);
		((CECLoginPlayer *)m_pCurPlayer)->Stand();
	}

	RefreshPlayerList();
	return true;
}

namespace
{
#define DEFINE_DIALOG_CREATOR(dialogclass) static PAUIDIALOG wmgj_new_##dialogclass(void){ return new dialogclass; }
#define MAP_FILE_TO_CREATOR(file, dialogclass) s_mDialogCreator[file] = &wmgj_new_##dialogclass; s_mDialogCreator["version01\\"##file] = &wmgj_new_##dialogclass;

	//	定义各对话框创建函数
	DEFINE_DIALOG_CREATOR(CDlgAgreement)
	DEFINE_DIALOG_CREATOR(CDlgCWindow)
	DEFINE_DIALOG_CREATOR(CDlgCustomize)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeEye)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeEye2)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeFace)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeFace2)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeHair)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeNoseMouth)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeNoseMouth2)
	DEFINE_DIALOG_CREATOR(CDlgCustomizePaint)
	DEFINE_DIALOG_CREATOR(CDlgCustomizePre)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeStature)
	DEFINE_DIALOG_CREATOR(CDlgExplorer)
	DEFINE_DIALOG_CREATOR(CDlgFaceName)
	DEFINE_DIALOG_CREATOR(CDlgFindServer)
	DEFINE_DIALOG_CREATOR(CDlgLogin)
	DEFINE_DIALOG_CREATOR(CDlgLoginButton)
	DEFINE_DIALOG_CREATOR(CDlgLoginOther)
	DEFINE_DIALOG_CREATOR(CDlgLoginPage)
	DEFINE_DIALOG_CREATOR(CDlgLoginServerList)
	DEFINE_DIALOG_CREATOR(CDlgLoginServerListButton)
	DEFINE_DIALOG_CREATOR(CDlgLoginVersion)
	DEFINE_DIALOG_CREATOR(CDlgPwdHint)
	DEFINE_DIALOG_CREATOR(CDlgPwdProtect)
	DEFINE_DIALOG_CREATOR(CDlgSelect)
	DEFINE_DIALOG_CREATOR(CDlgWebRoleTrade)
	DEFINE_DIALOG_CREATOR(CDlgWebTradeInfo)
	DEFINE_DIALOG_CREATOR(CDlgCreateProfession)
	DEFINE_DIALOG_CREATOR(CDlgCreateGenderName)
	DEFINE_DIALOG_CREATOR(CDlgLoginQueue)

	//	声明从界面文件名创建对话框的函数类型
	typedef	PAUIDIALOG	(*DialogCreatorFunc)(void);
	
	//	声明从界面文件名到对话框创建函数的映射
	typedef abase::hash_map<ConstChar, DialogCreatorFunc, ConstCharHashFunc>	DialogCreatorMap;
	
	//	定义映射函数
	static DialogCreatorFunc FindDialogCreator(const AString &strFileName)
	{
		static DialogCreatorMap	s_mDialogCreator;
		
		static bool	s_init(false);
		if (!s_init)
		{
			s_init = true;
			
			//	初始化映射函数
			MAP_FILE_TO_CREATOR("agreement.xml", CDlgAgreement)
			MAP_FILE_TO_CREATOR("cwindowmain.xml", CDlgCWindow)
			MAP_FILE_TO_CREATOR("customize.xml", CDlgCustomize)
			MAP_FILE_TO_CREATOR("faces\\chooseeye.xml", CDlgCustomizeEye)
			MAP_FILE_TO_CREATOR("faces\\chooseeye2.xml", CDlgCustomizeEye2)
			MAP_FILE_TO_CREATOR("faces\\chooseface.xml", CDlgCustomizeFace)
			MAP_FILE_TO_CREATOR("faces\\chooseface2.xml", CDlgCustomizeFace2)
			MAP_FILE_TO_CREATOR("faces\\choosehair.xml", CDlgCustomizeHair)
			MAP_FILE_TO_CREATOR("faces\\choosenosemouth.xml", CDlgCustomizeNoseMouth)
			MAP_FILE_TO_CREATOR("faces\\choosenosemouth2.xml", CDlgCustomizeNoseMouth2)
			MAP_FILE_TO_CREATOR("faces\\choosefacepainting.xml", CDlgCustomizePaint)
			MAP_FILE_TO_CREATOR("faces\\chooseprecustomize.xml", CDlgCustomizePre)
			MAP_FILE_TO_CREATOR("faces\\choosestature.xml", CDlgCustomizeStature)
			MAP_FILE_TO_CREATOR("explorer.xml", CDlgExplorer)
			MAP_FILE_TO_CREATOR("faces\\facename.xml", CDlgFaceName)
			MAP_FILE_TO_CREATOR("findserver.xml", CDlgFindServer)
			MAP_FILE_TO_CREATOR("login.xml", CDlgLogin)
			MAP_FILE_TO_CREATOR("loginbutton.xml", CDlgLoginButton)
			MAP_FILE_TO_CREATOR("loginother.xml", CDlgLoginOther)
			MAP_FILE_TO_CREATOR("loginpage.xml", CDlgLoginPage)
			MAP_FILE_TO_CREATOR("loginserverlist.xml", CDlgLoginServerList)
			MAP_FILE_TO_CREATOR("loginserverlistbutton.xml", CDlgLoginServerListButton)
			MAP_FILE_TO_CREATOR("loginversion.xml", CDlgLoginVersion)
			MAP_FILE_TO_CREATOR("pwdhint.xml", CDlgPwdHint)
			MAP_FILE_TO_CREATOR("pwdprotect.xml", CDlgPwdProtect)
			MAP_FILE_TO_CREATOR("pwdprotect1.xml", CDlgPwdProtect)
			MAP_FILE_TO_CREATOR("pwdprotect2.xml", CDlgPwdProtect)
			MAP_FILE_TO_CREATOR("select.xml", CDlgSelect)
			MAP_FILE_TO_CREATOR("web_roletrade.xml", CDlgWebRoleTrade)
			MAP_FILE_TO_CREATOR("web_tradeinfo.xml", CDlgWebTradeInfo)
			MAP_FILE_TO_CREATOR("createprofession.xml", CDlgCreateProfession)
			MAP_FILE_TO_CREATOR("creategendername.xml", CDlgCreateGenderName)
			MAP_FILE_TO_CREATOR("loginqueuevip.xml", CDlgLoginQueue)
			MAP_FILE_TO_CREATOR("loginqueuenormal.xml", CDlgLoginQueue)
		}
		
		AString strTemp = strFileName;
		strTemp.MakeLower();
		DialogCreatorMap::iterator it = s_mDialogCreator.find((const char *)strTemp);
		return (it != s_mDialogCreator.end()) ? it->second : NULL;
	}
}

PAUIDIALOG CECLoginUIMan::CreateDlgInstance(const AString strTemplName)
{
	DialogCreatorFunc f = FindDialogCreator(strTemplName);
	return (f != NULL) ? f() : new CDlgTheme;
}

void CECLoginUIMan::NewCharacter(const ACString& name)
{
	CECLoginPlayer* pPlayer = m_aModel[m_nCurProfession][m_nCurGender];
	if (!pPlayer)
	{
		ShowErrorMsg(210);
		return;
	}
	
	if (name.GetLength() <= 0)
	{
		ShowErrorMsg(207);
		return;
	}

	m_strReferrerID.Empty();
	
	//		if (!m_bReferrerBound)
	//		{
	//			PAUIDIALOG pDlgAddID = GetDialog("Win_AddID");
	//			pDlgAddID->GetDlgItem("Edt_ID")->SetText(_AL(""));
	//			pDlgAddID->Show(true, true);
	//			m_strReferrerID = name; // temp stored in this
	//			return true;
	// 		}

	NewCharacterImpl(name);
}

void CECLoginUIMan::NewCharacterImpl(const ACString& name)
{
	if(GetVecRoleInfo().size() >= CECLOGINUIMAN_MAX_ROLES_CREATE)
		return;

	CECLoginPlayer* pPlayer = m_aModel[m_nCurProfession][m_nCurGender];
	ASSERT(pPlayer != NULL);
	if (!pPlayer)
		return;

	ACString strName = name;
	ASSERT(strName.GetLength() > 0);
	if (strName.GetLength() <= 0)
		return;
	
	int i;
	GNET::RoleInfo Info;
	
	Info.gender = m_nCurGender;
	Info.occupation = m_nCurProfession;
	Info.name.replace((const ACHAR *)strName, strName.GetLength() * sizeof(ACHAR));
	for( i = 0; i < IVTRSIZE_EQUIPPACK; i++ )
		Info.equipment.add(0);
	
	//	Fill custom data
	Info.custom_data.replace(&pPlayer->GetCustomizeData(), sizeof (CECPlayer::PLAYER_CUSTOMIZEDATA));
	
	GNET::Octets refID;
	refID.replace(NULL, 0);
	if (!m_strReferrerID.IsEmpty())
		refID.replace((const ACHAR *)m_strReferrerID, m_strReferrerID.GetLength() * sizeof(ACHAR));
	
	CECGameSession *pSession = g_pGame->GetGameSession();
	pSession->CreateRole(Info, refID);
}

bool CECLoginUIMan::DealMessageBoxQuickKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 处理消息框的 Y/N 快捷键
	//
	bool bRet(false);

	if (m_pDlgActive && m_pDlgActive->GetType() == AUIDIALOG_TYPE_MESSAGEBOX && uMsg == WM_CHAR)
	{
		// 是消息框的按键消息
		PAUIDIALOG pMessageBox = m_pDlgActive;
		if (wParam == 'Y' || wParam == 'y')
		{
			// 查找是否有 IDOK/IDYES
			PAUIOBJECTLISTELEMENT pElement = pMessageBox->GetFirstControl();
			while( pElement )
			{
				const char *szCommand = pElement->pThis->GetCommand();
				if (szCommand)
				{
					if (!stricmp(szCommand, "IDOK"))
					{
						if (pMessageBox->OnCommand("IDOK"))
							bRet = true;
						break;
					}
					if (!stricmp(szCommand, "IDYES"))
					{
						if (pMessageBox->OnCommand("IDYES"))
							bRet = true;
						break;
					}
				}
				pElement = pMessageBox->GetNextControl(pElement);
			}
		}
		else if (wParam == 'N' || wParam == 'n')
		{
			// 查找是否有 IDCANCEL/IDNO
			PAUIOBJECTLISTELEMENT pElement = pMessageBox->GetFirstControl();
			while( pElement )
			{
				const char *szCommand = pElement->pThis->GetCommand();
				if (szCommand)
				{
					if (!stricmp(szCommand, "IDNO"))
					{
						if (pMessageBox->OnCommand("IDNO"))
							bRet = true;
						break;
					}
					if (!stricmp(szCommand, "IDCANCEL"))
					{
						if (pMessageBox->OnCommand("IDCANCEL"))							
							bRet = true;
						break;
					}
				}
				pElement = pMessageBox->GetNextControl(pElement);
			}
		}
	}
	return bRet;
}

void CECLoginUIMan::CalcWindowScale()
{
	CECConfigs *pConfig = g_pGame->GetConfigs();
	if (!pConfig || pConfig->GetSystemSettings().bScaleUI)
	{
		// 如果选项中允许缩放，则调用基类默认处理缩放的方法
		CECBaseUIMan::CalcWindowScale();
		a_Clamp(m_fWindowScale, 0.78125f, 1.0f);
	}
	else
	{
		// 否则，保持不缩放
		m_fWindowScale = 1.0f;
	}
}

void CECLoginUIMan::RearrangeWindows(A3DRECT rcOld, A3DRECT rcNew)
{
	CECBaseUIMan::RearrangeWindows(rcOld, rcNew);
	RepositionLoginDialog();
	ScaleDialogToAllVisibleByHeight("Win_CreateRace");
	RepositionAnimation();
}

bool CECLoginUIMan::ChangeLayout(const char *pszFilename)
{
	bool ret = CECBaseUIMan::ChangeLayout(pszFilename);

	if(ret)
	{
		SetWindowScale(GetWindowScale());

		// 界面可能还需要一些对齐调整（设置相同的 rect）	
		A3DVIEWPORTPARAM param = *g_pGame->GetViewport()->GetA3DViewport()->GetParam();
		A3DRECT rcNew(param.X, param.Y, param.X+param.Width, param.Y+param.Height);
		RearrangeWindows(GetRect(), rcNew);
	}

	return ret;
}

CECCustomizeMgr * CECLoginUIMan::GetCustomizeMgr()
{
	return &m_CustomizeMgr;
}

bool CECLoginUIMan::LoadShowModel(int prof, int gender)
{
	if (CECProfConfig::Instance().IsExist(prof, gender)){
		CECLoginPlayer * & pPlayer = m_aModel[prof][gender];
		if (!pPlayer){
			pPlayer = new CECLoginPlayer(NULL);			
			GNET::RoleInfo Info;
			Info.race = CECProfConfig::Instance().GetRaceByProfession(prof);
			Info.occupation = prof;
			Info.gender = gender;
			CECPlayer::PLAYER_CUSTOMIZEDATA custom_data;
			if (CECPlayer::LoadDefaultCustomizeData(prof, gender, custom_data)){
				Info.custom_data.swap(GNET::Octets((const void *)&custom_data, sizeof(custom_data)));
			}
			if (!pPlayer->Load(Info)){
				assert(false);
				A3DRELEASE(pPlayer);
				a_LogOutput(1, "CECLoginUIMan::LoadShowModel, Failed to Load roleinfo for prof=%d, gender=%d", prof, gender);
			}else{
				if (m_bNewCharEquipShow){
					SwitchShowModelEquip(prof, gender, true);
				}
			}
		}
		return pPlayer != NULL;
	}
	return false;
}

bool CECLoginUIMan::IsShownNewModelEquip()
{
	return m_bNewCharEquipShow;
}

void CECLoginUIMan::SwitchShowModelEquip(bool bShow)
{
	m_bNewCharEquipShow = bShow;
	for (int i = 0; i < NUM_PROFESSION; ++ i)
	{
		for (int j = 0; j < NUM_GENDER; ++ j)
		{
			if (CECProfConfig::Instance().IsExist(i, j)){
				SwitchShowModelEquip(i, j, bShow);
			}
		}
	}
}

void CECLoginUIMan::SwitchShowModelEquip(int prof, int gender, bool bShow)
{
	if (!CECProfConfig::Instance().IsExist(prof, gender)){
		assert(false);
		return;
	}
	CECLoginPlayer *pPlayer = m_aModel[prof][gender];
	if (!pPlayer){
		return;
	}
	int	aEquips[SIZE_ALL_EQUIPIVTR] = {0};
	if (bShow){
		int equipPos[NEWCHAR_NUM_EQUIP] = {
			EQUIPIVTR_WEAPON,
			EQUIPIVTR_BODY,
			EQUIPIVTR_LEG,
			EQUIPIVTR_WRIST,
			EQUIPIVTR_FOOT,
			EQUIPIVTR_FLYSWORD,
		};
		for (int i(0); i < NEWCHAR_NUM_EQUIP; ++ i)
		{
			int id = m_aCreateEquips[prof][i];
			if (id > 0){
				aEquips[equipPos[i]] = id;
			}
		}
	}
	pPlayer->ShowEquipments(aEquips, true);
	if (pPlayer->HasWingModel()){
		pPlayer->ShowWing(true);
	}
	if (GetCurScene() >= LOGIN_SCENE_FACECUSTOMIZE_BEGIN && GetCurScene() <= LOGIN_SCENE_FACECUSTOMIZE_END ||
		GetCurScene() >= LOGIN_SCENE_CREATE_BEGIN && GetCurScene() <= LOGIN_SCENE_CREATE_END ||
		GetCurScene() >= LOGIN_SCENE_BODYCUSTOMIZE_BEGIN && GetCurScene() <= LOGIN_SCENE_BODYCUSTOMIZE_END){
		pPlayer->StandForCustomize();
	}else{
		pPlayer->Stand(true);
	}
}

void CECLoginUIMan::ChangeShowModel(int prof, int gender)
{
	//	改变当前显示到最前端的模型（对应 LOGIN_SCENE_CREATE_CHOOSE_PROFESSION）、或当前显示的模型
	//	参数 prof gender 必须合法
	//	m_nCurProfession m_nCurGender 可以非法
	if (!CECProfConfig::Instance().IsExist(prof, gender)){
		assert(false);
		return;
	}
	int lastProfession(m_nCurProfession), lastGender(m_nCurGender);
	m_nCurProfession = prof;
	m_nCurGender = gender;
	if (GetCurScene() == LOGIN_SCENE_CREATE_CHOOSE_PROFESSION){
		int race = CECProfConfig::Instance().GetRaceByProfession(prof);
		int order = CECProfConfig::Instance().GetRaceShowOrder(race);
		if (!CECProfConfig::Instance().IsProfession(lastProfession)){
			//	设置显示到最前方的职业
			RotateAnimator::Instance().SetFirstIndicator(order);	//	保证 RotateAnimator::GetIndicatorPos(order) 会获取到内部0的位置，即对应最前的位置
		}else{
			RotateAnimator::Instance().RotateToIndicator(order);
		}
		DisplayActionTrigger::Instance().Trigger(prof);
		RefreshPlayerList();
		TickRaceModels(0);
	}else{
		if (!LoadShowModel(prof, gender)){
			return;
		}
		//	此处需重新设置各模型位置、朝向参数，因为共用模型在 DisplayRaceModels 中被改变位置
		CECLoginPlayer * pPlayer = m_aModel[prof][gender];
		pPlayer->SetPos(m_aCreatePos[prof]);
		A3DVECTOR3 vecDirCam = m_aCamPos[LOGIN_SCENE_CREATE_BEGIN + prof] - m_aCreatePos[prof];
		vecDirCam.y = 0.0f;
		pPlayer->ChangeModelMoveDirAndUp(vecDirCam, g_vAxisY);
		pPlayer->Tick(0);
		pPlayer->StandForCustomize();
		RefreshPlayerList();
	}
}

void CECLoginUIMan::SwitchToCreate(CECPlayer* player)
{
	if (!player){
		//	初入创建角色
		m_nCurProfession = -1;
		m_nCurGender = -1;
		DisplayActionTrigger::Instance().Reset();
		PAUIDIALOG pDlgCreateProfession = GetDialog("Win_CreateProfession");
		pDlgCreateProfession->Show(true);
	}else{
		//	从完整个性化界面过来
		m_nCurProfession = player->GetProfession();
		m_nCurGender = player->GetGender();
		PAUIDIALOG pDlgGenderName = GetDialog("Win_CreateGenderName");
		pDlgGenderName->Show(true);
	}
}

void CECLoginUIMan::LoadRaceDisplayModels()
{
	//	加载所有需展示模型
	for (int i = 0; i < NUM_PROFESSION; ++ i)
	{
		for (int j = 0; j < NUM_GENDER; ++ j)
		{
			if (CECProfConfig::Instance().CanShowOnCreate(i, j)){
				LoadShowModel(i, j);
			}
		}
	}
}

void CECLoginUIMan::TickRaceModels(DWORD dwDeltaTime)
{
	if (GetCurScene() != LOGIN_SCENE_CREATE_CHOOSE_PROFESSION){
		return;
	}
	RotateAnimator::Instance().Tick(dwDeltaTime);
	DisplayActionTrigger::Instance().Tick(dwDeltaTime);
	for (int i = 0; i < NUM_PROFESSION; ++ i)
	{
		bool moveFrontTriggered = DisplayActionTrigger::Instance().MoveFrontActionTriggered(i);
		bool goBackTriggered = DisplayActionTrigger::Instance().GoBackActionTriggered(i);
		bool displayActionTriggered = DisplayActionTrigger::Instance().DisplayActionTriggered(i);
		bool standActionTriggered = DisplayActionTrigger::Instance().StandActionTriggered(i);
		A3DVECTOR3 pos(0.0f), dir(g_vAxisY);
		DisplayActionTrigger::Instance().GetPosDir(i, pos, dir);
		for (int j = 0; j < NUM_GENDER; ++ j)
		{
			if (!CECProfConfig::Instance().CanShowOnCreate(i, j) ||
				m_aModel[i][j] == NULL){
				continue;
			}
			CECLoginPlayer *pPlayer = m_aModel[i][j];
			pPlayer->SetPos(pos);
			pPlayer->ChangeModelMoveDirAndUp(dir, g_vAxisY);
			if (moveFrontTriggered){
				pPlayer->MoveFront();
			}else if (goBackTriggered){
				pPlayer->GoBack();
			}else if (displayActionTriggered){
				pPlayer->ShowDisplayAction();
			}else if (standActionTriggered){
				pPlayer->Stand(true);
			}
			pPlayer->Tick(0);
		}
	}
}

void CECLoginUIMan::SwitchToSelectChar()
{
	m_pDlgSelect->Switch(true);	
	RefreshRole(GetIDCurRole());
}

void CECLoginUIMan::SwitchToCustomize(bool bCallForCreate)
{
	Init_Customize(bCallForCreate);
}

void CECLoginUIMan::CancelCustomize(){
	if (IsCustomizeCharacter()){
		GetDialog("Win_Customize")->OnCommand("IDCANCEL");
	}
}

void CECLoginUIMan::ConfirmCustomize(){
	if (IsCustomizeCharacter()){
		GetDialog("Win_Customize")->OnCommand("IDOK");
	}
}

void CECLoginUIMan::ClearRoleList(){
	for(int i = 0; i < int(m_vecRoleModel.size()); i++ )
		A3DRELEASE(m_vecRoleModel[i]);
	m_vecRoleModel.clear();
	m_vecRoleInfo.clear();
	m_idCurRole = -1;
	SetRoleListReady(false);
	RebuildRoleList();
	m_roleDisplayActionTrigger->Reset();
}

void CECLoginUIMan::SwitchToLogin()
{
	s_bNeedTestRoleAutoLogin = false;
	m_pDlgSelect->Switch(false);

	ClearRoleList();	
	
	g_pGame->GetGameSession()->Close();
	SelectRoleSent(false);
	
	if (!m_pDlgLogin->IsShow())
		m_pDlgLogin->Show(true);

	ChangeScene(LOGIN_SCENE_LOGIN);
}

void CECLoginUIMan::ShowLoginDlg()
{
	if (g_pGame->GetGameRun()->IsFirstShow()){
		//	当前服务器为空，根据情况选择默认服务器
		switch (CECLoginSwitch::Instance().GetLoginType())
		{
		case CECLoginSwitch::LT_DEFAULT:
			{
				const CECRecordAccount::Record *r = CECRecordAccount::Instance().GetLatestRecord();
				if (r){
					if (r->server >= 0){
						CECServerList::Instance().SelectServer(r->server);
					}
					if (!r->account.IsEmpty()){
						m_pDlgLogin->SetUser(r->account);
					}
				}
				break;
			}
		case CECLoginSwitch::LT_PWD:
			{
				ACString strUser = CECLoginSwitch::Instance().GetUser();
				const CECRecordAccount::Record *r = CECRecordAccount::Instance().FindRecord(strUser);
				if (r){
					if (r->server >= 0){
						CECServerList::Instance().SelectServer(r->server);
					}
				}
				break;
			}
		}
		if (CECServerList::Instance().GetSelected() < 0){
			CECServerList::Instance().LoadServer();
			CDlgLoginServerList *pDlgServerList = dynamic_cast<CDlgLoginServerList *>(GetDialog("Win_LoginServerList"));
			pDlgServerList->Show(true);
		}else{
			m_pDlgLogin->Show(true);
			DefaultLogin();
		}
		g_pGame->GetGameRun()->OnFirstShowEnd();
	}else{
		if (!m_pDlgLogin->IsShow()){
			m_pDlgLogin->Show(true);
		}
	}
}

void CECLoginUIMan::DefaultLogin()
{	
	if (CECLoginSwitch::Instance().CanAutoLogin()){
		ClickLoginButton();
	}
}

void CECLoginUIMan::ClickLoginButton(){
	m_pDlgLogin->OnCommand("confirm");
}

void CECLoginUIMan::ReclickLoginButton(){
	g_pGame->GetGameSession()->ReLogin(false);
	g_pGame->GetGameRun()->SetSellingRoleID(0);
	SelectRoleSent(false);
	m_pDlgSelect->EnableEnterGame(true);
	ClickLoginButton();
}

void CECLoginUIMan::SetIDCurRole(int idRole)
{
	m_idCurRole = idRole;
	if(m_idCurRole >= (int)m_vecRoleModel.size())
	{
		m_idCurRole = -1;
	}

	if(m_idCurRole < 0)
	{
		return;
	}

	const GNET::RoleInfo &role = m_vecRoleInfo[idRole];
	
	int nProf = role.occupation;
	int nIndex = LOGIN_SCENE_CREATE_BEGIN + nProf;	
	ChangeCameraByScene((LOGIN_SCENE)nIndex);

	//	为方便通过 CECGameRun::GetPlayerName 查询自身名称	
	ACString strName((const ACHAR *)role.name.begin(), role.name.size() / sizeof(ACHAR));
	g_pGame->GetGameRun()->AddPlayerName(role.roleid, strName);

	// create the role model
	bool bNew(false);
	if( !m_vecRoleModel[idRole] )
	{
		bNew = true;
		CECLoginPlayer *pRole = new CECLoginPlayer(NULL);		
		bool bval = pRole->Load(const_cast<GNET::RoleInfo &>(role));
		if( !bval ){
			AUI_ReportError(__LINE__, __FILE__);
			return;
		}
		m_vecRoleModel[idRole] = pRole;
	}
	CECLoginPlayer *pPlayer = m_vecRoleModel[idRole];
	pPlayer->SetPos(m_aCreatePos[nProf]);
	A3DVECTOR3 vecDirCam = m_aCamPos[nIndex] - m_aCreatePos[nProf];
	vecDirCam.y = 0.0f;
	pPlayer->ChangeModelMoveDirAndUp(vecDirCam, g_vAxisY);
	
	m_roleDisplayActionTrigger->Reset();
	if( CECCrossServer::Instance().IsWaitLogin() ||
		CECReconnect::Instance().IsReconnecting()){
		pPlayer->Stand(true);
	}else if (role.status == _ROLE_STATUS_NORMAL ||
		role.status == _ROLE_STATUS_ON_CROSSSERVER){
		if (pPlayer->TestShowDisplayAction(m_aCreateEquips[nProf][NEWCHAR_WEAPON])){
			m_roleDisplayActionTrigger->StartCounting(500);
			pPlayer->StandNow();	//	先清除上次选中结果、即重置为默认动作
		}else{
			pPlayer->Stand(true);
		}
	}else{
		pPlayer->SitDown();
	}

	// for update terrain water's current water height data
	g_pGame->GetGameRun()->GetWorld()->GetTerrainWater()->UpdateWaterHeight();
}

CECLoginPlayer * CECLoginUIMan::GetCurRoleModel()
{
	int idCurRole = GetIDCurRole();
	if (idCurRole >= 0 && idCurRole < (int)m_vecRoleModel.size()){
		return m_vecRoleModel[idCurRole];
	}
	return NULL;
}

bool CECLoginUIMan::CanModify(int roleid)
{
	for( int i = 0; i < (int)m_vecRoleInfo.size(); i++ )
	{
		const GNET::RoleInfo &info = m_vecRoleInfo[i];
		if( roleid != info.roleid ){
			continue;
		}
		time_t curTime = time(NULL);
		if (curTime < info.create_time ||	//	内服由于服务器时间修改为超前客户端的情况，允许个性化修改
			curTime - info.create_time < 2 * 24 * 3600){
			short* aStatus = (short*)info.custom_status.begin();
			int iNumStatus = info.custom_status.size() / sizeof (short);
			if (aStatus && iNumStatus){
				elementdataman* pDataMan = g_pGame->GetElementDataMan();
				for (int j=0; j < iNumStatus; j++){
					DATA_TYPE DataType = pDataMan->get_data_type(aStatus[j], ID_SPACE_ESSENCE);
					if (DataType == DT_FACEPILL_ESSENCE){
						return false;
					}
				}
			}
			return true;
		}
		break;
	}
	
	return false;
}

void CECLoginUIMan::RefreshRole(int index, bool bForce)
{	
	if(bForce)
	{
		SetIDCurRole(-1);
	}

	m_pDlgSelect->SelectChar(index);
}

bool CECLoginUIMan::LaunchRoleSelling(int roleid)
{	
	if( roleid > 0 )
	{
		for(int i = 0; i < (int)m_vecRoleInfo.size(); i++ )
		{
			if( m_vecRoleInfo[i].roleid == roleid )
			{
				ChangeScene(LOGIN_SCENE_SELCHAR);
				m_pDlgTrade->SetRoleInfo(m_vecRoleInfo[i]);
				RefreshRole(i, false);
				return true;
			}
		}
		
		ShowErrorMsg2(GetStringFromTable(5550));
	}

	return false;
}

bool CECLoginUIMan::PrepostCharacter(int idRole)
{
	for( int i = 0; i < (int)m_vecRoleInfo.size(); i++ )
	{
		if( idRole != m_vecRoleInfo[i].roleid ) continue;
		
		// unlock UI
		m_pDlgSelect->DeleteCharacter(i);
		
		m_vecRoleInfo[i].status = _ROLE_STATUS_FROZEN;
		m_vecRoleModel[i]->SitDown();
		m_roleDisplayActionTrigger->Reset();
		
		return true;
	}

	return false;
}

void CECLoginUIMan::WebTradeAction(int idAction, void *pData)
{
	// show the detailed info
	if(idAction == PROTOCOL_WEBTRADEGETDETAIL_RE)
	{
		CDlgWebTradeInfo* pDlgInfo = (CDlgWebTradeInfo*)GetDialog("Win_TradeInfo");
		if(pDlgInfo)
		{
			pDlgInfo->SetTradeDetail(*(GNET::WebTradeGetDetail_Re*)pData);
			pDlgInfo->Show(true);
		}
	}
	else
	{
		m_pDlgTrade->WebTradeAction(idAction, pData);
	}
}

void CECLoginUIMan::OnSSOGetTicket_Re(const GNET::SSOGetTicket_Re *pProtocol)
{
	if (!pProtocol)	return;
	
	if (pProtocol->retcode==0)
	{
		// 游戏由Arc启动后，用户在登录界面更换账户
		if (pProtocol->local_context == CECArcAsia::GetSingleton().GetArcAsiaContext()) {
			AString strTicket((const char *)pProtocol->ticket.begin(), pProtocol->ticket.size());
			if (strTicket.IsEmpty()) {
				a_LogOutput(1, "CECGameUIMan::OnSSOGetTicket_Re, arc ticket is empty.");
			} else {
				CECArcAsia::GetSingleton().OnSSOGetTicket_Re(AS2AC(strTicket));
			}
			return;
		}
	}
	//	显示错误码作为默认处理
	else
	{
		ACString strError = GetStringFromTable(9260+pProtocol->retcode);
		if (strError.IsEmpty())
			strError.Format(GetStringFromTable(9260), pProtocol->retcode);
		ShowErrorMsg(strError);
	}
}

//---------------------------------------------------------------------
void CECLoginUIMan::InitDragRole(LPARAM lParam)
{
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	
	m_bSelectCharDragRole = true;
	m_ptDragStart.x = GET_X_LPARAM(lParam) - p->X;
	m_ptDragStart.y = GET_Y_LPARAM(lParam) - p->Y;
	g_pGame->ShowCursor(false);	
}

void CECLoginUIMan::ResetDragRole(bool bflag) 
{ 
	m_bSelectCharDragRole = bflag; 
	g_pGame->ShowCursor(true); 
}

void CECLoginUIMan::DragRole(LPARAM lParam)
{
	CECLoginPlayer *pPlayer = GetCurRoleModel();
	if (!pPlayer){
		return;
	}
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int mx = GET_X_LPARAM(lParam) - p->X;
	int my = GET_Y_LPARAM(lParam) - p->Y;
				
	A3DMATRIX4 matRotate = RotateY((m_ptDragStart.x - mx) * 0.003f);
				
	A3DVECTOR3 vecDir = pPlayer->GetDir();
	A3DVECTOR3 vecUp = pPlayer->GetUp();
	vecDir = a3d_VectorMatrix3x3(vecDir, matRotate);
	vecUp = a3d_VectorMatrix3x3(vecUp, matRotate);
	pPlayer->ChangeModelMoveDirAndUp(vecDir, vecUp);
}


void CECLoginUIMan::ResetCursor()
{
	if( m_bSelectCharDragRole )
	{	// Restore cursor position to start position
		POINT pt = {m_ptDragStart.x, m_ptDragStart.y};
		::ClientToScreen(g_pGame->GetGameInit().hWnd, &pt);
		::SetCursorPos(pt.x, pt.y);
	}
}
void CECLoginUIMan::RepositionLoginDialog(){
	if (m_pDlgLogin){
		m_pDlgLogin->SetPosEx(m_rcWindow.Width()/2-40*GetWindowScale(), m_rcWindow.Height()/2);	//	2014年胧族资料片：与月亮对齐
	}
}

void CECLoginUIMan::ReselectRole(){
	SelectRoleSent(false);
	m_pDlgSelect->EnableEnterGame(true);
	m_pDlgSelect->EnableCreate(true);
}
