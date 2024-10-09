// Filename	: DlgPalette.h
// Creator	: Xu Wenbin
// Date		: 2010/1/14

#pragma once

#include "DlgPaletteBase.h"

class AUIStillImageButton;

class CDlgPalette : public CDlgPaletteFashion {

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgPalette();
	void OnCommand_ResetColor(const char * szCommand);
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual void OnSetColor(const A3DCOLOR& color);

	bool GetEquipEnable(enumChangeTarget target);
	void SaveOriginColor();

private:
	A3DCOLOR m_originColor[CHANGE_ALL];
	bool m_hasEquip[CHANGE_ALL];
	AUIStillImageButton* m_pBtnRestore;
};

