#ifndef _FWART_H_
#define _FWART_H_


#include "FWObject.h"
#include "CodeTemplate.h"
#include "FWParam.h"

class CCharOutline;
class CCharContour;

// base class for art serial
class FWArt : public FWObject
{
	FW_DECLARE_DYNAMIC(FWArt)
public:
	virtual ~FWArt(){}
	virtual FWArt * Clone() const = 0;
	virtual void MakeArtistic(CCharOutline *pOutline) const = 0;
	virtual void Serialize(FWArchive &ar) { FWObject::Serialize(ar); }
	// fill an array of params which is used by UI
	virtual void FillParamArray(FWParamArray &arrayParam) const {}
	// upate content from an array of params that passed by UI
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam) {}
};

// note : we implement a loop to access points in each derived
// class, instead of implementing loop in base class and calling a
// virtual computing function for each point, in order to minimize 
// the overhead of virtual-function-call

#define FOR_EACH_POINT \
{ \
	APtrList<CCharContour *> *pContourList = pOutline->GetCharContoursList(); \
	ALISTPOSITION pos = pContourList->GetHeadPosition(); \
	while (pos) \
	{ \
		CCharContour *pContour = pContourList->GetNext(pos); \
		CCharVerticesBuffer *pBuffer = pContour->GetCharVerticesBuffer(); \
		for (int i = 0; i < pBuffer->m_nVerticesCount; i++) \
		{ \
			double &x = pBuffer->m_paVertices[i][0], &y = pBuffer->m_paVertices[i][1];

#define END_FOR_EACH_POINT \
		} \
	} \
}


#endif 
