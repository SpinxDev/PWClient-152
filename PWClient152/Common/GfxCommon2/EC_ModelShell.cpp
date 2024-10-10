/*
* FILE: CECModelShell.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/01/06
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "EC_ModelShell.h"

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
//	Implement CECModelShell
//	
///////////////////////////////////////////////////////////////////////////

CECModelShell::CECModelShell(void)
: m_pECModelInst(NULL)
{
}

CECModelShell::CECModelShell(const char* szFile)
: m_pECModelInst(NULL)
{
	SwapECModel(szFile);
}

CECModelShell::~CECModelShell(void)
{
	delete m_pECModelInst;
	m_pECModelInst = NULL;
}

//	Swap the ECModel instance to another specified by the filename
bool CECModelShell::SwapECModel(const char* szFile)
{
	if (!szFile || !szFile[0])
		return false;

	CECModel* pECModel = new CECModel();
	if (!pECModel->Load(szFile))
	{
		a_LogOutput(1, "Error in CECModelShell::SwapECModel, Failed to load ecm file %s", szFile);
		return false;
	}

	if (m_pECModelInst)
	{
		pECModel->SetPos(m_pECModelInst->GetPos());
		pECModel->SetDirAndUp(m_pECModelInst->GetDir(), m_pECModelInst->GetUp());
		
		m_pECModelInst->Release();
		delete m_pECModelInst;
	}

	m_pECModelInst = pECModel;

	return true;
}