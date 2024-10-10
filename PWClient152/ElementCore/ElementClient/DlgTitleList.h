// Filename	: DlgTitleList.h
// Creator	: Han Guanghui
// Date		: 2013/5/16

#ifndef _DLGTITLELIST_H_ 
#define _DLGTITLELIST_H_

#include "DlgBase.h"
#include <vector>
#include "AUILabel.h"

class CDlgTitleList : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

	enum
	{
		PROP_NUM = 6,
	};
public:
	CDlgTitleList();
	virtual ~CDlgTitleList();

	void				OnCommandSearch(const char* szCommand);
	void				OnCommandPagePrevious(const char* szCommand);
	void				OnCommandPageNext(const char* szCommand);
	void				OnCommandCheckTitle(const char* szCommand);
	void				OnCommandCANCEL(const char * szCommand);
	void				OnCommand_ComplexTitle(const char * szCommand);

	void				OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	static void			QueryTitleList();
	void				Update();

protected:
	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();
	void				GetTitleDesc(unsigned short title_id);

	int					m_iTitleNum;
	int					m_iFirstIndex;
	ACString			m_strDesc;


	int m_totalPropValue[PROP_NUM];// phyatk,magatk,phydef,magdef,attack,armor

	PAUILABEL	m_pLbl_Prop[PROP_NUM]; // phyatk,magatk,phydef,magdef,attack,armor
};

#endif // _DLGTITLELIST_H_

