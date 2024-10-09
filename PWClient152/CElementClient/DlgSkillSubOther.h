// Filename	: DlgSkillSubOther.h
// Creator	: zhangyitian
// Date		: 2014/7/4

#ifndef _ELEMENTCLIENT_DLGSKILLSUBOTER_H_
#define _ELEMENTCLIENT_DLGSKILLSUBOTER_H_

#include "DlgBase.h"

class AUIImagePicture;
class AUILabel;
class CECSkill;

// 技能面板右侧部分，显示“其它技能”，包括生产技能、公共技能、物品技能和连续技能
class CDlgSkillSubOther : public CDlgBase {

	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgSkillSubOther();

	// 编辑、删除和新建，抄自DlgSkill
	void OnCommandEdit(const char * szCommand);
	void OnCommandDelete(const char * szCommand);
	void OnCommandNew(const char * szCommand);

	// 更新组合技能图标的显示，抄自DlgSkill
	void UpdateComboSkill();		

	// 更新生产技能的显示
	void UpdateProduceSkill();

	// 更新固有技能的显示
	void UpdateFixedSkill();
	
	// 更新物品技能的显示
	void UpdateItemSkill();

	// 鼠标点击组合技能图标，抄自DlgSkill
	void OnEventLButtonDownCombo(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	// 鼠标点击固有技能图标
	void OnEventLButtonDownFixed(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	// 鼠标点击物品技能图标
	void OnEventLButtonDownItem(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	// 选中组合技能，抄自DlgSkill
	void SelectComboSkill(int n);

	// 给一个IMGPICTURE设置一个技能的图标，抄自DlgSkill.
	void SetImage(AUIImagePicture *pImage, CECSkill *pSkill);

	// 更新一个技能图标的CD动画
	void UpdateImagePictureCD(AUIImagePicture* pImgPic, CECSkill* pSkill);

	virtual bool Tick(void);
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

private:
	int	m_nComboSelect;		// 当前选中的连锁技能

	abase::vector<unsigned int> m_fixedSkills;			// 固有技能列表
	abase::vector<AUIImagePicture*> m_fixedImgPics;		// 固有技能的图标
	abase::vector<AUILabel*> m_fixedTxts;				// 固有技能的文字

	abase::vector<unsigned int> m_produceSkills;		// 生产技能列表	

	abase::vector<unsigned int> m_itemSkills;			// 物品技能列表
	abase::vector<AUIImagePicture*> m_itemImgPics;		// 物品技能的图标
};

#endif