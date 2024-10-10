/*
 * FILE: ConvexHullArchive.h
 *
 * DESCRIPTION: Load & Save Convex hull data
 *
 * CREATED BY: 
 *
 * HISTORY:
 *
 * 
 */

#ifndef _CONVEXHULLARCHIVE_H_
#define _CONVEXHULLARCHIVE_H_

#include "ConvexHullData.h"

#include <AFile.h>
#include <A3DFuncs.h>
///////////////////////////////////////////////////////////////////////////
//
//	new version of convex hull data
//
// unsigned int magic
// unsigned int version
// char szModelFile[MAX_PATH]
// enum GenType (MOX / ECM)//  
// [ IF GenType == ECM] int nFrame;
// unsigned int  nNumHull;
// convex hull data 1..nNumHull
///////////////////////////////////////////////////////////////////////////
namespace CHBasedCD
{

class ConvexHullArchive
{

public:
	enum GENTYPE
	{
		NONE = 0,
		MOX = 1,
		ECM = 2,
	};
public:
	ConvexHullArchive();
	~ConvexHullArchive();

	//convex hull data generate from, mox or ecm?
	GENTYPE GetGenType() const { return m_genType; }
	//get ecm file the convex hull generate from
	const char* GetModelFile() const;
	int GetFrame() const;
	//get hull number
	int GetNumHull() const;

	void SetGenType(GENTYPE type) { m_genType = type; }
	void SetModelFile(const char* szModelFile);
	void SetFrame(int nFrame) { m_nFrame = nFrame; }
	void SetNumHull(int nNumHull) { m_nNumHull = nNumHull; }

	bool LoadHeader(AFile& af);
	inline bool LoadHull(AFile& af, CHBasedCD::CConvexHullData* pCHData)
	{		
		if (!pCHData->LoadBinaryData(&af))
		{
			return false;
		}
		if (m_bOldEcp)
			pCHData->Transform(TransformMatrix(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0), A3DVECTOR3(0)));
		return true;
	}
	bool SaveHeader(AFile& af);
	inline bool SaveHull(AFile& af, CHBasedCD::CConvexHullData* pCHData)
	{
		return pCHData->SaveBinaryData(&af);
	}

protected:
	int m_nNumHull;

	GENTYPE m_genType;//convex hull data generate from, mox or ecm?
	char m_szModelFile[MAX_PATH];
	int  m_nFrame;
	bool m_bOldEcp;//old version .ecp
};


}


#endif