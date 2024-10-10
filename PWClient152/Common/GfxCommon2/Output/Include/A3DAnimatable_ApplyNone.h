#ifndef A3DANIMATABLE_APPLYNONE_H
#define A3DANIMATABLE_APPLYNONE_H

#include "A3DAnimatable.h"

#define APPLY_NONE_IMPL(CLASSNAME) \
	virtual A3DAnimatableBase* Clone() const { \
		CLASSNAME *p = new CLASSNAME(); \
		p->CopyAnimatableImpl(*this); \
		return p; \
	}\
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName) {}

class A3DAnimatable_ApplyNoneInt : public A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableIntAdapter>
{
	APPLY_NONE_IMPL(A3DAnimatable_ApplyNoneInt);
};

class A3DAnimatable_ApplyNoneFloat : public A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableFloatAdapter>
{
	APPLY_NONE_IMPL(A3DAnimatable_ApplyNoneFloat);
};

class A3DAnimatable_ApplyNoneVector2 : public A3DAnimatableImpl<A3DAnimatable::APPLY_NONE , A3DAnimatableVector2Adapter>
{
	APPLY_NONE_IMPL(A3DAnimatable_ApplyNoneVector2);
};

class A3DAnimatable_ApplyNoneVector3 : public A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableVector3Adapter>
{
	APPLY_NONE_IMPL(A3DAnimatable_ApplyNoneVector3);
};

class A3DAnimatable_ApplyNoneVector4 : public A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableVector4Adapter>
{
	APPLY_NONE_IMPL(A3DAnimatable_ApplyNoneVector4);
};

class A3DAnimatable_ApplyNoneColor : public A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableColorAdapter>
{
	APPLY_NONE_IMPL(A3DAnimatable_ApplyNoneColor);
};

#endif //ndef A3DANIMATABLE_APPLYNONE_H
