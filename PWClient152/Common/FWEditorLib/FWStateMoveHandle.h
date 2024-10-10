#ifndef _FWSTATEMOVEHANDLE_H_
#define _FWSTATEMOVEHANDLE_H_

#include "FWState.h"

class FWCommandMoveHandle;
class FWGlyph;

class FWStateMoveHandle : public FWState  
{
	FW_DECLARE_DYNAMIC(FWStateMoveHandle)
public:
	virtual void OnMouseMove(UINT nFlags, APointI point);
	virtual void OnLButtonUp(UINT nFlags, APointI point);

	FWStateMoveHandle(FWView *pView, FWGlyph* pGlyph, int nHandle);
	virtual ~FWStateMoveHandle();

private:
	FWGlyph* m_pGlyph;
	int m_nHandle;
	APointI m_oldPoint;
};



#endif 