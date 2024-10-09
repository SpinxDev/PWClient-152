/********************************************************************
	created:	2005/09/06
	created:	6:9:2005   18:02
	file name:	ECScriptUIInGame.h
	author:		yaojun
	
	purpose:	implemnet of IECScritUI for game
*********************************************************************/

#pragma once

#include "ECScriptUI.h"
#include "ECScriptTipWorkMan.h"

class CDlgScriptHelp;
class CDlgInfo;
class AUITextArea;
class CECScriptContext;
class AUIObject;
class CDlgScriptHelpHistory;
class CDlgScriptTip;

class CECScriptUIInGame : public IECScriptUI
{
private:
	CECScriptContext * m_pContext;
private:
	CDlgScriptHelp * GetHelpDlg();
	CDlgInfo * GetInfoDlg();
	CDlgScriptHelpHistory * GetHelpHistoryDialog();
	CDlgScriptTip * GetTipDlg();
public:
	CECScriptUIInGame();
	virtual ~CECScriptUIInGame();

	bool Init(CECScriptContext * pContext);

	// IECScriptUI
	virtual void ShowHelpDialog();
	virtual void CloseHelpDialog(bool bCloseHistory = true);
	virtual void SetHasNextTip(bool bHasNext);
	virtual void SetHelpDialogContent(AString strContent);
	virtual void SetHelpDialogImage(AString strFile);
	virtual void AddToSpirit(int nScriptID, AString strName);
	virtual void SetTipDialogTitleAndContent(const AString& strTitle, const AString& strContent);
	virtual void ShowTip(int iGfxTime, int iInTime, int iStayTime, int iOutTime);
};