#ifndef _FWSTATENETSELECT_H_
#define _FWSTATENETSELECT_H_

#include "FWState.h"
#include <ARect.h>
#include <APoint.h>

typedef unsigned int UINT;

class FWStateNetSelect : public FWState  
{
	FW_DECLARE_DYNAMIC(FWStateNetSelect)
public:
	virtual void OnDraw();
	virtual void OnMouseMove(UINT nFlags, APointI point);
	virtual void OnLButtonUp(UINT nFlags, APointI point);
	virtual void OnLButtonDown(UINT nFlags, APointI point);

	FWStateNetSelect(FWView *pView, APointI mousePoint);
	virtual ~FWStateNetSelect();

private:
	ARectI m_Rect;
	APointI m_originalPoint;
};


#endif 