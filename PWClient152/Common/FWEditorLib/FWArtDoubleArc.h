#ifndef _FWARTDOUBLEARC_H_
#define _FWARTDOUBLEARC_H_

#include "FWArt.h"

class FWArtDoubleArc : public FWArt  
{
	FW_DECLARE_SERIAL(FWArtDoubleArc)
	PROPERTY_FLOAT(Ratio)
public:
	FWArtDoubleArc();
	virtual ~FWArtDoubleArc();
	FWArtDoubleArc & operator = (const FWArtDoubleArc & src);
	IMPLEMENT_CLONE_INLINE(FWArtDoubleArc, FWArt);
	
	virtual void MakeArtistic(CCharOutline *pOutline) const;
	virtual void Serialize(FWArchive &ar);
	virtual void FillParamArray(FWParamArray &arrayParam) const;
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam);

};

#endif 
