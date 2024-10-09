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

	void ResetGodEvil();	// 设置仙魔状态

	virtual void OnModelChange(const CECHostSkillModel *p, const CECObservableChange *q);

	void SelectSkill(int skillID);

	void ScrollToShowSelectedSkill();	// 滚动界面
	
	bool OnChangeLayoutBegin();			// 准备更改界面

	virtual void OnChangeLayoutEnd(bool bAllDone); 

	void ResetDialog();					// 重新对整个对话框进行布局

	void EnableUpgrade(bool bEnable);

	int GetSelectedSkillID();
	void SetSelectedSkillID(int skillID);
	bool IsEvil();

	bool IsSkillOrConvertSkillExist(int skillID);	// 技能本身或对应技能是否存在列表中

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

private:
	void InitRankDlgs();										// 初始化所有修真期对话框，一次性分配好内存
	void UpdateOneSubDlg(int skillID);							// 修改某一个技能条的状态
	void AddRankSubDig(int rankIndex);							// 增加一条修真级别对话框
	void AddSkillSubDlg(int skillID);							// 增加一条技能对话框，调用该函数后还需调用UpdateOneSubDlg
	void FitSize();

	void CreateOneRankDlg(const CECTaoistRank* taoistRank);		// 新建一个修真级别对应的对话框
	void AddDlgsOfOneRank(const CECTaoistRank* taoistRank);		// 将某一个修真级别对应的所有技能条加入对话框

	void ShowLastSelectedSkill();


private:
	abase::hash_map<int, AUISubDialog*> m_rankSubDialogs;		// 所有修真期ID对应的对话框，该数据结构不会变化
	abase::vector<AUISubDialog*> m_skillSubDialogs;				// 所有可能用到的技能对话框，该数据结构不会变化

	abase::hash_map<int, AUISubDialog*> m_skillSubDialogsMap;	// 记录每个技能ID对应的对话框，该数据结构会变化

	int m_skillSubCount;	// 当前显示的技能个数
	int m_curBottom;		// 当前底部的坐标
	int m_skillHeight;		// 技能对话框的高度
	int m_rankHeight;		// 修真对话框的高度

	AString m_rankDlgFile;		
	AString m_skillDlgFile;

	AString m_rankDlgName;
	AString m_skillDlgName;

	AString m_rankObjName;
	AString m_skillObjName;

	AUISubDialog* m_pSubSkill;
	AUISubDialog* m_pSubRank;

	int m_originWidth;		// 初始宽度
	int m_originHeight;		// 初始高度
	int m_originBottom;		// 初始底部坐标

	bool m_bAllocRankDlgs;			// 是否分配了修真子对话框
};

#endif