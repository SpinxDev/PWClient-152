// Filename	: DlgTaskConfirm.h
// Creator	: Han Guanghui
// Date		: 2012/12/26
#ifndef _DLG_TASKCONFIRM_H_
#define _DLG_TASKCONFIRM_H_

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"


class CDlgTaskConfirm : public CDlgBase  
{	
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgTaskConfirm();
	virtual ~CDlgTaskConfirm();

	void OnCommandFinishTask(const char *szCommand);
	void OnCommandMinimize(const char *szCommand);

	void UpdateTaskList();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

protected:
	virtual bool OnInitDialog();
	virtual void OnChangeLayoutEnd(bool bAllDone);
	
	int GetMaxShowTask(){return m_vecBtnTaskFnsh.size();}
private:
	abase::vector<PAUILABEL> m_vecLblTaskName;
	abase::vector<PAUISTILLIMAGEBUTTON> m_vecBtnTaskFnsh;
};

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

class CDlgTaskHintPop : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
public:
	CDlgTaskHintPop(){};
	~CDlgTaskHintPop(){}

	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_PopTaskList(const char * szCommand);

	void OnNewConfirmTask();


protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

private:
	PAUISTILLIMAGEBUTTON m_pFlashButton;
};
#endif