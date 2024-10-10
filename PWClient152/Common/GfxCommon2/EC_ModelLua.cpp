#include "StdAfx.h"
#include "EC_ModelLua.h"
#include "A3DCombActDynData.h"
#include <algorithm>


//////////////////////////////////////////////////////////////////////////
//
//	Local Functions
//
//////////////////////////////////////////////////////////////////////////

bool is_empty(const CScriptValue* pSV)
{
	if (!pSV || pSV->m_Type == CScriptValue::SVT_INVALID)
		return true;

	return false;
}

float toFloat(const CScriptValue& sv)
{
	if (sv.m_Type != CScriptValue::SVT_NUMBER)
	{
		assert(false && "Trying to apply one non-number type script value to toFloat");
		return 0.f;
	}
	
	return static_cast<float>(sv.GetDouble());
}

int toInteger(const CScriptValue& sv)
{
	if (sv.m_Type != CScriptValue::SVT_NUMBER)
	{
		ASSERT(FALSE && "Trying to apply one non-number type script value to toInteger");
		return 0;
	}
	
	return sv.GetInt();
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement luaValueWrapper
//
//////////////////////////////////////////////////////////////////////////


bool luaValueWrapper::is_equal(const CScriptValue& lhs, const CScriptValue& rhs)
{
	if (lhs.m_Type != rhs.m_Type) return false;
	switch(rhs.m_Type)
	{
	case CScriptValue::SVT_ARRAY:
		return is_equal_array(lhs, rhs);
	case CScriptValue::SVT_BOOL:
		return lhs.GetBool() == rhs.GetBool();
	case CScriptValue::SVT_STRING:
		return is_equal_string(lhs, rhs);
	case CScriptValue::SVT_NUMBER:
		return lhs.GetDouble() == rhs.GetDouble();
	case CScriptValue::SVT_USERDATA:
		return lhs.GetUserData() == rhs.GetUserData();
	default:
		assert(FALSE);
	}
	return false;
}

bool luaValueWrapper::is_equal_array(const CScriptValue& lhs, const CScriptValue& rhs)
{
	return std::equal(lhs.m_ArrKey.begin(), lhs.m_ArrKey.end(), rhs.m_ArrKey.begin())
		&& std::equal(lhs.m_ArrVal.begin(), lhs.m_ArrVal.end(), rhs.m_ArrVal.begin());
}

bool luaValueWrapper::is_equal_string(const CScriptValue& lhs, const CScriptValue& rhs)
{
	return strcmp(lhs.m_String.GetUtf8(), rhs.m_String.GetUtf8()) == 0;
}


luaValueWrapper::luaValueWrapper(const bool& bVal)
: m_val(bVal) {}

luaValueWrapper::luaValueWrapper(const double& dbVal)
: m_val(dbVal) {}

luaValueWrapper::luaValueWrapper(const unsigned int& nVal)
: m_val((double)nVal) {}

luaValueWrapper::luaValueWrapper(const AString& str)
{
	initStrUtf8(str, str.GetLength());
}

luaValueWrapper::luaValueWrapper(const char* szBuf)
{
	initStrUtf8(szBuf, strlen(szBuf));
}

luaValueWrapper::luaValueWrapper(void* pVoid)
: m_val(pVoid) {}

luaValueWrapper::luaValueWrapper(const CScriptValue& sv)
{
	copy(sv);	
}

luaValueWrapper::luaValueWrapper(const luaValueWrapper& rhs)
{
	copy(rhs.m_val);
}

luaValueWrapper& luaValueWrapper::operator = (const luaValueWrapper& rhs)
{
	if (&rhs == this)
		return *this;

	copy(rhs.m_val);
	return *this;
}

const char * luaValueWrapper::toString() const 
{ 
	if (m_val.m_Type != CScriptValue::SVT_STRING)		// for robustness
		return "";
	return m_val.m_String.GetUtf8();
}

float luaValueWrapper::toFloat() const
{
	if (m_val.m_Type != CScriptValue::SVT_NUMBER)		// for robustness
		return 0.f;
	return static_cast<float>(m_val.GetDouble());
}

bool luaValueWrapper::toBool() const
{
	if (m_val.m_Type != CScriptValue::SVT_BOOL)			// for robustness
		return false;
	return m_val.GetBool();
}

int luaValueWrapper::toInteger() const
{
	if (m_val.m_Type != CScriptValue::SVT_NUMBER)		// for robustness
		return 0;
	return m_val.GetInt();
}

CECModel* luaValueWrapper::toECM() const
{
	if (m_val.m_Type != CScriptValue::SVT_USERDATA)		// for robustness
		return NULL;
	return static_cast<CECModel*>(m_val.GetUserData());
}

EVENT_INFO* luaValueWrapper::toEventInfo() const
{
	if (m_val.m_Type != CScriptValue::SVT_USERDATA)		// for robustness
		return NULL;
	return static_cast<EVENT_INFO*>(m_val.GetUserData());
}

bool luaValueWrapper::operator == (const luaValueWrapper& rhs) const
{
	return is_equal(m_val, rhs.m_val);
}

bool luaValueWrapper::operator != (const luaValueWrapper& rhs) const
{
	return ! (*this == rhs);
}

bool luaValueWrapper::is_empty() const
{
	return m_val.m_Type == CScriptValue::SVT_INVALID;
}


void luaValueWrapper::initStrUtf8(const char* szUtf8Buf, int nLen)
{
	CScriptString str;
	str.SetUtf8(szUtf8Buf, nLen);
	m_val.SetVal(str);
}

void luaValueWrapper::copy(const CScriptValue& rhs)
{
	clear();
	m_val.m_Type = rhs.m_Type;
	switch(rhs.m_Type)
	{
	case CScriptValue::SVT_ARRAY:
		{
			m_val.m_ArrKey = rhs.m_ArrKey;
			m_val.m_ArrVal = rhs.m_ArrVal;
			break;
		}
	case CScriptValue::SVT_BOOL:
		{
			m_val.m_Bool = rhs.m_Bool;
			break;
		}
	case CScriptValue::SVT_STRING:
		{
			m_val.m_String = rhs.m_String;
			break;
		}
	case CScriptValue::SVT_NUMBER:
		{
			m_val.m_Number = rhs.m_Number;
			break;
		}
	case CScriptValue::SVT_USERDATA:
		{
			m_val.m_UserData = rhs.m_UserData;
			break;
		}
	default:
		return;
	}
}

void luaValueWrapper::clear()
{
	m_val.m_Type = CScriptValue::SVT_INVALID;
	m_val.m_ArrVal.clear();
	m_val.m_ArrKey.clear();
	m_val.m_String.Release();
	m_val.m_UserData = 0;
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement luaTableWrapper
//
//////////////////////////////////////////////////////////////////////////

luaTableWrapper::luaTableWrapper()
: m_refKeys(m_val.m_ArrKey), m_refVals(m_val.m_ArrVal) 
{
	m_val.m_Type = CScriptValue::SVT_ARRAY;
}

luaTableWrapper::luaTableWrapper(const CScriptValue& val)
: m_val(val), m_refKeys(m_val.m_ArrKey), m_refVals(m_val.m_ArrVal) 
{
	assert(isvalid());
}

luaTableWrapper::luaTableWrapper(const luaTableWrapper& table)
: m_val(table.m_val), m_refKeys(m_val.m_ArrKey), m_refVals(m_val.m_ArrVal) 
{
	assert(isvalid());
}

void luaTableWrapper::append(const luaValueWrapper& key, const luaValueWrapper& val)
{
	assert(isvalid());
	m_refKeys.push_back(key);
	m_refVals.push_back(val);
}

void luaTableWrapper::setitem(const luaValueWrapper& key, const luaValueWrapper& val)
{
	assert(isvalid());
	size_t off = getoffset(key);
	if (off == -1)
		return;
	m_refVals[off] = val;
}

luaValueWrapper luaTableWrapper::getitem(const luaValueWrapper& key) const
{
	assert(isvalid());
	size_t off = getoffset(key);
	if (off == -1)
		return CScriptValue();
	return luaValueWrapper(m_refVals[off]);
}

void luaTableWrapper::setitem(size_t nIdx, const luaValueWrapper& val)
{
	assert(isvalid() && nIdx >= 0 && nIdx < m_refKeys.size());
	m_refVals[nIdx] = val;
}

luaValueWrapper luaTableWrapper::getitem(size_t nIdx) const
{
	assert(isvalid() && nIdx >= 0 && nIdx < m_refKeys.size());
	return luaValueWrapper(m_refVals[nIdx]);
}

void luaTableWrapper::clear()
{
	assert(isvalid());
	m_refKeys.clear();
	m_refVals.clear();
}

size_t luaTableWrapper::size() const 
{
	assert(isvalid()); 
	return m_refKeys.size(); 
}

size_t luaTableWrapper::getoffset(const luaValueWrapper& key) const
{
	CScriptValue val = key;
	abase::vector<CScriptValue>::const_iterator itr = std::find(m_refKeys.begin(), m_refKeys.end(), val);
	if (itr == m_refKeys.end())
		return -1;
	return itr - m_refKeys.begin();
}

bool luaTableWrapper::isvalid() const
{
	return m_val.m_Type == CScriptValue::SVT_ARRAY
		&& m_refKeys.size() == m_refVals.size();
}

const CScriptValue* luaTableWrapper::raw_getitem(const char* szKey)
{
	assert(m_refKeys.size() == m_refVals.size());
	for (abase::vector<CScriptValue>::const_iterator itr = m_refKeys.begin()
		; itr != m_refKeys.end()
		; ++itr)
	{
		const CScriptValue& v = (*itr);
		if (v.m_Type != CScriptValue::SVT_STRING)
			continue;

		if (strcmp(v.m_String.GetUtf8(), szKey) == 0)
			return &m_refVals[itr - m_refKeys.begin()];
	}

	return NULL;
}

const CScriptValue* luaTableWrapper::raw_getitem(size_t nIdx)
{
	assert(nIdx <= m_refVals.size());
	return &m_refVals[nIdx];
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement GFX_PLAY_INFO
//	
///////////////////////////////////////////////////////////////////////////

IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_FilePath);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_HookName);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_OffSet);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_RelativeToECM);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_Yaw);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_Pitch);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_Rot);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_Scale);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_Alpha);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_PlaySpeed);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_FadeOut);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_BindParent);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_UseModelAlpha);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_IsOnce);
IMPL_INFO_PROPERTY(GFX_PLAY_INFO, GPI_TimeSpan);

GFX_PLAY_INFO::GFX_PLAY_INFO(const luaTableWrapper& table, A3DCombinedAction* pAct, A3DDevice* pDev)
: m_info(pAct)
{
	m_info.Init(pDev);
	luaValueWrapper v = table.getitem(GPI_FilePath);
	if (!v.is_empty())
		m_info.SetFilePath(v.toString());
	
	v = table.getitem(GPI_HookName);
	if (!v.is_empty())
		m_info.SetHookName(v.toString());
	
	v = table.getitem(GPI_OffSet);
	if (!v.is_empty()) {
		luaTableWrapper off(v);
		if (off.size() == 3) {
			m_info.SetOffset(A3DVECTOR3(off.getitem(0).toFloat(), off.getitem(1).toFloat(), off.getitem(2).toFloat()));
		}
	}
	
	v = table.getitem(GPI_RelativeToECM);
	if (!v.is_empty()) 
		m_info.SetRelativeToECModel(v.toBool());
	
	v = table.getitem(GPI_Yaw);
	if (!v.is_empty())
		m_info.SetYaw(v.toFloat());
	
	v = table.getitem(GPI_Pitch);
	if (!v.is_empty())
		m_info.SetPitch(v.toFloat());
	
	v = table.getitem(GPI_Rot);
	if (!v.is_empty())
		m_info.SetRot(v.toFloat());
	
	v = table.getitem(GPI_Scale);
	if (!v.is_empty())
		m_info.SetScale(v.toFloat());
	
	v = table.getitem(GPI_Alpha);
	if (!v.is_empty())
		m_info.SetAlpha(v.toFloat());
	
	v = table.getitem(GPI_PlaySpeed);
	if (!v.is_empty())
		m_info.SetPlaySpeed(v.toFloat());
	
	v = table.getitem(GPI_FadeOut);
	if (!v.is_empty())
		m_info.SetFadeOut(v.toBool());
	
	v = table.getitem(GPI_BindParent);
	if (!v.is_empty())
		m_info.SetBindParent(v.toBool());
	
	v = table.getitem(GPI_UseModelAlpha);
	if (!v.is_empty())
		m_info.SetModelAlpha(v.toBool());
	
	v = table.getitem(GPI_IsOnce);
	if (!v.is_empty())
		m_info.SetOnce(v.toBool());
	
	v = table.getitem(GPI_TimeSpan);
	if (!v.is_empty())
		m_info.SetTimeSpan(v.toInteger());
	
	m_info.BuildTranMat();
}

bool GFX_PLAY_INFO::Start(const char* szName, A3DCombActDynData* pDynAct) 
{
	m_info.SetStartTime(pDynAct->GetCurTimeSpan());
	pDynAct->AddDynEventInfo(szName, &m_info);
	return true;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement SFX_PLAY_INFO
//	
///////////////////////////////////////////////////////////////////////////

IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_FilePath);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_HookName);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_IsForce2D);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_IsLoop);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_Volume);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_IsAbsoluteVolume);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_MinDist);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_MaxDist);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_IsFadeOut);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_IsBindParent);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_IsOnce);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_Pitch);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_FixSpeed);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_SilentHeader);
IMPL_INFO_PROPERTY(SFX_PLAY_INFO, SPI_ParentEvent);

SFX_PLAY_INFO::SFX_PLAY_INFO(const luaTableWrapper& table, A3DCombinedAction* pAct, A3DDevice* pDev)
: m_info(pAct)
{
	m_info.Init(pDev);
	luaValueWrapper v = table.getitem(SPI_FilePath);
	if (!v.is_empty())
		m_info.SetFilePath(v.toString());
	
	v = table.getitem(SPI_HookName);
	if (!v.is_empty())
		m_info.SetHookName(v.toString());
	
	v = table.getitem(SPI_IsForce2D);
	if (!v.is_empty())
		m_info.GetSoundParamInfo().SetForce2D(v.toBool());
	
	v = table.getitem(SPI_Volume);
	if (!v.is_empty())
		m_info.GetSoundParamInfo().SetVolume(v.toInteger());

	v = table.getitem(SPI_IsAbsoluteVolume);
	if (!v.is_empty())
		m_info.GetSoundParamInfo().SetIsAbsoluteVolume(v.toBool());

	v = table.getitem(SPI_Pitch);
	if (!v.is_empty())
		m_info.GetSoundParamInfo().SetSoundPitch(v.toFloat(), v.toFloat());
	
	v = table.getitem(SPI_MinDist);
	if (!v.is_empty())
		m_info.GetSoundParamInfo().SetMinDist(v.toFloat());
	
	v = table.getitem(SPI_MaxDist);
	if (!v.is_empty())
		m_info.GetSoundParamInfo().SetMaxDist(v.toFloat());
	
	v = table.getitem(SPI_IsFadeOut);
	if (!v.is_empty())
		m_info.SetFadeOut(v.toBool());
	
	v = table.getitem(SPI_IsBindParent);
	if (!v.is_empty())
		m_info.SetBindParent(v.toBool());
	
	v = table.getitem(SPI_IsOnce);
	if (!v.is_empty())
		m_info.SetOnce(v.toBool());

	v = table.getitem(SPI_FixSpeed);
	if (!v.is_empty())
		m_info.GetSoundParamInfo().SetFixSpeed(v.toBool());

	v = table.getitem(SPI_SilentHeader);
	if (!v.is_empty())
		m_info.GetSoundParamInfo().SetSilentHeader(v.toInteger());

	v = table.getitem(SPI_ParentEvent);
	if (!v.is_empty())
		m_pParentEvent = v.toEventInfo();
	else
		m_pParentEvent = NULL;

	m_info.BuildTranMat();
}

bool SFX_PLAY_INFO::Start(const char* szName, A3DCombActDynData* pAct) 
{
	if (m_pParentEvent)
		m_info.SetStartTime(m_pParentEvent->GetStartTime());
	else
		m_info.SetStartTime(pAct->GetCurTimeSpan());
	pAct->AddDynEventInfo(szName, &m_info);
	return true;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CHILDACT_PLAY_INFO
//	
///////////////////////////////////////////////////////////////////////////

IMPL_INFO_PROPERTY(CHILDACT_PLAY_INFO, CAI_ChildActName);
IMPL_INFO_PROPERTY(CHILDACT_PLAY_INFO, CAI_HookName);
IMPL_INFO_PROPERTY(CHILDACT_PLAY_INFO, CAI_ChildActTime);

CHILDACT_PLAY_INFO::CHILDACT_PLAY_INFO(const luaTableWrapper& table, A3DCombinedAction* pAct, A3DDevice* pDev)
: m_info(pAct)
{
	m_info.Init(pDev);
	luaValueWrapper v = table.getitem(CAI_ChildActName);
	if (!v.is_empty())
		m_info.SetActName(v.toString());
	
	v = table.getitem(CAI_HookName);
	if (!v.is_empty())
		m_info.SetHHName(v.toString());
	
	// we do not allow trail be created by script
	m_info.SetIsTrailAct(false);
	
	v = table.getitem(CAI_ChildActTime);
	if (!v.is_empty())
		m_info.SetTrailTmSpan(v.toInteger());
}

bool CHILDACT_PLAY_INFO::Start(const char* szName, A3DCombActDynData* pAct) 
{
	//pAct->AddDynEventInfo(szName, &m_info);
	m_info.Start(pAct);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement BONE_SCALE_CHANGE_PLAY_INFO
//
//////////////////////////////////////////////////////////////////////////

IMPL_INFO_PROPERTY(BONE_SCALE_CHANGE_PLAY_INFO, BSI_BoneScales);
IMPL_INFO_PROPERTY(BONE_SCALE_CHANGE_PLAY_INFO, BSI_TimeSpan);

IMPL_INFO_PROPERTY(BONE_SCALE_CHANGE_PLAY_INFO, BSP_ScaleType);
IMPL_INFO_PROPERTY(BONE_SCALE_CHANGE_PLAY_INFO, BSP_StartScale);
IMPL_INFO_PROPERTY(BONE_SCALE_CHANGE_PLAY_INFO, BSP_Dests);

IMPL_INFO_PROPERTY(BONE_SCALE_CHANGE_PLAY_INFO, DST_Scale);
IMPL_INFO_PROPERTY(BONE_SCALE_CHANGE_PLAY_INFO, DST_Factor);
IMPL_INFO_PROPERTY(BONE_SCALE_CHANGE_PLAY_INFO, DST_Time);

BONE_SCALE_CHANGE_PLAY_INFO::BONE_SCALE_CHANGE_PLAY_INFO(const luaTableWrapper& table, A3DCombinedAction* pAct, A3DDevice* pDev)
: m_info(pAct)
, m_bInitSucceed(false)
{
	m_info.Init(pDev);
	
	luaValueWrapper v = table.getitem(BSI_TimeSpan);
	if (!v.is_empty())
		m_info.SetTimeSpan(v.toInteger());

	v = table.getitem(BSI_BoneScales);
	if (!v.is_empty())
	{
		if (RetriveBoneScaleParams(v))
			m_bInitSucceed = true;
	}

}

bool BONE_SCALE_CHANGE_PLAY_INFO::RetriveBoneScaleParams(const luaValueWrapper& v)
{
	const CScriptValue& sv = v.get();
	if (sv.m_Type != CScriptValue::SVT_ARRAY)
	{
		a_LogOutput(1, "Error in BONE_SCALE_CHANGE_PLAY_INFO::RetriveBoneScaleParams, BSI_BoneScales is not a table.");
		return false;
	}

	if (sv.m_ArrKey.size() != sv.m_ArrVal.size())
	{
		a_LogOutput(1, "Error in BONE_SCALE_CHANGE_PLAY_INFO::RetriveBoneScaleParams, bone scale table is not valid.");
		return false;
	}

	for (size_t nIdx = 0; nIdx < sv.m_ArrKey.size(); ++nIdx)
	{
		const CScriptValue& vCurKey = sv.m_ArrKey.at(nIdx);
		if (vCurKey.m_Type != CScriptValue::SVT_STRING)
		{
			a_LogOutput(1, "Error in BONE_SCALE_CHANGE_PLAY_INFO::RetriveBoneScaleParams, wrong bone scale key, expected a bone name. at(%d)", nIdx);
			continue;
		}

		const CScriptValue& vCurVal = sv.m_ArrVal.at(nIdx);
		if (vCurVal.m_Type != CScriptValue::SVT_ARRAY)
		{
			a_LogOutput(1, "Error in BONE_SCALE_CHANGE_PLAY_INFO::RetriveBoneScaleParams, wrong bone scale value, expected a table. at(%d)", nIdx);
			continue;
		}

		luaTableWrapper vTabWrapper(vCurVal);

		BoneScaleChange::BoneScaleParam Param;
		Param.m_strBone = vCurKey.m_String.GetUtf8();

		const CScriptValue* pSV = vTabWrapper.raw_getitem(BSP_StartScale);
		if (!is_empty(pSV))
			Param.m_fStartScale = toFloat(*pSV);
		else
			Param.m_fStartScale = 1.0f;

		pSV = vTabWrapper.raw_getitem(BSP_ScaleType);
		if (!is_empty(pSV))
			Param.m_iScaleType = toInteger(*pSV);
		else
			Param.m_iScaleType = CECModel::SCALE_WHOLE;

		pSV = vTabWrapper.raw_getitem(BSP_Dests);
		if (is_empty(pSV))
			continue;

		luaTableWrapper vDestsTabWrapper(*pSV);
		size_t nDestNum = a_Min(vDestsTabWrapper.size(), static_cast<size_t>(BoneScaleChange::SCALE_DEST_NUM));
		for (size_t nDestIdx = 0; nDestIdx < nDestNum; ++nDestIdx)
		{
			const CScriptValue* pTabVal = vDestsTabWrapper.raw_getitem(nDestIdx);
			if (is_empty(pTabVal))
				return false;
			
			luaTableWrapper vDest(*pTabVal);
			const CScriptValue* pDstVal = NULL;
			pDstVal = vDest.raw_getitem(DST_Scale);
			if (is_empty(pDstVal))
				return false;
			Param.m_aScaleArray[nDestIdx].m_fDestScale = toFloat(*pDstVal);

			pDstVal = vDest.raw_getitem(DST_Factor);
			if (is_empty(pDstVal))
				return false;
			Param.m_aScaleArray[nDestIdx].m_fScaleFactor = toFloat(*pDstVal);
			
			pDstVal = vDest.raw_getitem(DST_Time);
			if (is_empty(pDstVal))
				return false;
			Param.m_aScaleArray[nDestIdx].m_dwScaleTime = static_cast<DWORD>(toFloat(*pDstVal));
		}

		Param.m_iDestNum = nDestNum;
		m_info.AddBoneScaleParam(Param);
	}

	return true;
}

bool BONE_SCALE_CHANGE_PLAY_INFO::Start(const char* szName, A3DCombActDynData* pDynAct)
{
	if (!m_bInitSucceed)
		return false;
	
	m_info.SetStartTime(pDynAct->GetCurTimeSpan());
	pDynAct->AddDynEventInfo(szName, &m_info);
	return true;
}