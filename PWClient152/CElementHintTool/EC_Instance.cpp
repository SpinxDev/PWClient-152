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
	
	//	�����Զ�Ѱ·�ļ�����
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

	//	λ����ص� M ͼ��������
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
			//	�鿴�Ƿ���Mͼ����������ֹ��'}'
			if (!psf->PeekNextToken(true)){
				return false;
			}
			if (!wcscmp(psf->m_szToken, _AL("}"))){
				//	����ֹ�������Mͼ�������ã����������������
				psf->GetNextToken(true);
				break;
			}
			//	������ֹ������ȡһ��Mͼ��������
			PositionRelatedTexture config;
			config.rect.left = psf->GetNextTokenAsFloat(true);	//	���½�λ�� X
			config.rect.top = psf->GetNextTokenAsFloat(true);	//	���½�λ�� Z
			float width = psf->GetNextTokenAsFloat(true);		//	���
			if (width < 10.0f){
				return false;
			}
			config.rect.right = config.rect.left + width;
			float height = psf->GetNextTokenAsFloat(true);		//	�߶�
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

	//	���ý���
	if (!psf->MatchToken(_AL("}"), true)){
		return false;
	}

	return true;
}

//	������Ϸ��λ�� (x, z) �����Ƿ�λ�ò�ͬͼҲ��ͬ
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