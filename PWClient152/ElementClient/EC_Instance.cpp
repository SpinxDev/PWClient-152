/*
 * FILE: EC_Instance.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/8/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Instance.h"
#include "AWScriptFile.h"
#include <ALog.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECInstance
//	
///////////////////////////////////////////////////////////////////////////

CECInstance::CECInstance()
{
	m_id		= 0;
	m_iRowNum	= 0;
	m_iColNum	= 0;
	m_bLimitJump = true;
}

//	Load instance information from file
bool CECInstance::Load(AWScriptFile* psf)
{
	//	Read name
	if (!psf->GetNextToken(true))
		return false;

	m_strName = psf->m_szToken;

	if (!psf->MatchToken(_AL("{"), true))
		return false;

	//	Read ID
	m_id = psf->GetNextTokenAsInt(true);

	//	Read path
	if (!psf->GetNextToken(true))
		return false;

	m_strPath = WC2AS(psf->m_szToken);
	

	//	Read row and column number of map
	m_iRowNum = psf->GetNextTokenAsInt(true);
	m_iColNum = psf->GetNextTokenAsInt(false);

	m_bLimitJump = (psf->GetNextTokenAsInt(true) != 0);
	
	//	加载自动寻路文件名称
	if (!psf->GetNextToken(true))
		return false;
	ACString strTemp;
	do 
	{
		strTemp = psf->m_szToken;
		strTemp.TrimLeft();
		strTemp.TrimRight();
		strTemp.MakeLower();
		if (!strTemp.IsEmpty()){
			if (strTemp.Right(4) == _AL(".cfg")){
				m_routeFiles.push_back(WC2AS(strTemp));
			}else{
				a_LogOutput(1, "CECInstance::Load, Invalid file %s IGNORED!", AC2AS(psf->m_szToken));
			}
		}
	} while (psf->GetNextToken(false));

	//	位置相关的 M 图纹理配置
	m_positionRelatedTextures.clear();
	if (!psf->GetNextToken(true)){
		return false;
	}
	if (wcscmp(psf->m_szToken, _AL("{}")) &&
		wcscmp(psf->m_szToken, _AL("{"))){
		return false;
	}
	if (!wcscmp(psf->m_szToken, _AL("{"))){
		while (true){
			//	查看是否是M图纹理配置终止符'}'
			if (!psf->PeekNextToken(true)){
				return false;
			}
			if (!wcscmp(psf->m_szToken, _AL("}"))){
				//	是终止符，完成M图纹理配置，继续检查其它配置
				psf->GetNextToken(true);
				break;
			}
			//	不是终止符，读取一个M图纹理配置
			PositionRelatedTexture config;
			config.rect.left = psf->GetNextTokenAsFloat(true);	//	左下角位置 X
			config.rect.top = psf->GetNextTokenAsFloat(true);	//	左下角位置 Z
			float width = psf->GetNextTokenAsFloat(true);		//	宽度
			if (width < 10.0f){
				return false;
			}
			config.rect.right = config.rect.left + width;
			float height = psf->GetNextTokenAsFloat(true);		//	高度
			if (height < 10.0f){
				return false;
			}
			config.rect.bottom = config.rect.top + height;
			if (!psf->GetNextToken(true)){
				return false;
			}
			config.filePath = WC2AS(psf->m_szToken);
			m_positionRelatedTextures.push_back(config);
		}
	}

	//	配置结束
	if (!psf->MatchToken(_AL("}"), true)){
		return false;
	}

	return true;
}

//	根据游戏中位置 (x, z) 查找是否位置不同图也不同
bool CECInstance::GetPositionRelatedTexture(float x, float z, AString &filePath)const{
	bool bFound(false);
	for (PositionRelatedTextureArray::const_iterator cit = m_positionRelatedTextures.begin(); cit != m_positionRelatedTextures.end(); ++ cit){
		const PositionRelatedTexture &config = *cit;
		if (config.rect.PtInRect(x, z)){
			filePath = config.filePath;
			bFound = true;
			break;
		}
	}
	return bFound;
}