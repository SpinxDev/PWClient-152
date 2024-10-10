/*
	FILE: A3DAnimationTrack.h
	DESCRIPTION: Keyframe based animation track.
		classes: A3DAnimationTrack
	CREATED BY: xugang, 2012/3/27
 */

#ifndef _A3DANIMATIONTRACK_H_
#define _A3DANIMATIONTRACK_H_

#include <map>
#include <vector>
#include "A3DAnimatable.h"

// all animatables
class A3DAnimationTrack
{
public:
	A3DAnimationTrack() {}
	A3DAnimationTrack(const A3DAnimationTrack &obj);
	~A3DAnimationTrack();
	void Swap(A3DAnimationTrack &obj);
	// copy data of obj to self
	A3DAnimationTrack& operator =(const A3DAnimationTrack &obj);

	// apply all animatables, if pGfxElement is not set, just skip
	//@param time ms, start from 0 ms
	void Apply(int time, A3DGFXElement* pGfxElement);
	bool Save(AFile* file) const;
	bool Load(AFile* file);

	//以下为编辑器用
	bool IsEmpty() { return m_animatableMap.empty(); };
	bool HasKeyFrame() const;
	// get animatable name list
	void ListAnimatables(std::vector<AString> *pNames/*out*/) const;
	// list time of all keyframes
	void ListKeyFrames(std::vector<int> *pTimes/*out*/) const;
	// get animatable base class pointer by name, this pointer could be dynamic casted to sub class pointer
	A3DAnimatableBase* GetAnimatable(const char* name);
	// if exist one with the same name, return NULL
	//  else add new animatable with the given name and type, return pointer of the object  
	A3DAnimatableBase* AddNewAnimatable(const char* name, A3DAnimatableUValue::ValueType valueType, A3DAnimatable::ApplyType applyType);
	// remove animatable with the given name, return true if removed one
	bool RemoveAnimatable(const char* name);
	void RemoveAllAnimatables();
	bool MoveKeyFrame(int oldTime, int newTime);

private:
	// name -> data
	typedef std::map<AString, A3DAnimatableBase*> AnimatableMap;
	AnimatableMap m_animatableMap;

	// create an empty animatable according to the type
	A3DAnimatableBase* CreateEmptyAnimatable(A3DAnimatableUValue::ValueType valueType, A3DAnimatable::ApplyType applyType);
};

#endif //ndef _A3DANIMATIONTRACK_H_