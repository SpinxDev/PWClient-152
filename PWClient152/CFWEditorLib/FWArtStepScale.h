#ifndef _FWARTSTEPSCALE_H_
#define _FWARTSTEPSCALE_H_

#include "FWArt.h"

class FWArtStepScale : public FWArt  
{
	FW_DECLARE_SERIAL(FWArtStepScale)
	PROPERTY_FLOAT(RatioLeft)
	PROPERTY_FLOAT(RatioRight)
public:
	FWArtStepScale();
	virtual ~FWArtStepScale();

	FWArtStepScale & operator = (const FWArtStepScale & src);
	IMPLEMENT_CLONE_INLINE(FWArtStepScale, FWArt);
	
	virtual void MakeArtistic(CCharOutline *pOutline) const;
	virtual void Serialize(FWArchive &ar);
	virtual void FillParamArray(FWParamArray &arrayParam) const;
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam);
};

#endif 
