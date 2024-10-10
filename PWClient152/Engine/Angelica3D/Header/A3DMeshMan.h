/*
 * FILE: A3DMeshMan.h
 *
 * DESCRIPTION: Managing Class for mesh objects
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMESHMAN_H_
#define _A3DMESHMAN_H_

#include "A3DTypes.h"

class A3DMeshMan
{
private:
protected:
public:
	A3DMeshMan();
	virtual ~A3DMeshMan();

	bool Init();
	bool Release();
	bool Reset();

	bool Render();
};


#endif