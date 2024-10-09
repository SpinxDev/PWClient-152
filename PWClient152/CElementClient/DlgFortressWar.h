// Filename	: DlgFortressWar.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUIListbox.h"

namespace GNET
{
	class FactionFortressList_Re;
	class FactionFortressChallenge_Re;
	class FactionFortressGet_Re;
}

class CDlgFortressWar : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressWar();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Prev(const char *szCommand);
	void OnCommand_Next(const char *szCommand);
	void OnCommand_Refresh(const char *szCommand);
	void OnCommand_Search(const char *szCommand);
	void OnCommand_DeclareWar(const char *szCommand);
	
	void OnEventLButtonDown_Lst_Fortress(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnPrtcFactionFortressList_Re(const GNET::FactionFortressList_Re *);
	void OnPrtcFactionFortressChallenge_Re(const GNET::FactionFortressChallenge_Re *);
	void OnPrtcFactionFortressGet_Re(const GNET::FactionFortressGet_Re *);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void Refresh();							//	���ݵ�ǰҳ���趨�������б�
	void OnListSelChange();					//	�����б�ѡ��仯���½������
	int  GetDeclareWarTarget();				//	��ȡ��ǰѡ�еĿ���սĿ�����
	bool GetFactionName(unsigned int uID, ACString &strName, bool bRequestFromServer);
	ACString GetBuildingString(const void *pBuf, size_t sz);

	enum { FORTRESS_LIST_SIZE = 16 };		//	�����б�ÿҳ�Ĵ�С

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
		ST_INIT,			//	���ڳ�ʼ��
		ST_OPEN,			//	�ѿ���
		ST_CHALLENGE,		//	��ս�׶�
		ST_BATTLE_WAIT,		//	�ȴ�ս���׶�
		ST_BATTLE,			//	ս���׶�
	};
	
private:
	PAUIOBJECT	m_pEdit_Name;		//	ֱ�Ӳ�ѯ����Ŀ����Ϣ������ս��
	PAUILISTBOX	m_pLst_Fortress;	//	���ɻ����б�
	PAUIOBJECT	m_pBtn_DeclareWar;	//	��ս��ť

	int			m_nBegin;			//	��ʼҳ��
	FORTRESS_STATE	m_status;		//	��ǰҳ�Ļ���״̬
	bool		m_bAllInfoReady;	//	��ǰ�б����Ƿ����а�����Ϣ��������
};