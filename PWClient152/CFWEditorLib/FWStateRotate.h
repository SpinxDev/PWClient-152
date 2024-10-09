#ifndef _FWSTATEROTATE_H_
#define _FWSTATEROTATE_H_

#include "FWState.h"

class FWGlyph;
class FWCommandRotate;

class FWStateRotate : public FWState
{
	FW_DECLARE_DYNAMIC(FWStateRotate)
public:
	FWStateRotate(FWView *pView, FWGlyph *pGlyph);
	virtual ~FWStateRotate();

	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnLButtonUp(UINT nFlags, APointI point);
	virtual void OnLButtonDown(UINT nFlags, APointI point);
	virtual void OnMouseMove(UINT nFlags, APointI point);
private:
	FWGlyph *m_pGlyph;
	bool m_bIsLButtonDown;
	double m_fOldAngle;
	double m_fFirstPointAngle;
};

#endif 