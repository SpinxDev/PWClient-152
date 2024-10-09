#ifndef _FWALGORITHM_H_
#define _FWALGORITHM_H_

#include <math.h>
#include "FWArray.h"
#include <ARect.h>

struct GFX_PROPERTY;
class FWArchive;
class CCharOutline;
class A3DGFXElement;
class A3DGFXEx;
typedef A3DGFXEx FWAssembly;
typedef struct tagPOINTF POINTF;
typedef struct tagPOINTF *LPPOINTF;

// some common and frequently used algorithm in FWEditorLib
namespace FWAlgorithm  
{

	ARectI GetMinRect(const FWArray<APointI, APointI &> & PointArray);
	ARectI GetMinRect(const POINT* PointArray, int size);

	double GetLengthWithTwoPoint(const APointI &ptFirst, const APointI &ptSecond);

	bool IsPointsInLine(const FWArray<APointI, APointI&>& pointArray);

	APointI RotatePoint(APointI original, POINTF centre, double angle);
	void RotatePoint(const POINT* original, POINT* result, int count, POINTF centre, double angle);
	void RotatePoint(const FWArray<APointI, APointI &> & original, FWArray<APointI, APointI&> & result, 
		const POINTF & centre, double angle);

    void CopyPointArray(FWArray<APointI, APointI &> & dest, const FWArray<APointI, APointI &> & original);

	void Point2DoubleArray(const APointI& point, double * array);
	void Point2DoubleArray(const POINT& point, double * array);

	void ClampPointToRect(APointI &pt, const ARectI &rect);
	void ClampRectToRect(ARectI &rect, const ARectI &standard);

	void SerializeGFX_PROPERTY(GFX_PROPERTY & prop, FWArchive &ar);

	APointI GetRelativePointToRect(const ARectI * lpRect, APointI * lpPoint);

	
	double FIXEDToDouble(FIXED f);
	int DoubleToInt(double d);

	ARectI GetOutlineRect(const CCharOutline *pOutline);

	bool IsParticleSystem(const A3DGFXElement *pElement);
	int ComputeParticalCount(const FWAssembly *pAssembly);

	bool IsGfxSound(const A3DGFXElement *pElement);
	int ComputeSoundCount(const FWAssembly *pAssembly);
	void RemoveSoundElement(FWAssembly *pAssembly);
}

#endif 
