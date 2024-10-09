// Filename	: DlgForce.h
// Creator	: Feng Ning
// Date		: 2011/10/08

#pragma once

#include "DlgBase.h"

struct FORCE_CONFIG;
class CECDiagramRender;
class AUIImagePicture;
class CECDiagramRender;

class CDlgForce : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgForce();
	~CDlgForce();

	// update UI by specific force data
	bool UpdateAll(int forceID);

	virtual bool Render(void);
	virtual void OnTick();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	// update UI by global force
	void UpdateGlobalForce();

	// update UI by player force
	void UpdatePlayerForce();

	// update the suggest item
	void UpdateSuggestItem(int force_id);

private:
	
	// to avoid duplicated update
	const FORCE_CONFIG* m_pConfig;

	CECDiagramRender* m_pDiagramRender;
	AUIImagePicture* m_pCanvas;
	abase::vector<AUIImagePicture*> m_Icons;
};
