/*
	FILE: A3DAnimatable.h
	DESCRIPTION: Keyframe based animatable.
		classes: A3DAnimatableParam, A3DAnimatableValue, A3DAnimatable
	CREATED BY: xugang, 2012/3/28
*/

#ifndef A3DANIMATABLE_H_
#define A3DANIMATABLE_H_

#include <map>
#include <set>
#include <vector>
#include "AFile.h"
#include "A3DTypes.h"
#include <math.h>

namespace A3DAnimatable
{
	//	!!! Add entries in CreateEmptyAnimatable when change this enum
	enum ApplyType
	{
		APPLY_NONE = 0,					//	an animatable variable that has an empty Apply()
		APPLY_PS_CONST = 1,				//	an animatable variable that controls ps constants
	};
	
	// r, g, b: 0 ~ 255
	// A3DCOLORRGBA get incorrect color when r, g, b, a > 255
	inline A3DCOLOR MakeA3DColorInt(int red, int green, int blue, int alpha=255)
	{
		a_Clamp(red, 0, 255);
		a_Clamp(green, 0, 255);
		a_Clamp(blue, 0, 255);
		a_Clamp(alpha, 0, 255);
		return A3DCOLORRGBA(red, green, blue, alpha);
	}

	// r, g, b: 0.0 ~ 1.0
	inline A3DCOLOR MakeA3DColorFloat(float red, float green, float blue, float alpha=1.0f)
	{
		return MakeA3DColorInt(int(red*255), int(green*255), int(blue*255), int(alpha*255));
	}
};

// 一个数值，多种类型的 union
class A3DAnimatableUValue
{
public:
	enum ValueType
	{
		INVALID = 0,
		INT = 1,
		FLOAT = 2,
		VECTOR2 = 3,
		VECTOR3 = 4,
		VECTOR4 = 5,
		COLOR = 6,		// rgba

		FORCE_INT = 0x7fffffff,
	};
	A3DAnimatableUValue() : m_type(INVALID) {}

	static const char* TypeToName(ValueType type);
	static ValueType NameToType(const char* typeName);
	ValueType GetType() const { return m_type; };
	bool IsA(ValueType type) const { return GetType() == type; };
	bool IsEmpty() const { return GetType() == INVALID; };

	int GetInt() const;
	float GetFloat() const;
	APointF GetVector2() const;
	A3DVECTOR3 GetVector3() const;
	A3DVECTOR4 GetVector4() const;
	A3DCOLOR GetColor() const;

	void SetEmpty();
	void SetInt(int value);
	void SetFloat(float value);
	void SetVector2(const APointF& value);
	void SetVector3(const A3DVECTOR3& value);
	void SetVector4(const A3DVECTOR4& value);
	void SetColor(A3DCOLOR value);

	bool Save(AFile *file) const;
	bool Load(AFile *file);

	static A3DAnimatableUValue FromInt(int value) { A3DAnimatableUValue uvalue; uvalue.SetInt(value); return uvalue; }
	static A3DAnimatableUValue FromFloat(float value){ A3DAnimatableUValue uvalue; uvalue.SetFloat(value); return uvalue; }
	static A3DAnimatableUValue FromVector2(APointF value) { A3DAnimatableUValue uvalue; uvalue.SetVector2(value); return uvalue; }
	static A3DAnimatableUValue FromVector3(A3DVECTOR3 value) { A3DAnimatableUValue uvalue; uvalue.SetVector3(value); return uvalue; }
	static A3DAnimatableUValue FromVector4(A3DVECTOR4 value) { A3DAnimatableUValue uvalue; uvalue.SetVector4(value); return uvalue; }
	static A3DAnimatableUValue FromColor(A3DCOLOR value) { A3DAnimatableUValue uvalue; uvalue.SetColor(value); return uvalue; }

private:
	ValueType m_type;
	union
	{
		int intValue;
		unsigned int uintValue;
		float floatArr[4];
		A3DCOLOR colorValue;
	} m_value;
};

// 一个随时间变化的数值(float)
class A3DAnimatableParam
{
public:
	struct KeyFrameValue
	{
		float value;
		//TODO add bezier relative

		//KeyFrameValue& operator = (const KeyFrameValue& obj); // no need now
		bool Save(AFile *file) const;
		bool Load(AFile *file);
	};

	A3DAnimatableParam();
	A3DAnimatableParam(const A3DAnimatableParam& obj);
	void Swap(A3DAnimatableParam& obj);
	A3DAnimatableParam& operator =(const A3DAnimatableParam& obj);

	// if has no keyframe, GetInterpValue return this value
	void SetDefaultValue(float value) { m_defaultValue = value; };
	float GetDefaultValue() const { return m_defaultValue; };

	// get the interpolated value, should check IsEmpty() first
	//@param time ms, start from 0
	float GetInterpValue(int time) const;

	bool HasKeyFrame() const;
	// get keyframe time list
	//@param pTimes retrieve keyframes time
	void ListKeyFrames(std::vector<int> *pTimes/*out*/) const;
	//@param pValue receive keyfame value, could be NULL
	//@return if index is valid, store keyframe value into pValue and return true
	bool GetKeyFrame(int time, KeyFrameValue* pValue/*out*/) const;
	// add new keyframe or overwrite the old one
	void SetKeyFrame(int time, const KeyFrameValue &value);
	// if an old keyframe exist at the same time, return false
	//  or add a new keyframe and return true
	bool AddNewKeyFrame(int time);

	//@return whether removed one
	bool RemoveKeyFrame(int time);
	void RemoveAllKeyFrames();

	// move keyframe at oldTime to newTime. if newTime already has a keyframe, fail and return false
	bool MoveKeyFrame(int oldTime, int newTime);

	bool Save(AFile *file) const;
	bool Load(AFile *file);

private:
	float m_defaultValue;
	// time -> keyframe value
	typedef std::map<int, KeyFrameValue> KeyFrameMap;
	KeyFrameMap m_keyFrames;
};

class A3DGFXElement;

// 一个有确切含义的变量，可包含1～4个(GetParamCount) A3DAnimatableParam
// 数值类型为 GetValueType(如Vector2, Color等)，数值含义为 GetApplyType(如PixelShaderConst，Position等)
class A3DAnimatableBase
{
public:
	virtual ~A3DAnimatableBase()=0 {};
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName)=0;
	virtual bool Save(AFile* file) const=0;
	virtual bool Load(AFile* file)=0;
	virtual A3DAnimatableBase* Clone() const=0;

	//以下为编辑器用
	virtual A3DAnimatableUValue::ValueType GetValueType() const=0;
	virtual A3DAnimatable::ApplyType GetApplyType() const=0;
	bool IsA(A3DAnimatableUValue::ValueType type) const { return GetValueType() == type; };

	// 包含的 A3DAnimatableParam 数量
	virtual size_t GetParamCount() const=0;
	virtual A3DAnimatableParam* GetParam(size_t index)=0;

	virtual bool HasKeyFrame() const=0;
	// get keyframes time list
	virtual void ListKeyFrames(std::vector<int> *pTimes/*out*/) const=0;

	// if no keyframe, GetInterpValue() return uvalue
	virtual void SetDefaultValue(A3DAnimatableUValue uvalue)=0;
	virtual A3DAnimatableUValue GetDefaultValue()=0;

	//get value as A3DAnimatableValue
	// if a keyframe exists at given time, return its value; or return value with type EMPTY
	virtual A3DAnimatableUValue GetInterpValue(int time) const=0;

	// if a keyframe exists at given time, return its value; or return value with type EMPTY
	virtual A3DAnimatableUValue GetKeyFrame(int time) const=0;
	// and keyframe or overwrite the old
	virtual void SetKeyFrame(int time, const A3DAnimatableUValue &value)=0;
	// add a new keyframe, and set its value to interpolated value
	virtual void AddNewKeyFrame(int time)=0;
	// remove keyframe at given time
	virtual void RemoveKeyFrame(int time)=0;
	virtual void RemoveAllKeyFrame()=0;
	// move keyframe at oldTime to newTime. if newTime already has a keyframe, fail and return false
	virtual bool MoveKeyFrame(int oldTime, int newTime)=0;
};

// 实现 A3DAnimatableBase 的大部分接口函数
/*
typename ValueAdapter
{
public:
	static const int N_PARAMS;
	static A3DAnimatableUValue::ValueType GetValueType();
	static A3DAnimatableUValue ParamsToUValue(float params[N_PARAMS]);
	static void UValueToParams(A3DAnimatableUValue uvalue, float params[N_PARAMS]);	// params: output
};
*/
template <int APPLY_TYPE, typename ValueAdapter>
class A3DAnimatableImpl : public A3DAnimatableBase
{
public:
	static const int N_PARAMS = ValueAdapter::N_PARAMS;

	virtual ~A3DAnimatableImpl() {}
	virtual void Apply(int time, A3DGFXElement* pGfxElement, const char* paramName)=0;
	virtual bool Save(AFile* file) const;
	virtual bool Load(AFile* file);
	virtual A3DAnimatableBase* Clone() const=0;

	//以下为编辑器用
	virtual A3DAnimatableUValue::ValueType GetValueType() const { return ValueAdapter::GetValueType(); };
	virtual A3DAnimatable::ApplyType GetApplyType() const { return (A3DAnimatable::ApplyType)APPLY_TYPE; };
	virtual size_t GetParamCount() const { return N_PARAMS; };
	virtual A3DAnimatableParam* GetParam(size_t index);
	virtual bool HasKeyFrame() const;
	virtual void ListKeyFrames(std::vector<int> *pTimes/*out*/) const;

	//get value as A3DAnimatableValue
	virtual void SetDefaultValue(A3DAnimatableUValue uvalue);
	virtual A3DAnimatableUValue GetDefaultValue();
	virtual A3DAnimatableUValue GetInterpValue(int time) const;
	virtual A3DAnimatableUValue GetKeyFrame(int time) const;
	virtual void SetKeyFrame(int time, const A3DAnimatableUValue &value);
	virtual void AddNewKeyFrame(int time);
	virtual void RemoveKeyFrame(int time);
	virtual void RemoveAllKeyFrame();
	virtual bool MoveKeyFrame(int oldTime, int newTime);

protected:
	A3DAnimatableParam m_paramArr[N_PARAMS];
	// copy data of obj to self
	// sub classes use this function to implement Clone()
	void CopyAnimatableImpl(const A3DAnimatableImpl<APPLY_TYPE, ValueAdapter>& obj);
};

//
// ValueTypeAdapters
//

class A3DAnimatableIntAdapter
{
public:
	static const int N_PARAMS = 1;
	static A3DAnimatableUValue::ValueType GetValueType() { return A3DAnimatableUValue::INT; }
	static A3DAnimatableUValue ParamsToUValue(float params[1])
	{
		return A3DAnimatableUValue::FromInt((int)floorf(params[0]+0.5f));
	};
	static void UValueToParams(A3DAnimatableUValue uvalue, float params[1]/*out*/)
	{	
		params[0] = (float)uvalue.GetInt(); 
	}
};

class A3DAnimatableFloatAdapter
{
public:
	static const int N_PARAMS = 1;
	static A3DAnimatableUValue::ValueType GetValueType() { return A3DAnimatableUValue::FLOAT; }
	static A3DAnimatableUValue ParamsToUValue(float params[1])
	{
		return A3DAnimatableUValue::FromFloat(params[0]);
	};
	static void UValueToParams(A3DAnimatableUValue uvalue, float params[1]/*out*/)
	{	
		params[0] = uvalue.GetFloat(); 
	}
};

class A3DAnimatableVector2Adapter
{
public:
	static const int N_PARAMS = 2;
	static A3DAnimatableUValue::ValueType GetValueType() { return A3DAnimatableUValue::VECTOR2; }
	static A3DAnimatableUValue ParamsToUValue(float params[2])
	{
		return A3DAnimatableUValue::FromVector2(APointF(params[0], params[1]));
	};
	static void UValueToParams(A3DAnimatableUValue uvalue, float params[2]/*out*/)
	{
		const APointF& value = uvalue.GetVector2();
		params[0] = value.x; 
		params[1] = value.y; 
	}
};

class A3DAnimatableVector3Adapter
{
public:
	static const int N_PARAMS = 3;
	static A3DAnimatableUValue::ValueType GetValueType() { return A3DAnimatableUValue::VECTOR3; }
	static A3DAnimatableUValue ParamsToUValue(float params[3])
	{
		return A3DAnimatableUValue::FromVector3(A3DVECTOR3(params[0], params[1], params[2]));
	};
	static void UValueToParams(A3DAnimatableUValue uvalue, float params[3]/*out*/)
	{
		const A3DVECTOR3& value = uvalue.GetVector3();
		params[0] = value.x; 
		params[1] = value.y; 
		params[2] = value.z; 
	}
};

class A3DAnimatableVector4Adapter
{
public:
	static const int N_PARAMS = 4;
	static A3DAnimatableUValue::ValueType GetValueType() { return A3DAnimatableUValue::VECTOR4; }
	static A3DAnimatableUValue ParamsToUValue(float params[4])
	{
		return A3DAnimatableUValue::FromVector4(A3DVECTOR4(params[0], params[1], params[2], params[3]));
	};
	static void UValueToParams(A3DAnimatableUValue uvalue, float params[4]/*out*/)
	{
		const A3DVECTOR4& value = uvalue.GetVector4();
		params[0] = value.x;
		params[1] = value.y; 
		params[2] = value.z;
		params[3] = value.w;
	}
};

class A3DAnimatableColorAdapter
{
public:
	static const int N_PARAMS = 4;
	static A3DAnimatableUValue::ValueType GetValueType() { return A3DAnimatableUValue::COLOR; }
	static A3DAnimatableUValue ParamsToUValue(float params[4])
	{
		return A3DAnimatableUValue::FromColor(A3DAnimatable::MakeA3DColorFloat(params[0], params[1], params[2], params[3]));
	};
	static void UValueToParams(A3DAnimatableUValue uvalue, float params[4]/*out*/)
	{
		A3DCOLOR value = uvalue.GetColor();
		params[0] = A3DCOLOR_GETRED(value)/255.0f;
		params[1] = A3DCOLOR_GETGREEN(value)/255.0f;
		params[2] = A3DCOLOR_GETBLUE(value)/255.0f;
		params[3] = A3DCOLOR_GETALPHA(value)/255.0f;
	}
};

#endif //ndef A3DANIMATABLE_H_