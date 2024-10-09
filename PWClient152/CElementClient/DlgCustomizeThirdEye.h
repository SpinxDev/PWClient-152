
#pragma once

#include "DlgCustomizeBase.h"
#include "AUI_ImageGrid.h"


#define THIRDEYE_MAX_ROW 1
#define THIRDEYE_MAX_COL 3

class CDlgCustomizeThirdEye : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

protected:

	virtual void OnShowDialog();
	virtual void OnTick();

	virtual bool OnInitContext();

	AUI_ImageGrid m_LbxThirdEye;

public:

	CDlgCustomizeThirdEye();
	virtual ~CDlgCustomizeThirdEye();
	
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Close(const char * szCommand);

	void OnLButtonUpListBoxThirdEye(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseWheelListboxThirdEye(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	// this also used by CDlgCustomizeEye
	static bool InitResource(CECGame* pGame, CECPlayer* pPlayer, CECCustomizeMgr* pMgr, AUI_ImageGrid& imgGrid);
};
