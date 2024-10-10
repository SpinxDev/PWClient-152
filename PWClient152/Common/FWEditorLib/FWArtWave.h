#ifndef _FWARTWAVE_H_
#define _FWARTWAVE_H_

#include "FWArt.h"

class FWArtWave : public FWArt  
{
	FW_DECLARE_SERIAL(FWArtWave)
	PROPERTY_INT(WaveCount)
	PROPERTY_FLOAT(WaveHeightRatio)
public:
	FWArtWave();
	virtual ~FWArtWave();
	FWArtWave & operator = (const FWArtWave & art);
	IMPLEMENT_CLONE_INLINE(FWArtWave, FWArt);

	virtual void MakeArtistic(CCharOutline *pOutline) const;
	virtual void Serialize(FWArchive &ar);
	virtual void FillParamArray(FWParamArray &arrayParam) const;
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam);

};

#endif 
