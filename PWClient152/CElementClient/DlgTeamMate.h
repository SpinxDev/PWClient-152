// File		: DlgTeamMate.h
// Creator	: Xiao Zhou
// Date		: 2005/8/17

#pragma once

#include "DlgBase.h"

#define CDLGTEAMMATE_ST_MAX			14

class AUIImagePicture;
class AUILabel;
class AUIProgress;
class CECTeamMember;

class CDlgTeamMate : public CDlgBase  
{
	friend class CDlgTeamMain;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

	typedef abase::vector<POINT> VECTOR_POINT;

public:
	void OnCommandCANCAL(const char *szCommand);

	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	CDlgTeamMate();
	virtual ~CDlgTeamMate();

	void RefreshForceStatus(CECTeamMember* pMember);

protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnTick();
	
	AUIImagePicture *	m_pImgProf;
	AUIImagePicture *	m_pImgCombatMask;
	AUIImagePicture *	m_pImgLeader;
	AUILabel *			m_pTxtCharName;
	AUILabel *			m_pTxtLV;
	AUIProgress *		m_pPrgsHP;
	AUIProgress *		m_pPrgsMP;
	AUIImagePicture *	m_pImgSt[CDLGTEAMMATE_ST_MAX];
};
