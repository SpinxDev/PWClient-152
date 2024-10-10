// Filename	: DlgAutoTask.h
// Creator	: Feng Ning
// Date		: 2010/09/21

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"

class CDlgAutoTask : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgAutoTask();
	virtual ~CDlgAutoTask();

	void OnCommand_CANCEL(const char * szCommand);	
	void OnEventLButtonDown_Img_Reward(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Img_Reward(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void UpdateAutoDelTask(unsigned long taskID, unsigned long remainTime);
	void ConfirmAccept(unsigned long task_id);

	void UpdateShow();

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release();

	virtual void OnChangeLayoutEnd(bool bAllDone);

	struct AUTOTASK_DISPLAY_PARAM
	{
		const char*		file_icon;			//	图标路径名(type=path)
		const char*		file_gfx;			//	完成特效路径名(type=path)
		unsigned long	color;				//	文本颜色(type=color)
	};
	static bool GetDisplayParam(int taskid, AUTOTASK_DISPLAY_PARAM& param);

	POINT GetMousePosition(LPARAM lParam)const;
	void  ResetLButtonDownPosition();

private:
	unsigned long m_TaskID;
	unsigned long m_RemainTime;
	unsigned long m_LastTime;

	PAUIIMAGEPICTURE	m_pImgAutoTask;
	POINT				m_ptLButtonDown;

	static AString m_strIcon;
	static AString m_strGfx;
};
