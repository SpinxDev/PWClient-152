#ifndef _FWSTATECREATERECT_H_
#define _FWSTATECREATERECT_H_

#include "FWState.h"
#include <ARect.h>
#include <APoint.h>

class FWGlyph;
class CCreateCommand;
typedef unsigned int UINT;

class FWStateCreateRect : public FWState  
{
	FW_DECLARE_DYNAMIC(FWStateCreateRect);
private:
	bool m_bGlyphInUse;
	FWGlyph* m_pGlyph;
	bool m_bMouseDown;

	ARectI m_rect;
	APointI m_upleftPoint;

	// make rect width and height to be same
	ARectI RegularizeRect(ARectI *pRect);
public:
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDraw();
	virtual void OnMouseMove(UINT nFlags, APointI point);
	virtual void OnLButtonUp(UINT nFlags, APointI point);
	virtual void OnLButtonDown(UINT Flags, APointI point);

	FWStateCreateRect(FWView *pView, FWGlyph* pGlyph);
	virtual ~FWStateCreateRect();
};

#endif 