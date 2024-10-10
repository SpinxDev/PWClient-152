#ifndef _FWARTNOCHANGE_H_
#define _FWARTNOCHANGE_H_

#include "FWArt.h"

// no-change style of art text, this art change noting
// to the original outline
class FWArtNoChange : public FWArt  
{
	FW_DECLARE_SERIAL(FWArtNoChange)
public:
	FWArtNoChange();
	virtual ~FWArtNoChange();
	FWArtNoChange & operator = (const FWArtNoChange & src);
	IMPLEMENT_CLONE_INLINE(FWArtNoChange, FWArt)

	virtual void MakeArtistic(CCharOutline *pOutline) const {}
};

#endif
