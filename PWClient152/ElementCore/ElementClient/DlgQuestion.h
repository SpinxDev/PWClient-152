// File		: DlgQuestion.h
// Creator	: Xiao Zhou
// Date		: 2007/7/12

#pragma once

#include "DlgBase.h"

class CDlgQuestion : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
		
	typedef abase::vector<POINT> VECTOR_POINT;

public:
	CDlgQuestion();
	virtual ~CDlgQuestion();

	void OnCommandMinimize(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void SetQuestion(void *pData);

protected:
	virtual void OnTick();
	virtual void OnShowDialog();
	virtual bool Render();

	int	type;
	int	seq;
	int	reserved;
	int	endtime;
	BYTE* m_pAnswer[6];
	abase::vector<VECTOR_POINT> m_vecPath;

};
