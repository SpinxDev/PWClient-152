// File		: EC_ServerSimulate.h
// Creator	: Xu Wenbin
// Date		: 2013/12/6

#pragma once

#include "gnmarshal.h"
#include <AString.h>

//	模拟服务器、向客户端发送协议（用于测试）
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
