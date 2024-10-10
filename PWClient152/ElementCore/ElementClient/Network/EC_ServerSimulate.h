// File		: EC_ServerSimulate.h
// Creator	: Xu Wenbin
// Date		: 2013/12/6

#pragma once

#include "gnmarshal.h"
#include <AString.h>

//	ģ�����������ͻ��˷���Э�飨���ڲ��ԣ�
class CECServerSimulate
{
private:
	CECServerSimulate(){}
	CECServerSimulate(const CECServerSimulate &);
	CECServerSimulate & operator == (const CECServerSimulate &);

public:
	static CECServerSimulate & Instance();

	bool Come(GNET::Marshal::OctetsStream &data);
	bool Come(const AString *pTokens, int nToken);
};
