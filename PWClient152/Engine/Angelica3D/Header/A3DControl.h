/*
 * FILE: A3DControl.h
 *
 * DESCRIPTION: Base class which representing an abstract control
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCONTROL_H_
#define _A3DCONTROL_H_

#include "ABaseDef.h"

class A3DControl
{
private:
protected:
public:
	A3DControl();
	virtual ~A3DControl();
};

typedef A3DControl * PA3DControl;

#endif