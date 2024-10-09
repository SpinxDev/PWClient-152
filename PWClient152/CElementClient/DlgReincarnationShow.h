// Filename	: DlgReincarnationShow.h
// Creator	: Han Guanghui
// Date		: 2013/10/13

#pragma once

#include "DlgBase.h"

class CECLoginPlayer;

class CDlgFullScreenEffectShow: public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
		
public:
	enum EFFECT
	{
		REINCARNATION_EFFECT,	// 转生效果
	};

	CDlgFullScreenEffectShow();
	
	void SetEffectToShow(EFFECT nEffect) { m_nEffect = nEffect; }
	
protected:
	
	CECLoginPlayer*	m_pPlayer;
	EFFECT			m_nEffect;

protected:
	virtual void OnShowDialog();
	virtual bool Release();
	virtual void OnHideDialog();
	virtual void OnTick();

	void UpdateRenderCallback();
	void ResizeWindow(A3DRECT *prc);
};