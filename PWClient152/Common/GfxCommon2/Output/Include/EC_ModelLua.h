#ifndef _ECMODELLUA_H_
#define _ECMODELLUA_H_



//////////////////////////////////////////////////////////////////////////
//
//	Implement luaValueWrapper
//
//////////////////////////////////////////////////////////////////////////

class luaValueWrapper
{
public:

	luaValueWrapper(const bool& bVal);
	luaValueWrapper(const double& dbVal);
	luaValueWrapper(const unsigned int& nVal);
	luaValueWrapper(const AString& str);
	luaValueWrapper(const char* szBuf);
	luaValueWrapper(void* pVoid);
	luaValueWrapper(const CScriptValue& sv);
	luaValueWrapper(const luaValueWrapper& rhs);
	luaValueWrapper& operator = (const luaValueWrapper& rhs);

	operator CScriptValue() const { return m_val; }

	const CScriptValue& get() const { return m_val; }
	CScriptValue& get() { return m_val; }
	const char * toString() const;
	float toFloat() const;
	bool toBool() const;
	int toInteger() const;
	CECModel* toECM() const;
	EVENT_INFO* toEventInfo() const;
	bool operator == (const luaValueWrapper& rhs) const;
	bool operator != (const luaValueWrapper& rhs) const;
	bool is_empty() const;

public:

	static bool is_equal(const CScriptValue& lhs, const CScriptValue& rhs);
	static bool is_equal_array(const CScriptValue& lhs, const CScriptValue& rhs);
	static bool is_equal_string(const CScriptValue& lhs, const CScriptValue& rhs);

private:

	void initStrUtf8(const char* szUtf8Buf, int nLen);
	void copy(const CScriptValue& rhs);
	void clear();

private:
	
	CScriptValue m_val;
};

inline bool operator == (const CScriptValue& lhs, const CScriptValue& rhs)
{
	return luaValueWrapper::is_equal(lhs, rhs);
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement luaTableWrapper
//
//////////////////////////////////////////////////////////////////////////

// wraps the lua's table based on CScriptValue
class luaTableWrapper
{
public:	// constructors / destructor
	
	luaTableWrapper();
	luaTableWrapper(const CScriptValue& val);
	luaTableWrapper(const luaTableWrapper& table);

	static luaTableWrapper FromValue(const CScriptValue& val)
	{
		return luaTableWrapper(val);
	}
	
public:

	void append(const luaValueWrapper& key, const luaValueWrapper& val);
	void setitem(const luaValueWrapper& key, const luaValueWrapper& val);
	luaValueWrapper getitem(const luaValueWrapper& key) const;
	void setitem(size_t nIdx, const luaValueWrapper& val);
	luaValueWrapper getitem(size_t nIdx) const;
	operator CScriptValue() { return m_val; }
	void clear();
	size_t size() const;

	const CScriptValue* raw_getitem(const char* szKey);
	const CScriptValue* raw_getitem(size_t nIdx);

private:

	size_t getoffset(const luaValueWrapper& key) const;
	bool isvalid() const;

private:
	
	CScriptValue m_val;
	abase::vector<CScriptValue>& m_refKeys;
	abase::vector<CScriptValue>& m_refVals;
};


///////////////////////////////////////////////////////////////////////////
//	
//	class GFX_PLAY_INFO
//	
///////////////////////////////////////////////////////////////////////////

#define DECLARE_INFO_PROPERTY(Property) \
	static const char* Property
#define IMPL_INFO_PROPERTY(InfoType, Property) \
	const char* InfoType::Property = #Property

struct GFX_PLAY_INFO
{
public:
	GFX_PLAY_INFO(const luaTableWrapper& table, A3DCombinedAction* pAct, A3DDevice* pDev);
	const char* GetFilePath() const { return m_info.GetFilePath(); }
	const char* GetHookName() const { return m_info.GetHookName(); }
	A3DVECTOR3 GetOffSet() const { return m_info.GetOffset(); }
	bool GetRelativeToECModel() const { return m_info.RelativeToECModel(); }
	float GetYaw() const { return m_info.GetYaw(); }
	float GetPitch() const { return m_info.GetPitch(); }
	float GetRot() const { return m_info.GetRot(); }
	float GetScale() const { return m_info.GetScale(); }
	float GetAlpha() const { return m_info.GetAlpha(); }
	float GetPlaySpeed() const { return m_info.GetPlaySpeed(); }
	bool GetFadeOut() const { return m_info.CanFadeOut(); }
	bool GetBindParent() const { return m_info.IsBindParent(); }
	bool GetUseModelAlpha() const { return m_info.UseModelAlpha(); }
	bool GetIsOnce() const { return m_info.IsOnce(); }
	int GetTimeSpan() const { return m_info.GetTimeSpan(); }
	bool Start(const char* szName, A3DCombActDynData* pDynAct);

private:
// Gfx Play Info Index
	DECLARE_INFO_PROPERTY(GPI_FilePath);
	DECLARE_INFO_PROPERTY(GPI_HookName);
	DECLARE_INFO_PROPERTY(GPI_OffSet);
	DECLARE_INFO_PROPERTY(GPI_RelativeToECM);
	DECLARE_INFO_PROPERTY(GPI_Yaw);
	DECLARE_INFO_PROPERTY(GPI_Pitch);
	DECLARE_INFO_PROPERTY(GPI_Rot);
	DECLARE_INFO_PROPERTY(GPI_Scale);
	DECLARE_INFO_PROPERTY(GPI_Alpha);
	DECLARE_INFO_PROPERTY(GPI_PlaySpeed);
	DECLARE_INFO_PROPERTY(GPI_FadeOut);
	DECLARE_INFO_PROPERTY(GPI_BindParent);
	DECLARE_INFO_PROPERTY(GPI_UseModelAlpha);
	DECLARE_INFO_PROPERTY(GPI_IsOnce);
	DECLARE_INFO_PROPERTY(GPI_TimeSpan);

private:
	GFX_INFO m_info;
};

///////////////////////////////////////////////////////////////////////////
//	
//	class SFX_PLAY_INFO
//	
///////////////////////////////////////////////////////////////////////////

struct SFX_PLAY_INFO
{
public:

	SFX_PLAY_INFO(const luaTableWrapper& table, A3DCombinedAction* pAct, A3DDevice* pDev);
	const char* GetFilePath() const { return m_info.GetFilePath(); }
	const char* GetHookName() const { return m_info.GetHookName(); }
	bool GetIsForce2D() const { return m_info.GetSoundParamInfo().GetForce2D(); }
	int GetVolume() const { return m_info.GetSoundParamInfo().GetRandVolume(); }
	float GetMinDist() const { return m_info.GetSoundParamInfo().GetMinDist(); }
	float GetMaxDist() const { return m_info.GetSoundParamInfo().GetMaxDist(); }
	bool GetIsFadeOut() const { return m_info.CanFadeOut(); }
	bool GetIsBindParent() const { return m_info.IsBindParent(); }
	bool GetIsOnce() const { return m_info.IsOnce(); }
	bool Start(const char* szName, A3DCombActDynData* pAct);

private:
// sfx Play Info Index
	DECLARE_INFO_PROPERTY(SPI_FilePath);
	DECLARE_INFO_PROPERTY(SPI_HookName);
	DECLARE_INFO_PROPERTY(SPI_IsForce2D);
	DECLARE_INFO_PROPERTY(SPI_IsLoop);
	DECLARE_INFO_PROPERTY(SPI_Volume);
	DECLARE_INFO_PROPERTY(SPI_IsAbsoluteVolume);
	DECLARE_INFO_PROPERTY(SPI_MinDist);
	DECLARE_INFO_PROPERTY(SPI_MaxDist);
	DECLARE_INFO_PROPERTY(SPI_IsFadeOut);
	DECLARE_INFO_PROPERTY(SPI_IsBindParent);
	DECLARE_INFO_PROPERTY(SPI_IsOnce);
	DECLARE_INFO_PROPERTY(SPI_Pitch);
	DECLARE_INFO_PROPERTY(SPI_FixSpeed);
	DECLARE_INFO_PROPERTY(SPI_SilentHeader);
	DECLARE_INFO_PROPERTY(SPI_ParentEvent);

private:
	SFX_INFO m_info;
	EVENT_INFO *m_pParentEvent;
};

///////////////////////////////////////////////////////////////////////////
//	
//	class CHILDACT_PLAY_INFO
//	
///////////////////////////////////////////////////////////////////////////

struct CHILDACT_PLAY_INFO
{
public:

	CHILDACT_PLAY_INFO(const luaTableWrapper& table, A3DCombinedAction* pAct, A3DDevice* pDev);
	bool Start(const char* szName, A3DCombActDynData* pAct);

private:
// sfx Play Info Index
	DECLARE_INFO_PROPERTY(CAI_ChildActName);
	DECLARE_INFO_PROPERTY(CAI_HookName);
	DECLARE_INFO_PROPERTY(CAI_ChildActTime);

private:
	ChildActInfo m_info;
};

//////////////////////////////////////////////////////////////////////////
//
//	class MODEL_SCALE_CHANGE_PLAY_INFO
//
//////////////////////////////////////////////////////////////////////////

struct BONE_SCALE_CHANGE_PLAY_INFO
{
public:

	BONE_SCALE_CHANGE_PLAY_INFO(const luaTableWrapper& table, A3DCombinedAction* pAct, A3DDevice* pDev);
	bool Start(const char* szName, A3DCombActDynData* pDynAct);

private:

	DECLARE_INFO_PROPERTY(BSI_BoneScales);
	DECLARE_INFO_PROPERTY(BSI_TimeSpan);

	DECLARE_INFO_PROPERTY(BSP_ScaleType);
	DECLARE_INFO_PROPERTY(BSP_StartScale);
	DECLARE_INFO_PROPERTY(BSP_Dests);

	DECLARE_INFO_PROPERTY(DST_Scale);
	DECLARE_INFO_PROPERTY(DST_Factor);
	DECLARE_INFO_PROPERTY(DST_Time);

private:

	bool RetriveBoneScaleParams(const luaValueWrapper& v);
	bool m_bInitSucceed;
	BoneScaleChange m_info;
};

#endif	// _ECMODELLUA_H_