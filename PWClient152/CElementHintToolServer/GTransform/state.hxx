#ifndef __GNET_TESTCLIENT_STATE
#define __GNET_TESTCLIENT_STATE

#ifdef WIN32
#include "gnproto.h"
#else
#include "protocol.h"
#endif

namespace GNET
{

extern GNET::Protocol::Manager::Session::State state_GTransformServer;

};

#endif
