#ifndef _FWARTTILT_H_
#define _FWARTTILT_H_

#include "FWArt.h"

class FWArtTilt : public FWArt  
{
	FW_DECLARE_SERIAL(FWArtTilt)
	PROPERTY_INT(Angle)
public:
	FWArtTilt();
	virtual ~FWArtTilt();
	FWArtTilt & operator = (const FWArtTilt & src);
	IMPLEMENT_CLONE_INLINE(FWArtTilt, FWArt);

	virtual void MakeArtistic(CCharOutline *pOutline) const;
	virtual void Serialize(FWArchive &ar);
	virtual void FillParamArray(FWParamArray &arrayParam) const;
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam);
};

#endif 
