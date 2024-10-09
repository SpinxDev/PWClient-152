/*
 * FILE: ChangePill.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/7/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_ChangePill.h"
#include "AFileImage.h"

//----------------------------------------------------------
CECChangePill::CECChangePill()
{
	m_nCharacter = -1;
	m_nCharacter = -1;
}

//----------------------------------------------------------
CECChangePill::~CECChangePill()
{
}

//----------------------------------------------------------
bool CECChangePill::Load(const char* pszFileName)
{
	if(!pszFileName)
		return false;

	AFileImage filePill;
	if(!filePill.Open(pszFileName, AFILE_OPENEXIST|AFILE_BINARY|AFILE_TEMPMEMORY))
	{
		assert(0 && "CECChangePill::LoadChangePill, Failed to Open pill file!");
		a_LogOutput(1, "CECChangePill::LoadChangePill, Failed to Open pill file!");
		return false;
	}

	//清空原有变形丸数据
	ClearChangePillData();

	//载入变形丸数据
	int nCount;
	unsigned long uReadLength;
	
	filePill.Read(&m_nCharacter, sizeof(int), &uReadLength);
	filePill.Read(&m_nGender, sizeof(int), &uReadLength);
	filePill.Read(&nCount, sizeof(int), &uReadLength);

	if( nCount > 0)
	{
		ChgPillData_t *pChgPillData = (ChgPillData_t*)a_malloctemp(sizeof(ChgPillData_t)*nCount);
		filePill.Read(pChgPillData, sizeof(ChgPillData_t)* nCount, &uReadLength);

		for( int i = 0; i < nCount ; ++i)
		{
			m_vecChgPillData.push_back(pChgPillData[i]);
		}

		a_freetemp(pChgPillData);
	}
	

	filePill.Close();

	return true;
}


//-----------------------------------------------------------------------
//清空变形丸数据

void CECChangePill::ClearChangePillData(void)
{
	if(m_vecChgPillData.empty())
		return;
	m_nCharacter = -1;
	m_nGender = -1;
	m_vecChgPillData.clear();

}