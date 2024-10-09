// Filename	: DlgSkillSubOther.h
// Creator	: zhangyitian
// Date		: 2014/7/4

#ifndef _ELEMENTCLIENT_DLGSKILLSUBOTER_H_
#define _ELEMENTCLIENT_DLGSKILLSUBOTER_H_

#include "DlgBase.h"

class AUIImagePicture;
class AUILabel;
class CECSkill;

// ��������Ҳಿ�֣���ʾ���������ܡ��������������ܡ��������ܡ���Ʒ���ܺ���������
class CDlgSkillSubOther : public CDlgBase {

	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgSkillSubOther();

	// �༭��ɾ�����½�������DlgSkill
	void OnCommandEdit(const char * szCommand);
	void OnCommandDelete(const char * szCommand);
	void OnCommandNew(const char * szCommand);

	// ������ϼ���ͼ�����ʾ������DlgSkill
	void UpdateComboSkill();		

	// �����������ܵ���ʾ
	void UpdateProduceSkill();

	// ���¹��м��ܵ���ʾ
	void UpdateFixedSkill();
	
	// ������Ʒ���ܵ���ʾ
	void UpdateItemSkill();

	// �������ϼ���ͼ�꣬����DlgSkill
	void OnEventLButtonDownCombo(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	// ��������м���ͼ��
	void OnEventLButtonDownFixed(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	// �������Ʒ����ͼ��
	void OnEventLButtonDownItem(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	// ѡ����ϼ��ܣ�����DlgSkill
	void SelectComboSkill(int n);

	// ��һ��IMGPICTURE����һ�����ܵ�ͼ�꣬����DlgSkill.
	void SetImage(AUIImagePicture *pImage, CECSkill *pSkill);

	// ����һ������ͼ���CD����
	void UpdateImagePictureCD(AUIImagePicture* pImgPic, CECSkill* pSkill);

	virtual bool Tick(void);
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

private:
	int	m_nComboSelect;		// ��ǰѡ�е���������

	abase::vector<unsigned int> m_fixedSkills;			// ���м����б�
	abase::vector<AUIImagePicture*> m_fixedImgPics;		// ���м��ܵ�ͼ��
	abase::vector<AUILabel*> m_fixedTxts;				// ���м��ܵ�����

	abase::vector<unsigned int> m_produceSkills;		// ���������б�	

	abase::vector<unsigned int> m_itemSkills;			// ��Ʒ�����б�
	abase::vector<AUIImagePicture*> m_itemImgPics;		// ��Ʒ���ܵ�ͼ��
};

#endif