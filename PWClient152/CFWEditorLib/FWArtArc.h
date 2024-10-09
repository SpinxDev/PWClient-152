#ifndef _FWARTARC_H_
#define _FWARTARC_H_

#include "FWArt.h"

class FWArtArc : public FWArt  
{
	FW_DECLARE_SERIAL(FWArtArc)
	PROPERTY_INT(Angle)
public:
	FWArtArc();
	virtual ~FWArtArc();
	FWArtArc & operator = (const FWArtArc & src);
	IMPLEMENT_CLONE_INLINE(FWArtArc, FWArt);

	virtual void MakeArtistic(CCharOutline *pOutline) const;
	virtual void Serialize(FWArchive &ar);
	virtual void FillParamArray(FWParamArray &arrayParam) const;
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam);
};

#endif
