// Filename	: DlgComplexTitle.h
// Creator	: Han Guanghui
// Date		: 2013/5/16

#ifndef _DLGCOMPLEXTITLE_H_ 
#define _DLGCOMPLEXTITLE_H_

#include "DlgBase.h"
#include "AUIListBox.h"
#include <vector>

struct COMPLEX_TITLE_CONFIG;
class CDlgComplexTitle : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgComplexTitle();
	virtual ~CDlgComplexTitle();

	void				OnCommandTitleList(const char* szCommand);
	void				OnEventSelectList(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:
	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();
	void				SetPropertyText(int prop);
	void				Update();

	typedef std::vector<COMPLEX_TITLE_CONFIG*> COMPLEX_CONTAINER;
	COMPLEX_CONTAINER m_ComplexTitles;

	PAUILISTBOX			m_pListComplexTitles;
	PAUILISTBOX			m_pListTitles;
	int					m_iCurrentSelect;
	int					m_iLabIndex;
	int					m_iPropIndex;
};

#endif // _DLGCOMPLEXTITLE_H_

