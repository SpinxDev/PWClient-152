// Filename	: DlgEarthBagRank.h
// Creator	: Xu Wenbin
// Date		: 2011/5/12

#pragma once

#include "DlgBase.h"
#include <AUIListBox.h>
#include <AUIStillImageButton.h>
#include "EC_RoleTypes.h"

namespace	S2C
{
	struct cmd_dps_dph_rank;
}

class CDlgEarthBagRank : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgEarthBagRank();

	void UpdateRank(const S2C::cmd_dps_dph_rank *pCmd);
	
	void OnCommand_Btn_DPS(const char *szCommand);
	void OnCommand_Btn_DPH(const char *szCommand);
	void OnCommand_SortBy_Rank(const char *szCommand);
	void OnCommand_SortBy_Level(const char *szCommand);
    void OnCommand_RankList(const char* szCommand);

	void OnCommand_CANCEL(const char *szCommand);

	void OnEventRButtonUp_Lst_Rank(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void UpdateUI();
	bool AppendLine(PAUILISTBOX pLstBox, int rank, int id, int level, int value);
	void UpdateLine(PAUILISTBOX pLstBox, int line);

	void UpdateRefreshTime();

private:

	//	�б�ÿ�еĺ���
	enum LST_COLUMN_TYPE
	{
		LST_COLUMN_RANK,
		LST_COLUMN_NAME,
		LST_COLUMN_LEVEL,
		LST_COLUMN_VALUE,
	};

	//	�б�ÿ�������Զ�������
	enum LST_DATA_INDEX
	{
		LST_INDEX_PLAYER,				//	id
		LST_INDEX_NEED_PLAYER,	//	�Ƿ���Ҫ��ѯ�������
	};

	//	����ؼ�
	PAUILISTBOX							m_pLst_Rank;
	PAUILISTBOX							m_pLst_Self;

	PAUISTILLIMAGEBUTTON	m_pBtn_DPS;
	PAUISTILLIMAGEBUTTON	m_pBtn_DPH;

	//	���а���Ϣ
	struct RankInfo 
	{
		int	id;
		int	rank;
		int	level;
		int	value;
	};
	typedef abase::vector<RankInfo>	RankList;

	enum	RANKLIST_TYPE
	{
		RANKLIST_DPS,
		RANKLIST_DPH,
		RANKLIST_NUM,
	};
	RankList	    m_rankList[NUM_PROFESSION+1][RANKLIST_NUM];	//	���е����а�
	RANKLIST_TYPE	m_curList;								    //	��ǰ��ʾ�����а�
    int             m_iCurProfession;                           //  ��ǰ��ʾ��ְҵ

    PAUIOBJECT		m_pTxt_Time;
    DWORD			m_dwMinRefreshMoment;			                        //	���а���¸�����ʱ�������ڷ�ֹ���а����CDδ�ӷ�������ʱ���µ���Ƶ����������������Э��
	DWORD			m_dwNextRefreshMoment[NUM_PROFESSION+1][RANKLIST_NUM];	//	�´θ���ʱ�̣�ÿ�θ��º��ɷ���������

	enum SORT_METHOD
	{
		SORT_BY_RANK,
		SORT_BY_LEVEL,
	};
	SORT_METHOD		m_curSort;								//	��ǰ���򷽷�
};
