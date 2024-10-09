// Filename	: DlgSkillSubList.h
// Creator	: zhangyitian
// Date		: 2014/6/23

#ifndef _ELEMENTCLIENT_DLGSKILLSUBLIST_H_
#define _ELEMENTCLIENT_DLGSKILLSUBLIST_H_

#include "DlgBase.h"
#include "hashmap.h"
#include "EC_Observer.h"

class AUISubDialog;
class CECHostSkillModel;
class CECTaoistRank;


class CDlgSkillSubList : public CDlgBase, public CECObserver<CECHostSkillModel> {

	//AUI_DECLARE_EVENT_MAP();
	//AUI_DECLARE_COMMAND_MAP();

public:

	CDlgSkillSubList();
	~CDlgSkillSubList();

	virtual bool Release();

	void ResetGodEvil();	// ������ħ״̬

	virtual void OnModelChange(const CECHostSkillModel *p, const CECObservableChange *q);

	void SelectSkill(int skillID);

	void ScrollToShowSelectedSkill();	// ��������
	
	bool OnChangeLayoutBegin();			// ׼�����Ľ���

	virtual void OnChangeLayoutEnd(bool bAllDone); 

	void ResetDialog();					// ���¶������Ի�����в���

	void EnableUpgrade(bool bEnable);

	int GetSelectedSkillID();
	void SetSelectedSkillID(int skillID);
	bool IsEvil();

	bool IsSkillOrConvertSkillExist(int skillID);	// ���ܱ�����Ӧ�����Ƿ�����б���

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

private:
	void InitRankDlgs();										// ��ʼ�����������ڶԻ���һ���Է�����ڴ�
	void UpdateOneSubDlg(int skillID);							// �޸�ĳһ����������״̬
	void AddRankSubDig(int rankIndex);							// ����һ�����漶��Ի���
	void AddSkillSubDlg(int skillID);							// ����һ�����ܶԻ��򣬵��øú����������UpdateOneSubDlg
	void FitSize();

	void CreateOneRankDlg(const CECTaoistRank* taoistRank);		// �½�һ�����漶���Ӧ�ĶԻ���
	void AddDlgsOfOneRank(const CECTaoistRank* taoistRank);		// ��ĳһ�����漶���Ӧ�����м���������Ի���

	void ShowLastSelectedSkill();


private:
	abase::hash_map<int, AUISubDialog*> m_rankSubDialogs;		// ����������ID��Ӧ�ĶԻ��򣬸����ݽṹ����仯
	abase::vector<AUISubDialog*> m_skillSubDialogs;				// ���п����õ��ļ��ܶԻ��򣬸����ݽṹ����仯

	abase::hash_map<int, AUISubDialog*> m_skillSubDialogsMap;	// ��¼ÿ������ID��Ӧ�ĶԻ��򣬸����ݽṹ��仯

	int m_skillSubCount;	// ��ǰ��ʾ�ļ��ܸ���
	int m_curBottom;		// ��ǰ�ײ�������
	int m_skillHeight;		// ���ܶԻ���ĸ߶�
	int m_rankHeight;		// ����Ի���ĸ߶�

	AString m_rankDlgFile;		
	AString m_skillDlgFile;

	AString m_rankDlgName;
	AString m_skillDlgName;

	AString m_rankObjName;
	AString m_skillObjName;

	AUISubDialog* m_pSubSkill;
	AUISubDialog* m_pSubRank;

	int m_originWidth;		// ��ʼ���
	int m_originHeight;		// ��ʼ�߶�
	int m_originBottom;		// ��ʼ�ײ�����

	bool m_bAllocRankDlgs;			// �Ƿ�����������ӶԻ���
};

#endif