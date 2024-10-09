// Filename	: DlgSkillSubPool.h
// Creator	: zhangyitian
// Date		: 2014/08/24

#ifndef _ELEMENTSKILL_DLGSKILLSUBPOOL_H_
#define _ELEMENTSKILL_DLGSKILLSUBPOOL_H_

#include "DlgBase.h"
#include <set>

class AUIImagePicture;
class CECSkill;

class CDlgSkillSubPool : public CDlgBase {

	AUI_DECLARE_EVENT_MAP();

public:
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	static void TickSkillIcon(AUIImagePicture* pImg, CECSkill* pSkill);
protected:
	virtual void OnShowDialog();
	virtual void OnTick();
	void UpdateView();
	void SetImg(CECSkill* pSkill, AUIImagePicture* pImgPic);
	bool CheckSkillChanged();	// 检查所有已学技能是否改变
protected:
	std::set<int> m_skills;
};


#endif