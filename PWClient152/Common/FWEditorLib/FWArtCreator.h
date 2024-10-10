#ifndef _FWARTCREATOR_H_
#define _FWARTCREATOR_H_

#include "FWInfoSet.h"
#include "CodeTemplate.h"

class FWArt;
class FWRuntimeClass;

struct FWArtInfo : public FWInfo
{
	const FWRuntimeClass * pClass;
};

// used to manager all FWArtxxx class
class FWArtCreator : public FWInfoSet<FWArtInfo>
{
	DECLARE_SINGLETON(FWArtCreator)
protected:
	bool m_bInited;
public:
	FWArtCreator();
	virtual ~FWArtCreator();

	enum
	{
		ID_ART_DEFAULT = 0,
		ID_ART_NO_CHANGE = ID_ART_DEFAULT,
		ID_ART_TILT,
		ID_ART_ARC,
		ID_ART_DOUBLE_ARC,
		ID_ART_WAVE,
		ID_ART_STEP_SCALE,
		ID_ART_COUNT
	};

	FWArt * CreateArt(int nID);
	virtual bool Init();
};

#endif
