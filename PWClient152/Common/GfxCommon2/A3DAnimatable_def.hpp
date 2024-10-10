/*
	FILE: A3DAnimatable_def.hpp
	DESCRIPTION: template definition of A3DAnimatable.h
	CREATED BY: xugang, 2012/4/10
*/

#ifndef A3DANIMATABLE_DEF_HPP_
#define A3DANIMATABLE_DEF_HPP_

#include "A3DAnimatable.h"

/////////////////////////////////////////////////////
// implement template class A3DAnimatableImpl
/////////////////////////////////////////////////////
static const int ANIMATABLE_VERSION = 1;
static const char* format_animatableTagAndVersion = "Animatable version: %d";
static const char* format_animatableParamCount = "paramCount: %d";

template <int APPLY_TYPE, typename ValueAdapter>
bool A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::Save(AFile* file) const
{
	char line[AFILE_LINEMAXLEN];

	// tag, version
	sprintf(line, format_animatableTagAndVersion, ANIMATABLE_VERSION);
	file->WriteLine(line);

	// param count
	sprintf(line, format_animatableParamCount, N_PARAMS);
	file->WriteLine(line);

	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		bool ret = m_paramArr[iParam].Save(file);
		if (!ret)
		{
			//... log
			return false;
		}
	}

	return true;
}

template <int APPLY_TYPE, typename ValueAdapter>
bool A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::Load(AFile* file)
{
	char line[AFILE_LINEMAXLEN];
	DWORD readLength;

	// tag, version
	int version;
	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_animatableTagAndVersion, &version) != 1 || version != ANIMATABLE_VERSION)
	{
		//... log
		return false;
	}

	// param count
	int paramCount;
	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_animatableParamCount, &paramCount)!=1 || paramCount != N_PARAMS)
	{
		//... log
		return false;
	}

	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		bool ret = m_paramArr[iParam].Load(file);
		if (!ret)
		{
			//... log
			return false;
		}
	}

	return true;
}

template <int APPLY_TYPE, typename ValueAdapter>
A3DAnimatableParam* A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::GetParam(size_t index)
{
	if (index < N_PARAMS)
		return &m_paramArr[index];
	else
		return NULL;
}

template <int APPLY_TYPE, typename ValueAdapter>
bool A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::HasKeyFrame() const
{
	// 只要一个分量有Keyframe，则有效
	for (int i=0; i<N_PARAMS; ++i)
	{
		if (m_paramArr[i].HasKeyFrame())
			return true;
	}
	return false;
}

template <int APPLY_TYPE, typename ValueAdapter>
void A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::ListKeyFrames(std::vector<int> *pTimes/*out*/) const
{
	std::vector<int> listArr[N_PARAMS];		// 各参数各自的关键帧
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		m_paramArr[iParam].ListKeyFrames(&listArr[iParam]);
	}

	// 合并，某时间点上只要有一个参数有关键帧，则关键帧有效
	// 用 std::set 实现，未利用到 listArr[i] 的有序性，但此处性能要求不高
	std::set<int> listOr;
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		listOr.insert(listArr[iParam].begin(), listArr[iParam].end());
	}

	// 输出
	for (std::set<int>::iterator it=listOr.begin(); it!=listOr.end(); ++it)
	{
		pTimes->push_back(*it);
	}

	return;
}

template <int APPLY_TYPE, typename ValueAdapter>
void A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::SetDefaultValue(A3DAnimatableUValue uvalue)
{
	float params[N_PARAMS];
	ValueAdapter::UValueToParams(uvalue, params);

	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		m_paramArr[iParam].SetDefaultValue(params[iParam]);
	}
}

template <int APPLY_TYPE, typename ValueAdapter>
A3DAnimatableUValue A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::GetDefaultValue()
{
	float params[N_PARAMS];
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		params[iParam] = m_paramArr[iParam].GetDefaultValue();
	}

	return ValueAdapter::ParamsToUValue(params);
}


template <int APPLY_TYPE, typename ValueAdapter>
A3DAnimatableUValue A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::GetInterpValue(int time) const
{
	float params[N_PARAMS];
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		params[iParam] = m_paramArr[iParam].GetInterpValue(time);
	}

	return ValueAdapter::ParamsToUValue(params);
}

template <int APPLY_TYPE, typename ValueAdapter>
A3DAnimatableUValue A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::GetKeyFrame(int time) const
{
	bool isKey = false;		// 只要有一个 param 在此时间点上有 KeyFrame，则关键帧有效
	float params[N_PARAMS];

	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		A3DAnimatableParam::KeyFrameValue keyFrameValue;
		bool bKey = m_paramArr[iParam].GetKeyFrame(time, &keyFrameValue);
		if (bKey)
		{
			isKey = true;
			params[iParam] = keyFrameValue.value;
		}
		else
		{
			params[iParam] = m_paramArr[iParam].GetInterpValue(time);
		}
	}

	if (isKey)
		return ValueAdapter::ParamsToUValue(params);
	else
		return A3DAnimatableUValue();
}

template <int APPLY_TYPE, typename ValueAdapter>
void A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::SetKeyFrame(int time, const A3DAnimatableUValue &value)
{
	float params[N_PARAMS];
	ValueAdapter::UValueToParams(value, params);
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		A3DAnimatableParam::KeyFrameValue keyFrameValue;
		keyFrameValue.value = params[iParam];
		m_paramArr[iParam].SetKeyFrame(time, keyFrameValue);
	}
}

template <int APPLY_TYPE, typename ValueAdapter>
void A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::AddNewKeyFrame(int time)
{
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		m_paramArr[iParam].AddNewKeyFrame(time);
	}
}

template <int APPLY_TYPE, typename ValueAdapter>
void A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::RemoveKeyFrame(int time)
{
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		m_paramArr[iParam].RemoveKeyFrame(time);
	}
}

template <int APPLY_TYPE, typename ValueAdapter>
void A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::RemoveAllKeyFrame()
{
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		m_paramArr[iParam].RemoveAllKeyFrames();
	}
}

template <int APPLY_TYPE, typename ValueAdapter>
bool A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::MoveKeyFrame(int oldTime, int newTime)
{
	if (!GetKeyFrame(newTime).IsEmpty())
		return false;

	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		m_paramArr[iParam].MoveKeyFrame(oldTime, newTime);
	}
	return true;
}


template <int APPLY_TYPE, typename ValueAdapter>
void A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>::CopyAnimatableImpl(const A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>& obj)
{
	for (int iParam=0; iParam<N_PARAMS; ++iParam)
	{
		m_paramArr[iParam] = obj.m_paramArr[iParam];
	}
}

#endif //ndef A3DANIMATABLE_DEF_HPP_
