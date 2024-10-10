// Filename	: DlgSkillAction.h
// Creator	: zhangyitian
// Date		: 2014/6/24

#ifndef _ELEMENTCLIENT_DIALOGSKILLACTION_H_
#define _ELEMENTCLIENT_DIALOGSKILLACTION_H_

#include "DlgBase.h"

class CDlgSkillSubList;
class CDlgSkillSubTree;
class AUIRadioButton;
class AUISubDialog;
class AUILabel;
class AUICheckBox;

class CDlgSkillAction : public CDlgBase {

	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgSkillAction();
	
	void OnCommand_Evil(const char* szCommand);		// 点击“魔”
	void OnCommand_God(const char* szCommand);		// 点击“仙”
	void OnCommand_Skill(const char* szCommand);		// 点击“技能”
	void OnCommand_Action(const char* szCommand);		// 点击“动作”
	void OnCommand_Pool(const char* szCommand);			// 点击“已学技能”
	void OnCommand_CANCEL(const char* szCommand);
	void OnCommand_ShowSkillCanLearn(const char* szCommand);

	void ShowSkillTree(int skillID);					// 在右侧显示技能树
	void HideSkillTree();								// 隐藏右侧的技能树

	void TryOpenDialog(bool bAction);		// 通过菜单或快捷键打开技能对话框的操作。参数为是否打开动作界面
	void ForceShowDialog();					// 打开对话框，函数内判断打开技能或动作。该函数供外部调用

	bool IsEvil();
	int GetSelectedSkillID();
	void SetSelectedSkillID(int skillID);

	bool IsReceivedNPCGreeting();
	void SetReceivedNPCGreeting(bool bReceived);

	bool IsOnlyShowSkillCanLearn();			// 是否仅仅显示可学的技能

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnChangeLayoutBegin();
	virtual void OnChangeLayoutEnd(bool bAllDone);
	virtual void OnTick();

private:
	AUIRadioButton* m_radioBtnSkill;		// “技能”按钮
	AUIRadioButton* m_radioBtnAction;		// “动作”按钮
	AUIRadioButton* m_radioBtnPool;			// “已学技能”按钮
	AUIRadioButton* m_radioBtnEvil;			// “魔”按钮
	AUIRadioButton* m_radioBtnGod;			// “仙”按钮
	AUICheckBox*	m_pChkOnlyShowLearn;	//  
	AUILabel*		m_pLblOnlyShowLearn;	//

	// 包含子对话框控件的指针
	AUISubDialog* m_subDlgSkill;
	AUISubDialog* m_subDlgAction;
	AUISubDialog* m_subDlgPool;
	AUISubDialog* m_subDlgTreeEvil;
	AUISubDialog* m_subDlgTreeGod;
	AUISubDialog* m_subDlgTreeBase;
	AUISubDialog* m_subDlgOtherSkill;
	AUILabel* m_labelSp;

	bool m_bFirstShow;

	// 是否打开动作界面
	bool m_bOpenAction;

	int m_selectedSkill;	// 选中的技能
	bool m_bEvil;			// 是否处于魔状态，若false则为仙

	bool m_bReceivedNCPGreeting;	// 是否收到了NPC的Greeting
};

#endif