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
	
	void OnCommand_Evil(const char* szCommand);		// �����ħ��
	void OnCommand_God(const char* szCommand);		// ������ɡ�
	void OnCommand_Skill(const char* szCommand);		// ��������ܡ�
	void OnCommand_Action(const char* szCommand);		// �����������
	void OnCommand_Pool(const char* szCommand);			// �������ѧ���ܡ�
	void OnCommand_CANCEL(const char* szCommand);
	void OnCommand_ShowSkillCanLearn(const char* szCommand);

	void ShowSkillTree(int skillID);					// ���Ҳ���ʾ������
	void HideSkillTree();								// �����Ҳ�ļ�����

	void TryOpenDialog(bool bAction);		// ͨ���˵����ݼ��򿪼��ܶԻ���Ĳ���������Ϊ�Ƿ�򿪶�������
	void ForceShowDialog();					// �򿪶Ի��򣬺������жϴ򿪼��ܻ������ú������ⲿ����

	bool IsEvil();
	int GetSelectedSkillID();
	void SetSelectedSkillID(int skillID);

	bool IsReceivedNPCGreeting();
	void SetReceivedNPCGreeting(bool bReceived);

	bool IsOnlyShowSkillCanLearn();			// �Ƿ������ʾ��ѧ�ļ���

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnChangeLayoutBegin();
	virtual void OnChangeLayoutEnd(bool bAllDone);
	virtual void OnTick();

private:
	AUIRadioButton* m_radioBtnSkill;		// �����ܡ���ť
	AUIRadioButton* m_radioBtnAction;		// ����������ť
	AUIRadioButton* m_radioBtnPool;			// ����ѧ���ܡ���ť
	AUIRadioButton* m_radioBtnEvil;			// ��ħ����ť
	AUIRadioButton* m_radioBtnGod;			// ���ɡ���ť
	AUICheckBox*	m_pChkOnlyShowLearn;	//  
	AUILabel*		m_pLblOnlyShowLearn;	//

	// �����ӶԻ���ؼ���ָ��
	AUISubDialog* m_subDlgSkill;
	AUISubDialog* m_subDlgAction;
	AUISubDialog* m_subDlgPool;
	AUISubDialog* m_subDlgTreeEvil;
	AUISubDialog* m_subDlgTreeGod;
	AUISubDialog* m_subDlgTreeBase;
	AUISubDialog* m_subDlgOtherSkill;
	AUILabel* m_labelSp;

	bool m_bFirstShow;

	// �Ƿ�򿪶�������
	bool m_bOpenAction;

	int m_selectedSkill;	// ѡ�еļ���
	bool m_bEvil;			// �Ƿ���ħ״̬����false��Ϊ��

	bool m_bReceivedNCPGreeting;	// �Ƿ��յ���NPC��Greeting
};

#endif