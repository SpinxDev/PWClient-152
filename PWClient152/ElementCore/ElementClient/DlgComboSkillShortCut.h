// Filename	: DlgComboSkillShortCut.h
// Creator	: zhangyitian
// Date		: 2014/8/12

#ifndef _ELEMENTCLIENT_DLGCOMBOSKILLSHORTCUT_H_
#define _ELEMENTCLIENT_DLGCOMBOSKILLSHORTCUT_H_

#include "DlgBase.h"

#include "EC_ComboSkillState.h"
#include "EC_Observer.h"

class CECSkill;
class AUIImagePicture;
class AUIProgress;
class AUILabel;

class CECComboSkillShortCut {
	
	friend class CDlgComboShortCutBar;
public:
	CECComboSkillShortCut();
	void TimeOut();
	void OnTick();
	void CastSkill();
	void OnInitDialog(CDlgComboShortCutBar* pDlg, int index);
	bool IsShow();
	void Show(bool bShow, DWORD dwStartTime = 0, DWORD dwStayTime = 0, unsigned int skillID = 0);
private:
	void SetStartTime(DWORD dwStartTime);
	void SetStayTime(DWORD dwStayTime);
	void SetSkill(unsigned int skillID);
private:
	AUIImagePicture* m_pImgSkill;
	AUIProgress* m_pProgTime;
	AUIImagePicture* m_pImgCover;
	AUILabel* m_pLblKey;
	DWORD m_dwStartTime;
	DWORD m_dwStayTime;
	CECSkill* m_pSkill;
	bool m_bTimeOut;
	AString m_gfxName;
};

///////////////////////////////////////////////////////////////////////////////////////////

class CDlgComboShortCutBar : public CDlgBase, public CECObserver<CECComboSkillState> {
	friend class CECComboSkillShortCut;

	AUI_DECLARE_EVENT_MAP()

public:
	enum {
		COMBO_SKILL_SHORTCUT_COUNT = 3,
	};
	CDlgComboShortCutBar();
	~CDlgComboShortCutBar();
	virtual void OnModelChange(const CECComboSkillState *p, const CECObservableChange *q);

	virtual void OnTick();

	void CastSkill(int iKey);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	bool IsUsingQuickKey();			// 当前是否占用了快捷键

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
private:
	CECComboSkillShortCut m_comboShortCut[COMBO_SKILL_SHORTCUT_COUNT];
	DWORD m_dwHideTime;
};

///////////////////////////////////////////////////////////////////////////////////////////




#endif