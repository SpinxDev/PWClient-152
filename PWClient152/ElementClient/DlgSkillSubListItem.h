// Filename	: DlgSkillSubListItem.h
// Creator	: zhangyitian
// Date		: 2014/6/24

#ifndef _ELEMENTCLIENT_DLGSKILLSUBLISTITEM_H_
#define _ELEMENTCLIENT_DLGSKILLSUBLISTITEM_H_

#include "DlgBase.h"

class AUIImagePicture;
class AUITextArea;
class AUIStillImageButton;
class CDlgSkillSubList;
class CECSkill;

class CDlgSkillSubListItem : public CDlgBase {

	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

public:

	enum {
		NeedIconImagePictureCount = 6,	// 需求图标的数目，用enum代替const
	};

	CDlgSkillSubListItem();
	void UpdateSkill(int skillID);

	virtual bool OnInitDialog();

	void OnCommand_Upgrade(const char* szCommand);	// 点击升级按钮

	void SetHighlight(bool flag);		// 设置是否高亮

	int GetSkillID() {return m_skillID;}

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);		// 点击选中整个技能条

	void OnSkillPictureDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);		// 点击技能图标，拖动或施放

	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);			// 鼠标滚动

	void DoNoting(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {}

	virtual void OnTick(void);

	void EnableUpgrade(bool bEnable);

private:
	// 设置前置技能图标，参数分别为前置技能id、级别以及图标的位置
	void SetPreSkill(int skillID, int level, int slotIndex);

	// 设置前置物品图标，参数为物品id和图标的位置
	void SetRequiredItem(int itemId, int slotIndex);

	// 设置当前保存的是否已学、是否被动
	void SetSkillIsLearnedAndIsPassive(int id, bool learned);

	// 更新升级按钮
	void UpdateUpgradeBtn();

private:
	int m_skillID;		// 当前技能ID
	int m_curLevel;		// 当前技能级别

	AUIImagePicture* m_skillIconImgPic;		// 技能图标
	AUITextArea* m_skillNameLbl;			// 技能名称标签
	AUITextArea* m_needLbl;					// 需求文字标签
	AUIImagePicture* m_needIconImgPic[NeedIconImagePictureCount];	// 需求图标
	AUIStillImageButton* m_upgradeBtn;		// 升级按钮
	AUITextArea* m_levelLbl;				// 技能等级标签

	bool m_isHighlight;	// 是否高亮
	
	// 以下几个参数用于拖动技能(未学习和被动技能不能拖动)和施法
	CECSkill* m_pCECSkill;	// 当前的技能指针
	bool m_bSkillLearned;	// 当前技能是否已学
	bool m_bSkillPassive;	// 当前技能是否被动
};

#endif