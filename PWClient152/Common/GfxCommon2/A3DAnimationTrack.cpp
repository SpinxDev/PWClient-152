#include "StdAfx.h"
#include "A3DAnimationTrack.h"
#include "A3DAnimatable.h"
#include "A3DAnimatable_PSConst.h"
#include "A3DAnimatable_ApplyNone.h"

A3DAnimationTrack::A3DAnimationTrack(const A3DAnimationTrack &obj)
{
	for (AnimatableMap::const_iterator it=obj.m_animatableMap.begin(); it!=obj.m_animatableMap.end(); ++it)
	{
		const AString& name = it->first;
		A3DAnimatableBase* pAnimatable = it->second;
		A3DAnimatableBase* pAnimatableClone = pAnimatable->Clone();
		m_animatableMap[name] = pAnimatableClone;
	}
}

A3DAnimationTrack::~A3DAnimationTrack()
{
	RemoveAllAnimatables();
}

void A3DAnimationTrack::Swap(A3DAnimationTrack &obj)
{
	m_animatableMap.swap(obj.m_animatableMap);
}

A3DAnimationTrack& A3DAnimationTrack::operator =(const A3DAnimationTrack &obj)
{
	if (this == &obj)
		return *this;

	Swap(A3DAnimationTrack(obj));
	return *this;
}

void A3DAnimationTrack::Apply(int time, A3DGFXElement* pGfxElement)
{
	for (AnimatableMap::iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		const char* paramName = it->first;
		A3DAnimatableBase *pAnimatable = it->second;
		pAnimatable->Apply(time, pGfxElement, paramName);
	}

	// real apply
	pGfxElement->GetHLSLInfo().PixelShader()->ApplyConstValues();
}

static const int ANIMATION_TRACK_VERSION = 1;
static const char* format_animationTrackTagAndVersion = "AnimationTrack version: %d";
static const char* format_animatableCount = "animatableCount: %d";
static const char* format_animatableNameAndType = "name: %s valueType: %s applyType: %d";

bool A3DAnimationTrack::Save(AFile* file) const
{
	// count non-empty animatable
	int animatableCount = 0;
	for (AnimatableMap::const_iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		A3DAnimatableBase *pAnimatable = it->second;
		if (pAnimatable->HasKeyFrame())
			++animatableCount;
	}

	char line[AFILE_LINEMAXLEN];

	// object name, version, count of animatables
	sprintf(line, format_animationTrackTagAndVersion, ANIMATION_TRACK_VERSION);
	file->WriteLine(line);

	sprintf(line, format_animatableCount, animatableCount);
	file->WriteLine(line);

	// animatables
	for (AnimatableMap::const_iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		A3DAnimatableBase *pAnimatable = it->second;
		if (!pAnimatable->HasKeyFrame())
			continue;
		const AString& name = it->first;

		// animatable name and type
		sprintf(line, format_animatableNameAndType, 
			name, 
			A3DAnimatableUValue::TypeToName(pAnimatable->GetValueType()),
			pAnimatable->GetApplyType());
		file->WriteLine(line);
		
		bool ret = pAnimatable->Save(file);
		if (!ret)
		{
			a_LogOutput(1, "A3DAnimationTrack::Save, fail to save A3DAnimatable %s", name);
			return false;
		}
	}
	return true;
}

bool A3DAnimationTrack::Load(AFile* file)
{
	RemoveAllAnimatables();

	char line[AFILE_LINEMAXLEN];
	DWORD readLength;

	//read object name, version, count of animatables
	int version;
	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_animationTrackTagAndVersion, &version) != 1 || version != ANIMATION_TRACK_VERSION)
	{
		a_LogOutput(1, "A3DAnimationTrack::Load, invalid Tag or Version");
		return false;
	}

	int animatableCount;
	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_animatableCount, &animatableCount) != 1)
	{
		a_LogOutput(1, "A3DAnimationTrack::Load, invalid animatable Count");
		return false;
	}

	for (int iAnim=0; iAnim < animatableCount; ++iAnim)
	{
		//animatable name and type
		char name[64];
		char valueTypeName[64];
		int applyTypeValue;

		file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
		if (sscanf(line, format_animatableNameAndType, name, valueTypeName, &applyTypeValue) != 3)
		{
			a_LogOutput(1, "A3DAnimationTrack::Load, invalid Name or Type at index %d", iAnim);
			return false;
		}

		A3DAnimatableUValue::ValueType valueType = A3DAnimatableUValue::NameToType(valueTypeName);
		A3DAnimatable::ApplyType applyType = (A3DAnimatable::ApplyType)applyTypeValue;

		A3DAnimatableBase* pAnimatable = AddNewAnimatable(name, valueType, applyType);
		if (pAnimatable)
		{
			bool ret = pAnimatable->Load(file);
			if (!ret)
			{
				RemoveAnimatable(name);

				a_LogOutput(1, "A3DAnimationTrack::Load, fail to load A3DAnimatable %s", name);
				return false;
			}
		}
		else
		{
			a_LogOutput(1, "A3DAnimationTrack::Load, invalid type for animatable %s", name);
			return false;
		}
	}
	return true;
}

bool A3DAnimationTrack::HasKeyFrame() const
{
	for (AnimatableMap::const_iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		A3DAnimatableBase* pAnim = it->second;
		if (pAnim->HasKeyFrame())
			return true;
	}
	return false;
}

void A3DAnimationTrack::ListAnimatables(std::vector<AString> *pNames/*out*/) const
{
	pNames->clear();
	for (AnimatableMap::const_iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		const AString& name = it->first;
		pNames->push_back(name);
	}
}

void A3DAnimationTrack::ListKeyFrames(std::vector<int> *pTimes/*out*/) const
{
	// 只要一个Animatable有keyframe, 则此时间点有keyframe
	std::set<int> keyframeSet;
	for (AnimatableMap::const_iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		A3DAnimatableBase* anim = it->second;
		std::vector<int> keys;
		anim->ListKeyFrames(&keys);
		keyframeSet.insert(keys.begin(), keys.end());
	}
	pTimes->clear();
	pTimes->insert(pTimes->begin(), keyframeSet.begin(), keyframeSet.end());
}

A3DAnimatableBase* A3DAnimationTrack::GetAnimatable(const char* name)
{
	AnimatableMap::iterator it = m_animatableMap.find(name);
	if (it != m_animatableMap.end())
		return it->second;
	else
		return NULL;
}

A3DAnimatableBase* A3DAnimationTrack::AddNewAnimatable(const char* name, A3DAnimatableUValue::ValueType valueType, A3DAnimatable::ApplyType applyType)
{
	AnimatableMap::iterator it = m_animatableMap.find(name);
	if (it != m_animatableMap.end())
	{
		ASSERT(0 && "try to add animatable with exist name");
		return NULL;
	}

	A3DAnimatableBase* pAnimatable = CreateEmptyAnimatable(valueType, applyType);
	if (pAnimatable == NULL)
	{
		ASSERT(0 && "try to add animatable with invalid type");
		return NULL;
	}

	m_animatableMap[name] = pAnimatable;
	return pAnimatable;
}

bool A3DAnimationTrack::RemoveAnimatable(const char* name)
{
	AnimatableMap::iterator it = m_animatableMap.find(name);
	if (it != m_animatableMap.end())
	{
		A3DAnimatableBase *pAnimatable = it->second;
		delete pAnimatable;
		m_animatableMap.erase(it);
		return true;
	}
	else
	{
		return false;
	}
}

void A3DAnimationTrack::RemoveAllAnimatables()
{
	for (AnimatableMap::iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		A3DAnimatableBase *pAnimatable = it->second;
		delete pAnimatable;
	}
	m_animatableMap.clear();
}

bool A3DAnimationTrack::MoveKeyFrame(int oldTime, int newTime)
{
	for (AnimatableMap::iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		A3DAnimatableBase *pAnimatable = it->second;
		if (!pAnimatable->GetKeyFrame(newTime).IsEmpty())
			return false;
	}

	for (AnimatableMap::iterator it=m_animatableMap.begin(); it!=m_animatableMap.end(); ++it)
	{
		A3DAnimatableBase *pAnimatable = it->second;
		if (!pAnimatable->MoveKeyFrame(oldTime, newTime))
			return false;
	}

	return true;
}

A3DAnimatableBase* A3DAnimationTrack::CreateEmptyAnimatable(A3DAnimatableUValue::ValueType valueType, A3DAnimatable::ApplyType applyType)
{
	A3DAnimatableBase* pAnim = NULL;
	switch (applyType)
	{
	case A3DAnimatable::APPLY_NONE:
		switch(valueType)
		{
		case A3DAnimatableUValue::INT:
			pAnim = new A3DAnimatable_ApplyNoneInt();
			break;
		case A3DAnimatableUValue::FLOAT:
			pAnim = new A3DAnimatable_ApplyNoneFloat();
			break;
		case A3DAnimatableUValue::VECTOR2:
			pAnim = new A3DAnimatable_ApplyNoneVector2();
			break;
		case A3DAnimatableUValue::VECTOR3:
			pAnim = new A3DAnimatable_ApplyNoneVector3();
			break;
		case A3DAnimatableUValue::VECTOR4:
			pAnim = new A3DAnimatable_ApplyNoneVector4();
			break;
		case A3DAnimatableUValue::COLOR:
			pAnim = new A3DAnimatable_ApplyNoneColor();
			break;
		}
		break;
	case A3DAnimatable::APPLY_PS_CONST:
		switch(valueType)
		{
		case A3DAnimatableUValue::INT:
			pAnim = new A3DAnimatable_PSConstInt();
			break;
		case A3DAnimatableUValue::FLOAT:
			pAnim = new A3DAnimatable_PSConstFloat();
			break;
		case A3DAnimatableUValue::VECTOR2:
			pAnim = new A3DAnimatable_PSConstVector2();
			break;
		case A3DAnimatableUValue::VECTOR3:
			pAnim = new A3DAnimatable_PSConstVector3();
			break;
		case A3DAnimatableUValue::VECTOR4:
			pAnim = new A3DAnimatable_PSConstVector4();
			break;
		case A3DAnimatableUValue::COLOR:
			pAnim = new A3DAnimatable_PSConstColor();
			break;
		}
		break;
	}
	return pAnim;
}
