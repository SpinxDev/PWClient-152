#ifndef _CHAROUTLINE_H_
#define _CHAROUTLINE_H_

#include <wtypes.h>
#include <WinGDI.h>

class CCharContour;
template<class T>
class APtrList;
//struct _GLYPHMETRICS;
//typedef struct _GLYPHMETRICS GLYPHMETRICS;
//struct HDC__;
//typedef struct HDC__* HDC;
//typedef unsigned char BYTE;
//typedef unsigned long DWORD;

// convert windows provided char glyph info to 
// a outline in CCharVerticesBuffer format, using
// CCharContour as its helper.
class CCharOutline  
{
private:
	APtrList<CCharContour *> * m_pListCharContour;
	GLYPHMETRICS m_metrics;
	bool GetOutlineBuffer(int nChar, HDC hDC, BYTE **ppBuffer, DWORD *pdwBufferSize);
public:
	const GLYPHMETRICS & GetMetrics() const { return m_metrics; }
	GLYPHMETRICS & GetMetrics() { return m_metrics; }
	void Simplify(float fSimplifyFactor = 0.9f, float fMinLenPerCurve = 0.f);
	int GetXOffset() const { return m_metrics.gmCellIncX; }
	const APtrList<CCharContour *> * GetCharContoursList() const { return m_pListCharContour; }
	APtrList<CCharContour *> * GetCharContoursList() { return m_pListCharContour; }
	bool CreateFromChar(int nChar, HDC hdc);
	void Destroy();
	CCharOutline();
	virtual ~CCharOutline();
};

#endif
