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
		NeedIconImagePictureCount = 6,	// ����ͼ�����Ŀ����enum����const
	};

	CDlgSkillSubListItem();
	void UpdateSkill(int skillID);

	virtual bool OnInitDialog();

	void OnCommand_Upgrade(const char* szCommand);	// ���������ť

	void SetHighlight(bool flag);		// �����Ƿ����

	int GetSkillID() {return m_skillID;}

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);		// ���ѡ������������

	void OnSkillPictureDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);		// �������ͼ�꣬�϶���ʩ��

	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);			// ������

	void DoNoting(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {}

	virtual void OnTick(void);

	void EnableUpgrade(bool bEnable);

private:
	// ����ǰ�ü���ͼ�꣬�����ֱ�Ϊǰ�ü���id�������Լ�ͼ���λ��
	void SetPreSkill(int skillID, int level, int slotIndex);

	// ����ǰ����Ʒͼ�꣬����Ϊ��Ʒid��ͼ���λ��
	void SetRequiredItem(int itemId, int slotIndex);

	// ���õ�ǰ������Ƿ���ѧ���Ƿ񱻶�
	void SetSkillIsLearnedAndIsPassive(int id, bool learned);

	// ����������ť
	void UpdateUpgradeBtn();

private:
	int m_skillID;		// ��ǰ����ID
	int m_curLevel;		// ��ǰ���ܼ���

	AUIImagePicture* m_skillIconImgPic;		// ����ͼ��
	AUITextArea* m_skillNameLbl;			// �������Ʊ�ǩ
	AUITextArea* m_needLbl;					// �������ֱ�ǩ
	AUIImagePicture* m_needIconImgPic[NeedIconImagePictureCount];	// ����ͼ��
	AUIStillImageButton* m_upgradeBtn;		// ������ť
	AUITextArea* m_levelLbl;				// ���ܵȼ���ǩ

	bool m_isHighlight;	// �Ƿ����
	
	// ���¼������������϶�����(δѧϰ�ͱ������ܲ����϶�)��ʩ��
	CECSkill* m_pCECSkill;	// ��ǰ�ļ���ָ��
	bool m_bSkillLearned;	// ��ǰ�����Ƿ���ѧ
	bool m_bSkillPassive;	// ��ǰ�����Ƿ񱻶�
};

#endif