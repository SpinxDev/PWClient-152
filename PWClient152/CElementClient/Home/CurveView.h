/********************************************************************
	created:	2005/12/6   11:40
	filename: 	CurveView.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include <A3DTypes.h>

class CCurvePoint;
class FWFlatCollector;

class CCurveView
{
public:
	CCurveView(A3DRECT rect);
	virtual ~CCurveView();

	void SetRect(A3DRECT rect);
	void Attach(CCurvePoint * pCurvePoint, bool bFirstHandleDisabled);
	
	void Render();

	void BeginDrag(int x, int y);
	void OnDrag(int x, int y);
	void EndDrag(int x, int y);
	bool HitTest(int x, int y);

	int GetHandleCount();
	const float * GetPoints();
protected:
	APointI Logic2Screen(float x, float y);
	APointF Screen2Logic(int x, int y);

	APointF GetLogicHandlePos(int index);

	A3DRECT GetHandleRect(int index);
	A3DRECT GetTrueRect();
protected:
	CCurvePoint * m_pCurvePoint;
	A3DRECT m_Rect;
	FWFlatCollector * m_pFlatCollector;
	bool m_bOnDrag;
	int m_nClickHandle;
	bool m_bFirstHandleDisabled;
};