#include "StdAfx.h"
#include "A3DAnimatable_PSConst.h"
#include "A3DAnimatable.h"
#include "A3DAnimatable_def.hpp"
#include "A3DGFXElement.h"

// class A3DAnimatableInt
template class A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST, A3DAnimatableIntAdapter>;

A3DAnimatableBase* A3DAnimatable_PSConstInt::Clone() const
{
	A3DAnimatable_PSConstInt *p = new A3DAnimatable_PSConstInt();
	p->CopyAnimatableImpl(*this);
	return p;
}

void A3DAnimatable_PSConstInt::Apply(int time, A3DGFXElement* pGfxElement, const char* paramName)
{
	float value = m_paramArr[0].GetInterpValue(time);
	pGfxElement->GetHLSLInfo().PixelShader()->SetConstValueFloat(paramName, value);
}

// class A3DAnimatableFloat
template class A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST, A3DAnimatableFloatAdapter>;

A3DAnimatableBase* A3DAnimatable_PSConstFloat::Clone() const
{
	A3DAnimatable_PSConstFloat *p = new A3DAnimatable_PSConstFloat();
	p->CopyAnimatableImpl(*this);
	return p;
}

void A3DAnimatable_PSConstFloat::Apply(int time, A3DGFXElement* pGfxElement, const char* paramName)
{
	float value = m_paramArr[0].GetInterpValue(time);
	pGfxElement->GetHLSLInfo().PixelShader()->SetConstValueFloat(paramName, value);
}

// class A3DAnimatableVector2
template class A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST , A3DAnimatableVector2Adapter>;

A3DAnimatableBase* A3DAnimatable_PSConstVector2::Clone() const
{
	A3DAnimatable_PSConstVector2 *p = new A3DAnimatable_PSConstVector2();
	p->CopyAnimatableImpl(*this);
	return p;
}

void A3DAnimatable_PSConstVector2::Apply(int time, A3DGFXElement* pGfxElement, const char* paramName)
{
	float value1 = m_paramArr[0].GetInterpValue(time);
	float value2 = m_paramArr[1].GetInterpValue(time);
	pGfxElement->GetHLSLInfo().PixelShader()->SetConstValueFloat2(paramName, value1, value2);
}

// class A3DAnimatableVector3
template class A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST , A3DAnimatableVector2Adapter>;

A3DAnimatableBase* A3DAnimatable_PSConstVector3::Clone() const
{
	A3DAnimatable_PSConstVector3 *p = new A3DAnimatable_PSConstVector3();
	p->CopyAnimatableImpl(*this);
	return p;
}

void A3DAnimatable_PSConstVector3::Apply(int time, A3DGFXElement* pGfxElement, const char* paramName)
{
	float value1 = m_paramArr[0].GetInterpValue(time);
	float value2 = m_paramArr[1].GetInterpValue(time);
	float value3 = m_paramArr[2].GetInterpValue(time);
	pGfxElement->GetHLSLInfo().PixelShader()->SetConstValueFloat3(paramName, value1, value2, value3);
}

// class A3DAnimatableVector4
template class A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST , A3DAnimatableVector2Adapter>;

A3DAnimatableBase* A3DAnimatable_PSConstVector4::Clone() const
{
	A3DAnimatable_PSConstVector4 *p = new A3DAnimatable_PSConstVector4();
	p->CopyAnimatableImpl(*this);
	return p;
}

void A3DAnimatable_PSConstVector4::Apply(int time, A3DGFXElement* pGfxElement, const char* paramName)
{
	float value1 = m_paramArr[0].GetInterpValue(time);
	float value2 = m_paramArr[1].GetInterpValue(time);
	float value3 = m_paramArr[2].GetInterpValue(time);
	float value4 = m_paramArr[3].GetInterpValue(time);
	pGfxElement->GetHLSLInfo().PixelShader()->SetConstValueFloat4(paramName, value1, value2, value3, value4);
}

// class A3DAnimatableColor
template class A3DAnimatableImpl<A3DAnimatable::APPLY_PS_CONST , A3DAnimatableVector2Adapter>;

A3DAnimatableBase* A3DAnimatable_PSConstColor::Clone() const
{
	A3DAnimatable_PSConstColor *p = new A3DAnimatable_PSConstColor();
	p->CopyAnimatableImpl(*this);
	return p;
}

void A3DAnimatable_PSConstColor::Apply(int time, A3DGFXElement* pGfxElement, const char* paramName)
{
	float value1 = m_paramArr[0].GetInterpValue(time);
	float value2 = m_paramArr[1].GetInterpValue(time);
	float value3 = m_paramArr[2].GetInterpValue(time);
	float value4 = m_paramArr[3].GetInterpValue(time);
	pGfxElement->GetHLSLInfo().PixelShader()->SetConstValueFloat4(paramName, value1, value2, value3, value4);
}

