#ifndef _AI_TARGET_PARAM_H_
#define _AI_TARGET_PARAM_H_

#include "PolicyType.h"

class CTargetParam
{

public:

	CTargetParam(){}
	~CTargetParam(){}

};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class CTargetParam_Occupation_List : public CTargetParam
{

public:
	T_OCCUPATION m_Data;//ְҵ�����
};

#endif