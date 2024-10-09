// Filename	: EC_BaseUIMan.h
// Creator	: Feng Ning
// Date		: 2010/10/18

#pragma once

#include "AUILuaManager.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////
class CECUIAnimation;
class CDlgExplorer;
class CECCustomizeMgr;
class CECURLOSNavigator;

namespace GNET{
	class SSOGetTicket_Re;
}

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

class CECBaseUIMan : public AUILuaManager
{

public:		//	Constructor and Destructor
	CECBaseUIMan();
	virtual ~CECBaseUIMan();

	virtual bool Init(A3DEngine* pA3DEngine, A3DDevice* pA3DDevice, const char* szDCFile=NULL);
	virtual bool Release(void);
	virtual bool Render(void);
	virtual bool Tick(DWORD dwDeltaTime);

	void OptimizeSpriteResource(bool bOptimize);
	
	// whether the change is performing
	bool IsChanging() {return m_bChanging;}
	
	// use another DCF file to change the UI theme
	virtual bool ChangeLayout(const char *pszFilename);
	
	virtual PAUIDIALOG CreateDlgInstance(const AString strTemplName);

	virtual bool LoadTemplateFrame();

	//	角色个性化
	virtual CECCustomizeMgr * GetCustomizeMgr(){ return NULL; }
	bool IsCustomizeCharacter();
	bool AddRestoreDlgToCC(AUIDialog *pDlg);		//	CC 为 Customize Character 缩写
	bool RemoveRestoreDlgToCC(AUIDialog *pDlg);

	//	境界名称
	ACString GetRealmName(int realmLevel);
	
	// switch the debug mode
	virtual bool RenderUIDebug();
	virtual ACString GetDebugString();
	bool IsDebugMode() const {return m_IsDebug;}
	bool SetDebugMode(bool val) {return m_IsDebug = val;}

public:
	// return a formatted number such as 1,234,567,890
	ACString GetFormatNumber(__int64 i) const;
	
	// return a formatted time string that contains 2 unit
	ACString GetFormatTime(int time);

	// return a formatted time string that contains 1 unit
	ACString GetFormatTimeSimple(int time);

	ACString GetShopItemFormatTime(int nTime);
	
	A3DCOLOR GetPriceColor(__int64 i) const;

	// replace key color to new color value.
	// return value is the replaced color count.
	int ReplaceColor(ACString* pStr, A3DCOLOR keyColor, A3DCOLOR newColor) const;

	// Start a animation, the pointer will be stored in manager
	void StartAnimation(PAUIDIALOG pDlg, CECUIAnimation* pAni);
	// Reposition Animation after ResizeWindows() called
	void RepositionAnimation();
	//	Remove Animation when unneeded
	void RemoveAnimation(PAUIDIALOG pDlg);
	
	void NavigateURL(const ACString &strURL, const char *szWinExplorer="Win_Explorer", bool bByCoreClient=false, CECURLOSNavigator *pOSNavigatorInWinExplorer=NULL);
	void NavigateURL(const AString &strURL, const char *szWinExplorer="Win_Explorer", bool bByCoreClient=false, CECURLOSNavigator *pOSNavigatorInWinExplorer=NULL) { NavigateURL(AS2AC(strURL), szWinExplorer, bByCoreClient, pOSNavigatorInWinExplorer); }

	bool NavigateTWRechargeURL();
	void NavigateDefaultRechargeURL();
	void NavigateRechargeURL();

	bool PreTranslateMessage(MSG &msg);
	int	   GetWebBrowserDlgCount();
	CDlgExplorer * GetWebBrowserDlg(int index);
	bool  IsWebBrowser(HWND hWnd);

	bool IsDialogBefore(PAUIDIALOG pDialogBefore, PAUIDIALOG pDialogAfter);	
	void ScaleDialogToAllVisibleByHeight(const char *szDialogName);
	void SetLinearFilter(const char *szDialogName);

	AString GetURL(const char *szSec, const char *szKey);
	
	virtual A3DCOLOR GetMsgBoxColor();
	void ShowErrorMsg(const ACHAR *pszMsg, const char *pszName = "");
	void ShowErrorMsg(int idString);
	virtual void ShowReconnectMsg(const ACHAR *pszMsg, const char *pszName);
	virtual void OnSSOGetTicket_Re(const GNET::SSOGetTicket_Re *pProtocol){};

protected:
	abase::vector<CDlgExplorer *>	m_webBrowserDialogs;

private:
	bool m_bChanging;

	// UI Debug Related
	PAUIFRAME m_pAUIFrameDebug;
	bool m_IsDebug;

	// Animation related
	abase::vector<CECUIAnimation*> m_Animations;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

