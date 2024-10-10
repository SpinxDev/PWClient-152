#ifndef _FWGLYPHFIREWORK_H_
#define _FWGLYPHFIREWORK_H_

#include "FWGlyphGraphBase.h"


class FWGlyphFirework : public FWGlyph
{
	friend class FWStateCreateFirework;
	FW_DECLARE_SERIAL(FWGlyphFirework)
protected:
	// rect draw on screen
	// all FWGlyphFirework simply draw a rectangle on
	// screen to represent itself
	ARectI m_rect;
public:
	FWGlyphFirework();
	virtual ~FWGlyphFirework();

	// base class interface
	virtual bool IsCombinable() { return false; }
	virtual CCharOutline * CreateOutline() const { return NULL;}
	virtual ARectI GetHandleRect(int nIndex) const { return ARectI(-1, -1, -1, -1);}
	virtual void MoveHandleTo(int nHandle, APointI newPoint) {}
	virtual ARectI GetSizeRect() const;
	virtual void SetSizeRect(ARectI rect);
	virtual ARectI GetTrueRect() const { return m_rect; }
	virtual int GetHitHandle(APointI point) const { return 0;}
	virtual int GetHandleCount() const { return 0; }
	virtual APointI GetHandle(int nPoint) const { return APointI(0, 0); }
	virtual bool IsHit(const APointI& point) const { return m_rect.PtInRect(point); }
	virtual bool PrepareFireworkData(float fScale);
	virtual void Serialize(FWArchive &ar);
#ifdef _FW_EDITOR
	virtual void DrawHandles(FWView *pView) const;
	virtual void Draw(FWView *pView) const;
#endif
};

#endif 