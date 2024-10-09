/*
 * FILE: EC_AssureMove.h
 *
 * DESCRIPTION: AssureMove for the Element Client
 *
 * CREATED BY: Hedi, 2007/4/23 
 *
 * HISTORY:
 *
 * Copyright (c) 2007 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_ASSUREMOVE_H_
#define _EC_ASSUREMOVE_H_

#include <A3DVector.h>
#include <vector.h>
#include "aabbcd.h"
#include "A3DGeometry.h"

using namespace CHBasedCD;
using namespace abase;

struct BRUSHGRID
{
	vector<int>		listCCDBrushes;
};

class CECAssureMove
{
protected:
	vector<AString>						m_listFiles[7];

	vector<CCDBrush *>					m_CDBrushes;
	BRUSHGRID *							m_pBrushGrids;

	A3DVECTOR3							m_vecMapOrigin;
	float								m_fGridSize;
	int									m_nGridRows;
	int									m_nGridCols;

	bool								m_bHasCheatCD;
	bool								m_bHasCheatFly;
	A3DVECTOR3							m_vecCheatCDPos;
	A3DVECTOR3							m_vecCheatFlyPos;

	DWORD								m_dwFloatStart;
	int									m_nFloatTicks;

public:
	CECAssureMove();
	~CECAssureMove();

	bool LoadMap(const char * szMap, A3DVECTOR3 vecMapOrigin, float fMapWidth, float fMapHeight, float fGridSize=32.0f);
	bool ReleaseMap();

	// nStep is 0~7
	bool StepLoadMap(int nStep);

	bool AssureMove(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecEnd);
	bool NoAssureMove();

	inline bool IsCheatCD()			{ return m_bHasCheatCD; }
	inline bool IsCheatFly()		{ return m_bHasCheatFly; }

	inline const A3DVECTOR3& GetCheatCDPos() { return m_vecCheatCDPos; }
	inline const A3DVECTOR3& GetCheatFlyPos() { return m_vecCheatFlyPos; }
};

#endif //_EC_ASSUREMOVE_H_