// Filename	: DlgQuickBar.h
// Creator	: Tom Zhou
// Date		: October 11, 2005

#pragma once

#include "DlgBase.h"
class CECShortcutSet;
class AUIImagePicture;
class AUICheckBox;

class CDlgQuickBar : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgQuickBar();
	virtual ~CDlgQuickBar();

	void OnCommand_add(const char * szCommand);
	void OnCommand_minus(const char * szCommand);
	void OnCommand_openextra(const char * szCommand);
	void OnCommand_switch(const char * szCommand);
	void OnCommand_enableext(const char * szCommand);
	void OnCommand_new(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_lock(const char * szCommand);

	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	bool UpdateShortcuts();
	void DelGoblinSkillSC();
	static int GetCurPanel1();
	static int GetCurPanel2();

	void ParseName(int &size, int &num, int &index, bool &horizontal);
	
	friend class CECGameUIMan;

	static int m_nCurPanel1, m_nCurPanel2;
	static bool	m_bShowAll1, m_bShowAll2;
	static int	m_nDisplayPanels1, m_nDisplayPanels2;

	static void GetQuickBarNameAndSC(CECHostPlayer* pHost,abase::vector<AString>& pszPanel,abase::vector<CECShortcutSet *>* pSCS=NULL, int panel9=0,int panel8=0);

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();
	void SwitchHotkeyPanel(int nDelta);
	bool m_bDelGoblinSkillSC;

	abase::vector<AUIImagePicture*> m_vecImgCell;
	virtual bool Render();

	AUICheckBox* m_pChkLock;
};
