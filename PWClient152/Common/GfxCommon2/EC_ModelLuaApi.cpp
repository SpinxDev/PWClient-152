#include "StdAfx.h"
#include "LuaUtil.h"
#include "scriptvalue.h"
#include "LuaAPI.h"
#include "LuaState.h"
#include "EC_ModelLua.h"
#include "EC_ModelBlur.h"

/*
global function for lua
*/
using namespace LuaBind;

void ECM_ShowChildModel(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() == 3 && (args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING 
		|| args[2].m_Type != CScriptValue::SVT_BOOL) )
	{
		LUA_DEBUG_INFO("ECM_ShowChildModel, wrong args\n");
		return;
	}
	else if (args.size() == 4 && (args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING 
		|| args[2].m_Type != CScriptValue::SVT_BOOL))
	{
		LUA_DEBUG_INFO("ECM_ShowChildModel, wrong args\n");
		return;
	}

	if (args.size() != 3 && args.size() != 4)
	{
		LUA_DEBUG_INFO("ECM_ShowChildModel, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString szHangerName;
	bool bShow;
	bool bUpdateWhenHidden = false;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(szHangerName);
	bShow = args[2].GetBool();

	if (args.size() > 3)
	{
		bUpdateWhenHidden = args[3].GetBool();
	}

	pModel->ShowChildModel(szHangerName, bShow, bUpdateWhenHidden);
}
IMPLEMENT_SCRIPT_API(ECM_ShowChildModel);

void ECM_SetBoneTransFlag(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING 
		|| args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		LUA_DEBUG_INFO("ECM_SetBoneTransFlag, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString strBone;
	bool bFlag;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strBone);
	bFlag = args[2].GetBool();

	pModel->SetBoneTransFlag(strBone, bFlag);
}
IMPLEMENT_SCRIPT_API(ECM_SetBoneTransFlag);



void ECM_ChildChangeHook(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING 
		|| args[2].m_Type != CScriptValue::SVT_STRING )
	{
		LUA_DEBUG_INFO("ECM_ChildChangeHook, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString strHanger;
	AString strNewHook;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHanger);
	args[2].RetrieveUtf8(strNewHook);
	pModel->ChildChangeHook(strHanger, strNewHook);
}
IMPLEMENT_SCRIPT_API(ECM_ChildChangeHook);

void ECM_SetUseAbsTrack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_BOOL)
	{
		LUA_DEBUG_INFO("ECM_SetUseAbsTrack, wrong args\n");
		return;
	}
	CECModel* pModel;
	bool bUse;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	bUse = args[1].GetBool();

	pModel->SetUseAbsTrack(bUse);
}
IMPLEMENT_SCRIPT_API(ECM_SetUseAbsTrack);

void ECM_RegisterAbsTrackOfBone(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_RegisterAbsTrackOfBone, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString strBone;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strBone);
	pModel->RegisterAbsTrackOfBone(strBone);
}
IMPLEMENT_SCRIPT_API(ECM_RegisterAbsTrackOfBone);

void ECM_PlayGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() < 3 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_PlayGfx, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString strHook;
	AString strGfx;
	float fScale;
	bool bUseECMHook;
	DWORD dwFadeOutTime;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);

	if (args.size() > 3 && args[3].m_Type == CScriptValue::SVT_NUMBER)
		fScale = (float)args[3].GetDouble();
	else
		fScale = 1.0f;

	if (args.size() > 4 && args[4].m_Type == CScriptValue::SVT_BOOL)
		bUseECMHook = args[4].GetBool();
	else
		bUseECMHook = false;

	//	说明：以前的时候虽然bFadeOut传true，但是由于GFX_INFO中(析构函数)不支持淡出
	//  所以实际上并不存在淡出功能，从当前版本开始(2011.2.24)，GFX_INFO支持淡出了，同时新增了脚本参数用于控制淡出
	//	为了保留以前的行为，我们让该参数在没有设置的情况下还是默认为0，从而保持没有淡出的行为
	if (args.size() > 5 && args[5].m_Type == CScriptValue::SVT_NUMBER)
		dwFadeOutTime = args[5].GetInt();
	else
		dwFadeOutTime = 0;

	pModel->PlayGfx(strGfx, strHook, fScale, true, bUseECMHook, dwFadeOutTime);
}
IMPLEMENT_SCRIPT_API(ECM_PlayGfx);

void ECM_PlayGfxEx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() < 9 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING
		|| args[3].m_Type != CScriptValue::SVT_NUMBER
		|| args[4].m_Type != CScriptValue::SVT_NUMBER
		|| args[5].m_Type != CScriptValue::SVT_NUMBER
		|| args[6].m_Type != CScriptValue::SVT_NUMBER
		|| args[7].m_Type != CScriptValue::SVT_NUMBER
		|| args[8].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_PlayGfxEx, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString strHook;
	AString strGfx;
	double fOffsetx;
	double fOffsety;
	double fOffsetz;
	double fPitch;
	double fYaw;
	double fRot;
	float fScale;
	bool bUseECMHook;
	DWORD dwFadeOutTime;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);
	fOffsetx = args[3].GetDouble();
	fOffsety = args[4].GetDouble();
	fOffsetz = args[5].GetDouble();
	fPitch	 = args[6].GetDouble();
	fYaw	 = args[7].GetDouble();
	fRot	 = args[8].GetDouble();

	if (args.size() > 9 && args[9].m_Type == CScriptValue::SVT_NUMBER)
		fScale = (float)args[9].GetDouble();
	else
		fScale = 1.0f;

	if (args.size() > 10 && args[10].m_Type == CScriptValue::SVT_BOOL)
		bUseECMHook = args[10].GetBool();
	else
		bUseECMHook = false;

	//	参考ECM_PlayGfx处本参数的注释
	if (args.size() > 11 && args[11].m_Type == CScriptValue::SVT_NUMBER)
		dwFadeOutTime = args[11].GetInt();
	else
		dwFadeOutTime = 0;

	pModel->PlayGfx(strGfx
		, strHook
		, fScale
		, true
		, A3DVECTOR3(static_cast<float>(fOffsetx), static_cast<float>(fOffsety), static_cast<float>(fOffsetz))
		, static_cast<float>(fPitch)
		, static_cast<float>(fYaw)
		, static_cast<float>(fRot)
		, bUseECMHook
		, dwFadeOutTime);
}
IMPLEMENT_SCRIPT_API(ECM_PlayGfxEx);

int ECM_RemoveGfx(lua_State * L)
{
	CECModel* pModel;
	AString strHook;
	AString strGfx;

	if (!CheckValue(TypeWrapper<LuaUserData>(), L, 1, (void*&)pModel))
		return 0;

	if (!CheckValue(TypeWrapper<const char*>(), L, 2, strHook))
		return 0;

	if (!CheckValue(TypeWrapper<const char*>(), L, 3, strGfx))
		return 0;

	pModel->RemoveGfx(strGfx, strHook);
	return 0;
}

void ECM_RemoveGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_RemoveGfx, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString strHook;
	AString strGfx;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);

	pModel->RemoveGfx(strGfx, strHook);
}
IMPLEMENT_SCRIPT_API(ECM_RemoveGfx);

void logScriptValue(const CScriptValue& sv)
{
	switch(sv.m_Type)
	{
	case CScriptValue::SVT_BOOL:
		a_LogOutput(1, "Bool: %s", sv.GetBool() ? "True" : "False");
		break;
	case CScriptValue::SVT_NUMBER:
		a_LogOutput(1, "Double: %f", sv.GetDouble());
		break;
	case CScriptValue::SVT_STRING: 
		a_LogOutput(1, "String: %s", (LPCSTR)sv.m_String.GetUtf8());
		break;
	case CScriptValue::SVT_USERDATA:
		a_LogOutput(1, "UserData: %x", sv.GetUserData());
		break;
	default:
		a_LogOutput(1, "Unknown type of scriptvalue");
		break;
	}
}

// this functions is used as an example
void ECM_ArgPassIn(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ArgPassIn, wrong args\nArg 1st: Pointer to ECModel\nArg 2nd: A table");
		return;
	}
	CECModel* pModel;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	const abase::vector<CScriptValue>& keys = args[1].m_ArrKey;
	const abase::vector<CScriptValue>& array = args[1].m_ArrVal;

	bool bPrintKeys = false;
	if (keys.size() == array.size())
		bPrintKeys = true;

	for (size_t nIdx = 0; nIdx < array.size(); ++nIdx)
	{
		if (bPrintKeys) {
			a_LogOutput(1, "Key:");
			logScriptValue(keys[nIdx]);
		}
		a_LogOutput(1, "Value:");
		logScriptValue(array[nIdx]);
	}
}
IMPLEMENT_SCRIPT_API(ECM_ArgPassIn);

// this functions is used as an example
void ECM_ArgPassOut(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 1 || args[0].m_Type != CScriptValue::SVT_USERDATA)
	{
		LUA_DEBUG_INFO("ECM_ArgPassOut, wrong args\nArg 1st: Pointer to ECModel");
		return;
	}

	CECModel* pModel;
	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);

	luaTableWrapper luatab;
	luatab.append("ModFilePath", pModel->GetFilePath());
	unsigned int BoneScaleCount = pModel->GetBoneScaleExArr().size();
	luatab.append("BoneScaleCount", BoneScaleCount);

	results.clear();
	results.push_back(luatab);
}
IMPLEMENT_SCRIPT_API(ECM_ArgPassOut);

void ECM_ActPlayGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_STRING
		|| args[1].m_Type != CScriptValue::SVT_USERDATA
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ActPlayGfx, wrong args\nArg 1st: user defined unique name\nArg 2nd: act channel\nArg 3rd: pointer to ECModel\nArg 4th: table of params");
		return;
	}

	const char* szName = args[0].m_String.GetUtf8();
	if (!szName || !strlen(szName))
		return;

	CECModel* pModel;
	pModel = (CECModel *)args[1].GetUserData();
	assert(pModel);

	int nChannel = args[2].GetInt();

	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	if (!pAct)
		return;

	luaTableWrapper luatab(args[3]);
	GFX_PLAY_INFO gpi(luatab, pAct->GetComAct(), AfxGetA3DDevice());
	gpi.Start(szName, pAct);
}
IMPLEMENT_SCRIPT_API(ECM_ActPlayGfx);

void ECM_ActPlaySfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_STRING
		|| args[1].m_Type != CScriptValue::SVT_USERDATA
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ActPlaySfx, wrong args\nArg 1st: user defined unique name\nArg 2nd: act channel\nArg 3rd: pointer to ECModel\nArg 4th: table of params");
		return;
	}

	const char* szName = args[0].m_String.GetUtf8();
	if (!szName || !strlen(szName))
		return;

	CECModel* pModel;
	pModel = (CECModel *)args[1].GetUserData();
	assert(pModel);

	int nChannel = args[2].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	if (!pAct)
		return;

	luaTableWrapper luatab(args[3]);
	SFX_PLAY_INFO spi(luatab, pAct->GetComAct(), AfxGetA3DDevice());
	spi.Start(szName, pAct);
}
IMPLEMENT_SCRIPT_API(ECM_ActPlaySfx);

void ECM_ActPlayChildAct(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_STRING
		|| args[1].m_Type != CScriptValue::SVT_USERDATA
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ActPlayChildAct, wrong args\nArg 1st: user defined unique name\nArg 2nd: act channel\nArg 3rd: pointer to ECModel\nArg 4th: table of params");
		return;
	}

	const char* szName = args[0].m_String.GetUtf8();
	if (!szName || !strlen(szName))
		return;

	CECModel* pModel;
	pModel = (CECModel *)args[1].GetUserData();
	assert(pModel);

	int nChannel = args[2].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	if (!pAct)
		return;

	luaTableWrapper luatab(args[3]);
	CHILDACT_PLAY_INFO capi(luatab, pAct->GetComAct(), AfxGetA3DDevice());
	capi.Start(szName, pAct);
}
IMPLEMENT_SCRIPT_API(ECM_ActPlayChildAct);

void ECM_SetAlpha(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA  || args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_SetAlpha, wrong args\n");
		return;
	}

	CECModel* pModel;
	float fAlpha;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	fAlpha = (float)args[1].GetDouble();
	pModel->SetTransparent(1.0f - fAlpha);
}
IMPLEMENT_SCRIPT_API(ECM_SetAlpha);

void ECM_SetColor(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_USERDATA || args[1].m_Type != CScriptValue::SVT_NUMBER
		|| args[2].m_Type != CScriptValue::SVT_NUMBER || args[3].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_SetColor, wrong args\n");
		return;
	}

	CECModel* pModel;
	A3DCOLORVALUE cl;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	cl.r = (float)args[1].GetDouble();
	cl.g = (float)args[2].GetDouble();
	cl.b = (float)args[3].GetDouble();
	cl.a = 1.0f;
	pModel->SetColor(cl);
}
IMPLEMENT_SCRIPT_API(ECM_SetColor);

void ECM_HasEquip(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA  || args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_HasEquip, wrong args\n");
		return;
	}

	CECModel* pModel;
	int id;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	id = args[1].GetInt();
	results.push_back(CScriptValue(pModel->HasEquip(id)));
}
IMPLEMENT_SCRIPT_API(ECM_HasEquip);

void ECM_GetEquipId(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA  || args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_GetEquipId, wrong args\n");
		return;
	}

	CECModel* pModel;
	int index;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	index = args[1].GetInt();
	results.push_back(CScriptValue((double)pModel->GetEquipId(index)));
}
IMPLEMENT_SCRIPT_API(ECM_GetEquipId);

void ECM_HasGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() < 3 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_HasGfx, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString strHook;
	AString strGfx;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);
	results.push_back(CScriptValue(pModel->GetGfx(strGfx, strHook) != NULL));
}
IMPLEMENT_SCRIPT_API(ECM_HasGfx);

void ECM_ShowGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() < 4 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING
		|| args[3].m_Type != CScriptValue::SVT_BOOL)
	{
		LUA_DEBUG_INFO("ECM_ShowGfx, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString strHook;
	AString strGfx;
	bool bShow;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);
	bShow = args[3].GetBool();

	A3DGFXEx* pGfx = pModel->GetGfx(strGfx, strHook);

	if (pGfx)
		pGfx->SetVisible(bShow);
}
IMPLEMENT_SCRIPT_API(ECM_ShowGfx);

void ECM_GetChannelAct(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA  || args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_GetChannelAct, wrong args\n");
		return;
	}

	CECModel* pModel;
	int nChannel;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	nChannel = args[1].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	CScriptString str;

	if (pAct)
		str.SetUtf8(pAct->GetComAct()->GetName(), strlen(pAct->GetComAct()->GetName()));

	results.push_back(str);
}
IMPLEMENT_SCRIPT_API(ECM_GetChannelAct);

void ECM_GetFashionMode(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 1 || args[0].m_Type != CScriptValue::SVT_USERDATA)
	{
		LUA_DEBUG_INFO("ECM_GetFashionMode, wrong args\n");
		return;
	}

	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	results.push_back(CScriptValue(pModel->GetFashionMode()));
}
IMPLEMENT_SCRIPT_API(ECM_GetFashionMode);

void ECM_ReplaceSkinFile(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_ReplaceSkinFile, wrong args\n");
		return;
	}

	AString strSkinFile;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	int iIndex = args[1].GetInt();

	if (iIndex < 0 || iIndex >= pModel->GetSkinNum())
	{
		AString strMsg;
		strMsg.Format("ECM_ReplaceSkinFile, passed in index(%d) error, there are only %d skin exist.", iIndex, pModel->GetSkinNum());
		LUA_DEBUG_INFO(strMsg);
		return;
	}

	args[2].RetrieveUtf8(strSkinFile);

	bool bRet = pModel->ReplaceSkinFile(iIndex, strSkinFile);

	results.push_back(CScriptValue(bRet));
}
IMPLEMENT_SCRIPT_API(ECM_ReplaceSkinFile)

void ECM_GetSkinFile(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 
		|| args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_GetSkinFile, wrong args\n");
		return;
	}

	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	int iIndex = args[1].GetInt();
	if (iIndex < 0 || iIndex >= pModel->GetSkinNum())
	{
		AString strMsg;
		strMsg.Format("ECM_GetSkinFile, passed in index(%d) error, there are only %d skin exist.", iIndex, pModel->GetSkinNum());
		LUA_DEBUG_INFO(strMsg);
		return;
	}

	CScriptString str;
	A3DSkin* pSkin = pModel->GetA3DSkin(iIndex);
	if (pSkin != NULL)
		str.SetAString(pSkin->GetFileName());

	results.push_back(CScriptValue(str));
}
IMPLEMENT_SCRIPT_API(ECM_GetSkinFile)

void ECM_AddChildModelLink(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING		// hangerName
		|| args[2].m_Type != CScriptValue::SVT_STRING		// parentHookName
		|| args[3].m_Type != CScriptValue::SVT_STRING		// childAttacherName
		)
	{
		LUA_DEBUG_INFO("ECM_AddChildModelLink, wrong args\n");
		return;
	}

	AString strHanger, strParentHook, strChildAttacher;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);

	args[1].RetrieveUtf8(strHanger);
	args[2].RetrieveUtf8(strParentHook);
	args[3].RetrieveUtf8(strChildAttacher);

	bool bRet = pModel->AddChildModelLink(strHanger, strParentHook, strChildAttacher);
	results.push_back(CScriptValue(bRet));
}
IMPLEMENT_SCRIPT_API(ECM_AddChildModelLink)

void ECM_RemoveChildModelLink(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING		// hangerName
		|| args[2].m_Type != CScriptValue::SVT_STRING		// parentHookName
		|| args[3].m_Type != CScriptValue::SVT_STRING		// childAttacherName
		)
	{
		LUA_DEBUG_INFO("ECM_RemoveChildModelLink, wrong args\n");
		return;
	}

	AString strHanger, strParentHook, strChildAttacher;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);

	args[1].RetrieveUtf8(strHanger);
	args[2].RetrieveUtf8(strParentHook);
	args[3].RetrieveUtf8(strChildAttacher);

	bool bRet = pModel->RemoveChildModelLink(strHanger, strParentHook, strChildAttacher);
	results.push_back(CScriptValue(bRet));
}
IMPLEMENT_SCRIPT_API(ECM_RemoveChildModelLink)

void ECM_ResetChildModelInitPos(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING		// hangerName
		)
	{
		LUA_DEBUG_INFO("ECM_ResetChildModelInitPos, wrong args\n");
		return;
	}

	AString strHanger;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	if (!pModel)
	{
		assert(pModel);
		return;
	}

	args[1].RetrieveUtf8(strHanger);
	CECModel* pChildModel = pModel->GetChildModel(strHanger);
	if (!pChildModel)
		return;

	pChildModel->GetA3DSkinModel()->ResetToInitPose();
}
IMPLEMENT_SCRIPT_API(ECM_ResetChildModelInitPos)

void ECM_SetChildModelPhysState(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
#ifdef A3D_PHYSX

	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING		// hangerName
		|| args[2].m_Type != CScriptValue::SVT_BOOL			// true for phys state, false for animation state
		)
	{
		LUA_DEBUG_INFO("ECM_SetChildModelPhysState, wrong args\n");
		return;
	}

	AString strHanger;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	if (!pModel)
	{
		assert(pModel);
		return;
	}

	args[1].RetrieveUtf8(strHanger);
	bool bIsPhys = args[2].GetBool();

	pModel->ChangeChildModelPhysState(strHanger, bIsPhys ? A3DModelPhysSync::PHY_STATE_SIMULATE :  A3DModelPhysSync::PHY_STATE_ANIMATION);

#endif
}
IMPLEMENT_SCRIPT_API(ECM_SetChildModelPhysState)

void ECM_AddForce(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
#ifdef A3D_PHYSX

	if (args.size() < 6 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_NUMBER
		|| args[4].m_Type != CScriptValue::SVT_NUMBER
		|| args[5].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_AddForce, should take at least 6 args (model, x, y, z, force, type_name)");
		return;
	}

	A3DVECTOR3 vTarget;

	if (args.size() == 9)
	{
		if (args[6].m_Type != CScriptValue::SVT_NUMBER
			|| args[7].m_Type != CScriptValue::SVT_NUMBER
			|| args[8].m_Type != CScriptValue::SVT_NUMBER)
		{
			LUA_DEBUG_INFO("ECM_AddForce, target param wrong");
			return;
		}

		vTarget.Set((float)args[6].GetDouble(), (float)args[7].GetDouble(), (float)args[8].GetDouble());
	}
	else
		vTarget.Clear();

	CECModel* pModel = (CECModel*)args[0].GetUserData();
	A3DVECTOR3 vDir((float)args[1].GetDouble(), (float)args[2].GetDouble(), (float)args[3].GetDouble());
	float fForceMagnitude = (float)args[4].GetDouble();
	a_ClampFloor(fForceMagnitude, 0.0f);

	AString strForceType;
	args[5].RetrieveUtf8(strForceType);

	int iPhysForceType = CECModel::PFT_FORCE;
	if (strForceType.CompareNoCase("force") == 0)
		iPhysForceType = CECModel::PFT_FORCE;
	else if (strForceType.CompareNoCase("impulse") == 0)
		iPhysForceType = CECModel::PFT_IMPULSE;
	else if (strForceType.CompareNoCase("velocity") == 0)
		iPhysForceType = CECModel::PFT_VELOCITY_CHANGE;

	A3DVECTOR3 vStart = vTarget - vDir;
	vStart = pModel->GetAbsoluteTM() * vStart;
	vDir = a3d_VectorMatrix3x3(vDir, pModel->GetAbsoluteTM());
	vDir.Normalize();
	bool bRet = pModel->AddForce(vStart, vDir, fForceMagnitude, FLT_MAX, iPhysForceType);

	results.push_back(CScriptValue(bRet));

#endif
}
IMPLEMENT_SCRIPT_API(ECM_AddForce)

void ECM_ActChangeModelScale(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4
		|| args[0].m_Type != CScriptValue::SVT_STRING
		|| args[1].m_Type != CScriptValue::SVT_USERDATA
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ActChangeChildModelScale, wrong args.\n");
		return;
	}

	const char* szName = args[0].m_String.GetUtf8();
	if (!szName || !szName[0])
		return;

	CECModel* pModel;
	pModel = (CECModel *)args[1].GetUserData();
	assert(pModel);

	int nChannel = args[2].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	if (!pAct)
		return;

	BONE_SCALE_CHANGE_PLAY_INFO bone_scl_play(args[3], pAct->GetComAct(), AfxGetA3DDevice());
	bone_scl_play.Start(szName, pAct);
}
IMPLEMENT_SCRIPT_API(ECM_ActChangeModelScale)

void ECM_MotionBlur(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA || args[1].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_MotionBlur, wrong args\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	luaTableWrapper wrapper(args[1]);
	const CScriptValue* pColors				= wrapper.raw_getitem("Colors");
	const CScriptValue* pBones				= wrapper.raw_getitem("Bones");
	const CScriptValue* pTotalTime			= wrapper.raw_getitem("TotalTime");
	const CScriptValue* pInterval			= wrapper.raw_getitem("Interval");
	const CScriptValue* pApplyToChild		= wrapper.raw_getitem("ApplyToChild");
	const CScriptValue* pHighlight			= wrapper.raw_getitem("Highlight");
	const CScriptValue* pUsePS				= wrapper.raw_getitem("UsePS");
	const CScriptValue* pInChannel			= wrapper.raw_getitem("InChannel");

	std::auto_ptr<ECMActionBlurInfo> pBlurInfo(new ECMActionBlurInfo);

	if (pColors && pColors->m_ArrVal.size())
	{
		for (size_t i = 0; i < pColors->m_ArrVal.size(); i++)
		{
			const CScriptValue& c = pColors->m_ArrVal[i];

			if (c.m_ArrVal.size() == 4)
			{
				int val = (c.m_ArrVal[0].GetInt() << 24) | (c.m_ArrVal[1].GetInt() << 16) | (c.m_ArrVal[2].GetInt() << 8) |(c.m_ArrVal[3].GetInt());
				pBlurInfo->m_Colors.push_back(val);
			}
		}

		if (pBlurInfo->m_Colors.size() == 0)
			return;
	}
	else
		return;

	if (pBones)
	{
		for (size_t i = 0; i < pBones->m_ArrVal.size(); i++)
			pBlurInfo->m_BoneIndices.push_back(pBones->m_ArrVal[i].GetInt());
	}

	if (pTotalTime)
		pBlurInfo->m_dwTotalTime = pTotalTime->GetInt();
	else
		pBlurInfo->m_dwTotalTime = 0;

	if (pInterval)
		pBlurInfo->m_dwInterval = pInterval->GetInt();
	else
		pBlurInfo->m_dwInterval = 33;

	if (pApplyToChild)
		pBlurInfo->m_bApplyToChildren = pApplyToChild->GetBool();
	else
		pBlurInfo->m_bApplyToChildren = true;

	if (pHighlight)
		pBlurInfo->m_bHighlight = pHighlight->GetBool();
	else
		pBlurInfo->m_bHighlight = false;

	if (pUsePS)
	{
		AString strUsePS;
		pUsePS->RetrieveUtf8(strUsePS);
		pBlurInfo->SetUsePS(strUsePS);
	}

	if (pInChannel)
	{
		int iActChannel = pInChannel->GetInt();
		pBlurInfo->SetInChannel(iActChannel);
	}

	pBlurInfo->m_bRoot = true;
	pModel->SetMotionBlurInfo(pBlurInfo.release());
}

IMPLEMENT_SCRIPT_API(ECM_MotionBlur)

void ECM_ApplyPixelShader(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3
		|| args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_ApplyPixelShader, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	int nChannel = args[1].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);

	if (!pAct)
		return;

	const char* szPSPath = args[2].m_String.GetUtf8();
	AFileImage file;

	if (!file.Open(szPSPath, AFILE_OPENEXIST | AFILE_TEXT | AFILE_TEMPMEMORY))
		return;

	AString strPS, strTex;
	GfxPSConstVec vec;
	g_GfxLoadPixelShaderConsts(&file, strPS, strTex, vec);
	file.Close();
	pModel->SetReplaceShader(strPS, strTex, vec);
	pAct->SetResetPSWhenActStop(true);
}

IMPLEMENT_SCRIPT_API(ECM_ApplyPixelShader)

void ECM_RemovePixelShader(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2
		|| args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_RemovePixelShader, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	int nChannel = args[1].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);

	if (!pAct)
		return;

	pModel->RemoveReplaceShader();
	pAct->SetResetPSWhenActStop(false);
}

IMPLEMENT_SCRIPT_API(ECM_RemovePixelShader)

void ECM_SetActionNearestInterp(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
#ifdef _ANGELICA21

	if (args.size() != 2
		|| args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_SetActionNearestInterp, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	A3DSkinModel* pSkinModel = pModel->GetA3DSkinModel();

	if (!pSkinModel)
		return;

	const char* szAction = args[1].m_String.GetUtf8();
	A3DSkinModelActionCore* pAction = pSkinModel->GetAction(szAction);

	if (pAction)
		pAction->SetNearestInterp(true);

#endif
}

IMPLEMENT_SCRIPT_API(ECM_SetActionNearestInterp)

void ECM_SetAllActionNearestInterp(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
#ifdef _ANGELICA21

	if (args.size() != 1
		|| args[0].m_Type != CScriptValue::SVT_USERDATA)
	{
		LUA_DEBUG_INFO("ECM_SetActionNearestInterp, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	A3DSkinModel* pSkinModel = pModel->GetA3DSkinModel();

	if (!pSkinModel)
		return;

	pSkinModel->SetAllActionNearestInterp();

#endif
}

IMPLEMENT_SCRIPT_API(ECM_SetAllActionNearestInterp)

void ECM_ResetToInitPose(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 1
		|| args[0].m_Type != CScriptValue::SVT_USERDATA)
	{
		LUA_DEBUG_INFO("ECM_SetActionNearestInterp, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	A3DSkinModel* pSkinModel = pModel->GetA3DSkinModel();

	if (!pSkinModel)
		return;

	pSkinModel->ResetToInitPose();
}

IMPLEMENT_SCRIPT_API(ECM_ResetToInitPose)

void InitECMApi(CLuaState * pState)
{
	REGISTER_SCRIPT_API(ECMApi, ECM_ShowChildModel);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetBoneTransFlag);
	REGISTER_SCRIPT_API(ECMApi, ECM_ChildChangeHook);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetUseAbsTrack);
	REGISTER_SCRIPT_API(ECMApi, ECM_RegisterAbsTrackOfBone);
	REGISTER_SCRIPT_API(ECMApi, ECM_PlayGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_RemoveGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_PlayGfxEx);
	REGISTER_SCRIPT_API(ECMApi, ECM_ActPlayGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_ActPlaySfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_ActPlayChildAct);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetAlpha);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetColor);
	REGISTER_SCRIPT_API(ECMApi, ECM_HasEquip);
	REGISTER_SCRIPT_API(ECMApi, ECM_GetEquipId);
	REGISTER_SCRIPT_API(ECMApi, ECM_HasGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_ShowGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_GetChannelAct);
	REGISTER_SCRIPT_API(ECMApi, ECM_GetFashionMode);
	REGISTER_SCRIPT_API(ECMApi, ECM_ReplaceSkinFile);
	REGISTER_SCRIPT_API(ECMApi, ECM_GetSkinFile);
	REGISTER_SCRIPT_API(ECMApi, ECM_AddChildModelLink);
	REGISTER_SCRIPT_API(ECMApi, ECM_RemoveChildModelLink);
	REGISTER_SCRIPT_API(ECMApi, ECM_ResetChildModelInitPos);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetChildModelPhysState);
	REGISTER_SCRIPT_API(ECMApi, ECM_ActChangeModelScale);
	REGISTER_SCRIPT_API(ECMApi, ECM_MotionBlur);
	REGISTER_SCRIPT_API(ECMApi, ECM_AddForce);
	REGISTER_SCRIPT_API(ECMApi, ECM_ApplyPixelShader);
	REGISTER_SCRIPT_API(ECMApi, ECM_RemovePixelShader);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetActionNearestInterp);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetAllActionNearestInterp);
	REGISTER_SCRIPT_API(ECMApi, ECM_ResetToInitPose);

	pState->RegisterLibApi("ECMApi");
}