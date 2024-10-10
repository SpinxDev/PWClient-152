/*
 * FILE: AMVideoEngine.cpp
 *
 * DESCRIPTION: The class standing for the video engine of Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMEngine.h"
#include "AMVideoEngine.h"
#include "AMSoundEngine.h"

AMVideoEngine::AMVideoEngine()
{
	m_pAMEngine = NULL;
}

AMVideoEngine::~AMVideoEngine()
{
}

bool AMVideoEngine::Init(PAMEngine pAMEngine)
{
	m_pAMEngine = pAMEngine;
	
	// For direct show needs ActiveX control to be initialized
	CoInitialize(NULL);
	return true;
}

bool AMVideoEngine::Release()
{
	CoUninitialize();
	return true;
}

bool AMVideoEngine::Reset()
{
	CoUninitialize();
	CoInitialize(NULL);
	return true;
}

bool AMVideoEngine::Tick()
{
	return true;
}