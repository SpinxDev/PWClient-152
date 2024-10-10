#ifndef A3DANIMATABLE_PSCONST_H
#define A3DANIMATABLE_PSCONST_H

#include "A3DAnimatable.h"

#define CLONE_IMPL(CLASSNAME) \
	virtual A3DAnimatableBase* Clone() const { \
		CLASSNAME *p = new CLASSNAME(); \
		p->CopyAnimatableImpl(*this); \
		return p; \
	}

class A3DAnimatable_PSConstInt : public A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST, A3DAnimatableIntAdapter>
{
public:
	virtual A3DAnimatableBase* Clone() const;
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName);
};

class A3DAnimatable_PSConstFloat : public A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST, A3DAnimatableFloatAdapter>
{
public:
	virtual A3DAnimatableBase* Clone() const;
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName);
};

class A3DAnimatable_PSConstVector2 : public A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST , A3DAnimatableVector2Adapter>
{
public:
	virtual A3DAnimatableBase* Clone() const;
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName);
};

class A3DAnimatable_PSConstVector3 : public A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST, A3DAnimatableVector3Adapter>
{
public:
	virtual A3DAnimatableBase* Clone() const;
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName);
};


class A3DAnimatable_PSConstVector4 : public A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST, A3DAnimatableVector4Adapter>
{
public:
	virtual A3DAnimatableBase* Clone() const;
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName);
};

class A3DAnimatable_PSConstColor : public A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST, A3DAnimatableColorAdapter>
{
public:
	virtual A3DAnimatableBase* Clone() const;
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName);
};

#endif //ndef A3DANIMATABLE_PSCONST_H
