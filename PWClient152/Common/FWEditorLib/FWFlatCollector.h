#ifndef _FWFLATCOLLECTOR_H_
#define _FWFLATCOLLECTOR_H_

#include "a3dflatcollector.h"
#include "CodeTemplate.h"
#include "FWConfig.h"

class FWFlatCollector :	public A3DFlatCollector
{
public:
	FWFlatCollector(void);
	virtual ~FWFlatCollector(void);
	bool DrawRectangle2D(ARectI rect, DWORD dwLineColor, bool bFill = false, DWORD dwFillColor = COLOR_WHITE, bool bFlush = false);
	bool DrawLine(APointI pt1, APointI pt2, DWORD dwColor = COLOR_BLACK, bool bFlush = false);
};

#endif
