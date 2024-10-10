/********************************************************************
	created:	2006/11/16
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#include "PfConstant.h"

namespace AutoMove
{

int PF2D_NeighborD[PF_NEIGHBOR_COUNT*2] =
{
		-1,  0,   //left
		+1,  0,   //right
		0,   +1,  //top
		0,   -1,  //bottom
		+1,  +1,  //topright
		+1,  -1,  //bottomright
		-1,  +1,  //topleft
		-1,  -1   //bottomleft
};

float PF2D_NeighborCost[PF_NEIGHBOR_COUNT] = 
{
	1.0f,
	1.0f,
	1.0f,
	1.0f,
	(float)PF_ROOT_TWO,
	(float)PF_ROOT_TWO,
	(float)PF_ROOT_TWO,
	(float)PF_ROOT_TWO
};

}