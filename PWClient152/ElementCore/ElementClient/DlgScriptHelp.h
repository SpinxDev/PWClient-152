#pragma once

#include "DlgBase.h"

class AUIStillImageButton;
class AUITextArea;
class AUICheckBox;
class AUIEditBox;
class CECScriptContext;
class CDlgSettingScriptHelp;
class CDlgScriptHelpHistory;

class CDlgScriptHelp : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
	virtual void OnChangeLayoutEnd(bool bAllDone);
	
public:
	CDlgScriptHelp();
	virtual ~CDlgScriptHelp();
	virtual void DoDataExchange(bool bSave);


	void OnCommandIDCANCEL(const char * szCommand);
	void OnCommandNext(const char * szCommand);
	void OnCommandClose(const char * szCommand);
	void OnCommandDisableHelp(const char * szCommand);
	void OnCommandSetting(const char * szCommand);
	void OnCommandHistory(const char * szCommand);
	void OnMouseMove(WPARAM wParam, LPARAM lParam, AUIObject * pObj);


	void SetScriptContext(CECScriptContext * pContext);
	void SetTipText(const ACHAR * szText);
	void SetTipImage(const AString& szFile);
	void SetHasNextTip(bool bNext);
	bool GetIsDisableHelp();
	void SetIsDisableHelp(bool bDisable);
	void AdjustHelpHistoryWindowPosition();

private:
	CECScriptContext *		m_pContext;
	AUITextArea *			m_pTAreaTip;
	AUIStillImageButton *	m_pBtnNext;
	AUIStillImageButton *	m_pBtnClose;
	AUICheckBox *			m_pChkDisableHelp;
	AUIEditBox *			m_pEdtImage;
	AUIOBJECT_SETPROPERTY	m_defaultImgProperty;
private:
	CDlgSettingScriptHelp *	GetSettingDlg();
	CDlgScriptHelpHistory * GetHistoryDlg();
	void AjustPos();
};