#ifndef _FWGLYPHPOLYGON_H_
#define _FWGLYPHPOLYGON_H_

#include "FWGlyphGraphBase.h"
#include "FWArray.h"

class FWGlyphPolygon : public FWGlyphGraphBase
{
	FW_DECLARE_SERIAL(FWGlyphPolygon);
protected:
	FWArray<APointI, APointI&> m_HandleArray;
	FWArray<APointI, APointI&> m_RotatedHandleArray;
	ARectI m_SizeRect;
	ARectI m_RotatedSizeRect;
public:
	virtual void InsertHandleBefore(int nHandle, const APointArray &aryHandle, void *pDataIn = NULL);
	virtual void RemoveHandle(int nHandle, APointArray *pArrayHandleDeleted = NULL, void **ppDataOut = NULL);
	virtual void DuplicateHandle(int nHandle);

	// compute m_SizeRect and m_RotatedSizeRect
	void ComputeSizeRect();

	// set handles to be equal to the param
	void ResetHandles(const FWArray<APointI, APointI&> *HandleArray);

	// base class interface
	virtual CCharOutline * CreateOutline() const;
	virtual void MoveHandleTo(int nHandle, APointI newPoint);
	virtual ARectI GetSizeRect() const;
	virtual void SetSizeRect(ARectI rect);
	virtual void SetAngle(double angle);
	virtual ARectI GetTrueRect() const;
	virtual void SetCentralPoint(APointI point);
	virtual int GetHandleCount() const;
	virtual APointI GetHandle(int nPoint) const;
	virtual bool IsHit(const APointI& point) const;
	virtual void Serialize(FWArchive &ar);
	virtual const char * GetContextMenu();

	FWGlyphPolygon();
	virtual ~FWGlyphPolygon();

};

#endif 