// Filename	: DlgSkillSubListRankItem.h
// Creator	: zhangyitian
// Date		: 2014/7/10

#ifndef _ELEMENTCLIENT_DLGSUBLISTRANKITEM_H_
#define _ELEMENTCLIENT_DLGSUBLISTRANKITEM_H_

#include "DlgBase.h"

// �����б�����ʾ���������Ƶ��ӶԻ���
// ������Ψһ��������Ӧ�������¼���ʹ�ø��Ի�����й���

class CDlgSkillSubListRankItem : public CDlgBase {

	AUI_DECLARE_EVENT_MAP();

public:
	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
};

#endif