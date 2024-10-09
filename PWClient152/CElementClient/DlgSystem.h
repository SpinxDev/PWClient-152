// File		: DlgSystem.h
// Creator	: Xiao Zhou
// Date		: 2005/8/15

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUICheckBox.h"
#include "AUIStillImageButton.h"

// 具有横竖两种形态的对话框基类
class CDlgHorizontalVertical : public CDlgBase {
public:
	CDlgHorizontalVertical() : m_pTwinDlg(NULL) {}
	CDlgHorizontalVertical* GetTwinDialog();
	bool IsHorizontal();
private:
	CDlgHorizontalVertical* m_pTwinDlg;
	bool m_bHorizontal;
};


class CDlgSystem : public CDlgHorizontalVertical  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandInventory(const char * szCommand);
	void OnCommandCharacter(const char * szCommand);
	void OnCommandSkill(const char * szCommand);
	void OnCommandQuest(const char * szCommand);
	void OnCommandShop(const char * szCommand);
	void OnCommandMain2(const char * szCommand);
	void OnCommandMain3(const char * szCommand);
	void OnCommandMini(const char * szCommand);
	void OnCommandMain4(const char* szCommand);
	void OnCommandMain5(const char* szCommand);
	void OnCommandModeChange(const char* szCommand);

	CDlgSystem();
	virtual ~CDlgSystem();

	void ShowNewImg(bool bShow);

protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnTick(void);
	int	GetFPS(void);
	void OpenOrCloseSubMenuDialog(AUIDialog* pDlg);
	
	AUICheckBox *			m_pChktemp;
	AUIStillImageButton *	m_pBtnInventory;
	AUIStillImageButton *	m_pBtnCharacter;
	AUIStillImageButton *	m_pBtnSkill;
	AUIStillImageButton *	m_pBtnQuest;
	AUIStillImageButton *	m_pBtnConnection;
	AUIStillImageButton *	m_pBtnSystem;
	PAUISTILLIMAGEBUTTON	m_pBtnFunction;
	AUIStillImageButton *	m_pBtnModeChange;
	PAUIOBJECT			m_pFrameSystem2;
};
