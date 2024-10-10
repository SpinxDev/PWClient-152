// Filename	: DlgFortressWarList.h
// Creator	: Xu Wenbin
// Date		: 2010/10/12

#pragma once

#include "DlgBase.h"
#include "AUIListbox.h"

namespace GNET
{
	class FactionFortressBattleList_Re;
}

class CDlgFortressWarList : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressWarList();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Refresh(const char *szCommand);
	void OnCommand_Enter(const char *szCommand);
	void OnCommand_Lst_GvG(const char *szCommand);

	void OnPrtcFactionFortressBattleList_Re(const GNET::FactionFortressBattleList_Re *);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void OnListSelChange();					//	��ս���б�ѡ��仯���½������

	int  GetFortressToEnter();				//	��ȡ��ǰѡ�еĿɽ���Ŀ�����
	bool GetFactionName(unsigned int uID, ACString &strName, bool bRequestFromServer);

	enum
	{
		FORTRESS_LIST_OWNER_INDEX = 0,		//	����ӵ�а���id�±�
		FORTRESS_LIST_OFFENSE_INDEX = 1,	//	���ؽ���������id�±�
		FORTRESS_LIST_NAME1_INDEX = 2,		//	�����ط��Ƿ���ȷ��ʾ��ʶ�±�
		FORTRESS_LIST_NAME2_INDEX = 3,		//	���ع����Ƿ���ȷ��ʾ��ʶ�±�
	};	
	
	//	����״̬
	enum FORTRESS_STATE
	{
		ST_INIT,					//	���ڳ�ʼ��
		ST_OPEN,				//	�ѿ���
		ST_CHALLENGE,		//	��ս�׶�
		ST_BATTLE_WAIT,		//	�ȴ�ս���׶�
		ST_BATTLE,				//	ս���׶�
	};
	
private:
	PAUILISTBOX	m_pLst_GvG;			//	���ɶ�ս�б�
	PAUIOBJECT	m_pBtn_Enter;		//	������ذ�ť
	FORTRESS_STATE	m_status;		//	��ǰҳ�Ļ���״̬
	bool		m_bAllInfoReady;	//	��ǰ�б����Ƿ����а�����Ϣ��������
};