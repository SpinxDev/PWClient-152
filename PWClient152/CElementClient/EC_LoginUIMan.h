/*
 * FILE: EC_LoginUIMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AUI.h"
#include "roleinfo"
#include "Expression.h"
#include "FaceAnimation.h"
#include "EC_CustomizeMgr.h"
#include "EC_RoleTypes.h"
#include "EC_BaseUIMan.h"

class CECFace;
class CECPlayer;
class CECLoginPlayer;
class CDlgLogin;
class CDlgSelect;
class CDlgWebRoleTrade;
class CDlgLoginQueue;
class A3DSurface;
class A3DGFXEx;
class ActionTrigger;

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
#define CECLOGINUIMAN_MAX_ROLES_SHOW	8
#define CECLOGINUIMAN_MAX_ROLES_CREATE	8

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECLoginUIMan
//	
///////////////////////////////////////////////////////////////////////////

class CECLoginUIMan : public CECBaseUIMan
{
public:
	enum LOGIN_SCENE
	{
		LOGIN_SCENE_NULL		= -1,
		LOGIN_SCENE_LOGIN		= 0,
		LOGIN_SCENE_SELCHAR,

		LOGIN_SCENE_FACECUSTOMIZE_BEGIN,
		LOGIN_SCENE_FACECUSTOMIZE_END = LOGIN_SCENE_FACECUSTOMIZE_BEGIN + NUM_PROFESSION-1,

		LOGIN_SCENE_CREATE_BEGIN,
		LOGIN_SCENE_CREATE_END = LOGIN_SCENE_CREATE_BEGIN + NUM_PROFESSION-1,

		LOGIN_SCENE_BODYCUSTOMIZE_BEGIN,
		LOGIN_SCENE_BODYCUSTOMIZE_END = LOGIN_SCENE_BODYCUSTOMIZE_BEGIN + NUM_PROFESSION-1,

		LOGIN_SCENE_CREATE_CHOOSE_PROFESSION,
		
		LOGIN_SCENE_MAX
	};

public:		//	Constructor and Destructor
	CECLoginUIMan();
	virtual ~CECLoginUIMan();

	//	Initialize manager
	virtual bool Init(A3DEngine* pA3DEngine, A3DDevice* pA3DDevice, const char* szDCFile=NULL);
	//	Release manager
	virtual bool Release(void);

	virtual bool Tick(DWORD dwDeltaTime);
	//	On command
	virtual bool OnCommand(const char* szCommand, AUIDialog* pDlg);
	//	On message box
	virtual bool OnMessageBox(int iRetVal, AUIDialog* pDlg);
	//	Handle windows message
	virtual bool DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool DealRotateSelCharMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//	Handle event
	virtual bool OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, AUIDialog* pDlg, AUIObject* pObj);
	//  窗口缩放策略
	virtual void CalcWindowScale();
	//  窗口缩放额外处理
	virtual void RearrangeWindows(A3DRECT rcOld, A3DRECT rcNew);	
	virtual bool ChangeLayout(const char *pszFilename);
	virtual CECCustomizeMgr * GetCustomizeMgr();

	bool LaunchPreface();
	bool LaunchMatrixChallenge(void* pData);
	bool LaunchCharacter();
	bool LaunchLoading();
	bool LaunchRoleSelling(int roleid);

	bool CreateCharacter(GNET::RoleInfo &ri);	//	Modified by duyuxin
	bool AddCharacter(GNET::RoleInfo* pri);		//	Modified by duyuxin
	bool DelCharacter(int nResult, int idRole);
	bool SelectLatestCharacter();
	bool RestoreCharacter(int nResult, int idRole);
	bool ChangeCustomizeData(int idRole, int result);
	bool PrepostCharacter(int idRole);
	bool RemoveLocalCharacter(int idRole);
	
	bool UIControlCursor();
	virtual A3DCOLOR GetMsgBoxColor();
	void ShowErrorMsg2(const ACHAR *pszMsg, const char *pszName = "");
	virtual void ShowReconnectMsg(const ACHAR *pszMsg, const char *pszName);

	CECPlayer* GetCurPlayer(){ return m_pCurPlayer; };
	CECFace* GetCurFace(){ return m_pCurFace;};
	int GetIDCurRole() const { return m_idCurRole;};
	void SetIDCurRole(int nIDCurRole);
	CECLoginPlayer * GetCurRoleModel();
	const abase::vector<GNET::RoleInfo>& GetVecRoleInfo() const { return m_vecRoleInfo;}
	int PickRole(int x, int y);

	bool IsSceneValid(LOGIN_SCENE scene)const;
	void ChangeCameraByScene(LOGIN_SCENE scene);
	bool ChangeScene(LOGIN_SCENE scene);
	LOGIN_SCENE	GetCurScene(){ return m_curScene; }
	void ChangeSceneByRole();

	bool GetReferrerBound() const { return m_bReferrerBound; }
	void SetReferrerBound(bool bBound){ m_bReferrerBound = bBound; }
	
	char GetPasswdFlag() const { return m_passwdFlag; }
	void SetPasswdFlag(char passwd_flag){m_passwdFlag = passwd_flag;}
	
	char GetUsbBind() const { return m_usbBind; }
	void SetUsbBind(char usbbind);
	void SetRoleListReady(bool bReady){ m_roleListReady = bReady; }
	bool GetRoleListReady(){ return m_roleListReady; }

	bool LoadShowModel(int prof, int gender);
	void SwitchShowModelEquip(bool bShow);
	void SwitchShowModelEquip(int prof, int gender, bool bShow);
	bool IsShownNewModelEquip();
	void ChangeShowModel(int prof, int gender);
	void LoadRaceDisplayModels();
	void TickRaceModels(DWORD dwDeltaTime);
	int  GetCurProfession(){ return m_nCurProfession; }
	int  GetCurGender(){ return m_nCurGender; }

	void NewCharacter(const ACString& name);

	void SwitchToCreate(CECPlayer* player);
	void SwitchToCustomize(bool bCallForCreate);
	void CancelCustomize();
	void ConfirmCustomize();
	void SwitchToSelectChar();
	void SwitchToLogin();
	void RenderVersion();

	void ShowLoginDlg();
	void DefaultLogin();
	void ClickLoginButton();
	void ReclickLoginButton();

	bool LoginSelectedChar();
	bool SelectLoginChar(int iChar);
	void CancelAutoLogin();

	int  FindCharByName(const ACString &strCharName)const;
	int  FindCharByRoleID(int roleID)const;

	void RebuildRoleList();
	bool RefreshPlayerList();
	void RefreshRole(int index, bool bForce = true);

	bool CanModify(int roleid);

	void WebTradeAction(int idAction, void *pData);	
	
	void SelectRoleSent(bool bSent){ m_bSelectRoleSent = bSent;}
	bool IsSelectRoleSent()const { return m_bSelectRoleSent; }
	void ReselectRole();

	virtual void OnSSOGetTicket_Re(const GNET::SSOGetTicket_Re *pProtocol);

	// 选择角色界面右键旋转人物相关函数
	void InitDragRole(LPARAM lParam);
	void ResetDragRole(bool bflag); 
	void DragRole(LPARAM lParam);
	void ResetCursor(); 

protected:
	virtual PAUIDIALOG CreateDlgInstance(const AString strTemplName);
	bool DealMessageBoxQuickKey(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool OnCommand_Referreral(const char* szCommand, AUIDialog* pDlg);
	void RepositionLoginDialog();
	
	bool Init_Customize(bool bCallForCreate = true);
	void ClearRoleList();

	void NewCharacterImpl(const ACString& name);
	
	void AdjustCamera();
	void AdjustRole(int x, int y);
	
	void AutoLogin();
	void UpdateRedirectLoginPos();

protected:	//	Attributes

	CECPlayer *	m_pCurPlayer;			// current player to be customized
	CECFace *	m_pCurFace;				// current face to be customized

	LOGIN_SCENE	m_curScene;				// current scene

	A3DVECTOR3 m_aCamPos[LOGIN_SCENE_MAX];
	A3DVECTOR3 m_aCamDir[LOGIN_SCENE_MAX];
	A3DVECTOR3 m_aCamUp[LOGIN_SCENE_MAX];
	A3DVECTOR3 m_aCreatePos[LOGIN_SCENE_CREATE_END - LOGIN_SCENE_CREATE_BEGIN + 1];
	A3DVECTOR3 m_aCreateDisplayCenter;
	float	   m_aCreateDisplayRadius;
	float	   m_aCreateDeltaDist;
	float	   m_aCreateSelectMove;

	enum {
		NEWCHAR_WEAPON,
		NEWCHAR_UPPERBODY,
		NEWCHAR_LOWERBODY,
		NEWCHAR_WRIST,
		NEWCHAR_FOOT,
		NEWCHAR_WING,
		NEWCHAR_NUM_EQUIP,
	};
	int		   m_aCreateEquips[NUM_PROFESSION][NEWCHAR_NUM_EQUIP];

    // 登陆选择角色界面旋转人物标志
	bool  m_bSelectCharDragRole;
	APointI		m_ptDragStart;		// drag start position

	// Select role.
	int	m_idCurRole;

	typedef abase::vector<GNET::RoleInfo> RoleInfoList;
	RoleInfoList	m_vecRoleInfo;

	typedef abase::vector<CECLoginPlayer *> LoginPlayerList;
	LoginPlayerList m_vecRoleModel;
	ActionTrigger*		m_roleDisplayActionTrigger;

	int m_nCurProfession, m_nCurGender;
	CECLoginPlayer *m_aModel[NUM_PROFESSION][NUM_GENDER];
	bool			m_bNewCharEquipShow;
	
	CECCustomizeMgr m_CustomizeMgr;//自定义管理器

	// referrer related
	bool m_bReferrerBound;
	ACString m_strReferrerID;

	// password change notify
	static char m_passwdFlag;
	
	// u-key usage
	static char m_usbBind;

	// dlg handle
	CDlgLogin*				m_pDlgLogin;
	CDlgSelect*				m_pDlgSelect;
	CDlgWebRoleTrade*		m_pDlgTrade;	
	CDlgLoginQueue	*		m_pDlgLoginQueueVIP;
	CDlgLoginQueue	*		m_pDlgLoginQueueNormal;
	
	bool m_roleListReady;
	bool m_bSelectRoleSent;		//	是否刚已向服务器发送 SelectRole 协议
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

