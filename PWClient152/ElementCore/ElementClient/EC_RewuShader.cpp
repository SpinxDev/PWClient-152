// File		: EC_RewuShader.cpp
// Creator	: Xu Wenbin
// Date		: 2014/6/20

#include "EC_RewuShader.h"
#include "EC_Game.h"

#include <AFI.h>
#include <AString.h>
#include <A3DShader.h>
#include <A3DShaderMan.h>
#include <A3DEngine.h>
#include <A3DMacros.h>

#include <hashmap.h>

extern CECGame * g_pGame;

AString CECSpecialShader::MakeTexMap2With(const char *szTexMap){	
	char result[MAX_PATH];
	af_GetFilePath(szTexMap, result, MAX_PATH);
	strcat(result, "\\nb_");
	strcat(result, GetFileTitleLowerCase(szTexMap));
	return result;
}

bool CECSpecialShader::MatchTexture(const char * szTexMap, const char * szTexMap2)const{
	if (!szTexMap || !szTexMap[0] ||
		GetFileTitleLowerCase(szTexMap).Find(GetTextureMapLowerCasePrefix()) != 0){
		return false;
	}
	if (!szTexMap2){
		return true;
	}
	if (!szTexMap2[0]){
		return false;
	}
	AString strTemp = MakeTexMap2With(szTexMap);
	return stricmp(strTemp, szTexMap2) == 0;
}

bool CECSpecialShader::MatchShader(A3DShader *p)const{
	return p != NULL && strstr(p->GetMapFile(), GetShaderFileLowerCase()) != NULL;
}

A3DShader * CECSpecialShader::Load()const{
	A3DShader *result = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(GetShaderFileLowerCase());
	result->GetGeneralProps().dwMask |= SGMF_TTEMP;
	return result;
}

void CECSpecialShader::SetTexture(A3DShader *pShader, const char *szTexMap, const char *szTexMap2){
	pShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);	
	if (pShader->GetGeneralProps().pPShader){
		pShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)(szTexMap2 ? szTexMap2 : MakeTexMap2With(szTexMap)));
	}else{
		pShader->ShowStage(2, false);
	}	
	pShader->SetOnlyActiveStagesFlag(true);
	pShader->SetAlphaTextureFlag(IsAlphaTexture(szTexMap));
}

AString CECSpecialShader::GetFileTitleLowerCase(const char *szFilePath){
	char result[MAX_PATH] = {0};
	af_GetFileTitle(szFilePath, result, MAX_PATH);
	strlwr(result);
	return result;
}

bool CECSpecialShader::IsAlphaTexture(const char *szTexMap){
	bool result(false);
	
	typedef abase::hash_map<AString, bool>	IsAlphaTextureMap;
	static IsAlphaTextureMap s_isAlphaTextureMap;

	AString strTexMap = szTexMap;
	IsAlphaTextureMap::iterator it = s_isAlphaTextureMap.find(strTexMap);
	if (it != s_isAlphaTextureMap.end()){
		result = it->second;
	}else{
		if (af_IsFileExist(szTexMap)){
			A3DTexture *pTexture = new A3DTexture;
			if (pTexture->LoadFromFile(g_pGame->GetA3DDevice(), szTexMap, szTexMap)){
				result = pTexture->IsAlphaTexture();
			}
			A3DRELEASE(pTexture);
		}
		s_isAlphaTextureMap[strTexMap] = result;
	}
	return result;
}