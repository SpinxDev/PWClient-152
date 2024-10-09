#ifndef _FWSTATECREATEBEZIER_H_
#define _FWSTATECREATEBEZIER_H_

#include "FWState.h"

class FWGlyphBezier;

class FWStateCreateBezier : public FWState  
{
	FW_DECLARE_DYNAMIC(FWStateCreateBezier)

private:
	FWGlyphBezier * m_pBezier;
	bool m_bLButtonDowned;
	bool m_bGraphInUse;
	// position of the current mouse pointer
	APointI m_curentPoint;
public:
	FWStateCreateBezier(FWView *pView, FWGlyphBezier *pGlyph);
	virtual ~FWStateCreateBezier();
	
	virtual void OnLButtonUp(UINT nFlags, APointI point);
	virtual void OnLButtonDown(UINT nFlags, APointI point);
	virtual void OnLClick(UINT nFlags, APointI point);
	virtual void OnMouseMove(UINT nFlags, APointI point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDraw();
	virtual void OnRClick(UINT nFlags, APointI point);
};

#endif 