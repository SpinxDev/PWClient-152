#ifndef _FWFIREWORKELEMENT_H_
#define _FWFIREWORKELEMENT_H_

#include "CodeTemplate.h"
#include "FWFireworkLeaf.h"

class FWModifier;

// FWFireworkElement is represent a single element
// in custom firework
// it is internal used by FWFireworkCustom
class FWFireworkElement : public FWFireworkLeaf  
{
	PROPERTY_BOOL(EnableSound)
protected:
	PROPERTY_DEFINE_POINTER(FWModifier *, Modifier);
public:
	const FWModifier * GetModifier() const {return m_pModifier;}
	void SetModifier(FWModifier *pSrc);
public:
	FWFireworkElement();
	virtual ~FWFireworkElement();
	virtual bool CopyTo(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime);
};

#endif 