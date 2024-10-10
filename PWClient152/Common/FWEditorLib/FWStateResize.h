#ifndef _FWSTATERESIZE_H_
#define _FWSTATERESIZE_H_

#include "FWState.h"
#include <ARect.h>
#include <APoint.h>

class FWGlyph;
typedef unsigned int UINT;

class FWStateResize : public FWState     
{
	FW_DECLARE_DYNAMIC(FWStateResize)
public:
	virtual void OnMouseMove(UINT nFlags, APointI point);
	virtual void OnLButtonUp(UINT nFlags, APointI point);
	virtual void OnLButtonDown(UINT nFlags, APointI point);
	virtual void OnDraw();

	FWStateResize(FWView *pView, FWGlyph *pGlyph);
	virtual ~FWStateResize();

private:
	FWGlyph *m_pGlyph;
	bool m_bLButtonDowned;
	ARectI m_rect;
	APointI m_ptMouseOld;
};

#endif 