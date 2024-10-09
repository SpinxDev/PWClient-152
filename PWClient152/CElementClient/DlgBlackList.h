// Filename	: DlgBlackList.h
// Creator	: zhangyitian
// Date		: 2014/07/21
// 将原先的CDlgSettingBlackList略作修改，删除了一些内容

#include "DlgBase.h"
#include "EC_Configs.h"
#include <vector.h>
#include <AString.h>

class AUIListBox;
class AUIEditBox;
class AUICheckBox;
class AUIStillImageButton;

#ifndef _ELEMENTCLIENT_CDLGBLACKLIST_H_
#define _ELEMENTCLIENT_CDLGBLACKLIST_H_

class CDlgBlackList : public CDlgBase  
{
	friend class CDlgFriendRequest;
	
	AUI_DECLARE_COMMAND_MAP()
		
protected:
	typedef struct _BLACK_CACHE
	{
		DWORD dwTime;
		ACString strName;
	} BLACK_CACHE, * P_BLACK_CACHE;
	
	abase::vector<BLACK_CACHE> m_vecBlackCache;
	
	// DDX member
	AUIListBox * m_pLbxBlackList;
	PAUIOBJECT m_pEbxName;
	PAUIOBJECT m_pLevelBlock;
	AUIStillImageButton* m_pBtnApply;
	ACString m_strName;
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	void DoDataExchange(bool bSave);
	
	void SaveLevelBlock();

public:
	void OnCommandChooseName(const char * szCommand);
	void OnCommandRemove(const char * szCommand);
	void OnCommandAdd(const char * szCommand);
	void OnCommandApply(const char * szCommand);
	
	virtual void OnTick(void);
	virtual void UpdateView();
	
	CDlgBlackList();
	virtual ~CDlgBlackList();
};

#endif
