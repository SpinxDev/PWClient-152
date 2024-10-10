/*
 * FILE: ARandomGen.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ARANDOMGEN_H_
#define _ARANDOMGEN_H_

#include "ABaseDef.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ARandomGen
//	
///////////////////////////////////////////////////////////////////////////

class ARandomGen
{
public:		//	Types

public:		//	Constructor and Destructor

	ARandomGen();
	virtual ~ARandomGen() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize random number generator
	void Init(DWORD dwSeed) { RandomInitialize((int)(dwSeed % 31328), (int)(dwSeed % 30081)); }

	//	Generate a double random number
	double RandomUniform();
	//	Generate a double random number base on Gaussian formula
	double RandomGaussian(double mean, double stddev);
	//	Generate a integer random number
	int RandomInt(int lower, int upper);
	//	Generate a float random number
	float RandomFloat(float lower, float upper);

protected:	//	Attributes

	double	u[97], c, cd, cm;
	int		m_i97, m_j97;
	int		m_bTest;

protected:	//	Operations

	//	Initialize random number generator
	void RandomInitialize(int ij, int kl);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_ARANDOMGEN_H_
