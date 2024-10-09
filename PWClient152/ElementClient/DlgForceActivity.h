// Filename	: DlgForceActivity.h
// Creator	: Feng Ning
// Date		: 2011/10/08

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"

struct FORCE_GLOBAL_SUMMARY;
struct FORCE_CONFIG;

class CECDiagramRender
{
public:
	CECDiagramRender();
	~CECDiagramRender();

	bool Init();

	void Render(int count, const int* pRatio, const FORCE_CONFIG** ppConfig, PAUIIMAGEPICTURE pCanvas);
	
	void Render(const FORCE_GLOBAL_SUMMARY* pSummary, PAUIIMAGEPICTURE pCanvas);

	void ClearSummary();
	void RefreshSummary(int type, PAUIIMAGEPICTURE pCanvas);
	
	FORCE_GLOBAL_SUMMARY* GetSummary() const {return m_pSummary;}
	int GetRatioType() const { return m_RatioType; }

protected:
	A3DVECTOR3 *m_pVerts;
	WORD *m_pIndices;

	// the type of current diagram
	FORCE_GLOBAL_SUMMARY* m_pSummary;
	int m_RatioType;
};

class CDlgForceActivity : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgForceActivity();
	virtual ~CDlgForceActivity();
	
	virtual bool Render(void);

	void OnEvent_Canvas(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
private:
	PAUIIMAGEPICTURE m_pCanvas;
	CECDiagramRender* m_pDiagramRender;
};
