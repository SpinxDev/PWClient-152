#pragma once


#include <A3DTypes.h>
#include "AUIDialog.h"

class CCurveView;
class CCurvePoint;

class CDlgCurve : public AUIDialog
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
public:
	CDlgCurve();
	virtual ~CDlgCurve();
	void Attach(CCurvePoint *pCurvePoint, bool bFirstHandleDisabled);

	virtual bool Render();

	// event handler
	void OnLButtonDownGraph(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonUpGraph(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseMoveGraph(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnCommandIDOK(const char * szCommand);
protected:
	virtual bool OnInitDialog();
	A3DRECT GetViewRect();	
	virtual void OnShowDialog();

protected:
	CCurveView * m_pCurveView;
	CCurvePoint * m_pTempCurvePoint;
	CCurvePoint * m_pOutterCurvePoint;
};