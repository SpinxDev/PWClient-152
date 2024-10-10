// Filename	: DlgTitleChallenge.h
// Creator	: Han Guanghui
// Date		: 2013/5/16

#ifndef _DLGTITLECHALLENGE_H_ 
#define _DLGTITLECHALLENGE_H_

#include "DlgBase.h"
#include <vector>

class CDlgTitleChallenge : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgTitleChallenge();
	virtual ~CDlgTitleChallenge();

	void				OnCommandChallenge(const char* szCommand);
	void				OnCommandPagePrevious(const char* szCommand);
	void				OnCommandPageNext(const char* szCommand);
	
	void				AddChallenge(unsigned short id);
	void				RemoveChallenge(unsigned short id);
	void				OnReceiveChallenge(unsigned short id);
	void				ChangeWorldInstance(int idInstance);
	unsigned int		GetChallengeCount() const { return m_Challenges.size();}

	void				OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:

	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();
	virtual void		OnChangeLayoutEnd(bool bAllDone);
	void				Update();
	bool				IsConfirmingChallenge(unsigned short id);
	
	
	typedef std::vector<unsigned short> TASK_ID;
	TASK_ID				m_Challenges;
	TASK_ID				m_ConfirmChallenges;
	int					m_iFirstTaskIndex;
	bool				m_bAddInfo;
};

#endif // _DLGTITLECHALLENGE_H_

