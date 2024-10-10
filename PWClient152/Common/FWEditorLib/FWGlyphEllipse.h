#ifndef _FWGLYPHELLIPSE_H_
#define _FWGLYPHELLIPSE_H_

#include "FWGlyphRectBase.h"
#include <math.h>


class FWGlyphEllipse : public FWGlyphRectBase     
{
	FW_DECLARE_SERIAL(FWGlyphEllipse);
	// point count will be computed for outline
	enum {POINT_COUNT = 500}; 
public:
	FWGlyphEllipse();
	virtual ~FWGlyphEllipse();

	// base class interface
	virtual void SetAngle(double angle);
	virtual bool IsHit(const APointI &point) const;
	virtual CCharOutline * CreateOutline() const;
	virtual void MoveHandleTo(int nHandle, APointI newPoint);
	virtual void Serialize(FWArchive& ar);
	virtual void SetSizeRect(ARectI rect);
private:
	// compute rotated outline points
	void Rotate(double fDegree);

	// compute unrotated outline points
	void Reform();

	// points form the outline
	POINT m_ptList[POINT_COUNT];
	POINT m_rotatedList[POINT_COUNT];	
};

#endif 