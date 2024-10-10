/*
 * FILE: AMVideoEngine.h
 *
 * DESCRIPTION: The class standing for the video engine of Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMVIDEOENGINE_H_
#define _AMVIDEOENGINE_H_

#include "AMTypes.h"

typedef class AMEngine * PAMEngine;

class AMVideoEngine
{
private:
	PAMEngine			m_pAMEngine;

protected:

public:
	AMVideoEngine();
	~AMVideoEngine();

	bool Init(PAMEngine pAMEngine);
	bool Release();

	bool Tick();

	bool Reset();

	inline AMEngine * GetAMEngine() { return m_pAMEngine; }
};

typedef class AMVideoEngine * PAMVideoEngine;

#endif//_AMVIDEOENGINE_H_
