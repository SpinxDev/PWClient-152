#include "StdAfx.h"
#include "A3DAnimatable_ApplyNone.h"
#include "A3DAnimatable.h"
#include "A3DAnimatable_def.hpp"

template class A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableIntAdapter>;
template class A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableFloatAdapter>;
template class A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableVector2Adapter>;
template class A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableVector3Adapter>;
template class A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableVector4Adapter>;
template class A3DAnimatableImpl<A3DAnimatable::APPLY_NONE, A3DAnimatableColorAdapter>;

