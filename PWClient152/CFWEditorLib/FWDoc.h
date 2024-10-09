#ifndef _FWDOC_H_
#define _FWDOC_H_

#include "CodeTemplate.h"
#include "FWConfig.h"
#include <AList2.h>
#include "FWTemplate.h"

class FWGlyphList;
class FWCommandList;
class FWView;
class FWGlyphFirework;
class FWAssemblySet;
class FWGlyph;
class FWArchive;

typedef AList2<int, int> AIntList;

// FWDoc is responsible of taking care of all
// data used for drawing
class FWDoc : public FWTemplate
{
	friend class FWManager;

	PROPERTY_INT(MaxParticalQuota);
	PROPERTY_INT(CurrentParticalQuota);
	
	PROPERTY_INT(MaxSoundQuota);
	PROPERTY_INT(CurrentSoundQuota);
protected:
	FWDoc();
	virtual ~FWDoc();

	// this function must be called at least once
	// before any access to other member after the contruction
	// it is used internal by FWManager
	void AttachView(FWView *pView);

	// serialize, used internal
	// this function will not catch any exception,
	// we leave the work to SaveTo()/LoadFrom()
	virtual void Serialize(FWArchive& ar);	

protected:
	PROPERTY_DEFINE_POINTER(AIntList *, ParticalQuotaList);
	PROPERTY_DEFINE_POINTER(AIntList *, SoundQuotaList);
	PROPERTY_DEFINE_POINTER(FWCommandList *, CommandList);
	PROPERTY_DEFINE_POINTER(FWView *, View);
public:
	// update member that storing partical count
	void UpdateQuotaAfterAdd();
	void UpdateQuotaBeforeDelete(ALISTPOSITION posGlyph);
	void UpdateQuotaAfterUpdate(FWGlyph *pGlyph);

	// some property-get/set method
	PROPERTY_GET_POINTER(FWGlyphList *, GlyphList);
	PROPERTY_GET_POINTER(FWCommandList *, CommandList);
	PROPERTY_GET_POINTER(FWGlyphFirework *, GlyphLanch);
	PROPERTY_GET_POINTER(AIntList *, ParticalQuotaList);
	PROPERTY_GET_POINTER(AIntList *, SoundQuotaList);
};

#endif 