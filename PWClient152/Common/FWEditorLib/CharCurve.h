#ifndef _CHARCURVE_H_
#define _CHARCURVE_H_

#include "CharVerticesBuffer.h"

struct tagTTPOLYCURVE;
typedef struct tagTTPOLYCURVE TTPOLYCURVE;

// convert windows provided char glyph info to 
// a curve in CCharVerticesBuffer format.
class CCharCurve
{
private:
	bool BuildPolyline(double *pStartPoint, TTPOLYCURVE *pCurveHeader);
	void ConvertQBToCB(double *p0, double *p1, double *p2, int nDeepth);
	bool BuildQBLine(double *pStartPoint, TTPOLYCURVE *pCurveHeader, int nDeepth);
	CCharVerticesBuffer * m_pCharVerticesBuffer;
	int m_nCurrentIndex; // used to split qb line
public:
	void Destroy();
	const CCharVerticesBuffer * GetCharVerticesBuffer() const { return m_pCharVerticesBuffer; }
	CCharVerticesBuffer * GetCharVerticesBuffer() { return m_pCharVerticesBuffer; }
	bool CreateFromFont(double *pStartPoint, TTPOLYCURVE *pCurveHeader);
	CCharCurve();
	~CCharCurve();

};

#endif
