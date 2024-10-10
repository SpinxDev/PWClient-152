#ifndef _FWGLYPHRECTBASE_H_
#define _FWGLYPHRECTBASE_H_

#include "FWGlyphGraphBase.h"
#include "Global.h"
#include <ocidl.h>

// The base of all glyph whose outline is a rectangle
class FWGlyphRectBase : public FWGlyphGraphBase  
{
	FW_DECLARE_DYNAMIC(FWGlyphRectBase)
protected:
	virtual ARectI GetHandleRect(int nIndex) const;
	
	virtual ARectI GetMinimalRect() const
	{ 
		return ARectI(0, 0, 6, 6) + GetCentralPoint(); 
	}

	ARectI m_rect;
	
	// get center of float type
	POINTF GetCentralPointF() const
	{ 
		POINTF point = 
		{ 
			float(m_rect.right + m_rect.left) / 2,
			float(m_rect.bottom + m_rect.top) / 2
		};
		return point;
	}
	
	// if angle != 0, this pointer will return a pointer
	// to an array of 4 POINTs which are the four corners 
	// of the rotated rect. 
	// if angle = 0, this pointer will be null.
	POINT* GetCornerPoint() const{ return m_pPoint; }

	virtual void RecalcPoint();

public:
	FWGlyphRectBase();
	virtual ~FWGlyphRectBase();

	virtual void Serialize(FWArchive& ar);

	// If you derived this function, do call the base
	virtual void MoveHandleTo(int nHandle, APointI newPoint);

	// Test whecther the point is inside glyph
	virtual bool IsHit(const APointI& point) const;

	// Return the total number of handles
	virtual int GetHandleCount() const;

	// Get the point of the handle specified by index
	virtual APointI GetHandle(int nPoint) const;

	// Get the rect to adjust the size.
	// The size rect will keep constant when the glyph is 
	// rotated. It will always be the rect returned when angle = 0
	virtual ARectI GetSizeRect() const;

	// Move and resize the rect. The offset is based on the 
	// ARectI returned by GetSizeRect
	// The size rect will keep constant when the glyph is 
	// rotated. It will always be the rect returned when angle = 0
	// If you derived this function, do call the base
	virtual void SetSizeRect(ARectI rect);

	// Return the minimum rect the glyph has covered.
	virtual ARectI GetTrueRect() const;

	// If you derived this function, do call the base
	virtual void SetAngle(double angle);

private:
	// if angle != 0, this pointer will point to an array of 4 POINTs
	// which are the four corners of the rotated rect. 
	// if angle = 0, this pointer will be null.
	POINT* m_pPoint;
};

#endif 
