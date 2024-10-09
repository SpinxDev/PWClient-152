#ifndef _AI_CONDITION_PARAM_H_
#define _AI_CONDITION_PARAM_H_

#include "PolicyType.h"

class CConditionParam
{

public:
	CConditionParam(){}
	~CConditionParam(){}
	
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class CConditionParam_Time_Come :public CConditionParam
{

public:
	C_TIME_COME m_Data;
	
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

//class CConditionParam_Time_Point :public CConditionParam
//{

//public:
//	C_TIME_POINT m_Data;
	
//};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class CConditionParam_Be_Hurt : public CConditionParam
{

public:
	C_BE_HURT m_Data;

};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class CConditionParam_HP_Less :public CConditionParam
{

public:
	C_HP_LESS m_Data;
	
};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class CConditionParam_Random :public CConditionParam
{

public:
	C_RANDOM m_Data;

};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class CConditionParam_Start_Attack :public CConditionParam
{

public:
	//Not ready
	
};


#endif