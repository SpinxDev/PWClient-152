#ifndef _FWSTATEMOVE_H_
#define _FWSTATEMOVE_H_

#include "FWState.h"
#include <AList2.h>
#include <APoint.h>
#include <ARect.h>

class CMoveCommand;
class FWGlyphList;
typedef unsigned int UINT;

class FWStateMove : public FWState  
{
	FW_DECLARE_DYNAMIC(FWStateMove)
public:
	virtual void OnMouseMove(UINT nFlags, APointI point);
	virtual void OnLButtonUp(UINT nFlags, APointI point);

	FWStateMove(FWView *pView, APointI mousePoint);
	virtual ~FWStateMove();
private:
	AList2<APointI, APointI&> m_centerList;

	APointI m_originalPoint;
	ARectI m_originalRect;

	void ClampOffsetToView(const FWGlyphList* pList, APointI& offset);
};

#endif 