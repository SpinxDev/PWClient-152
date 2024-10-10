#include "StdAfx.h"
#include "A3DAnimatable.h"
#include "A3DGFXElement.h"
#include <assert.h>

///////////////////////////////////////////////
// class A3DAnimatableParam::KeyFrameValue
///////////////////////////////////////////////
static const char* format_keyFrameValue = "value: %f";

bool A3DAnimatableParam::KeyFrameValue::Save(AFile *file) const
{
	char line[AFILE_LINEMAXLEN];

	sprintf(line, format_keyFrameValue, value);
	file->WriteLine(line);

	return true;
}

bool A3DAnimatableParam::KeyFrameValue::Load(AFile *file)
{
	char line[AFILE_LINEMAXLEN];
	DWORD readLength;

	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_keyFrameValue, &value) != 1)
	{
		a_LogOutput(1, "A3DAnimatableParam::KeyFrameValue::Load, invalid value");
		return false;
	}
	return true;
}

///////////////////////////////////////////////
// class A3DAnimatableParam
///////////////////////////////////////////////

A3DAnimatableParam::A3DAnimatableParam()
{
	m_defaultValue = 0.0f;
}

A3DAnimatableParam::A3DAnimatableParam(const A3DAnimatableParam& obj)
	: m_defaultValue(obj.m_defaultValue), m_keyFrames(obj.m_keyFrames)
{
}

void A3DAnimatableParam::Swap(A3DAnimatableParam& obj)
{
	std::swap(m_defaultValue, obj.m_defaultValue);
	m_keyFrames.swap(obj.m_keyFrames);
}

A3DAnimatableParam& A3DAnimatableParam::operator =(const A3DAnimatableParam& obj)
{
	if (this == &obj)
		return *this;

	Swap(A3DAnimatableParam(obj));
	return *this;
}

// 目前只有线性插值
float A3DAnimatableParam::GetInterpValue(int time) const
{
	if (m_keyFrames.empty())	// no keyframe, return default value
		return m_defaultValue;

	KeyFrameMap::const_iterator it_upper_bound = m_keyFrames.upper_bound(time);
	if (it_upper_bound == m_keyFrames.end())	// time is too large, return the last value
	{
		const KeyFrameValue& keyframeValue = m_keyFrames.rbegin()->second;
		return keyframeValue.value;
	}
	else if (it_upper_bound == m_keyFrames.begin())	// time is too small, return the first value
	{
		const KeyFrameValue& keyframeValue = m_keyFrames.begin()->second;
		return keyframeValue.value;
	}

	KeyFrameMap::const_iterator it_lower = it_upper_bound;	--it_lower;
	assert(it_lower != m_keyFrames.end());

	int time1 = it_lower->first;
	float value1 = it_lower->second.value;
	int time2 = it_upper_bound->first;
	float value2 = it_upper_bound->second.value;

	assert(time1 < time2);
	// lerp
	float s = (float)(time-time1)/(float)(time2-time1);
	return value1 * (1-s) + value2 * s;
}

bool A3DAnimatableParam::HasKeyFrame() const
{
	return !m_keyFrames.empty();
}

void A3DAnimatableParam::ListKeyFrames(std::vector<int> *pTimes/*out*/) const
{
	pTimes->clear();
	for (KeyFrameMap::const_iterator it=m_keyFrames.begin(); it!=m_keyFrames.end(); ++it)
	{
		int time = it->first;
		pTimes->push_back(time);
	}
}

bool A3DAnimatableParam::GetKeyFrame(int time, KeyFrameValue* pValue/*out*/) const
{
	KeyFrameMap::const_iterator it=m_keyFrames.find(time);
	if (it != m_keyFrames.end())
	{
		if (pValue)
		{
			KeyFrameValue value = it->second;
			*pValue = value;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void A3DAnimatableParam::SetKeyFrame(int time, const KeyFrameValue &value)
{
	m_keyFrames[time] = value;
}

bool A3DAnimatableParam::AddNewKeyFrame(int time)
{
	KeyFrameMap::iterator it=m_keyFrames.find(time);
	if (it != m_keyFrames.end())
	{
		return false;
	}
	else
	{
		KeyFrameValue keyFrameValue;
		keyFrameValue.value = GetInterpValue(time);
		m_keyFrames[time] = keyFrameValue;
		return true;
	}
}

bool A3DAnimatableParam::RemoveKeyFrame(int time)
{
	return m_keyFrames.erase(time) > 0;
}

void A3DAnimatableParam::RemoveAllKeyFrames()
{
	m_keyFrames.clear();
}

bool A3DAnimatableParam::MoveKeyFrame(int oldTime, int newTime)
{
	if (m_keyFrames.find(newTime) != m_keyFrames.end())
		return false;

	KeyFrameMap::iterator itOld = m_keyFrames.find(oldTime);
	if (itOld != m_keyFrames.end())
	{
		m_keyFrames[newTime] = itOld->second;
		m_keyFrames.erase(oldTime);
	}

	return true;
}

static const char* format_animatableParamTagCountDefault = "param count: %d default: %f";
static const char* format_animatableParamTime = "time: %d";

bool A3DAnimatableParam::Save(AFile *file) const
{
	char line[AFILE_LINEMAXLEN];

	//tag, count, default value
	sprintf(line, format_animatableParamTagCountDefault, m_keyFrames.size(), m_defaultValue);
	file->WriteLine(line);

	for (KeyFrameMap::const_iterator it=m_keyFrames.begin(); it!=m_keyFrames.end(); ++it)
	{
		int time = it->first;
		const KeyFrameValue& value = it->second;

		sprintf(line, format_animatableParamTime, time, value);
		file->WriteLine(line);

		value.Save(file);
	}
	return true;
}

bool A3DAnimatableParam::Load(AFile *file)
{
	RemoveAllKeyFrames();

	char line[AFILE_LINEMAXLEN];
	DWORD readLength;

	//tag, count, default value
	int count;
	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_animatableParamTagCountDefault, &count, &m_defaultValue) != 2)
	{
		a_LogOutput(1, "A3DAnimatableParam::Load, invalid Tag Count or Default");
		return false;
	}

	for (int iKey=0; iKey<count; ++iKey)
	{
		// time
		int time;
		file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
		if (sscanf(line, format_animatableParamTime, &time) != 1)
		{
			a_LogOutput(1, "A3DAnimatableParam::Load, invalid Time at index %d", iKey);
			return false;
		}
		
		KeyFrameValue value;
		bool ret = value.Load(file);
		if (!ret)
		{
			a_LogOutput(1, "A3DAnimatableParam::Load, fail to load KeyFrameValue at index %d width time: %d", iKey, time);
			return false;
		}

		m_keyFrames[time] = value;
	}

	return true;
}

///////////////////////////////////////////////////////
// class A3DAnimatableValue
///////////////////////////////////////////////////////

//static
const char* A3DAnimatableUValue::TypeToName(ValueType type)
{
	switch(type)
	{
	case INT:
		return "int";
	case FLOAT:
		return "float";
	case VECTOR2:
		return "vector2";
	case VECTOR3:
		return "vector3";
	case VECTOR4:
		return "vector4";
	case COLOR:
		return "color";
	default:
		return "empty";
	}
}

//static
A3DAnimatableUValue::ValueType A3DAnimatableUValue::NameToType(const char* typeName)
{
	if (0==strcmp(typeName, "int"))
		return INT;
	else if (0==strcmp(typeName, "float"))
		return FLOAT;
	else if (0==strcmp(typeName, "vector2"))
		return VECTOR2;
	else if (0==strcmp(typeName, "vector3"))
		return VECTOR3;
	else if (0==strcmp(typeName, "vector4"))
		return VECTOR4;
	else if (0==strcmp(typeName, "color"))
		return COLOR;
	else
		return INVALID;
}

int A3DAnimatableUValue::GetInt() const
{
	if (m_type == INT)
	{
		return m_value.intValue;
	}
	else
	{
		ASSERT(0 && "try to GetInt while type is not INT");
		return 0;
	}
}

float A3DAnimatableUValue::GetFloat() const
{
	if (m_type == FLOAT)
	{
		return m_value.floatArr[0];
	}
	else
	{
		ASSERT(0 && "try to GetFloat while type is not FLOAT");
		return 0.0f;
	}
}

APointF A3DAnimatableUValue::GetVector2() const
{
	if (m_type == VECTOR2)
	{
		return APointF(m_value.floatArr[0], m_value.floatArr[1]);
	}
	else
	{
		ASSERT(0 && "try to GetVector2 while type is not VECTOR2");
		return APointF();
	}
}

A3DVECTOR3 A3DAnimatableUValue::GetVector3() const
{
	if (m_type == VECTOR3)
	{
		return A3DVECTOR3(m_value.floatArr[0], m_value.floatArr[1], m_value.floatArr[2]);
	}
	else
	{
		ASSERT(0 && "try to GetVector3 while type is not VECTOR3");
		return A3DVECTOR3();
	}
}

A3DVECTOR4 A3DAnimatableUValue::GetVector4() const
{
	if (m_type == VECTOR4)
	{
		return A3DVECTOR4(m_value.floatArr[0], m_value.floatArr[1], m_value.floatArr[2], m_value.floatArr[3]);
	}
	else
	{
		ASSERT(0 && "try to GetVector4 while type is not VECTOR4");
		return A3DVECTOR4();
	}
}

A3DCOLOR A3DAnimatableUValue::GetColor() const
{
	if (m_type == COLOR)
	{
		return m_value.colorValue;
	}
	else
	{
		ASSERT(0 && "try to GetColor while type is not COLOR");
		return 0;
	}
}

void A3DAnimatableUValue::SetEmpty()
{
	m_type = INVALID;
}

void A3DAnimatableUValue::SetInt(int value)
{
	m_type = INT;
	m_value.intValue = value;
}

void A3DAnimatableUValue::SetFloat(float value)
{
	m_type = FLOAT;
	m_value.floatArr[0] = value;
}

void A3DAnimatableUValue::SetVector2(const APointF& value)
{
	m_type = VECTOR2;
	m_value.floatArr[0] = value.x;
	m_value.floatArr[1] = value.y;
}

void A3DAnimatableUValue::SetVector3(const A3DVECTOR3& value)
{
	m_type = VECTOR3;
	m_value.floatArr[0] = value.x;
	m_value.floatArr[1] = value.y;
	m_value.floatArr[2] = value.z;
}

void A3DAnimatableUValue::SetVector4(const A3DVECTOR4& value)
{
	m_type = VECTOR4;
	m_value.floatArr[0] = value.x;
	m_value.floatArr[1] = value.y;
	m_value.floatArr[2] = value.z;
	m_value.floatArr[3] = value.w;
}

void A3DAnimatableUValue::SetColor(A3DCOLOR value)
{
	m_type = COLOR;
	m_value.colorValue = value;
}

static const char* format_uvalue_type = "uvalue_type: %d";
static const char* format_uvalue_empty = "empty";
static const char* format_uvalue_int = "%d";
static const char* format_uvalue_float = "%f";
static const char* format_uvalue_vector2 = "%f, %f";
static const char* format_uvalue_vector3 = "%f, %f, %f";
static const char* format_uvalue_vector4 = "%f, %f, %f, %f";
static const char* format_uvalue_color = "0x%08X";

bool A3DAnimatableUValue::Save(AFile *file) const
{
	char line[AFILE_LINEMAXLEN];
	sprintf(line, format_uvalue_type, GetType());
	file->WriteLine(line);

	switch(GetType())
	{
	case A3DAnimatableUValue::INVALID:
		sprintf(line, format_uvalue_empty);
		break;
	case A3DAnimatableUValue::INT:
		sprintf(line, format_uvalue_int, GetInt());
		break;
	case A3DAnimatableUValue::FLOAT:
		sprintf(line, format_uvalue_float, m_value.floatArr[0]);
		break;
	case A3DAnimatableUValue::VECTOR2:
		sprintf(line, format_uvalue_vector2, m_value.floatArr[0], m_value.floatArr[1]);
		break;
	case A3DAnimatableUValue::VECTOR3:
		sprintf(line, format_uvalue_vector3, m_value.floatArr[0], m_value.floatArr[1], m_value.floatArr[2]);
		break;
	case A3DAnimatableUValue::VECTOR4:
		sprintf(line, format_uvalue_vector4, m_value.floatArr[0], m_value.floatArr[1], m_value.floatArr[2], m_value.floatArr[3]);
		break;
	case A3DAnimatableUValue::COLOR:
		sprintf(line, format_uvalue_color, GetColor());
		break;
	}

	file->WriteLine(line);
	return true;
}

bool A3DAnimatableUValue::Load(AFile *file)
{
	char line[AFILE_LINEMAXLEN];
	DWORD readLength;

	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_uvalue_type, reinterpret_cast<int*>(&m_type)) != 1)
		return false;

	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	switch (m_type)
	{
	case A3DAnimatableUValue::INVALID:
		if (strcmp(line, format_uvalue_empty) != 0)
			return false;
		break;
	case A3DAnimatableUValue::INT:
		if (sscanf(line, format_uvalue_int, &m_value.intValue) != 1)
			return false;
		break;
	case A3DAnimatableUValue::FLOAT:
		if (sscanf(line, format_uvalue_float, &m_value.floatArr[0]) != 1)
			return false;
		break;
	case A3DAnimatableUValue::VECTOR2:
		if (sscanf(line, format_uvalue_vector2, &m_value.floatArr[0], &m_value.floatArr[1]) != 2)
			return false;
		break;
	case A3DAnimatableUValue::VECTOR3:
		if (sscanf(line, format_uvalue_vector3, &m_value.floatArr[0], &m_value.floatArr[1], &m_value.floatArr[2]) != 3)
			return false;
		break;
	case A3DAnimatableUValue::VECTOR4:
		if (sscanf(line, format_uvalue_vector4, &m_value.floatArr[0], &m_value.floatArr[1], 
			&m_value.floatArr[2], &m_value.floatArr[3]) != 4)
			return false;
		break;
	case A3DAnimatableUValue::COLOR:
		if (sscanf(line, format_uvalue_color, &m_value.colorValue) != 1)
			return false;
		break;
	default:
		return false;
	}

	return true;	
}
