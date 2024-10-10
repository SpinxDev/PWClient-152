// Filename	: DlgSkillSubListRankItem.h
// Creator	: zhangyitian
// Date		: 2014/7/10

#ifndef _ELEMENTCLIENT_DLGSUBLISTRANKITEM_H_
#define _ELEMENTCLIENT_DLGSUBLISTRANKITEM_H_

#include "DlgBase.h"

// 技能列表中显示修真期名称的子对话框
// 这个类的唯一作用是响应鼠标滚轮事件，使得父对话框进行滚动

class CDlgSkillSubListRankItem : public CDlgBase {

	AUI_DECLARE_EVENT_MAP();

public:
	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
};

#endif